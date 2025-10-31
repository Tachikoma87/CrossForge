
#include <Windows.h>
#include <glad/glad.h>
#include "RenderingController.h"

#include "../components/ColorComponent.h"
#include "../controllers/ShaderEntityController.h"
#include "../provider/SShaderProvider.h"

namespace crossforge {

	ShaderPropertiesComponentPtr RenderingController::m_pShaderProperties = std::make_shared<ShaderPropertiesComponent>();;

	RenderingController::RenderingController(): ControllerBase(RenderingController::identification) {

	}

	RenderingController::~RenderingController() {

	}

	void RenderingController::activateCanvas(CanvasEntityPtr pCanvas, bool clearBackground) {
		// set viewport
		auto pCanvasSettings = pCanvas->getCanvasSettingsComponent();

		glViewport(pCanvasSettings->viewportPosition().x(), pCanvasSettings->viewportPosition().y(),
			pCanvasSettings->viewportSize().x(), pCanvasSettings->viewportSize().y());

		if (clearBackground) {
			glScissor(pCanvasSettings->viewportPosition().x(), pCanvasSettings->viewportPosition().y(),
				pCanvasSettings->viewportSize().x(), pCanvasSettings->viewportSize().y());
			glEnable(GL_SCISSOR_TEST);

			Eigen::Vector4f color = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
			if (pCanvas->hasComponent(ColorComponent::identification)) color = pCanvas->getComponent<ColorComponent>()->color();

			glClearColor(color.x(), color.y(), color.z(), color.w());
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_SCISSOR_TEST);
		}
	}

	void RenderingController::drawActor(ShaderProvider::RenderPass renderPass, ActorInstanceEntityPtr pActorInstance, ActorPrefabEntityPtr pActorPrefab, CameraEntityPtr pCamera, LightsEntityPtr pLights) {
		if (nullptr == pActorInstance) throw NullpointerExcept("pActorInstance");
		if (nullptr == pActorPrefab) throw NullpointerExcept("pActorPrefab");
		if (nullptr == pCamera) throw NullpointerExcept("pCamera");

		auto pVertexArrayComp = pActorPrefab->getVertexArrayComponent();
		auto pRenderGroupsComp = pActorPrefab->getRenderGroupsComponent();
		auto pPbrMaterialsComp = pActorPrefab->getPBRMaterialsComponent();

		if (nullptr == pVertexArrayComp) throw MissingComponentException(VertexArrayComponent::identification);
		if (nullptr == pRenderGroupsComp) throw MissingComponentException(RenderGroupsComponent::identification);
		
		auto renderGroupsList = pRenderGroupsComp->renderGroups();
		auto materialIndexList = pRenderGroupsComp->renderGroupsMaterial();

		glBindVertexArray(pVertexArrayComp->glVertexArrayHandle());
		int32_t materialIndex = 0;
		for (auto renderGroup : renderGroupsList) {

			// set material
			auto pPbrMaterial = (nullptr != pPbrMaterialsComp) ? pPbrMaterialsComp->getMaterial(materialIndexList[materialIndex++]) : nullptr;
			// @ToDo: implement default material
			//if(nullptr == pPbrMaterial) // 
			
			auto pLightsConfig = pLights->getLightsConfigComponent();
			if (nullptr == pLightsConfig) throw MissingComponentException(LightsConfigComponent::identification);
			
			m_pShaderProperties->directionalLightsSize() = pLightsConfig->directionalLightsUBOSize();
			m_pShaderProperties->pointLightsSize() = pLightsConfig->pointLightsUBOSize();
			m_pShaderProperties->spotLightsSize() = pLightsConfig->spotLightsUBSSize();
			m_pShaderProperties->activeDirectionalLightsCount() = pLightsConfig->activeDirectionalLights();
			m_pShaderProperties->activePointLightsCount() = pLightsConfig->activePointLights();
			m_pShaderProperties->activeSpotLightsCount() = pLightsConfig->activeSpotLights();

			uint16_t featureMask = 0;
			if(m_pShaderProperties->directionalLightsSize() > 0 && m_pShaderProperties->activeDirectionalLightsCount() > 0) featureMask |= ShaderPropertiesComponent::SHADER_FEATURE_DIRECTIONAL_LIGHTS;
			if (m_pShaderProperties->pointLightsSize() > 0 && m_pShaderProperties->activePointLightsCount() > 0) featureMask |= ShaderPropertiesComponent::SHADER_FEATURE_POINT_LIGHTS;
			if (m_pShaderProperties->spotLightsSize() > 0 && m_pShaderProperties->activeSpotLightsCount() > 0) featureMask |= ShaderPropertiesComponent::SHADER_FEATURE_SPOT_LIGHTS;
			m_pShaderProperties->featureMask() = featureMask;

			ShaderEntityPtr pShader = ShaderProvider::instance()->getShader(renderPass, m_pShaderProperties);
			if (nullptr == pShader) {
				LogError("Failed to get appropriate shader. Can not draw mesh.");
				continue;
			}


			ShaderEntityController::bindRenderingShader(pShader);
			ShaderEntityController::bindTransformationDataUBO(pShader, pActorInstance->getUboTransformationDataComponent());
			ShaderEntityController::bindCameraDataUBO(pShader, pCamera->getUboCameraDataComponent());
			if(pShader->getRenderingShaderComponent()->baseUboBindingPoint(RenderingShaderComponent::BASE_UBO_DIRECTIONALLIGHTSDATA) != GL_INVALID_INDEX)
				ShaderEntityController::bindBaseUbo(pShader, pLights->getUBODirectionalLightsComponent(), RenderingShaderComponent::BASE_UBO_DIRECTIONALLIGHTSDATA);

			ShaderEntityController::bindBaseUbo(pShader, pPbrMaterial->uboPbrMaterial(), RenderingShaderComponent::BASE_UBO_MATERIALDATA_PBR);
			TextureEntityPtr pAlbedoTex = pPbrMaterial->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO);
			if (nullptr != pAlbedoTex) ShaderEntityController::bindTexture(pShader, pAlbedoTex, RenderingShaderComponent::BASE_TEX_ALBEDO);
			
		
			const uint64_t offset = renderGroup.x() * sizeof(uint32_t);
			glDrawElements(GL_TRIANGLES, renderGroup.y() - renderGroup.x(), GL_UNSIGNED_INT, (const void*)offset);
		}
		

		glBindVertexArray(0);
	}

	
}