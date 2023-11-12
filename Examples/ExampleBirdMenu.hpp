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
#include "Examples/ImGui/ImGuiUtility.h"

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

			m_ActiveTool = true;
			m_DemoWindow = true;

			ImGuiUtility::initImGui(&m_RenderWin);
		}//initialize

		void clear(void) override{
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
			ImGuiUtility::shutdownImGui();
		}//clear


		void mainLoop(void)override {
			m_RenderWin.update();

			
			
			
			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			//m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			//m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			

			
			ImGuiUtility::newFrame();

			//ImGuiUtility::newFrame();
			ImGui::ShowDemoWindow(&m_DemoWindow);

			ImGui::Begin("My first Tool", &m_ActiveTool, ImGuiWindowFlags_MenuBar);
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Open..", "Ctrl+O")) {}
					if (ImGui::MenuItem("Close", "Ctrl+W")) { m_ActiveTool = false; printf("Close\n"); }
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			else {
				SLogger::log("Problem creating ImGui window!");
			}
			ImGui::End();

			//ImGui::Render();
			//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			ImGuiUtility::render();
			

			Keyboard* pKeyboard = m_RenderWin.keyboard();
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
				ExampleMinimumGraphicsSetup* mScene = new ExampleMinimumGraphicsSetup();
				m_RenderWin.closeWindow();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
				init();
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
				ExampleBird* mScene = new ExampleBird();
				m_RenderWin.closeWindow();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
				init();
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
				ExampleFlappyBird* mScene = new ExampleFlappyBird();
				m_RenderWin.closeWindow();
				mScene->init();
				while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
				if (nullptr != mScene) delete mScene;
				mScene = nullptr;
				init();
			}

			m_RenderWin.swapBuffers();
			
			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());
			/*
			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}*/
		}



	protected:
		SGNTransformation m_RootSGN;

		bool m_DemoWindow;
		bool m_ActiveTool;

	};//ExampleBirdMenu

}//name space

#endif