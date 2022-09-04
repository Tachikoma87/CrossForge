/*****************************************************************************\
*                                                                           *
* File(s): SkyboxTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_GUITESTSCENE_HPP__
#define __CFORGE_GUITESTSCENE_HPP__

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"


#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"

#include "../../Examples/SceneUtilities.hpp" 

#include "../Actor/LODActor.h"

#include "../LODHandler.h"
#include "../SLOD.h"
#include "../MeshDecimate.h"

//since the includes of the GUI source files are so convoluted, this is enough to pull in the entirety of the GUI
//(or more like, the entirety of classes we would want to interface with)
#include "Terrain/GUI/Widgets/Form.h"

#include <iostream>
#include <filesystem>
#include <wchar.h>

using namespace Eigen;
using namespace std;

using namespace CForge;

class GUITestScene : public ITListener<GUICallbackObject> {
public:
	//https://stackoverflow.com/questions/11635/case-insensitive-string-comparison-in-c
	bool iequals(const string& a, const string& b)
	{
		unsigned int sz = a.size();
		if (b.size() != sz)
			return false;
		for (unsigned int i = 0; i < sz; ++i)
			if (tolower(a[i]) != tolower(b[i]))
				return false;
		return true;
	}

	void initPPQuad(ScreenQuad *quad) {
		vector<ShaderCode *> vsSources;
		vector<ShaderCode *> fsSources;
		string errorLog;

		SShaderManager *shaderManager = SShaderManager::instance();
		vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "420 core",
			0, "", ""));

		"";
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
			0, "", ""));
		GLShader *quadShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
		shaderManager->release();

		quad->init(0, 0, 1, 1, quadShader);
	}

	void guiTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();
		pSLOD = SLOD::instance();
		LODHandler lodHandler;
		std::string WindowTitle = "CForge - Minimum Graphics Setup";
		float FPS = 60.0f;

		bool const LowRes = true;

		uint32_t WinWidth = 1920;
		uint32_t WinHeight = 1080;

		if (LowRes) {
			WinWidth = 1280;
			WinHeight = 1024;
		}
		pSLOD->setResolution(Vector2i(WinWidth,WinHeight));
		// create an OpenGL capable windows
		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);
		gladLoadGL();

		// configure and initialize rendering pipeline
		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 1;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = WinHeight;
		Config.GBufferWidth = WinWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		uint32_t shadowMapRes = /*//8192;//4096;/*/1024;//*/
														// configure and initialize shader configuration device
		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PointLightCount = 1;
		LC.SpotLightCount = 0;
		LC.PCFSize = 1;
		LC.ShadowBias = 0.00004f/shadowMapRes*1024; //dynamic bias?
		LC.ShadowMapCount = 1;
		pSMan->configShader(LC);

		// initialize camera
		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
		DirectionalLight Sun;
		PointLight BGLight;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		// sun will cast shadows
		Sun.initShadowCasting(shadowMapRes, shadowMapRes, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
		BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.0f, Vector3f(0.0f, 0.0f, 0.0f));

		// set camera and lights
		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);
		RDev.addLight(&BGLight);

		// load skydome and a textured cube
		T3DMesh<float> M;
		StaticActor Skydome;

		SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
		SceneUtilities::setMeshShader(&M, 0.8f, 0.04f);
		M.computePerVertexNormals();
		Skydome.init(&M);
		M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));
		
		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		ScreenQuad ppquad;
		initPPQuad(&ppquad);

		T2DImage<uint8_t> shadowBufTex;

		GUI gui = GUI();
		gui.init (&RenderWin);

		FormWidget* form = gui.createOptionsWindow ( U"Graphics", 1, U"" );
		form->startListening ( this );
		form->addOption ( GUI_WIREFRAME, INPUTTYPE_BOOL, U"Wireframe" );
		form->setDefault ( GUI_WIREFRAME, Wireframe );
		form->addOption ( GUI_RENDER_AABB, INPUTTYPE_BOOL, U"Render LOD AABBs" );
		form->setDefault ( GUI_RENDER_AABB, renderLODAABB );
		form->addOption ( GUI_FRAMERATE, INPUTTYPE_BOOL, U"Unlock Framerate" );
		form->setDefault ( GUI_FRAMERATE, false );
		form->addOption ( GUI_CAMERA_PANNING, INPUTTYPE_RANGESLIDER, U"Camera Panning Speed" );
		form->setLimit ( GUI_CAMERA_PANNING, 0.0f, 5.0f );
		form->setStepSize ( GUI_CAMERA_PANNING, 0.5f );
		form->setDefault ( GUI_CAMERA_PANNING, cameraPanningAcceleration );
		form->addOption ( GUI_LODOFFSET, INPUTTYPE_RANGESLIDER, U"LOD Offest" );
		form->setLimit ( GUI_LODOFFSET, 0.0f, 5.0f );
		//form->setStepSize ( GUI_LODOFFSET, 0.1f );
		//form->setDefault ( GUI_LODOFFSET, pSLOD->LODOffset );
		//         form->addOption ( 7, INPUTTYPE_RANGESLIDER, U"Field of View" );
		//         form->setLimit ( 7, 60.0f, 120.0f );
		//         form->setStepSize ( 7, 1.0f );
		//         form->setDefault(7, 90);

		TextWidget* fpsWidget = gui.createPlainText();
		fpsWidget->setTextAlign(TextWidget::ALIGN_RIGHT);

		while (!RenderWin.shutdown()) {
			RDev.clearBuffer();
			RenderWin.update();
			pSLOD->update();
			SG.update(1.0f);

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				Wireframe = !Wireframe;
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
				renderLODAABB = !renderLODAABB;
			}
			Keyboard* pKeyboard = RenderWin.keyboard();
			float Step = (pKeyboard->keyState(Keyboard::KEY_Q) == 0) ? 0.1f : 1.0f;
			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse(), 0.4*Step, cameraPanningAcceleration, 1.0f / pSLOD->getDeltaTime());

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			RDev.activePass(RenderDevice::RENDERPASS_LOD);
			SG.render(&RDev);

			RDev.LODSG_assemble();
			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
			glDisable(GL_CULL_FACE);
			RDev.LODSG_render();
			glEnable(GL_CULL_FACE);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			if (Wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(1);
			}
			RDev.LODSG_render();
			if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);

			//render debug aabb
			if (renderLODAABB) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_CULL_FACE);
				glLineWidth(2);
				for (uint32_t i = 0; i < RDev.getLODSGActors().size(); i++) {
					if (RDev.getLODSGActors()[i]->className().compare("IRenderableActor::LODActor") == 0) {
						LODActor* act = (LODActor*) RDev.getLODSGActors()[i];
						uint8_t lvl = act->getLODLevel();
						glColorMask(lvl&4,lvl&2,lvl&1,GL_TRUE);
						RDev.setModelMatrix(RDev.getLODSGTrans()[i]);
						act->renderAABB(&RDev);
					}
				}
				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
				glEnable(GL_CULL_FACE);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			RDev.activePass(RenderDevice::RENDERPASS_POSTPROCESSING);
			ppquad.render(&RDev);

			gui.processEvents();
			gui.render(&RDev);
			
			RDev.PPBufferFinalize();
			
			RenderWin.swapBuffers();

			RDev.LODSG_clear();

			FPSCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", FPSCount);

				wchar_t text_wstring[100] = {0};
				int charcount = swprintf(text_wstring, 100, L"FPS: %d\nZweite Zeile", FPSCount);
				std::u32string text;
				//ugly cast to u32string from wchar[]
				for (int i = 0; i < charcount; i++) {
					text.push_back((char32_t)text_wstring[i]);
				}
				fpsWidget->setText(text);
				fpsWidget->setPosition(RenderWin.width()-fpsWidget->getWidth(), 0);

				FPS = float(FPSCount);
				FPSCount = 0;
				LastFPSPrint = CoreUtility::timestamp();

				// 				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();

	}//exampleMinimumGraphicsSetup

	void listen ( const GUICallbackObject Msg ) override {
		if (Msg.FormID == 1) {
			// 			cameraMode = *((int*)Msg.Data.at(1).pData) == 2;
			// 			shadows = *((bool*)Msg.Data.at(2).pData);
			Wireframe = *((bool*)Msg.Data.at(GUI_WIREFRAME).pData);
			renderLODAABB = *((bool*)Msg.Data.at(GUI_RENDER_AABB).pData);
			if (*((bool*)Msg.Data.at(GUI_FRAMERATE).pData)) {   //'unlock framerate'
				glfwSwapInterval(0);
			} else {
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

private:
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
	SLOD* pSLOD;
};

#endif
