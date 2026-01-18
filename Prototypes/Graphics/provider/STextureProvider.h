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

#include <crossforge/graphics/entities/TextureEntity.h>

namespace crossforge {
	class STextureProvider {
	public:	
		enum BasicTexture : int8_t {
			BASIC_TEXTURE_UNKNOWN = -1,
			BASIC_TEXTURE_8X8_RED = 0,
			BASIC_TEXTURE_8X8_GREEN,
			BASIC_TEXTURE_8X8_BLUE,
			BASIC_TEXTURE_8X8_WHITE,
			BASIC_TEXTURE_8X8_BLACK,
			BASIC_TEXTURE_COUNT
		};

		static inline std::string identification = "STextureProvider";
		static std::shared_ptr<STextureProvider> instance();
		static void destroy();

	
		bool registerTexture(std::string textureName, TextureEntityPtr pTexture);
		TextureEntityPtr createTexture(std::string textureName, std::string filename, bool generateMipmaps);
		
		bool isTextureRegistered(std::string textureName)const;
		uint32_t getTextureCount()const;
		TextureEntityPtr getTexture(uint32_t index);
		TextureEntityPtr getTexture(std::string textureName);
		TextureEntityPtr getTexture(BasicTexture basicTexture);
	
		~STextureProvider();

	protected:
		static std::shared_ptr<STextureProvider> m_pInstance;

		STextureProvider();

		TextureEntityPtr generateBasicTexture(BasicTexture basicTexture);

		std::unordered_map<std::string, TextureEntityPtr> m_textureMap ;
	};

	using TextureProvider = STextureProvider;
	using TextureProviderPtr = std::shared_ptr<STextureProvider>;
	using TextureProviderCPtr = std::shared_ptr<const STextureProvider>;
}

#endif 