#pragma once

#include <glad/glad.h>
#include <CForge/Graphics/Lights/DirectionalLight.h>
#include <CForge/Graphics/GraphicsUtility.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>

#include "Terrain/src/Map/TerrainMap.h"
#include "Terrain/src/Map/HeightMap.h"

#include "./Decoration/DecoSetup.hpp"
#include "CForge/AssetIO/T3DMesh.hpp"
#include "Water/OceanSimulation.hpp"
#include "Water/OceanObject.hpp"
#include "Water/WaterManager.hpp"
#include "Water/Settings.hpp"


#include "../../Prototypes/Graphics/SkyboxActor.h"

using namespace CForge;

namespace Terrain {

    void initCForge(GLWindow *window, RenderDevice *renderDevice, VirtualCamera *camera, DirectionalLight *sun,
                    DirectionalLight *light, float nearPlane, float farPlane, float screenWidth, float screenHeight) {
        uint32_t winWidth = screenWidth;
        uint32_t winHeight = screenHeight;

       /* winWidth = 1200;
        winHeight = 1200;*/

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
        camera->projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(45.0f), nearPlane, farPlane);
        renderDevice->activeCamera(camera);

        Vector3f sunPos = Vector3f(400.0f, 400.0f, 400.0f);
        sun->init(sunPos, (-sunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 7.5f);
        auto projection = GraphicsUtility::perspectiveProjection(winWidth, winHeight, GraphicsUtility::degToRad(45.0f),  nearPlane, farPlane);
        const uint32_t ShadowMapDim = 512;
        sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(400, 400), nearPlane, farPlane);
        renderDevice->addLight(sun);

