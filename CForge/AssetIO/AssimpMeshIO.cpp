#include "AssimpMeshIO.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../CForge/Core/SLogger.h"

using namespace Assimp;

namespace CForge {
	AssimpMeshIO::AssimpMeshIO(void): I3DMeshIO("AssimpMeshIO") {

	}//Constructor

	AssimpMeshIO::~AssimpMeshIO(void) {
		clear();
	}//Destructor

	void AssimpMeshIO::init(void) {
		
	}//initialize

	void AssimpMeshIO::clear(void) {
		
	}//clear

	void AssimpMeshIO::release(void) {
		delete this;
	}

	void AssimpMeshIO::load(const std::string Filepath, T3DMesh<float> *pMesh){
		const aiScene *pScene = m_Importer.ReadFile(Filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights | aiProcess_OptimizeGraph);

		if (nullptr == pScene) throw CForgeExcept("Failed to load model from resource " + Filepath);

		try {
			aiSceneTo3DMesh(pScene, pMesh, File::removeFilename(Filepath));
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
		}
		
		m_Importer.FreeScene();
	}//load

	void AssimpMeshIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {
		throw CForgeExcept("Storing 3D models not implemented by this plugin!");
	}//store

	bool AssimpMeshIO::accepted(const std::string Filepath, Operation Op) {
		bool Rval = false;
		if (Op == OP_LOAD) {
			if (Filepath.find(".fbx") != std::string::npos) Rval = true;
			else if (Filepath.find(".obj") != std::string::npos) Rval = true;
			else if (Filepath.find(".ply") != std::string::npos) Rval = true;
			else if (Filepath.find(".stl") != std::string::npos) Rval = true;
		}
		else {
			Rval = false;
		}
		
		return Rval;
	}//accepted

	void AssimpMeshIO::aiSceneTo3DMesh(const aiScene* pScene, T3DMesh<float>* pMesh, std::string Directory) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (nullptr == pScene) throw NullpointerExcept("pScene");

		pMesh->clear();


		std::vector<Eigen::Vector3f> Positions;
		std::vector<Eigen::Vector3f> Normals;
		std::vector<Eigen::Vector3f> Tangents;
		std::vector<Eigen::Vector3f> UVWs;

		std::vector<T3DMesh<float>::Bone*> Bones;
		std::vector<T3DMesh<float>::SkeletalAnimation*> SekeltonAnimations;

		uint32_t PositionsOffset = 0;
		uint32_t NormalsOffset = 0;
		uint32_t TangentsOffset = 0;
		uint32_t UVWsOffset = 0;

		uint32_t IndexOffset = 0;

		for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
			aiMesh* pM = pScene->mMeshes[i];

			for (uint32_t k = 0; k < pM->mNumVertices; ++k) {
				// collect vertices
				Positions.push_back(toEigenVec(pM->mVertices[k]));
				// collect normals
				if (pM->mNormals != nullptr) Normals.push_back(toEigenVec(pM->mNormals[k]));
				// collect tangents
				if (pM->mTangents != nullptr) Tangents.push_back(toEigenVec(pM->mTangents[k]));
				// collect texture coordinates
				if (pM->mTextureCoords[0] != nullptr && pM->GetNumUVChannels() > 0) UVWs.push_back(toEigenVec(pM->mTextureCoords[0][k]));
			}

			// now we retrieve the faces (create submesh)
			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh();
			for (uint32_t k = 0; k < pM->mNumFaces; ++k) {
				aiFace F = pM->mFaces[k];
				T3DMesh<float>::Face Face;
				Face.Material = (int32_t)pM->mMaterialIndex;
				for (uint32_t j = 0; j < F.mNumIndices && j < 4; j++) {
					Face.Vertices[j] = PositionsOffset + F.mIndices[j];
					if (nullptr != pM->mNormals) Face.Normals[j] = NormalsOffset + F.mIndices[j];
					if (nullptr != pM->mTangents) Face.Tangents[j] =  TangentsOffset + F.mIndices[j];
					if (nullptr != pM->mTextureCoords) Face.UVWs[j] = UVWsOffset + F.mIndices[j];
				}//for[face indices]

				pSubmesh->Faces.push_back(Face);
			}//for[number of faces]
			// add submesh to model
			pMesh->addSubmesh(pSubmesh, false);

			// add bones
			for (uint32_t k = 0; k < pM->mNumBones; k++) {

				T3DMesh<float>::Bone* pBone = new T3DMesh<float>::Bone();
				pBone->ID = Bones.size();
				pBone->OffsetMatrix = toEigenMat(pM->mBones[k]->mOffsetMatrix);
				pBone->Name = pM->mBones[k]->mName.C_Str();

				for (uint32_t j = 0; j < pM->mBones[k]->mNumWeights; ++j) {
					pBone->VertexInfluences.push_back(PositionsOffset + pM->mBones[k]->mWeights[j].mVertexId);
					pBone->VertexWeights.push_back(pM->mBones[k]->mWeights[j].mWeight);
				}
				
				Bones.push_back(pBone);
			}//for[bones]

