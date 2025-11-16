#include <glad/glad.h>
#include "ActorPrefabEntityController.h"

#include <crossforge/assetio/entities/TriangleMeshEntity.h>
#include "../../utility/GraphicsUtility.h"

#include "../components/shader/RenderingShaderComponent.h"

#include "../provider/STextureProvider.h"
#include "../provider/SShaderProvider.h"

namespace crossforge {

	ActorPrefabEntityController::ActorPrefabEntityController() : ControllerBase(ActorPrefabEntityController::identification) {

	}
	ActorPrefabEntityController::~ActorPrefabEntityController() {

	}
	ActorPrefabEntityController::ActorPrefabEntityController(const std::string childIdentification) : ControllerBase(ActorPrefabEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}

	bool ActorPrefabEntityController::buildStaticActor(ActorPrefabEntityPtr pActorPrefab, TriangleMeshEntityPtr pTriangleMeshEntity) {
		if (nullptr == pActorPrefab) throw NullpointerExcept("pActorEntity");
		if (nullptr == pTriangleMeshEntity) throw NullpointerExcept("pTriangleMeshEntity");


		buildVertexBuffer(pActorPrefab, pTriangleMeshEntity);
		buildIndexBuffer(pActorPrefab, pTriangleMeshEntity);
		buildRenderGroups(pActorPrefab, pTriangleMeshEntity);

		// generate the vertex array
		uint32_t vertexArray = GL_INVALID_INDEX;
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		setVertexAttributePointer(pActorPrefab);
		glBindVertexArray(0);

		if (!pActorPrefab->hasComponent(VertexArrayComponent::identification)) pActorPrefab->addComponent(std::make_shared<VertexArrayComponent>());
		pActorPrefab->getVertexArrayComponent()->glVertexArrayHandle() = vertexArray;

		// generate materials
		buildPbrMaterials(pActorPrefab, pTriangleMeshEntity);
		// set properties
		if (!pActorPrefab->hasComponent(ActorPrefabPropertiesComponent::identification)) pActorPrefab->addComponent(std::make_shared<ActorPrefabPropertiesComponent>());
		auto pProperties = pActorPrefab->getActorPrefabPropertiesComponent();
		/// @ToOd No usefully features available yet


		std::string glErrorLog = "";
		uint32_t error = GraphicsUtility::checkGLError(&glErrorLog);
		if (GL_NO_ERROR != error) {
			LogError("An OpenGL error occurred during building the static actor: " + glErrorLog);
		}
		return (GL_NO_ERROR == error);
	}