        Vector3f lightPos = Vector3f(-400.0f, 200.0f, -400.0f);
        light->init(lightPos, -lightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.0f);
        renderDevice->addLight(light);
    }

    void initDebugQuad(ScreenQuad *quad) {
        vector<ShaderCode *> vsSources;
        vector<ShaderCode *> fsSources;
        string errorLog;

        SShaderManager *shaderManager = SShaderManager::instance();
        vsSources.push_back(shaderManager->createShaderCode("Shader/ScreenQuad.vert", "420 core",
                                                            0, "", ""));
        fsSources.push_back(shaderManager->createShaderCode("Shader/WaterDebugQuad.frag", "420 core",
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
            MovementScale = 2.0f;
        }
        if (keyboard->keyPressed(Keyboard::KEY_Q)) {
            MovementScale = 0.02f;
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

    void setWindUBO(unsigned int windUBO, Vector3f windVec, float time) {
        glBindBuffer(GL_UNIFORM_BUFFER, windUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, windVec.data());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 3, sizeof(float), &time);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    float sign(Vector2f p1, Vector2f p2, Vector2f p3)
    {
        return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
    }

    bool PointInTriangle(Vector2f pt, Vector2f v1, Vector2f v2, Vector2f v3)
    {
        float d1, d2, d3;
        bool has_neg, has_pos;

        d1 = sign(pt, v1, v2);
        d2 = sign(pt, v2, v3);
        d3 = sign(pt, v3, v1);

        has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    void updateGrass(InstanceActor& iGrassActor, TerrainMap& map, VirtualCamera& camera) {
        iGrassActor.clearInstances();
        float sizeScale = 0.5;
        for (int i = 3; i > 0; i--) {
            float scale = exp2(i);
            float triangleHeight = 20 * scale * sizeScale;
            float triangleWidth = 30 * scale * sizeScale;
            float smallerTriangleHeight = (i == 1) ? 0.1 : triangleHeight / 2.0;
            float smallerTriangleWidth = (i == 1) ? 0.1 : triangleWidth / 2.0;

            float spacing = 1 * scale * sizeScale;

            Vector3f camDir = camera.dir();
            Vector3f camRig = camera.right();
            camDir.y() = 0;
            camRig.y() = 0;
            camDir.normalize();
            camRig.normalize();
            Vector3f A = camera.position();
            A -= camDir * 2;
            A.y() = 0;
            Vector3f B = A + camDir * triangleHeight + camRig * triangleWidth / 2.0;
            Vector3f C = A + camDir * triangleHeight + camRig * triangleWidth / -2.0;
            Vector3f D = A + camDir * smallerTriangleHeight + camRig * smallerTriangleWidth / 2.0;
            Vector3f E = A + camDir * smallerTriangleHeight + camRig * smallerTriangleWidth / -2.0;
            Vector2f minPos = Vector2f(min(min(A.x(), B.x()), C.x()), min(min(A.z(), B.z()), C.z()));
            Vector2f maxPos = Vector2f(max(max(A.x(), B.x()), C.x()), max(max(A.z(), B.z()), C.z()));
            Vector2f mSize = map.getMapSize();

            

            for (int z = minPos.y(); z < maxPos.y(); z += spacing) {
                for (int x = minPos.x(); x < maxPos.x(); x += spacing) {
                    if (PointInTriangle(Vector2f(x, z), Vector2f(A.x(), A.z()), Vector2f(B.x(), B.z()), Vector2f(C.x(), C.z())) &&
                        !PointInTriangle(Vector2f(x, z), Vector2f(A.x(), A.z()), Vector2f(D.x(), D.z()), Vector2f(E.x(), E.z()))) {
                        float xCord = ((int)(x / spacing)) * spacing;
                        float zCord = ((int)(z / spacing)) * spacing;

                        float distScale = max(sqrt((A - Vector3f(xCord, 0, zCord)).norm()), 3.0f) / 2 * sizeScale;
                        Matrix4f S = GraphicsUtility::scaleMatrix(Vector3f(distScale, distScale / 2, distScale));
                        if (xCord < mSize.x() / 2 && xCord > mSize.x() / -2 && zCord < mSize.y() / 2 && zCord > mSize.y() / -2) {
                            if (map.getHeightAt(xCord, zCord) > 210 && map.getHeightAt(xCord, zCord) < 255) {
                                iGrassActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                            }
                        }
                    }
                }
            }
        }
    }




    void placeDekoElements(TerrainMap &map, InstanceActor &iPineActor, InstanceActor &iPineLeavesActor, InstanceActor &iTreeActor, InstanceActor &iTreeLeavesActor, InstanceActor &iPalmActor, InstanceActor &iPalmLeavesActor, InstanceActor &iRockActor, InstanceActor& iBushActor, WaterManager& watermanager, vector<River>* rivers) {
        iPineActor.clearInstances();
        iPineLeavesActor.clearInstances();
        iTreeActor.clearInstances();
        iTreeLeavesActor.clearInstances();
        iPalmActor.clearInstances();
        iPalmLeavesActor.clearInstances();
        iRockActor.clearInstances();
        iBushActor.clearInstances();

        siv::PerlinNoise pNoise;
        float noiseOffset = randomF(-500, 500);
        float noiseScale = 1;

        float sizeScale = lowQuality ? 1.0 : 3;

        int ammount = lowQuality ? 30 : 500;
        Matrix4f S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale, sizeScale, sizeScale));
        Matrix4f R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY())));
        float randomSizeScale;
        for (int x = 2; x < ammount - 2; x++) {
            for (int z = 2; z < ammount - 2; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount;

               

                xCord += randomF(map.getMapSize().x() / (float)ammount / -2.0, map.getMapSize().x() / (float)ammount / 2.0);
                zCord += randomF(map.getMapSize().y() / (float)ammount / -2.0, map.getMapSize().y() / (float)ammount / 2.0);

                
                if (watermanager.validTreeLocation(zCord + map.getMapSize().y() / 2, xCord + map.getMapSize().x() / 2.0)) {

                    R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY())));
                    if (pNoise.accumulatedOctaveNoise3D(xCord * noiseScale + noiseOffset, 0, zCord * noiseScale, 1) < -0.1) {
                        if (map.getHeightAt(xCord, zCord) > 225 / (lowQuality ? 8 : 2) && map.getHeightAt(xCord, zCord) < 305 / (lowQuality ? 8 : 2)) {
                            randomSizeScale = randomF(0.25f, 1.5f);
                            S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                            iPineActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                            iPineLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                        }

                        else if (map.getHeightAt(xCord, zCord) > 202 / (lowQuality ? 8 : 2) && map.getHeightAt(xCord, zCord) < 208 / (lowQuality ? 8 : 2)) {
                            randomSizeScale = randomF(0.8f, 2.2f);
                            S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale, sizeScale * randomSizeScale, sizeScale));
                            iPalmActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord) - 0.3, zCord)) * R * S);
                            iPalmLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord) - 0.3, zCord)) * R * S);
                        }
                    }
                    else if (pNoise.accumulatedOctaveNoise3D(xCord * noiseScale + noiseOffset, 0, zCord * noiseScale, 1) > 0.1) {
                        if (map.getHeightAt(xCord, zCord) > 208 / (lowQuality ? 8 : 2) && map.getHeightAt(xCord, zCord) < 235 / (lowQuality ? 8 : 2)) {
                            randomSizeScale = randomF(1.25f, 1.75f);
                            S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                            iTreeActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                            iTreeLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                        }
                    }
                    else {
                        if (map.getHeightAt(xCord, zCord) > 208 / (lowQuality ? 8 : 2) && map.getHeightAt(xCord, zCord) < 255 / (lowQuality ? 8 : 2)) {
                            if (x % 5 == 0 && z % 5 == 0) {
                                randomSizeScale = randomF(0.2f, 0.8f);
                                S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                                iBushActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                            }
                        }
                    }
                }
            }
        }
        
        Vector2f rockScale = lowQuality ? Vector2f(0.4, 0.6) : Vector2f(0.6, 1.0);
        for (auto river : *rivers) {
            R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(90), Vector3f::UnitY()) * AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitX())));
            S = GraphicsUtility::scaleMatrix(Vector3f(river.getWidth(0) / 3.0, river.getWidth(0) / 3.0, river.getWidth(0) / 3.0));
            Vector3f pos = river.getPos(0);
            iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(pos.z(), pos.y(), pos.x())) * R * S);


            for (double t = 1; t < river.getLength(); t += randomF(1, 3)) {
                R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(90), Vector3f::UnitY()) * AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitX())));
                S = GraphicsUtility::scaleMatrix(Vector3f(randomF(rockScale.x(), rockScale.y()), randomF(rockScale.x(), rockScale.y()), randomF(rockScale.x(), rockScale.y())));
                pos = river.getPos(t) + river.getNormal(t) * river.getWidth(t) * randomF(-0.6, 0.6);
                iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(pos.z(), map.getHeightAt(pos.z(), pos.x()), pos.x())) * R * S);
            }
        }

        if (!lowQuality) {
            for (auto river : *rivers) {
                for (double t = 1; t < river.getLength(); t += randomF(0.1, 0.3)) {
                    R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(90), Vector3f::UnitY()) * AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitX())));
                    S = GraphicsUtility::scaleMatrix(Vector3f(randomF(0.1, 0.2), randomF(0.1, 0.2), randomF(0.1, 0.2)));
                    Vector3f pos = river.getPos(t) + river.getNormal(t) * river.getWidth(t) * randomF(-0.4, 0.4);
                    iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(pos.z(), map.getHeightAt(pos.z(), pos.x()), pos.x())) * R * S);
                }
            }
        }


        /*
        ammount /= 10;
        for (int x = 2; x < ammount - 2; x++) {
            for (int z = 2; z < ammount - 2; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount;

                xCord += randomF(map.getMapSize().x() / (float)ammount / -2.0, map.getMapSize().x() / (float)ammount / 2.0);
                zCord += randomF(map.getMapSize().y() / (float)ammount / -2.0, map.getMapSize().y() / (float)ammount / 2.0);

                int mapIndex = (int)(zCord + map.getMapSize().y() / 2) * map.getMapSize().x() + (int)(xCord + map.getMapSize().x() / 2.0);
                if (mapIndex >= map.getMapSize().x() * map.getMapSize().y()) mapIndex = 0;
                if (mapIndex < 0) mapIndex = 0;
                if (poolMap[mapIndex] == 0) {

                    if (map.getHeightAt(xCord, zCord) > 210 / (lowQuality ? 8 : 2) && map.getHeightAt(xCord, zCord) < 275 / (lowQuality ? 8 : 2)) {
                        R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY()) * AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitX())));
                        S = GraphicsUtility::scaleMatrix(Vector3f(randomF(0.5, 1), randomF(0.5, 1), randomF(0.5, 1)));
                        iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                    }
                }
            }
        }
        */
    }


    void loadNewDekoObjects(bool generateNew, DekoMesh &PineMesh, DekoMesh &PineLeavesMesh, DekoMesh &PalmMesh, DekoMesh &PalmLeavesMesh, DekoMesh &TreeMesh, DekoMesh &TreeLeavesMesh, DekoMesh &GrassMesh, DekoMesh &RockMesh, DekoMesh& BushMesh) {
        if (generateNew) {
            TreeGenerator::generateTrees(TreeGenerator::Needle, 1, "Assets/needleTree");
            TreeGenerator::generateTrees(TreeGenerator::Palm, 1, "Assets/palmTree");
            TreeGenerator::generateTrees(TreeGenerator::Normal, 1, "Assets/tree");
            GrassGenerator::generateGrass(GrassType::triangle, 1, "Assets/grass");
            GrassGenerator::generateGrass(GrassType::bush, 1, "Assets/bush");
            RockGenerator::generateRocks(RockGenerator::Normal, 1, "Assets/rock");
        }
        
        PineMesh.clear();
        PineMesh.load("Assets/needleTree0.obj");
        PineMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Dark_Bark_baseColor.jpg";
        PineMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        PineMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        PineMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

        PineLeavesMesh.clear();
        PineLeavesMesh.load("Assets/needleTreeLeaves0.obj");
        PineLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/needle_leaves_color_bright.png";
        PineLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/needle_leaves_alpha.png";
        PineLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        PalmMesh.clear();
        PalmMesh.load("Assets/palmTree0.obj");
        PalmMesh.getMaterial(0)->TexAlbedo = "Assets/richard/palm_color.jpg";
        PalmMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        PalmMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        PalmMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PalmMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

        PalmLeavesMesh.clear();
        PalmLeavesMesh.load("Assets/palmTreeLeaves0.obj");
        PalmLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/palmLeave_color.png";
        PalmLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/palmLeave_alpha.png";
        PalmLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PalmLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        TreeMesh.clear();
        TreeMesh.load("Assets/tree0.obj");
        TreeMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Bark_06_baseColor.jpg";
        TreeMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        TreeMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        TreeMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        TreeMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

        TreeLeavesMesh.clear();
        TreeLeavesMesh.load("Assets/treeLeaves0.obj");
        TreeLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/leaves3_color.png";
        TreeLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/leaves3_alpha.jpg";
        TreeLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        TreeLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        BushMesh.clear();
        BushMesh.load("Assets/bush0.obj");
        BushMesh.getMaterial(0)->TexAlbedo = "Assets/richard/bushColor2.png";
        BushMesh.getMaterial(0)->TexDepth = "Assets/richard/bushAlpha2.png";
        BushMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceGrassShader.vert");
        BushMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        GrassMesh.clear();
        GrassMesh.load("Assets/grass0.obj");
        GrassMesh.getMaterial(0)->TexAlbedo = "Assets/richard/grass_color.jpg";
        GrassMesh.getMaterial(0)->TexDepth = "Assets/richard/grassAlpha.png";
        GrassMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceGrassShader.vert");
        GrassMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        RockMesh.clear();
        RockMesh.load("Assets/rock0.obj");
        RockMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
        RockMesh.getMaterial(0)->TexNormal = "Assets/richard/Rock_035_normal.jpg";
        RockMesh.getMaterial(0)->TexDepth = "Assets/richard/Rock_035_Packed.png";
        RockMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceShader.vert");
        RockMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceRockShader.frag");
    }

    void TerrainSetup() {
        bool wireframe = false;
        bool debugTexture = false;
        bool shadows = false;
        bool richard = false;
        bool erode = false;
        bool cameraMode = false;
        bool generateNew = false;
        bool renderGrass = false;
        bool renderOcean = true;
        bool oceanOnly = false;

        float cameraHeight = 2;

        if (richard) {
            DecoSetup();
            return;
        }

        GLWindow window;
        RenderDevice renderDevice;
        VirtualCamera camera;
        DirectionalLight sun, light;
        float nearPlane = 1.0f;
        float farPlane = lowQuality ? 1000.0f : 10000.0f;
        float screenWidth = 5120 / 4;
        float screenHeight = 1440 / 2;
        initCForge(&window, &renderDevice, &camera, &sun, &light, nearPlane, farPlane, screenWidth, screenHeight);

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);

        ClipMap::ClipMapConfig clipMapConfig = {.sideLength = 128, .levelCount = 5};
        HeightMap::NoiseConfig noiseConfig = { .seed = 7863,//11797,// static_cast<uint32_t>(rand()),
            .scale = 1.0f,
            .octaves = 10,
            .persistence = 0.5f,
            .lacunarity = 2.0f};
        HeightMap::HeightMapConfig heightMapConfig = {.width = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)), .height = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)),
                                                      .mapHeight = (float)(lowQuality ? 50 : 200), .noiseConfig = noiseConfig};


        TerrainMap map = TerrainMap(&rootTransform);
        map.generateClipMap(clipMapConfig);    
        map.generateHeightMap(heightMapConfig);

        SceneGraph sceneGraph;

        OceanSimulation oceanSimulation(lowQuality ? 64 : 256, lowQuality ? 500 : 500, lowQuality ? 0.1 : 40, Vector2f(1.0f, 1.0f), lowQuality ? 1000 : 50);
        OceanObject oceanObject(lowQuality ? 16 : 256, lowQuality ? 25 : 100, lowQuality ? 1 : 2, 1, 1, 1, lowQuality ? 29 : 7, nearPlane, farPlane);
        
        bool pause = false;
        oceanSimulation.initOceanSimulation();
        oceanObject.init(screenWidth, screenHeight);
        unsigned int selectedTexture = oceanSimulation.mTextures[OceanSimulation::DISPLACEMENT];
        
        WaterManager waterManager(map.getHeightMap());
        waterManager.updateTextures();

        waterManager.updateShoreDistTexture();

        oceanObject.generateWaterGeometry(Vector2i(heightMapConfig.width, heightMapConfig.height),
            heightMapConfig.mapHeight, 1,
            waterManager.getHeightMap(), waterManager.getPoolMap(), waterManager.getRivers());














        DekoMesh PineMesh;
        InstanceActor iPineActor;
        DekoMesh PineLeavesMesh;
        InstanceActor iPineLeavesActor;
        DekoMesh PalmMesh;
        InstanceActor iPalmActor;
        DekoMesh PalmLeavesMesh;
        InstanceActor iPalmLeavesActor;
        DekoMesh TreeMesh;
        InstanceActor iTreeActor;
        DekoMesh TreeLeavesMesh;
        InstanceActor iTreeLeavesActor;
        DekoMesh GrassMesh;
        InstanceActor iGrassActor;
        DekoMesh BushMesh;
        InstanceActor iBushActor;
        DekoMesh RockMesh;
        InstanceActor iRockActor;

        loadNewDekoObjects(generateNew, PineMesh, PineLeavesMesh, PalmMesh, PalmLeavesMesh, TreeMesh, TreeLeavesMesh, GrassMesh, RockMesh, BushMesh);

        placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor, waterManager, waterManager.getRivers());
        iPineActor.init(&PineMesh);
        iPineLeavesActor.init(&PineLeavesMesh);
        iTreeActor.init(&TreeMesh);
        iTreeLeavesActor.init(&TreeLeavesMesh);
        iPalmActor.init(&PalmMesh);
        iPalmLeavesActor.init(&PalmLeavesMesh);
        iRockActor.init(&RockMesh);
        iGrassActor.init(&GrassMesh);
        iBushActor.init(&BushMesh);
        
        //wind
		Vector3f windVec = Vector3f(1, 0, 0);
		float windAngle = 0;
		float windStr = 1.0;
        float windWaveSpeedMultiplier = 0.25;
		float windAngleVariation = 0;
		float windAngleAcc = 1;
		unsigned int windUBO;
		glGenBuffers(1, &windUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, windUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 4, windUBO);
		setWindUBO(windUBO, Vector3f(1, 0, 0), 0);
        
        srand((unsigned int)time(NULL));
        
        sceneGraph.init(&rootTransform);

        ScreenQuad quad;
        initDebugQuad(&quad);

        // initialize skybox
        SkyboxActor Skybox;
        std::vector<string> BoxTexs;
        BoxTexs.push_back("Assets/skybox/right.jpg");
        BoxTexs.push_back("Assets/skybox/left.jpg");
        BoxTexs.push_back("Assets/skybox/top.jpg");
        BoxTexs.push_back("Assets/skybox/bottom.jpg");
        BoxTexs.push_back("Assets/skybox/back.jpg");
        BoxTexs.push_back("Assets/skybox/front.jpg");
        Skybox.init(BoxTexs[0], BoxTexs[1], BoxTexs[2], BoxTexs[3], BoxTexs[4], BoxTexs[5]);

        //fps counter
        int32_t FPSCount = 0;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 60;

        uint64_t LastFPS = CoreUtility::timestamp();

        uint32_t ScreenshotNumber = 0;

        while (!window.shutdown()) {
            current_ticks = clock(); //for fps counter

            if (window.keyboard()->keyPressed(Keyboard::KEY_P)) {
                window.keyboard()->keyState(Keyboard::KEY_P, Keyboard::KEY_RELEASED);
                shadows = !shadows;
            }

            
            // wind

            windAngleVariation += randomF(-windAngleAcc, windAngleAcc) / (float)fps;
            windAngleVariation *= 0.8;
            windAngle += windAngleVariation / (float)fps;
            windVec.x() = cos(windAngle) * windStr;
            windVec.z() = sin(windAngle) * windStr;
            setWindUBO(windUBO, Vector3f(1, 0, 0), current_ticks / (float)CLOCKS_PER_SEC * windWaveSpeedMultiplier);
            


            window.update();

            sceneGraph.update(1.0f);

            if (erode) {
                map.erode();
                waterManager.refreshMaps();
            }

            map.update(camera.position().x(), camera.position().z());

            if (shadows) {
                renderDevice.activePass(RenderDevice::RENDERPASS_SHADOW, &sun);
                sceneGraph.render(&renderDevice);
            }

            renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);

            if (!oceanOnly) {

                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    sceneGraph.render(&renderDevice);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
                else {
                    sceneGraph.render(&renderDevice);
                }

            }



            
            if (!oceanOnly) {

                //Deko instances
                iPineActor.render(&renderDevice);
                iTreeActor.render(&renderDevice);
                iPalmActor.render(&renderDevice);
                iRockActor.render(&renderDevice);

                glDisable(GL_CULL_FACE);
                if (renderGrass) {
                    updateGrass(iGrassActor, map, camera);
                    iGrassActor.init(&GrassMesh);
                    iGrassActor.render(&renderDevice);
                }
                iBushActor.render(&renderDevice);
                iPineLeavesActor.render(&renderDevice);
                iTreeLeavesActor.render(&renderDevice);
                iPalmLeavesActor.render(&renderDevice);
                glEnable(GL_CULL_FACE);

            }
            
            renderDevice.activePass(RenderDevice::RENDERPASS_LIGHTING);

            renderDevice.activePass(RenderDevice::RENDERPASS_FORWARD);
            renderDevice.requestRendering(&Skybox, Quaternionf::Identity(), Vector3f::Zero(), Vector3f::Ones());

         

            if (renderOcean) {
                renderDevice.gBuffer()->blitDepthBuffer(screenWidth, screenHeight);
                oceanObject.copyFBO(renderDevice.gBuffer()->retrieveFrameBuffer(), screenWidth, screenHeight);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }


                oceanSimulation.updateWaterSimulation(current_ticks / (float)CLOCKS_PER_SEC / 5 * (pause ? 0 : 1));
                //glDisable(GL_CULL_FACE);



                glBindImageTexture(0, oceanSimulation.mTextures[oceanSimulation.DISPLACEMENT], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, oceanSimulation.mTextures[oceanSimulation.NORMALS], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                oceanObject.renderOcean(&renderDevice, current_ticks / (float)CLOCKS_PER_SEC * (pause ? 0 : 1), waterManager.getHeightMapTexture(), waterManager.getShoreDistanceTexture());
                

                glBindImageTexture(0, oceanSimulation.mTextures[oceanSimulation.DISPLACEMENT], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                glBindImageTexture(1, oceanSimulation.mTextures[oceanSimulation.NORMALS], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                oceanObject.renderPool(&renderDevice, current_ticks / (float)CLOCKS_PER_SEC * (pause ? 0 : 1));
                
                if (!pause) {
                    glBindImageTexture(0, oceanSimulation.mTextures[oceanSimulation.DISPLACEMENT], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                    glBindImageTexture(1, oceanSimulation.mTextures[oceanSimulation.NORMALS], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
                    oceanObject.renderStreams(&renderDevice, current_ticks / (float)CLOCKS_PER_SEC * (pause ? 0 : 1));
                }

                glEnable(GL_CULL_FACE);



                if (wireframe) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                glDisable(GL_BLEND);
            }

           




            if (debugTexture) {
                glActiveTexture(GL_TEXTURE0);
                map.bindTexture();
                glBindTexture(GL_TEXTURE_2D, selectedTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                quad.render(&renderDevice);
            }

            window.swapBuffers();

            updateCamera(window.mouse(), window.keyboard(), renderDevice.activeCamera());

            if (cameraMode) {
                camera.position(Vector3f(camera.position().x(),
                                         map.getHeightAt(camera.position().x(), camera.position().z()) + cameraHeight,
                                         camera.position().z()));
            }

            if (window.keyboard()->keyPressed(Keyboard::KEY_1)) {
                window.keyboard()->keyState(Keyboard::KEY_1, Keyboard::KEY_RELEASED);
                
                selectedTexture = waterManager.getHeightMapTexture();
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_2)) {
                window.keyboard()->keyState(Keyboard::KEY_2, Keyboard::KEY_RELEASED);
                
                selectedTexture = waterManager.getPoolMapTexture();
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_3)) {
                window.keyboard()->keyState(Keyboard::KEY_3, Keyboard::KEY_RELEASED);
                
                selectedTexture = waterManager.getShoreDistanceTexture();
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_4)) {
                window.keyboard()->keyState(Keyboard::KEY_4, Keyboard::KEY_RELEASED);
                selectedTexture = oceanSimulation.mTextures[OceanSimulation::H0];
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_5)) {
                window.keyboard()->keyState(Keyboard::KEY_5, Keyboard::KEY_RELEASED);
                selectedTexture = oceanSimulation.mTextures[OceanSimulation::DISPLACEMENT];
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_6)) {
                window.keyboard()->keyState(Keyboard::KEY_6, Keyboard::KEY_RELEASED);
                selectedTexture = oceanSimulation.mTextures[OceanSimulation::NORMALS];
            }

            if (window.keyboard()->keyPressed(Keyboard::KEY_P)) {
                window.keyboard()->keyState(Keyboard::KEY_P, Keyboard::KEY_RELEASED);
                pause = !pause;
                
            }


            if (window.keyboard()->keyPressed(Keyboard::KEY_L)) {
                window.keyboard()->keyState(Keyboard::KEY_L, Keyboard::KEY_RELEASED);

                //waterManager.trySpawnLake((int)(camera.position().z() + map.getMapSize().y() / 2)* map.getMapSize().x() + (int)(camera.position().x() + map.getMapSize().x() / 2.0));
                waterManager.trySpawnLakes(lowQuality ? 75 : 200);

                waterManager.updateTextures();
                oceanObject.generateWaterGeometry(Vector2i(heightMapConfig.width, heightMapConfig.height),
                    heightMapConfig.mapHeight, 1,
                    waterManager.getHeightMap(), waterManager.getPoolMap(), waterManager.getRivers());

            }

            if (window.keyboard()->keyPressed(Keyboard::KEY_K)) {
                window.keyboard()->keyState(Keyboard::KEY_K, Keyboard::KEY_RELEASED);
                waterManager.tryGenerateRivers(lowQuality ? 200 : 300);
                waterManager.updateTextures();
                oceanObject.generateWaterGeometry(Vector2i(heightMapConfig.width, heightMapConfig.height),
                    heightMapConfig.mapHeight, 1,
                    waterManager.getHeightMap(), waterManager.getPoolMap(), waterManager.getRivers());
            }


            if (window.keyboard()->keyPressed(Keyboard::KEY_G)) {
                window.keyboard()->keyState(Keyboard::KEY_G, Keyboard::KEY_RELEASED);

                noiseConfig = { .seed = static_cast<uint32_t>(rand()),
                    .scale = 1.0f,
                    .octaves = 10,
                    .persistence = 0.5f,
                    .lacunarity = 2.0f };
                HeightMap::HeightMapConfig heightMapConfig = { .width = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)), .height = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)),
                                                      .mapHeight = (float)(lowQuality ? 50 : 200), .noiseConfig = noiseConfig };
                map.generateHeightMap(heightMapConfig);

                waterManager.refreshMaps();
                waterManager.updateTextures();
      
                waterManager.trySpawnLakes(lowQuality ? 75 : 200);
                waterManager.updateTextures();
                waterManager.tryGenerateRivers(lowQuality ? 200 : 300);
                waterManager.updateTextures();
                waterManager.updateShoreDistTexture();

                oceanObject.generateWaterGeometry(Vector2i(heightMapConfig.width, heightMapConfig.height),
                    heightMapConfig.mapHeight, 1,
                    waterManager.getHeightMap(), waterManager.getPoolMap(), waterManager.getRivers());

                map.updateHeights();
                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor, waterManager, waterManager.getRivers());
                iPineActor.init(&PineMesh);
                iPineLeavesActor.init(&PineLeavesMesh);
                iTreeActor.init(&TreeMesh);
                iTreeLeavesActor.init(&TreeLeavesMesh);
                iPalmActor.init(&PalmMesh);
                iPalmLeavesActor.init(&PalmLeavesMesh);
                iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);

                cout << "\n\n" << "Seed: " << noiseConfig.seed << "\n\n";
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
                HeightMap::HeightMapConfig heightMapConfig = { .width = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)), .height = (uint32_t)(1024 * (lowQuality ? 0.5f : 2)),
                                                      .mapHeight = (float)(lowQuality ? 50 : 200), .noiseConfig = noiseConfig };
                map.generateHeightMap(heightMapConfig);
                
                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor, waterManager, waterManager.getRivers());
                iPineActor.init(&PineMesh);
                iPineLeavesActor.init(&PineLeavesMesh);
                iTreeActor.init(&TreeMesh);
                iTreeLeavesActor.init(&TreeLeavesMesh);
                iPalmActor.init(&PalmMesh);
                iPalmLeavesActor.init(&PalmLeavesMesh);
                iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);

                cout << "\n\n" << noiseConfig.seed << "\n\n";

                waterManager.refreshMaps();
                waterManager.updateTextures();
                waterManager.updateShoreDistTexture();
                oceanObject.generateWaterGeometry(Vector2i(heightMapConfig.width, heightMapConfig.height),
                    heightMapConfig.mapHeight, 1,
                    waterManager.getHeightMap(), waterManager.getPoolMap(), waterManager.getRivers());
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F5)) {
                window.keyboard()->keyState(Keyboard::KEY_F5, Keyboard::KEY_RELEASED);
                map.updateHeights();
                
                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor, waterManager, waterManager.getRivers());
                iPineActor.init(&PineMesh);
                iPineLeavesActor.init(&PineLeavesMesh);
                iTreeActor.init(&TreeMesh);
                iTreeLeavesActor.init(&TreeLeavesMesh);
                iPalmActor.init(&PalmMesh);
                iPalmLeavesActor.init(&PalmLeavesMesh);
                iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);
                
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F6)) {
                window.keyboard()->keyState(Keyboard::KEY_F6, Keyboard::KEY_RELEASED);
                cameraMode = !cameraMode;
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F7)) {
                window.keyboard()->keyState(Keyboard::KEY_F7, Keyboard::KEY_RELEASED);
                erode = !erode;
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F8)) {
                window.keyboard()->keyState(Keyboard::KEY_F8, Keyboard::KEY_RELEASED);
                
                iPineActor.clearInstances();
                iPineLeavesActor.clearInstances();
                iTreeActor.clearInstances();
                iTreeLeavesActor.clearInstances();
                iPalmActor.clearInstances();
                iPalmLeavesActor.clearInstances();
                iRockActor.clearInstances();
                iBushActor.clearInstances();
                iPineActor.init(&PineMesh);
                iPineLeavesActor.init(&PineLeavesMesh);
                iTreeActor.init(&TreeMesh);
                iTreeLeavesActor.init(&TreeLeavesMesh);
                iPalmActor.init(&PalmMesh);
                iPalmLeavesActor.init(&PalmLeavesMesh);
                iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);
                
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F9)) {
                window.keyboard()->keyState(Keyboard::KEY_F9, Keyboard::KEY_RELEASED);
                renderGrass = !renderGrass;
            }
            if (false && window.keyboard()->keyPressed(Keyboard::KEY_F10)) {
                window.keyboard()->keyState(Keyboard::KEY_F10, Keyboard::KEY_RELEASED);
                
                loadNewDekoObjects(generateNew, PineMesh, PineLeavesMesh, PalmMesh, PalmLeavesMesh, TreeMesh, TreeLeavesMesh, GrassMesh, RockMesh, BushMesh);

                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor, waterManager, waterManager.getRivers());
                iPineActor.init(&PineMesh);
                iPineLeavesActor.init(&PineLeavesMesh);
                iTreeActor.init(&TreeMesh);
                iTreeLeavesActor.init(&TreeLeavesMesh);
                iPalmActor.init(&PalmMesh);
                iPalmLeavesActor.init(&PalmLeavesMesh);
                iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);
                
            }

            if (window.keyboard()->keyPressed(Keyboard::KEY_O)) {
                window.keyboard()->keyState(Keyboard::KEY_O, Keyboard::KEY_RELEASED);

                T2DImage<uint8_t>  Img;
                renderDevice.gBuffer()->retrievePositionBuffer(&Img);
                SAssetIO::store("../PositionBuffer.jpg", &Img);
                renderDevice.gBuffer()->retrieveNormalBuffer(&Img);
                SAssetIO::store("../NormalBuffer.jpg", &Img);
                renderDevice.gBuffer()->retrieveAlbedoBuffer(&Img);
                SAssetIO::store("../AlbedoBuffer.jpg", &Img);
            }

            if (window.keyboard()->keyPressed(Keyboard::KEY_P)) {
                window.keyboard()->keyState(Keyboard::KEY_P, Keyboard::KEY_RELEASED);

                T2DImage<uint8_t> ColorBufferImg;
                GraphicsUtility::retrieveFrameBuffer(&ColorBufferImg, nullptr);
                AssetIO::store("Screenshot_" + std::to_string(ScreenshotNumber++) + ".jpg", &ColorBufferImg);
            }

            //FPS counter
            FPSCount++;

            if (CoreUtility::timestamp() - LastFPS > 1000) {
                fps = FPSCount;
                FPSCount = 0;
                LastFPS = CoreUtility::timestamp();
                printf("FPS: %d\n", fps);
            }

           delta_ticks = clock() - current_ticks;
           
            if (delta_ticks > 0)
                fps = CLOCKS_PER_SEC / delta_ticks;
            fps > 10 ? fps : 10;
            if (FPSCount % 60 == 0) {
                cout << fps << endl;
            }
        }
    }
}

