/*****************************************************************************\
*                                                                           *
* File(s): WindowPropertiesComponent.h and WindowPropertiesComponent.cpp            *
*                                                                           *
* Content:                            *
*                                                                           *
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
#ifndef __CFORGE_WindowPropertiesComponent_H__
#define __CFORGE_WindowPropertiesComponent_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/ComponentBase.h"

namespace CForge {
	class WindowPropertiesComponent : public ComponentBase {
	public:
		inline static std::string identification = "WindowPropertiesComponent";

		WindowPropertiesComponent();
		~WindowPropertiesComponent();

		void initialize();
		void clear();

		uint32_t& width();
		uint32_t& height();
		std::string& title();
		Eigen::Vector2i position();

		uint32_t &glMinorVersion();
		uint32_t& glMajorVersion();
		uint32_t &multisampling();
		uint32_t vsync();

		void* getGlfwWindow();
		void setGlfwWindow(void* pGlfwWindow);

	protected:
		uint32_t m_width;
		uint32_t m_height;
		std::string m_title;
		Eigen::Vector2i m_position;

		uint32_t m_glMinorVersion;
		uint32_t m_glMajorVersion;
		uint32_t m_multisampling;
		uint32_t m_vsync;

		void* m_pGlfwWindow;
	};

	typedef std::shared_ptr<WindowPropertiesComponent> WindowPropertiesComponentPtr;
}

#endif 