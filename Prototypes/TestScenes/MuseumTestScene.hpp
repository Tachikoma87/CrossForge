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
#ifndef __CFORGE_MUSEUMTESTSCENE_HPP__
#define __CFORGE_MUSEUMTESTSCENE_HPP__

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
#include <thread>

using namespace Eigen;
using namespace std;

using namespace CForge;
#define THREADS 6
#define LOD_RENDERING true // make sure to change the corresponding macro in LODActor.cpp

class MuseumTestScene : public ITListener<GUICallbackObject> {
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

	void museumTestScene(void) {
		SShaderManager* pSMan = SShaderManager::instance();
		pSLOD = SLOD::instance();
		LODHandler lodHandler;
		std::string WindowTitle = "CForge - Minimum Graphics Setup";
		float FPS = 60.0f;

		bool const LowRes = true;

		uint32_t WinWidth = 1920;
		uint32_t WinHeight = 1080;

		if (LowRes) {
			WinWidth = 960;
			WinHeight = 540;
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

		uint32_t shadowMapRes = /*/*/8192;//4096;/*/1024;//*/
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
		pSLOD->setCFOV(45.0f);

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

		//SAssetIO::load("museumAssets/Dragon_0.02.obj", &M);
		//SAssetIO::load("Assets/sphere.obj", &M);
		//SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		//M.computePerVertexNormals();
		//Cube.init(&M);
		//M.clear();

		// build scene graph
		SceneGraph SG;
		SGNTransformation RootSGN;
		RootSGN.init(nullptr);
		SG.init(&RootSGN);

		// add skydome
		SGNGeometry SkydomeSGN;
		SkydomeSGN.init(&RootSGN, &Skydome);
		SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

		std::vector<LODActor*> museumActors;
		std::vector<SGNGeometry*> museumSGNG;
		std::vector<SGNTransformation*> museumSGNT;
		
		std::vector<T3DMesh<float>*> meshes;
		uint32_t modelAmount = 0;
		std::string path = "museumAssets/";
		std::thread generationThreads[THREADS];
		std::atomic<bool>* generationThreadsDone[THREADS];
		LODActor* generationThreadActors[THREADS];
		for (uint32_t i = 0; i < THREADS; i++) {
			generationThreadsDone[i] = new std::atomic<bool>(true);
			generationThreadActors[i] = nullptr;
		}
		
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::cout << entry.path() << std::endl;
			bool accepted = false;
			std::string extension = entry.path().extension().string();
			std::cout << extension << "\n";
			accepted = iequals(extension,".obj")
			        || iequals(extension,".stl")
			        || iequals(extension,".fbx")
			        || iequals(extension,".ply");
			
			if (accepted) {
				modelAmount++;
				// load model
				T3DMesh<float>* M = new T3DMesh<float>();
				meshes.push_back(M);
				SAssetIO::load(entry.path().string().c_str(), M);
				SceneUtilities::setMeshShader(M, 0.1f, 0.04f);
				M->computePerVertexNormals();
				M->computeAxisAlignedBoundingBox();
				
				//float triangleSize = lodHandler.getTriSizeInfo(*M,LODHandler::TRI_S_AVG);
				//std::cout << triangleSize << "\n";
				//
				//// get 2 longest sides of aabb
				//Eigen::Vector3f diag = M->aabb().diagonal();
				//float a = max(diag.x(),max(diag.y(),diag.z()));
				//float b = diag.x()+diag.y()+diag.z()-a-min(diag.x(),min(diag.y(),diag.z()));
				//
				//float aabbTriRatio = triangleSize / (a*b);
				//std::cout << a*b << "\n";
				//std::cout << aabbTriRatio << "\n";
				
				LODActor* newAct = new LODActor();
				newAct->init(M);
				museumActors.push_back(newAct);
				//M->clear();
				
				// add actor to scene
				SGNGeometry* SGN = new SGNGeometry();
				SGNTransformation* TransformSGN = new SGNTransformation();
				museumSGNG.push_back(SGN);
				museumSGNT.push_back(TransformSGN);
				
				float x = (float)(modelAmount%4)*6.0-7.5;
				float y = (float)(modelAmount/4)*6.0-7.5;

				T3DMesh<float>::AABB aabb = M->aabb();

				float scale = 1.0/aabb.diagonal().norm()*5.0;
				TransformSGN->init(&RootSGN, Vector3f(x, -aabb.Min.y()*scale, y));
				TransformSGN->scale(Vector3f(scale,scale,scale));
				Eigen::Vector3f center = -(aabb.Max*0.5+0.5*aabb.Min);
				TransformSGN->translation(TransformSGN->translation()+Eigen::Vector3f(center.x(),0.0,center.z())*scale);
				//TransformSGN->rotation((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX()));
				SGN->init(TransformSGN, newAct);
				//LODHandler().loadLODmeshes(newAct, entry.path().string(), *pSLOD->getLevels(), std::vector<float>());
				bool gotThread = false;
				while (!gotThread) {
					for (uint32_t i = 0; i < THREADS; i++) {
						std::atomic<bool>* done = generationThreadsDone[i];
						if (*done) { // thread available
							if (generationThreads[i].joinable()) {
								generationThreads[i].join();
								generationThreadActors[i]->initiateLODBuffers();
								generationThreadActors[i]->freeLODMeshes();
								generationThreadActors[i] = nullptr;
							}
							*done = false;
							LODActor** pActor = &(generationThreadActors[i]);
							*pActor = newAct;
							generationThreads[i] = std::thread([=] {
#if LOD_RENDERING
								LODHandler().loadLODmeshes(newAct, entry.path().string(), *pSLOD->getLevels(), std::vector<float>());
								newAct->calculateLODPercentages();
#endif
								*done = true;
							});
							
							gotThread = true;
							break;
						}
					}
				}
				
				//newAct->calculateLODPercentages();
			}
		}
		for (uint32_t i = 0; i < THREADS; i++) {
			if (generationThreads[i].joinable()) {
				generationThreads[i].join();
#if LOD_RENDERING
				generationThreadActors[i]->initiateLODBuffers();
#endif
				generationThreadActors[i]->freeLODMeshes();
				delete meshes[i];
				meshes[i] = nullptr;
				generationThreadActors[i] = nullptr;
			}
		}

		//for (uint32_t i = 0; i < museumActors.size(); i++) {
		//	auto newAct = museumActors[i];
		//	newAct->initiateLODBuffers();
		//	newAct->freeLODMeshes();
		//}
		
		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		uint64_t FPSTCount = 0;
		double FPSTmean = 0.0;

		int32_t FPSCount = 0;
		float FPSmean = 0.0;

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
		form->setLimit ( GUI_LODOFFSET, 0.0f, 100.0 );
		form->setStepSize ( GUI_LODOFFSET, 1.0f ); // TODO
		form->setDefault ( GUI_LODOFFSET, (float) pSLOD->TriangleSize );
//         form->addOption ( 7, INPUTTYPE_RANGESLIDER, U"Field of View" );
//         form->setLimit ( 7, 60.0f, 120.0f );
//         form->setStepSize ( 7, 1.0f );
//         form->setDefault(7, 90);

// 		FormWidget* form2 = gui.createOptionsWindow(U"WindowWidget", 1);
// //         form2->startListening(&callbacktest);
//         form2->addOption(1, INPUTTYPE_INT, U"InputNumberWidget");
//         form2->setLimit(1, 10);
//         form2->addOption(2, INPUTTYPE_BOOL, U"InputCheckboxWidget");
//         form2->addOption(3, INPUTTYPE_STRING, U"InputTextWidget");
//         form2->setLimit(3, 5);
//         form2->setDefault(3, std::u32string(U"Text"));
//         form2->addOption(5, INPUTTYPE_RANGESLIDER, U"InputSliderWidget");
//         form2->setLimit(5, 10);
//         form2->setStepSize(5, 0.3f);
//         form2->setDefault(5, 1.5f);
//         form2->addOption(4, INPUTTYPE_DROPDOWN, U"InputDropDownWidget");
//         std::map<int, std::u32string> testDropdown;
//         testDropdown[1] = U"First Option";
//         testDropdown[2] = U"Second Option";
//         testDropdown[3] = U"Third Option";
//         form2->setDropDownOptions(4, testDropdown);
//
// 		form2 = gui.createOptionsWindow(U"WindowWidget", 42, U"");
// //         form2->startListening(&callbacktest);
// //         form2->addOption(1, INPUTTYPE_INT, U"InputNumberWidget");
// //         form2->setLimit(1, 10);
// //         form2->addOption(2, INPUTTYPE_BOOL, U"InputCheckboxWidget");
// //         form2->addOption(3, INPUTTYPE_STRING, U"InputTextWidget");
// //         form2->setLimit(3, 5);
// //         form2->setDefault(3, std::u32string(U"Text"));
//         form2->addOption(5, INPUTTYPE_RANGESLIDER, U"InputSliderWidget");
//         form2->setLimit(5, 10);
//         form2->setStepSize(5, 0.3f);
//         form2->setDefault(5, 1.5f);
// //         form2->addOption(4, INPUTTYPE_DROPDOWN, U"InputDropDownWidget");
// //         form2->setDropDownOptions(4, testDropdown);
//
// 		TextWidget* ww = gui.createTextWindow(U"WindowWidget");
// 		ww->setText(U"Ein WindowWidget welches ein\neinfaches TextWidget beherbergt.");

// 		TextWidget* fpsWidget = gui.createTextWindow(U"Information");
		TextWidget* fpsWidget = gui.createPlainText();
		fpsWidget->setTextAlign(TextWidget::ALIGN_RIGHT);
		
		double FPSTmean = 0.0;
		uint64_t FPSTCount = 0;
		
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
			Step *= 60.0/(1.0/pSLOD->getDeltaTime());
			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse(), 0.4*Step, cameraPanningAcceleration, 50.0);

