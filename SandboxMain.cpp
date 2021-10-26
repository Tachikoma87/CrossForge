#ifdef __linux__
#include <unistd.h>
extern "C" {
#include <linux/gpio.h>
}
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

// basic os imports
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include <memory.h>
#include <thread>
#include <chrono>

// Glad and GLFW3
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "DevicesTestFunctions.hpp"

#include "CForge/Core/SLogger.h"
#include "CForge/Core/SCrossForgeDevice.h"

#include "CForge/Graphics/STextureManager.h"
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "CForge/Graphics/SceneGraph/SceneGraph.h"
#include "CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "CForge/Graphics/SceneGraph/SGNTransformation.h"
#include "CForge/Graphics/Actors/StaticActor.h"
#include "CForge/Graphics/Lights/DirectionalLight.h"
#include "CForge/Graphics/Lights/PointLight.h"
#include "CForge/Graphics/Lights/SpotLight.h"
#include "CForge/AssetIO/SAssetIO.h"
#include "CForge/Graphics/GraphicsUtility.h"

//#include "Prototypes/Shader/ShaderCode.h"
#include "Prototypes/Actor/SkeletalActor.h"
#include "Prototypes/Actor/SkeletalAnimationController.h"

using namespace CForge;
using namespace Eigen;

// quick and ditry hack for unix
#ifdef __unix__
uint32_t GetTickCount() {
	auto Millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return Millis.count();
}
#endif


