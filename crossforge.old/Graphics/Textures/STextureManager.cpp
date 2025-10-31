#include "STextureManager.h"
#include "../../AssetIO/SAssetIO.h"
#include "../../Core/SLogger.h"
#include "../../AssetIO/File.h"

namespace CForge {

	STextureManager* STextureManager::m_pInstance = nullptr;
	uint32_t STextureManager::m_InstanceCount = 0;

	STextureManager* STextureManager::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new STextureManager();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void STextureManager::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

    GLTexture2D* STextureManager::create(const std::string Filepath) {
		if (!File::exists(Filepath)) throw CForgeExcept("File " + Filepath + " does not exist!");

        GLTexture2D* pRval = nullptr;
        STextureManager* pTexMan = STextureManager::instance();
        pRval = pTexMan->createTexture2D(Filepath);
        pTexMan->release();
        return pRval;
    }//load

    // Todo: texture is not added to the internal texture cache m_Textures and thus not cleared up properly
	GLTexture2D* STextureManager::create(uint32_t Width, uint32_t Height, uint8_t R, uint8_t G, uint8_t B, bool GenerateMipmaps) {
		GLTexture2D* pRval = nullptr;
		STextureManager* pTexMan = STextureManager::instance();
		pRval = pTexMan->createColorTexture2D(Width, Height, R, G, B, GenerateMipmaps);
		pTexMan->release();
		return pRval;
	}//create

    // Todo: texture is not added to the internal texture cache m_Textures and thus not cleared up properly
    GLTexture2D* STextureManager::fromHandle(uint32_t handle) {
        return new GLTexture2D(handle);
    }

	void STextureManager::destroy(GLTexture2D* pTex) {
		if (nullptr == pTex) throw NullpointerExcept("pTex");
		bool contained = false;
		for (auto &i : m_pInstance->m_Textures) {
			if (nullptr == i) continue;
			if (i->Tex.objectID() == pTex->objectID()) {
				contained = true;
				if (i->ReferenceCount == 1) {
					delete i;
					i = nullptr;
				}
				else {
					i->ReferenceCount--;
				}
			}
		}//for[all textures
		if(!contained) //texture is not in scope of texture manager
			delete pTex;
	}//destroy

	STextureManager::STextureManager(void): CForgeObject("STextureManager") {

	}//Constructor

	STextureManager::~STextureManager(void) {
		clear();
	}//Destructor

	void STextureManager::init(void) {
		clear();
	}//initialize

	void STextureManager::clear(void) {
		for (auto& i : m_Textures) {
			delete i;
			i = nullptr;
		}
		m_Textures.clear();
	}//clear


	GLTexture2D* STextureManager::createTexture2D(const std::string Filepath) {
		if (!File::exists(Filepath)) throw CForgeExcept("Image file " + Filepath + " could not be found!");

		GLTexture2D* pRval = nullptr;

		// texture already available?
		for (auto i : m_Textures) {
			if (nullptr != i && 0 == Filepath.compare(i->Filepath)) {
				pRval = &i->Tex;
				i->ReferenceCount++;
				break;
			}
		}//for[all available textures]

		// load and build texture if not already available
		if (nullptr == pRval) {
			T2DImage<uint8_t> RawImg;
			Texture* pTex = nullptr;

			try {
				SAssetIO::load(Filepath, &RawImg);
				if (RawImg.width() == 0 || RawImg.height() == 0) throw CForgeExcept("Image " + Filepath + " contains no data!");
				pTex = new Texture();
				pTex->Filepath = Filepath;
				pTex->Tex.init(&RawImg, true);
				pTex->ReferenceCount = 1;
				m_Textures.push_back(pTex);
				pRval = &pTex->Tex;
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
				delete pTex;
				pTex = nullptr;
			}
		}

		return pRval;
	}//createTexture2D

	GLTexture2D* STextureManager::createColorTexture2D(uint32_t Width, uint32_t Height, uint8_t R, uint8_t G, uint8_t B, bool GenerateMipmaps) {
		if (Width == 0 || Height == 0) throw CForgeExcept("Invalid width and height for image specified!");

		T2DImage<uint8_t> Img;
		uint8_t* pData = new uint8_t[Width * Height * 3];
		for (uint32_t i = 0; i < Width * Height * 3; i += 3) {
			pData[i + 0] = R;
			pData[i + 1] = G;
			pData[i + 2] = B;
		}//for[pixeld data]

		Img.init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGB, pData);
		delete[] pData;
		pData = nullptr;

		GLTexture2D* pRval = new GLTexture2D();
		pRval->init(&Img, GenerateMipmaps);

		return pRval;
	}//createColorTextur2D

}//name space