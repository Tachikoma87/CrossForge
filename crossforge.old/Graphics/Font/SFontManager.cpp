#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "../../Core/CrossForgeException.h"
#include "SFontManager.h"

namespace CForge {

	uint32_t SFontManager::m_InstanceCount = 0;
	SFontManager* SFontManager::m_pInstance = nullptr;

	SFontManager* SFontManager::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SFontManager();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SFontManager::release(void) {
		if (m_InstanceCount == 0) throw CForgeExcept("Not enough instance for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release


	SFontManager::SFontManager(void): CForgeObject("SFontManager") {
		m_pFTLib = nullptr;
		init();
	}//Constructor

	SFontManager::~SFontManager(void) {
		clear();
	}//Destructor

	void SFontManager::init(void) {
		FT_Library Lib;
		auto Error = FT_Init_FreeType(&Lib);
		if (0 != Error) throw CForgeExcept("Initializing Freetype library failed with error code: " + std::to_string(Error));
		m_pFTLib = static_cast<void*>(Lib);
	}//initialize

	void SFontManager::clear(void) {
		if(nullptr != m_pFTLib) FT_Done_FreeType(static_cast<FT_Library>(m_pFTLib));
		m_pFTLib = nullptr;
	}//clear

	void* SFontManager::freetypeLibraryHandle(void) {
		return m_pFTLib;
	}//freetypeLibraryHandle

	Font* SFontManager::createFont(std::string Filename, uint32_t Size) {
		Font::FontStyle Style;
		Style.FileName = Filename;
		Style.PixelSize = Size;
		return createFont(Style);
	}//createFont

	Font* SFontManager::createFont(Font::FontStyle Style) {
		Font* pRval = nullptr;
		//does font already exist?
		for (auto i : m_FontInstances) {
			if (nullptr == i) continue;
			if (Style == i->StyleParams) {
				pRval = i->pInstance;
				i->InstanceCount++;
				break;
			}
		}

		if (nullptr == pRval) {
			FontInstance* pNewFont = new FontInstance();
			// create new font face
			pNewFont->pInstance = new Font(Style);
			pNewFont->InstanceCount = 0;
			pNewFont->StyleParams = Style;
			m_FontInstances.push_back(pNewFont);
			pRval = pNewFont->pInstance;
		}
		return pRval;
	}//createFont

	void SFontManager::releaseFont(Font* pFont) {

	}//releaseFont


	

}//name-space