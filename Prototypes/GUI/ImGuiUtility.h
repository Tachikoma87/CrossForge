/*****************************************************************************\
*                                                                           *
* File(s): ImGuiUtility.h and ImGuiUtility.cpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_IMGUIUTILITY_H__
#define __CFORGE_IMGUIUTILITY_H__

#include <crossforge/Graphics/GLWindow.h>
#include <imgui.h>

namespace CForge {
	class ImGuiUtility {
	public:
		static void initImGui(GLWindow* pWindow);
		static void newFrame(void);
		static void render(void);
		static void shutdownImGui(void);

	protected:
		ImGuiUtility(void);
		~ImGuiUtility(void);
	};

}//name-space

#endif