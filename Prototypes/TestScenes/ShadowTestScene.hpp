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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SHADOWTESTSCENE_HPP__
#define __CFORGE_SHADOWTESTSCENE_HPP__

#include <crossforge/Graphics/OpenGLHeader.h>

#include <GLFW/glfw3.h>

#include "../../Examples/ExampleSceneBase.hpp"
#include <crossforge/Graphics/Actors/SkeletalActor.h>
#include <crossforge/Graphics/Lights/SpotLight.h>

namespace CForge {

	class ShadowTestScene : public ExampleSceneBase {
	public:
		ShadowTestScene(void) {
			m_WindowTitle = "CrossForge Prototype - Shadow Test Scene";

			m_ObjCount = 0;

		}//Constructor

		~ShadowTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{
			
			//GLWindow RenderWin;
			m_RenderWin.init(Eigen::Vector2i(0, 0), Eigen::Vector2i(m_WinWidth, m_WinHeight), m_WindowTitle);
			m_RenderWin.startListening(this);

			m_pShaderMan = SShaderManager::instance();

#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif

			// RenderDevice
			ShaderCode::LightConfig LC;
			LC.DirLightCount = 2;
			LC.PointLightCount = 2;
			LC.SpotLightCount = 1;
			LC.ShadowBias = 0.000002f;
			LC.PCFSize = 1;
			LC.ShadowMapCount = 2;
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
			m_Cam.position(Eigen::Vector3f(0.0f, 15.0f, 75.0f));
			m_RenderDev.activeCamera(&m_Cam);

			// initialize  lights
			m_PointLights[0].init(Eigen::Vector3f(0.0f, 20.0f, 0.0f), -Eigen::Vector3f(0.0f, 10.0f, 0.0f).normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 0.5f, Eigen::Vector3f(0.5, 0.01, 0.0));
			m_PointLights[1].init(Eigen::Vector3f(100.0f, 20.0f, -100.0f), -Eigen::Vector3f(0.0f, 20.0f, 20.0f).normalized(), Eigen::Vector3f(1.0f, 1.0, 1.0f), 0.2f, Eigen::Vector3f(0.0, 0.2, 0.02));
	
			float InnerCutOff = CForgeMath::degToRad(15.0f);
			float OuterCutOff = CForgeMath::degToRad(25.0f);
			Eigen::Vector3f SpotPosition = Eigen::Vector3f(10.0f, 200.0f, 50.0f);
			m_Spot.init(SpotPosition, -SpotPosition.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Eigen::Vector3f(0.5f, 0.0f, 0.0f), Eigen::Vector2f(InnerCutOff, OuterCutOff));

			Eigen::Vector3f Sun1Pos(120.0f, 100.0f, -50.0f);
			Eigen::Vector3f Sun2Pos(-120.0f, 100.0f, 50.0f);

			const uint32_t ShadowMapDim = 1024;
			m_Sun1.init(Sun1Pos, -Sun1Pos.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 2.5f);
			m_Sun1.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(125, 125), 0.5f, 1000.0f);

			m_Sun2.init(Sun2Pos, -Sun2Pos.normalized(), Eigen::Vector3f(1.0f, 0.9f, 0.9f), 3.1f);
			m_Sun2.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(150, 150), 0.5f, 1000.0f);

			m_RenderDev.addLight(&m_Sun1);
			m_RenderDev.addLight(&m_Sun2);
			m_RenderDev.addLight(&m_PointLights[0]);
			m_RenderDev.addLight(&m_PointLights[1]);

			initSkybox();

