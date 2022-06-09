#include "SJ_SAssetIO.h"

#include "../../CForge/AssetIO/AssimpMeshIO.h"
#include "../../CForge/AssetIO/OpenCVImageIO.h"
#include "HairIO.h"

#include "../../CForge/AssetIO/File.h"

#include "../../CForge/Core/SLogger.h"

namespace CForge {
	SJ_SAssetIO* SJ_SAssetIO::m_pInstance = nullptr;
	uint32_t SJ_SAssetIO::m_InstanceCount = 0;

	SJ_SAssetIO* SJ_SAssetIO::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SJ_SAssetIO();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	std::string SJ_SAssetIO::readTextFile(const std::string Filepath) {
		std::string Rval = "";
		if (!File::exists(Filepath)) throw CForgeExcept("File " + Filepath + " does not exist!");

		File F;
		F.begin(Filepath, "r");
		uint8_t Buffer[128];

		while (!F.eof()) {
			uint32_t Length = F.read(Buffer, 128);
			Rval += std::string((const char* const)Buffer, Length);
		}

		return Rval;
	}//readTextFile

	void SJ_SAssetIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->storeModel(Filepath, pMesh);
		pInstance->release();
	}//store

	void SJ_SAssetIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->loadModel(Filepath, pMesh);
		pInstance->release();
	}//load

	void SJ_SAssetIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImage) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->storeImage(Filepath, pImage);
		pInstance->release();
	}//store

	void SJ_SAssetIO::load(const std::string Filepath, T2DImage<uint8_t>* pImage) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->loadImage(Filepath, pImage);
		pInstance->release();
	}//load

    void SJ_SAssetIO::store(const std::string Filepath, const T3DHair<float>* pHair) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->storeHair(Filepath, pHair);
		pInstance->release();
	}//store

	void SJ_SAssetIO::load(const std::string Filepath, T3DHair<float>* pHair) {
		SJ_SAssetIO* pInstance = SJ_SAssetIO::instance();
		pInstance->loadHair(Filepath, pHair);
		pInstance->release();
	}//load

	void SJ_SAssetIO::release(void) {
		if (m_InstanceCount == 0) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SJ_SAssetIO::SJ_SAssetIO(void): CForgeObject("SJ_SAssetIO") {
		m_ImageIOPlugins.clear();
		m_ModelIOPlugins.clear();
        m_HairIOPlugins.clear();
	}//constructor

	SJ_SAssetIO::~SJ_SAssetIO(void) {
		clear();
	}//destructor

	void SJ_SAssetIO::init(void) {
		// ModelIO Plugins
		ModelIOPlugin Plug;
		AssimpMeshIO *pAssimMeshIO = new AssimpMeshIO();
		pAssimMeshIO->init();
		Plug.pInstance = pAssimMeshIO;
		Plug.Name = pAssimMeshIO->pluginName();
		m_ModelIOPlugins.push_back(Plug);

		// ImageIO Plugins
		ImageIOPlugin ImgPlug;
		OpenCVImageIO* pOpenCVImageIO = new OpenCVImageIO();
		pOpenCVImageIO->init();
		ImgPlug.pInstance = pOpenCVImageIO;
		ImgPlug.Name = pOpenCVImageIO->pluginName();
		m_ImageIOPlugins.push_back(ImgPlug);

        // HairIO Plugins
        HairIOPlugin HairPlug;
        HairIO *pHairIO = new HairIO();
        pHairIO->init();
        HairPlug.pInstance = pHairIO;
        HairPlug.Name = pHairIO->pluginName();
        m_HairIOPlugins.push_back(HairPlug);

	}//initialize

	void SJ_SAssetIO::clear(void) {
		// clean up
		for (auto& i : m_ImageIOPlugins) {
			if (nullptr != i.pInstance) i.pInstance->release();
		}
		for (auto& i : m_ModelIOPlugins) {
			if (nullptr != i.pInstance) i.pInstance->release();
		}
        for (auto& i : m_HairIOPlugins) {
            if (nullptr != i.pInstance) i.pInstance->release();
        }

		m_ImageIOPlugins.clear();
		m_ModelIOPlugins.clear();
        m_HairIOPlugins.clear();
	}//clear

	void SJ_SAssetIO::storeModel(const std::string Filepath, const T3DMesh<float>* pMesh) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		for (auto i : m_ModelIOPlugins) {
			if (i.pInstance->accepted(Filepath, I3DMeshIO::OP_STORE)) {
				try {
					i.pInstance->store(Filepath, pMesh);
				}
				catch (CrossForgeException &e) {
					SLogger::logException(e);
					continue;
				}
				catch (...) {
					SLogger::log("An unhandled exception occurred during storing of " + Filepath);
				}
				break; // stored successfully
			}//if[suitable plugin found]
		}//for[all plugins]
	}//storeModel

	void SJ_SAssetIO::loadModel(const std::string Filepath, T3DMesh<float>* pMesh) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		for (auto i : m_ModelIOPlugins) {
			if (i.pInstance->accepted(Filepath, I3DMeshIO::OP_LOAD)) {
				try {
					i.pInstance->load(Filepath, pMesh);
				}
				catch (CrossForgeException& e) {
					SLogger::logException(e);
					continue;
				}
				catch (...) {
					SLogger::log("An unhandled exception occurred during loading of " + Filepath);
				}
				break; // loaded successfully
			}//if[matcing plugin found]
		}//for[all plugins]
	}//loadModel

	void SJ_SAssetIO::storeImage(const std::string Filepath, const T2DImage<uint8_t>* pImage) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified!");
		if (nullptr == pImage) throw NullpointerExcept("pImage");

		for (auto i : m_ImageIOPlugins) {
			if (i.pInstance->accepted(Filepath, I2DImageIO::OP_STORE) ) {
				try {
					i.pInstance->store(Filepath, pImage);
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
					continue;
				}
				catch (...) {
					SLogger::log("An unhandled error occurred during image storing to " + Filepath);
					continue;
				}
				break; // stored successfully
			}
		}//for[all plugins]
	}//storeImage

	void SJ_SAssetIO::loadImage(const std::string Filepath, T2DImage<uint8_t>* pImage) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified!");
		if (nullptr == pImage) throw NullpointerExcept("pImage");

		for (auto i : m_ImageIOPlugins) {
			if (i.pInstance->accepted(Filepath, I2DImageIO::OP_LOAD)) {
				try {

					i.pInstance->load(Filepath, pImage);
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
				}
				catch (...) {
					SLogger::log("An unhandled exception occurred during image loading from " + Filepath);
				}
				break; // successfully loaded
			}//if[accepted]
		}
	}//loadImage

    void SJ_SAssetIO::storeHair(const std::string Filepath, const T3DHair<float>* pHair) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
		if (nullptr == pHair) throw NullpointerExcept("pHair");

		for (auto i : m_HairIOPlugins) {
			if (i.pInstance->accepted(Filepath, I3DHairIO::OP_STORE)) {
				try {
					i.pInstance->store(Filepath, pHair);
				}
				catch (CrossForgeException &e) {
					SLogger::logException(e);
					continue;
				}
				catch (...) {
					SLogger::log("An unhandled exception occurred during storing of " + Filepath);
				}
				break; // stored successfully
			}//if[suitable plugin found]
		}//for[all plugins]
	}//storeHair

	void SJ_SAssetIO::loadHair(const std::string Filepath, T3DHair<float>* pHair) {
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified");
		if (nullptr == pHair) throw NullpointerExcept("pHair");

		for (auto i : m_HairIOPlugins) {
			if (i.pInstance->accepted(Filepath, I3DHairIO::OP_LOAD)) {
				try {
					i.pInstance->load(Filepath, pHair);
				}
				catch (CrossForgeException& e) {
					SLogger::logException(e);
					continue;
				}
				catch (...) {
					SLogger::log("An unhandled exception occurred during loading of " + Filepath);
				}
				break; // loaded successfully
			}//if[matcing plugin found]
		}//for[all plugins]
	}//loadHair

}//name space