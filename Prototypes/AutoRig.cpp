#include "AutoRig.hpp"
#include <limits>
#include <algorithm>

#include <iostream>
#include <thread>

namespace CForge {
	AutoRigger::AutoRigger() : CForgeObject("AutoRigger") {
		m_targetMesh = nullptr;
		m_targetSkeleton = nullptr;
	}
	AutoRigger::~AutoRigger() {
		clear();
	}
	void AutoRigger::init(T3DMesh<float>* targetMesh, T3DMesh<float>::Bone* targetSkeleton) {
		clear();
		
		AR_CORES = std::thread::hardware_concurrency()-1;
		
		m_targetMesh = targetMesh;
		m_targetSkeleton = targetSkeleton;
		
		T3DMesh<float> cubeMesh;
		AssetIO::load("Assets/cube.obj", &cubeMesh);
		SceneUtilities::setMeshShader(&cubeMesh, 0.8f, 0.04f);
		cubeMesh.computePerVertexNormals();
		cube.init(&cubeMesh);
		T3DMesh<float> sphereMesh;
		AssetIO::load("Assets/sphere.obj", &sphereMesh);
		SceneUtilities::setMeshShader(&sphereMesh, 0.8f, 0.04f);
		sphereMesh.computePerVertexNormals();
		sphere.init(&sphereMesh);
	}
	void AutoRigger::clear() {
		m_targetMesh = nullptr;
		m_targetSkeleton = nullptr;
	}
	void AutoRigger::process() {
		if (m_targetMesh == nullptr) throw CForgeExcept("auto rigger: no target mesh set");
		if (m_targetSkeleton == nullptr) throw CForgeExcept("auto rigger: no skeleton mesh set");
		buildGrid();
		
		std::thread* threads = new std::thread[AR_CORES];
		for (uint32_t i = 0; i < AR_CORES; i++) {
			uint32_t begin = (i*m_grid.size()/AR_CORES);
			uint32_t end =   ((i+1)*m_grid.size()/AR_CORES);
			if (i==AR_CORES-1)
				end = m_grid.size();
			threads[i] = std::thread([=] {this->computeSDFGrid(begin,end);});
		}
		for (uint32_t i = 0; i < AR_CORES; i++) {
			threads[i].join();
		}
		delete[] threads;
		threads = nullptr;
		
		// sort nodes so we begin in deepest part of the mesh
		std::sort(m_grid.begin(),m_grid.end(), cmpSDFGridCell);
		

		SphereNode root = {m_grid[0].pos, m_grid[0].closestTriDist*sphereScaleWhileMarking};
		m_nodes.push_back(root);
		
		// generate spheres
		for (uint32_t i = 1; i < m_grid.size(); i++) {
			GridCell* pCGC = &m_grid[i];
			
			bool insideNode = false;
			for (uint32_t j = 0; j < m_nodes.size(); j++) {
				if (insideSphere(m_nodes[j].pos,m_nodes[j].radius,pCGC->pos)) {
					insideNode = true;
					break;
				}
			}
			
			if (!insideNode && pCGC->sdf < 0.0f) {
				SphereNode node = {pCGC->pos,pCGC->closestTriDist*sphereScaleWhileMarking};
				m_nodes.push_back(node);
			}
		}
		
		// construct graph
		// find sphere neighbours
		for (uint32_t i = 0; i < m_nodes.size(); i++) {
			for (uint32_t j = 0; j < m_nodes.size(); j++) {
				if (i == j) continue;
				// check if spheres intersect
				if ((m_nodes[i].pos - m_nodes[j].pos).norm() < m_nodes[i].radius + m_nodes[j].radius) {
					m_nodes[i].neighbours.push_back(&m_nodes[j]);
					m_nodes[j].neighbours.push_back(&m_nodes[i]);
				}
			}
		}
		
		// connect graph
		MarkDFS(&m_nodes[0]);
		bool allReachable = false;
		SphereNode* pNCSN = closestNonVisitedNode();
		while (pNCSN) {
			// find closest node which is visited
			SphereNode* cSN = nullptr;
			float cD = std::numeric_limits<float>::max();
			// find next visited node
			for (uint32_t j = 0; j < m_nodes.size(); j++) {
				if (m_nodes[j].visited == 1) {
					float dist = (pNCSN->pos - m_nodes[j].pos).norm() - pNCSN->radius - m_nodes[j].radius;
					if (dist < cD) {
						cSN = &m_nodes[j];
						cD = dist;
					} // if distance smaller
				} // if node visited
			} // for nodes
			if (cSN) {
				pNCSN->neighbours.push_back(cSN);
				cSN->neighbours.push_back(pNCSN);
			}
			else {
				throw CForgeExcept("AutoRig: Graph connection failed, no available nodes");
			}
			// update colors
			MarkReset();
			MarkDFS(&m_nodes[0]);
			pNCSN = closestNonVisitedNode();
		}
		// reset marking sphere scaling
		for (uint32_t i = 0; i < m_nodes.size(); i++) {
			m_nodes[i].radius /= sphereScaleWhileMarking;
		}
		MarkReset();
		
		// assign corresponding endeffectors 
		// determine endeffectors of target skeleton
		std::vector<T3DMesh<float>::Bone*> targetSkeletonEndeffectors;
		targetSkeletonEndeffectors = getTSKEF(m_targetSkeleton);

		std::vector<SphereNode*> graphEF;
		// determine endeffectors
		for (uint32_t j = 0; j < targetSkeletonEndeffectors.size(); j++) {
			// find sphere furthest away
			SphereNode* cSN = nullptr;
			float cD = 0.0f;
			for (uint32_t i = 1; i < m_nodes.size(); i++) {
				float dist = (m_nodes[0].pos-m_nodes[i].pos).norm();
				if (dist > cD && m_nodes[i].visited == 0) {
					cSN = &m_nodes[i];
					cD = dist;
				}
			}
			if (cSN) {
				cSN->endeffector = true;
				graphEF.push_back(cSN);
			}
			// remove nodes from endeffector to root
			MarkBFS(cSN,j+1);
		}
		MarkReset();
		
		// match endeffectors using closest direction to center
		for (uint32_t i = 0; i < targetSkeletonEndeffectors.size(); i++) {
			auto o = targetSkeletonEndeffectors[i]->OffsetMatrix;
			Eigen::Vector3f tv = Eigen::Vector3f(o.data()[12],o.data()[13],o.data()[14]);
			tv.normalize();
			SphereNode* pCSN = nullptr;
			float cDir = -1.0f;
			
			for (uint32_t j = 0; j < graphEF.size(); j++) {
				Eigen::Vector3f ev = graphEF[j]->pos-m_nodes[0].pos;
				ev.normalize();
				float dot = ev.dot(tv);
				if (dot > cDir) {
					cDir = dot;
					pCSN = graphEF[j];
				}
			}
			if (!pCSN)
				throw CForgeExcept("AutoRig: no matching endeffector found");
			pCSN->cBone = targetSkeletonEndeffectors[i];
		}
		
		// TODO embed skeleton into spheres
		// start from center use BFS to determine shortest path to EF
		BFSPath(&m_nodes[0]);
		MarkReset();
		// TODO remove
		// visualize
		for (uint32_t i = 0; i < graphEF.size(); i++) {
			SphereNode* curr = graphEF[i];
			while (curr->pParent != &m_nodes[0]) {
				curr->visited = i+1;
				curr = curr->pParent;
				if (curr == nullptr) {
					std::cout << "broken at: " << i << "\n";
					break;
				}
			}
		}
		
		// TODO fit skeletton
		for (uint32_t i = 0; i < targetSkeletonEndeffectors.size(); i++) {
			
		}
		
		// TODO make embedding symmetric
		// symmetric angles, legs and arms need a cos angle of 1
		
		
		// TODO assign triangle weights to spheres
		
		// TODO collapse spheres to edges in graph
		
		// TODO set skeletton
		
		return;
	}
	
