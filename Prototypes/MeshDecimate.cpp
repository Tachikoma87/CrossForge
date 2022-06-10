#include "MeshDecimate.h"
#include "../CForge/AssetIO/SAssetIO.h"
#include <chrono>

using namespace Eigen;

namespace CForge {
	
	// TODO replace std::vector<std::vector<>> with std::vector<std::vector<>*>
	// TODO inMesh zu const& machen // cleanup split funcs
	void MeshDecimator::decimateMesh(const CForge::T3DMesh<float>* inMesh, CForge::T3DMesh<float>* outMesh, float amount)
	{
		auto start = std::chrono::high_resolution_clock::now();
		
		uint32_t faceCount = 0;
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			faceCount += inMesh->getSubmesh(i)->Faces.size();
		}
		
		Eigen::MatrixXd DV(inMesh->vertexCount(), 3);
		Eigen::MatrixXi DF(faceCount, 3);
		Eigen::VectorXi DuF; // used faces
		Eigen::VectorXi DuV; // used vertices
		
		for (uint32_t i = 0; i < inMesh->vertexCount(); i++) {
			DV.row(i) = inMesh->vertex(i).cast<double>();
		}
		
		std::vector<std::vector<uint32_t>> vertUsedInTri;
		vertUsedInTri.resize(DV.rows());
		
		std::vector<std::vector<uint32_t>> submeshTriangles; // contains Triangle IDs
		// add all faces from submeshes
		uint32_t startIndex = 0;
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			const CForge::T3DMesh<float>::Submesh* submesh = inMesh->getSubmesh(i);
			std::vector<uint32_t> triIdx;
			
			auto faces = submesh->Faces;
			
			for (uint32_t j = 0; j < faces.size(); j++) {
				triIdx.push_back(startIndex + j);
				Eigen::Vector3i subMVert(faces[j].Vertices[0], faces[j].Vertices[1], faces[j].Vertices[2]);
				for (uint8_t k = 0; k < 3; k++) {
					vertUsedInTri[faces[j].Vertices[k]].push_back(startIndex + j);
				}
				DF.row(startIndex + j) = subMVert;
			} // for every face of a submesh
			
