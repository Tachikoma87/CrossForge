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
#include "../../Prototypes/GUI/ImGuiUtility.h"
#include "../../Prototypes/Camera/VideoPlayer.h"
#include <crossforge/AssetIO/VideoRecorder.h>
#include "AdaptiveSkeletalActor.h"
using namespace Eigen;
using namespace std;

namespace CForge {

	class B02DemonstratorScene : public ExampleSceneBase {
	public:
		B02DemonstratorScene(void) {
			m_WindowTitle = "Synthesizing Gait Movements using Motion Signatures";
			m_WinWidth = 1280;
			m_WinHeight = 720;

			m_DrawHelpTexts = false;

			m_DrawFPSLabel = false;

			m_TrialCountPart1 = 6;
			m_TrialCountPart2 = 5;

			m_pSimulation = nullptr;
			m_DrawTitle = true;
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
			Vector3f SunPos = Vector3f(15.0f, 35.0f, 25.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			//m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(30.0f, 30.0f, 0.1f, 1000.0f));
			int32_t ShadowMapScale = 2;
			m_Sun.initShadowCasting(1024 * ShadowMapScale, 1024*ShadowMapScale, Vector2i(35, 35), 01.f, 1000.0f);
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			initFPSLabel();

			CForgeMath::randSeed(CForgeUtility::timestamp() * CForgeUtility::timestamp());
			m_pSimulation = CForgeSimulation::instance();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);
			initSkybox();

			ImGuiUtility::initImGui(&m_RenderWin);

			m_MaleVideos.push_back("01_1.mp4");
			m_MaleVideos.push_back("01_2.mp4");
			m_MaleVideos.push_back("03_1.mp4");
			m_MaleVideos.push_back("03_2.mp4");
			m_MaleVideos.push_back("04_1.mp4");
			m_MaleVideos.push_back("04_2.mp4");
			m_MaleVideos.push_back("07_1.mp4");
			m_MaleVideos.push_back("07_2.mp4");
			m_MaleVideos.push_back("09_1.mp4");
			m_MaleVideos.push_back("09_2.mp4");
			m_MaleVideos.push_back("11_1.mp4");
			m_MaleVideos.push_back("11_2.mp4");

			m_FemaleVideos.push_back("02_1.mp4");
			m_FemaleVideos.push_back("02_2.mp4");
			m_FemaleVideos.push_back("05_1.mp4");
			m_FemaleVideos.push_back("05_2.mp4");
			m_FemaleVideos.push_back("06_1.mp4");
			m_FemaleVideos.push_back("06_2.mp4");
			m_FemaleVideos.push_back("08_1.mp4");
			m_FemaleVideos.push_back("08_2.mp4");
			m_FemaleVideos.push_back("10_1.mp4");
			m_FemaleVideos.push_back("10_2.mp4");
			m_FemaleVideos.push_back("12_1.mp4");
			m_FemaleVideos.push_back("12_2.mp4");


			m_TitleText.init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SERIF, 40, true, false), "Text");
			m_TitleText.color(0.0f, 0.0f, 0.0f);
			m_TitleText.text("Synthesizing Gait Movements using Motion Signatures");
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
			m_pFontTileText = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf", 24);
			m_pFontStudyTitleLabel = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf", 40);

			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());


			alignDashboardTiles();

			initDisplayModels();

			// create background characters
			for (uint32_t i = 0; i < 30; ++i) {
				Actor* pA = new Actor();

				pA->TransformSGN.init(&m_RootSGN);
				pA->GeomSGN.init(&pA->TransformSGN, nullptr);
				pA->GeomSGN.scale(Vector3f(0.025f, 0.025f, 0.025f));
				m_BackgroundCharacters.push_back(pA);

			}//for[characters]
			

			m_LastFPSPrint = CForgeUtility::timestamp();
			m_FPS = 60.0f;
			m_LastCharacterSpawn = CForgeUtility::timestamp();
			m_LastInteraction = CForgeUtility::timestamp();

			// set study videos
			std::string BasePath = "MyAssets/B02Demonstrator/Study_Videos/";

			for (uint8_t i = 0; i < 12; ++i) {
				m_StudyVideos.push_back(BasePath + ((i < 9) ? "0" : "") + std::to_string(i + 1) + "_1.mp4");
				m_StudyVideos.push_back(BasePath + ((i < 9) ? "0" : "") + std::to_string(i + 1) + "_2.mp4");
			}

			m_DemoState = STATE_DASHBOARD;

		}//initialize

		void updateBackgroundCharacters(void) {

			for (auto i : m_BackgroundCharacters) {
				i->AnimationController.update(std::max(60.0f / m_FPS, 0.5f));
				if (i->Character.activeAnimation() == nullptr || i->Character.activeAnimation()->Finished) i->GeomSGN.enable(true, false);
			}//for[background characters]

			// try to spawn new character?

			if (m_pSimulation->timestamp() - m_LastCharacterSpawn > 1250) {

				for (auto i : m_BackgroundCharacters) {
					if (i->Character.activeAnimation() == nullptr || i->Character.activeAnimation()->Finished) {
						// need to spawn character
						std::string File = string("MyAssets/B02Demonstrator/GeneratedMotions/") + ((CForgeMath::rand() % 2) ? "m" : "f") + string("_") + std::to_string(CForgeMath::rand() % 11) + ".bvh";

						float OffsetRange = 4.0f;

						Vector3f Pos = Vector3f(-25.0f, 0.0f, -20.0f);
						Pos += Vector3f(CForgeMath::randRange(-OffsetRange, OffsetRange), 0.0f, CForgeMath::randRange(-OffsetRange, OffsetRange));
						Quaternionf Rot = Quaternionf::Identity();
						Rot = AngleAxisf(CForgeMath::degToRad(60.0f + CForgeMath::randRange(-5.0f, 5.0f)), Vector3f::UnitY());

						//int32_t Z = CForgeMath::randRange(0, 50000);
						//uint64_t Z = CForgeMath::rand();
						
						//i->DisplayModel = (CForgeMath::rand() % 3);
						//if (Z%5 == 0) i->DisplayModel = 2;

						i->DisplayModel = rand() % 3;

						spawnCharacter(i, File, Pos, Rot);
						i->GeomSGN.enable(true, true);

						m_LastCharacterSpawn = m_pSimulation->timestamp();
						break;
					}
				}//for[background characters
			}//if[spawn chacter]


		}//updateCharacterEmiter

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;

			ImGuiUtility::shutdownImGui();
		}//clear

		void alignDashboardTiles(void) {
			int32_t TileWidth = 375; // std::min(375.0f, m_RenderWin.width() * 0.25f);
			m_TileRects[0].init(1 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);
			m_TileRects[1].init(3 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);
			m_TileRects[2].init(5 * m_RenderWin.width() / 6 - TileWidth / 2, 75, TileWidth, 600);
		}//alignDashboardTiles

		void drawTextCentered(const std::string Text) {
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text.c_str()).x) / 2.0f);
			ImGui::Text(Text.c_str());
		}//drawTextCentered

		void updateDashboardGui(void) {
			ImGuiUtility::newFrame();

			bool m_ActiveTool = true;
			float ImgScale = 1.0f;

			int32_t TileWidth = m_TileRects[0].width();
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
			ImgScale = float(std::min(375, TileWidth-20)) / float(m_ImgStudyPart1.width());
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
			ImgScale = float(std::min(375, TileWidth-20)) / float(m_ImgStudyPart2.width());
			ImGui::Image((void*)(intptr_t)m_ImgStudyPart2.handle(), ImVec2(m_ImgStudyPart2.width()*ImgScale, m_ImgStudyPart2.height()*ImgScale));
			ImGui::PushFont(m_pFontTileText);
			ImGui::Text("Find the matching motion to a given one.\nCan you distinct between motion captured\nand synthesized? Check your performance\nin relation to our study results.");
			ImGui::PopFont();
			ImGui::End();
			if (SelectedTile == 1) ImGui::PopStyleColor();


			//// Motion Editor
			if (SelectedTile == 2) ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			ImGui::Begin("Motion Editor Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowSize(ImVec2(TileWidth, 600));
			ImGui::SetWindowPos(ImVec2(5 * m_RenderWin.width() / 6 - TileWidth / 2, 75));
			ImGui::PushFont(m_pFontTileHeading);
			drawTextCentered("Gait Motion Editor");
			ImGui::PopFont();
			ImgScale = float(std::min(375, TileWidth-20)) / float(m_ImgMotionEditor.width());
			ImGui::Image((void*)(intptr_t)m_ImgMotionEditor.handle(), ImVec2(m_ImgMotionEditor.width()*ImgScale, m_ImgMotionEditor.height()*ImgScale));
			ImGui::PushFont(m_pFontTileText);
			ImGui::Text("Try our motion editor to generate gait\nmotions by adjusting various parameters\nand explore motion signatures.");
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
				else if (SelectedTile == 1) {
					startStudyPart2();
					m_DemoState = STATE_STUDYPART2;
				}
				else if (SelectedTile == 2) {
					startMotionEditor();
				}

			}

		}//updateGui


		void updateStudyPart1GUI(void) {
			ImGuiUtility::newFrame();

			int32_t FrameWidth = std::max(800.0f, m_VideoPlayers[0].size().x() + 50);
			int32_t FrameHeight = m_RenderWin.height();
			//ImVec4 BGColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			ImVec4 BGColor = ImVec4(222.0f / 255.0f, 230.0f / 255.0f, 243.0f / 255.0f, 1.0f);
			ImVec4 TextColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, TextColor);

			if (m_Part1Data.ExperimentRunning) {
				ImVec4 SelectionColor = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, BGColor);
				// create selection of naturalness (1 through 5)
				ImGui::Begin("Evaluation Part1 Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
				ImGui::SetWindowPos(ImVec2(m_RenderWin.width() / 2 - FrameWidth / 2, 0));
				ImGui::SetWindowSize(ImVec2(FrameWidth, FrameHeight));

				std::string TitleLabel = "Evaluation of Naturalness (" + std::to_string(m_Part1Data.CurrentItem+1) + "/" + std::to_string(m_Part1Data.ExperimentData.size()) + ")";
				ImGui::PushFont(m_pFontStudyTitleLabel);
				drawTextCentered(TitleLabel);

				ImGui::SetCursorPosX(FrameWidth / 2 - ImGui::CalcTextSize("ReplayVideo").x/2);
				ImGui::SetCursorPosY(m_VideoPlayers[0].position().y() + m_VideoPlayers[0].size().y() + 25);
				ImGui::PopFont();

				ImGui::PushFont(m_pFontTileHeading);
				if (ImGui::Button("Replay Video")) m_VideoPlayers[0].play();
				ImGui::PopFont();
				ImGui::Separator();

				ImGui::PushFont(m_pFontTileText);
				ImGui::SetNextItemOpen(true);
				drawTextCentered("How natural or artificial did you find the motion sequence depicted in the video?");
				drawTextCentered("Please rate on the scale how natural or artificial the movement of the video looks.");
				ImGui::Separator();

				float Offset = ImGui::CalcTextSize("The movement of the video appears:").x;
				float CenteringOffset = (ImGui::GetWindowWidth() - Offset - 275) / 2.0f;

				ImGui::SetCursorPosX(CenteringOffset + Offset - 10);
				ImGui::Image((ImTextureID)m_Part1Data.ScaleImg.handle(), ImVec2(275, 50));

				ImGui::SetCursorPosX(CenteringOffset + Offset + 20 - 10);
				ImGui::Text("artificial"); ImGui::SameLine();
				ImGui::SetCursorPosX(CenteringOffset + Offset + 275 - ImGui::CalcTextSize("natural").x - 25);
				ImGui::Text("natural");

				ImGui::SetCursorPosX(CenteringOffset);
				ImGui::Text("The movement of the video appears:");
				//ImGui::SetCursorPosX(CenteringOffset + ImGui::CalcTextSize("The movement of the video appears:").x + 25 );

				for (int32_t n = 0; n < 5; ++n) {
					ImGui::SameLine();
					std::string Label = std::to_string(n);
					if (ImGui::Selectable(Label.c_str(), m_Part1Data.Selection == n, 0, ImVec2(50, 25))) m_Part1Data.Selection = n;
				}
	
				ImGui::PopFont();
				ImGui::Separator();
				// next button
				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Next").x) / 2);
				if (ImGui::Button("Next")) {
					studyPart1Next();
				}

				ImGui::PopFont();
				ImGui::PopStyleColor();
				ImGui::End();
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_WindowBg, BGColor);
				ImGui::Begin("Experiment finished", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
				ImGui::SetWindowPos(ImVec2(m_RenderWin.width() / 2 - FrameWidth / 2, 0));
				ImGui::SetWindowSize(ImVec2(FrameWidth, FrameHeight));

				std::string TitleLabel = "Experiment Finished";
				ImGui::PushFont(m_pFontStudyTitleLabel);
				drawTextCentered(TitleLabel);
				ImGui::PopFont();
				ImGui::Separator();

				ImGui::PushFont(m_pFontTileText);

				char Buff[64];
				sprintf(Buff, "%.3f", m_Part1Data.AverageUserScoreSynthetic);

				//std::string Result = "You gave an average rating of " + std::string(Buff) + ". The average rating in our study was 2.024.";
				//drawTextCentered(Result.c_str());
				

				// you gave the following rankings
				// original motions   | synthetic motions

				std::string Result = "You gave the videos the following rating:\n\n";
				Result += "Original Motions \t\t\t\t\t\tSynthetic Motions\n";
				for (int32_t i = 0; i < m_Part1Data.OriginalRatings.size(); ++i) {
					Result += "Video " + std::to_string(m_Part1Data.OriginalRatings[i].first) + ": " + std::to_string(int32_t(m_Part1Data.OriginalRatings[i].second)) + "\t\t\t\t\t\t\t\t\t\tVideo " + std::to_string(m_Part1Data.SyntheticRatings[i].first) + ": " + std::to_string(int32_t(m_Part1Data.SyntheticRatings[i].second)) + "\n";
				}//
				char Buf1[64];
				char Buf2[64];
				sprintf(Buf1, "%.2f", m_Part1Data.AverageUserScoreOriginal);
				sprintf(Buf2, "%.2f", m_Part1Data.AverageUserScoreSynthetic);
				Result += "\n";
				Result += "Average (you): " + std::string(Buf1) + "\t\t\t\t\tAverage(you): " + std::string(Buf2) + "\n";
				Result += "Average (study): 2.02 \t\t\t   Average(study): 2.05";


				drawTextCentered(Result.c_str());
				ImGui::PopFont();

				ImGui::Separator();
				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX( (ImGui::GetWindowWidth()- ImGui::CalcTextSize("Return to Dashboard").x)/2);
				if (ImGui::Button("Return to Dashboard")) m_DemoState = STATE_DASHBOARD;
				ImGui::PopFont();

				ImGui::PopStyleColor();

			}

			ImGui::PopStyleColor();

			ImGui::EndFrame();
		}//updateStudyPart1GUI

		void updateStudyPart2GUI(void) {
			ImGuiUtility::newFrame();

			int32_t FrameWidth = m_RenderWin.width(); // std::max(1000.0f, m_VideoPlayers[0].size().x() * 3 + 50);
			int32_t FrameHeight = m_RenderWin.height();
			ImVec4 BGColor = ImVec4(222.0f / 255.0f, 230.0f / 255.0f, 243.0f / 255.0f, 1.0f);

			ImVec4 TextColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, TextColor);

			if (m_Part2Data.ExperimentRunning) {
				ImVec4 SelectionColor = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, BGColor);
				// create selection of naturalness (1 through 5)
				ImGui::Begin("Evaluation Part1 Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
				ImGui::SetWindowPos(ImVec2(m_RenderWin.width() / 2 - FrameWidth / 2, 0));
				ImGui::SetWindowSize(ImVec2(FrameWidth, FrameHeight));

				std::string TitleLabel = "Evaluation of Synthesis Quality (" + std::to_string(m_Part2Data.CurrentItem + 1) + "/" + std::to_string(m_Part2Data.ExperimentData.size()) + ")";
				ImGui::PushFont(m_pFontStudyTitleLabel);
				drawTextCentered(TitleLabel);
				ImGui::Separator();

				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX(FrameWidth / 2 - ImGui::CalcTextSize("ReplayVideo").x/2);
				ImGui::SetCursorPosY(m_VideoPlayers[1].position().y() + m_VideoPlayers[1].size().y() + 25);
				if (ImGui::Button("Replay Videos")) 
				{
					for (uint8_t i = 0; i < 4; ++i) m_VideoPlayers[i].play();
				}
				ImGui::PopFont();
				ImGui::Separator();

				ImGui::PushFont(m_pFontTileText);

				ImGui::SetCursorPosX(FrameWidth / 2 - ImGui::CalcTextSize("Which of the three videos depicts the movement of the first video?").x / 2);
				ImGui::SetNextItemOpen(true);

				//drawTextCentered("Which of the three videos depicts the movement of the first video?");
				drawTextCentered("Which of the three videos in the bottom row depicts the movement of the video shown in the top row?");

				//drawTextCentered("Please indicate which video corresponds to the movement of the first video.");
				ImGui::Separator();

				for (int32_t n = 0; n < 3; ++n) {
					ImGui::SameLine();
					std::string Label = std::to_string(n+1) + ". Video";

					float Factor = 1.0f;
					if (n == 0) Factor = 1.5;
					if (n == 1) Factor = 0;
					if (n == 2) Factor = -1.5;
					ImGui::SetCursorPosX(FrameWidth / 2 - Factor * 100);
						
					if (ImGui::Selectable(Label.c_str(), m_Part2Data.Selection == n, 0, ImVec2(100, 25))) m_Part2Data.Selection = n;
				}
				ImGui::PopFont();

				// next button
				ImGui::Separator();
				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Next").x) / 2);
				if (ImGui::Button("Next") && m_Part2Data.Selection != -1) {
					studyPart2Next();
				}

				ImGui::PopFont();
				ImGui::PopStyleColor();
				ImGui::End();
			}
			else {
				
				ImGui::PushStyleColor(ImGuiCol_WindowBg, BGColor);
				ImGui::Begin("Experiment finished", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

				float FrameWidth2 = 2.0f * m_VideoPlayers[0].size().x() + 50.0f;

				ImGui::SetWindowPos(ImVec2(m_RenderWin.width() / 2 - FrameWidth2 / 2, 0));
				ImGui::SetWindowSize(ImVec2(FrameWidth2, FrameHeight));

				std::string TitleLabel = "Experiment Finished";
				ImGui::PushFont(m_pFontStudyTitleLabel);
				drawTextCentered(TitleLabel);
				ImGui::PopFont();
				ImGui::Separator();

				ImGui::PushFont(m_pFontTileText);

				std::string Result = "You selected " + std::to_string(m_Part2Data.CorrectSelections) + " times the correct video, " + std::to_string(m_Part2Data.SiblingSelections) + " times the twin,\nand " + std::to_string(m_Part2Data.DistractorSelections) + " times the distractor.\n";
				//drawTextCentered(Result);

				Result += "\n\n";
				//drawTextCentered(Result.c_str()); 

				Result += "The participants of our study selected 749 times (58.15%%) the\ncorrect video and 539 times (41.85%%) the twin.";
				// 1288
				drawTextCentered(Result.c_str());
				ImGui::PopFont();

				ImGui::Separator();
				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Return to Dashboard").x) / 2);
				if (ImGui::Button("Return to Dashboard")) m_DemoState = STATE_DASHBOARD;
				ImGui::PopFont();

				ImGui::PopStyleColor();
			}

			ImGui::PopStyleColor();

			ImGui::EndFrame();
		}//updateStudyPart2GUI

		void studyPart1Next(void) {
			// save current values
			if(m_Part1Data.CurrentItem >= 0) m_Part1Data.ExperimentData[m_Part1Data.CurrentItem].UserRating = m_Part1Data.Selection;
			m_Part1Data.CurrentItem++;
			if (m_Part1Data.CurrentItem >= m_Part1Data.ExperimentData.size()) {
				// experiment finished
				m_Part1Data.ExperimentRunning = false;

				// score synthetic
				float ScoreSyn = 0.0f;
				float ScoreOrig = 0.0f;
				int c = 1;
				for (auto i : m_Part1Data.ExperimentData) 
				{
					if (i.Synthetic) {
						ScoreSyn += float(i.UserRating);
						m_Part1Data.SyntheticRatings.push_back(std::pair(c, float(i.UserRating)));
					}
					else {
						ScoreOrig += float(i.UserRating);
						m_Part1Data.OriginalRatings.push_back(std::pair(c, float(i.UserRating)));
					}
					c++;
				}
				ScoreSyn /= (float(m_Part1Data.ExperimentData.size())*0.5f);
				ScoreOrig /= (float(m_Part1Data.ExperimentData.size()) * 0.5f);
				m_Part1Data.AverageUserScoreOriginal = ScoreOrig;
				m_Part1Data.AverageUserScoreSynthetic = ScoreSyn;
				printf("Experiment finished! You gave an average score of %.2f | %.2f\n", ScoreOrig, ScoreSyn);

			}
			else {
				int32_t VideoID = m_Part1Data.ExperimentData[m_Part1Data.CurrentItem].VideoID;
				m_VideoPlayers[0].play(m_StudyVideos[VideoID]);
				m_VideoPlayers[0].play();
				m_Part1Data.Selection = 2;
			}
		}//studyPart1Next

		void studyPart2Next(void) {
			
			if(m_Part2Data.CurrentItem >= 0) m_Part2Data.ExperimentData[m_Part2Data.CurrentItem].UserSelection = m_Part2Data.Selection+1;
			m_Part2Data.CurrentItem++;
			if (m_Part2Data.CurrentItem >= m_Part2Data.ExperimentData.size()) {
				// experiment finished
				m_Part2Data.ExperimentRunning = false;
				m_Part2Data.CorrectSelections = 0;
				m_Part2Data.SiblingSelections = 0;
				m_Part2Data.DistractorSelections = 0;

				for (auto k : m_Part2Data.ExperimentData) {
					if (k.UserSelection == k.Original) m_Part2Data.CorrectSelections++;
					else if (k.UserSelection == k.Sibling) m_Part2Data.SiblingSelections++;
					else if (k.UserSelection == k.Distractor) m_Part2Data.DistractorSelections++;
					else printf("Invalid selection: %d\n", k.UserSelection);
				}

				/*printf("Your results from experiment 2:\n");
				printf("\tCorrect answers: %d\n", m_Part2Data.CorrectSelections);
				printf("\tWrong answers: %d\n", m_Part2Data.SiblingSelections + m_Part2Data.DistractorSelections);*/
			}
			else {
				uint64_t Start = CForgeUtility::timestamp();

				int32_t VideoIDs[4];
				for (uint8_t i = 0; i < 4; ++i) VideoIDs[i] = m_Part2Data.ExperimentData[m_Part2Data.CurrentItem].VideoIDs[i];

				bool LoadMultiThreaded = true;

				if (LoadMultiThreaded) {
					// start thready and cache video
					std::thread CacheThreads[4];
					bool Ready[4];
					for (uint8_t i = 0; i < 4; ++i) {
						Ready[i] = false;
						CacheThreads[i] = std::thread(videoCacheThread, &m_VideoPlayers[i], m_StudyVideos[VideoIDs[i]], &Ready[i]);
					}

					bool C = false;
					while (!C) {
						C = true;
						for (uint8_t i = 0; i < 4; ++i) {
							if (Ready[i] != true) C = false;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}//while[caching not ready]

					for (uint8_t i = 0; i < 4; ++i) {
						CacheThreads[i].join();
					}
				}
				else {
					for (uint8_t i = 0; i < 4; ++i) m_VideoPlayers[i].play(m_StudyVideos[VideoIDs[i]]);
				}
				

				float CachingTime = float(CForgeUtility::timestamp() - Start)/1000.0f;

				printf("Time for caching: %.2f seconds\n", CachingTime);

				for (uint8_t i = 0; i < 4; ++i) m_VideoPlayers[i].play();
				m_Part2Data.Selection = -1;
			}
		}//studyPart2Next

		void mainLoop(void)override {

			switch (m_DemoState) {
			case STATE_DASHBOARD: {
				m_FPS = std::max(0.25f, m_FPS);
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);
				m_SkyboxSG.update(60.0f / m_FPS);
				(m_IsVideoRecording) ? m_pSimulation->advanceTime(1000.0f/60.0f) : m_pSimulation->advanceTime(0);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				if (m_RenderWin.mouse()->movement().norm() > 0.1f) m_LastInteraction = CForgeUtility::timestamp();
				m_RenderWin.mouse()->movement(Vector2f(0.0f, 0.0f));

				float IdleTime = (CForgeUtility::timestamp() - m_LastInteraction)/1000.0f;

				updateBackgroundCharacters();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_RenderDev.activeCamera(&m_Cam);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.render(&m_RenderDev);

				if (m_DrawFPSLabel) m_FPSLabel.render(&m_RenderDev);
				if (m_DrawHelpTexts) drawHelpTexts();

				if (m_DrawTitle) m_TitleText.render(&m_RenderDev);

				if (IdleTime < 60.0f) {
					updateDashboardGui();
					ImGuiUtility::render();
					if (m_RenderWin.isMouseCursorHidden()) {
						m_RenderWin.hideMouseCursor(false);
					}
				}
				else {
					if (!m_RenderWin.isMouseCursorHidden()) {
						m_RenderWin.hideMouseCursor(true);
					}
				}

				m_RenderWin.swapBuffers();

				if (m_IsVideoRecording) {
					T2DImage<uint8_t> Screenshot;
					CForgeUtility::retrieveFrameBuffer(&Screenshot);
					m_VideoRecorder.addFrame(&Screenshot, 0);
				}

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F4, true)) m_LastInteraction = CForgeUtility::timestamp() - static_cast<uint64_t>(60000);
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_SCREENSAVER;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F7, true))  m_DrawFPSLabel = !m_DrawFPSLabel;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F8, true)) {
					(m_IsVideoRecording) ? stopVideoRecording() : startVideoRecording();
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_T, true)) m_DrawTitle = !m_DrawTitle;

				/*if ((IdleTime > 75.0f && (CForgeUtility::timestamp() - m_LastB03VideoPlay) > 150000) || (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F5, true)) ) {
					for (auto i : m_BackgroundCharacters) i->Character.activeAnimation(nullptr);
					startB03Video();
				}*/
			}break;
			case STATE_STUDYPART1: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				//m_SkyboxSG.render(&m_RenderDev);

				if (m_DrawFPSLabel) m_FPSLabel.render(&m_RenderDev);
				updateStudyPart1GUI();
				ImGuiUtility::render();

				if(m_Part1Data.ExperimentRunning)
				{
					m_VideoPlayers[0].update();
					m_VideoPlayers[0].render(&m_RenderDev);
				}

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if(m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_P, true)) m_VideoPlayers[0].play();
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_DASHBOARD;

			}break;
			case STATE_STUDYPART2: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);

				updateStudyPart2GUI();
				ImGuiUtility::render();

				if(m_Part2Data.ExperimentRunning)
				{
					for (uint8_t i = 0; i < 4; ++i) {
						m_VideoPlayers[i].update();
						m_VideoPlayers[i].render(&m_RenderDev);
					}
				}
				
				if (m_DrawFPSLabel) m_FPSLabel.render(&m_RenderDev);
				

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_DASHBOARD;
			}break;
			case STATE_SCREENSAVER: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_DASHBOARD;
			}break;
			case STATE_B03VIDEO: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);

				m_B03Video.update();
				m_B03Video.render(&m_RenderDev);
				m_RenderWin.swapBuffers();

				if (m_B03Video.finished() || m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) {
					m_DemoState = STATE_DASHBOARD;
					m_LastB03VideoPlay = CForgeUtility::timestamp();
					m_LastCharacterSpawn = CForgeUtility::timestamp();
				}
			}break;
			}
			

		}//mainLoop

		void startB03Video(void) {
			if (m_B03VidoeInitialized) {
				m_B03Video.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				m_B03Video.play();
			}
			else {
				m_B03Video.init(Vector2f(0.0f, 0.0f), Vector2f(1.0f, 1.0f));
				m_B03Video.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				m_B03Video.play("MyAssets/B02Demonstrator/B03_Einspieler.mp4");
				m_B03VidoeInitialized = true;
			}
			m_DemoState = STATE_B03VIDEO;
		}//startB03Video

	protected:

		enum DemonstratorState {
			STATE_DASHBOARD = 0,
			STATE_STUDYPART1,
			STATE_STUDYPART2,
			STATE_MOTIONEDITOR,
			STATE_SCREENSAVER,
			STATE_B03VIDEO,
		};

		struct DisplayModel {
			int32_t ID;
			std::string FilePath;
			std::string OriginalMotion;
			std::vector<std::pair<int32_t, std::string>> AlbedoReplacements; // pair<int, string> = pair<material id, texture path>
			std::vector<std::pair<int32_t, std::string>> NormalReplacements; // pair<int, string> = pair<material id, texture path>
			std::vector<std::pair<int32_t, CForge::CForgeUtility::DefaultMaterial>> MaterialReplacements;
		};

		struct StudyPart1Item {
			int32_t VideoID; // 0 through 11
			int32_t UserRating; // user rating 0 through 4
			bool Synthetic;

			StudyPart1Item(void) {
				VideoID = -1;
				UserRating = -1;
				Synthetic = true;
			}
		};

		struct StudyPart2Item {
			int32_t VideoIDs[4]; // original, and videos 0,1,2
			// positions of the videos
			int32_t Original;
			int32_t Sibling;
			int32_t Distractor;

			int32_t UserSelection; // the actual user selection

			StudyPart2Item(void) {
				for (uint8_t i = 0; i < 4; ++i) VideoIDs[i] = -1;
				Original = -1;
				Sibling = -1;
				Distractor = -1;
			}
		};//StudyPart2Item

		struct StudyPart1Data {
			int32_t CurrentItem;
			int32_t Selection;
			float AverageUserScoreSynthetic;
			float AverageUserScoreOriginal;

			std::vector<std::pair<int32_t, float>> OriginalRatings;
			std::vector<std::pair<int32_t, float>> SyntheticRatings;

			GLTexture2D ScaleImg;
			bool ExperimentRunning;

			std::vector<StudyPart1Item> ExperimentData;

			StudyPart1Data(void) {
				Selection = 2;
				CurrentItem = 0;
				AverageUserScoreSynthetic = -1.0f;
				AverageUserScoreOriginal = -1.0f;
			}
		};//StudyPart1Data

		struct StudyPart2Data {
			int32_t CurrentItem;
			int32_t Selection; // video 0 through 2
			bool ExperimentRunning;

			// Results
			int32_t CorrectSelections;
			int32_t SiblingSelections;
			int32_t DistractorSelections;

			std::vector<StudyPart2Item> ExperimentData;
		};//StudyPart2Data

		void startVideoRecording() {
			m_VideoRecorder.startRecording("MyAssets/ScreenRecording.mp4", 1280, 720, 60.0f);
			m_IsVideoRecording = true;
		}//startVideoRecording

		void stopVideoRecording() {
			m_VideoRecorder.stopRecording();
			m_IsVideoRecording = false;
		}//isVideoRecording


		void initDisplayModels(void) {

			DisplayModel* pM = new DisplayModel();
			pM->ID = m_DisplayModels.size();
			pM->FilePath = "MyAssets/Models/B02FemaleModelMKIII.gltf";
			pM->OriginalMotion = "MyAssets/Models/RigFemale.bvh";
			pM->AlbedoReplacements.push_back(std::pair(0, "MyAssets/Models/Textures/young_lightskinned_female_diffuse.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/female_casualsuit01_diffuse.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/brown_eye.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(3, "MyAssets/Models/Textures/shoes06_diffuse.jpg"));
			pM->NormalReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/female_casualsuit01_normal.jpg"));

			m_DisplayModels.push_back(pM);

			pM = new DisplayModel();
			pM->ID = m_DisplayModels.size();
			pM->FilePath = "MyAssets/Models/B02MaleModelMKIII.gltf";
			pM->OriginalMotion = "MyAssets/Models/RigMale.bvh";

			pM->AlbedoReplacements.push_back(std::pair(0, "MyAssets/Models/Textures/young_lightskinned_male_diffuse2.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/male_casualsuit04_diffuse.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/brown_eye.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(3, "MyAssets/Models/Textures/shoes06_diffuse.jpg"));
			pM->NormalReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/male_casualsuit04_normal.jpg"));
			m_DisplayModels.push_back(pM);

			pM = new DisplayModel();
			pM->ID = m_DisplayModels.size();
			pM->FilePath = "MyAssets/Models/B02ARKitRobot.gltf";
			pM->OriginalMotion = "MyAssets/Models/RigMale.bvh";
			pM->AlbedoReplacements.push_back(std::pair(0, ""));
			pM->MaterialReplacements.push_back(std::pair(0, CForgeUtility::PLASTIC_GREY));
			m_DisplayModels.push_back(pM);

			//CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_GREY);

		}//initDisplayModels

		struct Actor {
			SkeletalAnimationController AnimationController;
			//SkeletalActor Character;
			AdaptiveSkeletalActor Character;

			SGNTransformation TransformSGN;
			SGNGeometry GeomSGN;

			int32_t DisplayModel;
		};//Actor

		void spawnCharacter(Actor* pActor, std::string Filepath, Vector3f Position, Quaternionf Rotation) {
			pActor->TransformSGN.translation(Position);
			pActor->TransformSGN.rotation(Rotation);

			updateMotion(pActor, Filepath);

			pActor->GeomSGN.actor(&pActor->Character);
			pActor->Character.activeAnimation(pActor->AnimationController.createAnimation(0, 1.0f, 0.0f));
		}//spawnCharacter

		void updateMotion(Actor* pActor, const std::string MotionData) {
			// load new animation data
			T3DMesh<float> Anim;
			AssetIO::load(MotionData, &Anim);

			// load display model ...
			DisplayModel* pDM = m_DisplayModels[pActor->DisplayModel];
			T3DMesh<float> M;
			AssetIO::load(pDM->FilePath, &M);

			// .. and apply original skeleton bvh-style
			T3DMesh<float> MOrig;
			AssetIO::load(pDM->OriginalMotion, &MOrig);
			M.clearSkeletalAnimations();
			for (uint32_t i = 0; i < MOrig.skeletalAnimationCount(); ++i) M.addSkeletalAnimation(MOrig.getSkeletalAnimation(i), true);
			recomputeInverseBindPoseMatrix(&M, true, true);

			// copy rotation data of new motion
			T3DMesh<float>::SkeletalAnimation Merged;
			mergeAnimationData(M.getSkeletalAnimation(0), Anim.getSkeletalAnimation(0), &Merged, "Hips");
			M.clearSkeletalAnimations();
			M.addSkeletalAnimation(&Merged);
			//recomputeInverseBindPoseMatrix(&M, true, true);

			// retrieve size differences of the two skeletons
			Vector2f MinMaxOrig = retrieveSkeletonExtrema(&MOrig);
			Vector2f MinMaxAnim = retrieveSkeletonExtrema(&Anim);

			float Deviation = (MinMaxAnim.y() - MinMaxAnim.x()) / (MinMaxOrig.y() - MinMaxOrig.x());

			for (auto i : M.getSkeletalAnimation(0)->Keyframes) {
				if (i->BoneName.compare("Hips") == 0) {
					//for (auto& k : i->Scalings) k = Vector3f(Deviation, Deviation, Deviation);
					for (auto& k : i->Positions) k = 1.0f / Deviation * k;
				}
			}

			// adapt material if necessary
			for (auto i : pDM->AlbedoReplacements) M.getMaterial(i.first)->TexAlbedo = i.second;
			for (auto i : pDM->NormalReplacements) M.getMaterial(i.first)->TexNormal = i.second;
			for (auto i : pDM->MaterialReplacements) CForgeUtility::defaultMaterial(M.getMaterial(i.first), i.second);

			M.computePerVertexNormals();
			BoundingVolume BV;

			pActor->AnimationController.init(&M, true);
			pActor->Character.init(&M, &pActor->AnimationController);
			pActor->Character.boundingVolume(BV);

			/*pTile->StickFigure.init(&M, &pTile->AnimationController);
			pTile->StickFigure.jointSize(pTile->StickFigure.jointSize() / 1.5f);
			pTile->StickFigure.boneSize(pTile->StickFigure.boneSize() * 0.75f);*/

		}//updateMotion

		void transformSkeleton(T3DMesh<float>::Bone* pBone, Eigen::Matrix4f ParentTransform, std::vector<T3DMesh<float>::BoneKeyframes*>* pKeyframes, Eigen::Vector4f* pMinMax, bool ConsiderTranslationOnly, bool DryRun) {
			// compute local transform
			T3DMesh<float>::BoneKeyframes* pFrame = nullptr;

			for (auto i : (*pKeyframes)) {
				if (i->BoneName.compare(pBone->Name) == 0) pFrame = i;
			}

			Eigen::Matrix4f JointTransform = Matrix4f::Identity();
			if (pFrame != nullptr) {
				Matrix4f R = (pFrame->Rotations.size() > 0) ? CForgeMath::rotationMatrix(pFrame->Rotations[0]) : Matrix4f::Identity();
				Matrix4f T = (pFrame->Positions.size() > 0) ? CForgeMath::translationMatrix(pFrame->Positions[0]) : Matrix4f::Identity();
				Matrix4f S = (pFrame->Scalings.size() > 0) ? CForgeMath::scaleMatrix(pFrame->Scalings[0]) : Matrix4f::Identity();

				if (ConsiderTranslationOnly) {
					S = R = Matrix4f::Identity();
				}

				if (pBone->pParent == nullptr) {
					JointTransform = R * S;
				}
				else {
					JointTransform = T * R * S;
				}
			}

			Matrix4f LocalTransform = ParentTransform * JointTransform;
			float x = LocalTransform(0, 3);
			float y = LocalTransform(1, 3);
			float z = LocalTransform(2, 3);
			// new y values
			if (x < pMinMax[0].z()) pMinMax[0].z() = x;
			if (y < pMinMax[1].z()) pMinMax[1].z() = y;
			if (z < pMinMax[2].z()) pMinMax[2].z() = z;

			if (pBone->pParent == nullptr) {
				pMinMax[0].w() = x;
				pMinMax[1].w() = y;
				pMinMax[2].w() = z;
			}

			// old y values
			x = pBone->OffsetMatrix.inverse()(0, 3);
			y = pBone->OffsetMatrix.inverse()(1, 3);
			z = pBone->OffsetMatrix.inverse()(2, 3);

			if (x < pMinMax[0].x()) pMinMax[0].x() = x;
			if (y < pMinMax[1].x()) pMinMax[1].x() = y;
			if (z < pMinMax[2].x()) pMinMax[2].x() = z;

			if (pBone->pParent == nullptr) {
				pMinMax[0].y() = x;
				pMinMax[1].y() = y;
				pMinMax[2].y() = z;
			}

			if (!DryRun)	pBone->OffsetMatrix = LocalTransform.inverse();

			// recursion
			for (auto i : pBone->Children) transformSkeleton(i, LocalTransform, pKeyframes, pMinMax, ConsiderTranslationOnly, DryRun);

		}//transformSkeleton

		void recomputeInverseBindPoseMatrix(T3DMesh<float>* pMesh, bool ScaleRootAnimation, bool ConsiderTranslationOnly) {
			if (nullptr == pMesh->rootBone()) throw CForgeExcept("Mesh contains no root bone!");
			if (0 == pMesh->skeletalAnimationCount()) throw CForgeExcept("Mesh contains no animations!");

			auto* pRoot = pMesh->rootBone();
			auto* pAnim = pMesh->getSkeletalAnimation(0);

			Eigen::Vector4f MinMax[3] = { Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero() };

			//for(auto i: pRoot->Children) transformSkeleton(i, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, false);
			transformSkeleton(pRoot, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, ConsiderTranslationOnly, false);


			if (ScaleRootAnimation) {
				// scale vertices of mesh
				std::vector<Eigen::Vector3f> Vertices;
				Eigen::Matrix3f ScaleMatrix = Matrix3f::Identity();
				ScaleMatrix(0, 0) = (MinMax[0].w() - MinMax[0].z()) / (MinMax[0].y() - MinMax[0].x());
				ScaleMatrix(1, 1) = (MinMax[1].w() - MinMax[1].z()) / (MinMax[1].y() - MinMax[1].x());
				ScaleMatrix(2, 2) = (MinMax[2].w() - MinMax[2].z()) / (MinMax[2].y() - MinMax[2].x());

				/*for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
					auto v = ScaleMatrix * pMesh->vertex(i);
					Vertices.push_back(v);
				}
				pMesh->vertices(&Vertices);*/

				float s = (MinMax[1].y() - MinMax[1].x()) / (MinMax[1].w() - MinMax[1].z());
				ScaleMatrix(0, 0) = s;
				ScaleMatrix(1, 1) = s;
				ScaleMatrix(2, 2) = s;


				for (uint32_t i = 0; i < pMesh->skeletalAnimationCount(); ++i) {
					auto* pAnim = pMesh->getSkeletalAnimation(i);

					for (uint32_t k = 0; k < pAnim->Keyframes.size(); ++k) {
						if (pAnim->Keyframes[k]->BoneName.compare(pMesh->rootBone()->Name) == 0) {
							for (auto& j : pAnim->Keyframes[i]->Positions) j = ScaleMatrix * j;
						}
					}

				}
			}//ScaleRootAnimation

		}//recomputeInverseBindPoseMatrix

		void retrieveSkeletonExtrema(T3DMesh<float>::Bone* pBone, Eigen::Matrix4f ParentTransform, std::vector<T3DMesh<float>::BoneKeyframes*>* pKeyframes, Eigen::Vector2f* pExtrema) {
			T3DMesh<float>::BoneKeyframes* pFrame = nullptr;

			for (auto i : (*pKeyframes)) {
				if (i->BoneName.compare(pBone->Name) == 0) pFrame = i;
			}

			Eigen::Matrix4f JointTransform = Matrix4f::Identity();
			if (pFrame != nullptr) {
				Matrix4f R = (pFrame->Rotations.size() > 0) ? CForgeMath::rotationMatrix(pFrame->Rotations[0]) : Matrix4f::Identity();
				Matrix4f T = (pFrame->Positions.size() > 0) ? CForgeMath::translationMatrix(pFrame->Positions[0]) : Matrix4f::Identity();
				Matrix4f S = (pFrame->Scalings.size() > 0) ? CForgeMath::scaleMatrix(pFrame->Scalings[0]) : Matrix4f::Identity();


				S = R = Matrix4f::Identity();

				if (pBone->pParent == nullptr) {
					JointTransform = R * S;
				}
				else {
					JointTransform = T * R * S;
				}
			}

			Matrix4f LocalTransform = ParentTransform * JointTransform;
			float x = LocalTransform(0, 3);
			float y = LocalTransform(1, 3);
			float z = LocalTransform(2, 3);

			if (pBone->pParent == nullptr) pExtrema->y() = y;

			//if (y > pExtrema->y()) pExtrema->y() = y;
			if (y < pExtrema->x()) pExtrema->x() = y;

			// recursion
			for (auto i : pBone->Children) retrieveSkeletonExtrema(i, LocalTransform, pKeyframes, pExtrema);

		}//retrieveSkeletonExtrema

		Eigen::Vector2f retrieveSkeletonExtrema(T3DMesh<float>* pMesh) {

			Eigen::Vector2f Rval = Vector2f(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
			retrieveSkeletonExtrema(pMesh->rootBone(), Eigen::Matrix4f::Identity(), &pMesh->getSkeletalAnimation(0)->Keyframes, &Rval);
			return Rval;

		}//retrieveSkeletonHeight

		void mergeAnimationData(T3DMesh<float>::SkeletalAnimation* pAnimOrig, T3DMesh<float>::SkeletalAnimation* pAnimNew, T3DMesh<float>::SkeletalAnimation* pMerged, std::string RootBone) {

			pMerged->Duration = pAnimNew->Duration;
			pMerged->Keyframes.clear();
			pMerged->SamplesPerSecond = pAnimNew->SamplesPerSecond;
			pMerged->Name = pAnimNew->Name;

			for (uint32_t i = 0; i < pAnimOrig->Keyframes.size(); ++i) {

				pMerged->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
				pMerged->Keyframes[i]->ID = i;
				pMerged->Keyframes[i]->BoneID = pAnimOrig->Keyframes[i]->BoneID;
				pMerged->Keyframes[i]->BoneName = pAnimOrig->Keyframes[i]->BoneName;

				// we use position and scale of first keyframe (rest pose)
				Vector3f Pos = (pAnimOrig->Keyframes[i]->Positions.size() > 0) ? pAnimOrig->Keyframes[i]->Positions[0] : Vector3f::Zero();
				Vector3f Scale = (pAnimOrig->Keyframes[i]->Scalings.size() > 0) ? pAnimOrig->Keyframes[i]->Scalings[0] : Vector3f::Ones();
				Quaternionf Rot = (pAnimOrig->Keyframes[i]->Rotations.size() > 0) ? pAnimOrig->Keyframes[i]->Rotations[0] : Quaternionf::Identity();

				// find matching keyframe in new animation
				T3DMesh<float>::BoneKeyframes* pKeyframeNew = nullptr;
				for (auto j : pAnimNew->Keyframes) {
					if (pAnimOrig->Keyframes[i]->BoneName.compare(j->BoneName) == 0) {
						pKeyframeNew = j;
						break;
					}
				}

				if (nullptr == pKeyframeNew) {
					printf("Did not find a valid match for bone %s\n", pMerged->Keyframes[i]->BoneName.c_str());
					continue;
				}

				for (uint32_t k = 0; k < pKeyframeNew->Rotations.size(); ++k) {

					pMerged->Keyframes[i]->Positions.push_back(Pos);
					pMerged->Keyframes[i]->Scalings.push_back(Scale);
					pMerged->Keyframes[i]->Rotations.push_back(pKeyframeNew->Rotations[k]);
					pMerged->Keyframes[i]->Timestamps.push_back(float(k)); // pKeyframeNew->Timestamps[k]);

				}//for[rotation data]

				// copy root bone data
				if (pMerged->Keyframes[i]->BoneName.compare(RootBone) == 0) {
					pMerged->Keyframes[i]->Positions = pKeyframeNew->Positions;
				}
			}//for[all keyframes]

		}//copyRotationData


		void startStudyPart1(void) {

			if (m_B03VidoeInitialized) {
				m_B03Video.clear();
				m_B03VidoeInitialized = false;
			}

			float Aspect = 1280.0f / 720.0f;

			float VideoWidth = 0.5f * m_RenderWin.width();
			float VideoHeight = 0.5f / Aspect * m_RenderWin.width();

			// abort if windows no high enough
			if (m_RenderWin.height() < 400) return;

			while (m_RenderWin.height() - VideoHeight < 350) {
				VideoWidth *= 0.9f;
				VideoHeight *= 0.9f;
			}

			float Center = (m_RenderWin.width() - VideoWidth) / 2.0f;

			m_VideoPlayers[0].init(Vector2f(Center, 0.075f), Vector2f(VideoWidth, VideoHeight), Vector2i(m_RenderWin.width(), m_RenderWin.height()));

			T2DImage<uint8_t> Img;
			AssetIO::load("MyAssets/B02Demonstrator/Scale.jpg", &Img);
			Img.flipRows();
			m_Part1Data.ScaleImg.init(&Img, false);

			m_Part1Data.ExperimentData.clear();
			m_Part1Data.OriginalRatings.clear();
			m_Part1Data.SyntheticRatings.clear();

			// setup data
			int32_t SynCount = 0;
			int32_t OrigCount = 0;
			while (m_Part1Data.ExperimentData.size() < m_TrialCountPart1) {
				StudyPart1Item Item;
				Item.VideoID = CForgeMath::randRange(0, int32_t(m_StudyVideos.size() - 1));
				Item.Synthetic = (m_StudyVideos[Item.VideoID].find("_1") != std::string::npos) ? false : true;

				if(SynCount < m_TrialCountPart1/2 && Item.Synthetic) 
				{
					m_Part1Data.ExperimentData.push_back(Item);
					SynCount++;
				}
				else if(OrigCount < m_TrialCountPart1/2 && !Item.Synthetic){
					m_Part1Data.ExperimentData.push_back(Item);
					OrigCount++;
				}
				
				//printf("Video: %d:%d\n", Item.VideoID, int32_t(Item.Synthetic));
			}

			m_Part1Data.ExperimentRunning = true;
			m_Part1Data.CurrentItem = -1;
			studyPart1Next();
		}//startStudyPart1

		bool isMaleVideo(const std::string File) {
			for (auto i : m_MaleVideos) {
				if (File.find(i) != std::string::npos) return true;
			}
			return false;
		}//isMaleVideo

		bool isFemaleVideo(const std::string File) {
			for (auto i : m_FemaleVideos) {
				if (File.find(i) != std::string::npos) return true;
			}
			return false;
		}//isFemaleVideo

		int32_t getDistractor(int32_t VideoID) {
			int32_t Rval = -1;
			std::string DisFile;
			if (isMaleVideo(m_StudyVideos[VideoID])) {
				DisFile = m_MaleVideos[CForgeMath::randRange(0, int32_t(m_MaleVideos.size()))];
			}
			else if (isFemaleVideo(m_StudyVideos[VideoID])) {
				DisFile = m_FemaleVideos[CForgeMath::randRange(0, int32_t(m_MaleVideos.size()))];
			}
			else {
				printf("Sex of video could not be detected!");
				DisFile = m_MaleVideos[0];
			}

			for (uint32_t i = 0; i < m_StudyVideos.size(); ++i) {
				if (m_StudyVideos[i].find(DisFile) != std::string::npos) Rval = i;
			}
			return Rval;
		}//getDistractor

		void startStudyPart2(void) {
			if (m_B03VidoeInitialized) {
				m_B03Video.clear();
				m_B03VidoeInitialized = false;
			}

			// initialize trial data
			m_Part2Data.ExperimentData.clear();
			for (uint32_t i = 0; i < m_TrialCountPart2; ++i) {
				// select video randomly
				StudyPart2Item Item;
				Item.VideoIDs[0] = CForgeMath::randRange(0, int32_t(m_StudyVideos.size()));

				// find sibling and distractor
				int32_t Original = Item.VideoIDs[0];
				int32_t Sibling = (m_StudyVideos[Item.VideoIDs[0]].find("_1") == std::string::npos) ? Item.VideoIDs[0] - 1 : Item.VideoIDs[0] + 1;
				int32_t Distractor = CForgeMath::randRange(0, int32_t(m_StudyVideos.size()));
				
				do {
					Distractor = getDistractor(Item.VideoIDs[0]);
				} while (Distractor == Original || Distractor == Sibling);
				
				printf("Triplet: (%d, %d, %d)\n", Original, Sibling, Distractor);

				std::vector<int32_t> Positions;
				Positions.push_back(0);
				Positions.push_back(1);
				Positions.push_back(2);

				for (uint8_t k = 1; k <= 3; ++k) {
					int32_t P = CForgeMath::randRange(0, 25000) % Positions.size();
					int32_t Next = Positions[P];

					if (Next == 0) {
						Item.VideoIDs[k] = Original;
						Item.Original = k;
					}
					else if (Next == 1) {
						Item.VideoIDs[k] = Sibling;
						Item.Sibling = k;
					}
					else if(Next == 2){
						Item.VideoIDs[k] = Distractor;
						Item.Distractor = k;
					}

					auto Pos = Positions.begin();
					for (uint8_t t = 0; t < P; t++) Pos++;
					Positions.erase(Pos);
				}//Next
				
				m_Part2Data.ExperimentData.push_back(Item);

			}//for[trials]

			// initialize video players
			float Aspect = 1280.0f / 720.0f;
			float WinAspect = float(m_RenderWin.width()) / float(m_RenderWin.height());
			float PlayerWidth = 0.30f;
			float PlayerHeight = (PlayerWidth) * (WinAspect/Aspect) * m_RenderWin.height();
			
			Vector2i Canvas = Vector2i(m_RenderWin.width(), m_RenderWin.height());

			float TopOffset = 60.0f;
			// abort if render win is too small
			if (m_RenderWin.height() < (200 + TopOffset + 25 + 10)) return;

			while (m_RenderWin.height() - (2.0f*PlayerHeight+TopOffset+25) < 200) {
				PlayerWidth *= 0.9f;
				PlayerHeight *= 0.9f;
			}//while[adjust video players]

			float t = 1.0f / 3.0f - PlayerWidth;
	
			m_VideoPlayers[0].init(Vector2f(1.0f / 3.0f + t / 2.0f, TopOffset), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[1].init(Vector2f(1.0f / 3.0f + t / 2.0f - PlayerWidth*1.10f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[2].init(Vector2f(1.0f / 3.0f + t / 2.0f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[3].init(Vector2f(1.0f / 3.0f + t / 2.0f + PlayerWidth*1.10f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);

			m_Part2Data.ExperimentRunning = true;
			m_Part2Data.Selection = -1;
			m_Part2Data.CurrentItem = -1;
			studyPart2Next();

		}//startStudyPart2

		virtual void startMotionEditor(void) {

			system("cd ./MyAssets/MotionEditor && start GaitMotionEditor.exe");
			
		}//startMotionEditor

		virtual void listen(GLWindowMsg Msg) override {
			ExampleSceneBase::listen(Msg);

			alignDashboardTiles();
			m_TitleText.canvasSize(m_RenderWin.width(), m_RenderWin.height());
			m_TitleText.position(m_RenderWin.width() / 2 - m_TitleText.textWidth() / 2, 15);

			for (uint8_t i = 0; i < 4; ++i) {
				m_VideoPlayers[i].canvasSize(m_RenderWin.width(), m_RenderWin.height());
			}
		}//listen

		static void videoCacheThread(VideoPlayer* pPlayer, std::string Filepath, bool* pReady) {
			pPlayer->play(Filepath);
			(*pReady) = true;
		}//videoCacheThread

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
		ImFont* m_pFontStudyTitleLabel;
		ImFont* m_pFontButtonText;


		SkeletalActor m_Character;
		SkeletalAnimationController m_CharacterController;
		SkeletalAnimationController::Animation* m_pCharacterAnim;

		SGNGeometry m_CharacterSGN;
		SGNTransformation m_CharacterTransformSGN;
		
		StudyPart1Data m_Part1Data;
		StudyPart2Data m_Part2Data;

		std::vector<std::string> m_StudyVideos;

		bool m_DrawFPSLabel;
		bool m_DrawTitle;

		int32_t m_TrialCountPart1;
		int32_t m_TrialCountPart2;

		std::vector<std::string> m_MaleVideos;
		std::vector<std::string> m_FemaleVideos;

		std::vector<DisplayModel*> m_DisplayModels;
		std::vector<Actor*> m_BackgroundCharacters;

		uint64_t m_LastCharacterSpawn;

		uint64_t m_LastInteraction;

		VideoPlayer m_B03Video;
		bool m_B03VidoeInitialized;
		uint64_t m_LastB03VideoPlay;

		CForgeSimulation* m_pSimulation;

		VideoRecorder m_VideoRecorder;
		bool m_IsVideoRecording;

		

	};//ExampleMinimumGraphicsSetup

}//name space

#endif