	void AutoRigger::BFSPath(SphereNode* root) {
		// make sure to first push back nodes with highest depth to stay most likely at the median line
		std::vector<SphereNode*> MarkBFSNext;
		MarkBFSNext.push_back(root);
		root->visited = 1;
		while (MarkBFSNext.size() > 0) {
			SphereNode* curr = MarkBFSNext[0];
			MarkBFSNext.erase(MarkBFSNext.begin());
			std::vector<SphereNode*> nB(curr->neighbours);
			while (!nB.empty()) {
				// get neighbour with most depth
				SphereNode* dSN = nullptr;
				float depth = 0.0f;
				uint32_t iR = 0;
				for (uint32_t i = 0; i < nB.size(); i++) {
					if (nB[i]->radius > depth) {
						depth = nB[i]->radius;
						dSN = nB[i];
						iR = i;
					} // if deeper
				} // for neighbours
				nB.erase(nB.begin()+iR);
				
				assert(dSN != nullptr);
				if (dSN->visited != 1) {
					if (std::find(MarkBFSNext.begin(),MarkBFSNext.end(),dSN) == MarkBFSNext.end()) {
						dSN->visited = 1;
						assert(dSN->pParent==nullptr);
						if (dSN->pParent == nullptr)
							dSN->pParent = curr;
						MarkBFSNext.push_back(dSN);
					} // if node is not in list
				} // for not visited neighbours
			} // for all neighbours
		} // while BFS list contains nodes
	}
	