void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
	for (uint32_t i = 0; i < pM->materialCount(); ++i) {
		T3DMesh<float>::Material* pMat = pM->getMaterial(i);
		pMat->VertexShaderSources.push_back("Shader/BasicGeometryPass.vert");
		pMat->FragmentShaderSources.push_back("Shader/BasicGeometryPass.frag");
		pMat->Metallic = Metallic;
		pMat->Roughness = Roughness;
	}//for[materials]
}//setMeshShader


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

	Keyboard *pKeyboard = RenderWin.keyboard();
	Mouse *pMouse = RenderWin.mouse();

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
	setMeshShader(&M, 0.8f, 0.04f);
	Ground.init(&M);
	M.clear();

	SAssetIO::load("Assets/TexturedCube.fbx", &M);
	setMeshShader(&M, 0.6f, 0.04f);
	Cube.init(&M);
	M.clear();

	SAssetIO::load("Assets/TexturedSphere.fbx", &M);
	setMeshShader(&M, 0.6f, 0.04f);
	Sphere.init(&M);
	M.clear();

	SAssetIO::load("Assets/Meeting Room/Armchair/Armchair.obj", &M);
	setMeshShader(&M, 0.1f, 0.04f);
	M.computePerVertexNormals();
	Armchair.init(&M);
	M.clear();

	/*SAssetIO::load("Assets/Head/Pasha_guard_head.obj", &M);
	setMeshShader(&M, 0.6f, 0.04f);
	M.computePerVertexNormals();
	Head.init(&M);
	M.clear();*/

	SAssetIO::load("Assets/Meeting Room/Picture/Picture.obj", &M);
	setMeshShader(&M, 0.2f, 0.1f);
	M.computePerVertexNormals();
	Picture.init(&M);
	M.clear();

	SkeletalAnimationController EricAnimController;
	//SAssetIO::load("Assets/Skel/Eric_Anim.fbx", &M);
	//SAssetIO::load("Assets/Skel/SmplMale_test.fbx", &M);
	SAssetIO::load("Assets/Skel/ExportTest2.fbx", &M);
	setMeshShader(&M, 0.6f, 0.04f);

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
	Sun1.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(125,125), 0.5f, 1000.0f);

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

		R = Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) /30.0f, Eigen::Vector3f::UnitY()) * Eigen::AngleAxisf(CoreUtility::randRange<float>(-1.0f, 1.0f) /60.0f, Eigen::Vector3f::UnitX());

		ObjRotationsDelta.push_back(R);

		float Scale = 0.25f + CoreUtility::randRange<float>(0.0f, 1.0f) * 1.0f;
		ObjScales.push_back(Eigen::Vector3f(Scale, Scale, Scale));
	}//for[generate objects]

	Eigen::Vector3f ArmchairPos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f PicturePos = Eigen::Vector3f(0.0f, 50.0f, -35.0f);
	float ArmchairScale = 1.0f/4.0f;
	float PictureScale = 1.0f/2.0f;
	Eigen::Quaternionf ArmchairRot = Eigen::Quaternionf::Identity();
	Eigen::Quaternionf PictureRot = Eigen::Quaternionf::Identity();

	PictureRot = Eigen::AngleAxis(GraphicsUtility::degToRad(-45.0f), Eigen::Vector3f::UnitY());

	uint32_t FrameCount = 0;
	uint32_t LastFPSPrint = GetTickCount();

	uint32_t TimingShadowPass = 0;
	uint32_t TimingGeometryPass = 0;
	uint32_t TimingLightingPass = 0;
	glGenQueries(1, &TimingShadowPass);
	glGenQueries(1, &TimingGeometryPass);
	glGenQueries(1, &TimingLightingPass);

	bool Wireframe = false;

	while (!RenderWin.shutdown()) {
		FrameCount++;
		if (GetTickCount() - LastFPSPrint > 5000) {

			LastFPSPrint = GetTickCount();
			float AvailableMemory = GraphicsUtility::gpuMemoryAvailable()/1000.0f;
			float MemoryInUse = AvailableMemory - GraphicsUtility::gpuFreeMemory()/1000.0f;

			uint32_t ShadowPassTime;
			uint32_t GeometryPassTime;
			uint32_t LightingPassTime;
			printf("FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB\n", FrameCount/5, MemoryInUse, AvailableMemory);

			if (nullptr != glGetQueryObjectuiv) {
				glGetQueryObjectuiv(TimingShadowPass, GL_QUERY_RESULT, &ShadowPassTime);
				glGetQueryObjectuiv(TimingGeometryPass, GL_QUERY_RESULT, &GeometryPassTime);
				glGetQueryObjectuiv(TimingLightingPass, GL_QUERY_RESULT, &LightingPassTime);
				printf("\tTimings: Shadow Pass: %.2fms\n", ShadowPassTime / (1000000.0f));
				printf("\tTiming Geometry Pass: %.2fms\n", GeometryPassTime / (1000000.0f));
				printf("\tTiming Lighting Pass: %.2fms\n", LightingPassTime / (1000000.0f));
			}
				
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

		
		if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT, Keyboard::KEY_9)) {
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

		if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT, Keyboard::KEY_8)) {
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
		GLFWwindow* pWin = (GLFWwindow*)RenderWin.handle();

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_1)) {
			glfwSwapInterval(0);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_2)) {
			glfwSwapInterval(1);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_3)) {
			glfwSwapInterval(2);
		}

		if (Keyboard::KEY_PRESSED == pKeyboard->keyPressed(Keyboard::KEY_4)) {
			Wireframe = !Wireframe;
			pKeyboard->keyState(Keyboard::KEY_4, Keyboard::KEY_RELEASED);
		}
		if (Keyboard::KEY_PRESSED == pKeyboard->keyPressed(Keyboard::KEY_5)) {
			pEricAnim->Speed = (pEricAnim->Speed < 0.5f) ? 1.0f : 0.0f;
			pKeyboard->keyState(Keyboard::KEY_5, Keyboard::KEY_RELEASED);
		}

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_ESCAPE)) {
			RenderWin.closeWindow();
			break;
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_W)) {
			Cam.forward(0.4f);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_S)) {
			Cam.forward(-0.4f);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_D)) {
			Cam.right(0.4f);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_A)) {
			Cam.right(-0.4f);
		}
		// rotation mode?
		if (glfwGetMouseButton(pWin, GLFW_MOUSE_BUTTON_RIGHT)) {
			if (!RotationMode) {
				MouseDelta = Eigen::Vector2f(0.0f, 0.0f);
				double x, y;
				glfwGetCursorPos(pWin, &x, &y);
				MousePos = Eigen::Vector2f(x, y);
				RotationMode = true;
			}
			else {
				double x, y;
				glfwGetCursorPos(pWin, &x, &y);
				MouseDelta = MousePos - Eigen::Vector2f(x, y);

				Cam.rotY(GraphicsUtility::degToRad(MouseDelta.x()) * 0.1f);
				Cam.pitch(GraphicsUtility::degToRad(MouseDelta.y()) * 0.1f);

				MousePos = Eigen::Vector2f(x, y);
			}
		}
		else {
			RotationMode = false;
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
		RDev.requestRendering(&Ground, Rot , GroundPos, GroundScale);

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
		

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_0)) {
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
		}
		
		RenderWin.swapBuffers();
	}


	pSMan->release();
	//pTexMan->release();

}//shadowTest

