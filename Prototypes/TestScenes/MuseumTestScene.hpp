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

#include <iostream>
#include <filesystem>

using namespace Eigen;
using namespace std;

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

namespace CForge {

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
		SLOD* pSLOD = SLOD::instance();
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

		bool Wireframe = false;
		uint32_t modelAmount = 0;
		std::string path = "museumAssets/";
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::cout << entry.path() << std::endl;
			bool accepted = false;
			std::string extension = entry.path().extension().string();
			std::cout << extension << "\n";
			accepted = iequals(extension,".obj")
			        || iequals(extension,".stl");
			
			if (accepted) {
				modelAmount++;
				// load model
				SAssetIO::load(entry.path().string().c_str(), &M);
				SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
				M.computePerVertexNormals();
				M.computeAxisAlignedBoundingBox();
				LODActor* newAct = new LODActor();
				newAct->init(&M);
				museumActors.push_back(newAct);
				M.clear();
				
				// add actor to scene
				SGNGeometry* SGN = new SGNGeometry();
				SGNTransformation* TransformSGN = new SGNTransformation();
				museumSGNG.push_back(SGN);
				museumSGNT.push_back(TransformSGN);
				
				float x = (float)(modelAmount%3)*4.0-7.5;
				float y = (float)(modelAmount/3)*4.0-7.5;

				T3DMesh<float>::AABB aabb = M.aabb();

				float scale = 1.0/aabb.diagonal().norm()*5.0;
				TransformSGN->init(&RootSGN, Vector3f(x, -aabb.Min.y()*scale, y));
				TransformSGN->scale(Vector3f(scale,scale,scale));
				//TransformSGN->rotation((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX()));
				SGN->init(TransformSGN, newAct);
			}
		}
		
		// stuff for performance monitoring
		uint64_t LastFPSPrint = CoreUtility::timestamp();
		int32_t FPSCount = 0;

		std::string GLError = "";
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		ScreenQuad ppquad;
		initPPQuad(&ppquad);
		
		T2DImage<uint8_t> shadowBufTex;

		bool renderLODAABB = true;
		
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
			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse(),0.4*Step,1.0,1.0f / pSLOD->getDeltaTime());

			//Sun.retrieveDepthBuffer(&shadowBufTex);
			//AssetIO::store("testMeshOut.jpg",&shadowBufTex);
			//return;

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

			RenderWin.swapBuffers();
			
			RDev.LODSG_clear();

			FPSCount++;
			if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
				char Buf[64];
				sprintf(Buf, "FPS: %d\n", FPSCount);
				FPS = float(FPSCount);
				FPSCount = 0;
				LastFPSPrint = CoreUtility::timestamp();

				RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
			}

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
		}//while[main loop]

		pSMan->release();
		
		//release museum actors
		for (uint32_t i = 0; i < museumActors.size(); i++) {
			delete museumActors[i];
			delete museumSGNG[i];
			delete museumSGNT[i];
		}

	}//exampleMinimumGraphicsSetup

}

#endif