			PositionsOffset += pM->mNumVertices;
			if (nullptr != pM->mNormals) NormalsOffset += pM->mNumVertices;
			if (nullptr != pM->mTangents) TangentsOffset += pM->mNumVertices;
			if (nullptr != pM->mTextureCoords) UVWsOffset += pM->mNumVertices;
		}//for[all meshes]

		// apply global transformation
		Eigen::Matrix4f GlobalTransform = toEigenMat(pScene->mRootNode->mTransformation);
		for (auto& i : Positions) {
			Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 1.0f);
			i = Eigen::Vector3f(p.x(), p.y(), p.z());
		}
		if (Normals.size() > 0) {
			for (auto& i : Normals) {
				Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 0.0f);
				i = Eigen::Vector3f(p.x(), p.y(), p.y());
			}
		}
		if (Tangents.size() > 0) {
			for (auto& i : Tangents) {
				Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 0.0f);
				i = Eigen::Vector3f(p.x(), p.y(), p.z());
			}
		}

		// set positions, normals, tangents
		pMesh->vertices(&Positions);
		if (Normals.size() > 0) pMesh->normals(&Normals);
		if (Tangents.size() > 0) pMesh->tangents(&Tangents);
		if (UVWs.size() > 0) pMesh->textureCoordinates(&UVWs);

		//add materials
		for (uint32_t i = 0; i < pScene->mNumMaterials; ++i) {

			aiMaterial *pMat = pScene->mMaterials[i];
			aiString Filepath;

			T3DMesh<float>::Material Mat;

			
			if (pMat->GetTextureCount(aiTextureType_DIFFUSE) > 0){
				pMat->GetTexture(aiTextureType_DIFFUSE, 0, &Filepath);
				Mat.TexAlbedo = File::absolute(Directory + Filepath.C_Str());
			}else if (pMat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
				pMat->GetTexture(aiTextureType_AMBIENT, 0, &Filepath);
				Mat.TexAlbedo = File::absolute(Directory + Filepath.C_Str());
			}

			if (pMat->GetTextureCount(aiTextureType_NORMALS) > 0 ){
				pMat->GetTexture(aiTextureType_NORMALS, 0, &Filepath);
				Mat.TexNormal = File::absolute(Directory + Filepath.C_Str());
			}

			if (pMat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
				pMat->GetTexture(aiTextureType_HEIGHT, 0, &Filepath);
				Mat.TexDepth = File::absolute(Directory + Filepath.C_Str());
			}

			ai_real Buffer[4];
			pMat->Get(AI_MATKEY_COLOR_DIFFUSE, (ai_real*)Buffer, nullptr);
			Mat.Color = Eigen::Vector4f(Buffer[0], Buffer[1], Buffer[2], Buffer[3]);
				
			pMesh->addMaterial(&Mat, true);
		}//for[all materials]

		// join identical bones (if names are identical)
	
		for (auto i : Bones) {
			if (nullptr == i) continue;
			for (uint32_t k = i->ID + 1; k < Bones.size(); ++k) {
				if (Bones[k] == nullptr) continue;
				if (i->Name.compare(Bones[k]->Name) == 0) {
					// copy vertx influces
					for (auto l : Bones[k]->VertexInfluences) i->VertexInfluences.push_back(l);
					for (auto l : Bones[k]->VertexWeights) i->VertexWeights.push_back(l);
					delete Bones[k];
					Bones[k] = nullptr;
				}
			}//for[following bones]
		}//for[all bones]

		std::vector<T3DMesh<float>::Bone*> Bones2;
		for (auto i : Bones) {
			if (i != nullptr) Bones2.push_back(i);
		}
		Bones = Bones2;

		for (uint32_t i = 0; i < Bones.size(); ++i) Bones[i]->ID = i;

		// skeleton
		aiNode* pRoot = pScene->mRootNode;
		retrieveBoneHierarchy(pRoot, &Bones);


		// find root bone (the one without parent)
		T3DMesh<float>::Bone* pRootBone = nullptr;
		for (auto i : Bones) {
			if (i->pParent == nullptr) pRootBone = i;
		}//for[all bones]

		std::vector<T3DMesh<float>::SkeletalAnimation*> BoneAnimations;

		// retrieve animation data
		for (uint32_t i = 0; i < pScene->mNumAnimations; ++i) {
			aiAnimation* pAnim = pScene->mAnimations[i];

			// create new animation
			T3DMesh<float>::SkeletalAnimation* pSkelAnim = new T3DMesh<float>::SkeletalAnimation();
			BoneAnimations.push_back(pSkelAnim);

			pSkelAnim->Duration = pAnim->mDuration;
			pSkelAnim->Speed = pAnim->mTicksPerSecond;
			pSkelAnim->Name = pAnim->mName.C_Str();

			// create keyframe for every bone
			for (uint32_t k = 0; k < Bones.size(); k++) {
				pSkelAnim->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
				pSkelAnim->Keyframes[k]->BoneID = k;
				pSkelAnim->Keyframes[k]->ID = k;
			}//for[all bones]

			for (uint32_t k = 0; k < pAnim->mNumChannels; ++k) {
				aiNodeAnim *pNodeAnim = pAnim->mChannels[k];

				T3DMesh<float>::Bone* pB = getBoneFromName(pNodeAnim->mNodeName.C_Str(), &Bones);

				if (nullptr != pB) {
					T3DMesh<float>::BoneKeyframes* pKeys = pSkelAnim->Keyframes[pB->ID];

					for (uint32_t l = 0; l < pNodeAnim->mNumPositionKeys; l++) {
						pKeys->Positions.push_back( toEigenVec(pNodeAnim->mPositionKeys[l].mValue) );
						pKeys->Timestamps.push_back(pNodeAnim->mPositionKeys[l].mTime);
					}//for[positions]

					for (uint32_t l = 0; l < pNodeAnim->mNumRotationKeys; l++) {
						pKeys->Rotations.push_back(toEigenQuat(pNodeAnim->mRotationKeys[l].mValue));
					}//for[rotations]
					
					for (uint32_t l = 0; l < pNodeAnim->mNumScalingKeys; l++) {
						pKeys->Scalings.push_back(toEigenVec(pNodeAnim->mScalingKeys[l].mValue));
					}
				}
				else {
					printf("Got animation data for unhandled node: %s\n", pNodeAnim->mNodeName.C_Str());
				}
				
			}//for[channels]

		}//for[all animations]


		if (Bones.size() > 0) {
			// set skeleton
			// the mesh structure can have the allocated memory
			pMesh->bones(&Bones, false);
			Bones.clear();
		}

		for (auto i : BoneAnimations) pMesh->addSkeletalAnimation(i, false);

	}//aiMeshTo3DMesh

	void AssimpMeshIO::retrieveBoneHierarchy(aiNode* pNode, std::vector<T3DMesh<float>::Bone*>* pBones) {
		if (nullptr == pNode) return; // end of recursion
		if (nullptr == pBones) throw NullpointerExcept("pBones");

		T3DMesh<float>::Bone* pCurrentBone = getBoneFromName(pNode->mName.C_Str(), pBones);
		if (nullptr != pCurrentBone) {
			// retrieve parent
			if(nullptr != pNode->mParent) pCurrentBone->pParent = getBoneFromName(pNode->mParent->mName.C_Str(), pBones);
			// add children
			for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
				T3DMesh<float>::Bone* pChild = getBoneFromName(pNode->mChildren[i]->mName.C_Str(), pBones);
				if(nullptr != pChild) pCurrentBone->Children.push_back(pChild);
			}//for[all child nodes]	
		}//if[current bone was found]

		// recursion
		for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
			retrieveBoneHierarchy(pNode->mChildren[i], pBones);
		}

	}//retriveBoneHierarchy

	T3DMesh<float>::Bone* AssimpMeshIO::getBoneFromName(std::string Name, std::vector<T3DMesh<float>::Bone*>* pBones) {
		T3DMesh<float>::Bone* pRval = nullptr;

		for (auto i : (*pBones)) {
			if (0 == i->Name.compare(Name)) {
				pRval = i;
				break;
			}
		}//for[all bones]

		return pRval;
	}//

	Eigen::Vector3f AssimpMeshIO::toEigenVec(const aiVector3D Vec)const {
		return Eigen::Vector3f(Vec.x, Vec.y, Vec.z);
	}//toEigenVec

	Eigen::Matrix4f AssimpMeshIO::toEigenMat(const aiMatrix4x4 Mat)const {
		Eigen::Matrix4f Rval;
		Rval(0, 0) = Mat.a1;
		Rval(0, 1) = Mat.a2;
		Rval(0, 2) = Mat.a3;
		Rval(0, 3) = Mat.a4;

		Rval(1, 0) = Mat.b1;
		Rval(1, 1) = Mat.b2;
		Rval(1, 2) = Mat.b3;
		Rval(1, 3) = Mat.b4;

		Rval(2, 0) = Mat.c1;
		Rval(2, 1) = Mat.c2;
		Rval(2, 2) = Mat.c3;
		Rval(2, 3) = Mat.c4;

		Rval(3, 0) = Mat.d1;
		Rval(3, 1) = Mat.d2;
		Rval(3, 2) = Mat.d3;
		Rval(3, 3) = Mat.d4;

		return Rval;
	}//toEigenMat

	Eigen::Quaternionf AssimpMeshIO::toEigenQuat(const aiQuaternion Q)const {
		Eigen::Quaternionf Rval;
		Rval.x() = Q.x;
		Rval.y() = Q.y;
		Rval.z() = Q.z;
		Rval.w() = Q.w;
		return Rval;
	}//toEigenQuat

}//name space