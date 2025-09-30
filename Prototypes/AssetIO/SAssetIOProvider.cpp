#include "SAssetIOProvider.h"

#include "../utility/FileUtility.h"
#include "controller/FileIOSystemController.h"
#include "controller/Image2DIOStbController.h"
#include "controller/TriangleMeshIOAssimpController.h"

namespace crossforge {
	std::shared_ptr<SAssetIOProvider> SAssetIOProvider::m_pInstance = nullptr;

	std::shared_ptr<SAssetIOProvider> SAssetIOProvider::instance() {
		if (nullptr == m_pInstance) {
			AssetIOProvider* pAssetIOProvider = new SAssetIOProvider();
			pAssetIOProvider->initialize();
			m_pInstance = std::make_shared<SAssetIOProvider>(*pAssetIOProvider);
		}
		return m_pInstance;
	}

	void SAssetIOProvider::destroy() {
		m_pInstance = nullptr;
	}

	SAssetIOProvider::SAssetIOProvider() {
		
	}
	SAssetIOProvider::~SAssetIOProvider() {
		clear();
	}

	void SAssetIOProvider::initialize() {
		clear();

		m_fileIOControllers.push_back(std::make_shared<FileIOSystemController>());
		m_image2DIOControllers.push_back(std::make_shared<Image2DIOStbController>());
		m_triangleMeshIOControllers.push_back(std::make_shared<TriangleMeshIOAssimpController>());

	}
	void SAssetIOProvider::clear() {
		m_fileIOControllers.clear();
		m_image2DIOControllers.clear();
		m_triangleMeshIOControllers.clear();
	}

	bool SAssetIOProvider::loadMesh(TriangleMeshEntityPtr pTriangleMeshEntity, const std::string filepath) {
		if (nullptr == pTriangleMeshEntity) throw NullpointerExcept("pTriangleMeshEntity");
		if (!FileUtility::exists(filepath)) {
			LogError("File " + filepath + " does not exist. Can not load!");
			return false;
		}
		bool result = false;
		for (auto pController : m_triangleMeshIOControllers) {
			try {
				if (pController->canAcceptFile(filepath, TriangleMeshIOControllerBase::OP_LOAD) && pController->load(pTriangleMeshEntity, filepath)) {
					result = true;
					break;
				}
			}
			catch (CrossForgeException e) {
				Logger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during loading of triangle mesh from " + filepath);
			}
		}
		return result;
	}

	bool SAssetIOProvider::loadImage2D(Image2DEntityPtr pImage2DEntity, const std::string filepath) {
		if (nullptr == pImage2DEntity) throw NullpointerExcept("pimage2DEntity");
		if (!FileUtility::exists(filepath)) {
			LogError("File " + filepath + " does not exist. Can not load image!");
			return false;
		}
		bool result = false;
		for (auto pController : m_image2DIOControllers) {
			try {
				if (pController->canAcceptFile(filepath, Image2DIOControllerBase::OP_LOAD) && pController->load(pImage2DEntity, filepath)) {
					result = true;
					break;
				}
			}
			catch (CrossForgeException e) {
				Logger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during loading of image from " + filepath);
			}
		}
		return true;
	}
	bool SAssetIOProvider::loadFile(FileEntityPtr pFileEntity, const std::string filepath, bool binary) {
		if (nullptr == pFileEntity) throw NullpointerExcept("pFileEntity");
		if (!FileUtility::exists(filepath)) {
			LogError("File " + filepath + " does not exist. can not load file!");
			return false;
		}
		bool result = false;
		for (auto pController : m_fileIOControllers) {
			try {
				if (pController->load(pFileEntity, filepath, binary)) {
					result = true;
					break;
				}
			}
			catch (CrossForgeException e) {
				Logger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during loading image from " + filepath);
			}
		}
		return true;
	}

	bool SAssetIOProvider::canLoadMeshFile(const std::string filepath) {
		bool result = false;
		for (auto pController : m_triangleMeshIOControllers) {
			if (pController->canAcceptFile(filepath, TriangleMeshIOControllerBase::OP_LOAD)) {
				result = true;
				break;
			}
		}
		return result;
	}
	bool SAssetIOProvider::canLoadImage2DFile(const std::string filepath) {
		bool result = false;
		for (auto pController : m_image2DIOControllers) {
			if (pController->canAcceptFile(filepath, Image2DIOControllerBase::OP_LOAD)) {
				result = true;
				break;
			}
		}
		return result;
	}
	bool SAssetIOProvider::canStoreMeshFile(const std::string filepath) {
		bool result = false;
		for (auto pController : m_triangleMeshIOControllers) {
			if (pController->canAcceptFile(filepath, TriangleMeshIOControllerBase::OP_STORE)) {
				result = true;
				break;
			}
		}
		return result;
	}
	bool SAssetIOProvider::canStoreImage2DFile(const std::string filepath) {
		bool result = false;
		for (auto pController : m_image2DIOControllers) {
			if (pController->canAcceptFile(filepath, Image2DIOControllerBase::OP_STORE)) {
				result = true;
				break;
			}
		}
		return result;
	}

