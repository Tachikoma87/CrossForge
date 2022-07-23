/*****************************************************************************\
*                                                                           *
* File(s): exampleSceneBase.hpp                                             *
*                                                                           *
* Content: Base class for the example scenes. Takes care of common stuff    *
*          such as creating a window and setting up input.                  *
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
#ifndef __CFORGE_EXAMPLESCENEBASE_HPP__
#define __CFORGE_EXAMPLESCENEBASE_HPP__


#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/Graphics/Shader/SShaderManager.h"
#include "../CForge/Graphics/STextureManager.h"

#include "../CForge/Graphics/GLWindow.h"
#include "../CForge/Graphics/GraphicsUtility.h"
#include "../CForge/Graphics/RenderDevice.h"

#include "../CForge/Graphics/Lights/DirectionalLight.h"
#include "../CForge/Graphics/Lights/PointLight.h"

#include "../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../CForge/Graphics/Actors/StaticActor.h"
#include "../CForge/Graphics/Actors/SkyboxActor.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSceneBase : public ITListener<GLWindowMsg> {
	public:
		ExampleSceneBase(void) {
			m_WindowTitle = "CrossForge Example - Scene Base Class";
			m_WinWidth = 1280;
			m_WinHeight = 720;
			m_RenderBufferScale = 1;
			m_ScreenshotCount = 0;
			m_FPS = 60.0f;
			m_FPSCount = 0;
		}//Constructor

		~ExampleSceneBase(void) {
			clear();
		}//Destructor

		virtual void init() {
			initWindowAndRenderDevice();
			initCameraAndLights();

			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		virtual void clear(void) {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		virtual void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run

	protected:

		// reacts to changing window size
		virtual void listen(GLWindowMsg Msg) {
			// retrieve viewport
			RenderDevice::Viewport VP;

			VP.Size = Vector2i(Msg.iParam[0], Msg.iParam[1]);
			VP.Position = Vector2i(0, 0);

			// change all viewports to new resolution
			m_RenderDev.viewport(RenderDevice::RENDERPASS_COUNT, VP);

			// change GBuffer resolution
			m_RenderDev.gBuffer()->init(Msg.iParam[0]/m_RenderBufferScale, Msg.iParam[1]/m_RenderBufferScale);
			m_Cam.projectionMatrix(Msg.iParam[0]/m_RenderBufferScale, Msg.iParam[1]/m_RenderBufferScale, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		}//listen[GLWindow]

		virtual void initWindowAndRenderDevice(void) {
			m_RenderWin.init(Vector2i(100, 100), Vector2i(m_WinWidth, m_WinHeight), m_WindowTitle);
			m_RenderWin.startListening(this);

			m_pShaderMan = SShaderManager::instance();

			// configure and initialize rendering pipeline
			RenderDevice::RenderDeviceConfig Config;
			Config.DirectionalLightsCount = 1;
			Config.PointLightsCount = 1;
			Config.SpotLightsCount = 0;
			Config.ExecuteLightingPass = true;
			Config.GBufferWidth = m_WinWidth/m_RenderBufferScale;
			Config.GBufferHeight = m_WinHeight/m_RenderBufferScale;	
			Config.pAttachedWindow = &m_RenderWin;
			Config.PhysicallyBasedShading = true;
			Config.UseGBuffer = true;
			m_RenderDev.init(&Config);

			// configure and initialize shader configuration device
			ShaderCode::LightConfig LC;
			LC.DirLightCount = 1;
			LC.PointLightCount = 1;
			LC.SpotLightCount = 0;
			LC.PCFSize = 1;
			LC.ShadowBias = 0.00001f;
			LC.ShadowMapCount = 1;
			m_pShaderMan->configShader(LC);

			ShaderCode::PostProcessingConfig PPC;
			PPC.Exposure = 1.0f;
			PPC.Gamma = 2.2f;
			PPC.Saturation = 1.1f;
			PPC.Brightness = 1.05f;
			PPC.Contrast = 1.05f;
			m_pShaderMan->configShader(PPC);
		}//initWindowAndRenderDevice

		virtual void initCameraAndLights(void) {
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);
		}//initCameraAndLights

		void initSkybox(void) {

			vector<string> ClearSky;
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_rt.jpg");
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_lf.jpg");
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_up.jpg");
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_dn.jpg");
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_ft.jpg");
			ClearSky.push_back("Assets/ExampleScenes/skybox/bluecloud_bk.jpg");
			m_Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);

			// set initialize color adjustment values
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.0f);
			m_Skybox.saturation(1.2f);

			// create scene graph for the Skybox
			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxGeomSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);

			Quaternionf Rot;
			Rot = AngleAxisf(GraphicsUtility::degToRad(-0.25f / 60.0f), Vector3f::UnitY());
			m_SkyboxTransSGN.rotationDelta(Rot);
		}//initSkybox

		virtual void updateFPS(void) {
			m_FPSCount++;
			if (CoreUtility::timestamp() - m_LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", m_FPSCount);
				m_FPS = float(m_FPSCount);
				m_FPSCount = 0;
				m_LastFPSPrint = CoreUtility::timestamp();

				m_RenderWin.title(m_WindowTitle + "[" + std::string(Buf) + "]");
			}
		}//updateFPS


		virtual void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");

				pMat->Metallic = Metallic;
				pMat->Roughness = Roughness;
			}//for[materials]
		}//setMeshShader

		void defaultCameraUpdate(VirtualCamera* pCamera, Keyboard* pKeyboard, Mouse* pMouse, const float MovementSpeed = 0.4f, const float RotationSpeed = 1.0f, const float SpeedScale = 4.0f) {
			if (nullptr == pCamera) throw NullpointerExcept("pCamera");
			if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");
			if (nullptr == pMouse) throw NullpointerExcept("pMouse");

			float S = 1.0f;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) S = SpeedScale;

			if (pKeyboard->keyPressed(Keyboard::KEY_W)) pCamera->forward(S * MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_S)) pCamera->forward(S * -MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_A)) pCamera->right(-S * MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_D)) pCamera->right(S * MovementSpeed);

			if (pMouse->buttonState(Mouse::BTN_RIGHT)) {
				const Eigen::Vector2f MouseDelta = pMouse->movement();
				pCamera->rotY(GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.x()));
				pCamera->pitch(GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.y()));
				pMouse->movement(Eigen::Vector2f::Zero());
			}
		}//defaultCameraUpdate

		void defaultKeyboardUpdate(Keyboard* pKeyboard) {
			if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");
			if (pKeyboard->keyPressed(Keyboard::KEY_F9, true)) {
				m_RenderWin.vsync(!m_RenderWin.vsync());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F10, true)) {
				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				std::string ScreenshotURI = "Screenshot_" + std::to_string(m_ScreenshotCount++) + ".webp";
				takeScreenshot(ScreenshotURI);
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_ESCAPE)) {
				m_RenderWin.closeWindow();
			}
		}//defaultKeyboardUpdate

		void takeScreenshot(std::string Filepath) {
			T2DImage<uint8_t> ColorBuffer;
			GraphicsUtility::retrieveFrameBuffer(&ColorBuffer);
			SAssetIO::store(Filepath, &ColorBuffer);
		}//takeScreen

		std::string m_WindowTitle;
		int32_t m_WinWidth;
		int32_t m_WinHeight;
		int32_t m_RenderBufferScale;

		SShaderManager* m_pShaderMan;

		GLWindow m_RenderWin;
		RenderDevice m_RenderDev;

		SceneGraph m_SG;
		VirtualCamera m_Cam;

		// Performance monitoring
		float m_FPS;
		uint64_t m_LastFPSPrint;
		uint32_t m_FPSCount;

		DirectionalLight m_Sun;
		PointLight m_BGLight; ///< Background light

		uint32_t m_ScreenshotCount;

		// Skybox
		SkyboxActor m_Skybox;
		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;

	};//ExampleMinimumGraphicsSetup



	void exampleSceneBase(void) {
		ExampleSceneBase Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMinimumGraphicsSetup

}

#endif