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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GLWINDOW_H__
#define __CFORGE_GLWINDOW_H__

#include "../Core/CForgeObject.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Core/ITCaller.hpp"

namespace CForge {

	struct GLWindowMsg {
		enum MsgCode {
			MC_RESIZE = 0,
		};

		MsgCode Code;
		void* pHandle;
		Eigen::Vector4i iParam;
		Eigen::Vector4f fParam;
	};

	/**
	* \brief A system window that also provides an OpenGL context. Powered by the GLFW library.
	*
	* \todo Full documentation
	* \todo Think about better names for shutdown and closeWindow
	* \todo Implement callback system for window changes
	*/
	class CFORGE_API GLWindow: public CForgeObject, public ITCaller<GLWindowMsg> {
	public:
		GLWindow(void);
		~GLWindow(void);

		void init(Eigen::Vector2i Position, Eigen::Vector2i Size, std::string WindowTitle, uint32_t Multisample = 0, uint32_t GLMajorVersion = 0, uint32_t GLMinorVersion = 0);
		void clear(void);

		void update(void);
		void swapBuffers(void);

		Eigen::Vector2i position(void)const;
		Eigen::Vector2i size(void)const;

		void position(const int32_t X, const int32_t Y);
		void size(const int32_t Width, const int32_t Height);

		uint32_t width(void)const;
		uint32_t height(void)const;
		void* handle(void)const;

		bool shutdown(void)const;	
		void closeWindow(void);

		void toggleFullscreen(void);
		bool fullscreen(void);

		Keyboard* keyboard(void);
		Mouse* mouse(void);

		std::string title(void)const;
		void title(const std::string Title);

		void setCharacterCallback(class GLFWindow* pWin, uint32_t Codepoint);

		void vsync(bool Enable, int8_t ThrottleFactor = 1);
		bool vsync(int8_t *pThrottleFactor = nullptr)const;

		void makeCurrent(void)const;

		void hideMouseCursor(bool Hide);
		bool isMouseCursorHidden(void)const;

	private:
		struct GLFWwindow *createGLWindow(uint32_t Width, uint32_t Height, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion);

		void* m_pHandle;
		Keyboard m_Keyboard;
		Mouse m_Mouse;
		class SInputManager* m_pInputMan;

		bool m_VSync;
		int8_t m_ThrottleFactor;
		bool m_Fullscreen;

		bool m_MouseHidden;

		Eigen::Vector4i m_WindowPosBackup;

		std::string m_Title; ///< The windows title

		static void sizeCallback(struct GLFWwindow* pHandle, int32_t Width, int32_t Height);
		static std::map<GLWindow*, struct GLFWwindow*> m_WindowList;

	};//GLWindow

}//name space

#endif