			//Sun.retrieveDepthBuffer(&shadowBufTex);
			//AssetIO::store("testMeshOut.jpg",&shadowBufTex);
			//return;
#if LOD_RENDERING
			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			RDev.activePass(RenderDevice::RENDERPASS_LOD);
			SG.render(&RDev);

			RDev.LODSG_assemble();
#endif
			RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
#if LOD_RENDERING
			RDev.LODSG_render();
#else
			SG.render(&RDev);
#endif

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			if (Wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(1);
			}
#if LOD_RENDERING
			RDev.LODSG_render();
#else
			SG.render(&RDev);
#endif
			if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			
#if LOD_RENDERING
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
#endif
			
			RDev.activePass(RenderDevice::RENDERPASS_POSTPROCESSING);
			ppquad.render(&RDev);

// 			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			gui.processEvents();
			gui.render(&RDev);

			RDev.PPBufferFinalize();
			RenderWin.swapBuffers();
			
#if LOD_RENDERING
			RDev.LODSG_clear();
#endif
			FPSCount++;
			FPSmean += pSLOD->getDeltaTime();
			
			FPSTCount++;
			FPSTmean += pSLOD->getDeltaTime();
			
			if (CoreUtility::timestamp() - LastFPSPrint > 100U) {
				
				wchar_t text_wstring[100] = {0};
				FPSmean /= FPSCount;
				int charcount = swprintf(text_wstring, 100, L"FPS: %f\n Frametime: %f", 1.0/FPSmean, FPSmean);
				std::u32string text = wstringToU32String(std::wstring(text_wstring, charcount));
				fpsWidget->setText(text);
				fpsWidget->setPosition(RenderWin.width()-fpsWidget->getWidth(), 0);
				
				LastFPSPrint = CoreUtility::timestamp();
				FPSmean = 0.0;
				FPSCount = 0;
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]
		
		FPSTmean /= FPSTCount;
		std::cout << "FPSTmean: " << FPSTmean << "\n";
		
		pSMan->release();
		
		//release museum actors
		for (uint32_t i = 0; i < museumActors.size(); i++) {
			delete museumActors[i];
			delete museumSGNG[i];
			delete museumSGNT[i];
		}
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

			pSLOD->TriangleSize = (int) *((float*)Msg.Data.at(GUI_LODOFFSET).pData);
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
	bool renderLODAABB = false;
	float cameraPanningAcceleration = 1;
	SLOD* pSLOD;
};

#endif