	bool ActorPrefabEntityController::buildVertexBuffer(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pMeshEntity){
		if (nullptr == pMeshEntity) throw NullpointerExcept("pMeshEntity");

		std::vector<uint8_t> vertexBuffer;

		uint16_t attributeMask = 0;
		uint32_t vertexSize = 0;
		uint64_t bufferSize = 0;
		PositionsComponentPtr pPositionData = nullptr;
		NormalsComponentPtr pNormalData = nullptr;
		TextureCoordinatesComponentPtr pUvwData = nullptr;

		uint32_t positionOffset = 0;
		uint32_t normalOffset = 0;
		uint32_t uvwOffset = 0;
		

		if (pMeshEntity->hasComponent(PositionsComponent::identification)) {
			attributeMask |= VertexBufferComponent::ATTRIBUTE_POSITION;
			positionOffset = vertexSize;
			vertexSize += sizeof(float) * 3;
			pPositionData = pMeshEntity->getPositionsComponent();
		}
		if (pMeshEntity->hasComponent(NormalsComponent::identification)) {
			attributeMask |= VertexBufferComponent::ATTRIBUTE_NORMAL;
			normalOffset = vertexSize;
			vertexSize += sizeof(float) * 3;
			pNormalData = pMeshEntity->getNormalsComponent();
		}
		if (pMeshEntity->hasComponent(TextureCoordinatesComponent::identification)) {
			attributeMask |= VertexBufferComponent::ATTRIBUTE_UVW;
			uvwOffset = vertexSize;
			vertexSize += sizeof(float) * 3;
			pUvwData = pMeshEntity->getTextureCoordinatesComponent();
		}

		bufferSize = vertexSize * pPositionData->getPositionCount();
		vertexBuffer.resize(bufferSize);

		// set position data
		if (attributeMask & VertexBufferComponent::ATTRIBUTE_POSITION) {
			for (uint32_t i = 0; i < pPositionData->getPositionCount(); ++i) {
				float* pPos = (float*)&(vertexBuffer.data()[i * vertexSize + positionOffset]);
				Eigen::Vector3f pos = pPositionData->getPosition(i);
				pPos[0] = pos.x();
				pPos[1] = pos.y();
				pPos[2] = pos.z();
			}
		}

		// set normal data
		if (attributeMask & VertexBufferComponent::ATTRIBUTE_NORMAL) {
			for (uint32_t i = 0; i < pNormalData->getNormalCount(); ++i) {
				float* pNormal = (float*)&(vertexBuffer.data()[i * vertexSize + normalOffset]);
				Eigen::Vector3f normal = pNormalData->getNormal(i);
				pNormal[0] = normal.x();
				pNormal[1] = normal.y();
				pNormal[2] = normal.z();
			}
		}

		// set uvw data
		if (attributeMask & VertexBufferComponent::ATTRIBUTE_UVW) {
			for (uint32_t i = 0; i < pUvwData->getTextureCoorindatesCount(); ++i) {
				float* pUvw = (float*)&(vertexBuffer.data()[i * vertexSize + uvwOffset]);
				Eigen::Vector3f uvw = pUvwData->getTextureCoordinate(i);
				pUvw[0] = uvw.x();
				pUvw[1] = uvw.y();
				pUvw[2] = uvw.z();
			}
		}

		// fill vertex buffer component
		if (!pActorEntity->hasComponent(VertexBufferComponent::identification)) pActorEntity->addComponent(std::make_shared<VertexBufferComponent>());
		auto pVertexBufferComp = pActorEntity->getVertexBufferComponent();

		pVertexBufferComp->attributeMask() = attributeMask;
		pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_POSITION) = positionOffset;
		pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_NORMAL) = normalOffset;
		pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_UVW) = uvwOffset;
		pVertexBufferComp->vertexCount() = pPositionData->getPositionCount();
		pVertexBufferComp->vertexSize() = vertexSize;

		glGenBuffers(1, &pVertexBufferComp->glBufferHandle());
		glBindBuffer(GL_ARRAY_BUFFER, pVertexBufferComp->glBufferHandle());
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexBuffer.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::string errorText;
		if (GL_NO_ERROR != GraphicsUtility::checkGLError(&errorText)) {
			LogError("OpenGl error occurred during generation of vertex buffer: " + errorText);
		}

		return true;
	}

	bool ActorPrefabEntityController::buildIndexBuffer(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity) {
		auto pMeshDefinitions = pTriangleMeshEntity->getMeshDefinitionsComponent();
		if (nullptr == pMeshDefinitions) return false;

		std::vector<uint32_t> vertexIndexList;
		vertexIndexList.reserve(100000);

		for (uint32_t i = 0; i < pMeshDefinitions->getMeshDefinitionsCount(); ++i) {
			auto pMesh = pMeshDefinitions->getMeshDefinition(i);
			for (uint32_t k = 0; k < pMesh->getFaceCount(); ++k) {
				Eigen::Vector3i face = pMesh->getFace(k);
				vertexIndexList.push_back(face.x());
				vertexIndexList.push_back(face.y());
				vertexIndexList.push_back(face.z());
			}
		}
		if (vertexIndexList.size() < 3) return false;

		if (!pActorEntity->hasComponent(IndexBufferComponent::identification)) pActorEntity->addComponent(std::make_shared<IndexBufferComponent>());
		auto pIndexBufferComponent = pActorEntity->getIndexBufferComponent();


		glGenBuffers(1, &pIndexBufferComponent->glBufferHandle());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexBufferComponent->glBufferHandle());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndexList.size() * sizeof(uint32_t), vertexIndexList.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		pIndexBufferComponent->bufferSize() = vertexIndexList.size() * sizeof(uint32_t);
		pIndexBufferComponent->indexCount() = vertexIndexList.size();


		std::string errorText;
		if (GL_NO_ERROR != GraphicsUtility::checkGLError(&errorText)) {
			LogWarning("GL error occurred during builindg index buffer: " + errorText);
		}
		return true;

	}

	bool ActorPrefabEntityController::buildRenderGroups(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity) {
		auto pMeshDefinitions = pTriangleMeshEntity->getMeshDefinitionsComponent();
		if (nullptr == pMeshDefinitions) return false;

		std::vector<Eigen::Vector2i> renderGroups;
		std::vector<int32_t> renderGroupsMaterial;
		uint32_t currentIndex = 0;
		for (uint32_t i = 0; i < pMeshDefinitions->getMeshDefinitionsCount(); ++i) {
			uint32_t endIndex = currentIndex + pMeshDefinitions->getMeshDefinition(i)->getFaceCount() * 3;
			renderGroups.push_back(Eigen::Vector2i(currentIndex, endIndex));
			currentIndex = endIndex;

			renderGroupsMaterial.push_back(pMeshDefinitions->getMeshDefinition(i)->getMaterialindex());
		}

		if (!pActorEntity->hasComponent(RenderGroupsComponent::identification)) pActorEntity->addComponent(std::make_shared<RenderGroupsComponent>());
		auto pRenderGroupsComponent = pActorEntity->getRenderGroupsComponent();
		pRenderGroupsComponent->renderGroups() = renderGroups;
		pRenderGroupsComponent->renderGroupsMaterial() = renderGroupsMaterial;

		return true;

	}

	bool ActorPrefabEntityController::setVertexAttributePointer(ActorPrefabEntityPtr pActorEntity) {
		if (nullptr == pActorEntity) throw NullpointerExcept("pActorEntity");
		if (!pActorEntity->hasComponent(VertexBufferComponent::identification)) throw MissingComponentException(VertexBufferComponent::identification);
		if (!pActorEntity->hasComponent(IndexBufferComponent::identification)) throw MissingComponentException(IndexBufferComponent::identification);

		auto pVertexBufferComp = pActorEntity->getVertexBufferComponent();
		auto pIndexBufferComp = pActorEntity->getIndexBufferComponent();

		uint32_t vertexBufferHandle = pVertexBufferComp->glBufferHandle();
		uint32_t indexBufferHandle = pIndexBufferComp->glBufferHandle();

		if (!glIsBuffer(vertexBufferHandle)) {
			LogError("Given vertex buffer handle is not a valid OpenGL buffer.");
			return false;
		}
		if (!glIsBuffer(indexBufferHandle)) {
			LogError("Given index buffer handle is not  valid OpenGL buffer.");
			return false;
		}

		// bind array and index buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferHandle);
 
		//// position array
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_POSITION)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_POSITION);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_POSITION);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 3, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);
		}

		// normal array
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_NORMAL)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_NORMAL);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_NORMAL);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 3, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);

		}

		// tangent array
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_TANGENT)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_TANGENT);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_TANGENT);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 3, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);
		}

		// texture coordinates
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_UVW)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_UVW);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_UVW);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 3, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);

		}

		// vertex colors
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_COLOR)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_COLOR);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_COLOR);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 3, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);
		}

		// bone indices
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_BONE_INDICES)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_BONE_INDICES);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_BONE_INDICES);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribIPointer(attribArrayIndex, 4, GL_INT, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);
		}

		// bone weights
		if (pVertexBufferComp->hasAttribute(VertexBufferComponent::ATTRIBUTE_BONE_WEIGHTS)) {
			const int32_t attribArrayIndex = RenderingShaderComponent::getAttributeArrayIndex(RenderingShaderComponent::ATTRIB_ARRAY_BONE_WEIGHTS);
			const uint64_t attributeOffset = pVertexBufferComp->attributeOffset(VertexBufferComponent::ATTRIBUTE_BONE_WEIGHTS);
			glEnableVertexAttribArray(attribArrayIndex);
			glVertexAttribPointer(attribArrayIndex, 4, GL_FLOAT, GL_FALSE, pVertexBufferComp->vertexSize(), (const void*)attributeOffset);
		}

		std::string errorLog;
		uint32_t error = GraphicsUtility::checkGLError(&errorLog);
		if (GL_NO_ERROR != error) {
			LogError("OpenGL error occurred while setting attribute pointer: " + errorLog);
		}
		return (GL_NO_ERROR == error);

	}

	bool ActorPrefabEntityController::buildPbrMaterials(ActorPrefabEntityPtr pActorEntity, TriangleMeshEntityPtr pTriangleMeshEntity) {
		bool result = false;
		if (nullptr == pActorEntity) throw NullpointerExcept("pActorEntity");
		if (nullptr == pTriangleMeshEntity) throw NullpointerExcept("pTriangleMeshEntity");

		auto pMaterialsComp = pTriangleMeshEntity->getMeshMaterialsComponent();
		if (nullptr == pMaterialsComp) throw MissingComponentException(MeshMaterialsComponent::identification);

		if (!pActorEntity->hasComponent(PbrMaterialsComponent::identification)) pActorEntity->addComponent(std::make_shared<PbrMaterialsComponent>());
		auto pPbrMaterialsComp = pActorEntity->getPBRMaterialsComponent();
		pPbrMaterialsComp->clear();

		for (uint32_t i = 0; i < pMaterialsComp->getMaterialCount(); ++i) {
			auto pMat = pMaterialsComp->getMaterial(i);
			auto pPbrMat = std::make_shared<PbrMaterial>();
			pPbrMaterialsComp->addPbrMaterial(pPbrMat);

			pPbrMat->metallic() = pMat->getMetallic();
			pPbrMat->roughness() = pMat->getRoughness();
			pPbrMat->ambientOcclusion() = 0.0f;

			// retrieve colors
			const Eigen::Vector4f ambientColor = pMat->getColor(MeshMaterial::COLOR_TYPE_AMBIENT);
			const Eigen::Vector4f diffuseColor = pMat->getColor(MeshMaterial::COLOR_TYPE_DIFFUSE);
			pPbrMat->color(PbrMaterial::COLOR_TYPE_ALBEDO) = (ambientColor.sum() > diffuseColor.sum()) ? ambientColor : diffuseColor;
			pPbrMat->color(PbrMaterial::COLOR_TYPE_EMISSIVE) = pMat->getColor(MeshMaterial::COLOR_TYPE_EMISSIVE);
			pPbrMat->color(PbrMaterial::COLOR_TYPE_REFLECTIVE) = pMat->getColor(MeshMaterial::COLOR_TYPE_REFLECTIVE);
			pPbrMat->color(PbrMaterial::COLOR_TYPE_SPECULAR) = pMat->getColor(MeshMaterial::COLOR_TYPE_SPECULAR);

			// add textures, if set
			TextureProviderPtr pTexProvider = TextureProvider::instance();
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_DIFFUSE).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_DIFFUSE);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, true);
				}
				catch (CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (nullptr == pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO) && !pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_AMBIENT).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_AMBIENT);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, true);
				}
				catch (CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_NORMAL).empty()) {
				try{
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_NORMAL);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_NORMAL) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, true);
				}
				catch (CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_HEIGHT).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_HEIGHT);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_HEIGHT) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, false);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_METALLIC_ROUGHNESS).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_METALLIC_ROUGHNESS);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_METALLIC_ROUGHNESS) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, false);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_EMISSIVE).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_EMISSIVE);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_EMISSIVE) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, false);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
			if (!pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_OCCLUSION).empty()) {
				try {
					const std::string texPath = pMat->getTexturePath(MeshMaterial::TEXTURE_TYPE_OCCLUSION);
					pPbrMat->texture(PbrMaterial::TEXTURE_TYPE_OCCLUSION) = (pTexProvider->isTextureRegistered(texPath)) ? pTexProvider->getTexture(texPath) : pTexProvider->createTexture(texPath, texPath, false);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}

			pPbrMat->updateUbo();
		}


		return result;
	}


	






	

}