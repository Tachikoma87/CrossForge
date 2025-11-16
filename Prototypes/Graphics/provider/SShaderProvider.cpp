#include "SShaderProvider.h"

#include <crossforge/assetio/SAssetIOProvider.h>
#include "../controllers/ShaderEntityController.h"
#include <crossforge/ecs/SEntityManager.h>

namespace crossforge {

	std::shared_ptr<SShaderProvider> SShaderProvider::m_pInstance;

	std::shared_ptr<SShaderProvider> SShaderProvider::instance() {
		if (nullptr == m_pInstance) {
			SShaderProvider *pProvider = new SShaderProvider();
			m_pInstance = std::make_shared<SShaderProvider>(*pProvider);
		}
		return m_pInstance;
	}
	void SShaderProvider::destroy() {
		m_pInstance = nullptr;
	}

	SShaderProvider::SShaderProvider() {

	}
	SShaderProvider::~SShaderProvider() {
		m_shaderMap.clear();
	}

	bool SShaderProvider::registerShader(std::string shaderName, ShaderEntityPtr pShader) {
		if (nullptr == pShader) throw NullpointerExcept("pShader");
		bool result = false;
		if (shaderName.empty()) LogError("Empty shader name specified!");
		else {
			m_shaderMap[shaderName] = pShader;
			result = true;
		}
		return result;
	}

	bool SShaderProvider::isShaderRegistered(std::string shaderName)const {
		return (m_shaderMap.end() != m_shaderMap.find(shaderName));
	}
	uint32_t SShaderProvider::getShaderCount()const {
		return m_shaderMap.size();
	}
	ShaderEntityPtr SShaderProvider::getShader(std::string shaderName) {
		auto pItem = m_shaderMap.find(shaderName);
		return (m_shaderMap.end() == pItem) ? nullptr : pItem->second;

	}
	ShaderEntityPtr SShaderProvider::getShader(uint32_t index) {
		if (index >= m_shaderMap.size()) throw NullpointerExcept("index");
		auto iter = m_shaderMap.begin();
		for (uint32_t i = 0; i < index; ++i) iter++;
		return (*iter).second;
	}


	ShaderEntityPtr SShaderProvider::getShader(RenderPass pass, ShaderPropertiesComponentPtr pShaderProperties) {
		if (RENDER_PASS_UNKNOWN >= pass || pass >= RENDER_PASS_COUNT) throw IndexOutOfBoundsExcept("type");
		if (nullptr == pShaderProperties) throw NullpointerExcept("pShaderProperties");

		ShaderEntityPtr pResult = nullptr;

		// base name and feature mask
		std::string shaderId = "shader-" + std::to_string(pass) + "-" + std::to_string(pShaderProperties->featureMask()) + "-";
		// size of light UBOs
		shaderId += std::to_string(pShaderProperties->directionalLightsSize()) + std::to_string(pShaderProperties->pointLightsSize()) + std::to_string(pShaderProperties->spotLightsSize()) + "-";
		// number of active lights
		shaderId += std::to_string(pShaderProperties->activeDirectionalLightsCount()) + std::to_string(pShaderProperties->activePointLightsCount()) + std::to_string(pShaderProperties->activeSpotLightsCount());

		pResult = m_shaderMap[shaderId];
		if (nullptr == pResult) {
			pResult = std::make_shared<ShaderEntity>(ShaderEntity::COMPONENTS_ALL);
			pResult->getShaderPropertiesComponent()->initialize(pShaderProperties);
			auto pShaderSourceComp = pResult->getShaderSourceComponent();

			// @ToDo: Retrieve sources based on render pass
			FileEntityPtr pFileEntity = std::make_shared<FileEntity>();
			AssetIOProvider::instance()->loadFile(pFileEntity, "./Assets/Shader/ForwardPassPBS.vert", false);
			pShaderSourceComp->vertexShaderSources().push_back(pFileEntity->getStringComponent()->stringData());
			AssetIOProvider::instance()->loadFile(pFileEntity, "./Assets/Shader/ForwardPassPBS.frag", false);
			pShaderSourceComp->fragmentShaderSources().push_back(pFileEntity->getStringComponent()->stringData());

			try {
				if (!ShaderEntityController::configureShaderSource(pResult)) {
					LogError("Failed to configure shader " + shaderId + "!");
					pResult = nullptr;
				}
				else if (!ShaderEntityController::buildRenderingShader(pResult)) {
					LogError("Failed to build render shader " + shaderId + "!");
				}
				else {
					EntityManager::instance()->registerEntity(pResult);
					registerShader(shaderId, pResult);
					LogInfo("Successfully build render shader " + shaderId);
				}
			}
			catch (CrossForgeException& e) {
				LogError("Exception while configuring shader source: " + e.getMessage());
			}	
		}

		return pResult;
	}
}