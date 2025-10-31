/*****************************************************************************\
*                                                                           *
* File(s): STextureProvider.h and STextureProvider.cpp     *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_STEXTUREPROVIDER_H__
#define __CROSSFORGE_STEXTUREPROVIDER_H__

#include <crossforge/core/CoreDefinitions.h>
#include <crossforge/assetio/entities/Image2DEntity.h>
#include "../entities/TextureEntity.h"

namespace crossforge {
	class STextureProvider {
	public:	
		static inline std::string identification = "STextureProvider";
		static std::shared_ptr<STextureProvider> instance();
		static void destroy();

	
		bool registerTexture(std::string textureName, TextureEntityPtr pTexture);
		TextureEntityPtr createTexture(std::string textureName, std::string filename, bool generateMipmaps);
		
		bool isTextureRegistered(std::string textureName)const;
		uint32_t getTextureCount()const;
		TextureEntityPtr getTexture(uint32_t index);
		TextureEntityPtr getTexture(std::string textureName);
	
		~STextureProvider();

	protected:
		static std::shared_ptr<STextureProvider> m_pInstance;

		STextureProvider();

		std::unordered_map<std::string, TextureEntityPtr> m_textureMap ;

	};

	typedef STextureProvider TextureProvider;
	typedef std::shared_ptr<STextureProvider> TextureProviderPtr;
}

#endif 