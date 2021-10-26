/*****************************************************************************\
*                                                                           *
* File(s): SAssetIO.h and SAssetIO.cpp                                     *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SASSETIO_H__
#define __CFORGE_SASSETIO_H__

#include "I3DMeshIO.h"
#include "I2DImageIO.h"

namespace CForge {

	/**
	* \brief This singleton class handles import and export of meshes and image files.
	*
	* \todo Do full documentation
	* \todo Design and implement mesh file format for crossforge
	* \todo Implement TImagePyramid (handles image pyramid with different resolutions)
	* \todo implement DDS import/export
	*/

	class CFORGE_IXPORT SAssetIO: public CForgeObject {
	public:
		static SAssetIO* instance(void);

		static void store(const std::string Filepath, const T3DMesh<float>* pMesh);
		static void load(const std::string Filepath, T3DMesh<float>* pMesh);

		static void store(const std::string Filepath, const T2DImage<uint8_t>* pImage);
		static void load(const std::string Filepath, T2DImage<uint8_t>* pImage);

		static std::string readTextFile(const std::string Filepath);

		void release(void);

	protected:
		SAssetIO(void);
		~SAssetIO(void);

		void init(void);
		void clear(void);

		void storeModel(const std::string Filepath, const T3DMesh<float>* pMesh);
		void loadModel(const std::string Filepath, T3DMesh<float>* pMesh);
		
		void storeImage(const std::string Filepath, const T2DImage<uint8_t>* pImage);
		void loadImage(const std::string Filepath, T2DImage<uint8_t>* pImage);

	private:
		struct ModelIOPlugin {
			I3DMeshIO* pInstance;
			std::string Name;

			ModelIOPlugin(void) {
				pInstance = nullptr;
				Name = "";
			}
		};

		struct ImageIOPlugin {
			I2DImageIO* pInstance;
			std::string Name;

			ImageIOPlugin(void) {
				pInstance = nullptr;
				Name = "";
			}
		};

		std::vector<ModelIOPlugin> m_ModelIOPlugins;
		std::vector<ImageIOPlugin> m_ImageIOPlugins;

		static SAssetIO* m_pInstance;
		static uint32_t m_InstanceCount;
	};//SAssetIO

	typedef SAssetIO AssetIO;

}//name space


#endif