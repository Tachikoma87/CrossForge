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
#include <Examples/ExampleFlappyBirdCollision.hpp>
#include <Examples/ExampleMapBuilderGrid.hpp>
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
			AssetIO::load("MyAssets/Menu/example.png", &ExampleImage);
			AssetIO::load("MyAssets/Menu/bird_example.png", &BirdImage);
			AssetIO::load("MyAssets/Menu/flappy_sample.png", &FlappyImage);
			AssetIO::load("MyAssets/Menu/MapBuilder1.png", &MapBuilderImage);


			ExampleTexture.init(&ExampleImage);

			BirdTexture.init(&BirdImage);

			FlappyTexture.init(&FlappyImage);

			MapBuilderTexture.init(&MapBuilderImage);

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

		void TextCentered(std::string text) {
			auto windowWidth = ImGui::GetContentRegionAvail().x;
			auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			ImGui::Text(text.c_str());
		}

		void handleGUI(void) {
			ImGuiUtility::newFrame();

			//ImGuiUtility::newFrame();
			//ImGui::ShowDemoWindow(&m_DemoWindow);
			ImGui::Begin("My first Tool", &m_ActiveTool, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
			ImGui::SetWindowSize(ImVec2(m_RenderWin.width(), m_RenderWin.height()));
			ImVec2 image_size{ 300, 200 };
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
			TextCentered("Start any of the following examples");

			ImGui::NewLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 3) - (image_size.x * 2 / 3));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
			if (ImGui::ImageButton((void*)ExampleTexture.handle(), image_size, ImVec2(0, 1), ImVec2(1, 0))) {
				ImGui::PopStyleVar();
				startExample();
				//ImGui::End();
				return;
			}
			ImGui::SameLine((ImGui::GetContentRegionAvail().x * 2 / 3) - (image_size.x / 3));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
			if (ImGui::ImageButton((void*)BirdTexture.handle(), image_size, ImVec2(0, 1), ImVec2(1, 0))) {
				ImGui::PopStyleVar();
				startBirdVR();
				//ImGui::End();
				return;
			}

			ImGui::NewLine();
			string text = "Example";
			auto textwidth = ImGui::CalcTextSize(text.c_str()).x;
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 3) - (image_size.x / 6) - (textwidth / 2));
			ImGui::Text("Example");

			text = "Bird";
			textwidth = ImGui::CalcTextSize(text.c_str()).x;
			ImGui::SameLine((ImGui::GetContentRegionAvail().x * 2 / 3) + (image_size.x / 6) - (textwidth / 2));
			ImGui::Text("Bird");

			ImGui::NewLine();
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 3) - (image_size.x * 2 / 3));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 40);
			if (ImGui::ImageButton((void*)FlappyTexture.handle(), image_size, ImVec2(0, 1), ImVec2(1, 0))) {
				ImGui::PopStyleVar();
				startFlappy();
				//ImGui::End();
				return;
			}
			ImGui::SameLine((ImGui::GetContentRegionAvail().x * 2 / 3) - (image_size.x / 3));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 40); 
			if (ImGui::ImageButton((void*)MapBuilderTexture.handle(), image_size, ImVec2(0, 1), ImVec2(1, 0))) {
				ImGui::PopStyleVar();
				startBuilder();
				//ImGui::End();
				return;
			}

			ImGui::NewLine();
			text = "Flappy";
			textwidth = ImGui::CalcTextSize(text.c_str()).x;
			ImGui::SameLine((ImGui::GetContentRegionAvail().x / 3) - (image_size.x / 6) - (textwidth / 2));
			ImGui::Text("Flappy");

			text = "Map-Builder";
			textwidth = ImGui::CalcTextSize(text.c_str()).x;
			ImGui::SameLine((ImGui::GetContentRegionAvail().x * 2 / 3) + (image_size.x / 6) - (textwidth / 2));
			ImGui::Text("Map-Builder");

			ImGui::PopStyleVar();

			ImGui::End();
			//ImGui::Render();
			//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			ImGuiUtility::render();

			return;
		}

		void startExample(void) {
			ExampleMinimumGraphicsSetup* mScene = new ExampleMinimumGraphicsSetup();
			m_RenderWin.closeWindow();
			ImGuiUtility::shutdownImGui();
			SShaderManager::instance()->reset();
			mScene->init();
			while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
			if (nullptr != mScene) delete mScene;
			mScene = nullptr;
			init();
			return;
		}

		void startBirdVR(void) {
			ExampleBird* mScene = new ExampleBird();
			m_RenderWin.closeWindow();
			ImGuiUtility::shutdownImGui();
			SShaderManager::instance()->reset();
			mScene->init();
			while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
			if (nullptr != mScene) delete mScene;
			mScene = nullptr;
			init();
			return;
		}

		void startFlappy(void) {
			ExampleFlappyBirdCollision* mScene = new ExampleFlappyBirdCollision();
			m_RenderWin.closeWindow();
			ImGuiUtility::shutdownImGui();
			SShaderManager::instance()->reset();
			mScene->init();
			while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
			if (nullptr != mScene) delete mScene;
			mScene = nullptr;
			init();
			return;
		}

		void startBuilder(void) {
			ExampleMapBuilderGrid* mScene = new ExampleMapBuilderGrid();
			m_RenderWin.closeWindow();
			ImGuiUtility::shutdownImGui();
			SShaderManager::instance()->reset();
			mScene->init();
			while (!mScene->renderWindow()->shutdown()) mScene->mainLoop();
			if (nullptr != mScene) delete mScene;
			mScene = nullptr;
			init();
			return;
		}

		void mainLoop(void)override {
			m_RenderWin.update();

			
			
			
			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			//m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			//m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			
			handleGUI();

			Keyboard* pKeyboard = m_RenderWin.keyboard();
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
				startExample();
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
				startBirdVR();
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
				startFlappy();
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) {
				startBuilder();
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

		T2DImage<uint8_t> ExampleImage;
		T2DImage<uint8_t> BirdImage;
		T2DImage<uint8_t> FlappyImage;
		T2DImage<uint8_t> MapBuilderImage;

		GLTexture2D ExampleTexture;
		GLTexture2D BirdTexture;
		GLTexture2D FlappyTexture;
		GLTexture2D MapBuilderTexture;

		bool m_DemoWindow;
		bool m_ActiveTool;

	};//ExampleBirdMenu

}//name space

#endif