/*****************************************************************************\
*                                                                           *
* File(s): Image2DEntity.h and Image2DEntity.cpp                            *
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
#ifndef __CROSSFORGE_IMAGE2DENTITY_H__
#define __CROSSFORGE_IMAGE2DENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/RawImage2DDataComponent.h"

namespace crossforge {
	class Image2DEntity : public EntityBase {
	public:
		inline static std::string identification = "Image2DEntity";

		enum Image2DComponents : uint8_t {
			RAW_IMAGE_2D_DATA_COMPONENT = 0x01,
			COMPONENTS_ALL = 0xFF
		};

		Image2DEntity(uint8_t componentBitmask = 0);
		~Image2DEntity();

		void initialize(uint8_t componentBitmask);
		void clear();

		RawImage2DDataComponentptr getRawImage2DDataComponent();

	protected:

	};
	typedef std::shared_ptr<Image2DEntity> Image2DEntityPtr;
}

#endif