	bool SAssetIOProvider::storeMesh(TriangleMeshEntityPtr pTrianglMeshEntity, const std::string filepath) {
		if (nullptr == pTrianglMeshEntity) throw NullpointerExcept("pTriangleMeshEntity");
		bool result = false;
		for (auto pController : m_triangleMeshIOControllers) {
			try {
				if (pController->canAcceptFile(filepath, TriangleMeshIOControllerBase::OP_STORE) && pController->store(pTrianglMeshEntity, filepath)) {
					result = true; 
					break;
				}
			}
			catch (CrossForgeException e) {
				SLogger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during storing of triangle mesh to " + filepath);
			}
		}
		return result;
	}
	bool SAssetIOProvider::storeImage2D(Image2DEntityPtr pImage2DEntity, const std::string filepath) {
		if (nullptr == pImage2DEntity) throw NullpointerExcept("pImage2DEntity");
		bool result = false;
		for (auto pController : m_image2DIOControllers) {
			try {
				if (pController->canAcceptFile(filepath, Image2DIOControllerBase::OP_STORE) && pController->store(pImage2DEntity, filepath)) {
					result = true;
					break;
				}
			}
			catch (CrossForgeException e) {
				SLogger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during storing of triangle mesh to " + filepath);
			}
		}
		return result;
	}
	bool SAssetIOProvider::storeFile(FileEntityPtr pFileEntity, const std::string filepath, bool binary) {
		if (nullptr == pFileEntity) throw NullpointerExcept("pFileEntity");
		bool result = false;
		for (auto pController : m_fileIOControllers) {
			try {
				if (pController->store(pFileEntity, filepath, binary)) {
					result = true;
					break;
				}
			}
			catch (CrossForgeException e) {
				SLogger::logException(e);
			}
			catch (...) {
				LogError("An unexpected exception occurred during storing of triangle mesh to " + filepath);
			}
		}
		return result;
	}
	bool SAssetIOProvider::hasFileIOController(const std::string identification) {
		bool result = false;
		for (auto pController : m_fileIOControllers) {
			if (0 == identification.compare(pController->getIdentification())) {
				result = true;
				break;
			}
		}
		return result;
	}
	bool SAssetIOProvider::hasTriangleMeshIOController(const std::string identification) {
		bool result = false;
		for (auto pController : m_triangleMeshIOControllers) {
			if (0 == identification.compare(pController->getIdentification())) {
				result = true;
				break;
			}
		}
		return result;
	}
	bool SAssetIOProvider::hasImage2DIOController(const std::string identification) {
		bool result = false;
		for (auto pController : m_image2DIOControllers) {
			if (0 == identification.compare(pController->getIdentification())) {
				result = true;
				break;
			}
		}
		return result;
	}

	bool SAssetIOProvider::addFileIOController(FileIOControllerBasePtr pController) {
		if (nullptr == pController) throw NullpointerExcept("pController");

		if (hasFileIOController(pController->getIdentification())) {
			LogError("File controller with identification " + pController->getIdentification() + " already exists!");
			return false;
		}
		
		m_fileIOControllers.push_back(pController);
		return true;
	}
	bool SAssetIOProvider::addTrianglMeshIOController(TriangleMeshIOControllerBasePtr pController) {
		if (nullptr == pController) throw NullpointerExcept("pController");
		if (hasTriangleMeshIOController(pController->getIdentification())) {
			LogError("Triangle mesh io controller with identification " + pController->getIdentification() + " already exists!");
			return false;
		}
		m_triangleMeshIOControllers.push_back(pController);
		return true;
	}
	bool SAssetIOProvider::addImage2DIOController(Image2DIOControllerBasePtr pController) {
		if (nullptr == pController) throw NullpointerExcept("pController");
		if (hasImage2DIOController(pController->getIdentification())) {
			LogError("Image 2D io controller with identification " + pController->getIdentification() + " already exists!");
			return false;
		}
		return true;
	}

	
}