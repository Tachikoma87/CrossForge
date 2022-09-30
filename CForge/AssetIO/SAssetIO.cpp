#include "SAssetIO.h"

#include "AssimpMeshIO.h"

#include "StbImageIO.h"
#include "WebPImageIO.h"

#include "OpenCVImageIO.h"

#include "../Core/SLogger.h"

namespace CForge {
	SAssetIO* SAssetIO::m_pInstance = nullptr;
	uint32_t SAssetIO::m_InstanceCount = 0;
	
	SAssetIO* SAssetIO::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SAssetIO();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	std::string SAssetIO::readTextFile(const std::string Filepath) {
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


	void SAssetIO::store(const std::string Filepath, const T3DMesh<float>* pMesh) {
		SAssetIO* pInstance = SAssetIO::instance();
		pInstance->storeModel(Filepath, pMesh);
		pInstance->release();
	}//store

	void SAssetIO::load(const std::string Filepath, T3DMesh<float>* pMesh) {
		SAssetIO* pInstance = SAssetIO::instance();
		pInstance->loadModel(Filepath, pMesh);
		pInstance->release();
	}//load

	void SAssetIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImage) {
		SAssetIO* pInstance = SAssetIO::instance();
		pInstance->storeImage(Filepath, pImage);
		pInstance->release();
	}//store

	void SAssetIO::load(const std::string Filepath, T2DImage<uint8_t>* pImage) {
		SAssetIO* pInstance = SAssetIO::instance();
		pInstance->loadImage(Filepath, pImage);
		pInstance->release();
	}//load

	void SAssetIO::release(void) {
		if (m_InstanceCount == 0) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release


	SAssetIO::SAssetIO(void): CForgeObject("SAssetIO") {
		m_ImageIOPlugins.clear();
		m_ModelIOPlugins.clear();
	}//Constructor

	SAssetIO::~SAssetIO(void) {
		clear();
	}//Destructor

	void SAssetIO::init(void) {
		// ModelIO Plugins
		ModelIOPlugin Plug;

		// Assimp mesh IO
		AssimpMeshIO *pAssimMeshIO = new AssimpMeshIO();
		pAssimMeshIO->init();
		Plug.pInstance = pAssimMeshIO;
		Plug.Name = pAssimMeshIO->pluginName();
		m_ModelIOPlugins.push_back(Plug);


		// Image Plugins
		ImageIOPlugin ImgPlug;

		StbImageIO* pStbImageIO = new StbImageIO();
		pStbImageIO->init();
		ImgPlug.pInstance = pStbImageIO;
		ImgPlug.Name = pStbImageIO->pluginName();
		m_ImageIOPlugins.push_back(ImgPlug);

		WebPImageIO* pWebPImageIO = new WebPImageIO();
		pWebPImageIO->init();
		ImgPlug.pInstance = pWebPImageIO;
		ImgPlug.Name = pWebPImageIO->pluginName();
		m_ImageIOPlugins.push_back(ImgPlug);

		OpenCVImageIO* pOpenCVImageIO = new OpenCVImageIO();
		pOpenCVImageIO->init();
		ImgPlug.pInstance = pOpenCVImageIO;
		ImgPlug.Name = pOpenCVImageIO->pluginName();
		m_ImageIOPlugins.push_back(ImgPlug);
	}//initialize

	void SAssetIO::clear(void) {
		// clean up
		for (auto& i : m_ImageIOPlugins) {
			if (nullptr != i.pInstance) i.pInstance->release();
		}
		for (auto& i : m_ModelIOPlugins) {
			if (nullptr != i.pInstance) i.pInstance->release();
		}

		m_ImageIOPlugins.clear();
		m_ModelIOPlugins.clear();
	}//initialize

	void SAssetIO::storeModel(const std::string Filepath, const T3DMesh<float>* pMesh) {
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
	}//store

	void SAssetIO::loadModel(const std::string Filepath, T3DMesh<float>* pMesh) {
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
	}//load


	void SAssetIO::storeImage(const std::string Filepath, const T2DImage<uint8_t>* pImage) {
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

	void SAssetIO::loadImage(const std::string Filepath, T2DImage<uint8_t>* pImage) {
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

}//name space