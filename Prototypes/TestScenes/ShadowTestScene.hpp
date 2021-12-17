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

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"
#include "../../CForge/Graphics/Lights/SpotLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"

#include "../../CForge/Graphics/Actors/SkeletalActor.h"

#include "../../Examples/SceneUtilities.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace CForge {

	void shadowTest(void) {
		const bool LowRes = false;
		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 704;
			WinHeight = 576;
		}

		GLWindow RenderWin;
		RenderWin.init(Eigen::Vector2i(0, 0), Eigen::Vector2i(WinWidth, WinHeight), "CrossForge Sandbox");

		Keyboard* pKeyboard = RenderWin.keyboard();
		Mouse* pMouse = RenderWin.mouse();

		GraphicsUtility::GPUTraits Traits = GraphicsUtility::retrieveGPUTraits();
		printf("Created Render window with OpenGL Version: %d.%d\n", Traits.GLMajorVersion, Traits.GLMinorVersion);

		// we have to call this so we can use OpenGL Functions in this program too
		gladLoadGL();

		// create static actors
		StaticActor Ground;
		StaticActor Cube;
		StaticActor Sphere;
		StaticActor Armchair;
		StaticActor Picture;
		StaticActor Head;

		SkeletalActor Eric;

		T3DMesh<float> M;
		SAssetIO::load("Assets/TexturedGround.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		Ground.init(&M);
		M.clear();

		SAssetIO::load("Assets/TexturedCube.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.6f, 0.04f);
		Cube.init(&M);
		M.clear();

		SAssetIO::load("Assets/TexturedSphere.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.6f, 0.04f);
		Sphere.init(&M);
		M.clear();

		SAssetIO::load("Assets/Meeting Room/Armchair/Armchair.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		M.computePerVertexNormals();
		Armchair.init(&M);
		M.clear();

		/*SAssetIO::load("Assets/Head/Pasha_guard_head.obj", &M);
		setMeshShader(&M, 0.6f, 0.04f);
		M.computePerVertexNormals();
		Head.init(&M);
		M.clear();*/

		SAssetIO::load("Assets/Meeting Room/Picture/Picture.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.2f, 0.1f);
		M.computePerVertexNormals();
		Picture.init(&M);
		M.clear();

		SkeletalAnimationController EricAnimController;
		//SAssetIO::load("Assets/Skel/Eric_Anim.fbx", &M);
		//SAssetIO::load("Assets/Skel/SmplMale_test.fbx", &M);
		SAssetIO::load("Assets/Skel/ExportTest2.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.6f, 0.04f);

		M.getMaterial(0)->TexAlbedo = "Assets/Skel/MHTextures/young_lightskinned_male_diffuse2.png";
		M.getMaterial(1)->TexAlbedo = "Assets/Skel/MHTextures/brown_eye.png";
		M.getMaterial(2)->TexAlbedo = "Assets/Skel/MHTextures/male_casualsuit04_diffuse.png";
		M.getMaterial(3)->TexAlbedo = "Assets/Skel/MHTextures/shoes06_diffuse.png";

		if (M.normalCount() == 0) M.computePerVertexNormals();
		EricAnimController.init(&M, true);
		Eric.init(&M, &EricAnimController);
		M.clear();

		SkeletalAnimationController::Animation* pEricAnim = EricAnimController.createAnimation(6, 1.0f, 0.0f);
		//SkeletalAnimationController::Animation* pEricAnim = EricAnimController.createAnimation(0, 1.0f, 0.0f);
		Eric.activeAnimation(pEricAnim);

		std::string GLError;
		if (GraphicsUtility::checkGLError(&GLError)) {
			printf("GL ErrorOccured: %s\n", GLError.c_str());
		}

		const bool FullHD = false;

		// RenderDevice
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig RDConfig;
		RDConfig.init();
		RDConfig.UseGBuffer = true;
		if (FullHD) {
			RDConfig.GBufferWidth = 1920;
			RDConfig.GBufferHeight = 1080;
		}
		else {
			RDConfig.GBufferWidth = 1280;
			RDConfig.GBufferHeight = 720;
		}

		if (LowRes) {
			RDConfig.GBufferHeight = WinHeight;
			RDConfig.GBufferWidth = WinWidth;
		}

		SShaderManager* pSMan = SShaderManager::instance();
		ShaderCode::LightConfig LC;
		LC.DirLightCount = 2;
		LC.PointLightCount = 2;
		LC.SpotLightCount = 1;


		LC.ShadowBias = 0.0002f;
		LC.PCFSize = 1;

		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		RDConfig.pAttachedWindow = &RenderWin;
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
		pSMan->configShader(PPC);

		RDev.init(&RDConfig);
		// two sunlight
		DirectionalLight Sun1;
		DirectionalLight Sun2;

		PointLight PointLights[2];
		PointLights[0].init(Eigen::Vector3f(0.0f, 20.0f, 0.0f), -Eigen::Vector3f(0.0f, 10.0f, 0.0f).normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 0.2f, Eigen::Vector3f(0.5, 0.01, 0.0));
		PointLights[1].init(Eigen::Vector3f(100.0f, 20.0f, -100.0f), -Eigen::Vector3f(0.0f, 20.0f, 20.0f).normalized(), Eigen::Vector3f(1.0f, 1.0, 1.0f), 0.5f, Eigen::Vector3f(0.0, 0.2, 0.02));

		SpotLight Spot;
		float InnerCutOff = GraphicsUtility::degToRad(15.0f);
		float OuterCutOff = GraphicsUtility::degToRad(25.0f);
		Eigen::Vector3f SpotPosition = Eigen::Vector3f(10.0f, 200.0f, 50.0f);
		Spot.init(SpotPosition, -SpotPosition.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Eigen::Vector3f(0.5f, 0.0f, 0.0f), Eigen::Vector2f(InnerCutOff, OuterCutOff));


		Eigen::Vector3f Sun1Pos(120.0f, 250.0f, -50.0f);
		Eigen::Vector3f Sun2Pos(50.0f, 50.0f, 150.0f);

		const uint32_t ShadowMapDim = 4096;
		Sun1.init(Sun1Pos, -Sun1Pos.normalized(), Eigen::Vector3f(1.0f, 1.0f, 1.0f), 4.5f);
		Sun1.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(125, 125), 0.5f, 1000.0f);

		Sun2.init(Sun2Pos, -Sun2Pos.normalized(), Eigen::Vector3f(1.0f, 0.9f, 0.9f), 5.1f);
		//Sun2.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(150, 150), 0.5f, 1000.0f);

		RDev.addLight(&Sun1);
		RDev.addLight(&Sun2);
		RDev.addLight(&PointLights[0]);
		RDev.addLight(&PointLights[1]);
		//RDev.addLight(&Spot);

		VirtualCamera Cam;
		Cam.init(Eigen::Vector3f(), Eigen::Vector3f());
		Cam.projectionMatrix(WinWidth, WinHeight, 45.0f * 3.14f / 180.0f, 0.1f, 1000.0f);
		Cam.position(Eigen::Vector3f(0.0f, 30.0f, 125.0f));
		RDev.activeCamera(&Cam);

		bool RotationMode = false;

		Eigen::Vector2f MouseDelta = Eigen::Vector2f(0.0f, 0.0f);
		Eigen::Vector2f MousePos = Eigen::Vector2f(0, 0);

		Eigen::Vector3f GroundPos(0.0f, 0.0f, 0.0f);
		Eigen::Vector3f GroundScale(300.0f, 300.0f, 300.0f);
		Eigen::Quaternionf GroundRot;
		GroundRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());

		SceneGraph SG;
		SGNTransformation SGNRoot;
		SGNGeometry GroundSGN;
		SGNTransformation Armchair01Transform;
		SGNGeometry Armchair01SGN;



		SGNRoot.init(nullptr, Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Quaternionf::Identity(), Eigen::Vector3f::Ones());
		GroundSGN.init(&SGNRoot, &Ground, GroundPos, GroundRot, GroundScale);

		Armchair01Transform.init(&SGNRoot);
		Quaternionf ACRotDelta;
		ACRotDelta = AngleAxisf(GraphicsUtility::degToRad(-45.0f / 60.0f), Vector3f::UnitY());
		Armchair01Transform.rotationDelta(ACRotDelta);

		Armchair01SGN.init(&Armchair01Transform, &Head);
		Armchair01SGN.scale(Vector3f(0.2f, 0.2f, 0.2f));
		Armchair01SGN.position(Vector3f(50.0f, 50.0f, -50.0f));
		Quaternionf HeadRot;
		HeadRot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		Armchair01SGN.rotation(HeadRot);

		SGNTransformation EricTransform;
		SGNGeometry EricSGN;

		float EricScale = 0.25f;
		Quaternionf EricRot;
		EricRot = Quaternionf::Identity();
		EricRot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());


		EricTransform.init(&SGNRoot);
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(60.0f), Vector3f::UnitY());
		EricTransform.rotation(R);
		EricTransform.translation(Vector3f(0.0f, -16.0f, 30.0f));
		EricSGN.init(&EricTransform, &Eric, Vector3f(0.0f, 15.0f, 15.0f), EricRot, Vector3f(EricScale, EricScale, EricScale));

		SG.init(&SGNRoot);

		const uint32_t ObjCount = 0;
		const uint32_t ObjTypeCount = 2;
		std::vector<uint32_t> ObjType;
		std::vector<Eigen::Vector3f> ObjPositions;
		std::vector<Eigen::Vector3f> ObjMovements;
		std::vector<Eigen::Vector3f> ObjScales;
		std::vector<Eigen::Quaternionf> ObjRotations;
		std::vector<Eigen::Quaternionf> ObjRotationsDelta;

		// create random objects
		for (uint32_t i = 0; i < ObjCount; ++i) {
			// object type
			ObjType.push_back(rand() % ObjTypeCount);
			// positions
			Eigen::Vector3f Pos;
			Pos.x() = CoreUtility::randRange<float>(-1.0f, 1.0f) * 100.0f;
			Pos.y() = CoreUtility::randRange<float>(0.0f, 1.0f) * 50.0f;
			Pos.z() = CoreUtility::randRange<float>(-1.0f, 1.0f) * 100.0f;
			ObjPositions.push_back(Pos);

			Eigen::Vector3f Move;
			Move.x() = CoreUtility::randRange<float>(-1.0f, 1.0f);
			Move.y() = CoreUtility::randRange<float>(-1.0f, 1.0f);
			Move.z() = CoreUtility::randRange<float>(-1.0f, 1.0f);

			Move.x() = Move.y() = Move.z() = 0.0f;
			ObjMovements.push_back(Move);
			if (ObjMovements[i].y() > -0.1f && ObjMovements[i].y() < 0.1f) ObjMovements[i].y() = 0.2f;

			float RotY = CoreUtility::randRange<float>(-1.0f, 1.0f) * 90.0f;
			Eigen::Quaternionf R;
			R = Eigen::AngleAxisf(GraphicsUtility::degToRad(RotY), Eigen::Vector3f::UnitY());
			ObjRotations.push_back(R);

			R = Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) / 30.0f, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) / 60.0f, Eigen::Vector3f::UnitX());

			ObjRotationsDelta.push_back(R);

			float Scale = 0.25f + CoreUtility::randRange<float>(0.0f, 1.0f) * 1.0f;
			ObjScales.push_back(Eigen::Vector3f(Scale, Scale, Scale));
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

		uint32_t TimingShadowPass = 0;
		uint32_t TimingGeometryPass = 0;
		uint32_t TimingLightingPass = 0;
		glGenQueries(1, &TimingShadowPass);
		glGenQueries(1, &TimingGeometryPass);
		glGenQueries(1, &TimingLightingPass);

		bool Wireframe = false;

		while (!RenderWin.shutdown()) {
			FrameCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 2000) {

				LastFPSPrint = CoreUtility::timestamp();
				float AvailableMemory = GraphicsUtility::gpuMemoryAvailable() / 1000.0f;
				float MemoryInUse = AvailableMemory - GraphicsUtility::gpuFreeMemory() / 1000.0f;

				uint32_t ShadowPassTime;
				uint32_t GeometryPassTime;
				uint32_t LightingPassTime;
				uint8_t WindowTitle[256];
				sprintf((char*)WindowTitle, "FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB\n", uint32_t(FrameCount / 2.0f), MemoryInUse, AvailableMemory);

				if (nullptr != glGetQueryObjectuiv) {
					glGetQueryObjectuiv(TimingShadowPass, GL_QUERY_RESULT, &ShadowPassTime);
					glGetQueryObjectuiv(TimingGeometryPass, GL_QUERY_RESULT, &GeometryPassTime);
					glGetQueryObjectuiv(TimingLightingPass, GL_QUERY_RESULT, &LightingPassTime);
					/*printf("\tTimings: Shadow Pass: %.2fms\n", ShadowPassTime / (1000000.0f));
					printf("\tTiming Geometry Pass: %.2fms\n", GeometryPassTime / (1000000.0f));
					printf("\tTiming Lighting Pass: %.2fms\n", LightingPassTime / (1000000.0f));*/
				}

				RenderWin.title("ShadowTestScene [" + std::string((const char*)WindowTitle) + "]");

				FrameCount = 0;
			}

			EricAnimController.update(1.0f);

			if (pEricAnim->t >= EricAnimController.animation(pEricAnim->AnimationID)->Duration) {
				pEricAnim->t = 0.0f;
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
				Cam.resetToOrigin();
				printf("To origin\n");
				pMouse->buttonState(Mouse::BTN_MIDDLE, false);
			}

			RenderWin.update();
			SG.update(1.0f);


			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) && pKeyboard->keyPressed( Keyboard::KEY_9)) {
				PPC.Saturation -= 0.10f;
				pSMan->configShader(PPC);
				pKeyboard->keyState(Keyboard::KEY_9, Keyboard::KEY_RELEASED);
				printf("Saturation now: %.2f\n", PPC.Saturation);
			}
			else if (Keyboard::KEY_PRESSED == pKeyboard->keyState(Keyboard::KEY_9)) {
				PPC.Saturation += 0.10f;
				pSMan->configShader(PPC);
				pKeyboard->keyState(Keyboard::KEY_9, Keyboard::KEY_RELEASED);
				printf("Saturation now: %.2f\n", PPC.Saturation);
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) && pKeyboard->keyPressed(Keyboard::KEY_8)) {
				PPC.Contrast -= 0.05f;
				pSMan->configShader(PPC);
				pKeyboard->keyState(Keyboard::KEY_8, Keyboard::KEY_RELEASED);
				printf("Contrast now: %.2f\n", PPC.Contrast);
			}
			else if (Keyboard::KEY_PRESSED == pKeyboard->keyState(Keyboard::KEY_8)) {
				PPC.Contrast += 0.05f;
				pSMan->configShader(PPC);
				pKeyboard->keyState(Keyboard::KEY_8, Keyboard::KEY_RELEASED);
				printf("Contrast now: %.2f\n", PPC.Contrast);
			}


			// handle input
			if (pKeyboard->keyPressed(Keyboard::KEY_W)) Cam.forward(0.4f);
			if (pKeyboard->keyPressed(Keyboard::KEY_S)) Cam.forward(-0.4f);
			if (pKeyboard->keyPressed(Keyboard::KEY_D)) Cam.right(0.4f);
			if (pKeyboard->keyPressed(Keyboard::KEY_A)) Cam.right(-0.4f);

			
			 //rotation mode?
			if (pMouse->buttonState(Mouse::BTN_RIGHT)) {
				Vector2f MouseDelta = pMouse->movement();
				Cam.rotY(GraphicsUtility::degToRad(MouseDelta.x()) * -0.1f);
				Cam.pitch(GraphicsUtility::degToRad(MouseDelta.y()) * -0.1f);
				pMouse->movement(Vector2f::Zero());

			}

			
			//move objects
			for (uint32_t i = 0; i < ObjCount; ++i) {
				ObjPositions[i] += ObjMovements[i];
				ObjRotations[i] *= ObjRotationsDelta[i];

				if (ObjPositions[i].y() < 0.0f) {
					ObjPositions[i].y() = 0.0f;

					//ObjMovements[i].x() = randFloat();
					ObjMovements[i].y() *= -1.0f * CoreUtility::randRange<float>(0.0f, 1.0f);
					//ObjMovements[i].z() = randFloat();

					if (ObjMovements[i].y() > -0.1f && ObjMovements[i].y() < 0.1f) ObjMovements[i].y() = 0.1f;
				}
				else if (ObjPositions[i].y() > 100.0f) {
					ObjPositions[i].y() = 100.0f;

					//ObjMovements[i].x() = randFloat();
					ObjMovements[i].y() *= -1.0f * CoreUtility::randRange<float>(0.0f, 1.0f);
					//ObjMovements[i].z() = randFloat();

					if (ObjMovements[i].y() > -0.1f && ObjMovements[i].y() < 0.1f) ObjMovements[i].y() = -0.1f;
				}
			}

			//// shadow pass
			if (Sun1.castsShadows()) {
				glBeginQuery(GL_TIME_ELAPSED, TimingShadowPass);
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun1);
				SG.render(&RDev);

				Eigen::Quaternionf Rot;
				Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
				// render ground
				RDev.requestRendering(&Ground, Rot, GroundPos, GroundScale);
				RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(-50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				RDev.requestRendering(&Picture, PictureRot, PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

				// render objects
				for (uint32_t i = 0; i < ObjCount; ++i) {
					StaticActor* pActor = nullptr;
					switch (ObjType[i]) {
					case 0: pActor = &Cube; break;
					case 1: pActor = &Sphere; break;
					default: pActor = &Cube; break;

					}
					RDev.requestRendering(pActor, ObjRotations[i], ObjPositions[i], ObjScales[i]);
				}
				glCullFace(GL_BACK);
				glEndQuery(GL_TIME_ELAPSED);
			}

			if (Sun2.castsShadows()) {
				RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun2);
				SG.render(&RDev);

				Eigen::Quaternionf Rot;
				Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
				// render ground
				RDev.requestRendering(&Ground, Rot, GroundPos, GroundScale);
				RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
				RDev.requestRendering(&Picture, PictureRot, PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

				// render objects
				for (uint32_t i = 0; i < ObjCount; ++i) {
					StaticActor* pActor = nullptr;
					switch (ObjType[i]) {
					case 0: pActor = &Cube; break;
					case 1: pActor = &Sphere; break;
					default: pActor = &Cube; break;
					}
					RDev.requestRendering(pActor, ObjRotations[i], ObjPositions[i], ObjScales[i]);
				}
			}

			// geometry pass
			if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBeginQuery(GL_TIME_ELAPSED, TimingGeometryPass);
			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			SG.render(&RDev);
			// render ground
			Eigen::Quaternionf Rot;
			Rot = Eigen::AngleAxis(GraphicsUtility::degToRad(-90.0f), Eigen::Vector3f::UnitX());
			RDev.requestRendering(&Ground, Rot, GroundPos, GroundScale);

			RDev.requestRendering(&Sphere, Eigen::Quaternionf::Identity(), Sun1.position(), Eigen::Vector3f(50.0f, 50.0f, 50.0f));
			//RDev.requestRendering(&Cube, Eigen::Quaternionf::Identity(), Sun2.position(), Eigen::Vector3f(50.0f, 50.0f, 50.0f));
			RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos, Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
			RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
			RDev.requestRendering(&Armchair, ArmchairRot, ArmchairPos + Eigen::Vector3f(-50.0f, 0.0f, 0.0f), Eigen::Vector3f(ArmchairScale, ArmchairScale, ArmchairScale));
			RDev.requestRendering(&Picture, PictureRot, PicturePos, Eigen::Vector3f(PictureScale, PictureScale, PictureScale));

			// render objects
			for (uint32_t i = 0; i < ObjCount; ++i) {
				StaticActor* pActor = nullptr;
				switch (ObjType[i]) {
				case 0: pActor = &Cube; break;
				case 1: pActor = &Sphere; break;
				default: pActor = &Cube; break;
				}
				RDev.requestRendering(pActor, ObjRotations[i], ObjPositions[i], ObjScales[i]);
			}
			glEndQuery(GL_TIME_ELAPSED);

			glBeginQuery(GL_TIME_ELAPSED, TimingLightingPass);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			// lighting pass
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
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

			RenderWin.swapBuffers();

			if (pKeyboard->keyPressed(Keyboard::KEY_ESCAPE)) {
				pKeyboard->keyState(Keyboard::KEY_ESCAPE, Keyboard::KEY_RELEASED);
				RenderWin.closeWindow();
			}
		}


		pSMan->release();
		//pTexMan->release();

	}//shadowTest

}
#endif