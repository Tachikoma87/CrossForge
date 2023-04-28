/*****************************************************************************\
*                                                                           *
* File(s): LODTestScene.hpp                                            *
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
#ifndef __CFORGE_LODTESTSCENE_HPP__
#define __CFORGE_LODTESTSCENE_HPP__

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"

#include "../../Examples/SceneUtilities.hpp"

#include "../Actor/LODActor.h"

//#include "../LODHandler.h"
#include "../SLOD.h"
#include "../MeshDecimate.h"
#include <chrono>
#include <iostream>

using namespace Eigen;
using namespace std;

//TODO load other Asset from Assets folder
namespace CForge {

	void initPPQuad2(ScreenQuad *quad) {
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
	//void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
	//	for (uint32_t i = 0; i < pM->materialCount(); ++i) {
	//		T3DMesh<float>::Material* pMat = pM->getMaterial(i);
	//		pMat->VertexShaderSources.push_back("Shader/BasicGeometryPass.vert");
	//		pMat->FragmentShaderSources.push_back("Shader/BasicGeometryPass.frag");
	//		pMat->Metallic = Metallic;
	//		pMat->Roughness = Roughness;
	//	}//for[materials]
	//}//setMeshShader

	void LODTestScene(void) {
		SAssetIO* pAssIO = SAssetIO::instance();
		STextureManager* pTexMan = STextureManager::instance();
		SShaderManager* pSMan = SShaderManager::instance();
		SLOD* pSLOD = SLOD::instance();

		bool const LowRes = true;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;
		
		if (LowRes) {
			WinWidth = 1440;//720;
			WinHeight = 1080-80;//576;
		}

		pSLOD->setResolution(Vector2i(WinWidth, WinHeight));

		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), "Absolute Minimum Setup");
		#ifndef __EMSCRIPTEN__
		gladLoadGL();
		#endif
		//glfwSwapInterval(0);

		GraphicsUtility::GPUTraits gpuTraits = GraphicsUtility::retrieveGPUTraits();
		std::cout << "MaxUniformBlockSize: " << gpuTraits.MaxUniformBlockSize << "\n";

		std::string GLError;
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		RenderDevice RDev;
		RenderDevice::RenderDeviceConfig Config;
		Config.DirectionalLightsCount = 1;
		Config.PointLightsCount = 0;
		Config.SpotLightsCount = 0;
		Config.ExecuteLightingPass = true;
		Config.GBufferHeight = WinHeight;
		Config.GBufferWidth = WinWidth;
		Config.pAttachedWindow = &RenderWin;
		Config.PhysicallyBasedShading = true;
		Config.UseGBuffer = true;
		RDev.init(&Config);

		ShaderCode::LightConfig LC;
		LC.DirLightCount = 1;
		LC.PCFSize = 1;
		LC.PointLightCount = 0;
		LC.ShadowBias = 0.0004f;
		LC.ShadowMapCount = 1;
		LC.SpotLightCount = 0;
		pSMan->configShader(LC);


		VirtualCamera Cam;
		Cam.init(Vector3f(0.0f, 0.0f, 5.0f), Vector3f::UnitY());
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(90.0f), 0.1f, 1000.0f);

		Vector3f SunPos = Vector3f(5.0f, 25.0f, 25.0f);
		DirectionalLight Sun;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);

		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);

		SceneGraph SGTest;
		SGNTransformation SGSGN;
		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		SGNTransformation CubeTransformSGN2;
		LODActor Cube;

		T3DMesh<float> M;
		//LODHandler lodHandler;

		//SAssetIO::load("Assets/tree0.obj", &M);
		//lodHandler.generateLODmodels("Assets/tree0.obj");
		
		pSLOD->useLibigl = true;
	//	SAssetIO::load("MyAssets/blub/blub.obj", &M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		Cube.init(&M);
		Cube.generateLODModells();

		pSLOD->useLibigl = false;
		T3DMesh<float> M2;
		SGNGeometry MirrorSGN;
		LODActor Mirror2;
		//SAssetIO::load("MyAssets/blub/blub2.obj", &M2);
		SceneUtilities::setMeshShader(&M2, 0.1f, 0.04f);
		Mirror2.init(&M2);
		Mirror2.generateLODModells();

		LODActor MirrorJoined;
		T3DMesh<float> M3;
		SGNGeometry MirrorJoinedSGN;
		//SAssetIO::load("Assets/mirror/mirrorJoined.obj", &M3);
		//SAssetIO::load("texTest/Textured Vase.obj", &M3);
		//SAssetIO::load("Assets/blub/blub.obj", &M3);
		SceneUtilities::setMeshShader(&M3, 0.1f, 0.04f);
		MirrorJoinedSGN.scale(Eigen::Vector3f(0.05f,0.05f,0.05f));
		M3.computePerVertexNormals();
		MirrorJoined.init(&M3);
		//MirrorJoined.generateLODModells();
		
		//SAssetIO::load("Assets/blub/blub.obj", &M); // complexMan, blub/blub
		//SAssetIO::load("museumAssets/Dragon_0.1.obj", &M);
		//lodHandler.generateLODmodels("Assets/testMeshOut.obj");
		//SAssetIO::load("Assets/cubeSep.obj", &M);
		//lodHandler.generateLODmodels("Assets/cubeSep.obj");
		//SAssetIO::load("Assets/submeshTest.obj", &M);
		//lodHandler.generateLODmodels("Assets/submeshTest.obj");

		//M.computePerVertexNormals();

		//T3DMesh<float> testMesh;
		//MeshDecimator::decimateMesh(&M, &testMesh, 0.5);
		//SceneUtilities::setMeshShader(&testMesh, 0.1f, 0.04f);
		//testMesh.computePerVertexNormals();
		//Cube.init(&testMesh);

		SGSGN.init(nullptr);
		CubeTransformSGN.init(nullptr);
		CubeTransformSGN2.init(nullptr);
		
		bool joined = false;
		CubeSGN.init(&CubeTransformSGN, &Cube);
		MirrorSGN.init(&CubeTransformSGN2, &Mirror2);
		SGTest.init(&SGSGN);
		SGSGN.addChild(&CubeTransformSGN);
		SGSGN.addChild(&CubeTransformSGN2);
		
		//SAssetIO::store("Assets/testMeshOut.obj", &testMesh);
		
		// rotate about the y-axis at 45 degree every second
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(45.0f*100.0f / 60.0f), Vector3f::UnitY());
		//CubeTransformSGN.rotationDelta(R);
		//CubeTransformSGN.translation(Vector3f(0.0, -5.0, 2.0));
		//CubeTransformSGN.translation(Vector3f(0.0, -0.0, 3.0));
		//CubeTransformSGN.translation(Vector3f(0.0,-3.0,3.0));
		CubeTransformSGN.translation(Vector3f(1.0,0.0,4.0));
		CubeTransformSGN2.translation(Vector3f(-1.0,0.0,4.0));
		
		bool Wireframe = true;
		
		glLineWidth(GLfloat(1.0f));
		
		uint32_t cubeLODlevel = 0;
		ScreenQuad ppquad;
		initPPQuad2(&ppquad);
		
		glLineWidth(2);

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			pSLOD->update();
			SceneUtilities::defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse(), 2.5f * pSLOD->getDeltaTime(), 200.0f * pSLOD->getDeltaTime());
			
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
				cubeLODlevel++;
				cubeLODlevel %= /*2;//*/6;
				Cube.bindLODLevel(cubeLODlevel);
				Mirror2.bindLODLevel(cubeLODlevel);
				//MirrorJoined.bindLODLevel(cubeLODlevel);
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
				cubeLODlevel = std::max(0, int32_t(cubeLODlevel)-1);
				Cube.bindLODLevel(cubeLODlevel);
				Mirror2.bindLODLevel(cubeLODlevel);
				//MirrorJoined.bindLODLevel(cubeLODlevel);
			}
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
				CubeTransformSGN.removeAllChildren();
				if (joined) {
					CubeSGN.init(&CubeTransformSGN, &Cube);
					MirrorSGN.init(&CubeTransformSGN, &Mirror2);
				} else {
					MirrorJoinedSGN.init(&CubeTransformSGN, &MirrorJoined);
					MirrorJoinedSGN.scale(Eigen::Vector3f(0.05f,0.05f,0.05f));
				}
				joined = !joined;
			}

			//R = AngleAxisf(GraphicsUtility::degToRad(45.0f*100.0f / 60.0f), Vector3f::UnitY());
			//CubeTransformSGN.rotationDelta(R);
			static float animation = 0.0;
			animation += pSLOD->getDeltaTime()*0.5;
			if (animation > 3.14)
				animation = 0.0;
			//CubeTransformSGN.translation(Vector3f(0.0, 0.0, 0.0+3*sin(animation)));
			
			
			SGTest.update(1.0f*pSLOD->getDeltaTime());
			
			RDev.clearBuffer();
			//RDev.activePass(RenderDevice::RENDERPASS_LOD);
			//SGTest.render(&RDev);
			
			// sorted Geometry front to back
			// std::vector<SGNGeometry> RDev.getLODGeometry();
			// std::vector<Eigen::Matrix4d> RDev.getLODTransforms();
			
			// activate Occlusion Culling in RENDERPASS_GEOMETRY
			// RDev.enableOcclusionCulling();
			
			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				Wireframe = !Wireframe;
			}
#ifndef __EMSCRIPTEN__
			if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
			
			// Terrain vor objekte Rendern um als occluder zu dienen
			
			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			//RDev.renderLODSG();
			
			SGTest.render(&RDev);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_POSTPROCESSING);
			ppquad.render(&RDev);

			RDev.PPBufferFinalize();
			RenderWin.swapBuffers();

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}
			//printf("deltaTime: %f\tFPS:%f", pSLOD->getDeltaTime(), 1.0/pSLOD->getDeltaTime());
			std::stringstream newTitle;
			newTitle << "deltaTime: " << pSLOD->getDeltaTime() << "	FPS: " << 1.0/pSLOD->getDeltaTime();
			RenderWin.title(newTitle.str());
		}//while[main loop]


		pAssIO->release();
		pTexMan->release();
		pSMan->release();
		pSLOD->release();
	}//PITestScene

}

#endif
