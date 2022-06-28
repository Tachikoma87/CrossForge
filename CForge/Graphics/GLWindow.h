/*****************************************************************************\
*                                                                           *
* File(s): GLWindow.h and GLWindow.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GLWINDOW_H__
#define __CFORGE_GLWINDOW_H__

#include "../Core/CForgeObject.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Input/Character.h"

namespace CForge {

	/**
	* \brief A system window that also provides an OpenGL context. Powered by the GLFW library.
	*
	* \todo Full documentation
	* \todo Think about better names for shutdown and closeWindow
	* \todo Implement callback system for window changes
	*/
	class CFORGE_API GLWindow: public CForgeObject {
	public:
		GLWindow(void);
		~GLWindow(void);

		void init(Eigen::Vector2i Position, Eigen::Vector2i Size, std::string WindowName, uint32_t GLMajorVersion = 0, uint32_t GLMinorVersion = 0, bool Fullscreen = false);
		void clear(void);

		void update(void);
		void swapBuffers(void);

		uint32_t width(void)const;
		uint32_t height(void)const;
		void* handle(void)const;

		bool shutdown(void);	
		void closeWindow(void);

		Keyboard* keyboard(void);
		Mouse* mouse(void);
		Character* character(void);

		std::string title(void)const;
		void title(const std::string Title);

	private:
		class GLFWwindow *createGLWindow(uint32_t Width, uint32_t Height, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion, bool Fullscreen);

		void* m_pHandle;
		Keyboard m_Keyboard;
		Mouse m_Mouse;
		Character m_Character;
		class SInputManager* m_pInputMan;

		std::string m_Title; ///< The windows title

	};//GLWindow

}//name space

#endif
