/*****************************************************************************\
*                                                                           *
* File(s): ShadowTestScene.hpp                                            *
*                                                                           *
* Content: Class to interact with an MF52 NTC Thermistor by using a basic   *
*          voltage divider circuit.                                         *
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
#ifndef __CFORGE_SHADOWTESTSCENE_HPP__
#define __CFORGE_SHADOWTESTSCENE_HPP__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../Examples/exampleSceneBase.hpp"
#include "../../CForge/Graphics/Actors/SkeletalActor.h"
#include "../../CForge/Graphics/Lights/SpotLight.h"

namespace CForge {

	class ShadowTestScene : public ExampleSceneBase {
	public:
		ShadowTestScene(void) {
			m_WindowTitle = "CrossForge Prototype - Shadow Test Scene";

			m_ObjCount = 500;
			m_ObjTypeCount = 2;
		}//Constructor

		~ShadowTestScene(void) {
			clear();
		}//Destructor

		void init(void) {
			
			//GLWindow RenderWin;
			m_RenderWin.init(Eigen::Vector2i(0, 0), Eigen::Vector2i(m_WinWidth, m_WinHeight), m_WindowTitle);
			m_RenderWin.startListening(this);

			m_pShaderMan = SShaderManager::instance();

			gladLoadGL();

			// RenderDevice
			ShaderCode::LightConfig LC;
			LC.DirLightCount = 2;
			LC.PointLightCount = 2;
			LC.SpotLightCount = 1;
			LC.ShadowBias = 0.000002f;
			LC.PCFSize = 1;
			LC.ShadowMapCount = 1;
			m_pShaderMan->configShader(LC);

			RenderDevice::RenderDeviceConfig RDConfig;
			RDConfig.init();
			RDConfig.UseGBuffer = true;
			RDConfig.GBufferWidth = m_WinWidth;
			RDConfig.GBufferHeight = m_WinHeight;
			RDConfig.pAttachedWindow = &m_RenderWin;
			RDConfig.DirectionalLightsCount = LC.DirLightCount;
			RDConfig.PointLightsCount = LC.PointLightCount;
			RDConfig.SpotLightsCount = LC.SpotLightCount;
			RDConfig.PhysicallyBasedShading = true;

			ShaderCode::PostProcessingConfig PPC;
			PPC.Exposure = 1.0f;
			PPC.Gamma = 2.2f;
			PPC.Saturation = 1.1f;
			PPC.Brightness = 1.05f;
			PPC.Contrast = 1.05f;
			m_pShaderMan->configShader(PPC);

			m_RenderDev.init(&RDConfig);

			m_Cam.init(Eigen::Vector3f(), Eigen::Vector3f());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, 45.0f * 3.14f / 180.0f, 0.1f, 1000.0f);
			m_Cam.position(Eigen::Vector3f(0.0f, 30.0f, 125.0f));
			m_RenderDev.activeCamera(&m_Cam);

			
			m_PointLights[0].init(Eigen::Vector3f(0.0f, 20.0f, 0.0f), -Eigen::Vector3f(0.0f, 10.0f, 0.0f).normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 0.2f, Eigen::Vector3f(0.5, 0.01, 0.0));
			m_PointLights[1].init(Eigen::Vector3f(100.0f, 20.0f, -100.0f), -Eigen::Vector3f(0.0f, 20.0f, 20.0f).normalized(), Eigen::Vector3f(1.0f, 1.0, 1.0f), 0.5f, Eigen::Vector3f(0.0, 0.2, 0.02));

			
			float InnerCutOff = GraphicsUtility::degToRad(15.0f);
			float OuterCutOff = GraphicsUtility::degToRad(25.0f);
			Eigen::Vector3f SpotPosition = Eigen::Vector3f(10.0f, 200.0f, 50.0f);
			m_Spot.init(SpotPosition, -SpotPosition.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Eigen::Vector3f(0.5f, 0.0f, 0.0f), Eigen::Vector2f(InnerCutOff, OuterCutOff));


			Eigen::Vector3f Sun1Pos(120.0f, 250.0f, -50.0f);
			Eigen::Vector3f Sun2Pos(50.0f, 50.0f, 150.0f);

			const uint32_t ShadowMapDim = 4096;
			m_Sun1.init(Sun1Pos, -Sun1Pos.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 4.5f);
			m_Sun1.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(125, 125), 0.5f, 1000.0f);

			m_Sun2.init(Sun2Pos, -Sun2Pos.normalized(), Eigen::Vector3f(1.0f, 0.9f, 0.9f), 5.1f);
			//Sun2.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(150, 150), 0.5f, 1000.0f);

			m_RenderDev.addLight(&m_Sun1);
			m_RenderDev.addLight(&m_Sun2);
			m_RenderDev.addLight(&m_PointLights[0]);
			m_RenderDev.addLight(&m_PointLights[1]);


			T3DMesh<float> M;
			SAssetIO::load("MyAssets/TexturedGround.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/TexturedUnitCube.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			m_Cube.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/TexturedUnitCube.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			m_Sphere.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Armchair/Armchair.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Armchair.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/ManMulti.fbx", &M);

			setMeshShader(&M, 0.6f, 0.04f);

			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_male_diffuse2.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/male_casualsuit04_diffuse.png";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";

			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_EricAnimController.init(&M, true);
			m_Eric.init(&M, &m_EricAnimController);
			M.clear();

			m_pEricAnim = m_EricAnimController.createAnimation(0, 1.0f, 0.0f);
			//SkeletalAnimationController::Animation* pEricAnim = EricAnimController.createAnimation(0, 1.0f, 0.0f);
			m_Eric.activeAnimation(m_pEricAnim);

			std::string GLError;
			if (GraphicsUtility::checkGLError(&GLError)) {
				printf("GL ErrorOccured: %s\n", GLError.c_str());
			}

			bool RotationMode = false;

			Eigen::Vector2f MouseDelta = Eigen::Vector2f(0.0f, 0.0f);
			Eigen::Vector2f MousePos = Eigen::Vector2f(0, 0);

			Eigen::Vector3f GroundPos(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f GroundScale(300.0f, 300.0f, 300.0f);
			Eigen::Quaternionf GroundRot;
			GroundRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());

			m_SGNRoot.init(nullptr, Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity(), Eigen::Vector3f::Ones());
			m_GroundSGN.init(&m_SGNRoot, &m_Ground, GroundPos, GroundRot, GroundScale);

			m_Armchair01Transform.init(&m_SGNRoot);
			Quaternionf ACRotDelta;
			ACRotDelta = AngleAxisf(GraphicsUtility::degToRad(-45.0f / 60.0f), Vector3f::UnitY());
			m_Armchair01Transform.rotationDelta(ACRotDelta);

			m_Armchair01SGN.init(&m_Armchair01Transform, &m_Head);
			m_Armchair01SGN.scale(Vector3f(0.2f, 0.2f, 0.2f));
			m_Armchair01SGN.position(Vector3f(50.0f, 50.0f, -50.0f));
			Quaternionf HeadRot;
			HeadRot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_Armchair01SGN.rotation(HeadRot);

			float EricScale = 0.25f;
			Quaternionf EricRot;
			EricRot = Quaternionf::Identity();
			EricRot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());


			m_EricTransform.init(&m_SGNRoot);
			Quaternionf R;
			R = AngleAxisf(GraphicsUtility::degToRad(60.0f), Vector3f::UnitY());
			m_EricTransform.rotation(R);
			m_EricTransform.translation(Vector3f(0.0f, -16.0f, 30.0f));
			m_EricSGN.init(&m_EricTransform, &m_Eric, Vector3f(0.0f, 15.0f, 15.0f), EricRot, Vector3f(EricScale, EricScale, EricScale));

			m_SG.init(&m_SGNRoot);

			

			// create random objects
			for (uint32_t i = 0; i < m_ObjCount; ++i) {
				// object type
				m_ObjType.push_back(rand() % m_ObjTypeCount);
				// positions
				Eigen::Vector3f Pos;
				Pos.x() = CoreUtility::randRange<float>(-1.0f, 1.0f) * 100.0f;
				Pos.y() = CoreUtility::randRange<float>(0.0f, 1.0f) * 50.0f;
				Pos.z() = CoreUtility::randRange<float>(-1.0f, 1.0f) * 100.0f;
				m_ObjPositions.push_back(Pos);

				Eigen::Vector3f Move;
				Move.x() = CoreUtility::randRange<float>(-1.0f, 1.0f);
				Move.y() = CoreUtility::randRange<float>(-1.0f, 1.0f);
				Move.z() = CoreUtility::randRange<float>(-1.0f, 1.0f);

				Move.x() = Move.y() = Move.z() = 0.0f;
				m_ObjMovements.push_back(Move);
				if (m_ObjMovements[i].y() > -0.1f && m_ObjMovements[i].y() < 0.1f) m_ObjMovements[i].y() = 0.2f;

				float RotY = CoreUtility::randRange<float>(-1.0f, 1.0f) * 90.0f;
				Eigen::Quaternionf R;
				R = Eigen::AngleAxisf(GraphicsUtility::degToRad(RotY), Eigen::Vector3f::UnitY());
				m_ObjRotations.push_back(R);

				R = Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) / 30.0f, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) / 60.0f, Eigen::Vector3f::UnitX());

				m_ObjRotationsDelta.push_back(R);

				float Scale = 0.25f + CoreUtility::randRange<float>(0.0f, 1.0f) * 1.0f;
				m_ObjScales.push_back(Eigen::Vector3f(Scale, Scale, Scale));
			}//for[generate objects]

			Eigen::Vector3f ArmchairPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f PicturePos = Eigen::Vector3f(0.0f, 50.0f, -35.0f);
			float ArmchairScale = 1.0f / 4.0f;
			float PictureScale = 1.0f / 2.0f;
			Eigen::Quaternionf ArmchairRot = Eigen::Quaternionf::Identity();
			Eigen::Quaternionf PictureRot = Eigen::Quaternionf::Identity();

			PictureRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-45.0f), Eigen::Vector3f::UnitY());

			uint32_t FrameCount = 0;
			uint64_t LastFPSPrint = CoreUtility::timestamp();

			m_TimingShadowPass = 0;
			m_TimingGeometryPass = 0;
			m_TimingLightingPass = 0;
			glGenQueries(1, &m_TimingShadowPass);
			glGenQueries(1, &m_TimingGeometryPass);
			glGenQueries(1, &m_TimingLightingPass);

			bool Wireframe = false;

		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();
		}//clear

		void run(void) {

			Eigen::Vector3f GroundPos(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f GroundScale(300.0f, 300.0f, 300.0f);
			Eigen::Quaternionf GroundRot;
			GroundRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
			Eigen::Vector3f ArmchairPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f PicturePos = Eigen::Vector3f(0.0f, 50.0f, -35.0f);
			float ArmchairScale = 1.0f / 4.0f;
			float PictureScale = 1.0f / 2.0f;
			Eigen::Quaternionf ArmchairRot = Eigen::Quaternionf::Identity();
			Eigen::Quaternionf PictureRot = Eigen::Quaternionf::Identity();
			bool Wireframe = false;

			PictureRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-45.0f), Eigen::Vector3f::UnitY());


			Mouse* pMouse = m_RenderWin.mouse();
			Keyboard* pKeyboard = m_RenderWin.keyboard();

			while (!m_RenderWin.shutdown()) {

				m_FrameCount++;
				if (CoreUtility::timestamp() - m_LastFPSPrint > 2000) {

					m_LastFPSPrint = CoreUtility::timestamp();
					float AvailableMemory = GraphicsUtility::gpuMemoryAvailable() / 1000.0f;
					float MemoryInUse = AvailableMemory - GraphicsUtility::gpuFreeMemory() / 1000.0f;

					uint32_t ShadowPassTime;
					uint32_t GeometryPassTime;
					uint32_t LightingPassTime;
					uint8_t WindowTitle[256];
					sprintf((char*)WindowTitle, "FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB\n", uint32_t(m_FrameCount / 2.0f), MemoryInUse, AvailableMemory);

					if (nullptr != glGetQueryObjectuiv) {
						glGetQueryObjectuiv(m_TimingShadowPass, GL_QUERY_RESULT, &ShadowPassTime);
						glGetQueryObjectuiv(m_TimingGeometryPass, GL_QUERY_RESULT, &GeometryPassTime);
						glGetQueryObjectuiv(m_TimingLightingPass, GL_QUERY_RESULT, &LightingPassTime);
						/*printf("\tTimings: Shadow Pass: %.2fms\n", ShadowPassTime / (1000000.0f));
						printf("\tTiming Geometry Pass: %.2fms\n", GeometryPassTime / (1000000.0f));
						printf("\tTiming Lighting Pass: %.2fms\n", LightingPassTime / (1000000.0f));*/
					}

					m_RenderWin.title("ShadowTestScene [" + std::string((const char*)WindowTitle) + "]");

					m_FrameCount = 0;
				}

				m_EricAnimController.update(1.0f);

				if (m_pEricAnim->t >= m_EricAnimController.animation(m_pEricAnim->AnimationID)->Duration) {
					m_pEricAnim->t = 0.0f;
				}

				Vector2f MWheel = pMouse->wheel();
				if (MWheel.x() > 0.01f || MWheel.x() < -0.01f) {
					printf("Wheel-x: %.2f\n", MWheel.x());
				}
				if (MWheel.y() > 0.01f || MWheel.y() < -0.01f) {
					printf("Wheel-y: %.2f\n", MWheel.y());
				}
				pMouse->wheel(Vector2f(0.0f, 0.0f));

				if (pMouse->buttonState(Mouse::BTN_MIDDLE)) {
					m_Cam.resetToOrigin();
					printf("To origin\n");
					pMouse->buttonState(Mouse::BTN_MIDDLE, false);
				}

				m_RenderWin.update();
				m_SG.update(1.0f);

				defaultCameraUpdate(&m_Cam, pKeyboard, pMouse);

				//move objects
				for (uint32_t i = 0; i < m_ObjCount; ++i) {
					m_ObjPositions[i] += m_ObjMovements[i];
					m_ObjRotations[i] *= m_ObjRotationsDelta[i];

					if (m_ObjPositions[i].y() < 0.0f) {
						m_ObjPositions[i].y() = 0.0f;

						//ObjMovements[i].x() = randFloat();
						m_ObjMovements[i].y() *= -1.0f * CoreUtility::randRange<float>(0.0f, 1.0f);
						//ObjMovements[i].z() = randFloat();

						if (m_ObjMovements[i].y() > -0.1f && m_ObjMovements[i].y() < 0.1f) m_ObjMovements[i].y() = 0.1f;
					}
					else if (m_ObjPositions[i].y() > 100.0f) {
						m_ObjPositions[i].y() = 100.0f;

						//ObjMovements[i].x() = randFloat();
						m_ObjMovements[i].y() *= -1.0f * CoreUtility::randRange<float>(0.0f, 1.0f);
						//ObjMovements[i].z() = randFloat();

						if (m_ObjMovements[i].y() > -0.1f && m_ObjMovements[i].y() < 0.1f) m_ObjMovements[i].y() = -0.1f;
					}
				}

				//// shadow pass
				if (m_Sun1.castsShadows()) {
					glBeginQuery(GL_TIME_ELAPSED, m_TimingShadowPass);
					m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun1);
					m_SG.render(&m_RenderDev);

					Eigen::Quaternionf Rot;
					Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
					// render ground
					m_RenderDev.requestRendering(&m_Ground, Rot, GroundPos, GroundScale);
					m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
					m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
					m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(-50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
					//m_RenderDev.requestRendering(&m_Picture, m_PictureRot, m_PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

					// render objects
					for (uint32_t i = 0; i < m_ObjCount; ++i) {
						StaticActor* pActor = nullptr;
						switch (m_ObjType[i]) {
						case 0: pActor = &m_Cube; break;
						case 1: pActor = &m_Sphere; break;
						default: pActor = &m_Cube; break;

						}
						m_RenderDev.requestRendering(pActor, m_ObjRotations[i], m_ObjPositions[i], m_ObjScales[i]);
					}
					glCullFace(GL_BACK);
					glEndQuery(GL_TIME_ELAPSED);
				}

				if (m_Sun2.castsShadows()) {
					m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun2);
					m_SG.render(&m_RenderDev);

					Eigen::Quaternionf Rot;
					Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
					// render ground
					m_RenderDev.requestRendering(&m_Ground, Rot, GroundPos, GroundScale);
					m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
					m_RenderDev.requestRendering(&m_Picture, PictureRot, PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

					// render objects
					for (uint32_t i = 0; i < m_ObjCount; ++i) {
						StaticActor* pActor = nullptr;
						switch (m_ObjType[i]) {
						case 0: pActor = &m_Cube; break;
						case 1: pActor = &m_Sphere; break;
						default: pActor = &m_Cube; break;
						}
						m_RenderDev.requestRendering(pActor, m_ObjRotations[i], m_ObjPositions[i], m_ObjScales[i]);
					}
				}

				// geometry pass
				if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glBeginQuery(GL_TIME_ELAPSED, m_TimingGeometryPass);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);
				// render ground
				Eigen::Quaternionf Rot;
				Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
				m_RenderDev.requestRendering(&m_Ground, Rot, GroundPos, GroundScale);

				m_RenderDev.requestRendering(&m_Sphere, Eigen::Quaternionf::Identity(), m_Sun1.position(), Eigen::Vector3f(50.0f, 50.0f, 50.0f));
				//RDev.requestRendering(&Cube, Eigen::Quaternionf::Identity(), Sun2.position(), Eigen::Vector3f(50.0f, 50.0f, 50.0f));
				m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				m_RenderDev.requestRendering(&m_Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(-50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				m_RenderDev.requestRendering(&m_Picture, PictureRot, PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

				// render objects
				for (uint32_t i = 0; i < m_ObjCount; ++i) {
					StaticActor* pActor = nullptr;
					switch (m_ObjType[i]) {
					case 0: pActor = &m_Cube; break;
					case 1: pActor = &m_Sphere; break;
					default: pActor = &m_Cube; break;
					}
					m_RenderDev.requestRendering(pActor, m_ObjRotations[i], m_ObjPositions[i], m_ObjScales[i]);
				}
				glEndQuery(GL_TIME_ELAPSED);

				glBeginQuery(GL_TIME_ELAPSED, m_TimingLightingPass);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				// lighting pass
				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
				glEndQuery(GL_TIME_ELAPSED);


				// copy depth buffer and prepare forward rendering pass
				//RDev.activePass(RenderDevice::RENDERPASS_FORWARD);


				/*if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_0)) {
					T2DImage<uint8_t> Img;
					RDev.gBuffer()->retrievePositionBuffer(&Img);
					SAssetIO::store("Assets/Temp/PosBuffer.jpg", &Img);
					RDev.gBuffer()->retrieveNormalBuffer(&Img);
					SAssetIO::store("Assets/Temp/NormalBuffer.jpg", &Img);
					RDev.gBuffer()->retrieveAlbedoBuffer(&Img);
					SAssetIO::store("Assets/Temp/AlbedoBuffer.jpg", &Img);
					if (Sun1.castsShadows()) {
						Sun1.retrieveDepthBuffer(&Img);
						SAssetIO::store("Assets/Temp/Sun1ShadowMap.jpg", &Img);
					}
					if (Sun2.castsShadows()) {
						Sun2.retrieveDepthBuffer(&Img);
						SAssetIO::store("Assets/Temp/Sun2ShadowMap.jpg", &Img);
					}

				}

				if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_9)) {
					T2DImage<uint8_t> ColorBuffer;
					T2DImage<uint8_t> DepthBuffer;
					GraphicsUtility::retrieveFrameBuffer(&ColorBuffer, &DepthBuffer, 0.1f, 1000.0f);
					SAssetIO::store("Assets/Temp/ScreenshotColor.jpg", &ColorBuffer);
					SAssetIO::store("Assets/Temp/ScreenshotDepth.jpg", &DepthBuffer);
				}*/

				m_RenderWin.swapBuffers();

				defaultKeyboardUpdate(pKeyboard);

			}//while[run]
		}//run
	protected:

		DirectionalLight m_Sun1;
		DirectionalLight m_Sun2;
		PointLight m_PointLights[2];
		SpotLight m_Spot;

		StaticActor m_Ground;
		StaticActor m_Cube;
		StaticActor m_Sphere;
		StaticActor m_Armchair;
		StaticActor m_Picture;
		StaticActor m_Head;

		SkeletalActor m_Eric;
		SkeletalAnimationController m_EricAnimController;
		SkeletalAnimationController::Animation* m_pEricAnim;


		SGNTransformation m_SGNRoot;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_Armchair01Transform;
		SGNGeometry m_Armchair01SGN;

		SGNTransformation m_EricTransform;
		SGNGeometry m_EricSGN;


		uint32_t m_ObjCount;
		uint32_t m_ObjTypeCount;
		std::vector<uint32_t> m_ObjType;
		std::vector<Eigen::Vector3f> m_ObjPositions;
		std::vector<Eigen::Vector3f> m_ObjMovements;
		std::vector<Eigen::Vector3f> m_ObjScales;
		std::vector<Eigen::Quaternionf> m_ObjRotations;
		std::vector<Eigen::Quaternionf> m_ObjRotationsDelta;

		uint32_t m_FrameCount;
		uint32_t m_TimingShadowPass;
		uint32_t m_TimingGeometryPass;
		uint32_t m_TimingLightingPass;
	};//ShadowTestScene

	void shadowTest(void) {

		ShadowTestScene Scene;
		Scene.init();
		Scene.run();
		Scene.clear();

	}//shadowTest

}
#endif