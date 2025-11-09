/*****************************************************************************\
*                                                                           *
* File(s): VideoEntity.h and VideoEntity.cpp                                                *
*                                                                           *
* Content:           *
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
#ifndef __CROSSFORGE_VIDEOENTITY_H__
#define __CROSSFORGE_VIDEOENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/VideoDataComponent.h"

namespace crossforge {
	class VideoEntity : public EntityBase {
	public:
		static inline std::string identification = "VideoEntity";

		enum VideoEntityComponents: uint8_t {
			COMPONENT_VIDEO_DATA = 0x01,
			COMPONENTS_ALL = 0xFF,
		};

		VideoEntity(uint8_t componentsMap = 0);
		~VideoEntity();

		void initialize(uint8_t componentsMap);
		void clear();

		VideoDataComponentPtr getVideoDataComponent();

	protected:
		VideoEntity(const std::string childIdentification);
	};

	typedef std::shared_ptr<VideoEntity> VideoEntityPtr;
}

#endif 