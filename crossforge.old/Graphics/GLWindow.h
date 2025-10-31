/*****************************************************************************\
*                                                                           *
* File(s): GLWindow.h and GLWindow.cpp                                      *
*                                                                           *
* Content: A system window that also provides and OpenGL context.           *
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
#ifndef __CFORGE_GLWINDOW_H__
#define __CFORGE_GLWINDOW_H__

#include "../Core/CForgeObject.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Core/ITCaller.hpp"

namespace CForge {

	/**
	* \brief Message a windows sens to listener.
	* 
	* \todo Change handle to GLWindow* or object ID
	*/
	struct GLWindowMsg {

		/**
		* \brief Possible message codes.
		* \ingroup Graphics
		*/
		enum MsgCode {
			MC_RESIZE = 0,	///< Window resized.
		};

		MsgCode Code;			///< Type of the message.
		void* pHandle;			///< Window handle from the sending window.
		Eigen::Vector4i iParam;	///< iParam, content depends on message type.
		Eigen::Vector4f fParam;	///< fParam, content depends on message type.
	};

	/**
	* \brief A system window that also provides an OpenGL context. Powered by the GLFW library.
	* \ingroup Graphics
	* 
	* \todo Think about better names for shutdown and closeWindow
	* \todo Implement callback system for window changes
	* \todo Check constructor
	* \todo Add vector versions of the setters.
	* \todo Add functionality so window is maximized on correct monitor.
	* \todo Change pass by pointer to pass by reference.
	*/
	class CFORGE_API GLWindow: public CForgeObject, public ITCaller<GLWindowMsg> {
	public:
		/**
		* \brief Constructor
		*/
		GLWindow(void);

		/**
		* \brief Destructor
		*/
		~GLWindow(void);

		/**
		* \brief Initialization method.
		* 
		* \param[in] Position The window's position relative to its parent (usually the desktop).
		* \param[in] Size The window's size in pixel.
		* \param[in] Whether or not the windows should support multisampling.
		* \param[in] GLMajorVersion Major version of OpenGL. Latest will be chosen if parameter is 0.
		* \param[in] GLMinorVersion Minor version of OpenGL. Latest will be chosen if parameter is 0.
		*/
		void init(Eigen::Vector2i Position, Eigen::Vector2i Size, std::string WindowTitle, uint32_t Multisample = 0, uint32_t GLMajorVersion = 0, uint32_t GLMinorVersion = 0);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Update the window and process events. Should be called once per main cycle.
		*/
		void update(void);

		/**
		* \brief Swaps buffers to display the generated image.
		*/
		void swapBuffers(void);

		/**
		* \brief Getter for the position of the window.
		* 
		* \return Position of the window.
		*/
		Eigen::Vector2i position(void)const;

		/**
		* \brief Getter for the size of the window.
		* 
		* \return Size of the window.
		*/
		Eigen::Vector2i size(void)const;

		/**
		* \brief Setter for the position of the window.
		* 
		* \param[in] X New x position.
		* \param[in] Y new y position.
		*/
		void position(const int32_t X, const int32_t Y);

		/**
		* \brief Setter for the size of the window.
		* 
		* \param[in] Width New width.
		* \param[in] Height New height.
		*/
		void size(const int32_t Width, const int32_t Height);


		/**
		* \brief Getter for the width.
		* 
		* \return Width of the window.
		*/
		uint32_t width(void)const;

		/**
		* \brief Getter for the height.
		* 
		* \return Height of the window.
		*/
		uint32_t height(void)const;

		/**
		* \brief Getter for the glfw window handle. Be careful what you do with it!
		* 
		* \return The glfw window handle of type GLFWwindow.
		*/
		void* handle(void)const;

		/**
		* \brief Returns whether the window is about to close.
		* 
		* \return Whether the window is about to close.
		*/
		bool shutdown(void)const;	

		/**
		* \brief Closes the window.
		*/
		void closeWindow(void);

		/**
		* \brief Switches to fullscreen if in windowed mode and vice versa.
		* 
		* \note Fullscreen means maximized borderless window.
		*/
		void toggleFullscreen(void);

		/**
		* \brief Returns whether window is in fullscreen mode not.
		* 
		* \return True if in fullscreen mode, false otherwise.
		*/
		bool fullscreen(void);

		/**
		* \brief Getter for the associated keyboard.
		* 
		* \return Associated keyboard.
		*/
		Keyboard* keyboard(void);

		/**
		* \brief Getter for the associated mouse.
		* 
		* \return Associated mouse.
		*/
		Mouse* mouse(void);

		/**
		* \brief Getter for the window title.
		* 
		* \return The window title.
		*/
		std::string title(void)const;

		/**
		* \brief Setter for the window title.
		* 
		* \param[in] Title The new window title.
		*/
		void title(const std::string Title);

		/**
		* \brief Sets the vertical synchronization option.
		* 
		* \param[in] Enable Whether or not to enable vsync.
		* \param[in] ThrottleFactor Number of screen refreshes to wait before the next frame is shown. 
		*/
		void vsync(bool Enable, int8_t ThrottleFactor = 1);

		/**
		* \brief Getter for vertical synchronization.
		* 
		* \param[out] pThrottleFactor Stores the throttle factor. Can be nullptr.
		* \return True if vync is enabled, false otherwise.
		*/
		bool vsync(int8_t *pThrottleFactor = nullptr)const;

		/**
		* \brief Makes the OpenGL context of this window the current context.
		*/
		void makeCurrent(void)const;

		/**
		* \brief Set whether mouse cursor is hidden or not.
		* 
		* \para[in] Hide If true mouse cursor will be hidden and shown if set to false.
		*/
		void hideMouseCursor(bool Hide);

		/**
		* \brief Returns whether the mouse cursor is hidden.
		* 
		* \return True if the mouse cursor is hidden, false otherwise.
		*/
		bool isMouseCursorHidden(void)const;

	private:

		/**
		* \brief Utility method to create a glfw window.
		* 
		* \param[in] Width Initial width of the window.
		* \param[in] Height Initial height of the window.
		* \param[in] Title Initial title of the window.
		* \param[in] GLMajorVersion OpenGL major version. Latest if 0.
		* \param[in] GLMinorVersion OpenGL minor version. Latest if 0.
		* 
		* \return Handle to the created window.
		*/
		struct GLFWwindow *createGLWindow(uint32_t Width, uint32_t Height, std::string Title, uint32_t GLMajorVersion, uint32_t GLMinorVersion);

		void* m_pHandle;		///< Glfw window handle.
		Keyboard m_Keyboard;	///< Associated keyboard.
		Mouse m_Mouse;			///< Associated mouse.
		class SInputManager* m_pInputMan;	///< Input manager instance.

		bool m_VSync;				///< Vertical synchronization state.
		int8_t m_ThrottleFactor;	///< Vsync throttle factor.
		bool m_Fullscreen;			///< Whether window is in full screen mode.

		bool m_MouseHidden;			///< Whether mouse is hidden or visible.

		Eigen::Vector4i m_WindowPosBackup;	///< Backup position and size for full screen toggle.

		std::string m_Title;	///< The windows title.

		/**
		* \brief Size change callback from glfw.
		* 
		* \param[in] pHandle Glfw window handle, i.e. the window that has changed.
		* \param[in] Width New window width.
		* \param[in] Height New window height.
		*/
		static void sizeCallback(struct GLFWwindow* pHandle, int32_t Width, int32_t Height);

		static std::map<GLWindow*, struct GLFWwindow*> m_WindowList;	///< Singleton list of active windows.

	};//GLWindow

}//name space

#endif