#pragma once

#include <glad/glad.h>
#include <CForge/Graphics/Lights/DirectionalLight.h>
#include <CForge/Graphics/GraphicsUtility.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>

#include "Terrain/src/Map/TerrainMap.h"
#include "./Decoration/DecoSetup.hpp"

using namespace CForge;

namespace Terrain {

    void initCForge(GLWindow *window, RenderDevice *renderDevice, VirtualCamera *camera, DirectionalLight *sun,
                    DirectionalLight *light) {
        uint32_t winWidth = 720;
        uint32_t winHeight = 720;

        winWidth = 1200;
        winHeight = 1200;

        window->init(Vector2i(200, 200), Vector2i(winWidth, winHeight), "Terrain Setup");
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
        SShaderManager *shaderManager = SShaderManager::instance();
        shaderManager->configShader(lightConfig);
        shaderManager->release();

        camera->init(Vector3f(.0f, 400.0f, 100.0f), Vector3f::UnitY());
        camera->pitch(GraphicsUtility::degToRad(-15.0f));
        camera->projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), 1.0f, 100000.0f);
        renderDevice->activeCamera(camera);

        Vector3f sunPos = Vector3f(600.0f, 600.0f, 0.0f);
        sun->init(sunPos, -sunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 2.5f);
        auto projection = GraphicsUtility::perspectiveProjection(winWidth, winHeight, GraphicsUtility::degToRad(45.0f),
                                                                 1.0f, 10000.0f);
        const uint32_t ShadowMapDim = 4096;
        sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(1250, 1250), 1000.0f, 100000.0f);
        renderDevice->addLight(sun);

        Vector3f lightPos = Vector3f(-2000.0f, 2000.0f, -1000.0f);
        light->init(lightPos, -lightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 0.5f);
        renderDevice->addLight(light);
    }

    void initDebugQuad(ScreenQuad *quad) {
        vector<ShaderCode *> vsSources;
        vector<ShaderCode *> fsSources;
        string errorLog;

        SShaderManager *shaderManager = SShaderManager::instance();
        vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "330 core",
                                                            0, "", ""));
        fsSources.push_back(shaderManager->createShaderCode("Shader/DebugQuad.frag", "330 core",
                                                            0, "", ""));
        GLShader *quadShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();

        quad->init(0, 0, 1, 1, quadShader);
    }

    void updateCamera(Mouse *mouse, Keyboard *keyboard, VirtualCamera *camera) {
        if (nullptr == keyboard) return;

        const float movementSpeed = 4;

        float MovementScale = 0.1f;
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) || keyboard->keyPressed(Keyboard::KEY_RIGHT_SHIFT)) {
            MovementScale = 6.0f;
        }

        if (keyboard->keyPressed(Keyboard::KEY_W) || keyboard->keyPressed(Keyboard::KEY_UP))
            camera->forward(movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_S) || keyboard->keyPressed(Keyboard::KEY_DOWN))
            camera->forward(-movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_A) || keyboard->keyPressed(Keyboard::KEY_LEFT))
            camera->right(-movementSpeed * MovementScale);
        if (keyboard->keyPressed(Keyboard::KEY_D) || keyboard->keyPressed(Keyboard::KEY_RIGHT))
            camera->right(movementSpeed * MovementScale);
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

    void setWindUBO(unsigned int windUBO, Vector3f& windVec, float time) {
        glBindBuffer(GL_UNIFORM_BUFFER, windUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, windVec.data());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 3, sizeof(float), &time);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void updateGrass(InstanceActor &iGrassActor, TerrainMap &map, VirtualCamera &camera) {
        float oriSpacing = 1;
        float spacing = oriSpacing;
        int numRows = 50;
        int numColls = numRows;
        Vector2f mSize = map.getMapSize();
        Vector3f camPos = camera.position();
        Vector3f rowStep = camera.dir();
        rowStep.y() = 0;
        rowStep.normalize(); 
        //rowStep = AngleAxisf(GraphicsUtility::degToRad(0), Vector3f::UnitY()) * rowStep;
        Vector3f colStep = camera.right();
        colStep.y() = 0;
        colStep.normalize();
        colStep = AngleAxisf(GraphicsUtility::degToRad(90), Vector3f::UnitY()) * rowStep;
        iGrassActor.clearInstances();


        
        camPos.x() = ((int)(camPos.x() / spacing)) * spacing;
        camPos.z() = ((int)(camPos.z() / spacing)) * spacing;
        for (int i = 0; i < numRows; i++) {
            int maxJ = numColls * (float)i / numRows;
            Matrix4f S = GraphicsUtility::scaleMatrix(Vector3f(0.5, 0.5, 0.5));
            for (int j = -maxJ; j <= maxJ; j++) {
                Vector3f pos = camPos + rowStep * spacing * i + colStep * spacing * j;
                float xCord = ((int)(pos.x() / oriSpacing)) * oriSpacing;
                float zCord = ((int)(pos.z() / oriSpacing)) * oriSpacing;
                if (xCord < mSize.x() / 2 && xCord > mSize.x() / -2 && zCord < mSize.y() / 2 && zCord > mSize.y() / -2) {
                    iGrassActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                }
            }
        }
    }

    void TerrainSetup() {
        bool wireframe = false;
        bool debugTexture = false;
        bool shadows = true;
        bool richard = false;
        bool erode = false;
        bool cameraMode = false;

        if (richard) {
            DecoSetup();
            return;
        }

        GLWindow window;
        RenderDevice renderDevice;
        VirtualCamera camera;
        DirectionalLight sun, light;
        initCForge(&window, &renderDevice, &camera, &sun, &light);

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);

        ClipMap::ClipMapConfig clipMapConfig = {.sideLength = 128, .levelCount = 5};
        HeightMap::NoiseConfig noiseConfig = {.seed = static_cast<uint32_t>(rand()),
            .scale = 1.0f,
            .octaves = 10,
            .persistence = 0.5f,
            .lacunarity = 2.0f};
        HeightMap::HeightMapConfig heightMapConfig = {.width = 1024 / 1, .height = 1024 / 1,
                                                      .mapHeight = 400, .noiseConfig = noiseConfig};

        TerrainMap map = TerrainMap(&rootTransform);
        map.generateClipMap(clipMapConfig);
        map.generateHeightMap(heightMapConfig);

        SceneGraph sceneGraph;
        

        // Todo: richard hier fass
        // printf("%f, %f \n", map.getMapSize().x(), map.getMapSize().y());
        // map.getHeightAt(0, 0);
        bool generateNew = false;

        if (generateNew) {
            TreeGenerator::generateTrees(TreeGenerator::Needle, 1, "Assets/");
        }
        DekoMesh PineMesh;
        PineMesh.load("Assets/tree0.obj");
        PineMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Dark_Bark_baseColor.jpg";
        PineMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        PineMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        PineMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");
        InstanceActor iPineActor;

        DekoMesh PineLeavesMesh;
        PineLeavesMesh.load("Assets/leaves0.obj");
        PineLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/needle_leaves_color_bright.png";
        PineLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/needle_leaves_alpha.png";
        PineLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceFoliageShader.frag");
        InstanceActor iPineLeavesActor;

        int ammount = 32;
        for (int x = 0; x < ammount; x++) {
            for (int z = 0; z < ammount; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float) ammount / 4;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float) ammount / 4;

                if (map.getHeightAt(xCord, zCord) > 235 && map.getHeightAt(xCord, zCord) < 295) {
                    iPineActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)));
                    iPineLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)));
                }
                
            }
        }
        iPineActor.init(&PineMesh);
        iPineLeavesActor.init(&PineLeavesMesh);

        if (generateNew) {
            GrassGenerator::generateGrass(GrassType::triangle, 1, "Assets/");
        }
        DekoMesh GrassMesh;
        GrassMesh.load("Assets/grass0.obj");
        GrassMesh.getMaterial(0)->TexAlbedo = "Assets/richard/grass_color.jpg";
        GrassMesh.getMaterial(0)->TexDepth = "Assets/richard/grassAlpha.png";
        GrassMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceGrassShader.vert");
        GrassMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");
        InstanceActor iGrassActor;
        
        Matrix4f S = GraphicsUtility::scaleMatrix(Vector3f(0.5, 0.5, 0.5));
        ammount = 250;
        for (int x = 0; x < ammount; x++) {
            for (int z = 0; z < ammount; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount / 4;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount / 4;
                if (map.getHeightAt(xCord, zCord) > 235 && map.getHeightAt(xCord, zCord) < 295) {
                    iGrassActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                }
            }
        }
        iGrassActor.init(&GrassMesh);

        if (generateNew) {
            RockGenerator::generateRocks(RockGenerator::Normal, 1, "Assets/");
        }
        DekoMesh RockMesh;
        RockMesh.load("Assets/rock0.obj");
        RockMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
        RockMesh.getMaterial(0)->TexNormal = "Assets/richard/Rock_035_normal.jpg";
        RockMesh.getMaterial(0)->TexDepth = "Assets/richard/Rock_035_Packed.png";
        RockMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceShader.vert");
        RockMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");
        InstanceActor iRockActor;

        S = GraphicsUtility::scaleMatrix(Vector3f(1, 1, 1));
        ammount = 25;
        for (int x = 0; x < ammount; x++) {
            for (int z = 0; z < ammount; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount / 4;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount / 4;
                if (map.getHeightAt(xCord, zCord) > 235 && map.getHeightAt(xCord, zCord) < 295) {
                    iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                }
            }
        }
        iRockActor.init(&RockMesh);

        //wind
		Vector3f windVec = Vector3f(1, 0, 0);
		float windAngle = 0;
		float windStr = 1.5;
		float windAngleVariation = 0;
		float windAngleAcc = 100;
		unsigned int windUBO;
		glGenBuffers(1, &windUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, windUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 4, windUBO);
		setWindUBO(windUBO, windVec, 0);



        sceneGraph.init(&rootTransform);

        ScreenQuad quad;
        initDebugQuad(&quad);

        //fps counter
        int32_t FPSCount = 0;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 60;

        while (!window.shutdown()) {
            current_ticks = clock(); //for fps counter

            // wind
            windAngleVariation += randomF(-windAngleAcc, windAngleAcc) / (float)fps;
            windAngleVariation *= 0.8;
            windAngle += windAngleVariation / (float)fps;
            windVec.x() = cos(windAngle) * windStr;
            windVec.z() = sin(windAngle) * windStr;
            setWindUBO(windUBO, windVec, current_ticks / 60.0);

            window.update();

            sceneGraph.update(1.0f);

            if (erode) {
                map.erode();
            }

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
            } else {
                sceneGraph.render(&renderDevice);
            }

            //Deko instances
            iPineActor.render(&renderDevice);
            iRockActor.render(&renderDevice);

            updateGrass(iGrassActor, map, camera);
            iGrassActor.init(&GrassMesh);

            glDisable(GL_CULL_FACE);
            iGrassActor.render(&renderDevice);
            iPineLeavesActor.render(&renderDevice);
            glEnable(GL_CULL_FACE);

            renderDevice.activePass(RenderDevice::RENDERPASS_LIGHTING);

            renderDevice.activePass(RenderDevice::RENDERPASS_FORWARD);

            if (debugTexture) {
                glActiveTexture(GL_TEXTURE0);
                map.bindTexture();
                quad.render(&renderDevice);
            }

            window.swapBuffers();

            updateCamera(window.mouse(), window.keyboard(), renderDevice.activeCamera());

            if (cameraMode) {
                camera.position(Vector3f(camera.position().x(),
                                         map.getHeightAt(camera.position().x(), camera.position().z()) + 5,
                                         camera.position().z()));
            }

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
                    .octaves = 10,
                    .persistence = 0.5f,
                    .lacunarity = 2.0f};
                heightMapConfig = {.width = 1024 / 1, .height = 1024 / 1,
                    .mapHeight = 400, .noiseConfig = noiseConfig};
                map.generateHeightMap(heightMapConfig);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F5)) {
                window.keyboard()->keyState(Keyboard::KEY_F5, Keyboard::KEY_RELEASED);
                map.heightMapFromTexture(STextureManager::create("Assets/height_map1.jpg"), 10);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F6)) {
                window.keyboard()->keyState(Keyboard::KEY_F6, Keyboard::KEY_RELEASED);
                cameraMode = !cameraMode;
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F7)) {
                window.keyboard()->keyState(Keyboard::KEY_F7, Keyboard::KEY_RELEASED);
                erode = !erode;
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

            //FPS counter
            FPSCount++;
            delta_ticks = clock() - current_ticks;
            if (delta_ticks > 0)
                fps = CLOCKS_PER_SEC / delta_ticks;
            if (FPSCount % 60 == 0) {
                cout << fps << endl;
            }
        }
    }
}
