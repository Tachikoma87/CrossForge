#include "PinocchioTools.hpp"
#include <cmath>
#include <iostream>
#include "PinocchioTools.hpp"

namespace nsPinocchioTools {

	void pnSkeleton::PinitCompressed() {
		initCompressed();
	}
	map<string, int> pnSkeleton::getJointNames() {
		return jointNames;
	}
	void pnSkeleton::setJointNames(map<string, int> jointNames) {
		this->jointNames = jointNames;
	}
	void pnSkeleton::PmakeJoint(const string& name, const Vector3& pos, const string& previous) {
		makeJoint(name,pos,previous);
	}
	void pnSkeleton::PmakeSymmetric(const string& name1, const string& name2) {
		makeSymmetric(name1, name2);
	}
	void pnSkeleton::PsetFoot(const string& name) {
		setFoot(name); 
	}
	void pnSkeleton::PsetFat(const string& name) {
		setFat(name);
	}

	std::vector<CForge::T3DMesh<float>::Bone*> gatherBones(CForge::T3DMesh<float>::Bone* root) {
		std::vector<CForge::T3DMesh<float>::Bone*> ret;
		ret.push_back(root);
		for (uint32_t i = 0; i < root->Children.size(); i++) {
			std::vector<CForge::T3DMesh<float>::Bone*> bones = gatherBones(root->Children[i]);
			ret.insert(ret.end(),bones.begin(),bones.end());
		}
		return ret;
	}
	
	void convertSkeleton(CForge::T3DMesh<float>::Bone* in, nsPiR::Skeleton* out, CVScalingInfo* CVSInfo,
			std::vector<BonePair> symmetry,
			std::vector<CForge::T3DMesh<float>::Bone*> fat,
			std::vector<CForge::T3DMesh<float>::Bone*> foot,
			Eigen::Matrix3f rotation,
			std::vector<Eigen::Vector3f>* joints) {
		
		std::vector<CForge::T3DMesh<float>::Bone*> inList = gatherBones(in);
		
		// find scaling and offsetfor skeleton
		// bounding box of skeleton
		Eigen::Vector3f min,max,center,d;
		std::vector<Eigen::Vector3f> poss;
		for (uint32_t i = 0; i < inList.size(); i++) {
			Eigen::Matrix4f mat = inList[i]->OffsetMatrix.inverse();
			Eigen::Vector3f pos = rotation * Eigen::Vector3f(-mat.data()[12],mat.data()[13],mat.data()[14]);
			poss.push_back(pos);
			if (i==0)
				min = max = pos;
			for (uint32_t j = 0; j < 3; j++) {
				min[j] = std::fmin(min[j],pos[j]);
				max[j] = std::fmax(max[j],pos[j]);
			}
		}
		center = (min+max)*0.5f;
		d = max-min;
		float maxBound = 0.8f;
		CVSInfo->scaling = maxBound*2.0f/std::fmax(d[0],std::fmax(d[1],d[2]));
		CVSInfo->offset = center*CVSInfo->scaling; //TODO check for correctness
		
		// create Skeleton
		pnSkeleton* skl = (pnSkeleton*) out;
		
		// apply scaling and offset
		for (uint32_t i = 0; i < inList.size(); i++) {
			Eigen::Matrix4f mat = inList[i]->OffsetMatrix.inverse();
			Vector3 o = Vector3(CVSInfo->offset[0],CVSInfo->offset[1],CVSInfo->offset[2]);
			Vector3 pos = Vector3(poss[i][0],poss[i][1],poss[i][2])*CVSInfo->scaling - o;
			joints->push_back(Eigen::Vector3f(pos[0],pos[1],pos[2]));
			if (inList[i]->pParent)
				skl->PmakeJoint(inList[i]->Name, pos, inList[i]->pParent->Name);
			else
				skl->PmakeJoint(inList[i]->Name, pos);
			if (inList[i]->Children.size()==0 && inList[i]->pParent) { //endeffector, add one node
				Eigen::Matrix4f mat = inList[i]->pParent->OffsetMatrix.inverse();
				Eigen::Vector3f apos = rotation * Eigen::Vector3f(-mat.data()[12],mat.data()[13],mat.data()[14]);
				Vector3 piApos = Vector3(apos[0],apos[1],apos[2]);
				piApos = piApos*CVSInfo->scaling - o;
				Vector3 epos = piApos + (pos-piApos)*1.1f;
				skl->PmakeJoint(inList[i]->Name + "_piTend", epos, inList[i]->Name);
			}
		}
		
		for (uint32_t i = 0; i < symmetry.size(); i++)
			skl->PmakeSymmetric(symmetry[i].pair[0]->Name,symmetry[i].pair[1]->Name);
		
		skl->PinitCompressed();
		
		for (uint32_t i = 0; i < foot.size(); i++)
			skl->PsetFoot(foot[i]->Name+"_piTend");
		for (uint32_t i = 0; i < fat.size(); i++)
			skl->PsetFat(fat[i]->Name);
	}

