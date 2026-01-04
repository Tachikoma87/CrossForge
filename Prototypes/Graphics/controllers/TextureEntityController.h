/*****************************************************************************\
*                                                                           *
* File(s): TextureEntityController.h and TextureEntityController.cpp                       *
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
#ifndef __CROSSFORGE_TEXTUREENTITYCONTROLLER_H__
#define __CROSSFORGE_TEXTUREENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/TextureEntity.h"
#include <crossforge/assetio/entities/Image2DEntity.h>

namespace crossforge {
	class TextureEntityController : public ControllerBase {
	public:
		static inline std::string identification = "TextureEntityController";

		static bool buildTexture2D(TextureEntityPtr pTexture, Image2DEntityPtr pImage, bool generateMitmaps = false);

		~TextureEntityController();
	protected:
		TextureEntityController(const std::string childIdentification);
		
	};

	using TextureEntityControllerPtr = std::shared_ptr<TextureEntityController>;
	using TextureEntityControllerCPtr = std::shared_ptr<const TextureEntityController>;
}

#endif 