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

#include "CForge/AssetIO/SAssetIO.h"
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "CForge/Graphics/STextureManager.h"

#include "CForge/Graphics/GLWindow.h"
#include "CForge/Graphics/GraphicsUtility.h"
#include "CForge/Graphics/RenderDevice.h"

#include "CForge/Graphics/Lights/DirectionalLight.h"

#include "CForge/Graphics/SceneGraph/SceneGraph.h"
#include "CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "CForge/Graphics/Actors/StaticActor.h"

#include "Terrain/src/Decoration/RockMesh.hpp"
#include "Terrain/src/Decoration/DekoMesh.hpp"
#include "Terrain/src/Decoration/TreeGenerator.hpp"
#include "Terrain/src/Decoration/RockGenerator.hpp"
#include "Terrain/src/Decoration/GrassGenerator.hpp"

using namespace Eigen;
using namespace std;
using namespace Terrain;

namespace CForge {

	void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
		for (uint32_t i = 0; i < pM->materialCount(); ++i) {
			T3DMesh<float>::Material* pMat = pM->getMaterial(i);

			//pMat->TexAlbedo = "Assets/richard/leaves3_color.png";
			//pMat->TexDepth = "Assets/richard/leaves3_alpha2.png";

			//pMat->TexAlbedo = "Assets/richard/free grass.png";
			//pMat->TexDepth = "Assets/richard/grassAlpha.png";

			//pMat->TexAlbedo = "Assets/richard/Aspen_bark_001_COLOR.jpg";
			//pMat->TexNormal = "Assets/richard/Aspen_bark_001_NORM.jpg";
			//pMat->TexDepth = "Assets/richard/Aspen_bark_001_Packed.png";

			//pMat->TexAlbedo = "Assets/richard/Bark_06_baseColor.jpg";
			//pMat->TexNormal = "Assets/richard/Bark_06_normal.jpg";
			//pMat->TexDepth = "Assets/richard/Bark_06_Packed.png";

			//pMat->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
			//pMat->TexNormal = "Assets/richard/Rock_035_normal.jpg";
			//pMat->TexDepth = "Assets/richard/Rock_035_Packed.png";

			//pMat->TexAlbedo = "Assets/richard/Rock_040_basecolor.jpg";
			//pMat->TexNormal = "Assets/richard/Rock_040_normal.jpg";
			//pMat->TexDepth = "Assets/richard/Rock_040_Packed.png";

			pMat->VertexShaderSources.push_back("Shader/RockShader.vert");
			//pMat->FragmentShaderSources.push_back("Shader/RockShader.frag");
			pMat->FragmentShaderSources.push_back("Shader/GrassShader.frag");
			pMat->Metallic = Metallic;
			pMat->Roughness = Roughness;
		}//for[materials]
	}//setMeshShader

	void DecoSetup(void) {
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
		//Cam.init(Vector3f(0.0f, 5.0f, 7.0f), Vector3f::UnitY());
		Cam.init(Vector3f(0.0f, 5.0f, 7.0f), Vector3f::UnitY());
		Cam.yaw(GraphicsUtility::degToRad(0));
		//Cam.init(Vector3f(0.0f, 0.0f, -5.0f), Vector3f::UnitY());
		//Cam.yaw(GraphicsUtility::degToRad(180));
		Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(80.0f), 0.1f, 1000.0f);

		Vector3f SunPos = Vector3f(30, 30, 30);
		DirectionalLight Sun;
		Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);

		RDev.activeCamera(&Cam);
		RDev.addLight(&Sun);



		SceneGraph SGTest;
		SGNTransformation objectTransformSGN;
		objectTransformSGN.init(nullptr);
		SGNGeometry objectSGN;
		StaticActor object;
		SGNGeometry objectSGN2;
		StaticActor object2;

		DekoMesh M;
		DekoMesh M2;

		enum DekoObject {rock, grass, tree, leaves, treeAndLeaves};
		bool generateNew = true;

		switch (treeAndLeaves) {
		case rock:
			if (generateNew) {
				RockGenerator::generateRocks(RockGenerator::Normal, 1, "Assets/");
			}
			M.load("Assets/rock0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
			M.getMaterial(0)->TexNormal = "Assets/richard/Rock_035_normal.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/Rock_035_Packed.png";
			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/RockShader.frag");

			object.init(&M);
			objectTransformSGN.translation(Vector3f(0, 5, 0));
			objectSGN.init(&objectTransformSGN, &object);
			SGTest.init(&objectTransformSGN);
			break;
		case grass:
			if (generateNew) {
				GrassGenerator::generateGrass(GrassType::triangle, 1, "Assets/");
			}
			M.load("Assets/grass0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/grass_color.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/grassAlpha.png";
			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/GrassShader.frag");
			
			object.init(&M);
			objectTransformSGN.translation(Vector3f(0, 3, 0));
			objectSGN.init(&objectTransformSGN, &object);
			SGTest.init(&objectTransformSGN);
			break;
		case tree:
			if (generateNew) {
				TreeGenerator::generateTrees(TreeGenerator::Normal, 1, "Assets/");
			}
			M.load("Assets/tree0.obj");
			//M.getMaterial(0)->TexAlbedo = "Assets/richard/Aspen_bark_001_COLOR.jpg";
			//M.getMaterial(0)->TexNormal = "Assets/richard/Aspen_bark_001_NORM.jpg";
			//M.getMaterial(0)->TexDepth = "Assets/richard/Aspen_bark_001_Packed.png";

			M.getMaterial(0)->TexAlbedo = "Assets/richard/Bark_06_baseColor.jpg";
			M.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/GrassShader.frag");

			object.init(&M);
			objectSGN.init(&objectTransformSGN, &object);
			SGTest.init(&objectTransformSGN);
			break;
		case leaves:
			if (generateNew) {
				TreeGenerator::generateTrees(TreeGenerator::Normal, 1, "Assets/");
			}
			M.load("Assets/leaves0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/leaves3_color.png";
			M.getMaterial(0)->TexDepth = "Assets/richard/leaves3_alpha.jpg";
			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/GrassShader.frag");

			object.init(&M);
			objectSGN.init(&objectTransformSGN, &object);
			SGTest.init(&objectTransformSGN);
			break;
		case treeAndLeaves:
			if (generateNew) {
				TreeGenerator::generateTrees(TreeGenerator::Normal, 1, "Assets/");
			}

			M.load("Assets/tree0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/Bark_06_baseColor.jpg";
			M.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/GrassShader.frag");

			object.init(&M);
			objectSGN.init(&objectTransformSGN, &object);
			SGTest.init(&objectTransformSGN);


			M2.load("Assets/leaves0.obj");
			M2.getMaterial(0)->TexAlbedo = "Assets/richard/leaves3_color.png";
			M2.getMaterial(0)->TexDepth = "Assets/richard/leaves3_alpha.jpg";
			M2.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M2.getMaterial(0)->FragmentShaderSources.push_back("Shader/GrassShader.frag");

			object2.init(&M2);
			objectSGN2.init(&objectTransformSGN, &object2);
			SGTest.init(&objectTransformSGN);
			break;
		}

		//printf("\n\nVector x: %f, y: %f, z: %f\n\n", testVector.x(), testVector.y(), testVector.z());

		// rotate about the y-axis at 45 degree every second
		Quaternionf R;

		R = AngleAxisf(GraphicsUtility::degToRad(15.0f / 60.0f), Vector3f::UnitY());
		objectTransformSGN.rotationDelta(R);

		int32_t FPSCount = 0;

		while (!RenderWin.shutdown()) {
			RenderWin.update();
			SGTest.update(1.0f);

			RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			
			SGTest.render(&RDev);
			
			RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			RDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			

			RenderWin.swapBuffers();

			if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				RenderWin.closeWindow();
			}

			FPSCount++;

		}//while[main loop]


		pAssIO->release();
		pTexMan->release();
		pSMan->release();

	}//PITestScene

}

#endif