	float AutoRigger::Angle2Bones(Eigen::Vector3f a,Eigen::Vector3f b,Eigen::Vector3f c) {
		Eigen::Vector3f ab = (b-a).normalized();
		Eigen::Vector3f bc = (c-b).normalized();
		return ab.dot(bc);
	}
	
	std::vector<T3DMesh<float>::Bone*> AutoRigger::getTSKEF(T3DMesh<float>::Bone* bone) {
		std::vector<T3DMesh<float>::Bone*> ret;
		if (bone->Children.size() > 0) {
			for (uint32_t i = 0; i < bone->Children.size(); i++) {
				std::vector<T3DMesh<float>::Bone*> v;
				T3DMesh<float>::Bone* c = bone->Children.at(i);
				v = getTSKEF(c);
				ret.insert(ret.end(), v.begin(), v.end());
			}
		}
		else {
			ret.push_back(bone); // push back parent of end
		}
		return ret;
	}
	
	
	AutoRigger::SphereNode* AutoRigger::closestNonVisitedNode() {
		SphereNode* cSN = nullptr;
		float cD = std::numeric_limits<float>::max();
		for (uint32_t i = 0; i < m_nodes.size(); i++) {
			if (m_nodes[i].visited == 0) {
				for (uint32_t j = 0; j < m_nodes.size(); j++) {
					if (m_nodes[j].visited == 1) {
						float dist = (m_nodes[i].pos - m_nodes[j].pos).norm() - m_nodes[i].radius - m_nodes[j].radius;
						if (dist < cD) {
							cSN = &m_nodes[i];
							cD = dist;
						} // if distance smaller
					} // if node is visited
				} // for nodes
			} // if node is not visited
		} // for all nodes
		return cSN;
	}
	
	void AutoRigger::MarkBFS(SphereNode* root, uint32_t color) {
		if (!root) throw CForgeExcept("AutoRigger: BFS root was nullptr");
		std::vector<SphereNode*> MarkBFSNext;
		MarkBFSNext.push_back(root);
		root->visited = color;
		while (MarkBFSNext.size() > 0) {
			SphereNode* curr = MarkBFSNext[0];
			if (curr == &m_nodes[0]) {
				MarkBFSNext.clear();
				return;
			}
			MarkBFSNext.erase(MarkBFSNext.begin());
			for (uint32_t i = 0; i < curr->neighbours.size(); i++) {
				if (curr->neighbours[i]->visited != color) {
					if (std::find(MarkBFSNext.begin(),MarkBFSNext.end(),curr->neighbours[i]) == MarkBFSNext.end()) {
						curr->neighbours[i]->visited = color;
						MarkBFSNext.push_back(curr->neighbours[i]);
					} // if node is not in list
				} // for not visited neighbours
			} // for all neighbours
		} // while BFS list contains nodes
	}
	
	void AutoRigger::MarkDFS(SphereNode* root) {
		root->visited = 1;
		for (uint32_t i = 0; i < root->neighbours.size(); i++) {
			if (root->neighbours[i]->visited == 0)
				MarkDFS(root->neighbours[i]);
		}
	}
	
	void AutoRigger::MarkReset() {
		for (uint32_t i = 0; i < m_nodes.size(); i++) {
			m_nodes[i].visited = 0;
		}
	}
	
	bool AutoRigger::cmpSDFGridCell(const GridCell& a, const GridCell& b) {
		return a.sdf < b.sdf;
	}
	
