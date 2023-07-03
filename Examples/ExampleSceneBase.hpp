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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_EXAMPLESCENEBASE_HPP__
#define __CFORGE_EXAMPLESCENEBASE_HPP__

#include <crossforge/Math/CForgeMath.h>
#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/AssetIO/SAssetIO.h>
#include <crossforge/Graphics/Shader/SShaderManager.h>
#include <crossforge/Graphics/STextureManager.h>

#include <crossforge/Graphics/GLWindow.h>
#include <crossforge/Graphics/RenderDevice.h>

#include <crossforge/Graphics/Lights/DirectionalLight.h>
#include <crossforge/Graphics/Lights/PointLight.h>

#include <crossforge/Graphics/SceneGraph/SceneGraph.h>
#include <crossforge/Graphics/SceneGraph/SGNGeometry.h>
#include <crossforge/Graphics/SceneGraph/SGNTransformation.h>

#include <crossforge/Graphics/Actors/StaticActor.h>
#include <crossforge/Graphics/Actors/SkyboxActor.h>

#include <crossforge/Graphics/Font/LineOfText.h>
#include <crossforge/MeshProcessing/PrimitiveShapeFactory.h>

#ifdef __EMSCRIPTEN__
#include <crossforge/Graphics/OpenGLHeader.h>
#else
#include <glad/glad.h>
#endif

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
			m_ScreenshotExtension = "webp";
			m_FPS = 60.0f;
			m_FPSCount = 0;
			m_LastFPSPrint = CForgeUtility::timestamp();
			m_CameraRotation = false;
			m_FPSLabelActive = false;
		}//Constructor

		~ExampleSceneBase(void) {
			clear();
		}//Destructor

		virtual void init() {
			initWindowAndRenderDevice();
			initCameraAndLights();

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		virtual void clear(void) {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		virtual void release(void) {
			delete this;
		}//release

		const GLWindow* renderWindow(void)const {
			return &m_RenderWin;
		}//renderWindow

		virtual void mainLoop(void){ 
			m_RenderWin.update();


			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}//mainLoop

	protected:

		// reacts to changing window size
		virtual void listen(GLWindowMsg Msg) override {
			// retrieve viewport
			RenderDevice::Viewport VP;

			VP.Size = Vector2i(Msg.iParam[0], Msg.iParam[1]);
			VP.Position = Vector2i(0, 0);

			VP.Size.x() = std::max(8, VP.Size.x());
			VP.Size.y() = std::max(8, VP.Size.y());

			// change all viewports to new resolution
			m_RenderDev.viewport(RenderDevice::RENDERPASS_COUNT, VP);

			// change GBuffer resolution
			m_RenderDev.gBuffer()->init(VP.Size.x()/m_RenderBufferScale, VP.Size.y()/m_RenderBufferScale);
			m_Cam.projectionMatrix(VP.Size.x()/m_RenderBufferScale, VP.Size.y()/m_RenderBufferScale, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// re-position label to bottom right of screen
			if (GLWindowMsg::MC_RESIZE == Msg.Code && m_FPSLabelActive) {
				float XPos = VP.Size.x() - m_FPSLabel.font()->computeStringWidth(m_FPSLabel.text()) - 10;
				float YPos = VP.Size.y() - 30;
				m_FPSLabel.position(XPos, YPos);
				m_FPSLabel.canvasSize(VP.Size.x(), VP.Size.y());
			}
			
			if (GLWindowMsg::MC_RESIZE == Msg.Code) {
				for (auto i : m_HelpTexts) if (nullptr != i) i->canvasSize(VP.Size.x(), VP.Size.y());
			}

		}//listen[GLWindow]

		virtual void initWindowAndRenderDevice(bool UseGBuffer = true, uint32_t DirLightCount = 1, uint32_t PointLightCount = 1, uint32_t SpotLightCount = 0) {
			m_RenderWin.init(Vector2i(100, 100), Vector2i(m_WinWidth, m_WinHeight), m_WindowTitle);
			m_RenderWin.startListening(this);

			auto Traits = CForgeUtility::retrieveGPUTraits();
			SLogger::log("Created context with GL version: " + Traits.GLVersion + "\n", "ProgramFlow");

			m_pShaderMan = SShaderManager::instance();

			// configure and initialize rendering pipeline
			RenderDevice::RenderDeviceConfig Config;
			Config.DirectionalLightsCount = DirLightCount;
			Config.PointLightsCount = PointLightCount;
			Config.SpotLightsCount = SpotLightCount;
			Config.ExecuteLightingPass = true;
			Config.GBufferWidth = m_WinWidth/m_RenderBufferScale;
			Config.GBufferHeight = m_WinHeight/m_RenderBufferScale;	
			Config.pAttachedWindow = &m_RenderWin;
			Config.PhysicallyBasedShading = true;
			Config.UseGBuffer = UseGBuffer;
			
			// configure and initialize shader configuration device
			ShaderCode::LightConfig LC;
			LC.DirLightCount = DirLightCount;
			LC.PointLightCount = PointLightCount;
			LC.SpotLightCount = SpotLightCount;
			LC.PCFSize = 1;
			LC.ShadowBias = 0.00001f;
			LC.ShadowMapCount = 1;
			m_pShaderMan->configShader(LC);

			m_RenderDev.init(&Config);

			ShaderCode::PostProcessingConfig PPC;
			PPC.Exposure = 1.0f;
			PPC.Gamma = 2.2f;
			PPC.Saturation = 1.1f;
			PPC.Brightness = 1.05f;
			PPC.Contrast = 1.05f;
			m_pShaderMan->configShader(PPC);
		}//initWindowAndRenderDevice

		virtual void initCameraAndLights(bool CastShadows = true) {
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows

			//m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
			if(CastShadows) m_Sun.initShadowCasting(1024, 1024, Vector2i(10, 10), 0.1f, 1000.0f);
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
			Rot = AngleAxisf(CForgeMath::degToRad(-0.25f / 60.0f), Vector3f::UnitY());
			m_SkyboxTransSGN.rotationDelta(Rot);
		}//initSkybox

		virtual void updateFPS(void) {	
			const uint32_t UpdateInterval = 250; // update of FPS 4 times per second
			m_FPSCount++;

			if (CForgeUtility::timestamp() - m_LastFPSPrint >= UpdateInterval) {
				char Buf[64];
				m_FPS = float(m_FPSCount * 1000.0f / (CForgeUtility::timestamp() - m_LastFPSPrint));
				m_FPS = std::max(m_FPS, 1.0f);
				sprintf(Buf, "FPS: %.1f", m_FPS);		
				
				m_RenderWin.title(m_WindowTitle + "[" + std::string(Buf) + "]");
				if (m_FPSLabelActive) m_FPSLabel.text(std::string(Buf));

				m_FPSCount = 0;
				m_LastFPSPrint = CForgeUtility::timestamp();

				std::string ErrorMsg;
				if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
					SLogger::log("OpenGL error occurred: " + ErrorMsg, "MainLoop", SLogger::LOGTYPE_ERROR);
				}
			}
			
		}//updateFPS


		virtual void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");

				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");

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

			if (pMouse->buttonState(Mouse::BTN_RIGHT) || pMouse->buttonState(Mouse::BTN_LEFT)) {
				if (m_CameraRotation) {
					const Eigen::Vector2f MouseDelta = pMouse->movement();
					pCamera->rotY(CForgeMath::degToRad(-0.1f * RotationSpeed * MouseDelta.x()));
					pCamera->pitch(CForgeMath::degToRad(-0.1f * RotationSpeed * MouseDelta.y()));
					
				}
				else {
					m_CameraRotation = true;
					
				}
				pMouse->movement(Eigen::Vector2f::Zero());
			}
			else {
				m_CameraRotation = false;
			}
		}//defaultCameraUpdate

		void defaultKeyboardUpdate(Keyboard* pKeyboard) {
			if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");

			if (pKeyboard->keyPressed(Keyboard::KEY_F1, true)) {
				m_DrawHelpTexts = !m_DrawHelpTexts;
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_F9, true)) {
				m_RenderWin.vsync(!m_RenderWin.vsync());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F10, true)) {
				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				std::string ScreenshotURI = "Screenshots/Screenshot_" + std::to_string(m_ScreenshotCount++) + "." + m_ScreenshotExtension;
				takeScreenshot(ScreenshotURI);
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_F11, true)) {
				m_RenderWin.toggleFullscreen();
				m_RenderWin.vsync(true);
			}



			if (pKeyboard->keyPressed(Keyboard::KEY_ESCAPE)) {
#ifdef __EMSCRIPTEN__
				emscripten_cancel_main_loop();
#else
				m_RenderWin.closeWindow();
#endif
			}
		}//defaultKeyboardUpdate

		void takeScreenshot(std::string Filepath) {
			T2DImage<uint8_t> ColorBuffer;
			CForgeUtility::retrieveFrameBuffer(&ColorBuffer);
			SAssetIO::store(Filepath, &ColorBuffer);
		}//takeScreen

		void initFPSLabel(void) {
			Font* pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 20);
			m_FPSLabel.init(pFont, "FPS: 60.0");
			m_FPSLabel.position(m_RenderWin.width() - pFont->computeStringWidth("FPS: 60.0") - 10, m_RenderWin.height() - 30);
			m_FPSLabel.color(0.0f, 0.0f, 0.0f, 1.0f);
			m_FPSLabelActive = true;
		}//initFPSLabel

		void initGroundPlane(SGNTransformation *pParent, float Scale, float UVTiling) {
			T3DMesh<float> M;
			PrimitiveShapeFactory::plane(&M, Vector2f(Scale, Scale), Vector2i(1, 1));
			//setMeshShader(&M, 0.4f, 0.04f);
			M.changeUVTiling(Vector3f(UVTiling, UVTiling, 1.0f));
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_WHITE);
			M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/Textures/Tiles107/Tiles107_1K_Color.webp";
			M.getMaterial(0)->TexNormal = "Assets/ExampleScenes/Textures/Tiles107/Tiles107_1K_NormalGL.webp";
			BoundingVolume BV;
			m_GroundPlane.init(&M);
			m_GroundPlane.boundingVolume(BV);
			m_GroundPlaneSGN.init(pParent, &m_GroundPlane);
			M.clear();
		}//initGroundPlane

		void drawHelpTexts(Eigen::Vector2f Offset = Vector2f::Zero()) {
			Vector2f Position = Vector2f(5.0f, 5.0f) + Offset;
			for (auto i : m_HelpTexts) {
				if (nullptr == i) continue;
				i->position(Position);
				i->render(&m_RenderDev);
				Position += Vector2f(0.0f, i->font()->style().PixelSize * 1.5f);
			}
		}//drawHelpTexts


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
		std::string m_ScreenshotExtension;

		// Skybox
		SkyboxActor m_Skybox;
		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;

		// ground plane
		StaticActor m_GroundPlane;
		SGNGeometry m_GroundPlaneSGN;

		// FPS Label
		LineOfText m_FPSLabel;
		bool m_FPSLabelActive;

		bool m_CameraRotation;

		std::vector<LineOfText*> m_HelpTexts;
		bool m_DrawHelpTexts;
	};//ExampleMinimumGraphicsSetup

}

#endif