void DOTestScene(void) {
	SAssetIO* pAssIO = SAssetIO::instance();
	STextureManager* pTexMan = STextureManager::instance();
	SShaderManager* pSMan = SShaderManager::instance();

	const bool FullHD = false;

	// initialize a window to render
	uint32_t WinWidth = 1280;
	uint32_t WinHeight = 720;
	if (FullHD) {
		WinWidth = 1920;
		WinHeight = 1080;
	}
	GLWindow RenderWin;
	RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), "Digitale Objektrekonstruktion - Practical Course");

	// initialize render device
	RenderDevice::RenderDeviceConfig RDConfig;
	RDConfig.init();
	RDConfig.pAttachedWindow = &RenderWin;
	RDConfig.UseGBuffer = true;
	RDConfig.GBufferWidth = WinWidth;
	RDConfig.GBufferHeight = WinHeight;
	RDConfig.ExecuteLightingPass = true;
	RDConfig.PhysicallyBasedShading = true;
	RDConfig.DirectionalLightsCount = 2;
	RDConfig.PointLightsCount = 2;
	RDConfig.SpotLightsCount = 1;
	RenderDevice RDev;
	RDev.init(&RDConfig);
	
	// we need a camera
	VirtualCamera Camera;
	Camera.init(Vector3f(0.0f, 1.5f, 2.0f), Vector3f::UnitY());
	Camera.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.05f, 50.0f);
	bool RotationMode = false;
	Vector2f MousePos = Vector2f(0.0f, 0.0f);
	Vector2f MouseDelta = Vector2f(0.0f, 0.0f);
	//Camera.position(Vector3f(0.0f, 10.0f, 50.0f));

	// an a light
	DirectionalLight Sun;
	Vector3f SunPos = Vector3f(-4.0f, 2.5f, 0.5f);
	Vector3f SunTarget = Vector3f(0.0f, 1.0f, 0.0f);
	Sun.init(SunPos, (SunTarget - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 6.0f);
	const uint32_t ShadowMapSize = 4 * 1024;;
	Sun.initShadowCasting(ShadowMapSize, ShadowMapSize, Vector2i(4, 4), 0.05f, 50.0f);

	PointLight RoomLamp;
	Vector3f RoomLampPos = Vector3f(0.0f, 4.0f, 2.5f);
	RoomLamp.init(RoomLampPos, -RoomLampPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 4.5f, Vector3f(0.0f, 0.1f, 0.01f));

	RDev.activeCamera(&Camera);
	RDev.addLight(&Sun);
	RDev.addLight(&RoomLamp);

#pragma region SceneGraphBuilding
	// and we need a scene graph so we actually have something to render
	SceneGraph SGRoom;
	SGNTransformation SGRoomRoot;
	SGRoomRoot.init(nullptr);
	SGRoom.rootNode(&SGRoomRoot);

	// floor actor
	T3DMesh<float> M;

	StaticActor RoomFloor;
	SGNGeometry SGRoomFloor;

	SAssetIO::load("Assets/Digitale Objektrekonstruktion/LabRoom/LabRoom_Floor.obj", &M);
	if (M.normalCount() == 0) M.computePerVertexNormals();
	setMeshShader(&M, 0.4, 0.04);
	RoomFloor.init(&M);
	SGRoomFloor.init(&SGRoomRoot, &RoomFloor);
	M.clear();

	StaticActor RoomWalls[8];
	SGNGeometry SGRoomWalls[8];

	for (uint8_t i = 0; i < 8; ++i) {
		SAssetIO::load("Assets/Digitale Objektrekonstruktion/LabRoom/LabRoom_Wall0" + std::to_string(i+1) + ".obj", &M);
		if (M.normalCount() == 0) M.computePerVertexNormals();
		setMeshShader(&M, 0.6, 0.04);
		RoomWalls[i].init(&M);
		SGRoomWalls[i].init(&SGRoomRoot, &RoomWalls[i]);
		M.clear();
	}

	StaticActor RoomEnclosure;
	SGNGeometry SGRoomEnclosure;
	SAssetIO::load("Assets/Digitale Objektrekonstruktion/LabRoom/LabRoom_Enclosure.obj", &M);
	if (M.normalCount() == 0) M.computePerVertexNormals();
	setMeshShader(&M, 0.4, 0.04);
	RoomEnclosure.init(&M);
	SGRoomEnclosure.init(&SGRoomRoot, &RoomEnclosure);
	M.clear();

	StaticActor Cabinet;
	SGNGeometry SGCabinets[4];
	SAssetIO::load("Assets/Digitale Objektrekonstruktion/LabRoom/LabRoom_Cabinet.obj", &M);
	if (M.normalCount() == 0) M.computePerVertexNormals();
	setMeshShader(&M, 0.4, 0.04);
	Cabinet.init(&M);
	for (uint8_t i = 0; i < 4; ++i) {
		SGCabinets[i].init(&SGRoomRoot, &Cabinet);
		Vector3f Pos(i * 2.0f, 0.0f, 0.0f);
		SGCabinets[i].position(Pos);
	}
	M.clear();


	Vector3f CabinetPos(-2.0f, 2.0f, -3.0f);
	
	SGNTransformation SGNDisplayItemTransforms[4];
	SGNGeometry SGNDisplayItems[4];

	SGNDisplayItemTransforms[0].init(&SGRoomRoot, CabinetPos);
	SGNDisplayItemTransforms[1].init(&SGRoomRoot, CabinetPos + Vector3f(2.0f, 0.0f, 0.0f));
	SGNDisplayItemTransforms[2].init(&SGRoomRoot, CabinetPos + Vector3f(4.0f, 0.0f, 0.0f));
	SGNDisplayItemTransforms[3].init(&SGRoomRoot, CabinetPos + Vector3f(6.0f, 0.0f, 0.0f));
	SGNDisplayItems[0].init(&SGNDisplayItemTransforms[0], nullptr);
	SGNDisplayItems[1].init(&SGNDisplayItemTransforms[1], nullptr);
	SGNDisplayItems[2].init(&SGNDisplayItemTransforms[2], nullptr);
	SGNDisplayItems[3].init(&SGNDisplayItemTransforms[3], nullptr);

	Quaternionf DisplayRotation;
	DisplayRotation = AngleAxisf(GraphicsUtility::degToRad(22.5f / 60.0f), Vector3f::UnitY());

	/*StaticActor SunActor;
	SGNGeometry SGSun;
	SAssetIO::load("Assets/TexturedSphere.fbx", &M);
	setMeshShader(&M, 0.1, 0.04);
	M.computeAxisAlignedBoundingBox();
	float S = 0.5f / M.aabb().diagonal().norm();
	SunActor.init(&M);
	SGSun.init(&SGRoomRoot, &SunActor);
	SGSun.position(CabinetPos);
	SGSun.scale(Vector3f(S, S, S));
	M.clear();*/

	float S = 1.0f;

	const bool LoadHead = true;
	const bool LoadDragon = true;
	const bool LoadSeaShell = true;
	const bool LoadStatue = true;

	StaticActor Dragon;
	StaticActor Head;
	StaticActor SeaShell;
	StaticActor Statue;

	if (LoadHead) {
		SAssetIO::load("Assets/Head/Pasha_guard_head.obj", &M);
		
		if (M.normalCount() == 0) M.computePerVertexNormals();
		setMeshShader(&M, 0.7, 0.04);
		Head.init(&M);
		M.computeAxisAlignedBoundingBox();
		S = 1.5f / M.aabb().diagonal().norm();
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		SGNDisplayItems[0].actor(&Head);
		SGNDisplayItems[0].rotation(R);
		SGNDisplayItems[0].scale(Vector3f(S, S, S));
		M.clear();
	}
	
	if (LoadDragon) {
		SAssetIO::load("Assets/Digitale Objektrekonstruktion/Artec Dragon/DragonReduced.obj", &M);
		if (M.normalCount() == 0) M.computePerVertexNormals();
		setMeshShader(&M, 0.15f, 0.6f);
		for (uint32_t i = 0; i < M.materialCount(); ++i) M.getMaterial(i)->Color = Vector4f(218.0f, 165.0f, 32.0f, 255.0f) / 255.0f;
		Dragon.init(&M);
		M.computeAxisAlignedBoundingBox();
		S = 2.0f / M.aabb().diagonal().norm();
		SGNDisplayItems[1].actor(&Dragon);
		SGNDisplayItems[1].scale(Vector3f(S, S, S));
		SGNDisplayItems[1].position(Vector3f(0.0f, -0.5f, 0.0f));
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		SGNDisplayItems[1].rotation(R);
		M.clear();
	}

	if (LoadSeaShell) {
		SAssetIO::load("Assets/Digitale Objektrekonstruktion/Sea Shell OBJ/sea_shell.obj", &M);
		if (M.normalCount() == 0) M.computePerVertexNormals();
		setMeshShader(&M, 0.2f, 0.04f);
		SeaShell.init(&M);
		M.computeAxisAlignedBoundingBox();
		S = 2.0f / M.aabb().diagonal().norm();
		SGNDisplayItems[2].actor(&SeaShell);
		SGNDisplayItems[2].scale(Vector3f(S, S, S));
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		SGNDisplayItems[2].rotation(R);
		M.clear();	
	}

	if (LoadStatue) {
		SAssetIO::load("Assets/Digitale Objektrekonstruktion/Artec Bronze Statue/Bronze_Statue.obj", &M);
		if(M.normalCount() == 0) M.computePerVertexNormals();
		setMeshShader(&M, 0.3f, 0.8f);

		for (uint32_t i = 0; i < M.materialCount(); ++i) {
			M.getMaterial(i)->Color = Vector4f(116.0f, 55.0f, 45.0f, 255.0f) / 255.0f;
		}


		Statue.init(&M);
		M.computeAxisAlignedBoundingBox();
		S = 2.0f / M.aabb().diagonal().norm();
		SGNDisplayItems[3].actor(&Statue);
		SGNDisplayItems[3].scale(Vector3f(S, S, S));
		SGNDisplayItems[3].position(Vector3f(0.0f, -0.5f, 0.0f));
		Quaternionf R;
		//R = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
		//SGNDisplayItems[3].rotation(R);
		M.clear();
	}

	float FPSScale = 1.0f;
	uint32_t Timeout = GetTickCount();
	bool Wireframe = false;

#pragma endregion

	uint32_t FrameCount = 0;
	uint32_t LastFPSPrint = GetTickCount();

	while (!RenderWin.shutdown()) {

		FrameCount++;
		if (GetTickCount() - LastFPSPrint > 1000) {

			LastFPSPrint = GetTickCount();
			float AvailableMemory = GraphicsUtility::gpuMemoryAvailable() / 1000.0f;
			float MemoryInUse = AvailableMemory - GraphicsUtility::gpuFreeMemory() / 1000.0f;

			printf("FPS: %d | GPU Memory Usage: %.2f MB/%.2f MB\n", FrameCount, MemoryInUse, AvailableMemory);
			FPSScale = 60.0f / FrameCount;	
			FrameCount = 0;
		}

		RenderWin.update();
		SGRoom.update(FPSScale);

#pragma region Input
		GLFWwindow* pWin = (GLFWwindow*)RenderWin.handle();
		if (glfwGetKey(pWin, GLFW_KEY_ESCAPE)) {
			RenderWin.closeWindow();
			break;
		}
		float MouseSpeed = 0.025f;
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_ESCAPE)) {
			RenderWin.closeWindow();
			break;
		}

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_W)) {
			Camera.up(MouseSpeed* FPSScale);
		}
		else if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_W)) {
			Camera.forward(MouseSpeed * FPSScale);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_S)) {
			Camera.up(-MouseSpeed * FPSScale);
		}
		else if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_S)) {
			Camera.forward(-MouseSpeed * FPSScale);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_D)) {
			Camera.right(MouseSpeed* FPSScale);
		}
		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_A)) {
			Camera.right(-MouseSpeed * FPSScale);
		}
		// rotation mode?
		if (glfwGetMouseButton(pWin, GLFW_MOUSE_BUTTON_RIGHT)) {
			if (!RotationMode) {
				MouseDelta = Eigen::Vector2f(0.0f, 0.0f);
				double x, y;
				glfwGetCursorPos(pWin, &x, &y);
				MousePos = Eigen::Vector2f(x, y);
				RotationMode = true;
			}
			else {
				double x, y;
				glfwGetCursorPos(pWin, &x, &y);
				MouseDelta = MousePos - Eigen::Vector2f(x, y);

				Camera.rotY(GraphicsUtility::degToRad(MouseDelta.x()) * 0.1f);
				Camera.pitch(GraphicsUtility::degToRad(MouseDelta.y()) * 0.1f);

				MousePos = Eigen::Vector2f(x, y);
			}
		}
		else {
			RotationMode = false;
		}


		if (glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(pWin, GLFW_KEY_1)) {
			SGNDisplayItemTransforms[0].rotationDelta(Quaternionf::Identity());
		}
		else if (glfwGetKey(pWin, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(pWin, GLFW_KEY_1)) {
			if (GetTickCount() - Timeout > 250) {
				bool Enabled;
				SGNDisplayItems[0].enabled(nullptr, &Enabled);
				SGNDisplayItems[0].enable(true, !Enabled);
				Timeout = GetTickCount();
			}
			
		}
		else if (glfwGetKey(pWin, GLFW_KEY_1)) {
			SGNDisplayItemTransforms[0].rotationDelta(DisplayRotation);
		}

		if (glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(pWin, GLFW_KEY_2)) {
			SGNDisplayItemTransforms[1].rotationDelta(Quaternionf::Identity());
		}
		else if (glfwGetKey(pWin, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(pWin, GLFW_KEY_2)) {
			if (GetTickCount() - Timeout > 250) {
				bool Enabled;
				SGNDisplayItems[1].enabled(nullptr, &Enabled);
				SGNDisplayItems[1].enable(true, !Enabled);
				Timeout = GetTickCount();
			}
			
		}
		else if (glfwGetKey(pWin, GLFW_KEY_2)) {
			SGNDisplayItemTransforms[1].rotationDelta(DisplayRotation);
		}

		if (glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(pWin, GLFW_KEY_3)) {
			SGNDisplayItemTransforms[2].rotationDelta(Quaternionf::Identity());
		}
		else if (glfwGetKey(pWin, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(pWin, GLFW_KEY_3)) {
			if (GetTickCount() - Timeout > 250) {
				bool Enabled;
				SGNDisplayItems[2].enabled(nullptr, &Enabled);
				SGNDisplayItems[2].enable(true, !Enabled);
				Timeout = GetTickCount();
			}
			
		}
		else if (glfwGetKey(pWin, GLFW_KEY_3)) {
			SGNDisplayItemTransforms[2].rotationDelta(DisplayRotation);
		}

		if (glfwGetKey(pWin, GLFW_KEY_LEFT_SHIFT) && glfwGetKey(pWin, GLFW_KEY_4)) {
			SGNDisplayItemTransforms[3].rotationDelta(Quaternionf::Identity());
		}
		else if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(pWin, GLFW_KEY_4)) {
			if (GetTickCount() - Timeout > 250) {
				bool Enabled;
				SGNDisplayItems[3].enabled(nullptr, &Enabled);
				SGNDisplayItems[3].enable(true, !Enabled);
				Timeout = GetTickCount();
			}
			
		}
		else if (glfwGetKey(pWin, GLFW_KEY_4)) {
			SGNDisplayItemTransforms[3].rotationDelta(DisplayRotation);
		}

		if (glfwGetKey(pWin, GLFW_KEY_F1)) {
			
			Wireframe = true;
		}
		if (glfwGetKey(pWin, GLFW_KEY_F2)) {
			Wireframe = false;
			
		}

#pragma endregion

		if (Sun.castsShadows()) {
			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			SGRoom.render(&RDev);
		}

		if(Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
		SGRoom.render(&RDev);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_0)) {
			T2DImage<uint8_t> Img;
			RDev.gBuffer()->retrievePositionBuffer(&Img);
			SAssetIO::store("Assets/Temp/PosBuffer.jpg", &Img);
			RDev.gBuffer()->retrieveNormalBuffer(&Img);
			SAssetIO::store("Assets/Temp/NormalBuffer.jpg", &Img);
			RDev.gBuffer()->retrieveAlbedoBuffer(&Img);
			SAssetIO::store("Assets/Temp/AlbedoBuffer.jpg", &Img);
			if (Sun.castsShadows()) {
				Sun.retrieveDepthBuffer(&Img);
				SAssetIO::store("Assets/Temp/Sun1ShadowMap.jpg", &Img);
			}
		}

		if (GLFW_PRESS == glfwGetKey(pWin, GLFW_KEY_9)) {
			T2DImage<uint8_t> ColorBuffer;
			T2DImage<uint8_t> DepthBuffer;
			GraphicsUtility::retrieveFrameBuffer(&ColorBuffer, &DepthBuffer, 0.1f, 100.0f);
			SAssetIO::store("Assets/Temp/ScreenshotColor.jpg", &ColorBuffer);
			SAssetIO::store("Assets/Temp/ScreenshotDepth.jpg", &DepthBuffer);
		}

		RenderWin.swapBuffers();
	}//while[Main loop]


	pAssIO->release();
	pTexMan->release();
	pSMan->release();

}

