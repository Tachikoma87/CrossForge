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

			m_DrawFPSLabel = false;

			m_TrialCountPart1 = 5;
			m_TrialCountPart2 = 5;
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

			CForgeMath::randSeed(CForgeUtility::timestamp());

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
			m_pFontTileText = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf", 24);
			m_pFontStudyTitleLabel = pImgGuiIo.Fonts->AddFontFromFileTTF("Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf", 40);

			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());
			m_TileRects.push_back(Rectangle());


			alignDashboardTiles();
			
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

			// set study videos
			std::string BasePath = "MyAssets/B02Demonstrator/Study_Videos/";

			for (uint8_t i = 0; i < 12; ++i) {
				m_StudyVideos.push_back(BasePath + ((i < 9) ? "0" : "") + std::to_string(i + 1) + "_1.mp4");
				m_StudyVideos.push_back(BasePath + ((i < 9) ? "0" : "") + std::to_string(i + 1) + "_2.mp4");
			}

			m_DemoState = STATE_DASHBOARD;

		}//initialize

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
			//if (SelectedTile == 2) ImGui::PushStyleColor(ImGuiCol_WindowBg, SelectionColor);
			//ImGui::Begin("Motion Editor Tile", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			//ImGui::SetWindowSize(ImVec2(TileWidth, 600));
			//ImGui::SetWindowPos(ImVec2(5 * m_RenderWin.width() / 6 - TileWidth / 2, 75));
			//ImGui::PushFont(m_pFontTileHeading);
			//drawTextCentered("Try Our Motion Editor");
			//ImGui::PopFont();
			//ImgScale = float(std::min(375, TileWidth-20)) / float(m_ImgMotionEditor.width());
			//ImGui::Image((void*)(intptr_t)m_ImgMotionEditor.handle(), ImVec2(m_ImgMotionEditor.width()*ImgScale, m_ImgMotionEditor.height()*ImgScale));
			//ImGui::PushFont(m_pFontTileText);
			//ImGui::Text("Use our motion editor to generator gait motions by\nadjusting various parameters.");
			//ImGui::PopFont();
			//ImGui::End();
			//if (SelectedTile == 2) ImGui::PopStyleColor();

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

			}

		}//updateGui


		void updateStudyPart1GUI(void) {
			ImGuiUtility::newFrame();

			int32_t FrameWidth = m_VideoPlayers[0].size().x() + 50;
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
				if (ImGui::TreeNode("How natural or artificial did you find the motion sequence depicted in the video?")) {
					ImGui::Text("Please rate on the scale how natural or artificial the movement of the video looks.");
					ImGui::Separator();

					ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 20);
					ImGui::Image((void*)m_Part1Data.ScaleImg.handle(), ImVec2(275, 50));

					ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 35);
					ImGui::Text("artificial"); ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::CalcTextSize("The movement of the video appears:").x + 35 + 250 - ImGui::CalcTextSize("natural").x);
					ImGui::Text("natural");

					ImGui::Text("The movement of the video appears:");

					for (int32_t n = 0; n < 5; ++n) {
						ImGui::SameLine();
						std::string Label = std::to_string(n);
						if (ImGui::Selectable(Label.c_str(), m_Part1Data.Selection == n, 0, ImVec2(50, 25))) m_Part1Data.Selection = n;
					}

					ImGui::TreePop();
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
				std::string Result = "You gave an average rating of " + std::to_string(m_Part1Data.AverageUserScore) + ". The average rating in our study was 2.99";
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

			int32_t FrameWidth = m_RenderWin.width(); // m_VideoPlayers[0].size().x() * 3 + 50;
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
				if (ImGui::TreeNode("Which of the three videos depicts the movement of the first video?")) {
					drawTextCentered("Please indicate which video corresponds to the movement of the first video.");
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

					ImGui::TreePop();
				}
				ImGui::PopFont();


				// next button
				ImGui::Separator();
				ImGui::PushFont(m_pFontTileHeading);
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Next").x) / 2);
				if (ImGui::Button("Next")) {
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
				std::string Result = "You answered " + std::to_string(m_Part2Data.CorrectSelections) + " times correct and " + std::to_string(m_Part2Data.SiblingSelections + m_Part2Data.DistractorSelections) +" times incorrect.";
				drawTextCentered(Result.c_str()); 
				Result = "The participants of our study answered 749 times (54,2%%) correct ";
				drawTextCentered(Result.c_str());
				Result = "and 632 times(45, 8 % %) incorrect.";
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

				// score
				float Score = 0.0f;
				for (auto i : m_Part1Data.ExperimentData) Score += float(i.UserRating);
				Score /= float(m_Part1Data.ExperimentData.size());
				m_Part1Data.AverageUserScore = Score;
				printf("Experiment finished! You gave an average score of %.2f\n", Score);

			}
			else {
				int32_t VideoID = m_Part1Data.ExperimentData[m_Part1Data.CurrentItem].VideoID;
				m_VideoPlayers[0].play(m_StudyVideos[VideoID]);
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

				printf("Your results from experiment 2:\n");
				printf("\tCorrect answers: %d\n", m_Part2Data.CorrectSelections);
				printf("\tWrong answers: %d\n", m_Part2Data.SiblingSelections + m_Part2Data.DistractorSelections);
			}
			else {
				int32_t VideoIDs[4];
				for (uint8_t i = 0; i < 4; ++i) VideoIDs[i] = m_Part2Data.ExperimentData[m_Part2Data.CurrentItem].VideoIDs[i];
				for (uint8_t i = 0; i < 4; ++i) m_VideoPlayers[i].play(m_StudyVideos[VideoIDs[i]]);

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

				if(m_DrawFPSLabel) m_FPSLabel.render(&m_RenderDev);
				if (m_DrawHelpTexts) drawHelpTexts();

				m_TitleText.render(&m_RenderDev);


				updateDashboardGui();
				ImGuiUtility::render();

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_SCREENSAVER;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F7, true))  m_DrawFPSLabel = !m_DrawFPSLabel;

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
			}break;
			case STATE_SCREENSAVER: {
				m_RenderWin.update();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) m_DemoState = STATE_DASHBOARD;
			}break;
			}
			

		}//mainLoop

	protected:

		enum DemonstratorState {
			STATE_DASHBOARD = 0,
			STATE_STUDYPART1,
			STATE_STUDYPART2,
			STATE_MOTIONEDITOR,
			STATE_SCREENSAVER,
		};

		struct StudyPart1Item {
			int32_t VideoID; // 0 through 11
			int32_t UserRating; // user rating 0 through 4

			StudyPart1Item(void) {
				VideoID = -1;
				UserRating = -1;
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
			float AverageUserScore;
			
			GLTexture2D ScaleImg;
			bool ExperimentRunning;

			std::vector<StudyPart1Item> ExperimentData;

			StudyPart1Data(void) {
				Selection = 2;
				CurrentItem = 0;
				AverageUserScore = -1.0f;
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

		void startStudyPart1(void) {
			float Aspect = 1280.0f / 720.0f;

			float VideoWidth = 0.5f * m_RenderWin.width();
			float VideoHeight = 0.5f / Aspect * m_RenderWin.width();

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

			// setup data
			for (uint32_t i = 0; i < m_TrialCountPart1; ++i) {
				StudyPart1Item Item;
				Item.VideoID = CForgeMath::randRange(0, int32_t(m_StudyVideos.size()-1));
				m_Part1Data.ExperimentData.push_back(Item);
			}
			m_Part1Data.ExperimentRunning = true;
			m_Part1Data.CurrentItem = -1;
			studyPart1Next();
		}//startStudyPart1

		void startStudyPart2(void) {
			// initialize video players

			m_Part2Data.ExperimentData.clear();
			for (uint32_t i = 0; i < m_TrialCountPart2; ++i) {
				// select video randomly
				StudyPart2Item Item;
				Item.VideoIDs[0] = CForgeMath::randRange(0, int32_t(m_StudyVideos.size()));

				// find sibling and distractor
				int32_t Sibling = (m_StudyVideos[Item.VideoIDs[0]].find("_1") == std::string::npos) ? Item.VideoIDs[0] - 1 : Item.VideoIDs[0] + 1;
				int32_t Distractor = CForgeMath::randRange(0, int32_t(m_StudyVideos.size()));
				int32_t Original = Item.VideoIDs[0];

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

			float t = 1.0f / 3.0f - PlayerWidth;

			float TopOffset = 60.0f;

			m_VideoPlayers[0].init(Vector2f(1.0f/3.0f + t/2.0f, TopOffset), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[1].init(Vector2f(t/2.0f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[2].init(Vector2f(1.0f/3.0f + t/2.0f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);
			m_VideoPlayers[3].init(Vector2f(2.0f/3.0f + t/2.0f, PlayerHeight + TopOffset + 25), Vector2f(PlayerWidth, PlayerHeight), Canvas);

			m_Part2Data.ExperimentRunning = true;
			m_Part2Data.Selection = -1;
			m_Part2Data.CurrentItem = -1;
			studyPart2Next();

		}//startStudyPart2


		virtual void listen(GLWindowMsg Msg) override {
			ExampleSceneBase::listen(Msg);

			alignDashboardTiles();
			m_TitleText.canvasSize(m_RenderWin.width(), m_RenderWin.height());
			m_TitleText.position(m_RenderWin.width() / 2 - m_TitleText.textWidth() / 2, 15);

			for (uint8_t i = 0; i < 4; ++i) {
				m_VideoPlayers[i].canvasSize(m_RenderWin.width(), m_RenderWin.height());
			}
		}//listen

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

		int32_t m_TrialCountPart1;
		int32_t m_TrialCountPart2;
	};//ExampleMinimumGraphicsSetup

}//name space

#endif