	void adaptSkeleton( nsPiR::PinocchioOutput* in, nsPiR::Skeleton* inSkl, CForge::T3DMesh<float>::Bone * out) {
		
		std::vector<CForge::T3DMesh<float>::Bone*> inList = gatherBones(out);
		
		for (uint32_t i = 0; i < inList.size(); i++) {
			// adapt offset matrix
			Eigen::Matrix4f mat = inList[i]->OffsetMatrix.inverse();
			uint32_t sklIndex = inSkl->getJointForName(inList[i]->Name);
			Vector3 piPos = in->embedding[sklIndex];
			
			// get direction of bone for rotation
			Eigen::Vector3f pos = Eigen::Vector3f(piPos[0],piPos[1],piPos[2]);
			Eigen::Vector3f vec = Eigen::Vector3f(0.0f,0.0f,0.0f);
			if (inList[i]->Children.size() == 1) {
				uint32_t sklIndex = inSkl->getJointForName(inList[i]->Children[0]->Name);
				Vector3 piVec2 = in->embedding[sklIndex];
				Eigen::Vector3f pos2 = Eigen::Vector3f(piVec2[0],piVec2[1],piVec2[2]);
				vec = pos2-pos;
			}
			else if (inList[i]->pParent){
				uint32_t sklIndex = inSkl->getJointForName(inList[i]->pParent->Name);
				Vector3 piVec2 = in->embedding[sklIndex];
				Eigen::Vector3f pos2 = Eigen::Vector3f(piVec2[0],piVec2[1],piVec2[2]);
				vec = pos-pos2;
			}
			else if (inList[i]->Children.size()>0) {
				uint32_t sklIndex = inSkl->getJointForName(inList[i]->Children[0]->Name);
				Vector3 piVec2 = in->embedding[sklIndex];
				Eigen::Vector3f pos2 = Eigen::Vector3f(piVec2[0],piVec2[1],piVec2[2]);
				vec = pos2-pos;
			}
			
			Eigen::Matrix3f rot = GraphicsUtility::alignVectors(Eigen::Vector3f(0.0,1.0,0.0),vec.normalized());
			
			mat.data()[12] = pos[0];
			mat.data()[13] = pos[1];
			mat.data()[14] = pos[2];
			mat.block<3,3>(0,0) = rot;

			inList[i]->OffsetMatrix = mat.inverse();
		}
	}