	void AutoRigger::computeSDFGrid(uint32_t start, uint32_t end) {
		std::vector<float> PRV_sdf;
		for (uint32_t c = 0; c < sdfChecks; c++) {
			PRV_sdf.push_back(0.0);
		}
		
		for (uint32_t k = start; k < end; k++) {
			GridCell* pCGC = &m_grid[k];
			
			for (uint32_t i = 0; i < m_targetMesh->submeshCount(); i++) {
				T3DMesh<float>::Submesh* p_subM = m_targetMesh->getSubmesh(i);
				for (uint32_t j = 0; j < p_subM->Faces.size(); j++) {
					T3DMesh<float>::Face face = p_subM->Faces[j];
					Face f = {m_targetMesh->vertex(face.Vertices[0]),
					          m_targetMesh->vertex(face.Vertices[1]),
					          m_targetMesh->vertex(face.Vertices[2])};
					float dist = distTriApprox(f,pCGC->pos);
					
					// assumtion that the closest triangle gives us the correct sdf
					if (dist < pCGC->closestTriDist) { // only check triangles closer
						// update SDF
						pCGC->closestTriDist = dist;
						float sdf = sdfTriApprox(f,pCGC->pos);
						for (uint32_t c = sdfChecks-1; c > 0; c--) {
							PRV_sdf[c] = PRV_sdf[c-1];
						}
						PRV_sdf[0] = sdf;
						pCGC->sdf = sdf;
					}
				} // for faces
				if (start == 0) // pring progress
					std::cout << (float) k/end << "\n";
			} // for submeshes
			for (uint32_t i = 0; i < sdfChecks; i++) {
				if (PRV_sdf[i] > 0.0) {
					pCGC->sdf = PRV_sdf[i];
				}
			}
		} // for grid
	}
	
	void AutoRigger::buildGrid() {
		m_grid.resize(m_gridDim[0]*m_gridDim[1]*m_gridDim[2]);
		m_targetMesh->computeAxisAlignedBoundingBox();
		//Eigen::Vector3f aabbE = m_targetMesh->aabb().Max-m_targetMesh->aabb().Min;
		m_cell_dimension = m_targetMesh->aabb().Max-m_targetMesh->aabb().Min;
		m_cell_dimension = m_cell_dimension.cwiseQuotient(m_gridDim.cast<float>());
		
		for (uint32_t x = 0; x < m_gridDim[0]; x++) {
			for (uint32_t y = 0; y < m_gridDim[1]; y++) {
				for (uint32_t z = 0; z < m_gridDim[2]; z++) {
					uint32_t index = x+y*m_gridDim[0] + z*m_gridDim[0]*m_gridDim[1];
					m_grid[index].pos = m_targetMesh->aabb().Min + m_cell_dimension.cwiseProduct(Eigen::Vector3f(x,y,z)) + 0.5*m_cell_dimension;
					m_grid[index].closestTriDist = std::numeric_limits<float>::max();
					m_grid[index].closestTri = -1;
				}
			}
		}
		return;
	}

	inline float AutoRigger::sdfTriApprox(Face face, Eigen::Vector3f point) {
		Eigen::Vector3f n = (face.vert[1]-face.vert[0]).cross(face.vert[2]-face.vert[0]);
		Eigen::Vector3f c = (face.vert[0]+face.vert[1]+face.vert[2])/3.0f;
		float d = n.dot(point-c);
		float dist = (c-point).norm();
		return dist*(d/std::abs(d));
	}
	
	inline float AutoRigger::distTriApprox(Face face, Eigen::Vector3f point) {
		Eigen::Vector3f c = (face.vert[0]+face.vert[1]+face.vert[2])/3.0f;
		return (c-point).norm();
	}
	
	inline bool AutoRigger::insideSphere(Eigen::Vector3f c, float r, Eigen::Vector3f p) {
		return (p-c).norm() < r;
	}
	
	void AutoRigger::renderGrid(RenderDevice* pRDev, Eigen::Matrix4f transf, Eigen::Matrix4f scale) {
		counter += 0.1;
		counter = std::fmod(counter, (float) m_nodes.size());
		counter = (float) m_nodes.size();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(1);
		for (uint32_t i = 0; i < m_grid.size(); i++) {
			if (m_grid[i].sdf < 0.0f) {
				Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(m_grid[i].pos);
				Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(m_cell_dimension);
				pRDev->modelUBO()->modelMatrix(transf*scale*cubeTransform*cubeScale);
				//cube.render(pRDev);
			}
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//std::cout << (uint32_t) counter << "\n";
		for (uint32_t i = 0; i < (uint32_t) counter; i++) {
			Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(m_nodes[i].pos);
			float r = m_nodes[i].radius;
			Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
			pRDev->modelUBO()->modelMatrix(transf*scale*cubeTransform*cubeScale);
			switch (m_nodes[i].visited) {
				case 0:
					glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
					break;
				case 1:
					glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_TRUE);
					break;
				case 2:
					glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
					break;
				case 3:
					glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
					break;
				case 4:
					glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
					break;
				case 5:
					glColorMask(GL_TRUE,GL_FALSE,GL_TRUE,GL_TRUE);
					break;
				default:
					break;
			}
			//if (m_nodes[i].endeffector || i==0)
			//	glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
			sphere.render(pRDev);
			//if (m_nodes[i].endeffector || i==0)
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		}
	}
}