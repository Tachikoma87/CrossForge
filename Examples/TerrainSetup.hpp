
#ifndef TERRAIN_SETUP_HPP
#define TERRAIN_SETUP_HPP

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

using namespace Eigen;
using namespace std;
using namespace CForge;

namespace Terrain {

	void TerrainSetup() {
		SShaderManager* pSMan = SShaderManager::instance();

		uint32_t winWidth = 1280;
		uint32_t winHeight = 720;

		GLWindow window;
		window.init(Vector2i(100, 100), Vector2i(winWidth, winHeight), "Terrain Setup");

		string GLError;
		GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		RenderDevice renderDevice;
		RenderDevice::RenderDeviceConfig renderConfig;
        renderConfig.DirectionalLightsCount = 1;
        renderConfig.PointLightsCount = 0;
        renderConfig.SpotLightsCount = 0;
        renderConfig.ExecuteLightingPass = true;
        renderConfig.GBufferWidth = winWidth;
        renderConfig.GBufferHeight = winHeight;
        renderConfig.pAttachedWindow = &window;
        renderConfig.PhysicallyBasedShading = true;
        renderConfig.UseGBuffer = true;
		renderDevice.init(&renderConfig);

		ShaderCode::LightConfig lightConfig;
        lightConfig.DirLightCount = 1;
        lightConfig.PCFSize = 1;
        lightConfig.PointLightCount = 0;
        lightConfig.ShadowBias = 0.0004f;
        lightConfig.ShadowMapCount = 1;
        lightConfig.SpotLightCount = 0;
		pSMan->configShader(lightConfig);

		VirtualCamera camera;
		camera.init(Vector3f(0.0f, 0.0f, 5.0f), Vector3f::UnitY());
		camera.projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
        renderDevice.activeCamera(&camera);

		Vector3f sunPos = Vector3f(5.0f, 25.0f, 25.0f);
		DirectionalLight sun;
		sun.init(sunPos, -sunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		renderDevice.addLight(&sun);

        vector<Vector3f> vertices = {
            Vector3f(-1, 0, 0),
            Vector3f(1, 0, 0),
            Vector3f(0, 1, 0),
        };

        // face
        T3DMesh<float>::Face face;
        face.Material = 0;
        face.Vertices[0] = 0;
        face.Vertices[1] = 1;
        face.Vertices[2] = 2;

        // submesh
        T3DMesh<float>::Submesh submesh;
        submesh.Faces.push_back(face);

        // material
        T3DMesh<float>::Material material;
        material.ID = 0;
        material.Color = Vector4f(1, 0, 0, 1);
        material.VertexShaderSources.emplace_back("Shader/BasicGeometryPass.vert");
        material.FragmentShaderSources.emplace_back("Shader/BasicGeometryPass.frag");

        // mesh
		T3DMesh<float> mesh;
        mesh.vertices(&vertices);
        mesh.addMaterial(&material, true);
        mesh.addSubmesh(&submesh, true);
		mesh.computePerVertexNormals();

        printf("%d\n", mesh.vertexCount());

        SceneGraph sceneGraph;
        SGNGeometry cubeGeometry;
        SGNTransformation cubeTransform;
        StaticActor cube;

		cube.init(&mesh);
		cubeTransform.init(nullptr);
		cubeGeometry.init(&cubeTransform, &cube);
		sceneGraph.init(&cubeTransform);

		// rotate about the y-axis at 45 degree every second
//		Quaternionf R;
//		R = AngleAxisf(GraphicsUtility::degToRad(45.0f / 60.0f), Vector3f::UnitY());
//		cubeTransform.rotationDelta(R);

		while (!window.shutdown()) {
			window.update();
			sceneGraph.update(1.0f);

			renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			sceneGraph.render(&renderDevice);

			renderDevice.activePass(RenderDevice::RENDERPASS_LIGHTING);

			window.swapBuffers();

			if (window.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				window.closeWindow();
			}
		}

		pSMan->release();
	}
}

#endif