	void applyWeights(nsPiR::Skeleton* in, CForge::T3DMesh<float>* out, const CVScalingInfo& CVSInfo,
	                  nsPiR::PinocchioOutput& piO, uint32_t vertexCount) {
		if (!piO.attachment)
			throw CForgeExcept("PinocchioTools: applyWeights attatchment not valid");
		std::vector<CForge::T3DMesh<float>::Bone*> outList = gatherBones(out->rootBone());
		
		// center embedding and mesh at boneRoot = 0
		std::vector<Eigen::Vector3f> centeredVertices;
		Eigen::Vector3f center = Eigen::Vector3f(piO.embedding[0][0],piO.embedding[0][1],piO.embedding[0][2]);
		for (uint32_t i = 0; i < out->vertexCount(); i++) {
			centeredVertices.push_back(out->vertex(i)-center);
			//centeredVertices.back() *= 1.0/CVSInfo.scaling;
		}
		out->vertices(&centeredVertices);
		Vector3 root = piO.embedding[0];
		for (uint32_t i = 0; i < piO.embedding.size(); i++) {
			piO.embedding[i] -= root;
			//piO.embedding[i] *= 1.0/CVSInfo.scaling;
		}
		
		//for (uint32_t k=0;k<piO.embedding.size();k++) {
		//	for (uint32_t j = 0; j < 3; j++) {
		//		std::cout << piO.embedding.at(k)[j] << " ";
		//	}
		//	std::cout << "\n";
		//}
		
		// TODO set mesh to T-Pose
		// set position of bones
		adaptSkeleton(&piO,in,out->rootBone());
		//apply weights
		for (uint32_t i = 0; i < vertexCount; i++) {
			Vector<double,-1> weights = piO.attachment->getWeights(i);
			// TODO assumption that attatchment and outList index is the same
			for (uint32_t k = 0; k < outList.size(); k++) {
				uint32_t jointIndex = in->getJointForName(outList[k]->Name);
				for (uint32_t j = 0; j < weights.size(); j++) {
					if (weights[j] > 0.0 && j==jointIndex) {
						outList[k]->VertexInfluences.push_back(i);
						outList[k]->VertexWeights.push_back(weights[j]);
					}
				}
			}
		}
	}

	void copyAnimation(CForge::T3DMesh<float>* source, CForge::T3DMesh<float>* target, uint32_t animationIndex) {
		
		Eigen::Vector3f min,max,d;
		{
			Eigen::Matrix4f mat = source->getBone(0)->OffsetMatrix.inverse();
			min = Eigen::Vector3f(mat.data()[12],mat.data()[13],mat.data()[14]);
			max = min;
		}
		for (uint32_t i = 1; i < source->boneCount(); i++) {
			Eigen::Matrix4f mat = source->getBone(i)->OffsetMatrix.inverse();
			for (uint32_t j = 0; j < 3; j++) {
				min[j] = std::fmin(min[j],mat.data()[12+j]);
				max[j] = std::fmax(max[j],mat.data()[12+j]);
			}
		}
		d = max-min;
		float scale = 1.0/std::fmax(d[0],std::fmax(d[1],d[2]));
		
		target->addSkeletalAnimation(source->getSkeletalAnimation(animationIndex));
		T3DMesh<float>::SkeletalAnimation* anim = target->getSkeletalAnimation(target->skeletalAnimationCount()-1);
		for (uint32_t i = 0; i < anim->Keyframes.size(); i++) {
			for (uint32_t j = 0; j < anim->Keyframes[i]->Positions.size(); j++) {
				anim->Keyframes[i]->Positions[j] = anim->Keyframes[i]->Positions[j]*scale;
			}
		}
	}
	
	void MeshToTPose(nsPiR::Mesh* in, nsPiR::Skeleton* pinSkl, T3DMesh<float>::Bone* targetSkl) {
		
	}
	
