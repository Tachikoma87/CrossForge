/*****************************************************************************\
*                                                                           *
* File(s): SAssetIOProvider.h and SAssetIOProvider.cpp                              *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_SASSETIOPROVIDER_H__
#define __CROSSFORGE_SASSETIOPROVIDER_H__

#include "controller/FileIOControllerBase.h"
#include "controller/Image2DIOControllerBase.h"
#include "controller/TriangleMeshIOControllerBase.h"

namespace crossforge {
	class SAssetIOProvider {
	public:
		static std::shared_ptr<SAssetIOProvider> instance();
		static void destroy();

		bool loadMesh(TriangleMeshEntityPtr pTrianglMeshEntity, const std::string filepath);
		bool loadImage2D(Image2DEntityPtr pImage2DEntity, const std::string filepath);
		bool loadFile(FileEntityPtr pFileEntity, const std::string filepath, bool binary);

		bool canLoadMeshFile(const std::string filepath);
		bool canLoadImage2DFile(const std::string filepath);
		bool canStoreMeshFile(const std::string filepath);
		bool canStoreImage2DFile(const std::string filepath);

		bool storeMesh(TriangleMeshEntityPtr pTrianglMeshEntity, const std::string filepath);
		bool storeImage2D(Image2DEntityPtr pImage2DEntity, const std::string filepath);
		bool storeFile(FileEntityPtr pFileEntity, const std::string filepath, bool binary);

		bool addFileIOController(FileIOControllerBasePtr pController);
		bool addTrianglMeshIOController(TriangleMeshIOControllerBasePtr pController);
		bool addImage2DIOController(Image2DIOControllerBasePtr pController);

		bool hasFileIOController(const std::string identification);
		bool hasTriangleMeshIOController(const std::string identification);
		bool hasImage2DIOController(const std::string identification);
		

		~SAssetIOProvider();
	protected:
		static std::shared_ptr<SAssetIOProvider> m_pInstance;

		SAssetIOProvider();

		void initialize();
		void clear();

		std::vector<FileIOControllerBasePtr> m_fileIOControllers;
		std::vector<Image2DIOControllerBasePtr> m_image2DIOControllers;
		std::vector<TriangleMeshIOControllerBasePtr> m_triangleMeshIOControllers;

	};

	typedef SAssetIOProvider AssetIOProvider;
	typedef std::shared_ptr<SAssetIOProvider> AssetIOProviderPtr;
}



#endif