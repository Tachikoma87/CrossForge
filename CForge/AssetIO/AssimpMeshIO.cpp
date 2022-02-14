#include "AssimpMeshIO.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>
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
		const aiScene *pScene = m_Importer.ReadFile(Filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure);

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
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified!");
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		std::string FileType = "";
		if (Filepath.find(".fbx") != std::string::npos) FileType = "fbx";
		else if (Filepath.find(".obj") != std::string::npos) FileType = "obj";
		else if (Filepath.find(".ply") != std::string::npos) FileType = "ply";
		else if (Filepath.find(".stl") != std::string::npos) FileType = "stl";
		else if (Filepath.find(".x") != std::string::npos) FileType = "x";

		// convert mesh to aiScene
		aiScene S;
		T3DMeshToAiScene(pMesh, &S);
		Exporter Ex;
		Ex.Export(&S, FileType.c_str(), Filepath.c_str());
	}//store

	bool AssimpMeshIO::accepted(const std::string Filepath, Operation Op) {
		bool Rval = false;
		if (Op == OP_LOAD) {
			if (Filepath.find(".fbx") != std::string::npos) Rval = true;
			else if (Filepath.find(".obj") != std::string::npos) Rval = true;
			else if (Filepath.find(".ply") != std::string::npos) Rval = true;
			else if (Filepath.find(".stl") != std::string::npos) Rval = true;
			else if (Filepath.find(".x") != std::string::npos) Rval = true;
			else if (Filepath.find(".glb") != std::string::npos) Rval = true;
			else if (Filepath.find(".bvh") != std::string::npos) Rval = true;
		}
		else {
			if (Filepath.find(".fbx") != std::string::npos) Rval = true;
			else if (Filepath.find(".obj") != std::string::npos) Rval = true;
			else if (Filepath.find(".ply") != std::string::npos) Rval = true;
			else if (Filepath.find(".stl") != std::string::npos) Rval = true;
			else if (Filepath.find(".x") != std::string::npos) Rval = true;
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
			for (uint32_t k = 0; k < Bones.size() /*pAnim->mNumChannels*/; k++) {
				pSkelAnim->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
				pSkelAnim->Keyframes[k]->ID = k;
			}//for[all bones]

			for (uint32_t k = 0; k < pAnim->mNumChannels; ++k) {
				aiNodeAnim *pNodeAnim = pAnim->mChannels[k];

				T3DMesh<float>::Bone* pB = getBoneFromName(pNodeAnim->mNodeName.C_Str(), &Bones);

				int32_t KeyID = (nullptr == pB) ? k : pB->ID;

				pSkelAnim->Keyframes[KeyID]->BoneID = (nullptr == pB) ? -1 : pB->ID;
				pSkelAnim->Keyframes[KeyID]->BoneName = pNodeAnim->mNodeName.C_Str();

				T3DMesh<float>::BoneKeyframes* pKeys = (pB == nullptr) ? pSkelAnim->Keyframes[k] : pSkelAnim->Keyframes[pB->ID];

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

	void AssimpMeshIO::T3DMeshToAiScene(const T3DMesh<float>* pMesh, aiScene* pScene) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (nullptr == pScene) throw NullpointerExcept("pScene");
	
		pScene->mRootNode = new aiNode();
		pScene->mRootNode->mName = "root";

		// add meshes
		pScene->mNumMeshes = pMesh->submeshCount();
		pScene->mRootNode->mNumMeshes = pMesh->submeshCount();
		pScene->mMeshes = new aiMesh*[pMesh->submeshCount()];
		pScene->mRootNode->mMeshes = new unsigned int[pMesh->submeshCount()];
		for (uint32_t i = 0; i < pMesh->submeshCount(); ++i) {
			pScene->mMeshes[i] = new aiMesh();
			pScene->mRootNode->mMeshes[i] = i;
		}

		for (uint32_t i = 0; i < pMesh->submeshCount(); ++i) {
			std::vector<int32_t> VertexIDs;

			aiMesh* pM = pScene->mMeshes[i];

			// set faces
			const T3DMesh<float>::Submesh* pSub = pMesh->getSubmesh(i);

			pM->mMaterialIndex = pSub->Faces[0].Material;

			pM->mFaces = new aiFace[pSub->Faces.size()];
			for (uint32_t k = 0; k < pSub->Faces.size(); ++k) {
				aiFace F;
				F.mNumIndices = 3;
				F.mIndices = new unsigned int[3];
				F.mIndices[0] = pSub->Faces[k].Vertices[0];
				F.mIndices[1] = pSub->Faces[k].Vertices[1];
				F.mIndices[2] = pSub->Faces[k].Vertices[2];
				pM->mFaces[k] = F;

				// collect vertex ids
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), pSub->Faces[k].Vertices[0])) VertexIDs.push_back(pSub->Faces[k].Vertices[0]);
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), pSub->Faces[k].Vertices[1])) VertexIDs.push_back(pSub->Faces[k].Vertices[1]);
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), pSub->Faces[k].Vertices[2])) VertexIDs.push_back(pSub->Faces[k].Vertices[2]);

			}//for[faces]
			pM->mNumFaces = pSub->Faces.size();

			uint32_t MaxID = 0;
			for (auto k : VertexIDs) {
				if (k > MaxID) MaxID = k;
			}

			std::vector<int32_t> VertexMap;
			VertexMap.reserve(MaxID);
			for (uint32_t k = 0; k <= MaxID; ++k) VertexMap.push_back(-1);

			pM->mNumVertices = VertexIDs.size();
			pM->mVertices = new aiVector3D[VertexIDs.size()];

			for (uint32_t k = 0; k < VertexIDs.size(); ++k) {
				pM->mVertices[k] = toAiVector(pMesh->vertex(VertexIDs[k]));
				VertexMap[VertexIDs[k]] = k;
			}

			if (pMesh->normalCount() > 0) {
				pM->mNormals = new aiVector3D[VertexIDs.size()];
				for (uint32_t k = 0; k < VertexIDs.size(); k++) pM->mNormals[k] = toAiVector(pMesh->normal(VertexIDs[k]));
			}
			if (pMesh->textureCoordinatesCount() > 0) {
				pM->mTextureCoords[0] = new aiVector3D[VertexIDs.size()];
				pM->mNumUVComponents[0] = 2;
				for (uint32_t k = 0; k < VertexIDs.size(); ++k) pM->mTextureCoords[0][k] = toAiVector(pMesh->textureCoordinate(VertexIDs[k]));
			}

			// Map vertices
			for (uint32_t k = 0; k < pSub->Faces.size(); ++k) {
				pM->mFaces[k].mIndices[0] = VertexMap[pM->mFaces[k].mIndices[0]];
				pM->mFaces[k].mIndices[1] = VertexMap[pM->mFaces[k].mIndices[1]];
				pM->mFaces[k].mIndices[2] = VertexMap[pM->mFaces[k].mIndices[2]];
			}

			std::vector<aiBone*> Bones;
			// gather influences
			for (uint32_t k = 0; k < pMesh->boneCount(); ++k) {
				auto* pBone = pMesh->getBone(k);

				std::vector<int32_t> InfluenceIDs;
				std::vector<float> Weights;

				for (uint32_t j = 0; j < pBone->VertexInfluences.size(); ++j) {
					int32_t Index = pBone->VertexInfluences[j];
					if (Index < VertexMap.size() && VertexMap[Index] != -1) {
						InfluenceIDs.push_back(VertexMap[Index]);
						Weights.push_back(pBone->VertexWeights[j]);
					}
				}//for[influences]
				
				if (InfluenceIDs.size() > 0) {
					aiBone* pB = new aiBone();
					pB->mName = pBone->Name.c_str();
					pB->mOffsetMatrix = toAiMatrix(pBone->OffsetMatrix);
					pB->mNumWeights = InfluenceIDs.size();
					pB->mWeights = new aiVertexWeight[pB->mNumWeights];
					for (uint32_t j = 0; j < InfluenceIDs.size(); ++j) {
						pB->mWeights[j].mVertexId = InfluenceIDs[j];
						pB->mWeights[j].mWeight = Weights[j];
					}
					Bones.push_back(pB);
				}//if[valid bone]
			}//for[all bones]

			if (Bones.size() > 0) {
				// store bones of this mesh
				pM->mBones = new aiBone*[Bones.size()];
				pM->mNumBones = Bones.size();
				for (uint32_t k = 0; k < Bones.size(); ++k) pM->mBones[k] = Bones[k];
				Bones.clear();
			}
			

		}//for[subMeshes]

		// store materials
		pScene->mNumMaterials = pMesh->materialCount();
		pScene->mMaterials = new aiMaterial*[pMesh->materialCount()];
		for (uint32_t i = 0; i < pScene->mNumMaterials; ++i) {
			pScene->mMaterials[i] = new aiMaterial();

			const T3DMesh<float>::Material* pMat = pMesh->getMaterial(i);

			aiString MatName = aiString(("Mat-" + std::to_string(i)).c_str());
			
			pScene->mMaterials[i]->AddProperty(&MatName, AI_MATKEY_NAME);

			pScene->mMaterials[i]->AddProperty(pMat->Color.data(), 4, AI_MATKEY_COLOR_AMBIENT);
			pScene->mMaterials[i]->AddProperty(pMat->Color.data(), 4, AI_MATKEY_COLOR_DIFFUSE);

			aiString Filepath;
			if (!pMat->TexAlbedo.empty()) {
				Filepath.Set(pMat->TexAlbedo.c_str());
				pScene->mMaterials[i]->AddProperty(&Filepath, AI_MATKEY_TEXTURE_AMBIENT(0));
				pScene->mMaterials[i]->AddProperty(&Filepath, AI_MATKEY_TEXTURE_DIFFUSE(0));
			}
			if (!pMat->TexNormal.empty()) {
				Filepath.Set(pMat->TexNormal.c_str());
				pScene->mMaterials[i]->AddProperty(&Filepath, AI_MATKEY_TEXTURE_NORMALS(0));
			}
			if (!pMat->TexDepth.empty()) {
				Filepath.Set(pMat->TexDepth.c_str());
				pScene->mMaterials[i]->AddProperty(&Filepath, AI_MATKEY_TEXTURE_DISPLACEMENT(0));
				pScene->mMaterials[i]->AddProperty(&Filepath, AI_MATKEY_TEXTURE_HEIGHT(0));
			}
		}//For[materials]

		// store skeleton
		if (pMesh->rootBone() != nullptr) {
			aiNode* pSkeletonRoot = new aiNode();
			pSkeletonRoot->mParent = pScene->mRootNode;
			pScene->mRootNode->addChildren(1, &pSkeletonRoot);
			writeBone(pScene->mRootNode->mChildren[pScene->mRootNode->mNumChildren-1], pMesh->rootBone());
			
		}
		
		pScene->mMetaData = new aiMetadata();
		pScene->mMetaData->Add<int32_t>("UpAxis", 1);
		pScene->mMetaData->Add<int32_t>("UpAxisSign", 1);
		pScene->mMetaData->Add<int32_t>("FrontAxis", 2);
		pScene->mMetaData->Add<int32_t>("FrontAxisSign", -1);
		pScene->mMetaData->Add<int32_t>("CoordAxis", 0);
		pScene->mMetaData->Add<int32_t>("CoordAxisSign", -1);
		pScene->mMetaData->Add<int32_t>("OriginalUpAxis", -1);
		pScene->mMetaData->Add<int32_t>("OriginalUpAxisSign", -1);
		pScene->mMetaData->Add<double>("UnitScaleFactor", 1.00);
		pScene->mMetaData->Add<int32_t>("FrameRate", 11);


	}//T3DMeshToAiScene

	void AssimpMeshIO::writeBone(aiNode* pNode, const T3DMesh<float>::Bone* pBone) {

		pNode->mName = pBone->Name.c_str();
		pNode->mTransformation.Translation(toAiVector(pBone->Position), pNode->mTransformation);
		
		if (pBone->Children.size() > 0) {
			pNode->mNumChildren = pBone->Children.size();
			pNode->mChildren = new aiNode*[pBone->Children.size()];
			for (uint32_t i = 0; i < pBone->Children.size(); ++i) {
				pNode->mChildren[i] = new aiNode();
				pNode->mChildren[i]->mParent = pNode;
				writeBone(pNode->mChildren[i], pBone->Children[i]);
			}
		}

	}//writeSkeleton



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

	aiVector3D AssimpMeshIO::toAiVector(const Eigen::Vector3f Vec)const {
		aiVector3D Rval;
		Rval.x = Vec.x();
		Rval.y = Vec.y();
		Rval.z = Vec.z();
		return Rval;
	}//toAiVector

	aiMatrix4x4 AssimpMeshIO::toAiMatrix(const Eigen::Matrix4f Mat)const {
		aiMatrix4x4 Rval;
		Rval.a1 = Mat(0, 0);
		Rval.a2 = Mat(0, 1);
		Rval.a3 = Mat(0, 2);
		Rval.a4 = Mat(0, 3);

		Rval.b1 = Mat(1, 0);
		Rval.b2 = Mat(1, 1);
		Rval.b3 = Mat(1, 2);
		Rval.b4 = Mat(1, 3);

		Rval.c1 = Mat(2, 0);
		Rval.c2 = Mat(2, 1);
		Rval.c3 = Mat(2, 2);
		Rval.c4 = Mat(2, 3);

		Rval.d1 = Mat(3, 0);
		Rval.d2 = Mat(3, 1);
		Rval.d3 = Mat(3, 2);
		Rval.d4 = Mat(3, 3);

		return Rval;

	}//toAiMatrix

	aiQuaternion AssimpMeshIO::toAiQuat(const Eigen::Quaternionf Q)const {
		aiQuaternion Rval;
		Rval.x = Q.x();
		Rval.y = Q.y();
		Rval.z = Q.z();
		Rval.w = Q.w();
		return Rval;
	}//toAiQuat

}//name space