#ifndef TERRAIN_SETUP_HPP
#define TERRAIN_SETUP_HPP

#include "CForge/Graphics/Shader/SShaderManager.h"
#include "CForge/Graphics/GLWindow.h"
#include "CForge/Graphics/GraphicsUtility.h"
#include "CForge/Graphics/RenderDevice.h"
#include "CForge/Graphics/Lights/DirectionalLight.h"
#include "CForge/Graphics/SceneGraph/SceneGraph.h"
#include "CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "CForge/Graphics/SceneGraph/SGNTransformation.h"
#include <glad/glad.h>

#include "MapActor.h"

using namespace CForge;
using namespace Eigen;
using namespace std;

namespace Terrain {

    void initCForge(GLWindow* window, RenderDevice* renderDevice) {
        uint32_t winWidth = 720;
        uint32_t winHeight = 720;

        window->init(Vector2i(100, 100), Vector2i(winWidth, winHeight), "Terrain Setup");

        string GLError;
        GraphicsUtility::checkGLError(&GLError);
        if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

        RenderDevice::RenderDeviceConfig renderConfig;
        renderConfig.DirectionalLightsCount = 1;
        renderConfig.PointLightsCount = 0;
        renderConfig.SpotLightsCount = 0;
        renderConfig.ExecuteLightingPass = true;
        renderConfig.GBufferWidth = winWidth;
        renderConfig.GBufferHeight = winHeight;
        renderConfig.pAttachedWindow = window;
        renderConfig.PhysicallyBasedShading = true;
        renderConfig.UseGBuffer = true;
        renderDevice->init(&renderConfig);

        ShaderCode::LightConfig lightConfig;
        lightConfig.DirLightCount = 1;
        lightConfig.PCFSize = 1;
        lightConfig.PointLightCount = 0;
        lightConfig.ShadowBias = 0.0004f;
        lightConfig.ShadowMapCount = 1;
        lightConfig.SpotLightCount = 0;
        SShaderManager* shaderManager = SShaderManager::instance();
        shaderManager->configShader(lightConfig);
        shaderManager->release();

        VirtualCamera camera;
        camera.init(Vector3f(0.0f, 0.0f, 5.0f), Vector3f::UnitY());
        camera.projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
        renderDevice->activeCamera(&camera);

        Vector3f sunPos = Vector3f(5.0f, 25.0f, 25.0f);
        DirectionalLight sun;
        sun.init(sunPos, -sunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 100.0f);
        renderDevice->addLight(&sun);
    }

	void TerrainSetup() {
        bool wireframe = true;

        GLWindow window;
        RenderDevice renderDevice;
        initCForge(&window, &renderDevice);

        MapActor mapActor;
        mapActor.init();

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);
        SGNGeometry mapGeometry;
        mapGeometry.init(&rootTransform, &mapActor);
        SceneGraph sceneGraph;
        sceneGraph.init(&rootTransform);

		while (!window.shutdown()) {
			window.update();

			sceneGraph.update(1.0f);

			renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);

            if (wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(2);
			    sceneGraph.render(&renderDevice);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else {
                sceneGraph.render(&renderDevice);
            }

			renderDevice.activePass(RenderDevice::RENDERPASS_LIGHTING);

			window.swapBuffers();

			if (window.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
				window.closeWindow();
			}
		}
	}
}

#endif