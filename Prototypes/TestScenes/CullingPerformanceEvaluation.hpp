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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_CULLINGPERFORMANCEEVALUATION_HPP__
#define __CFORGE_CULLINGPERFORMANCEEVALUATION_HPP__

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

namespace CForge {
	
	double CPErand(int range) {
		double r = rand()%range - range/2;
		double f = (double) rand()/RAND_MAX;
		r += f;
		//std::cout << r << "\n";
		return r;
	}
	
	void CullingPerformanceEvaluation(void) {
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
		gladLoadGL();
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
		
		LODActor Cube;
		T3DMesh<float> M;
		AssetIO::load("Assets/cube.obj", & M);
		SceneUtilities::setMeshShader(&M, 0.1f, 0.04f);
		Cube.init(&M);
		
		uint32_t amount = 10000000;
		srand(1);
		Eigen::Matrix4f* mat = new Eigen::Matrix4f[amount];
		for (uint32_t i = 0; i < amount; i++) {
			Eigen::Matrix4f t = GraphicsUtility::translationMatrix(Eigen::Vector3f(CPErand(1000),CPErand(1000),CPErand(1000)));
			Eigen::Matrix4f r = GraphicsUtility::rotationMatrix((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(CPErand(360)), Vector3f::UnitY()));
			mat[i] = t*r;
		}

#define FRUSTUM_C false
		double t = 0.0;
		auto start = std::chrono::steady_clock::now();
		for (uint32_t i = 0; i < amount; i++) {
#if FRUSTUM_C
			Cube.frustumCulling(&RDev,&mat[i]);
#else
			Cube.fovCulling(&RDev,&mat[i]);
#endif
		}
		auto end = std::chrono::steady_clock::now();
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
		std::cout << "m: " << microseconds << "\n";
		t = (double) microseconds;
		std::cout << "t: " << t << "\n";
		std::cout << "t one: " << t / amount << "\n";
		
		delete[] mat;
		mat = nullptr;
		
		pAssIO->release();
		pTexMan->release();
		pSMan->release();
		pSLOD->release();
	}//PITestScene
}

#endif
