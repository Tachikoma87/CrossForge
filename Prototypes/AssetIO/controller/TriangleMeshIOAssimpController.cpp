
#include <filesystem>
#include <assimp/postprocess.h>
#include <crossforge/utility/GeneralUtility.hpp>

#include "../../utility/FileUtility.h"
#include "TriangleMeshIOAssimpController.h"

#include "../components/TriangleMesh/MeshDefinition.h"
#include "../components/TriangleMesh/MeshMaterial.h"
#include "../Components/MaterialDataComponent.h"


using namespace Assimp;

namespace crossforge {

	TriangleMeshIOAssimpController::TriangleMeshIOAssimpController(): TriangleMeshIOControllerBase(TriangleMeshIOAssimpController::identification) {

	}

	TriangleMeshIOAssimpController::~TriangleMeshIOAssimpController() {

	}

	TriangleMeshIOAssimpController::TriangleMeshIOAssimpController(const std::string childIdentification) : TriangleMeshIOControllerBase(TriangleMeshIOAssimpController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	



	// overriding methods from parent class
	bool TriangleMeshIOAssimpController::canAcceptFile(const std::string filePath, const Operation operation) const {
		std::filesystem::path path = filePath;
		bool result = false;

		std::string extension = GeneralUtility::toLowerCase(path.extension().string());

		switch (operation) {
		case OP_LOAD:
			if (0 == extension.compare(".fbx")) result = true;
			else if (0 == extension.compare(".obj")) result = true;
			else if (0 == extension.compare(".ply")) result = true;
			else if (0 == extension.compare(".gltf")) result = true;
			else if (0 == extension.compare(".stl")) result = true;
			else if (0 == extension.compare(".glb")) result = true;
			break;

		case OP_STORE:
			if (0 == extension.compare(".fbx")) result = true;
			else if (0 == extension.compare(".obj")) result = true;
			else if (0 == extension.compare(".ply")) result = true;
			else if (0 == extension.compare(".stl")) result = true;
			else if (0 == extension.compare(".x")) result = true;
			break;
		default: break;
		}
	
		return result;	
	}


	bool TriangleMeshIOAssimpController::load(TriangleMeshEntityPtr pEntity, const std::string filepath) {
		const aiScene* pScene = m_Importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure);

		if (nullptr == pScene) {
			std::string ErrorMsg = m_Importer.GetErrorString();
			throw CrossForgeExcept("Failed to load model from resource " + filepath + "\n\t" + ErrorMsg);
		}

		bool result = false;

		try {
			aiSceneToTriangleMeshEntity(pScene, pEntity, FileUtility::removeFilename(filepath));
			result = true;
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);	
		}