void PITestScene(void) {
	SAssetIO* pAssIO = SAssetIO::instance();
	STextureManager* pTexMan = STextureManager::instance();
	SShaderManager* pSMan = SShaderManager::instance();

	bool const LowRes = true;

	uint32_t WinWidth = 1280;
	uint32_t WinHeight = 720;

	if (LowRes) {
		WinWidth = 720;
		WinHeight = 576;
	}

	GLWindow RenderWin;
	RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), "Raspi OpenGL");

	GraphicsUtility::GPUTraits Traits = GraphicsUtility::retrieveGPUTraits();
	printf("Created OpenGL Window with version %d.%d (%s)\n", Traits.GLMajorVersion, Traits.GLMinorVersion, Traits.GLVersion.c_str());

	printf("Max Vertex Attribs: %d\n", Traits.MaxVertexAttribs);
	printf("Max framebuffer: %dx%d\n\n", Traits.MaxFramebufferWidth, Traits.MaxFramebufferHeight);
	printf("Max texture image units: %d\n", Traits.MaxTextureImageUnits);

	//glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

	std::string GLError;
	GraphicsUtility::checkGLError(&GLError);
	if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

	RenderDevice RDev;
	RenderDevice::RenderDeviceConfig Config;
	Config.DirectionalLightsCount = 2;
	Config.PointLightsCount = 2;
	Config.SpotLightsCount = 1;
	Config.ExecuteLightingPass = true;
	Config.GBufferHeight = WinHeight;
	Config.GBufferWidth = WinWidth;
	Config.pAttachedWindow = &RenderWin;
	Config.PhysicallyBasedShading = true;
	Config.UseGBuffer = true;
	RDev.init(&Config);


	VirtualCamera Cam;
	Cam.init(Vector3f(0.0f, 0.0f, 5.0f), Vector3f::UnitY());
	Cam.projectionMatrix(WinWidth, WinHeight, 45.0f, 0.1f, 1000.0f);

	Vector3f SunPos = Vector3f(5.0f, 25.0f, 25.0f);
	DirectionalLight Sun;
	Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);

	RDev.activeCamera(&Cam);
	RDev.addLight(&Sun);

	SceneGraph SGTest;
	SGNGeometry CubeSGN;
	StaticActor Cube;

	/*T3DMesh<float> M;
	SAssetIO::load("Assets/TexturedCube.fbx", &M);
	setMeshShader(&M, 0.1f, 0.04f);
	Cube.init(&M);
	CubeSGN.init(nullptr, &Cube);
	SGTest.init(&CubeSGN);*/

	int64_t LastFPSPrint = GetTickCount();
	int32_t FPSCount = 0;

	/*M.clear();
	AssetIO::load("Assets/Skel/Eric_anim.fbx", &M);
	SkeletalActor SkelActor;
	SkelActor.init(&M, nullptr);*/



	while (!RenderWin.shutdown()) {
		RenderWin.update();
		SGTest.update(1.0f);

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

		RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
		SGTest.render(&RDev);

		RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

		RenderWin.swapBuffers();

		GLFWwindow* pWin = (GLFWwindow*)RenderWin.handle();
		if (glfwGetKey(pWin, GLFW_KEY_ESCAPE)) {
			RenderWin.closeWindow();
		}

		FPSCount++;
		if (GetTickCount() - LastFPSPrint > 1000) {
			printf("FPS: %d\n", FPSCount);
			FPSCount = 0;
			LastFPSPrint = GetTickCount();
		}

	}//while[main loop]


	pAssIO->release();
	pTexMan->release();
	pSMan->release();

}//PITestScene