			startIndex += submesh->Faces.size();
			submeshTriangles.push_back(triIdx);
		} // for all submeshes

		//std::cout << "DV\n" << DV << "\n";
		//std::cout << "DF\n" << DF << "\n";
		
		// merge points together
		std::vector<Eigen::Vector3d> DVnoMulVec;
		for (uint32_t i = 0; i < DV.rows(); i++) {
			
			Eigen::Vector3d vert = DV.row(i);
			int32_t idx = DVnoMulVec.size();
			auto itr = std::find(DVnoMulVec.begin(), DVnoMulVec.end(), vert);
			if (itr == DVnoMulVec.end()) { // vertex not contained, create new index
				DVnoMulVec.push_back(vert);
				idx = DVnoMulVec.size() - 1;
			} else
				idx = itr - DVnoMulVec.begin();
			
			// reindex face
			for (uint32_t triIdx : vertUsedInTri[i]) {
				for (uint32_t j = 0; j < 3; j++) {
					if (DF.row(triIdx)[j] == i)
						DF.row(triIdx)[j] = idx;
				}
			}
		} // for vertices
		
		Eigen::MatrixXd DVnoMul(DVnoMulVec.size(),3);
		for (uint32_t i = 0; i < DVnoMulVec.size(); i++) {
			DVnoMul.row(i) = DVnoMulVec[i];
		}
		
		//std::cout << "DVnoMul\n" << DVnoMul << "\n";
		//std::cout << "DV\n" << DV << "\n";
		//std::cout << "DF\n" << DF << "\n";
		//bool iglDecRes = igl::decimate(DVnoMul, DF, amount*DF.rows(), DVnoMul, DF, DuF, DuV);
		bool iglDecRes = decimateOctree(DVnoMul, DF, &DuF, &DuV, amount*DF.rows());
		if (!iglDecRes) {
			std::cout << "an error occured while decimating!\n";
			//std::exit(-1);
		}
		std::cout << "AFTERWARDS:\n";
		//std::cout << "DVnoMul\n" << DVnoMul << "\n";
		//std::cout << "DF\n" << DF << "\n";
		//std::cout << "DuF\n" << DuF << "\n";
		//std::cout << "DuV\n" << DuV << "\n";
				
		//std::cout << "startIndexes:\n";
		//for (uint32_t i : startIndexes)
		//	std::cout << i << ", ";
		//std::cout << "\nDstartIndexes:\n";
		//for (uint32_t i : DstartIndexes)
		//	std::cout << i << ", ";
		//std::cout << "\n";
		
		outMesh->clear();
		std::vector<Eigen::Vector3f> newVerts, newNormals, newTangents, newUVs, newColors;
		std::vector<uint32_t> UV;
		
		// copy all materials
		for (uint32_t i = 0; i < inMesh->materialCount(); i++) {
			T3DMesh<float>::Material* pM = new T3DMesh<float>::Material();
			pM->init(inMesh->getMaterial(i));
			outMesh->addMaterial(pM, false);
			outMesh->getMaterial(i)->Metallic = inMesh->getMaterial(i)->Metallic;
			outMesh->getMaterial(i)->Roughness = inMesh->getMaterial(i)->Roughness;
		}
		
		uint32_t oldStartSize = 0;
		uint32_t newStartSize = 0;
		// reassemble submeshes
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh();
			const T3DMesh<float>::Submesh* pOldSubmesh = inMesh->getSubmesh(i);
			
			std::vector<uint32_t> faces; // list of triangle IDs of Decimated Mesh corresponding to submesh
			uint32_t newTriAmount = 0;
			// only add used faces
			for (uint32_t j = 0; j < submeshTriangles[i].size(); j++) {
				uint32_t triID = submeshTriangles[i][j];
				for (uint32_t k = 0; k < DuF.size(); k++) {
					if (DuF[k] == triID) {
						faces.push_back(triID);
						newTriAmount++;
						break;
					}
				} // for used tris
			} // for old submesh tris
			
			std::vector<std::vector<Eigen::Vector4i>> newUVWs; //TODO rename newUVs conflict
			
			// add materials and set indices of submesh tris
			for (uint32_t j = 0; j < faces.size(); j++) {
				T3DMesh<float>::Face Face;
				const T3DMesh<float>::Face* pOldSMFace = &(pOldSubmesh->Faces[faces[j] - oldStartSize]);
				
				Face.Material = pOldSMFace->Material;
				for (uint32_t k = 0; k < 3; k++) { //TODO fourth entry not used?
					int32_t vertID = DF.row(newStartSize + j)[k];
					newVerts.push_back(DVnoMul.row(vertID).cast<float>());
					uint32_t newSize = newVerts.size()-1;
					Face.Vertices[k] = newSize;
					
					//newNormals.push_back(Eigen::Vector3f::Zero());
					newNormals.push_back(inMesh->normal(pOldSMFace->Normals[k]));
					Face.Normals[k] = newSize;
					
					if (inMesh->textureCoordinatesCount() > 0) {
						newUVs.push_back(inMesh->textureCoordinate(pOldSMFace->UVWs[k]));
						Face.UVWs[k] = newSize;
					}
					if (inMesh->colorCount() > 0) {
						newColors.push_back(inMesh->color(pOldSMFace->Colors[k]));
						Face.Colors[k] = newSize;
					}
					if (inMesh->tangentCount() > 0) {
						newTangents.push_back(inMesh->tangent(pOldSMFace->Tangents[k]));
						Face.Tangents[k] = newSize;
					}
				}//for[face indices]
				
				pSubmesh->Faces.push_back(Face);
			}//for[number of faces]
			outMesh->addSubmesh(pSubmesh, false);
			
			oldStartSize += inMesh->getSubmesh(i)->Faces.size();
			newStartSize += newTriAmount;
		} // for all submeshes

		outMesh->vertices(&newVerts);
		outMesh->normals(&newNormals);
		outMesh->tangents(&newTangents);
		outMesh->textureCoordinates(&newUVs);
		outMesh->colors(&newColors);
		
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() -start).count();
		std::cout << "Decimation Finished, time took: " << double(microseconds)*0.001 << "ms \n";
	} // decimateMesh func

	bool MeshDecimator::insideAABB(T3DMesh<float>::AABB BoundingBox, Eigen::Vector3f Vertex) {
		for (int i = 0; i < 3; i++) {
			if (Vertex[i] > BoundingBox.Max[i] || Vertex[i] < BoundingBox.Min[i])
				return false;
		}
		return true;
	}//insideAABB

	void MeshDecimator::releaseOctree(octreeNode* root) {
		for (uint8_t i = 0; i < 8; i++) {
			if (root->childs[i])
				releaseOctree(root->childs[i]);
		}
		delete root;
	}

	void MeshDecimator::createOctree(octreeNode* pNode, Eigen::MatrixXd* DV, std::vector<std::vector<octreeNode*>>* depthNodes) {
		// Proposed steps for creating the octree.
		// Step 1: Create all 8 AABBs of the respective octants
		Vector3f Maxs[8];
		Vector3f Mins[8];

		Vector3f Diag = pNode->BoundingBox.diagonal();
		Vector3f Min = pNode->BoundingBox.Min;
		Vector3f Max = pNode->BoundingBox.Max;
		Vector3f Center = Min + Diag / 2;

		Mins[0] = Min;
		Maxs[0] = Center;
		Mins[1] = Vector3f(Center.x(), Min.y(), Min.z()); // front bottom right
		Maxs[1] = Vector3f(Max.x(), Center.y(), Center.z());
		Mins[2] = Vector3f(Min.x(), Center.y(), Min.z()); // top left
		Maxs[2] = Vector3f(Center.x(), Max.y(), Center.z());
		Mins[3] = Vector3f(Center.x(), Center.y(), Min.z()); // top right
		Maxs[3] = Vector3f(Max.x(), Max.y(), Center.z());

		Mins[4] = Vector3f(Min.x(), Min.y(), Center.z()); // back bottom left
		Maxs[4] = Vector3f(Center.x(), Center.y(), Max.z());
		Mins[5] = Vector3f(Center.x(), Min.y(), Center.z()); // bottom right
		Maxs[5] = Vector3f(Max.x(), Center.y(), Max.z());
		Mins[6] = Vector3f(Min.x(), Center.y(), Center.z()); // top left
		Maxs[6] = Vector3f(Center.x(), Max.y(), Max.z());
		Mins[7] = Center;
		Maxs[7] = Max;

		// Step 2: create and initialize the 8 child nodes
		for (uint8_t i = 0; i < 8; ++i) {
			T3DMesh<float>::AABB boundingBox;
			boundingBox.Max = Maxs[i];
			boundingBox.Min = Mins[i];

			pNode->childs[i] = new octreeNode();
			pNode->childs[i]->BoundingBox = boundingBox;
			pNode->childs[i]->parent = pNode;
			pNode->childs[i]->depth = pNode->depth + 1;
		}//for[each octant]

		// Step 3: Iterate other all the node's vertex IDs and sort them into the child nodes
		// Do not forget, that we have the insideAABB utility method
		for (auto id : pNode->VertexIDs) {
			// sort vertices into child nodes

			for (int i = 0; i < 8; i++) {
				if (insideAABB(pNode->childs[i]->BoundingBox, DV->row(id).cast<float>()))
					pNode->childs[i]->VertexIDs.push_back(id);
			}
		}//for[all vertexes]

		for (uint8_t i = 0; i < 8; ++i) {
			if (pNode->childs[i]->VertexIDs.size() != 0) {
				while (depthNodes->size() < pNode->childs[i]->depth)
					depthNodes->push_back(std::vector<octreeNode*>()); // TODO this does not look safe, pls change
				depthNodes->at(pNode->childs[i]->depth - 1).push_back(pNode->childs[i]);
			}
		}//for[octants]

		// Step4: Recursion
		for (uint8_t i = 0; i < 8; ++i) {
			// kill empty nodes
			if (pNode->childs[i]->VertexIDs.size() == 0) {
				delete pNode->childs[i];
				pNode->childs[i] = nullptr;
			}
			// call method again, if conditions for further subdivision are met
			else if (pNode->depth < m_MaxOctreeDepth && pNode->VertexIDs.size() > m_MaxLeafVertexCount) {
				//std::cout << "adding Node with depth " << pNode->Depth << " and size " << pNode->VertexIDs.size() << std::endl;
				createOctree(pNode->childs[i], DV, depthNodes);
			}
		}//for[octants]
	}


	bool MeshDecimator::decimateOctree(Eigen::MatrixXd& DV, Eigen::MatrixXi& DF, Eigen::VectorXi* DuF, Eigen::VectorXi* DuV, uint32_t faceAmount) {
		if (DV.size() == 0 || DF.size() == 0)
			return false;

		uint32_t removeFaceCount = 0;
		std::vector<uint32_t> facesToRemove;
		std::vector<uint32_t> pointsToRemove;
		std::vector<std::vector<octreeNode*>> depthNodes;

		// construct octree
		octreeNode* Root = new octreeNode();
		Root->depth = 0;
		//set BB to first val or else we might have a prob
		for (uint8_t j = 0; j < 3; j++) {
			Root->BoundingBox.Min[j] = DV.row(0)[j];
			Root->BoundingBox.Max[j] = DV.row(0)[j];
		}
		for (uint32_t i = 0; i < DV.rows(); i++) {
			for (uint8_t j = 0; j < 3; j++) {
				float span = DV.row(i)[j];
				Root->BoundingBox.Min[j] = std::min(Root->BoundingBox.Min[j], span);
				Root->BoundingBox.Max[j] = std::max(Root->BoundingBox.Max[j], span);
			}
			Root->VertexIDs.push_back(i);
		}
		createOctree(Root, &DV, &depthNodes);

		// decimate, find targets
		while (DF.rows() - removeFaceCount > faceAmount) {
			// get last largest depth node
			std::vector<octreeNode*>* largestDepth = &(depthNodes.back());
			octreeNode* parent = largestDepth->at(0)->parent;
			if (!parent)
				break;
			//largestDepth->pop_back();

			// points which get merged into one
			std::vector<uint32_t> targetPoints;

			// join all child vertices into parent as one vertex
			for (uint32_t i = 0; i < 8; i++) {
				octreeNode* child = parent->childs[i];
				if (child) {
					for (uint32_t vertID : child->VertexIDs) {
						targetPoints.push_back(vertID);
					}
					// remove child pointer from depth list
					largestDepth->erase(std::remove(largestDepth->begin(), largestDepth->end(), child), largestDepth->end());

					delete child;
					parent->childs[i] = nullptr;
				}
			}

			pointsToRemove.insert(std::end(pointsToRemove), std::next(std::begin(targetPoints)), std::end(targetPoints));
			
			std::vector<uint32_t> addedFaces = joinPoints(&DV, &DF, targetPoints);
			facesToRemove.insert(std::end(facesToRemove), std::begin(addedFaces), std::end(addedFaces));
			removeFaceCount = facesToRemove.size();

			// push back new node
			parent->VertexIDs.push_back(targetPoints[0]);
			
			// reduce depth if no nodes are in it anymore
			if (depthNodes.back().empty())
				depthNodes.pop_back();
		}

		std::vector<uint32_t> DuFVec;
		// remove faces from DF
		for (uint32_t i = 0; i < DF.rows(); i++) {
			if (std::find(facesToRemove.begin(), facesToRemove.end(), i) == facesToRemove.end())
				DuFVec.push_back(i);
		}
		Eigen::MatrixXi newDF = Eigen::MatrixXi(DuFVec.size(), 3);
		*DuF = Eigen::VectorXi(DuFVec.size());
		for (uint32_t i = 0; i < DuFVec.size(); i++) {
			//*DuF << DuFVec[i];
			(*DuF)[i] = DuFVec[i];
			newDF.row(i) = DF.row(DuFVec[i]);
		}
		//delete DF;
		DF = newDF;

		// TODO redo same with points // TODO check if necessary
		// free octree
		for (uint32_t i = 0; i < depthNodes.size(); i++) {
			for (uint32_t j = 0; j < depthNodes[i].size(); j++) {
				delete depthNodes[i][j];
			}
		}
		delete Root;
		//releaseOctree(Root);
		return true;
	}

	// merges points and returns vector of faces to remove
	std::vector<uint32_t> MeshDecimator::joinPoints(Eigen::MatrixXd* DV, Eigen::MatrixXi* DF, const std::vector<uint32_t>& targets) {

		std::vector<uint32_t> removedFaces;
		// calculate new Point position
		Eigen::Vector3f newPoint = Eigen::Vector3f::Zero();
		for (uint32_t i = 0; i < targets.size(); i++) {
			newPoint += DV->row(targets[i]).cast<float>();
		}
		newPoint /= targets.size();

		// set position of all points to new Point
		DV->row(targets[0]) = newPoint.cast<double>();
		//for (uint32_t i = 0; i < targets.size(); i++) {
		//	DV->row(targets[i]) = newPoint.cast<double>();
		//}

		// TODO make search faster by using bidirectional ref vert to tri
		for (uint32_t i = 0; i < DF->rows(); i++) {
			// check if tri contains more than 2 points in targets
			Eigen::Vector3i tri = DF->row(i);

			uint8_t contained = 0;
			for (uint8_t j = 0; j < 3; j++) {
				if (std::find(targets.begin(), targets.end(), tri[j]) != targets.end()) {
					// new point is in tri contained
					contained++;
					// point index to first target (new index of merged point)
					DF->row(i)[j] = targets[0];
				}
			}

			// 0. tri has 0 verts -> stays
			// 1. tri has 1 verts -> new point gets moved
			// 2. tri has 2 verts -> tri gets removed
			// 3. tri has 3 verts -> tri gets removed
			if (contained >= 2) { // remove tri
				removedFaces.push_back(i);
			}
		}

		return removedFaces;
	}

	MeshDecimator::MeshDecimator(void) {
	}//Constructor

	MeshDecimator::~MeshDecimator(void) {
		//clear();
	}//Destructor
}