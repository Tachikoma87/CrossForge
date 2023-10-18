/*****************************************************************************\
*                                                                           *
* File(s): exampleBirdMenu.hpp                                            *
*                                                                           *
* Content:   *
*                      *
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
#ifndef __CFORGE_EXAMPLEBIRDMENU_HPP__
#define __CFORGE_EXAMPLEBIRDMENU_HPP__


#include "ExampleSceneBase.hpp"
#include <Examples/ExampleMinimumGraphicsSetup.hpp>
#include <Examples/ExampleBirdVR.hpp>
#include <Examples/ExampleCollisionTest.hpp>
#include <Examples/ExampleFlappyBird.hpp>
#include <Examples/ExampleMapBuilderGrid.hpp>

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleBirdMenu : public ExampleSceneBase {
	public:
		ExampleBirdMenu(void) {
			m_WindowTitle = "CrossForge Example - Bird Game Menu";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ExampleBirdMenu(void) {
			clear();
		}//Destructor

		void init() override{

			initWindowAndRenderDevice();
			initCameraAndLights();
			/*
			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}
			*/
		}//initialize

		void clear(void) override{
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear


		void mainLoop(void)override {
			m_RenderWin.update();

			Keyboard* pKeyboard = m_RenderWin.keyboard();
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
				ExampleMinimumGraphicsSetup* mScene = new ExampleMinimumGraphicsSetup();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
				ExampleBird* mScene = new ExampleBird();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
				ExampleFlappyBird* mScene = new ExampleFlappyBird();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
			}
			
			/*
			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderWin.swapBuffers();
			*/
			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());
			/*
			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}*/
		}



	protected:

		bool activeScene = false;

	};//ExampleBirdMenu

}//name space

#endif