	void convertMesh(CForge::T3DMesh<float>* in, nsPiR::Mesh* out) {
		nsPiR::Mesh* ret = out;
		
		// add vertices
		for (uint32_t i = 0; i < in->vertexCount(); i++) {
			double x, y, z;
			x = in->vertex(i)[0];
			y = in->vertex(i)[1];
			z = in->vertex(i)[2];
			ret->vertices.resize(ret->vertices.size() + 1);
			ret->vertices.back().pos = Vector3(x, y, z);
		}
		// add faces
		for (uint32_t i = 0; i < in->submeshCount(); i++) {
			auto curSubM = in->getSubmesh(i);
			for (uint32_t j = 0; j < curSubM->Faces.size(); j++) {
				T3DMesh<float>::Face face = curSubM->Faces[j];

				int first = ret->edges.size();
				ret->edges.resize(ret->edges.size() + 3);
				ret->edges[first].vertex = face.Vertices[0];
				ret->edges[first + 1].vertex = face.Vertices[1];
				ret->edges[first + 2].vertex = face.Vertices[2];
			}
		}

		//reconstruct the rest of the information
		int verts = ret->vertices.size();

		if(verts == 0)
			return;

		for(int i = 0; i < (int)ret->edges.size(); ++i) { //make sure all vertex indices are valid
			if(ret->edges[i].vertex < 0 || ret->edges[i].vertex >= verts) {
				std::cout << "Error: invalid vertex index " << ret->edges[i].vertex << endl;
			}
		}

		fixDupFaces(ret);
		ret->computeTopology();
		if(ret->integrityCheck())
			std::cout << "Correct convert: " << ret->vertices.size() << " vertices, " << ret->edges.size() << " edges" << endl;
		else
			std::cout << "Somehow convert: " << ret->vertices.size() << " vertices, " << ret->edges.size() << " edges" << endl;
		ret->computeVertexNormals();
	}
	
	
	// from nsPiR::Mesh
	struct MFace
	{
		MFace(int v1, int v2, int v3)
		{
			v[0] = v1; v[1] = v2; v[2] = v3;
			sort(v, v + 3);
		}

		bool operator<(const MFace &f) const { return lexicographical_compare(v, v + 3, f.v, f.v + 3); }
		int v[3];
	};
	void fixDupFaces(nsPiR::Mesh* mesh)
	{
		int i;
		map<MFace, int> faces;
		for(i = 0; i < (int)mesh->edges.size(); i += 3) {
			MFace current(mesh->edges[i].vertex, mesh->edges[i + 1].vertex, mesh->edges[i + 2].vertex);

			if(faces.count(current)) {
				int oth = faces[current];
				if(oth == -1) {
					faces[current] = i;
					continue;
				}
				faces[current] = -1;
				int newOth = mesh->edges.size() - 6;
				int newCur = mesh->edges.size() - 3;

				mesh->edges[oth] = mesh->edges[newOth];
				mesh->edges[oth + 1] = mesh->edges[newOth + 1];
				mesh->edges[oth + 2] = mesh->edges[newOth + 2];
				mesh->edges[i] = mesh->edges[newCur];
				mesh->edges[i + 1] = mesh->edges[newCur + 1];
				mesh->edges[i + 2] = mesh->edges[newCur + 2];

				MFace newOthF(mesh->edges[newOth].vertex, mesh->edges[newOth + 1].vertex, mesh->edges[newOth + 2].vertex);
				faces[newOthF] = newOth;

				mesh->edges.resize(mesh->edges.size() - 6);
				i -= 3;
			}
			else {
				faces[current] = i;
			}
		}

		//scan for unreferenced vertices and get rid of them
		set<int> referencedVerts;
		for(i = 0; i < (int)mesh->edges.size(); ++i) {
			if(mesh->edges[i].vertex < 0 || mesh->edges[i].vertex >= (int)mesh->vertices.size())
				continue;
			referencedVerts.insert(mesh->edges[i].vertex);
		}

		vector<int> newIdxs(mesh->vertices.size(), -1);
		int curIdx = 0;
		for(i = 0; i < (int)mesh->vertices.size(); ++i) {
			if(referencedVerts.count(i))
				newIdxs[i] = curIdx++;
		}

		for(i = 0; i < (int)mesh->edges.size(); ++i) {
			if(mesh->edges[i].vertex < 0 || mesh->edges[i].vertex >= (int)mesh->vertices.size())
				continue;
			mesh->edges[i].vertex = newIdxs[mesh->edges[i].vertex];
		}
		for(i = 0; i < (int)mesh->vertices.size(); ++i) {
			if(newIdxs[i] > 0)
				mesh->vertices[newIdxs[i]] = mesh->vertices[i];
		}
		mesh->vertices.resize(referencedVerts.size());
	}
}
