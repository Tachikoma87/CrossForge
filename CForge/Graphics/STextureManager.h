/*****************************************************************************\
*                                                                           *
* File(s): STextureManager.h and STextureManager.cpp                             *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_STEXTUREMANAGER_H__
#define __CFORGE_STEXTUREMANAGER_H__

#include "GLTexture2D.h"

namespace CForge {
	/**
	* \brief Manager object implemented as singleton that handles creation of textures and prevents redundant loading/creation.
	*
	* \todo Full documentation.
	*/
	class CFORGE_API STextureManager: public CForgeObject {
	public:
		static STextureManager* instance(void);
		void release(void);

		static GLTexture2D* create(const std::string Filepath);
		static GLTexture2D* create(uint32_t Width, uint32_t Height, uint8_t R, uint8_t G, uint8_t B, bool GenerateMipmaps = false);
		static GLTexture2D* fromHandle(uint32_t handle);
        static void destroy(GLTexture2D* pTex);

	protected:
		STextureManager(void);
		~STextureManager(void);
		void init(void);
		void clear(void);

	private:
		static STextureManager* m_pInstance;
		static uint32_t m_InstanceCount;

		GLTexture2D *createTexture2D(const std::string Filepath);
		GLTexture2D* createColorTexture2D(uint32_t Width, uint32_t Height, uint8_t R, uint8_t G, uint8_t B, bool GenerateMipmaps = false);

		struct Texture {
			std::string Filepath;
			GLTexture2D Tex;
			uint32_t ReferenceCount;
		};//Texture

		std::vector<Texture*> m_Textures;

	};//STextureMAnager

}//name space


#endif