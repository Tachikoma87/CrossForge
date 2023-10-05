/*****************************************************************************\
*                                                                           *
* File(s): SideBySideMotionScene.hpp                                            *
*                                                                           *
* Content: Test scene for testing screen recording feature.   *
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
#ifndef __CFORGE_SIDEBYSIDEMOTIONSCENE_HPP__
#define __CFORGE_SIDEBYSIDEMOTIONSCENE_HPP__

#ifdef USE_OPENCV

#include <Examples/ExampleSceneBase.hpp>
#include "../Camera/VideoRecorder.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class SideBySideMotionScene : public ExampleSceneBase {
	public:
		SideBySideMotionScene(void) {
			m_WindowTitle = "CrossForge Example - Screen Recording";
			m_WinWidth = 1280;
			m_WinHeight = 720;
			m_RenderBufferScale = 1;
			m_TileCount = 2;
		}//Constructor

		~SideBySideMotionScene(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice();
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(15.0f, 15.0f, 25.0f);
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


			m_Cam.position(Vector3f(15.0f, 5.0f, 35.0f));
			m_Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
			initFPSLabel();
			initSkybox();

			m_GBufferVP.Position = Vector2i(0, 0);
			m_GBufferVP.Size = Vector2i(m_RenderDev.gBuffer()->width(), m_RenderDev.gBuffer()->height());
			m_RenderDev.viewport(RenderDevice::RENDERPASS_GEOMETRY, m_GBufferVP);


			initDisplayModels();

			m_RootSGN.init(nullptr);
			initGroundPlane(&m_RootSGN, 400.0f, 60.0f);

			for (uint8_t i = 0; i < 4; ++i)	{
				m_DisplayTiles.push_back(new DisplayTile);
				DisplayTile* pTile = m_DisplayTiles[i];

				pTile->RootSGN.init(nullptr);
				pTile->SG.init(&m_DisplayTiles[i]->RootSGN);
				pTile->GroundPlaneTransformSGN.init(&pTile->RootSGN);
				pTile->GroundPlaneSGN.init(&pTile->GroundPlaneTransformSGN, &m_GroundPlane);


				float Scale = 0.025f;

				pTile->CharacterTransformSGN.init(&pTile->RootSGN);
				pTile->CharacterTransformSGN.scale(Vector3f(Scale, Scale, Scale));
				pTile->CharacterSGN.init(&pTile->CharacterTransformSGN, &pTile->CharacterData.Character);

				pTile->CharacterTransformSGN2.init(&pTile->RootSGN);
				pTile->CharacterTransformSGN2.scale(Vector3f(Scale, Scale, Scale));
				pTile->CharacterTransformSGN2.translation(Vector3f(0.0f, 0.0f, -2.5f));
				pTile->CharacterSGN2.init(&pTile->CharacterTransformSGN2, &pTile->CharacterData2.Character);
				pTile->CharacterSGN2.enable(false, false);

				/*pTile->StickFigureTransformSGN.init(&pTile->RootSGN);
				pTile->StickFigureTransformSGN.scale(Vector3f(Scale, Scale, Scale));
				pTile->StickFigureSGN.init(&pTile->StickFigureTransformSGN, &pTile->CharacterData.StickFigure);*/

				int32_t FontSize = 20;
				pTile->VideoDescriptionLabel.init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, FontSize, true));
				pTile->VideoDescriptionLabel.color(0.0f, 0.0f, 0.0f, 1.0f);	
				pTile->VideoDescriptionLabel.text("Video Description Label");
			}
			updateMotion(&m_DisplayTiles[0]->CharacterData, "MyAssets/1_f_63_o.bvh");
			updateMotion(&m_DisplayTiles[1]->CharacterData, "MyAssets/0_m_54_o.bvh");


			updateViewportsAndCamera();

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			m_HelpTexts.push_back(pKeybindings);
			m_DrawHelpTexts = true;

			m_IsRecording = false;

			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}

			// side camera 1
			Vector3f CamPos = Vector3f(13.23f, 2.68f, 13.64f);
			Vector3f CamDir = Vector3f(-0.92f, -0.04f, -0.38f);

			// side camera 2
			/*CamPos = Vector3f(12.28, 3.61, 17.68);
			CamDir = Vector3f(-0.82, -0.11, -0.56);*/

			// frontal camera
			/*CamPos = Vector3f(-1.39, 3.73, 28.07);
			CamDir = Vector3f(0.08, -0.11, -0.99);*/

			// close side camera (For top bottom)
			CamPos = Vector3f(14.30, 2.70, 16.00);
			CamDir = Vector3f(-0.94, -0.05, -0.32);

			// side camera for 2 characters
			/*CamPos = Vector3f(20.53, 3.15, 18.82);
			CamDir = Vector3f(-0.87, -0.07, -0.49);*/

			m_Cam.lookAt(CamPos, CamPos + CamDir);

			//m_RenderWin.vsync(true, 2);


			/*m_MotionFiles.push_back("MyAssets/20230510/0_f_model_female_5_skeleton_female_5.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/1_m_model_female_5_skeleton_male_9_scaled_stride_length.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/2_f_model_female_12_skeleton_12.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/3_f_model_female_group_skeleton_12.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/4_f_model_female_8_skeleton_8_without_modification.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/5_f_model_female_8_skeleton_8_with_mean_pose_and_range_modification.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/6_m_model_female_12_skeleton_male_9.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/7_f_model_female_group_inc_12_skeleton_12.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/8_f_model_female_28_skeleton_female_28.bvh");
			m_MotionFiles.push_back("MyAssets/20230510/9_f_model_female_group24_skeleton_female_28.bvh");

			m_Combinations.push_back(pair<int32_t, int32_t>(8, 9));*/

			m_MotionFiles.push_back("MyAssets/2023-06-23/0_m_54.bvh");
			m_MotionFiles.push_back("MyAssets/2023-06-23/1_m_54_neutral.bvh");
			m_MotionFiles.push_back("MyAssets/2023-06-23/2_f_63.bvh");
			m_MotionFiles.push_back("MyAssets/2023-06-23/3_f_63_neutral.bvh");
			

			m_Combinations.push_back(pair<int32_t, int32_t>(1, 0));
			m_Combinations.push_back(pair<int32_t, int32_t>(3, 2));

		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear


		void mainLoop(void)override {
			m_RenderWin.update();
			if (m_IsRecording) {
				m_SG.update(1.0f); // 30 fps
				m_SkyboxSG.update(0.0f); // background should not move as it is distracting
				for (auto i : m_DisplayTiles) 
				{
					i->CharacterData.Controller.update(1.0f);
					i->CharacterData2.Controller.update(1.0f);
				}
			}
			else {
				m_SG.update(60.0f / m_FPS);
				m_SkyboxSG.update(60.0f / m_FPS);
				for (auto i : m_DisplayTiles) 
				{
					i->CharacterData.Controller.update(60.0f / m_FPS);
					i->CharacterData2.Controller.update(60.0f / m_FPS);
				}
			}
			
			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());


			for (uint8_t i = 0; i < m_TileCount; ++i) {
				// render scene as usual
				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
				m_DisplayTiles[i]->SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_RenderDev.activeCamera(&m_Cam);
				m_DisplayTiles[i]->SG.render(&m_RenderDev);

				// set viewport and perform lighting pass
				// this will produce the correct tile in the final output window (backbuffer to be specific)
				m_RenderDev.viewport(RenderDevice::RENDERPASS_LIGHTING, m_DisplayTiles[i]->VP);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, (i == 0) ? true : false);

				m_RenderDev.viewport(RenderDevice::RENDERPASS_FORWARD, m_DisplayTiles[i]->VP);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.render(&m_RenderDev);
				
				m_DisplayTiles[i]->VideoDescriptionLabel.render(&m_RenderDev);
			}

			m_RenderDev.viewport(RenderDevice::RENDERPASS_FORWARD, m_FullScreenVP);
			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);

			//for (uint8_t i = 0; i < m_TileCount; ++i) m_DisplayTiles[i]->VideoDescriptionLabel.render(&m_RenderDev);

			//m_FPSLabel.render(&m_RenderDev);
			//if (m_DrawHelpTexts) drawHelpTexts();

			if (m_IsRecording) {
				T2DImage<uint8_t> Img;
				CForgeUtility::retrieveFrameBuffer(&Img);
				m_Recorder.addFrame(&Img, 0);
			}

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_0, true)) {
				if (m_Recorder.isRecording()) {
					m_Recorder.finish();
					printf("Finished screen recording!\n");
					m_IsRecording = false;
				}
				else {
					m_Recorder.init("MyAssets\\Recording.mp4", 30.0f);
					printf("Screen recording started!");
					m_IsRecording = true;
				}
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_P, true)) {
				for (auto i : m_DisplayTiles) {
					if(i->CharacterData.Controller.animationCount() > 0)					{
						i->CharacterData.pActiveAnimation = i->CharacterData.Controller.createAnimation(0, 1.0f, 0.0f);
						i->CharacterData.Character.activeAnimation(i->CharacterData.pActiveAnimation);
					}
					if (i->CharacterData2.Controller.animationCount() > 0) {
						i->CharacterData2.pActiveAnimation = i->CharacterData.Controller.createAnimation(0, 1.0f, 0.0f);
						i->CharacterData2.Character.activeAnimation(i->CharacterData.pActiveAnimation);
					}
				}
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_H, true)) {
				for (auto i : m_DisplayTiles) {
					if (i->CharacterData.Character.activeAnimation() != nullptr) {
						if (i->CharacterData.Character.activeAnimation()->Speed <= 0.02f) {
							i->CharacterData.Character.activeAnimation()->Speed = 1;
						}
						else {
							i->CharacterData.Character.activeAnimation()->Speed = 0.0f;
						}
					}
					if (i->CharacterData2.Character.activeAnimation() != nullptr) {
						if (i->CharacterData2.Character.activeAnimation()->Speed <= 0.02f) {
							i->CharacterData2.Character.activeAnimation()->Speed = 1;
						}
						else {
							i->CharacterData2.Character.activeAnimation()->Speed = 0.0f;
						}
					}
				}//for[display tiles]
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_C, true)) {
				Vector3f CamDir = m_Cam.dir();
				Vector3f CamPos = m_Cam.position();

				printf("Camera: (%.2f, %.2f, %.2f) | (%.2f, %.2f, %.2f)", CamPos[0], CamPos[1], CamPos[2], CamDir[0], CamDir[1], CamDir[2]);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT, false) && m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_R, true)) {
				generateVideoClipsTopBottom(1.0f, "_frontal");
				generateVideoClipsTopBottom(0.25f, "_frontal_slow");
			}
			else if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_R, true)) {
				generateVideoClipsTopBottom(1.0f, "");
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_T, true)) {
				generateVideoClips2Characters(1.0f, "");
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_Z, true)) {
				// rendering of 2 characters
				Vector3f CamPos = Vector3f(20.53, 3.15, 18.82);
				Vector3f CamDir = Vector3f(-0.87, -0.07, -0.49);
				m_Cam.lookAt(CamPos, CamPos + CamDir);
				m_TileCount = 1;
				updateViewportsAndCamera();
				generateVideoClips2Characters(1.0f, "");

				// rendering characters top and bottom
				CamPos = Vector3f(12.03, 2.58, 10.22);
				CamDir = Vector3f(-0.96, -0.05, -0.29);
				m_Cam.lookAt(CamPos, CamPos + CamDir);
				m_TileCount = 2;
				updateViewportsAndCamera();
				generateVideoClipsTopBottom(1.0f, "");
			}


		}//mainLoop

	protected:
		struct DisplayModel {
			int32_t ID;
			std::string FilePath;
			std::vector<std::pair<int32_t, std::string>> AlbedoReplacements; // pair<int, string> = pair<material id, texture path>
			std::vector<std::pair<int32_t, std::string>> NormalReplacements; // pair<int, string> = pair<material id, texture path>
		};

		struct CharacterModelData {
			SkeletalAnimationController Controller;
			SkeletalAnimationController::Animation* pActiveAnimation;
			SkeletalActor Character;
			StickFigureActor StickFigure;
		};

		struct DisplayTile {
			RenderDevice::Viewport VP;
			SceneGraph SG;
			SGNTransformation RootSGN;

			SGNTransformation GroundPlaneTransformSGN;
			SGNGeometry GroundPlaneSGN;

			CharacterModelData CharacterData;
			SGNTransformation CharacterTransformSGN;
			SGNGeometry CharacterSGN;

			CharacterModelData CharacterData2;
			SGNTransformation CharacterTransformSGN2;
			SGNGeometry CharacterSGN2;

			SGNTransformation StickFigureTransformSGN;
			SGNGeometry StickFigureSGN;

			LineOfText VideoDescriptionLabel;
		};

		void generateVideoClipsTopBottom(float Speedscale, const std::string Tag) {

			m_DisplayTiles[0]->CharacterSGN2.enable(false, false);

			for (auto i : m_Combinations) {
				printf("Recording Combination %d-%d ... ", i.first, i.second);
				// set motion data
				std::string TopMotionFile = m_MotionFiles[i.first];
				std::string BottomMotionFile = m_MotionFiles[i.second];
				updateMotion(&m_DisplayTiles[0]->CharacterData, TopMotionFile);
				updateMotion(&m_DisplayTiles[1]->CharacterData, BottomMotionFile);

				m_DisplayTiles[0]->VideoDescriptionLabel.text(File::retrieveFilename(m_MotionFiles[i.first]));
				m_DisplayTiles[1]->VideoDescriptionLabel.text(File::retrieveFilename(m_MotionFiles[i.second]));

				// start animation
				for (auto i : m_DisplayTiles) {
					if (i->CharacterData.Controller.animationCount() > 0) {
						i->CharacterData.pActiveAnimation = i->CharacterData.Controller.createAnimation(0, 1.0f*Speedscale, 0.0f);
						i->CharacterData.Character.activeAnimation(i->CharacterData.pActiveAnimation);
					}
				}

				string OutFileName = "MyAssets/" + to_string(i.first) + "b_" + to_string(i.second) + "t" + Tag + ".mp4";
				m_Recorder.init(OutFileName, 60.0f);
				m_IsRecording = true;

				// play animation until finished
				bool Finished = false;
				while (!Finished) {
					mainLoop();

					if (m_DisplayTiles[0]->CharacterData.Character.activeAnimation() == nullptr && m_DisplayTiles[1]->CharacterData.Character.activeAnimation() == nullptr) {
						Finished = true;
						m_IsRecording = false;
					}
				}

				m_Recorder.finish();

				printf("done\n");

			}//for[all combinations]

		}//generateVideoClips

		void generateVideoClips2Characters(float Speedscale, const std::string Tag) {

			for (auto i : m_Combinations) {
				printf("Recording Combination %d-%d ... ", i.first, i.second);
				// set motion data
				std::string FrontMotionFile = m_MotionFiles[i.first];
				std::string BackMotionFile = m_MotionFiles[i.second];
				updateMotion(&m_DisplayTiles[0]->CharacterData, FrontMotionFile);
				updateMotion(&m_DisplayTiles[0]->CharacterData2, BackMotionFile);

				m_DisplayTiles[0]->CharacterSGN2.enable(true, true);

				m_DisplayTiles[0]->VideoDescriptionLabel.text(
					File::retrieveFilename(m_MotionFiles[i.first]) + " | " +
					File::retrieveFilename(m_MotionFiles[i.second])
				);


				// start animation
				for (auto i : m_DisplayTiles) {
					if (i->CharacterData.Controller.animationCount() > 0) {
						i->CharacterData.pActiveAnimation = i->CharacterData.Controller.createAnimation(0, 1.0f * Speedscale, 0.0f);
						i->CharacterData.Character.activeAnimation(i->CharacterData.pActiveAnimation);
					}
					if (i->CharacterData2.Controller.animationCount() > 0) {
						i->CharacterData2.pActiveAnimation = i->CharacterData.Controller.createAnimation(0, 1.0f * Speedscale, 0.0f);
						i->CharacterData2.Character.activeAnimation(i->CharacterData.pActiveAnimation);
					}
				}

				string OutFileName = "MyAssets/" + to_string(i.first) + "front_" + to_string(i.second) + "back" + Tag + ".mp4";
				m_Recorder.init(OutFileName, 60.0f);
				m_IsRecording = true;

				// play animation until finished
				bool Finished = false;
				while (!Finished) {
					mainLoop();

					if (m_DisplayTiles[0]->CharacterData.Character.activeAnimation() == nullptr && m_DisplayTiles[0]->CharacterData2.Character.activeAnimation() == nullptr) {
						Finished = true;
						m_IsRecording = false;
					}
				}

				m_Recorder.finish();

				printf("done\n");

			}//for[all combinations]

		}//generateVideoClips

		void updateViewportsAndCamera(void) {
			uint32_t RenderWinWidth = m_RenderWin.width();
			uint32_t RenderWinHeight = m_RenderWin.height();

			Vector2i VPSize = Vector2i::Zero();

			if (m_TileCount == 1) {
				VPSize.x() = std::max(8u, m_RenderWin.width());
				VPSize.y() = std::max(8u, m_RenderWin.height());

				m_DisplayTiles[0]->VP.Position = Vector2i(0, 0);
				m_DisplayTiles[0]->VP.Size = VPSize;
			}
			else if (m_TileCount == 2) {
				VPSize.x() = std::max(8u, m_RenderWin.width());
				VPSize.y() = std::max(8u, m_RenderWin.height() / 2);

				m_DisplayTiles[0]->VP.Position = Vector2i(0, 0);
				m_DisplayTiles[1]->VP.Position = Vector2i(0, m_RenderWin.height()/2);

				m_DisplayTiles[0]->VP.Size = VPSize;
				m_DisplayTiles[1]->VP.Size = VPSize;

			}
			else if (m_TileCount == 4) {
				uint32_t Margin = 0;
				VPSize = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) - 2 * Vector2i(Margin, Margin) + Vector2i(Margin / 2, Margin / 2);

				m_DisplayTiles[0]->VP.Position = Vector2i(0, RenderWinHeight / 2) + Vector2i(Margin, Margin / 2);

				// Top left
				//m_VPs[0].Position = Vector2i(0, RenderWinHeight / 2) + Vector2i(Margin, Margin / 2);
				m_DisplayTiles[0]->VP.Position = Vector2i(0, RenderWinHeight / 2) + Vector2i(Margin, Margin / 2);
				// top right
				//m_VPs[1].Position = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) + Vector2i(Margin / 2, Margin / 2);
				m_DisplayTiles[1]->VP.Position = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) + Vector2i(Margin / 2, Margin / 2);
				// bottom left
				//m_VPs[2].Position = Vector2i(Margin, Margin);
				m_DisplayTiles[2]->VP.Position = Vector2i(Margin, Margin);
				// bottom right
				//m_VPs[3].Position = Vector2i(RenderWinWidth / 2, 0) + Vector2i(Margin / 2, Margin);
				m_DisplayTiles[3]->VP.Position = Vector2i(RenderWinWidth / 2, 0) + Vector2i(Margin / 2, Margin);

				for (uint8_t i = 0; i < 4; ++i) m_DisplayTiles[i]->VP.Size = VPSize;
			}

			for (uint8_t i = 0; i < m_TileCount; ++i) {
				m_DisplayTiles[i]->VideoDescriptionLabel.position(10, 5); // pTile->VP.Size.y() - FontSize - 10);
				m_DisplayTiles[i]->VideoDescriptionLabel.canvasSize(m_DisplayTiles[i]->VP.Size.x(), m_DisplayTiles[i]->VP.Size.y());
			}

			

			m_RenderDev.gBuffer()->init(VPSize[0], VPSize[1]);

			//m_Cam.projectionMatrix(m_VPs[0].Size[0], m_VPs[0].Size[1], CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);
			m_Cam.projectionMatrix(VPSize[0], VPSize[1], CForgeMath::degToRad(45.0f/2), 0.1f, 1000.0f);

			m_GBufferVP.Size = Vector2i(m_RenderDev.gBuffer()->width(), m_RenderDev.gBuffer()->height());
			m_RenderDev.viewport(RenderDevice::RENDERPASS_GEOMETRY, m_GBufferVP);

			// viewport for forward pass (required for correct screenshots)
			m_FullScreenVP.Position = Vector2i(0, 0);
			m_FullScreenVP.Size = Vector2i(RenderWinWidth, RenderWinHeight);

		}//updateViewports

		void listen(GLWindowMsg Msg) override {
			ExampleSceneBase::listen(Msg);

			m_FPSLabel.position(m_FPSLabel.position() - Vector2f(10, 10));

			updateViewportsAndCamera();
		}

		void initDisplayModels(void) {

			DisplayModel* pM = new DisplayModel();
			pM->ID = m_DisplayModels.size();
			pM->FilePath = "MyAssets/Models/B02FemaleModel2.gltf";
			pM->AlbedoReplacements.push_back(std::pair(0, "MyAssets/Models/Textures/young_lightskinned_female_diffuse.png"));
			pM->AlbedoReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/female_casualsuit01_diffuse.png"));
			pM->AlbedoReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/brown_eye.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(3, "MyAssets/Models/Textures/shoes06_diffuse.jpg"));
			pM->NormalReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/female_casualsuit01_normal.png"));

			m_DisplayModels.push_back(pM);

			pM = new DisplayModel();
			pM->ID = m_DisplayModels.size();
			pM->FilePath = "MyAssets/Models/B02MaleModel.gltf";
			pM->AlbedoReplacements.push_back(std::pair(0, "MyAssets/Models/Textures/young_lightskinned_male_diffuse2.png"));
			pM->AlbedoReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/male_casualsuit04_diffuse.png"));
			pM->AlbedoReplacements.push_back(std::pair(1, "MyAssets/Models/Textures/brown_eye.jpg"));
			pM->AlbedoReplacements.push_back(std::pair(3, "MyAssets/Models/Textures/shoes06_diffuse.jpg"));
			pM->NormalReplacements.push_back(std::pair(2, "MyAssets/Models/Textures/male_casualsuit04_normal.png"));
			m_DisplayModels.push_back(pM);

		}//initDisplayModels

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


			LocalTransform(0, 3) = x;
			LocalTransform(1, 3) = y;
			LocalTransform(2, 3) = z;
			if (!DryRun)	pBone->OffsetMatrix = LocalTransform.inverse();

			// recursion
			for (auto i : pBone->Children) transformSkeleton(i, LocalTransform, pKeyframes, pMinMax, ConsiderTranslationOnly, DryRun);

		}//transformSkeleton

		void recomputeInverseBindPoseMatrix(T3DMesh<float>* pMesh, bool ScaleRootAnimation, bool ConsiderTranslationOnly, Vector2f *pLegHeights) {
			if (nullptr == pMesh->rootBone()) throw CForgeExcept("Mesh contains no root bone!");
			if (0 == pMesh->skeletalAnimationCount()) throw CForgeExcept("Mesh contains no animations!");

			auto* pRoot = pMesh->rootBone();
			auto* pAnim = pMesh->getSkeletalAnimation(0);

			Eigen::Vector4f MinMax[3] = { Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero() };

			//for(auto i: pRoot->Children) transformSkeleton(i, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, false);
			transformSkeleton(pRoot, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, ConsiderTranslationOnly, false);

			if (pLegHeights != nullptr) {
				pLegHeights->x() = MinMax[1].w() - MinMax[1].z();
				pLegHeights->y() = MinMax[1].y() - MinMax[1].x();
			}

			if (ScaleRootAnimation) {
				// scale vertices of mesh
				std::vector<Eigen::Vector3f> Vertices;
				Eigen::Matrix3f ScaleMatrix = Matrix3f::Identity();
				ScaleMatrix(0, 0) = (MinMax[0].w() - MinMax[0].z()) / (MinMax[0].y() - MinMax[0].x());
				ScaleMatrix(1, 1) = (MinMax[1].w() - MinMax[1].z()) / (MinMax[1].y() - MinMax[1].x());
				ScaleMatrix(2, 2) = (MinMax[2].w() - MinMax[2].z()) / (MinMax[2].y() - MinMax[2].x());

				for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
					auto v = ScaleMatrix * pMesh->vertex(i);
					Vertices.push_back(v);
				}
				//pMesh->vertices(&Vertices);

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

		void updateMotion(CharacterModelData* pData, const std::string MotionData, SGNTransformation *pTrans = nullptr, SGNTransformation *pStickTrans = nullptr) {
			string Filename = File::retrieveFilename(MotionData);
			DisplayModel* pDM = (Filename[2] == 'm') ? m_DisplayModels[1] : m_DisplayModels[0];
			T3DMesh<float> M;
			AssetIO::load(pDM->FilePath, &M);
			for (auto i : pDM->AlbedoReplacements) M.getMaterial(i.first)->TexAlbedo = i.second;
			for (auto i : pDM->NormalReplacements) M.getMaterial(i.first)->TexNormal = i.second;
			M.computePerVertexNormals();

			if (MotionData.size() > 0) {
				T3DMesh<float> Anim;
				AssetIO::load(MotionData, &Anim);

				M.clearSkeletalAnimations();
				for (uint32_t i = 0; i < Anim.skeletalAnimationCount(); ++i) M.addSkeletalAnimation(Anim.getSkeletalAnimation(i), true);
				Vector2f LegHeights;
				recomputeInverseBindPoseMatrix(&M, false, true, &LegHeights);
				/*float Diff = LegHeights.y() - LegHeights.x();
				float Scale = LegHeights.x() / LegHeights.y();
				Vector3f T = pTrans->translation();
				Vector3f S = pTrans->scale();
				T.y() = Diff * pTrans->scale().y();
				S = S.cwiseProduct(Vector3f(Scale, Scale, Scale));*/
				//pTrans->translation(T);
				/*pTrans->scale(S);*/
				//pStickTrans->translation(-T);


			}
			

			BoundingVolume BV;

			pData->Controller.init(&M, true);

			pData->Character.init(&M, &pData->Controller);
			pData->Character.boundingVolume(BV);

			pData->StickFigure.init(&M, &pData->Controller);
			pData->StickFigure.jointSize(pData->StickFigure.jointSize() / 1.5f);
			pData->StickFigure.boneSize(pData->StickFigure.boneSize() * 0.75f);

		}//updateMotion

		std::vector<DisplayModel*> m_DisplayModels; // 0 is female model, 1 is male model

		// Scene Graph
		SGNTransformation m_RootSGN;


		VideoRecorder m_Recorder;
		bool m_IsRecording;

		RenderDevice::Viewport m_GBufferVP;
		//RenderDevice::Viewport m_VPs[4];
		RenderDevice::Viewport m_FullScreenVP;

		std::vector<DisplayTile*> m_DisplayTiles;

		int32_t m_TileCount;

		std::vector<string> m_MotionFiles;
		std::vector<std::pair<int32_t, int32_t>> m_Combinations;

	};//SideBySideMotionScene

}//name space

#endif

#endif // USE_OPENCV