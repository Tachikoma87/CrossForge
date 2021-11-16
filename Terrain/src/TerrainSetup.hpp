#ifndef TERRAIN_SETUP_HPP
#define TERRAIN_SETUP_HPP

#include <glad/glad.h>
#include <CForge/Graphics/Lights/DirectionalLight.h>
#include <CForge/Graphics/GraphicsUtility.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/Shader/SShaderManager.h>

#include "Terrain/src/Map/TerrainMap.h"

using namespace CForge;

namespace Terrain {

    void initCForge(GLWindow* window, RenderDevice* renderDevice, VirtualCamera* camera, DirectionalLight* sun) {
        uint32_t winWidth = 720;
        uint32_t winHeight = 720;

        window->init(Vector2i(100, 100), Vector2i(winWidth, winHeight), "Terrain Setup");
        gladLoadGL();

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

        camera->init(Vector3f(.0f, 800.0f, 100.0f), Vector3f::UnitY());
        camera->pitch(GraphicsUtility::degToRad(-15.0f));
        camera->projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 10000.0f);
        renderDevice->activeCamera(camera);

        Vector3f sunPos = Vector3f(10.0f, 100.0f, 0.0f);
        sun->init(sunPos, -sunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 2.0f);
        renderDevice->addLight(sun);
    }

    void updateCamera(Mouse* mouse, Keyboard* keyboard, VirtualCamera* camera) {
        if (nullptr == keyboard) return;

        const float movementSpeed = 2;

        float MovementScale = 1.0f;
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) || keyboard->keyPressed(Keyboard::KEY_RIGHT_SHIFT)) {
            MovementScale = 4.0f;
        }

        if (keyboard->keyPressed(Keyboard::KEY_W) || keyboard->keyPressed(Keyboard::KEY_UP)) camera->forward(movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_S) || keyboard->keyPressed(Keyboard::KEY_DOWN)) camera->forward(-movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_A) || keyboard->keyPressed(Keyboard::KEY_LEFT)) camera->right(-movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_D) || keyboard->keyPressed(Keyboard::KEY_RIGHT)) camera->right(movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_ALT)) camera->up(-movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) camera->up(movementSpeed * MovementScale);

        // rotation
        if (mouse->buttonState(Mouse::BTN_RIGHT)) {
            Vector2f MouseDelta = mouse->movement();

            camera->rotY(GraphicsUtility::degToRad(MouseDelta.x()) * -0.1f * movementSpeed);
            camera->pitch(GraphicsUtility::degToRad(MouseDelta.y()) * -0.1f * movementSpeed);

            mouse->movement(Eigen::Vector2f::Zero());
        }
    }

    void initDebugQuad(ScreenQuad* quad) {
        vector<ShaderCode*> vsSources;
        vector<ShaderCode*> fsSources;
        string errorLog;

        SShaderManager* shaderManager = SShaderManager::instance();
        vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "330 core",
                                                            0, "", ""));
        fsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.frag", "330 core",
                                                            0, "", ""));
        GLShader *quadShader  = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();

        quad->init(0, 0, 1, 1, quadShader);
    }

	void TerrainSetup() {
        bool wireframe = false;
        bool debugTexture = false;

        GLWindow window;
        RenderDevice renderDevice;
        VirtualCamera camera;
        DirectionalLight sun;
        initCForge(&window, &renderDevice, &camera, &sun);

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);

        ClipMap::ClipMapConfig clipMapConfig = {.sideLength = 64, .levelCount = 4};
        HeightMap::HeightMapConfig heightMapConfig = {.width = 1024, .height = 1024};

        TerrainMap map = TerrainMap(&rootTransform);
        map.generateClipMap(clipMapConfig);
        map.generateHeightMap(heightMapConfig);

        SceneGraph sceneGraph;
        sceneGraph.init(&rootTransform);

        ScreenQuad quad;
        initDebugQuad(&quad);

		while (!window.shutdown()) {
			window.update();

			sceneGraph.update(1.0f);

            map.update(camera.position().x(), camera.position().z());

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

            renderDevice.activePass(RenderDevice::RENDERPASS_FORWARD);

            if (debugTexture) {
                glActiveTexture(GL_TEXTURE0);
                map.bindTexture();
                quad.render(&renderDevice);
            }

			window.swapBuffers();

            updateCamera(window.mouse(), window.keyboard(), renderDevice.activeCamera());

            if (window.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
                window.closeWindow();
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F1)) {
                window.keyboard()->keyState(Keyboard::KEY_F1, Keyboard::KEY_RELEASED);
                wireframe = !wireframe;
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F2)) {
                window.keyboard()->keyState(Keyboard::KEY_F2, Keyboard::KEY_RELEASED);
                debugTexture = !debugTexture;
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F3)) {
                window.keyboard()->keyState(Keyboard::KEY_F3, Keyboard::KEY_RELEASED);
                clipMapConfig = {.sideLength = 256, .levelCount = 5};

                map.generateClipMap(clipMapConfig);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F4)) {
                window.keyboard()->keyState(Keyboard::KEY_F4, Keyboard::KEY_RELEASED);
                heightMapConfig = {.width = 1024 * 8, .height = 1024 * 8};

                map.generateHeightMap(heightMapConfig);
            }
		}
	}
}

#endif