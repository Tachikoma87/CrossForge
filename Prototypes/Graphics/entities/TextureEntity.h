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

		TextureEntity();
		~TextureEntity();

		void initialize();
		void clear();

		Texture2DComponentPtr getTexture2DComponent(bool createIfNotExists = false);

	protected:
		TextureEntity(const std::string childIdentification);

	};

	using TextureEntityPtr = std::shared_ptr<TextureEntity>;
	using TextureEntityCPtr = std::shared_ptr<const TextureEntity>;
}

#endif