		m_Importer.FreeScene();
		return result;
	}//load

	bool TriangleMeshIOAssimpController::store(TriangleMeshEntityPtr pEntity, const std::string Filepath) {
		if (Filepath.empty()) throw CrossForgeExcept("Empty filepath specified!");
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");

		std::string Str = GeneralUtility::toLowerCase(Filepath);

		std::string fileType = "";
		if (Str.find(".fbx") != std::string::npos) fileType = "fbx";
		else if (Str.find(".obj") != std::string::npos) fileType = "obj";
		else if (Str.find(".ply") != std::string::npos) fileType = "ply";
		else if (Str.find(".stl") != std::string::npos) fileType = "stl";
		else if (Str.find(".x") != std::string::npos) fileType = "x";

		// convert mesh to aiScene
		aiScene scene;
		triangleMeshEntityToAiScene(pEntity, &scene);
		return (AI_SUCCESS == m_Exporter.Export(&scene, fileType.c_str(), Filepath.c_str()));
	}//store

	

	void TriangleMeshIOAssimpController::aiSceneToTriangleMeshEntity(const aiScene* pScene, TriangleMeshEntityPtr pEntity, const std::string Directory) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (nullptr == pScene) throw NullpointerExcept("pScene");


		std::vector<Eigen::Vector3f> positions;
		std::vector<Eigen::Vector3f> normals;
		std::vector<Eigen::Vector3f> tangents;
		std::vector<Eigen::Vector3f> textureCoordinates;

		std::vector<MeshDefinitionPtr> meshDefinitions;

		//std::vector<T3DMesh<float>::Bone*> Bones;
		//std::vector<T3DMesh<float>::SkeletalAnimation*> SekeltonAnimations;

		if (nullptr == pEntity->getMeshDefinitionsComponent()) pEntity->addComponent(std::make_shared<MeshDefinitionsComponent>());
		else pEntity->getMeshDefinitionsComponent()->clear();

		uint32_t positionsOffset = 0;
		uint32_t normalsOffset = 0;
		uint32_t tangentsOffset = 0;
		uint32_t textureCoordOffset = 0;

		uint32_t indexOffset = 0;

		for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
			aiMesh* pM = pScene->mMeshes[i];

			for (uint32_t k = 0; k < pM->mNumVertices; ++k) {
				// collect vertices
				positions.push_back(toEigenVec(pM->mVertices[k]));
				// collect normals
				if (pM->mNormals != nullptr) normals.push_back(toEigenVec(pM->mNormals[k]));
				// collect tangents
				if (pM->mTangents != nullptr) tangents.push_back(toEigenVec(pM->mTangents[k]));
				// collect texture coordinates
				if (pM->mTextureCoords[0] != nullptr && pM->GetNumUVChannels() > 0) textureCoordinates.push_back(toEigenVec(pM->mTextureCoords[0][k]));
			}

			// now we retrieve the faces (create submesh)
			MeshDefinitionPtr pSubmesh = std::make_shared<MeshDefinition>();
			pSubmesh->setMaterialIndex((int32_t)pM->mMaterialIndex);
			for (uint32_t k = 0; k < pM->mNumFaces; ++k) {
				aiFace F = pM->mFaces[k];
				Eigen::Vector3i face;
				for (uint32_t j = 0; j < F.mNumIndices && j < 3; ++j) face[j] = positionsOffset + F.mIndices[j];
				pSubmesh->getFaces().push_back(face);
			}
			pEntity->getMeshDefinitionsComponent()->addMeshDefinition(pSubmesh);

			

			// add bones
			//for (uint32_t k = 0; k < pM->mNumBones; k++) {

			//	T3DMesh<float>::Bone* pBone = new T3DMesh<float>::Bone();
			//	pBone->ID = Bones.size();
			//	pBone->InvBindPoseMatrix = toEigenMat(pM->mBones[k]->mOffsetMatrix);
			//	pBone->Name = pM->mBones[k]->mName.C_Str();

			//	for (uint32_t j = 0; j < pM->mBones[k]->mNumWeights; ++j) {
			//		pBone->VertexInfluences.push_back(positionsOffset + pM->mBones[k]->mWeights[j].mVertexId);
			//		pBone->VertexWeights.push_back(pM->mBones[k]->mWeights[j].mWeight);
			//	}

			//	Bones.push_back(pBone);
			//}//for[bones]

			positionsOffset += pM->mNumVertices;
			if (nullptr != pM->mNormals) normalsOffset += pM->mNumVertices;
			if (nullptr != pM->mTangents) tangentsOffset += pM->mNumVertices;
			if (nullptr != pM->mTextureCoords[0]) textureCoordOffset += pM->mNumVertices;
		}//for[all meshes]


		// apply global transformation
		Eigen::Matrix4f GlobalTransform = toEigenMat(pScene->mRootNode->mTransformation);
		for (auto& i : positions) {
			Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 1.0f);
			i = Eigen::Vector3f(p.x(), p.y(), p.z());
		}
		if (normals.size() > 0) {
			for (auto& i : normals) {
				Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 0.0f);
				i = Eigen::Vector3f(p.x(), p.y(), p.y());
			}
		}
		if (tangents.size() > 0) {
			for (auto& i : tangents) {
				Eigen::Vector4f p = GlobalTransform * Eigen::Vector4f(i.x(), i.y(), i.z(), 0.0f);
				i = Eigen::Vector3f(p.x(), p.y(), p.z());
			}
		}

		// set positions, normals, tangents
		if (!pEntity->hasComponent(PositionDataComponent::identification)) pEntity->addComponent(std::make_shared<PositionDataComponent>());
		pEntity->getPositionDataComponent()->setPositions(positions);

		if (normals.size() > 0) {
			if (!pEntity->hasComponent(NormalDataComponent::identification)) pEntity->addComponent(std::make_shared<NormalDataComponent>());
			pEntity->getNormalDataComponent()->setNormals(normals);
		}
		if (textureCoordinates.size() > 0) {
			if (!pEntity->hasComponent(TextureCoordinateComponent::identification)) pEntity->addComponent(std::make_shared<TextureCoordinateComponent>());
			pEntity->getTextureCoordinatesComponent()->setTextureCoordinates(textureCoordinates);
		}


		//add materials
		for (uint32_t i = 0; i < pScene->mNumMaterials; ++i) {

			aiMaterial* pMat = pScene->mMaterials[i];
			aiString filepath;

			//T3DMesh<float>::Material Mat;
			MeshMaterialPtr pMeshMat = std::make_shared<MeshMaterial>();

			std::string texAmbientFilepath = "";
			std::string texDiffuseFilepath = "";
			std::string texNormalFilepath = "";
			std::string texHeightFilepath = "";

			if (pMat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
				pMat->GetTexture(aiTextureType_AMBIENT, 0, &filepath);
				if (FileUtility::exists(filepath.C_Str())) texAmbientFilepath = std::string(filepath.C_Str());
				else texAmbientFilepath = FileUtility::absolute(Directory + filepath.C_Str());
			}
			if (pMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				pMat->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);
				
				if (FileUtility::exists(filepath.C_Str())) texDiffuseFilepath = std::string(filepath.C_Str());
				else texDiffuseFilepath = FileUtility::absolute(Directory + filepath.C_Str());
			}
			if (pMat->GetTextureCount(aiTextureType_NORMALS) > 0) {
				pMat->GetTexture(aiTextureType_NORMALS, 0, &filepath);
				if (FileUtility::exists(filepath.C_Str())) texNormalFilepath = std::string(filepath.C_Str());
				else texNormalFilepath = FileUtility::absolute(Directory + filepath.C_Str());
			}

			if (pMat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
				pMat->GetTexture(aiTextureType_HEIGHT, 0, &filepath);
				if (FileUtility::exists(filepath.C_Str())) texHeightFilepath = std::string(filepath.C_Str());
				else texHeightFilepath = FileUtility::absolute(Directory + filepath.C_Str());
			}

			pMeshMat->setTexturePath(texAmbientFilepath, MeshMaterial::TEXTURE_TYPE_AMBIENT);
			pMeshMat->setTexturePath(texDiffuseFilepath, MeshMaterial::TEXTURE_TYPE_DIFFUSE);
			pMeshMat->setTexturePath(texNormalFilepath, MeshMaterial::TEXTURE_TYPE_NORMAL);
			pMeshMat->setTexturePath(texHeightFilepath, MeshMaterial::TEXTURE_TYPE_HEIGHT);


			ai_real buffer[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	
			if (AI_SUCCESS == pMat->Get(AI_MATKEY_COLOR_AMBIENT, (ai_real*)buffer, nullptr)) {
				pMeshMat->setColor(Eigen::Vector4f(buffer[0], buffer[1], buffer[2], buffer[3]), MeshMaterial::COLOR_TYPE_AMBIENT);
			}
			if (AI_SUCCESS == pMat->Get(AI_MATKEY_COLOR_DIFFUSE, (ai_real*)buffer, nullptr)) {
				pMeshMat->setColor(Eigen::Vector4f(buffer[0], buffer[1], buffer[2], buffer[3]), MeshMaterial::COLOR_TYPE_DIFFUSE);
			}
			if (AI_SUCCESS == pMat->Get(AI_MATKEY_COLOR_EMISSIVE, (ai_real*)buffer, nullptr)) {
				pMeshMat->setColor(Eigen::Vector4f(buffer[0], buffer[1], buffer[2], buffer[3]), MeshMaterial::COLOR_TYPE_EMISSIVE);

			}
	
			if (!pEntity->hasComponent(MaterialDataComponent::identification)) pEntity->addComponent(std::make_shared<MaterialDataComponent>());
			auto pMaterialComponent = pEntity->getComponent<MaterialDataComponent>();
			pMaterialComponent->addMaterial(pMeshMat);

		
		}//for[all materials]

		// join identical bones (if names are identical)
		//for (auto i : Bones) {
		//	if (nullptr == i) continue;
		//	for (uint32_t k = i->ID + 1; k < Bones.size(); ++k) {
		//		if (Bones[k] == nullptr) continue;
		//		if (i->Name.compare(Bones[k]->Name) == 0) {
		//			// copy vertx influces
		//			for (auto l : Bones[k]->VertexInfluences) i->VertexInfluences.push_back(l);
		//			for (auto l : Bones[k]->VertexWeights) i->VertexWeights.push_back(l);
		//			delete Bones[k];
		//			Bones[k] = nullptr;
		//		}
		//	}//for[following bones]
		//}//for[all bones]

		//std::vector<T3DMesh<float>::Bone*> Bones2;
		//for (auto i : Bones) {
		//	if (i != nullptr) Bones2.push_back(i);
		//}
		//Bones = Bones2;

		//for (uint32_t i = 0; i < Bones.size(); ++i) Bones[i]->ID = i;

		//// skeleton
		//aiNode* pRoot = pScene->mRootNode;
		//retrieveBoneHierarchy(pRoot, &Bones);

		//// find root bone (the one without parent)
		//T3DMesh<float>::Bone* pRootBone = nullptr;
		//for (auto i : Bones) {
		//	if (i->pParent == nullptr) pRootBone = i;
		//}//for[all bones]


		//std::vector<T3DMesh<float>::SkeletalAnimation*> BoneAnimations;

		//// retrieve animation data
		//for (uint32_t i = 0; i < pScene->mNumAnimations; ++i) {
		//	aiAnimation* pAnim = pScene->mAnimations[i];

		//	// create new animation
		//	T3DMesh<float>::SkeletalAnimation* pSkelAnim = new T3DMesh<float>::SkeletalAnimation();
		//	BoneAnimations.push_back(pSkelAnim);

		//	pSkelAnim->Duration = pAnim->mDuration;
		//	pSkelAnim->SamplesPerSecond = pAnim->mTicksPerSecond;
		//	pSkelAnim->Name = pAnim->mName.C_Str();

		//	// create keyframe for every bone
		//	for (uint32_t k = 0; k < std::max((uint32_t)Bones.size(), pAnim->mNumChannels); k++) {
		//		pSkelAnim->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
		//		pSkelAnim->Keyframes[k]->ID = k;
		//	}//for[all bones]

		//	for (uint32_t k = 0; k < pAnim->mNumChannels; ++k) {
		//		aiNodeAnim* pNodeAnim = pAnim->mChannels[k];

		//		T3DMesh<float>::Bone* pB = getBoneFromName(pNodeAnim->mNodeName.C_Str(), &Bones);


		//		int32_t KeyID = (nullptr == pB) ? k : pB->ID;

		//		pSkelAnim->Keyframes[KeyID]->BoneID = (nullptr == pB) ? -1 : pB->ID;
		//		pSkelAnim->Keyframes[KeyID]->BoneName = pNodeAnim->mNodeName.C_Str();

		//		T3DMesh<float>::BoneKeyframes* pKeys = (pB == nullptr) ? pSkelAnim->Keyframes[k] : pSkelAnim->Keyframes[pB->ID];

		//		for (uint32_t l = 0; l < pNodeAnim->mNumPositionKeys; l++) {
		//			pKeys->Positions.push_back(toEigenVec(pNodeAnim->mPositionKeys[l].mValue));
		//			pKeys->Timestamps.push_back(pNodeAnim->mPositionKeys[l].mTime);
		//		}//for[positions]

		//		for (uint32_t l = 0; l < pNodeAnim->mNumRotationKeys; l++) {
		//			pKeys->Rotations.push_back(toEigenQuat(pNodeAnim->mRotationKeys[l].mValue));
		//		}//for[rotations]

		//		for (uint32_t l = 0; l < pNodeAnim->mNumScalingKeys; l++) {
		//			pKeys->Scalings.push_back(toEigenVec(pNodeAnim->mScalingKeys[l].mValue));
		//		}

		//	}//for[channels]

		//}//for[all animations]

		//if (Bones.size() > 0) {
		//	// set skeleton
		//	// the mesh structure can have the allocated memory
		//	pMesh->bones(&Bones, false);
		//	Bones.clear();
		//}

		//for (auto i : BoneAnimations) pMesh->addSkeletalAnimation(i, false);

	}//aiMeshTo3DMesh

	void TriangleMeshIOAssimpController::triangleMeshEntityToAiScene(TriangleMeshEntityPtr pEntity, aiScene* pScene) {
		if (nullptr == pEntity) throw NullpointerExcept("pMesh");
		if (nullptr == pScene) throw NullpointerExcept("pScene");

		pScene->mRootNode = new aiNode();
		pScene->mRootNode->mName = "root";

		// add meshes

		pScene->mNumMeshes = pEntity->getMeshDefinitionsComponent()->getMeshDefinitionsCount();
		pScene->mRootNode->mNumMeshes = pScene->mNumMeshes;
		pScene->mMeshes = new aiMesh * [pScene->mNumMeshes];
		pScene->mRootNode->mMeshes = new unsigned int[pScene->mNumMeshes];
		for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
			pScene->mMeshes[i] = new aiMesh();
			pScene->mRootNode->mMeshes[i] = i;
		}

		for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
			std::vector<int32_t> VertexIDs;

			aiMesh* pM = pScene->mMeshes[i];

			// set faces
			auto pSub = pEntity->getMeshDefinitionsComponent()->getMeshDefinition(i);

			pM->mMaterialIndex = pSub->getMaterialindex();
			auto faceList = pSub->getFaces();

			pM->mFaces = new aiFace[faceList.size()];
			for (uint32_t k = 0; k < faceList.size(); ++k) {
				aiFace F;
				F.mNumIndices = 3;
				F.mIndices = new unsigned int[3];
				F.mIndices[0] = faceList[k][0]; 
				F.mIndices[1] = faceList[k][1];  
				F.mIndices[2] = faceList[k][2];
				pM->mFaces[k] = F;

				// collect vertex ids
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), faceList[k][0])) VertexIDs.push_back(faceList[k][0]);
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), faceList[k][1])) VertexIDs.push_back(faceList[k][1]);
				if (VertexIDs.end() == std::find(VertexIDs.begin(), VertexIDs.end(), faceList[k][2])) VertexIDs.push_back(faceList[k][2]);

			}//for[faces]
			pM->mNumFaces = faceList.size();

			uint32_t MaxID = 0;
			for (auto k : VertexIDs) {
				if (k > MaxID) MaxID = k;
			}

			std::vector<int32_t> VertexMap;
			VertexMap.reserve(MaxID);
			for (uint32_t k = 0; k <= MaxID; ++k) VertexMap.push_back(-1);

			pM->mNumVertices = VertexIDs.size();
			pM->mVertices = new aiVector3D[VertexIDs.size()];


			std::vector<Eigen::Vector3f> vertexList = pEntity->getPositionDataComponent()->getPositions();
			std::vector<Eigen::Vector3f> normalList = (pEntity->hasComponent(NormalDataComponent::identification)) ? pEntity->getNormalDataComponent()->getNormals() : std::vector<Eigen::Vector3f>();
			std::vector<Eigen::Vector3f> texCoordsList = (pEntity->hasComponent(TextureCoordinateComponent::identification)) ? pEntity->getTextureCoordinatesComponent()->getTextureCoordinates() : std::vector<Eigen::Vector3f>();

			for (uint32_t k = 0; k < VertexIDs.size(); ++k) {
				pM->mVertices[k] = toAiVector(vertexList.at(VertexIDs[k]));
				VertexMap[VertexIDs[k]] = k;
			}

			if (normalList.size() > 0) {
				pM->mNormals = new aiVector3D[VertexIDs.size()];
				for (uint32_t k = 0; k < VertexIDs.size(); k++) pM->mNormals[k] = toAiVector(normalList.at(VertexIDs[k]));
			}
			if (texCoordsList.size() > 0) {
				pM->mTextureCoords[0] = new aiVector3D[VertexIDs.size()];
				for (uint32_t k = 0; k < VertexIDs.size(); ++k) pM->mTextureCoords[0][k] = toAiVector(texCoordsList.at(VertexIDs[k]));
			}

			// Map vertices
			for (uint32_t k = 0; k < faceList.size(); ++k) {
				pM->mFaces[k].mIndices[0] = (unsigned int)VertexMap[pM->mFaces[k].mIndices[0]];
				pM->mFaces[k].mIndices[1] = (unsigned int)VertexMap[pM->mFaces[k].mIndices[1]];
				pM->mFaces[k].mIndices[2] = (unsigned int)VertexMap[pM->mFaces[k].mIndices[2]];
			}

			//std::vector<aiBone*> Bones;
			//// gather influences
			//for (uint32_t k = 0; k < pMesh->boneCount(); ++k) {
			//	auto* pBone = pMesh->getBone(k);

			//	std::vector<int32_t> InfluenceIDs;
			//	std::vector<float> Weights;

			//	for (uint32_t j = 0; j < pBone->VertexInfluences.size(); ++j) {
			//		int32_t Index = pBone->VertexInfluences[j];
			//		if (Index < VertexMap.size() && VertexMap[Index] != -1) {
			//			InfluenceIDs.push_back(VertexMap[Index]);
			//			Weights.push_back(pBone->VertexWeights[j]);
			//		}
			//	}//for[influences]

			//	if (InfluenceIDs.size() > 0) {
			//		aiBone* pB = new aiBone();
			//		pB->mName = pBone->Name.c_str();
			//		pB->mOffsetMatrix = toAiMatrix(pBone->InvBindPoseMatrix);
			//		pB->mNumWeights = InfluenceIDs.size();
			//		pB->mWeights = new aiVertexWeight[pB->mNumWeights];
			//		for (uint32_t j = 0; j < InfluenceIDs.size(); ++j) {
			//			pB->mWeights[j].mVertexId = InfluenceIDs[j];
			//			pB->mWeights[j].mWeight = Weights[j];
			//		}
			//		Bones.push_back(pB);
			//	}//if[valid bone]
			//}//for[all bones]

			//if (Bones.size() > 0) {
			//	// store bones of this mesh
			//	pM->mBones = new aiBone * [Bones.size()];
			//	pM->mNumBones = Bones.size();
			//	for (uint32_t k = 0; k < Bones.size(); ++k) pM->mBones[k] = Bones[k];
			//	Bones.clear();
			//}


		}//for[subMeshes]

		//// store materials
		auto pMatComponent = pEntity->getComponent<MaterialDataComponent>();
		if (nullptr != pMatComponent && pMatComponent->getMaterialCount() > 0) {
			pScene->mNumMaterials = pMatComponent->getMaterialCount();
			pScene->mMaterials = new aiMaterial * [pScene->mNumMaterials];

			for (uint32_t i = 0; i < pScene->mNumMaterials; ++i) {
				pScene->mMaterials[i] = new aiMaterial();

				auto pMeshMaterial = pMatComponent->getMaterial(i);

				aiString MatName = aiString(("Mat-" + std::to_string(i)).c_str());

				pScene->mMaterials[i]->AddProperty(&MatName, AI_MATKEY_NAME);

				aiColor4D ambient = toAiColor(pMeshMaterial->getColor(MeshMaterial::COLOR_TYPE_AMBIENT));
				aiColor4D diffuse = toAiColor(pMeshMaterial->getColor(MeshMaterial::COLOR_TYPE_DIFFUSE));

				pScene->mMaterials[i]->AddProperty(&ambient, 1, AI_MATKEY_COLOR_AMBIENT);
				pScene->mMaterials[i]->AddProperty(&diffuse, 1, AI_MATKEY_COLOR_DIFFUSE);

				aiString filepath;
				if (!pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_AMBIENT).empty()) {
					filepath.Set(pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_AMBIENT).c_str());
					pScene->mMaterials[i]->AddProperty(&filepath, AI_MATKEY_TEXTURE_AMBIENT(0));
				}
				if (!pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_DIFFUSE).empty()) {
					filepath.Set(pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_DIFFUSE).c_str());
					pScene->mMaterials[i]->AddProperty(&filepath, AI_MATKEY_TEXTURE_DIFFUSE(0));
				}
				if (!pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_NORMAL).empty()) {
					filepath.Set(pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_NORMAL).c_str());
					pScene->mMaterials[i]->AddProperty(&filepath, AI_MATKEY_TEXTURE_NORMALS(0));
				}
				if (!pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_HEIGHT).empty()) {
					filepath.Set(pMeshMaterial->getTexturePath(MeshMaterial::TEXTURE_TYPE_HEIGHT).c_str());
					pScene->mMaterials[i]->AddProperty(&filepath, AI_MATKEY_TEXTURE_HEIGHT(0));
				}

			}//For[materials]
		}

		//// store skeleton
		//if (pMesh->rootBone() != nullptr) {
		//	aiNode* pSkeletonRoot = new aiNode();
		//	pSkeletonRoot->mParent = pScene->mRootNode;
		//	pScene->mRootNode->addChildren(1, &pSkeletonRoot);
		//	writeBone(pScene->mRootNode->mChildren[pScene->mRootNode->mNumChildren - 1], pMesh->rootBone());

		//}

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


	}

	//void AssimpMeshIO::writeBone(aiNode* pNode, const T3DMesh<float>::Bone* pBone) {

	//	pNode->mName = pBone->Name.c_str();
	//	pNode->mTransformation.Translation(toAiVector(pBone->Position), pNode->mTransformation);

	//	if (pBone->Children.size() > 0) {
	//		pNode->mNumChildren = pBone->Children.size();
	//		pNode->mChildren = new aiNode * [pBone->Children.size()];
	//		for (uint32_t i = 0; i < pBone->Children.size(); ++i) {
	//			pNode->mChildren[i] = new aiNode();
	//			pNode->mChildren[i]->mParent = pNode;
	//			writeBone(pNode->mChildren[i], pBone->Children[i]);
	//		}
	//	}

	//}//writeSkeleton


	//void AssimpMeshIOSystem::retrieveBoneHierarchy(aiNode* pNode, std::vector<T3DMesh<float>::Bone*>* pBones) {
	//	if (nullptr == pNode) return; // end of recursion
	//	if (nullptr == pBones) throw NullpointerExcept("pBones");

	//	T3DMesh<float>::Bone* pCurrentBone = getBoneFromName(pNode->mName.C_Str(), pBones);
	//	if (nullptr != pCurrentBone) {
	//		// retrieve parent
	//		if (nullptr != pNode->mParent) pCurrentBone->pParent = getBoneFromName(pNode->mParent->mName.C_Str(), pBones);
	//		// add children
	//		for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
	//			T3DMesh<float>::Bone* pChild = getBoneFromName(pNode->mChildren[i]->mName.C_Str(), pBones);
	//			if (nullptr != pChild) pCurrentBone->Children.push_back(pChild);
	//		}//for[all child nodes]	
	//	}//if[current bone was found]

	//	// recursion
	//	for (uint32_t i = 0; i < pNode->mNumChildren; ++i) {
	//		retrieveBoneHierarchy(pNode->mChildren[i], pBones);
	//	}

	//}//retriveBoneHierarchy

	//T3DMesh<float>::Bone* AssimpMeshIOSystem::getBoneFromName(std::string Name, std::vector<T3DMesh<float>::Bone*>* pBones) {
	//	T3DMesh<float>::Bone* pRval = nullptr;

	//	for (auto i : (*pBones)) {
	//		if (0 == i->Name.compare(Name)) {
	//			pRval = i;
	//			break;
	//		}
	//	}//for[all bones]

	//	return pRval;
	//}//

	Eigen::Vector3f TriangleMeshIOAssimpController::toEigenVec(const aiVector3D Vec)const {
		return Eigen::Vector3f(Vec.x, Vec.y, Vec.z);
	}//toEigenVec

	Eigen::Matrix4f TriangleMeshIOAssimpController::toEigenMat(const aiMatrix4x4 Mat)const {
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

	Eigen::Quaternionf TriangleMeshIOAssimpController::toEigenQuat(const aiQuaternion Q)const {
		Eigen::Quaternionf Rval;
		Rval.x() = Q.x;
		Rval.y() = Q.y;
		Rval.z() = Q.z;
		Rval.w() = Q.w;
		return Rval;
	}//toEigenQuat

	aiVector3D TriangleMeshIOAssimpController::toAiVector(const Eigen::Vector3f Vec)const {
		aiVector3D Rval;
		Rval.x = Vec.x();
		Rval.y = Vec.y();
		Rval.z = Vec.z();
		return Rval;
	}//toAiVector

	aiMatrix4x4 TriangleMeshIOAssimpController::toAiMatrix(const Eigen::Matrix4f Mat)const {
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

	aiQuaternion TriangleMeshIOAssimpController::toAiQuat(const Eigen::Quaternionf Q)const {
		aiQuaternion Rval;
		Rval.x = Q.x();
		Rval.y = Q.y();
		Rval.z = Q.z();
		Rval.w = Q.w();
		return Rval;
	}//toAiQuat

	aiColor4D TriangleMeshIOAssimpController::toAiColor(const Eigen::Vector4f color)const {
		aiColor4D result;
		result.r = color.x();
		result.g = color.y();
		result.b = color.z();
		result.a = color.w();
		return result;
	}

}