			// load assets
			T3DMesh<float> M;
			SAssetIO::load("Assets/ExampleScenes/TexturedGround.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.computePerVertexNormals();
			m_Cube.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.computePerVertexNormals();
			m_Sphere.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Armchair/Armchair.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Armchair.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Helmet/DamagedHelmet.gltf", &M);
			setMeshShader(&M, 0.15f, 0.25f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Helmet.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Gehen_3-005.glb", &M);

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
			if (CForgeUtility::checkGLError(&GLError)) {
				printf("GL ErrorOccured: %s\n", GLError.c_str());
			}

			
			Eigen::Vector3f GroundPos(0.0f, 0.0f, 0.0f);
			Eigen::Vector3f GroundScale(30.0f, 30.0f, 30.0f);
			Eigen::Quaternionf GroundRot = Quaternionf::Identity();
			//GroundRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());

			// initialize scene graph
			m_SGNRoot.init(nullptr);
			m_SG.init(&m_SGNRoot);

			// initialize ground
			m_GroundSGN.init(&m_SGNRoot, &m_Ground, GroundPos, GroundRot, GroundScale);

			m_HelmetTransform.init(&m_SGNRoot);
			Quaternionf ACRotDelta;
			ACRotDelta = AngleAxisf(CForgeMath::degToRad(-45.0f / 60.0f), Vector3f::UnitY());
			m_HelmetTransform.rotationDelta(ACRotDelta);

			m_HelmetGeom.init(&m_HelmetTransform, &m_Helmet);
			m_HelmetGeom.scale(Vector3f(10.0f, 10.0f, 10.0f));
			m_HelmetGeom.position(Vector3f(50.0f, 50.0f, -50.0f));	

			// put some armchairs on the ground
			for (uint8_t i = 0; i < 3; ++i) {

				Vector3f Pos = Vector3f(-35.0f + i * 35.0f, 0.0f, 0.0f);
				float Scale = 0.2f;
				Quaternionf Rot;
				Rot = AngleAxisf(CForgeMath::degToRad(0.0f), Vector3f::UnitX());

				m_ArmchairsTransform[i].init(&m_SGNRoot, Pos);
				m_ArchmairsGeom[i].init(&m_ArmchairsTransform[i], &m_Armchair, Vector3f::Zero(), Rot, Vector3f(Scale, Scale, Scale));

			}//for[create armchairs]


			// create walking character
			float EricScale = 0.25f;
			Quaternionf EricRot;
			EricRot = Quaternionf::Identity();
			//EricRot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_EricTransform.init(&m_SGNRoot);
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(60.0f), Vector3f::UnitY());
			m_EricTransform.rotation(R);
			m_EricTransform.translation(Vector3f(-100.0f, -17.0f, -120.0f));
			m_EricSGN.init(&m_EricTransform, &m_Eric, Vector3f(0.0f, 15.0f, 15.0f), EricRot, Vector3f(EricScale, EricScale, EricScale));

			m_ObjGroupSGN.init(&m_SGNRoot);
			// create random objects
			for (uint32_t i = 0; i < m_ObjCount; ++i) {
				// object type
				// positions
				Eigen::Vector3f Pos;
				Pos.x() = CForgeMath::randRange<float>(-1.0f, 1.0f) * 100.0f;
				Pos.y() = CForgeMath::randRange<float>(0.0f, 1.0f) * 50.0f;
				Pos.z() = CForgeMath::randRange<float>(-1.0f, 1.0f) * 100.0f;

				Eigen::Vector3f Move;
				Move.x() = CForgeMath::randRange<float>(-1.0f, 1.0f);
				Move.y() = CForgeMath::randRange<float>(-1.0f, 1.0f);
				Move.z() = CForgeMath::randRange<float>(-1.0f, 1.0f);
				Move *= 0.5f;
				if (Move.y() > -0.1f && Move.y() < 0.1f) Move.y() = 0.2f;	
				float Scale = 0.5f + CForgeMath::randRange<float>(0.0f, 1.0f) * 2.0f;

				SGNTransformation* pTransform = new SGNTransformation();
				SGNGeometry* pGeom = new SGNGeometry();

				pTransform->init(&m_ObjGroupSGN);
				pTransform->translation(Pos);
				pTransform->translationDelta(Move);

				float RotY = CForgeMath::randRange<float>(-1.0f, 1.0f) * 90.0f;
				Eigen::Quaternionf R;
				R = Eigen::AngleAxisf(CForgeMath::degToRad(RotY), Eigen::Vector3f::UnitY());
				pTransform->rotation(R);

				R = Eigen::AngleAxisf(CForgeMath::randRange<float>(-1.0f, 1.0f) / 30.0f, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(CForgeMath::randRange<float>(-1.0f, 1.0f) / 60.0f, Eigen::Vector3f::UnitX());
				pTransform->rotationDelta(R);

				pGeom->init(pTransform, (CForgeMath::rand() % 2) ? &m_Cube : &m_Sphere);
				pGeom->scale(Vector3f(Scale, Scale, Scale));

				m_ObjTransforms.push_back(pTransform);
				m_ObjGeoms.push_back(pGeom);

			}//for[generate objects]


			m_TimingShadowPass = 0;
			m_TimingGeometryPass = 0;
			m_TimingLightingPass = 0;
			m_TimingForwardPass = 0;
			glGenQueries(1, &m_TimingShadowPass);
			glGenQueries(1, &m_TimingGeometryPass);
			glGenQueries(1, &m_TimingLightingPass);
			glGenQueries(1, &m_TimingForwardPass);

			m_FPS = 60.0f;
			m_LastFPSPrint = CForgeUtility::timestamp();
			m_FrameCount = 0;

		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override {	
			

			while (!m_RenderWin.shutdown()) {

				m_FrameCount++;
				if (CForgeUtility::timestamp() - m_LastFPSPrint > 2000) {

					m_LastFPSPrint = CForgeUtility::timestamp();
					float AvailableMemory = CForgeUtility::gpuMemoryAvailable() / 1000.0f;
					float MemoryInUse = AvailableMemory - CForgeUtility::gpuFreeMemory() / 1000.0f;

					uint32_t ShadowPassTime;
					uint32_t GeometryPassTime;
					uint32_t LightingPassTime;
					uint32_t ForwardPassTime;
					uint8_t WindowTitle[256];
					sprintf((char*)WindowTitle, "FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB\n", uint32_t(m_FrameCount / 2.0f), MemoryInUse, AvailableMemory);
					m_FPS = m_FrameCount / 2.0f;

					if (nullptr != glGetQueryObjectuiv) {
						glGetQueryObjectuiv(m_TimingShadowPass, GL_QUERY_RESULT, &ShadowPassTime);
						glGetQueryObjectuiv(m_TimingGeometryPass, GL_QUERY_RESULT, &GeometryPassTime);
						glGetQueryObjectuiv(m_TimingLightingPass, GL_QUERY_RESULT, &LightingPassTime);
						glGetQueryObjectuiv(m_TimingForwardPass, GL_QUERY_RESULT, &ForwardPassTime);
						
						sprintf((char*)WindowTitle, "FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB (SP: %.1fms | GP: %.1fms | LP: %.1fms | FP: %.1fms)", 
							uint32_t(m_FrameCount / 2.0f), MemoryInUse, AvailableMemory,
							ShadowPassTime/1000000.0f,
							GeometryPassTime/1000000.0f,
							LightingPassTime/1000000.0f,
							ForwardPassTime/1000000.0f);

					}

					m_RenderWin.title(m_WindowTitle + "[" + std::string((const char*)WindowTitle) + "]");

					m_FrameCount = 0;

					printf("FPS: %.2f\n", m_FPS);
				}

				m_EricAnimController.update(60.0f/m_FPS);

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


				for (auto i : m_ObjTransforms) {
					if (i->translation().y() < 0.0f || i->translation().y() > 100.0f) i->translationDelta(i->translationDelta() * -1.0f);
				}//for[all transform nodes]

				m_RenderWin.update();
				m_SG.update(60.0f/m_FPS);

				defaultCameraUpdate(&m_Cam, pKeyboard, pMouse);

				
				//// shadow pass
				glBeginQuery(GL_TIME_ELAPSED, m_TimingShadowPass);
				if (m_Sun1.castsShadows()) {			
					m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun1);
					m_SG.render(&m_RenderDev);	
				}

				if (m_Sun2.castsShadows()) {
					m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun2);
					m_SG.render(&m_RenderDev);
					
				}
				glCullFace(GL_BACK);
				glEndQuery(GL_TIME_ELAPSED);

				// geometry pass
#ifndef __EMSCRIPTEN__
				if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

				glBeginQuery(GL_TIME_ELAPSED, m_TimingGeometryPass);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);
				glEndQuery(GL_TIME_ELAPSED);

				glBeginQuery(GL_TIME_ELAPSED, m_TimingLightingPass);
#ifndef __EMSCRIPTEN__
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
				// lighting pass
				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
				glEndQuery(GL_TIME_ELAPSED);

				glBeginQuery(GL_TIME_ELAPSED, m_TimingForwardPass);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.update(m_FPS/60.0f);
				m_SkyboxSG.render(&m_RenderDev);
				glEndQuery(GL_TIME_ELAPSED);


				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_0, true) ) {
					T2DImage<uint8_t> Img;
					m_RenderDev.gBuffer()->retrievePositionBuffer(&Img);
					SAssetIO::store("Screenshots/PosBuffer.jpg", &Img);
					m_RenderDev.gBuffer()->retrieveNormalBuffer(&Img);
					SAssetIO::store("Screenshots/NormalBuffer.jpg", &Img);
					m_RenderDev.gBuffer()->retrieveAlbedoBuffer(&Img);
					SAssetIO::store("Screenshots/AlbedoBuffer.jpg", &Img);
					if (m_Sun1.castsShadows()) {
						Img.clear();
						m_Sun1.retrieveDepthBuffer(&Img);
						SAssetIO::store("Screenshots/Sun1ShadowMap.jpg", &Img);
					}
					if (m_Sun2.castsShadows()) {
						Img.clear();
						m_Sun2.retrieveDepthBuffer(&Img);
						SAssetIO::store("Screenshots/Sun2ShadowMap.jpg", &Img);
					}

				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_9, true)) {
					T2DImage<uint8_t> ColorBuffer;
					T2DImage<uint8_t> DepthBuffer;
					CForgeUtility::retrieveFrameBuffer(&ColorBuffer, &DepthBuffer, 0.1f, 1000.0f);
					SAssetIO::store("Screenshots/ScreenshotColor.jpg", &ColorBuffer);
					SAssetIO::store("Screenshots/ScreenshotDepth.jpg", &DepthBuffer);
				}

				m_RenderWin.swapBuffers();

				defaultKeyboardUpdate(pKeyboard);

				// change light settings

				// point lights get darker and brighter [0, 2.0f]
				//uint32_t Stamp = (uint32_t)CoreUtility::timestamp();
				//float PLIntensity =  (std::sin(Stamp/1000.0f) + 1.0f);
				//m_PointLights[0].intensity(0.0f);
				//m_PointLights[1].intensity(0.0f);

				////printf("Intensity: %.2f\n", PLIntensity);

				//float ColorTeint = (std::sin(Stamp / 1000.0f) + 1.0f) / 2.0f;
				////m_Sun1.color(Vector3f(ColorTeint, 0.1f, 0.1f));

				//// change Position of second sun
				//float OffsetX = std::sin(Stamp / 5000.0f) * 100.0f;
				//float OffsetZ = std::cos(Stamp / 5000.0f) * 100.0f;
				//Vector3f Pos = m_Sun2.position();
				//Pos.x() = OffsetX;
				//Pos.z() = OffsetZ;
				//Vector3f Dir = -Pos.normalized();
				//m_Sun2.direction(Dir);
				//m_Sun2.position(Pos);

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
		StaticActor m_Helmet;

		SkeletalActor m_Eric;
		SkeletalAnimationController m_EricAnimController;
		SkeletalAnimationController::Animation* m_pEricAnim;


		SGNTransformation m_SGNRoot;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_HelmetTransform;
		SGNGeometry m_HelmetGeom;

		SGNTransformation m_ArmchairsTransform[3];
		SGNGeometry m_ArchmairsGeom[3];

		SGNTransformation m_ObjGroupSGN;

		SGNTransformation m_EricTransform;
		SGNGeometry m_EricSGN;

		uint32_t m_ObjCount;
		std::vector<SGNTransformation*> m_ObjTransforms;
		std::vector<SGNGeometry*> m_ObjGeoms;

		uint32_t m_FrameCount;
		uint32_t m_TimingShadowPass;
		uint32_t m_TimingGeometryPass;
		uint32_t m_TimingLightingPass;
		uint32_t m_TimingForwardPass;

		bool Wireframe = false;
		Mouse* pMouse = m_RenderWin.mouse();
		Keyboard* pKeyboard = m_RenderWin.keyboard();
		

	};//ShadowTestScene


}
#endif