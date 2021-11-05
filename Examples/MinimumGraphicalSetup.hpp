/***************************************************************************\
*                                                                           *
* File(s): PITestScene.hpp													*
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
\***************************************************************************/
#ifndef __CFORGE_PITESTSCENE_HPP__
#define __CFORGE_PITESTSCENE_HPP__

#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/Graphics/Shader/SShaderManager.h"
#include "../CForge/Graphics/STextureManager.h"

#include "../CForge/Graphics/GLWindow.h"
#include "../CForge/Graphics/GraphicsUtility.h"
#include "../CForge/Graphics/RenderDevice.h"

#include "../CForge/Graphics/Lights/DirectionalLight.h"

#include "../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../CForge/Graphics/Actors/StaticActor.h"

#include "../CForge/AssetIO/RockMesh.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
		for (uint32_t i = 0; i < pM->materialCount(); ++i) {
			T3DMesh<float>::Material* pMat = pM->getMaterial(i);



			pMat->TexAlbedo = "Assets/cobbles01.jpg";

			pMat->VertexShaderSources.push_back("Shader/BasicGeometryPass.vert");
			pMat->FragmentShaderSources.push_back("Shader/GrassShader.frag");
			pMat->Metallic = Metallic;
			pMat->Roughness = Roughness;
		}//for[materials]
	}//setMeshShader

	void MinimumGraphicalSetup(void) {
		SAssetIO* pAssIO = SAssetIO::instance();
		STextureManager* pTexMan = STextureManager::instance();
		SShaderManager* pSMan = SShaderManager::instance();

		bool const LowRes = false;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}

		GLWindow RenderWin;
		RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), "Absolute Minimum Setup");


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
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(80.0f), 0.1f, 1000.0f);

		Vector3f SunPos = Vector3f(5.0f, 25.0f, 25.0f);
		DirectionalLight Sun;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);

		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);

		SceneGraph SGTest;
		SGNGeometry CubeSGN;
		SGNTransformation CubeTransformSGN;
		StaticActor Cube;

		RockMesh M;
		setMeshShader(&M, 0.1f, 0.04f);
		M.computePerVertexNormals();
		Cube.init(&M);

		CubeTransformSGN.init(nullptr);
		CubeSGN.init(&CubeTransformSGN, &Cube);
		SGTest.init(&CubeTransformSGN);

		// rotate about the y-axis at 45 degree every second
		Quaternionf R;
		R = AngleAxisf(GraphicsUtility::degToRad(10.0f / 60.0f), Vector3f::UnitY());
		CubeTransformSGN.rotationDelta(R);

		int64_t LastFPSPrint = GetTickCount();
		int32_t FPSCount = 0;

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SGTest.update(1.0f);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			

			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);

			SGTest.render(&RDev);

			RenderWin.swapBuffers();

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
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

}

#endif