void rndTest(void) {
	const uint32_t Count = 1000;
	uint32_t Negs = 0;
	uint32_t Pos = 0;

	for (uint32_t i = 0; i < Count; ++i) {
		float RndNbr = CoreUtility::randRange<float>(-1.0f, 1.0f);
		std::string Msg = std::to_string(RndNbr);
		SLogger::log(Msg, "Rnd Number Test", SLogger::LOGTYPE_DEBUG);

		if (RndNbr < 0) Negs++;
		else Pos++;
	}

	std::string Msg = "Negs: " + std::to_string(Negs) + " and positive: " + std::to_string(Pos);
	SLogger::log(Msg, "Rnd Number Test", SLogger::LOGTYPE_DEBUG);

}

void skelTest(void) {
	T3DMesh<float> M;
	AssetIO::load("Assets/Skel/Eric_anim.fbx", &M);

	printf("Eric has %d bones and %d animations!\n", M.boneCount(), M.skeletalAnimationCount());
}

int main(int argc, char* argv[]) {
#ifdef WIN32
	_CrtMemState S1, S2, S3;
	_CrtMemCheckpoint(&S1);
#endif

	SCrossForgeDevice* pDev = nullptr;

	try {
		 pDev = SCrossForgeDevice::instance();

		 SLogger::logFile("Logs/ErrorLog.txt", SLogger::LOGTYPE_ERROR, true, true);
		 SLogger::logFile("Logs/DebugLog.txt", SLogger::LOGTYPE_DEBUG, true, true);
		 SLogger::logFile("Logs/InfoLog.txt", SLogger::LOGTYPE_INFO, true, true);
		 SLogger::logFile("Logs/WarningLog.txt", SLogger::LOGTYPE_WARNING, true, true);
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred during init. See Log.");
		if (nullptr != pDev) pDev->release();
		pDev = nullptr;
		char c;
		scanf("%c", &c);
		return -1;
	}

	try {
		//DOTestScene();
		//shadowTest();
		PITestScene();
		//rndTest();
		//skelTest();
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred. See Log.");
	}
	catch (...) {
		printf("A not handled exception occurred!\n");
	}
	

	if(nullptr != pDev) pDev->release();
	
	char c;
	//scanf("%c", &c);

	
#ifdef WIN32
	// dump memory statics
	_CrtMemCheckpoint(&S2);
	_CrtMemDifference(&S3, &S1, &S2);
	_CrtMemDumpStatistics(&S3);

	// disable memleak dump
	int Tmp = _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
	Tmp &= ~_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(Tmp);
#endif
}//main