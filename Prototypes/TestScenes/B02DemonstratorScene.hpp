/*****************************************************************************\
*                                                                           *
* File(s): B02Demonstartor.hpp                                            *
*                                                                           *
* Content: Demonstrator Scene for B02 project.   *
*                        *
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
#ifndef __CFORGE_B02DEMONSTRATORSCENE_HPP__
#define __CFORGE_B02DEMONSTRATORSCENE_HPP__


#include <Examples/ExampleSceneBase.hpp>
#include <crossforge/Math/Rectangle.hpp>
#include "../ImGuiUtility.h"
#include "../Camera/VideoPlayer.h"
using namespace Eigen;
using namespace std;

namespace CForge {

	class B02DemonstratorScene : public ExampleSceneBase {
	public:
		B02DemonstratorScene(void) {
			m_WindowTitle = "B02 Demonstrator - Disguising Undesired Cues in Motion";
			m_WinWidth = 1280;
			m_WinHeight = 720;

			m_DrawHelpTexts = false;
		}//Constructor

		~B02DemonstratorScene(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice();
			gladLoadGL();
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-15.0f, 15.0f, 25.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			//m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(30.0f, 30.0f, 0.1f, 1000.0f));
			m_Sun.initShadowCasting(1024, 1024, Vector2i(30, 30), 01.f, 1000.0f);
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			initFPSLabel();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);
			initSkybox();

			ImGuiUtility::initImGui(&m_RenderWin);


			m_TitleText.init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 40, true, false), "Text");
			m_TitleText.color(0.0f, 0.0f, 0.0f);
			m_TitleText.text("B02 Demonstrator - Disguising Undesired Cues in Motion");
			m_TitleText.position(m_RenderWin.width() / 2 - m_TitleText.textWidth()/2, 15);

			T3DMesh<float> M;


			T2DImage<uint8_t> Img;
			AssetIO::load("MyAssets/B02Demonstrator/B02StudyPart01.jpg", &Img);
			Img.flipRows();
			m_ImgStudyPart1.init(&Img);

			AssetIO::load("MyAssets/B02Demonstrator/B02StudyPart02.jpg", &Img);
			Img.flipRows();
			m_ImgStudyPart2.init(&Img);

			AssetIO::load("MyAssets/B02Demonstrator/MotionEditor.jpg", &Img);
			Img.flipRows();
			m_ImgMotionEditor.init(&Img);
			
			// create help text
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			m_HelpTexts.push_back(pKeybindings);
			m_DrawHelpTexts = false;

			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}

			// add fonts for ImGui
			ImGuiIO &pImgGuiIo = ImGui::GetIO();
			m_pFontTileHeading = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-SemiBold.ttf", 32);
			m_pFontTileText = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf", 18);

			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());

			int32_t TileWidth = 350;
			m_TileRects[0].init(1 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);
			m_TileRects[1].init(3 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);
			m_TileRects[2].init(5 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);


			// load model to show in background
			SAssetIO::load("MyAssets/B02Demonstrator/ModelNewSkeleton.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";

			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";

			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_CharacterController.init(&M, true);
			m_Character.init(&M, &m_CharacterController);
			BoundingVolume BV;
			m_Character.boundingVolume(BV);
			M.clear();

			// insert in scene graph

			float CharacterScale = 0.025f;
			Quaternionf CharacterRot;
			CharacterRot = Quaternionf::Identity();
			CharacterRot = AngleAxisf(CForgeMath::degToRad(20.0f), Vector3f::UnitY());
			m_CharacterTransformSGN.init(&m_RootSGN);
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(60.0f), Vector3f::UnitY());
			m_CharacterTransformSGN.rotation(R);
			m_CharacterTransformSGN.translation(Vector3f(-20.0f, 0.0f, -30.0f));
			m_CharacterSGN.init(&m_CharacterTransformSGN, &m_Character, Vector3f(0.0f, 0.0f, 15.0f), CharacterRot, Vector3f(CharacterScale, CharacterScale, CharacterScale));

			m_LastFPSPrint = CForgeUtility::timestamp();
			m_FPS = 60.0f;

			m_DemoState = STATE_DASHBOARD;

		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;

			ImGuiUtility::shutdownImGui();
		}//clear

		void drawTextCentered(const std::string Text) {
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text.c_str()).x) / 2.0f);
			ImGui::Text(Text.c_str());
		}

		void updateDashboardGui(void) {
			ImGuiUtility::newFrame();

			bool m_ActiveTool = true;
			float ImgScale = 1.0f;

			int32_t TileWidth = 350;
			int8_t SelectedTile = -1;
			ImVec4 SelectionColor = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);

			// check if a tile is hovered
			Eigen::Vector2f MousePos = m_RenderWin.mouse()->position();
			for (uint8_t i = 0; i < uint8_t(m_TileRects.size()); ++i) {
				if (m_TileRects[i].isPointInside(MousePos)) {
					SelectedTile = i;
				}
			}
			

			// Evaluation of Naturalness
			if(SelectedTile == 0) ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			ImGui::Begin("Evaluation of Naturalness Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(TileWidth, 600));
			ImGui::SetWindowPos(ImVec2(1 * m_RenderWin.width() / 6 - TileWidth/2, 75));
			ImGui::PushFont(m_pFontTileHeading);
			drawTextCentered("Evaluation of Naturalness");
			ImGui::PopFont();
			ImgScale = float(TileWidth-20) / float(m_ImgStudyPart1.width());
			ImGui::Image((void*)(intptr_t)m_ImgStudyPart1.handle(), ImVec2(m_ImgStudyPart1.width()*ImgScale, m_ImgStudyPart1.height()*ImgScale));
			ImGui::PushFont(m_pFontTileText);
			ImGui::Text("Evaluate the naturalness of motion captured\nand synthesized Motions.");
			ImGui::PopFont();
			ImGui::End();
			if(SelectedTile == 0) ImGui::PopStyleColor();

			// Evaluation of Synthesis Quality
			if (SelectedTile == 1) ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			ImGui::Begin("Evaluation of Synthesis Quality Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(TileWidth, 600));
			ImGui::SetWindowPos(ImVec2(3 * m_RenderWin.width() / 6 - TileWidth / 2, 75));
			ImGui::PushFont(m_pFontTileHeading);
			drawTextCentered("Evaluation of Synthesis Quality");
			ImGui::PopFont();
			ImgScale = float(TileWidth-20) / float(m_ImgStudyPart2.width());
			ImGui::Image((void*)(intptr_t)m_ImgStudyPart2.handle(), ImVec2(m_ImgStudyPart2.width()*ImgScale, m_ImgStudyPart2.height()*ImgScale));
			ImGui::PushFont(m_pFontTileText);
			ImGui::Text("Find the matching motion to a given one. Can you\ndistinct between motion captured and synthesized?\nCheck your performance in relation to our study results.");
			ImGui::PopFont();
			ImGui::End();
			if (SelectedTile == 1) ImGui::PopStyleColor();


			// Motion Editor
			if (SelectedTile == 2) ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			ImGui::Begin("Motion Editor Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(TileWidth, 600));
			ImGui::SetWindowPos(ImVec2(5 * m_RenderWin.width() / 6 - TileWidth / 2, 75));
			ImGui::PushFont(m_pFontTileHeading);
			drawTextCentered("Try Our Motion Editor");
			ImGui::PopFont();
			ImgScale = float(TileWidth-20) / float(m_ImgMotionEditor.width());
			ImGui::Image((void*)(intptr_t)m_ImgMotionEditor.handle(), ImVec2(m_ImgMotionEditor.width()*ImgScale, m_ImgMotionEditor.height()*ImgScale));
			ImGui::PushFont(m_pFontTileText);
			ImGui::Text("Use our motion editor to generator gait motions by\nadjusting various parameters.");
			ImGui::PopFont();
			ImGui::End();
			if (SelectedTile == 2) ImGui::PopStyleColor();

			ImGui::EndFrame();

			if (m_RenderWin.mouse()->buttonState(Mouse::BTN_LEFT)) {
				m_RenderWin.mouse()->buttonState(Mouse::BTN_LEFT, false);

				if (SelectedTile == 0) {
					startStudyPart1();
					m_DemoState = STATE_STUDYPART1;
				}

			}

		}//updateGui


		void updateStudyPart1GUI(void) {
			ImGuiUtility::newFrame();

			int32_t FrameWidth = 720;
			int32_t FrameHeight = 500;



			ImVec4 SelectionColor = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			// create selection of naturalness (1 through 5)
			ImGui::Begin("Evaluation Part1 Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos(ImVec2(m_RenderWin.width() / 2 - FrameWidth/2, 400));
			ImGui::SetWindowSize(ImVec2(FrameWidth, FrameHeight));

			ImGui::PushFont(m_pFontTileText);
			ImGui::SetCursorPosX(FrameWidth/2 - ImGui::CalcTextSize("ReplayVideo").x);
			if (ImGui::Button("Replay Video")) m_VideoPlayers[0].play();

			ImGui::SetNextItemOpen(true);
			if (ImGui::TreeNode("How natural or artificial did you find the motion sequence depicted in the video?")) {
				ImGui::Text("Please rate on the scale how natural or artificial the movement of the video looks.");
				ImGui::Separator();

				ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 35);
				ImGui::Image((void*)m_Part1Data.ScaleImg.handle(), ImVec2(250, 50));

				ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 35);
				ImGui::Text("artificial"); ImGui::SameLine(); 
				ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 35 + 250 - ImGui::CalcTextSize("natural").x);
				ImGui::Text("natural");

				ImGui::Text("The movement of the video appears:");

				for (int32_t n = 0; n < 5; ++n) {
					ImGui::SameLine();
					std::string Label = std::to_string(n);
					if (ImGui::Selectable(Label.c_str(), m_Part1Data.Selection == n, 0, ImVec2(50,25))) m_Part1Data.Selection = n;		
				}			

				ImGui::TreePop();
			}
			
			// next button
			ImGui::SetCursorPosX(FrameWidth / 2 - ImGui::CalcTextSize("Next").x);
			if (ImGui::Button("Next")) {
				studyPart1Next();
			}

			ImGui::PopFont();

			ImGui::PopStyleColor();

			ImGui::End();

			ImGui::EndFrame();
		}//updateStudyPart1GUI

		void studyPart1Next(void) {

		}//studyPart1Next

		void mainLoop(void)override {


			switch (m_DemoState) {
			case STATE_DASHBOARD: {
				m_FPS = std::max(0.25f, m_FPS);
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);
				m_SkyboxSG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_CharacterController.update(60.0f / m_FPS);
				if (m_Character.activeAnimation() == nullptr) {
					m_pCharacterAnim = m_CharacterController.createAnimation(0, 1000.0f / 60.0f, 0.0f);
					m_Character.activeAnimation(m_pCharacterAnim);
				}


				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_RenderDev.activeCamera(&m_Cam);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.render(&m_RenderDev);

				m_FPSLabel.render(&m_RenderDev);
				if (m_DrawHelpTexts) drawHelpTexts();

				m_TitleText.render(&m_RenderDev);


				updateDashboardGui();
				ImGuiUtility::render();

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());
			}break;
			case STATE_STUDYPART1: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				//m_SkyboxSG.render(&m_RenderDev);

				m_VideoPlayers[0].update();
				m_VideoPlayers[0].render(&m_RenderDev);

				m_FPSLabel.render(&m_RenderDev);
				updateStudyPart1GUI();
				ImGuiUtility::render();

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if(m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_P, true)) m_VideoPlayers[0].play();

			}break;
			}
			

		}//mainLoop

	protected:

		enum DemonstratorState {
			STATE_DASHBOARD = 0,
			STATE_STUDYPART1,
			STATE_STUDYPART2,
			STATE_MOTIONEDITOR,
		};

		struct StudyPart1Data {
			int32_t Selection;
			GLTexture2D ScaleImg;

			StudyPart1Data(void) {
				Selection = 2;
			}
		};

		void startStudyPart1(void) {
			m_VideoPlayers[0].init(Vector2f(0.25f, 0.05f), Vector2f(0.5f, 0.5f));
			m_VideoPlayers[0].play("MyAssets/Study_Videos/01_1.mp4");

			T2DImage<uint8_t> Img;
			AssetIO::load("MyAssets/B02Demonstrator/Scale.jpg", &Img);
			Img.flipRows();
			m_Part1Data.ScaleImg.init(&Img, false);
		}

		DemonstratorState m_DemoState;

		VideoPlayer m_VideoPlayers[4]; // The 4 required video players

		// Scene Graph
		SGNTransformation m_RootSGN;

		LineOfText m_TitleText;

		std::vector<LineOfText*> TileHeadings;

		std::vector<Rectangle> m_TileRects;

		GLTexture2D m_TexTest;
		GLTexture2D m_ImgStudyPart1;
		GLTexture2D m_ImgStudyPart2;
		GLTexture2D m_ImgMotionEditor;

		ImFont* m_pFontTileHeading;
		ImFont* m_pFontTileText;


		SkeletalActor m_Character;
		SkeletalAnimationController m_CharacterController;
		SkeletalAnimationController::Animation* m_pCharacterAnim;

		SGNGeometry m_CharacterSGN;
		SGNTransformation m_CharacterTransformSGN;
		
		StudyPart1Data m_Part1Data;
	};//ExampleMinimumGraphicsSetup

}//name space

#endif