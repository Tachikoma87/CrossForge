#include "STextureProvider.h"
#include "../controllers/TextureEntityController.h"
#include <crossforge/utility/FileUtility.h>
#include <crossforge/assetio/SAssetIOProvider.h>
#include <crossforge/ecs/SEntityManager.h>

namespace crossforge {

	std::shared_ptr<STextureProvider> STextureProvider::m_pInstance = nullptr;

	std::shared_ptr<STextureProvider> STextureProvider::instance() {
		if (nullptr == m_pInstance) {
			STextureProvider* pTextureProvider = new STextureProvider();
			m_pInstance = std::make_shared<STextureProvider>(*pTextureProvider);
		}
		return m_pInstance;
	}
	void STextureProvider::destroy() {
		m_pInstance = nullptr;
	}

	STextureProvider::STextureProvider() {

	}
	STextureProvider::~STextureProvider() {
		m_textureMap.clear();
	}

	bool STextureProvider::registerTexture(std::string textureName, TextureEntityPtr pTexture) {
		if (nullptr == pTexture) throw NullpointerExcept("pTexture");
		bool result = false;
		if (textureName.empty()) {
			LogError("Empty texture name specified!");
		}else if (isTextureRegistered(textureName)) {
			LogError("Texture with name " + textureName + " already exists. Can not register again!");
		}
		else {
			m_textureMap.insert(std::pair(textureName, pTexture));
			result = true;
		}
		return result;
	}
	TextureEntityPtr STextureProvider::createTexture(std::string textureName, std::string filename, bool generateMipmaps) {
		TextureEntityPtr pResult = nullptr;
		Image2DEntityPtr pImage = std::make_shared<Image2DEntity>();
		if (textureName.empty()) LogError("Empty texture name specified!");
		else if (filename.empty()) LogError("Empty filename specified!");
		else if (!FileUtility::exists(filename)) LogError("File with filename " + filename + " does not exist!");
		else if (!AssetIOProvider::instance()->loadImage2D(pImage, filename)) LogError("Failed to load image file " + filename);
		else{
			pResult = std::make_shared<TextureEntity>();

			if (!TextureEntityController::buildTexture2D(pResult, pImage, generateMipmaps)) {
				LogError("Building 2D texture from file " + filename + " failed.");
				pResult = nullptr;
			}
			else {
				SEntityManager::getInstance()->registerEntity(pResult);
				registerTexture(textureName, pResult);
			}
		}

		return pResult;
	}

	bool STextureProvider::isTextureRegistered(std::string textureName)const {
		return (m_textureMap.end() != m_textureMap.find(textureName));
	}
	uint32_t STextureProvider::getTextureCount()const {
		return m_textureMap.size();
	}
	TextureEntityPtr STextureProvider::getTexture(uint32_t index) {
		if (index >= m_textureMap.size()) throw IndexOutOfBoundsExcept("index");
		auto pIter = m_textureMap.begin();
		for (uint32_t i = 0; i < index; ++i) pIter++;
		return pIter->second;
		
	}
	TextureEntityPtr STextureProvider::getTexture(std::string textureName) {
		auto pItem = m_textureMap.find(textureName);
		return (m_textureMap.end() == pItem) ? nullptr : pItem->second;
	}
}