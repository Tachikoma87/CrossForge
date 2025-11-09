/*****************************************************************************\
*                                                                           *
* File(s): VideoDataComponent.h and VideoDataComponent.cpp                                                *
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
#ifndef __CROSSFORGE_VIDEODATACOMPONENT_H__
#define __CROSSFORGE_VIDEODATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class VideoDataComponent : public ComponentBase {
	public:
		static inline std::string identification = "VideoDataComponent";

		VideoDataComponent();
		~VideoDataComponent();

		float& framerate();
		uint32_t& width();
		uint32_t& height();
		std::string& filename();
		bool& isRecording();

	protected:
		VideoDataComponent(const std::string childIdentification);

		float m_framefrate;
		uint32_t m_width;
		uint32_t m_height;
		std::string m_filename;
		bool m_isRecording;
	};

	typedef std::shared_ptr<VideoDataComponent> VideoDataComponentPtr;
}

#endif 
