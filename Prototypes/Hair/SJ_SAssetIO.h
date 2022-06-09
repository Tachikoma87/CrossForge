/*****************************************************************************\
*                                                                           *
* File(s): SJ_SAssetIO.h and SJ_SAssetIO.cpp                                      *
*                                                                           *
* Content:                                                                  *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann, Sascha Jüngel                                     *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SJSASSETIO_H__
#define __CFORGE_SJSASSETIO_H__

#include "../../CForge/AssetIO/I3DMeshIO.h"
#include "../../CForge/AssetIO/I2DImageIO.h"
#include "I3DHairIO.h"

namespace CForge {

	/**
	* \brief This singleton class handles import and export of meshes, image and hair files.
	*
    * \todo integrate hair code into crossforge SAssetIO class and delete this
	* \todo Do full documentation
	* \todo Design and implement mesh file format for crossforge
	* \todo Implement TImagePyramid (handles image pyramid with different resolutions)
	* \todo implement DDS import/export
	*/

	class CFORGE_API SJ_SAssetIO: public CForgeObject {
	public:
		static SJ_SAssetIO* instance(void);

		static void store(const std::string Filepath, const T3DMesh<float>* pMesh);
		static void load(const std::string Filepath, T3DMesh<float>* pMesh);

		static void store(const std::string Filepath, const T2DImage<uint8_t>* pImage);
		static void load(const std::string Filepath, T2DImage<uint8_t>* pImage);

        static void store(const std::string Filepath, const T3DHair<float>* pHair);
		static void load(const std::string Filepath, T3DHair<float>* pHair);

		static std::string readTextFile(const std::string Filepath);

		void release(void);

	protected:
		SJ_SAssetIO(void);
		~SJ_SAssetIO(void);

		void init(void);
		void clear(void);

		void storeModel(const std::string Filepath, const T3DMesh<float>* pMesh);
		void loadModel(const std::string Filepath, T3DMesh<float>* pMesh);

		void storeImage(const std::string Filepath, const T2DImage<uint8_t>* pImage);
		void loadImage(const std::string Filepath, T2DImage<uint8_t>* pImage);

        void storeHair(const std::string Filepath, const T3DHair<float>* pHair);
		void loadHair(const std::string Filepath, T3DHair<float>* pHair);

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

        struct HairIOPlugin {
			I3DHairIO* pInstance;
			std::string Name;

			HairIOPlugin(void) {
				pInstance = nullptr;
				Name = "";
			}
		};

		std::vector<ModelIOPlugin> m_ModelIOPlugins;
		std::vector<ImageIOPlugin> m_ImageIOPlugins;
        std::vector<HairIOPlugin> m_HairIOPlugins;

		static SJ_SAssetIO* m_pInstance;
		static uint32_t m_InstanceCount;
	};//SJ_SAssetIO

	//typedef SJ_SAssetIO AssetIO;

}//name space

#endif