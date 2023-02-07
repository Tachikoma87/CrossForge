/*****************************************************************************\
*                                                                           *
* File(s): VertexColorTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_IMAGETESTSCENE_HPP__
#define __CFORGE_IMAGETESTSCENE_HPP__

#include <CForge/Graphics/Actors/SkyboxActor.h>
#include <CForge/GUI/Widgets/Form.h>
#include "../../Examples/ExampleSceneBase.hpp"


using namespace Eigen;
using namespace std;

namespace CForge {

	class ImageTestScene : public ExampleSceneBase, public ITListener<GUICallbackObject>{
	public:
		void initPPQuad(ScreenQuad* quad) {
			vector<ShaderCode*> vsSources;
			vector<ShaderCode*> fsSources;
			string errorLog;

			SShaderManager* shaderManager = SShaderManager::instance();
			vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "420 core",
				0, ""));

			const char* fragmentShaderSource = "#version 420 core\n"
				"layout (std140) uniform CameraData{			 \n"
				"mat4 ViewMatrix;								 \n"
				"mat4 ProjectionMatrix;							 \n"
				"vec4 Position;									 \n"
				"}Camera;										 \n"
				"layout(binding=0) uniform sampler2D texColor;	 \n"
				"layout(binding=1) uniform sampler2D texPosition;\n"
				"layout(binding=2) uniform sampler2D texNormal;	 \n"
				"in vec2 TexCoords;\n"
				"out vec4 FragColor;\n"
				"void main()\n"
				"{\n"
				"	FragColor = vec4(vec3(texture(texColor, TexCoords)), 1.0);\n"
				"}\n\0";
			fsSources.push_back(shaderManager->createShaderCode(fragmentShaderSource, "420 core",
				0, ""));
			GLShader* quadShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
			shaderManager->release();

			quad->init(0, 0, 1, 1, quadShader);
		}

		ImageTestScene(void) {
			m_WindowTitle = "CrossForge Prototype - Image import/export Test scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ImageTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();

			#ifndef __EMSCRIPTEN__
			gladLoadGL();
			#endif

			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-55.0f, 50.0f, 50.0f);
			Vector3f SunLookAt = Vector3f(0.0f, 0.0f, -30.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
			m_Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 14.0f);
			// sun will cast shadows
			//m_Sun.initShadowCasting(2048, 2048, GraphicsUtility::orthographicProjection(70.0f, 70.0f, 0.1f, 1000.0f));
			m_Sun.initShadowCasting(2048, 2048, Vector2i(70, 70), 0.1f, 1000.0f);

			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 13.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			T3DMesh<float> M;

			SAssetIO::load("MyAssets/TexturedGround.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 15.0f;
			M.getMaterial(0)->TexAlbedo = "MyAssets/ground14.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/ground14n.jpg";
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_TexturedGround.init(&M);
			M.clear();


			AssetIO::load("Assets/tmp/lowpolytree.obj", &M);
			setMeshShader(&M, 0.7f, 0.94f);
			M.computePerVertexNormals();
			m_Tree1.init(&M);
			M.clear();

			AssetIO::load("Assets/tmp/Lowpoly_tree_sample.obj", &M);
			setMeshShader(&M, 0.7f, 0.94f);
			M.computePerVertexNormals();
			m_Tree2.init(&M);
			M.clear();


			vector<string> ClearSky;
			ClearSky.push_back("MyAssets/Cloudy/graycloud_rt.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_lf.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_up.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_dn.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_ft.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_bk.jpg");


			m_Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);

			// set initialize color adjustment values
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.1f);
			m_Skybox.saturation(1.2f);

			// create scene graph for the Skybox
			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxGeomSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);

			Quaternionf Rot;
			Rot = AngleAxisf(CForgeMath::degToRad(-2.5f / 60.0f), Vector3f::UnitY());
			m_SkyboxTransSGN.rotationDelta(Rot);

			// build scene graph

			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add ground

			m_GroundSGN.init(&m_RootSGN, &m_TexturedGround);
			m_GroundSGN.scale(Vector3f(500.0f, 500.0f, 500.0f));
			Rot = AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX());
			m_GroundSGN.rotation(Rot);

			// generate a forest
			const uint32_t TreeCount = 200;
			SGNGeometry* pTreeNodes = new SGNGeometry[TreeCount];
			SGNTransformation* pTreeTransNodes = new SGNTransformation[TreeCount];

			
			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());


			ScreenQuad ppquad;
			initPPQuad(&ppquad);

			//T2DImage<uint8_t> shadowBufTex;

			//GUI gui = GUI();
			m_GUI.init(&m_RenderWin);

			FormWidget* form = m_GUI.createOptionsWindow(U"Graphics", 1, U"");
			form->startListening(this);
			form->addOption(GUI_WIREFRAME, INPUTTYPE_BOOL, U"Wireframe");
			form->setDefault(GUI_WIREFRAME, Wireframe);
			form->addOption(GUI_RENDER_AABB, INPUTTYPE_BOOL, U"Render LOD AABBs");
			form->setDefault(GUI_RENDER_AABB, renderLODAABB);
			form->addOption(GUI_FRAMERATE, INPUTTYPE_BOOL, U"Unlock Framerate");
			form->setDefault(GUI_FRAMERATE, false);
			form->addOption(GUI_CAMERA_PANNING, INPUTTYPE_RANGESLIDER, U"Camera Panning Speed");
			form->setLimit(GUI_CAMERA_PANNING, 0.0f, 5.0f);
			form->setStepSize(GUI_CAMERA_PANNING, 0.5f);
			form->setDefault(GUI_CAMERA_PANNING, cameraPanningAcceleration);
			form->addOption(GUI_LODOFFSET, INPUTTYPE_RANGESLIDER, U"LOD Offest");
			form->setLimit(GUI_LODOFFSET, 0.0f, 5.0f);

			//TextWidget* fpsWidget = m_GUI.createPlainText();
			m_pFPSWidget = m_GUI.createPlainText();
			m_pFPSWidget->setTextAlign(TextWidget::ALIGN_RIGHT);

		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override {

			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			/*m_RenderDev.cameraUBO()->position(m_Sun.position());
			m_RenderDev.cameraUBO()->projectionMatrix(m_Sun.projectionMatrix());
			m_RenderDev.cameraUBO()->viewMatrix(m_Sun.viewMatrix());*/

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			m_SkyboxSG.render(&m_RenderDev);

			m_GUI.processEvents();
			m_GUI.render(&m_RenderDev);

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				static int32_t ScreenshotCount = 0;
				T2DImage<uint8_t> Img;
				T2DImage<uint8_t> DepthBuffer;
				CForgeUtility::retrieveFrameBuffer(&Img, &DepthBuffer, 0.1f, 200.0f);
				AssetIO::store("../../Screenshot_" + std::to_string(ScreenshotCount) + ".jpg", &Img);
				AssetIO::store("../../DepthBuffer_" + std::to_string(ScreenshotCount) + ".jpg", &DepthBuffer);

				Img.clear();

				ScreenshotCount++;
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F3, true)) {
				T2DImage<uint8_t> Img;
				m_Sun.retrieveDepthBuffer(&Img);
				AssetIO::store("../../SunDepthbuffer.jpg", &Img);
			}

			static uint64_t LastFPSPrint = CForgeUtility::timestamp();

			if (CForgeUtility::timestamp() - LastFPSPrint > 500) {
				LastFPSPrint = CForgeUtility::timestamp();

				wchar_t text_wstring[100] = { 0 };
				int charcount = swprintf(text_wstring, 100, L"FPS: %.2f", m_FPS);
				std::u32string text;
				//ugly cast to u32string from wchar[]
				for (int i = 0; i < charcount; i++) {
					text.push_back((char32_t)text_wstring[i]);
				}
				m_pFPSWidget->setText(text);
				m_pFPSWidget->setPosition(m_RenderWin.width() - m_pFPSWidget->getWidth(), 0);
			}
				
			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());

		}//mainLoop

	protected:

		void listen(const GUICallbackObject Msg) override {
			if (Msg.FormID == 1) {
				// 			cameraMode = *((int*)Msg.Data.at(1).pData) == 2;
				// 			shadows = *((bool*)Msg.Data.at(2).pData);
				Wireframe = *((bool*)Msg.Data.at(GUI_WIREFRAME).pData);
				renderLODAABB = *((bool*)Msg.Data.at(GUI_RENDER_AABB).pData);
				if (*((bool*)Msg.Data.at(GUI_FRAMERATE).pData)) {   //'unlock framerate'
					glfwSwapInterval(0);
				}
				else {
					glfwSwapInterval(1);
				};
				cameraPanningAcceleration = *((float*)Msg.Data.at(GUI_CAMERA_PANNING).pData);
				// 			CAM_FOV = *((float*)Msg.Data.at(7).pData);
				// 			if (cameraPointerForCallbackHandling) {
				// 				cameraPointerForCallbackHandling->projectionMatrix(WINWIDTH, WINHEIGHT, GraphicsUtility::degToRad(CAM_FOV), 0.1f, 5000.0f);
				// 			}

				// TODO pSLOD->LODOffset = *((float*)Msg.Data.at(GUI_LODOFFSET).pData);
			}
		};

		StaticActor m_TexturedGround;
		StaticActor m_Tree1;
		StaticActor m_Tree2;
		SkyboxActor m_Skybox;

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_GroundSGN;

		enum GUISettings : int {
			GUI_WIREFRAME,
			GUI_RENDER_AABB,
			GUI_FRAMERATE,
			GUI_CAMERA_PANNING,
			GUI_LODOFFSET,
			GUI_FOV
		};
		bool Wireframe = false;
		bool renderLODAABB = true;
		float cameraPanningAcceleration = 1;

		GUI m_GUI;
		TextWidget *m_pFPSWidget;
	};//ForestTestScene


}

#endif