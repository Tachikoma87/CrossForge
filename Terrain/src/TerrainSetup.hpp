#pragma once

#include <glad/glad.h>
#include <CForge/Graphics/Lights/DirectionalLight.h>
#include <CForge/Graphics/GraphicsUtility.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>

#include "Terrain/src/Map/TerrainMap.h"

using namespace CForge;

namespace Terrain {

    void initCForge(GLWindow* window, RenderDevice* renderDevice, VirtualCamera* camera, DirectionalLight* sun, DirectionalLight* light) {
        uint32_t winWidth = 720;
        uint32_t winHeight = 720;

        winWidth =  1720;
        winHeight = 1720;

        window->init(Vector2i(100, 100), Vector2i(winWidth, winHeight), "Terrain Setup");
        gladLoadGL();

        string GLError;
        GraphicsUtility::checkGLError(&GLError);
        if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

        RenderDevice::RenderDeviceConfig renderConfig;
        renderConfig.DirectionalLightsCount = 2;
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
        lightConfig.DirLightCount = 2;
        lightConfig.PCFSize = 1;
        lightConfig.PointLightCount = 0;
        lightConfig.ShadowBias = 0.0004f;
        lightConfig.ShadowMapCount = 1;
        lightConfig.SpotLightCount = 0;
        SShaderManager* shaderManager = SShaderManager::instance();
        shaderManager->configShader(lightConfig);
        shaderManager->release();

        camera->init(Vector3f(.0f, 1400.0f, 100.0f), Vector3f::UnitY());
        camera->pitch(GraphicsUtility::degToRad(-15.0f));
        camera->projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 1.0f, 100000.0f);
        renderDevice->activeCamera(camera);

        Vector3f sunPos = Vector3f(2000.0f, 2000.0f, 0.0f);
        sun->init(sunPos, -sunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 2.5f);
        auto projection = GraphicsUtility::perspectiveProjection(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 1.0f, 10000.0f);
        const uint32_t ShadowMapDim = 4096;
        sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(1250, 1250), 1000.0f, 10000.0f);
        renderDevice->addLight(sun);

        Vector3f lightPos = Vector3f(-2000.0f, 2000.0f, -1000.0f);
        light->init(lightPos, -lightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 0.5f);
        renderDevice->addLight(light);
    }

    void initDebugQuad(ScreenQuad* quad) {
        vector<ShaderCode*> vsSources;
        vector<ShaderCode*> fsSources;
        string errorLog;

        SShaderManager* shaderManager = SShaderManager::instance();
        vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "330 core",
                                                            0, "", ""));
        fsSources.push_back(shaderManager->createShaderCode("Shader/DebugQuad.frag", "330 core",
                                                            0, "", ""));
        GLShader *quadShader  = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();

        quad->init(0, 0, 1, 1, quadShader);
    }

    void updateCamera(Mouse* mouse, Keyboard* keyboard, VirtualCamera* camera) {
        if (nullptr == keyboard) return;

        const float movementSpeed = 4;

        float MovementScale = 1.0f;
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) || keyboard->keyPressed(Keyboard::KEY_RIGHT_SHIFT)) {
            MovementScale = 6.0f;
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

	void TerrainSetup() {
        bool wireframe = false;
        bool debugTexture = false;
        bool shadows = true;

        GLWindow window;
        RenderDevice renderDevice;
        VirtualCamera camera;
        DirectionalLight sun, light;
        initCForge(&window, &renderDevice, &camera, &sun, &light);

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);

        ClipMap::ClipMapConfig clipMapConfig = {.sideLength = 64, .levelCount = 8};
        HeightMap::NoiseConfig noiseConfig = {.seed = 0,
                                              .scale = 1.0f,
                                              .octaves = 10,
                                              .persistence = 0.5f,
                                              .lacunarity = 2.0f};
        HeightMap::HeightMapConfig heightMapConfig = {.width = 1024 * 8, .height = 1024 * 8, .noiseConfig = noiseConfig};

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

            if (shadows) {
                renderDevice.activePass(RenderDevice::RENDERPASS_SHADOW, &sun);
                sceneGraph.render(&renderDevice);
            }

			renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);

            if (wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(1);
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
                noiseConfig = {.seed = static_cast<uint32_t>(rand()),
                    .scale = 1.0f,
                    .octaves = 8,
                    .persistence = 0.5f,
                    .lacunarity = 2.0f};
                heightMapConfig = {.width = 1024 * 8, .height = 1024 * 8, .noiseConfig = noiseConfig};
                map.setMapHeight(2000);

                map.generateHeightMap(heightMapConfig);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F5)) {
                window.keyboard()->keyState(Keyboard::KEY_F5, Keyboard::KEY_RELEASED);
                map.heightMapFromTexture(STextureManager::create("Assets/height_map1.jpg"));
                map.setMapHeight(100);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F6)) {
                window.keyboard()->keyState(Keyboard::KEY_F6, Keyboard::KEY_RELEASED);
                shadows = !shadows;
            }
            static float scale = 1.0f;
            if (window.keyboard()->keyPressed(Keyboard::KEY_F8)) {
                window.keyboard()->keyState(Keyboard::KEY_F8, Keyboard::KEY_RELEASED);
                scale *= 1.1;
                map.setMapScale(scale);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F9)) {
                window.keyboard()->keyState(Keyboard::KEY_F9, Keyboard::KEY_RELEASED);
                scale *= 0.9;
                map.setMapScale(scale);
            }
		}
	}
}
