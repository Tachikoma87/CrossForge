#pragma once

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

#include "Terrain/src/Decoration/DekoMesh.hpp"
#include "Terrain/src/Decoration/TreeGenerator.hpp"
#include "Terrain/src/Decoration/RockGenerator.hpp"
#include "Terrain/src/Decoration/GrassGenerator.hpp"
#include "Terrain/src/Decoration/InstanceActor.h"
#include "Terrain/src/Decoration/InstanceSGN.h"

using namespace Eigen;
using namespace std;
using namespace Terrain;

namespace CForge {

	float randomF(float min, float max) {
		return (min + (float)rand() / (float)(RAND_MAX) * (max - min));
	}

	void placeInstances(int ammount, std::vector<InstanceSGN> &nodes, SGNTransformation &trans, SceneGraph &sceneGraph, InstanceActor &iActor) {
		int i = 0;
		float offset = 2.25;
		for (int x = 0; x < ammount; x++) {
			for (int z = 0; z < ammount; z++) {
				
				
				nodes[i].init(&trans, &iActor, Vector3f((x - ammount / 2) * offset + randomF(-offset / 2, offset / 2), -10, -1 - z * offset + randomF(-offset / 2, offset / 2)), Quaternionf::Identity(), Vector3f::Ones());
				i++;
			}
		}
		sceneGraph.init(&trans);
	}

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
		Cam.init(Vector3f(0.0f, 0.0f, 6.0f), Vector3f::UnitY());
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

		InstanceActor iActor;

		DekoMesh M;
		DekoMesh M2;

		std::vector<InstanceSGN> nodes;
		int ammount = 32;
		for (int i = 0; i < ammount * ammount; i++) {
			nodes.push_back(InstanceSGN());
		}

		enum DekoObject {rock, grass, tree, leaves, treeAndLeaves, instanceGrass};

		bool generateNew = false;
		DekoObject selected = instanceGrass;

		switch (selected) {
		case instanceGrass:
			M.load("Assets/grass0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/grass_color.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/grassAlpha.png";

			M.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

			placeInstances(ammount, nodes, objectTransformSGN, SGTest, iActor);
			
			
			
			
			
			break;
		case rock:
			if (generateNew) {
				RockGenerator::generateRocks(RockGenerator::LowPoly, 1, "Assets/");
			}
			M.load("Assets/rock0.obj");
			M.getMaterial(0)->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
			M.getMaterial(0)->TexNormal = "Assets/richard/Rock_035_normal.jpg";
			M.getMaterial(0)->TexDepth = "Assets/richard/Rock_035_Packed.png";

			M.getMaterial(0)->VertexShaderSources.push_back("Shader/RockShader.vert");
			M.getMaterial(0)->FragmentShaderSources.push_back("Shader/RockShader.frag");

			object.init(&M);
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
			

			iActor.clearInstances();
			SGTest.render(&RDev);
			iActor.init(&M);
			iActor.render(&RDev);
			


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
