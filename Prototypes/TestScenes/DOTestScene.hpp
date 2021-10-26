/*****************************************************************************\
*                                                                           *
* File(s): DOTestScene.hpp                                            *
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
#ifndef __CFORGE_DOTESTSCENE_HPP__
#define __CFORGE_DOTESTSCENE_HPP__

namespace CForge {

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
			SAssetIO::load("Assets/Digitale Objektrekonstruktion/LabRoom/LabRoom_Wall0" + std::to_string(i + 1) + ".obj", &M);
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
			if (M.normalCount() == 0) M.computePerVertexNormals();
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
				Camera.up(MouseSpeed * FPSScale);
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
				Camera.right(MouseSpeed * FPSScale);
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

			if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

}//name space

#endif 