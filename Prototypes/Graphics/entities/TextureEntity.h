/*****************************************************************************\
*                                                                           *
* File(s): TextureEntity.h and TextureEntity.cpp                       *
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
#ifndef __CROSSFORGE_TEXTUREENTITY_H__
#define __CROSSFORGE_TEXTUREENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/Texture2DComponent.h"

namespace crossforge {
	class TextureEntity : public EntityBase {
	public:
		static inline std::string identification = "TextureEntity";

		enum TextureEntityCompnents {
			COMPONENT_TEXUTRE2D = 0x01,
			COMPONENTS_ALL = 0xFF
		};

		TextureEntity(uint8_t componentsMask = 0);
		~TextureEntity();

		void initialize(uint8_t componentsMask);
		void clear();

		Texture2DComponentPtr getTexture2DComponent();


	protected:
		TextureEntity(const std::string childIdentification);

	};

	typedef std::shared_ptr<TextureEntity> TextureEntityPtr;
}

#endif