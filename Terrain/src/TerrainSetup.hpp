#pragma once

#include <CForge/Graphics/OpenGLHeader.h>

#include <CForge/Graphics/Lights/DirectionalLight.h>
#include <CForge/Graphics/Lights/PointLight.h>
#include <CForge/Graphics/GraphicsUtility.h>
#include <CForge/Graphics/SceneGraph/SceneGraph.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Graphics/STextureManager.h>

#include "Terrain/src/Map/TerrainMap.h"
#include "./Decoration/DecoSetup.hpp"

#include <CForge/GUI/GUI.h>
#include <CForge/GUI/Widgets/Form.h>
#include <wchar.h>

//#include "../../Prototypes/Graphics/SkyboxActor.h"
#include "../../CForge/Graphics/Actors/SkyboxActor.h"
#include <GLFW/glfw3.h>
//#include "../../CForge/Graphics/GLWindow.h"

#include "PPScreenQuad.h"
#include "../../Prototypes/SLOD.h"
#include "../../Prototypes/Actor/LODActor.h"

using namespace CForge;
float CAM_FOV = 90.0;
int WINWIDTH = 1920;
int WINHEIGHT = 1080;
#define FULLSCREEN false
#define LOD false
float cameraPanSpeed = 1.0;

namespace Terrain {
class TerrainSetup : public ITListener<GUICallbackObject> {
public:

    void initCForge(GLWindow *window, RenderDevice *renderDevice, VirtualCamera *camera, DirectionalLight *sun,
                    PointLight *light) {
		
		if (FULLSCREEN) {

			const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			WINWIDTH = mode->width;
			WINHEIGHT = mode->height;
		}
        uint32_t winWidth = WINWIDTH;
        uint32_t winHeight = WINHEIGHT;

//         glfwWindowHint(GLFW_SAMPLES, 4);

		if (FULLSCREEN)
			window->init(Vector2i(200, 200), Vector2i(winWidth, winHeight), "Terrain Setup", 0, 0, true);
		else
			window->init(Vector2i(200, 200), Vector2i(winWidth, winHeight), "Terrain Setup", 0, 0, false);
        #ifndef __EMSCRIPTEN__
        gladLoadGL();
        #endif

		//glfwSwapInterval(0);

        string GLError;
        GraphicsUtility::checkGLError(&GLError);
        if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

        //RenderDevice::RenderDeviceConfig renderConfig;
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
        lightConfig.ShadowBias = 0.0003f;
        lightConfig.ShadowMapCount = 1;
        lightConfig.SpotLightCount = 0;
        SShaderManager *shaderManager = SShaderManager::instance();
        shaderManager->configShader(lightConfig);
        shaderManager->release();

        camera->init(Vector3f(.0f, 400.0f, 100.0f), Vector3f::UnitY());
        camera->pitch(GraphicsUtility::degToRad(-15.0f));
        camera->projectionMatrix(winWidth, winHeight, GraphicsUtility::degToRad(CAM_FOV), 0.1f, 5000.0f);
        renderDevice->activeCamera(camera);

        Vector3f sunPos = Vector3f(-200.0f, 400.0f, 50.0f);
		Vector3f sunDir = Vector3f(-1.0,1.0,1.0);
		sunDir.normalize();
		//Vector3f sunPos = Vector3f(-5.0f, 15.0f, 35.0f);
		sun->init(sunDir*450.0, -sunDir, Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		const uint32_t ShadowMapDim = 2*8192;//4096; //2048;
		sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(1000, 1000), -500.0f, 800.0f);
		//sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(500, 500), -100.0f, 700.0f);
        renderDevice->addLight(sun);

        //Vector3f lightPos = Vector3f(-400.0f, 200.0f, -400.0f);
        //light->init(sunPos, -sunDir.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Vector3f(0.0f, 0.0f, 0.0f));
        //renderDevice->addLight(light);
    }



    void initDebugQuad(ScreenQuad *quad, const std::string vertSource, const std::string fragSource) {
        vector<ShaderCode *> vsSources;
        vector<ShaderCode *> fsSources;
        string errorLog;

        SShaderManager *shaderManager = SShaderManager::instance();
        vsSources.push_back(shaderManager->createShaderCode(vertSource, "420 core",
                                                            0, ""));
        fsSources.push_back(shaderManager->createShaderCode(fragSource, "420 core",
                                                            0, ""));
        GLShader *quadShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);
        shaderManager->release();

        quad->init(0, 0, 1, 1, quadShader);
    }

    void updateCamera(Mouse *mouse, Keyboard *keyboard, VirtualCamera *camera, float deltaTime) {
        if (nullptr == keyboard) return;

        const float movementSpeed = 4;

        float MovementScale = 0.1f*60.0/(1.0/deltaTime);
        if (keyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) || keyboard->keyPressed(Keyboard::KEY_RIGHT_SHIFT)) {
            MovementScale *= 3.0;
        }
        if (keyboard->keyPressed(Keyboard::KEY_Q)) {
            MovementScale *= 0.2;
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

            camera->rotY(GraphicsUtility::degToRad(MouseDelta.x()) * (-cameraPanSpeed/10.0f) * movementSpeed);
            camera->pitch(GraphicsUtility::degToRad(MouseDelta.y()) * (-cameraPanSpeed/10.0f) * movementSpeed);

            mouse->movement(Eigen::Vector2f::Zero());
        }
    }

    void setWindUBO(unsigned int windUBO, Vector3f& windVec, float time) {
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

    void updateGrass(LODActor& iGrassActor, TerrainMap& map, VirtualCamera& camera) {
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
            Vector3f camRig = camera.right()*std::fmin(std::fmax(std::tan(GraphicsUtility::degToRad(CAM_FOV)*0.5),0.0),10.0)*3.0;
            camDir.y() = 0;
            camRig.y() = 0;
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


    void placeDekoElements(TerrainMap &map, LODActor &iPineActor, LODActor &iPineLeavesActor, LODActor &iTreeActor, LODActor &iTreeLeavesActor, LODActor &iPalmActor, LODActor &iPalmLeavesActor, LODActor &iRockActor, LODActor& iBushActor) {
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
        float noiseScale = 0.01;


        float sizeScale = 1;

        int ammount = 200;
        Matrix4f S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale, sizeScale, sizeScale));
        Matrix4f R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY())));
        float randomSizeScale;
        for (int x = 0; x < ammount; x++) {
            for (int z = 0; z < ammount; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount * 0.9;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount * 0.9;

               

                xCord += randomF(map.getMapSize().x() / (float)ammount / -2.0, map.getMapSize().x() / (float)ammount / 2.0);
                zCord += randomF(map.getMapSize().y() / (float)ammount / -2.0, map.getMapSize().y() / (float)ammount / 2.0);

                R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY())));
                if (pNoise.accumulatedOctaveNoise3D(xCord * noiseScale + noiseOffset, 0, zCord * noiseScale, 1) < -0.1) {
                    if (map.getHeightAt(xCord, zCord) > 225 && map.getHeightAt(xCord, zCord) < 255) {
                        randomSizeScale = randomF(0.25f, 1.5f);
                        S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                        iPineActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                        iPineLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * S);
                    }

                    else if (map.getHeightAt(xCord, zCord) > 202 && map.getHeightAt(xCord, zCord) < 208) {
                        randomSizeScale = randomF(0.8f, 2.2f);
                        S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale, sizeScale * randomSizeScale, sizeScale));
                        iPalmActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord) - 0.3, zCord)) * R * S);
                        iPalmLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord) - 0.3, zCord)) * R * S);
                    }
                }
                else if (pNoise.accumulatedOctaveNoise3D(xCord * noiseScale + noiseOffset, 0, zCord * noiseScale, 1) > 0.1) {
                    if (map.getHeightAt(xCord, zCord) > 208 && map.getHeightAt(xCord, zCord) < 235) {
                        randomSizeScale = randomF(1.25f, 1.75f);
                        S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                        iTreeActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                        iTreeLeavesActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                    }
                }
                else {
                    if (false && map.getHeightAt(xCord, zCord) > 208 && map.getHeightAt(xCord, zCord) < 255) {
                        if (x % 5 == 0 && z % 5 == 0) {
                            randomSizeScale = randomF(0.8f, 1.25f);
                            S = GraphicsUtility::scaleMatrix(Vector3f(sizeScale * randomSizeScale, sizeScale * randomSizeScale, sizeScale * randomSizeScale));
                            iBushActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                        }
                    }
                }
            }
        }


        
        ammount /= 10;
        for (int x = 0; x < ammount; x++) {
            for (int z = 0; z < ammount; z++) {
                float xCord = (x - ammount / 2.0 + 0.5) * map.getMapSize().x() / (float)ammount * 0.9;
                float zCord = (z - ammount / 2.0 + 0.5) * map.getMapSize().y() / (float)ammount * 0.9;

                xCord += randomF(map.getMapSize().x() / (float)ammount / -2.0, map.getMapSize().x() / (float)ammount / 2.0);
                zCord += randomF(map.getMapSize().y() / (float)ammount / -2.0, map.getMapSize().y() / (float)ammount / 2.0);
                if (map.getHeightAt(xCord, zCord) > 210 && map.getHeightAt(xCord, zCord) < 275) {
                    R = GraphicsUtility::rotationMatrix(static_cast<Quaternionf>(AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitY()) * AngleAxisf(GraphicsUtility::degToRad(randomF(0, 360)), Vector3f::UnitX())));
                    S = GraphicsUtility::scaleMatrix(Vector3f(randomF(0.5, 1), randomF(0.5, 1), randomF(0.5, 1)));
                    iRockActor.addInstance(GraphicsUtility::translationMatrix(Vector3f(xCord, map.getHeightAt(xCord, zCord), zCord)) * R * S);
                }
            }
        }
    }

    void loadNewDekoObjects(bool generateNew, DekoMesh &PineMesh, DekoMesh &PineLeavesMesh, DekoMesh &PalmMesh, DekoMesh &PalmLeavesMesh, DekoMesh &TreeMesh, DekoMesh &TreeLeavesMesh, DekoMesh &GrassMesh, DekoMesh &RockMesh, DekoMesh& BushMesh) {
        if (generateNew) {
            TreeGenerator::generateTrees(TreeGenerator::Needle, 1, "Assets/needleTree");
            TreeGenerator::generateTrees(TreeGenerator::Palm, 1, "Assets/palmTree");
            TreeGenerator::generateTrees(TreeGenerator::Normal, 1, "Assets/tree");
            GrassGenerator::generateGrass(GrassType::triangle, 1, "Assets/grass");
            GrassGenerator::generateGrass(GrassType::cross, 1, "Assets/bush");
            RockGenerator::generateRocks(RockGenerator::Normal, 1, "Assets/rock");
        }
        
        PineMesh.clear();
        PineMesh.load("Assets/needleTree0.obj");
        PineMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Dark_Bark_basecolor.jpg";
        PineMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        PineMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        /*PineMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");*/

        PineLeavesMesh.clear();
        PineLeavesMesh.load("Assets/needleTreeLeaves0.obj");
        PineLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/needle_leaves_color_bright.png";
        PineLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/needle_leaves_alpha.png";
       /* PineLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        PineLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");*/

        PalmMesh.clear();
        PalmMesh.load("Assets/palmTree0.obj");
        PalmMesh.getMaterial(0)->TexAlbedo = "Assets/richard/palm_color.jpg";
        PalmMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        PalmMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        //PalmMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        //PalmMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

        PalmLeavesMesh.clear();
        PalmLeavesMesh.load("Assets/palmTreeLeaves0.obj");
        PalmLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/palmLeave_color.png";
        PalmLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/palmLeave_alpha.png";
        //PalmLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        //PalmLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        TreeMesh.clear();
        TreeMesh.load("Assets/tree0.obj");
        TreeMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Bark_06_basecolor.jpg";
        TreeMesh.getMaterial(0)->TexNormal = "Assets/richard/Bark_06_normal.jpg";
        TreeMesh.getMaterial(0)->TexDepth = "Assets/richard/Bark_06_Packed.png";
        //TreeMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        //TreeMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceShader.frag");

        TreeLeavesMesh.clear();
        TreeLeavesMesh.load("Assets/treeLeaves0.obj");
        TreeLeavesMesh.getMaterial(0)->TexAlbedo = "Assets/richard/leaves3_color.png";
        TreeLeavesMesh.getMaterial(0)->TexDepth = "Assets/richard/leaves3_alpha.jpg";
        //TreeLeavesMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstancePineShader.vert");
        //TreeLeavesMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        BushMesh.clear();
        BushMesh.load("Assets/bush0.obj");
        BushMesh.getMaterial(0)->TexAlbedo = "Assets/richard/placeholder.png";
        BushMesh.getMaterial(0)->TexDepth = "Assets/richard/placeholderAlpha.png";
        //BushMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceGrassShader.vert");
        //BushMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader.frag");

        GrassMesh.clear();
        GrassMesh.load("Assets/grass0.obj");
        GrassMesh.getMaterial(0)->TexAlbedo = "Assets/richard/grass_color.jpg";
        GrassMesh.getMaterial(0)->TexDepth = "Assets/richard/grassAlpha.png";
        //GrassMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceGrassShader.vert");
        //GrassMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceGrassShader (2).frag");

        RockMesh.clear();
        RockMesh.load("Assets/rock0.obj");
        RockMesh.getMaterial(0)->TexAlbedo = "Assets/richard/Rock_035_baseColor.jpg";
        RockMesh.getMaterial(0)->TexNormal = "Assets/richard/Rock_035_normal.jpg";
        RockMesh.getMaterial(0)->TexDepth = "Assets/richard/Rock_035_Packed.png";
        //RockMesh.getMaterial(0)->VertexShaderSources.push_back("Shader/InstanceShader.vert");
        //RockMesh.getMaterial(0)->FragmentShaderSources.push_back("Shader/InstanceRockShader.frag");
    }


    void listen ( const GUICallbackObject Msg ) override {
        if (Msg.FormID == 1) {
            //GRAPHICS SETTINGS
            cameraMode = *((int*)Msg.Data.at(GUI_CAMERAMODE).pData) == 2;
            shadows = *((bool*)Msg.Data.at(GUI_SHADOWS).pData);
            wireframe = *((bool*)Msg.Data.at(GUI_WIREFRAME).pData);
            renderGrass = *((bool*)Msg.Data.at(GUI_RENDERGRASS).pData);
            if (*((bool*)Msg.Data.at(GUI_UNLOCKEDFRAMERATE).pData)) {   //'unlock framerate'
                glfwSwapInterval(0);
            } else {
                glfwSwapInterval(1);
            };
            cameraPanSpeed = *((float*)Msg.Data.at(GUI_CAMERASPEED).pData);
            CAM_FOV = *((float*)Msg.Data.at(GUI_FOV).pData);
            camera.projectionMatrix(WINWIDTH, WINHEIGHT, GraphicsUtility::degToRad(CAM_FOV), 0.1f, 5000.0f);
        } else if (Msg.FormID == 2) {
            //GENERATE NEW TERRAIN
            const int seed = *((int*)Msg.Data.at(GUI_TERRAIN_SEED).pData);
            const float scale = *((float*)Msg.Data.at(GUI_TERRAIN_SCALE).pData);
            const int octaves = *((int*)Msg.Data.at(GUI_TERRAIN_OCTAVES).pData);
            const float persistance = *((float*)Msg.Data.at(GUI_TERRAIN_PERSISTANCE).pData);
            const float lacunarity = *((float*)Msg.Data.at(GUI_TERRAIN_LACUNARITY).pData);
            const float height = *((float*)Msg.Data.at(GUI_TERRAIN_HEIGHT).pData);


            noiseConfig.seed = (seed == 0) ? static_cast<uint32_t>(rand()) : seed;
            noiseConfig.scale = scale;
            noiseConfig.octaves = static_cast<uint32_t>(octaves);
            noiseConfig.persistence = persistance;
            noiseConfig.lacunarity = lacunarity;

            heightMapConfig.width = 1024;
            heightMapConfig.height = 1024;
            heightMapConfig.mapHeight = height;
            heightMapConfig.noiseConfig = noiseConfig;

            mapPointer->generateHeightMap(heightMapConfig);
            placeDekoElements(*mapPointer, *piPineActor, *piPineLeavesActor, *piTreeActor, *piTreeLeavesActor, *piPalmActor, *piPalmLeavesActor, *piRockActor, *piBushActor);
            //iPineActor.init(&PineMesh);
            //iPineLeavesActor.init(&PineLeavesMesh);
            //iTreeActor.init(&TreeMesh);
            //iTreeLeavesActor.init(&TreeLeavesMesh);
            //iPalmActor.init(&PalmMesh);
            //iPalmLeavesActor.init(&PalmLeavesMesh);
            //iRockActor.init(&RockMesh);
//             iBushActor.init(&BushMesh);
        } else if (Msg.FormID == 3) {
			sunAngleY = *((float*)Msg.Data.at(GUI_LIGHTING_DIRECTION).pData);
			sunAngleX = *((float*)Msg.Data.at(GUI_LIGHTING_TIME).pData);
			sunAuto = *((bool*)Msg.Data.at(GUI_LIGHTING_AUTO).pData);
			sunTime = 0.0;
		}
    };

    TerrainSetup() {

        noiseConfig.seed = static_cast<uint32_t>(rand());
        noiseConfig.scale = 1.0f;
        noiseConfig.octaves = 10;
        noiseConfig.persistence = 0.5f;
        noiseConfig.lacunarity = 2.0f;

        heightMapConfig.width = 1024 / 1;
        heightMapConfig.height = 1024 / 1;
        heightMapConfig.mapHeight = 400;
        heightMapConfig.noiseConfig = noiseConfig;

		SLOD* pSLOD = SLOD::instance();
		
		if (!LOD) {
			pSLOD->setLODLevels(std::vector<float>{1.0});
		}

        float cameraHeight = 2;

        if (richard) {
            DecoSetup();
            return;
        }

        initCForge(&window, &renderDevice, &camera, &sun, &light);

        SGNTransformation rootTransform;
        rootTransform.init(nullptr);
		//ClipMap::ClipMapConfig clipMapConfig = { .sideLength = 16, .levelCount = 5 };
		ClipMap::ClipMapConfig clipMapConfig;
        clipMapConfig.sideLength = 4;
        clipMapConfig.levelCount = 6;
		//ClipMap::ClipMapConfig clipMapConfig = { .sideLength = 128, .levelCount = 5 };

        TerrainMap map = TerrainMap(&rootTransform);
        mapPointer = &map;
        map.generateClipMap(clipMapConfig);    
        map.generateHeightMap(heightMapConfig);

        SceneGraph sceneGraph;

        DekoMesh PineMesh;
        LODActor iPineActor;
        DekoMesh PineLeavesMesh;
        LODActor iPineLeavesActor;
        DekoMesh PalmMesh;
        LODActor iPalmActor;
        DekoMesh PalmLeavesMesh;
        LODActor iPalmLeavesActor;
        DekoMesh TreeMesh;
        LODActor iTreeActor;
        DekoMesh TreeLeavesMesh;
        LODActor iTreeLeavesActor;
        DekoMesh GrassMesh;
        LODActor iGrassActor;
        DekoMesh BushMesh;
        LODActor iBushActor;
        DekoMesh RockMesh;
        LODActor iRockActor;
        piPineActor = &iPineActor;
        piPineLeavesActor = &iPineLeavesActor;
        piPalmActor = &iPalmActor;
        piPalmLeavesActor = &iPalmLeavesActor;
        piTreeActor = &iTreeActor;
        piTreeLeavesActor = &iTreeLeavesActor;
        piGrassActor = &iGrassActor;
        piBushActor = &iBushActor;
        piRockActor = &iRockActor;

        loadNewDekoObjects(generateNew, PineMesh, PineLeavesMesh, PalmMesh, PalmLeavesMesh, TreeMesh, TreeLeavesMesh, GrassMesh, RockMesh, BushMesh);

        placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor);
		printf("iPineActor\n");
        iPineActor.init(&PineMesh, true, true);
		printf("iPineLeavesActor\n");
        iPineLeavesActor.init(&PineLeavesMesh, true, true);
		printf("iTreeActor\n");
        iTreeActor.init(&TreeMesh, true, true);
		printf("iTreeLeavesActor\n");
        iTreeLeavesActor.init(&TreeLeavesMesh, true, true);
		printf("iPalmActor\n");
        iPalmActor.init(&PalmMesh, true, true);
		printf("iPalmLeavesActor\n");
        iPalmLeavesActor.init(&PalmLeavesMesh, true, true);
		printf("iRockActor\n");
        iRockActor.init(&RockMesh, true, true);
		iGrassActor.init(&GrassMesh, true, true);
		//iGrassActor.m_castShadows = false;
		iBushActor.init(&BushMesh, true, true);

		if (LOD) {
			piPineActor->generateLODModells();
			piPineLeavesActor->generateLODModells();
			piPalmActor->generateLODModells();
			piPalmLeavesActor->generateLODModells();
			piTreeActor->generateLODModells();
			piTreeLeavesActor->generateLODModells();
			piGrassActor->generateLODModells();
			piRockActor->generateLODModells();
		}
		
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
		glBindBufferBase(GL_UNIFORM_BUFFER, 5, windUBO);
		setWindUBO(windUBO, windVec, 0);
        srand((unsigned int)time(NULL));

        sceneGraph.init(&rootTransform);

        ScreenQuad quad;
        initDebugQuad(&quad, "Shader/ScreenQuad.vert", "Shader/DebugQuad.frag");

		// initialize skybox
		//SkyboxActor Skybox;
		//std::vector<string> BoxTexs;
		//BoxTexs.push_back("Assets/skybox/right.jpg");
		//BoxTexs.push_back("Assets/skybox/left.jpg");
		//BoxTexs.push_back("Assets/skybox/top.jpg");
		//BoxTexs.push_back("Assets/skybox/bottom.jpg");
		//BoxTexs.push_back("Assets/skybox/back.jpg");
		//BoxTexs.push_back("Assets/skybox/front.jpg");
		//Skybox.init(BoxTexs[0], BoxTexs[1], BoxTexs[2], BoxTexs[3], BoxTexs[4], BoxTexs[5]);

        GUI gui = GUI();
        gui.init (&window);

        CallbackTestClass callbacktest;
        FormWidget* form = gui.createOptionsWindow(U"Graphics", 1, U"");
        form->startListening(this);
        form->startListening(&callbacktest);
        form->addOption(GUI_CAMERAMODE, INPUTTYPE_DROPDOWN, U"Camera Mode");
        std::map<int, std::u32string> cameraModes;
        cameraModes[1] = U"Free Floating";
        cameraModes[2] = U"Ground Actor";
        form->setDropDownOptions(GUI_CAMERAMODE, cameraModes);
        form->addOption(GUI_SHADOWS, INPUTTYPE_BOOL, U"Enable Shadows");
        form->setDefault(GUI_SHADOWS, shadows);
        form->addOption(GUI_WIREFRAME, INPUTTYPE_BOOL, U"Wireframe");
        form->setDefault(GUI_WIREFRAME, wireframe);
        form->addOption(GUI_RENDERGRASS, INPUTTYPE_BOOL, U"Enable Grass");
        form->setDefault(GUI_RENDERGRASS, renderGrass);
        form->addOption(GUI_UNLOCKEDFRAMERATE, INPUTTYPE_BOOL, U"Unlock Framerate");
        form->setDefault(GUI_UNLOCKEDFRAMERATE, false);
        form->addOption(GUI_CAMERASPEED, INPUTTYPE_RANGESLIDER, U"Camera Panning Speed");
        form->setLimit(GUI_CAMERASPEED, 0.0f, 5.0f);
        form->setStepSize(GUI_CAMERASPEED, 0.5f);
        form->setDefault(GUI_CAMERASPEED, 1);
        form->addOption(GUI_FOV, INPUTTYPE_RANGESLIDER, U"Field of View");
        form->setLimit(GUI_FOV, 60.0f, 120.0f);
        form->setStepSize(GUI_FOV, 1.0f);
        form->setDefault(GUI_FOV, CAM_FOV);

        //re-use the form variable for other windows because after the setup,
        //the widget's pointer not very useful for us (and it's easier to copy/paste)
        form = gui.createOptionsWindow(U"Terrain", 2, U"Generate");
        form->startListening(this);
        form->startListening(&callbacktest);
        form->addOption(GUI_TERRAIN_HEIGHT, INPUTTYPE_RANGESLIDER, U"Terrain Height");
        form->setLimit(GUI_TERRAIN_HEIGHT, 0.0f, 1000.0f);
        form->setStepSize(GUI_TERRAIN_HEIGHT, 10.0f);
        form->setDefault(GUI_TERRAIN_HEIGHT, heightMapConfig.mapHeight);
        form->addOption(GUI_TERRAIN_SEED, INPUTTYPE_INT, U"Noise Seed\n(0 for random)");
        form->setLimit(GUI_TERRAIN_SEED, 0, 1000000);  //1 million isn't an int's max value but should be enough for this
        form->setDefault(GUI_TERRAIN_SEED, 0);
        form->addOption(GUI_TERRAIN_SCALE, INPUTTYPE_RANGESLIDER, U"Noise Scale");
        form->setLimit(GUI_TERRAIN_SCALE, 0.0f, 5.0f);
        form->setStepSize(GUI_TERRAIN_SCALE, 0.1f);
        form->setDefault(GUI_TERRAIN_SCALE, noiseConfig.scale);
        form->addOption(GUI_TERRAIN_OCTAVES, INPUTTYPE_INT, U"Noise Octaves");
        form->setLimit(GUI_TERRAIN_OCTAVES, 0, 20);  //1 million isn't an int's max value but should be enough for this
        form->setDefault(GUI_TERRAIN_OCTAVES, (int)noiseConfig.octaves);
        form->addOption(GUI_TERRAIN_PERSISTANCE, INPUTTYPE_RANGESLIDER, U"Noise Persistance");
        form->setLimit(GUI_TERRAIN_PERSISTANCE, 0.0f, 1.0f);
        form->setStepSize(GUI_TERRAIN_PERSISTANCE, 0.05f);
        form->setDefault(GUI_TERRAIN_PERSISTANCE, noiseConfig.persistence);
        form->addOption(GUI_TERRAIN_LACUNARITY, INPUTTYPE_RANGESLIDER, U"Noise Lacunarity");
        form->setLimit(GUI_TERRAIN_LACUNARITY, 0.0f, 10.0f);
        form->setStepSize(GUI_TERRAIN_LACUNARITY, 0.1f);
        form->setDefault(GUI_TERRAIN_LACUNARITY, noiseConfig.lacunarity);

		form = gui.createOptionsWindow(U"Lighting", 3, U"");
		form->startListening(this);
		form->startListening(&callbacktest);
		form->addOption(GUI_LIGHTING_DIRECTION, INPUTTYPE_RANGESLIDER, U"Sun Direction");
		form->setLimit(GUI_LIGHTING_DIRECTION, 0.0f, 360.0f);
		form->setStepSize(GUI_LIGHTING_DIRECTION, 1.0f);
		form->setDefault(GUI_LIGHTING_DIRECTION, sunAngleY);
		form->addOption(GUI_LIGHTING_TIME, INPUTTYPE_RANGESLIDER, U"Sun Time");
		form->setLimit(GUI_LIGHTING_TIME, 0.0f, 360.0f);
		form->setStepSize(GUI_LIGHTING_TIME, 1.0f);
		form->setDefault(GUI_LIGHTING_TIME, sunAngleX);
		form->addOption(GUI_LIGHTING_AUTO, INPUTTYPE_BOOL, U"Auto Timer");
		form->setDefault(GUI_LIGHTING_AUTO, sunAuto);

/*        FormWidget* form2 = gui.createOptionsWindow(U"Test", 42);
        form2->startListening(&callbacktest);
//         form2->addOption(1, INPUTTYPE_INT, U"InputNumberWidget");
//         form2->setLimit(1, 10);
//         form2->addOption(2, INPUTTYPE_BOOL, U"InputCheckboxWidget");
//         form2->addOption(3, INPUTTYPE_STRING, U"InputTextWidget");
//         form2->setLimit(3, 5);
//         form2->setDefault(3, std::u32string(U"Text"));
//         form2->addOption(5, INPUTTYPE_RANGESLIDER, U"InputSliderWidget");
//         form2->setLimit(5, 10);
//         form2->setStepSize(5, 0.3f);
//         form2->setDefault(5, 1.5f);
        form2->addOption(4, INPUTTYPE_DROPDOWN, U"InputDropDownWidget");
        std::map<int, std::u32string> testDropdown;
        testDropdown[1] = U"First Option";
        testDropdown[2] = U"Second Option";
        testDropdown[3] = U"Third Option";
        form2->setDropDownOptions(4, testDropdown);*/

        //fps counter
        TextWidget* fpsWidget = gui.createPlainText();
        fpsWidget->setTextAlign(TextWidget::ALIGN_RIGHT);
        int32_t FPSCount = 0;
        clock_t current_ticks, delta_ticks;
        clock_t fps = 60;

        uint64_t LastFPS = CoreUtility::timestamp();

        uint32_t ScreenshotNumber = 0;
		
		// sea shader
		PPScreenQuad postProc;
		PPSQ_SSAO ppSSAO;
		initDebugQuad(&postProc, "Shader/ScreenQuad.vert", "Shader/sunSkyClouds.frag");
		initDebugQuad(&ppSSAO, "Shader/ScreenQuad.vert", "Shader/ssao.frag");
		float oceanTimer = 0.0;
		
		iPineLeavesActor.setFaceCulling(false);
		iPalmLeavesActor.setFaceCulling(false);
		iTreeLeavesActor.setFaceCulling(false);
		
		pSLOD->setResolution(Vector2i(WINWIDTH,WINHEIGHT));
        while (!window.shutdown()) {
			pSLOD->update();
			
			if (sunAuto)
				sunTime += pSLOD->getDeltaTime()*0.1;
			
			// update sun
			Eigen::Vector4f sd = GraphicsUtility::rotationMatrix(Quaternionf(AngleAxisf(sunAngleY/180.0*3.141592,Vector3f(0.0,1.0,0.0))))
			                     * GraphicsUtility::rotationMatrix(Quaternionf(AngleAxisf(sunAngleX/180.0*3.141592+sunTime,Vector3f(1.0,0.0,0.0))))
			                     * Eigen::Vector4f(0.0,0.0,-1.0,1.0);
			Eigen::Vector3f sunDir(sd[0],sd[1],sd[2]);

			//renderDevice.removeLight(&sun);
			sun.position(sunDir*450.0);
			sun.direction(-sunDir);
			sun.color(Vector3f(1.0,1.0,1.0));
			float intensity = std::max(sunDir.dot(Vector3f(0.0,1.0,0.0)),0.0f);
			sun.intensity(intensity*7.5);
			sun.color(Vector3f(1.0,1.0,1.0) * intensity + Vector3f(1.0,0.0,0.0)*(1.0-intensity));
			//sun.init(sunDir*450.0, -sunDir, Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			//const uint32_t ShadowMapDim = 8192;//4096; //2048;
			//sun.initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(1000, 1000), -500.0f, 800.0f);
			//sun->initShadowCasting(ShadowMapDim, ShadowMapDim, Eigen::Vector2i(500, 500), -100.0f, 700.0f);
			//renderDevice.updateLight(&sun);
			
			
			//renderDevice.clearBuffer();
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
            setWindUBO(windUBO, windVec, current_ticks / 60.0 * windWaveSpeedMultiplier);

            window.update();

            sceneGraph.update(1.0f);

            if (erode) {
                map.erode();
            }
			
			// take focus point into consideration
			float mapCenterX = camera.position().x() + camera.dir().x()*clipMapConfig.sideLength*(1/ CAM_FOV)*250.0;
			float mapCenterY = camera.position().z() + camera.dir().z()*clipMapConfig.sideLength*(1/ CAM_FOV)*250.0;
			map.update(mapCenterX, mapCenterY);
            //map.update(camera.position().x(), camera.position().z());

            renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			
            if (wireframe) {
#ifndef __EMSCRIPTEN__
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(1);
                //sceneGraph.render(&renderDevice);
				map.renderMap(&renderDevice);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
            } else {
                //sceneGraph.render(&renderDevice);
				map.renderMap(&renderDevice);
            }
			
			//renderDevice.activePass(RenderDevice::RENDERPASS_LOD);
			iPineActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iTreeActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iPalmActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iRockActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iPineLeavesActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iTreeLeavesActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iPalmLeavesActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			iGrassActor.testAABBvis(&renderDevice, Eigen::Matrix4f::Zero());
			
			renderDevice.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			/*renderDevice.LODSG_assemble();
			renderDevice.LODSG_render();*/
            //Deko instances
            //iPineActor.render(&renderDevice);
            //iTreeActor.render(&renderDevice);
            //iPalmActor.render(&renderDevice);
            //iRockActor.render(&renderDevice);
            //iBushActor.render(&renderDevice);

            glDisable(GL_CULL_FACE);
            if (renderGrass) {
                updateGrass(iGrassActor, map, camera);
                //iGrassActor.init(&GrassMesh);
                iGrassActor.render(&renderDevice);
            }
            //iPineLeavesActor.render(&renderDevice);
            //iTreeLeavesActor.render(&renderDevice);
            //iPalmLeavesActor.render(&renderDevice);
            glEnable(GL_CULL_FACE);
			
			renderDevice.activePass(RenderDevice::RENDERPASS_SHADOW, &sun);
			if (sunDir.dot(Vector3f(0.0,1.0,0.0)) > 0.0 && shadows) {
				map.renderMap(&renderDevice);
				//renderDevice.LODSG_render();
				if (renderGrass) {
					//iGrassActor.render(&renderDevice);
				}
			}
			
			//renderDevice.LODSG_clear();
			
            renderDevice.activePass(RenderDevice::RENDERPASS_LIGHTING);
			
			renderDevice.activePass(RenderDevice::RENDERPASS_FORWARD);
			//renderDevice.requestRendering(&Skybox, Quaternionf::Identity(), Vector3f::Zero(), Vector3f::Ones());

			//renderDevice.activePass(RenderDevice::RENDERPASS_POSTPROCESSING);
			oceanTimer += pSLOD->getDeltaTime();
			//std::cout << oceanTimer << "\n";
			ppSSAO.render(&renderDevice, oceanTimer, Eigen::Vector2i(WINWIDTH,WINHEIGHT));
			
			//renderDevice.PPBufferUpdate();
			postProc.render(&renderDevice, heightMapConfig.mapHeight*0.5, oceanTimer*1.5, (float)WINWIDTH/WINHEIGHT, sunDir, Eigen::Vector2i(WINWIDTH,WINHEIGHT));
			
            gui.processEvents();
            gui.render(&renderDevice);

            if (debugTexture) {
                glActiveTexture(GL_TEXTURE0);
                map.bindTexture();
                quad.render(&renderDevice);
            }
			
			//renderDevice.PPBuffersSwap();
//			renderDevice.PPBufferFinalize();
            window.swapBuffers();

            updateCamera(window.mouse(), window.keyboard(), renderDevice.activeCamera(), pSLOD->getDeltaTime());

			if (cameraMode) {
				camera.position(Vector3f(camera.position().x(),
			                             map.getHeightAt(camera.position().x(), camera.position().z()) + cameraHeight,
			                             //std::max(map.getHeightAt(camera.position().x(), camera.position().z()), heightMapConfig.mapHeight*0.5f+ cameraHeight) + cameraHeight,
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
                clipMapConfig.sideLength = 256;
                clipMapConfig.levelCount = 5;

                map.generateClipMap(clipMapConfig);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F4)) {
                window.keyboard()->keyState(Keyboard::KEY_F4, Keyboard::KEY_RELEASED);

                noiseConfig.seed = static_cast<uint32_t>(rand());
                noiseConfig.scale = 1.0f;
                noiseConfig.octaves = 10;
                noiseConfig.persistence = 0.5f;
                noiseConfig.lacunarity = 2.0f;

                heightMapConfig.width = 1024 / 1;
                heightMapConfig.height = 1024 / 1;
                heightMapConfig.mapHeight = 400;
                heightMapConfig.noiseConfig = noiseConfig;
                

                map.generateHeightMap(heightMapConfig);
                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor);
                //iPineActor.init(&PineMesh);
                //iPineLeavesActor.init(&PineLeavesMesh);
                //iTreeActor.init(&TreeMesh);
                //iTreeLeavesActor.init(&TreeLeavesMesh);
                //iPalmActor.init(&PalmMesh);
                //iPalmLeavesActor.init(&PalmLeavesMesh);
                //iRockActor.init(&RockMesh);
                iBushActor.init(&BushMesh);
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F5)) {
                window.keyboard()->keyState(Keyboard::KEY_F5, Keyboard::KEY_RELEASED);
                map.updateHeights();
                placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor);
                //iPineActor.init(&PineMesh);
                //iPineLeavesActor.init(&PineLeavesMesh);
                //iTreeActor.init(&TreeMesh);
                //iTreeLeavesActor.init(&TreeLeavesMesh);
                //iPalmActor.init(&PalmMesh);
                //iPalmLeavesActor.init(&PalmLeavesMesh);
                //iRockActor.init(&RockMesh);
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
            }
            if (window.keyboard()->keyPressed(Keyboard::KEY_F9)) {
                window.keyboard()->keyState(Keyboard::KEY_F9, Keyboard::KEY_RELEASED);
                renderGrass = !renderGrass;
            }
            //if (false && window.keyboard()->keyPressed(Keyboard::KEY_F10)) {
            //    window.keyboard()->keyState(Keyboard::KEY_F10, Keyboard::KEY_RELEASED);
            //    loadNewDekoObjects(generateNew, PineMesh, PineLeavesMesh, PalmMesh, PalmLeavesMesh, TreeMesh, TreeLeavesMesh, GrassMesh, RockMesh, BushMesh);

            //    placeDekoElements(map, iPineActor, iPineLeavesActor, iTreeActor, iTreeLeavesActor, iPalmActor, iPalmLeavesActor, iRockActor, iBushActor);
            //    iPineActor.init(&PineMesh);
            //    iPineLeavesActor.init(&PineLeavesMesh);
            //    iTreeActor.init(&TreeMesh);
            //    iTreeLeavesActor.init(&TreeLeavesMesh);
            //    iPalmActor.init(&PalmMesh);
            //    iPalmLeavesActor.init(&PalmLeavesMesh);
            //    iRockActor.init(&RockMesh);
            //    iBushActor.init(&BushMesh);
            //}

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
//                 printf("FPS: %d\n", fps);
                wchar_t text_wstring[100] = {0};
                int charcount = swprintf(text_wstring, 100, L"FPS: %d", fps);
                std::u32string text;
                //ugly cast to u32string from wchar[]
                for (int i = 0; i < charcount; i++) {
                    text.push_back((char32_t)text_wstring[i]);
                }
                fpsWidget->setText(text);
                fpsWidget->setPosition(window.width()-fpsWidget->getWidth(), 0);
            }

           /* delta_ticks = clock() - current_ticks;
            if (delta_ticks > 0)
                fps = CLOCKS_PER_SEC / delta_ticks;
            if (FPSCount % 60 == 0) {
                cout << fps << endl;
            }*/
        }
    }
private:
    enum GUI_OPTIONS {
        GUI_WIREFRAME,
        GUI_RENDERGRASS,
        GUI_CAMERAMODE,
        GUI_SHADOWS,
        GUI_UNLOCKEDFRAMERATE,
        GUI_CAMERASPEED,
        GUI_FOV,

        GUI_TERRAIN_SEED,
        GUI_TERRAIN_SCALE,
        GUI_TERRAIN_OCTAVES,
        GUI_TERRAIN_PERSISTANCE,
        GUI_TERRAIN_LACUNARITY,
        GUI_TERRAIN_HEIGHT,
		
		GUI_LIGHTING_DIRECTION,
		GUI_LIGHTING_TIME,
		GUI_LIGHTING_AUTO,
    };

    bool wireframe = false;
    bool debugTexture = false;
    bool shadows = true;
    bool richard = false;
    bool erode = false;
    bool cameraMode = false;
    bool generateNew = true;
    bool renderGrass = false;
	
	float sunAngleX = 0.0;
	float sunAngleY = 0.0;
	float sunTime = 0.0;
	bool sunAuto = false;
	
    TerrainMap* mapPointer;
    HeightMap::NoiseConfig noiseConfig;
    HeightMap::HeightMapConfig heightMapConfig;
    LODActor* piPineActor;
    LODActor* piPineLeavesActor;
    LODActor* piPalmActor;
    LODActor* piPalmLeavesActor;
    LODActor* piTreeActor;
    LODActor* piTreeLeavesActor;
    LODActor* piGrassActor;
    LODActor* piBushActor;
    LODActor* piRockActor;

    GLWindow window;
	RenderDevice::RenderDeviceConfig renderConfig;
    RenderDevice renderDevice;
    VirtualCamera camera;
    DirectionalLight sun;
    PointLight light;
};
}

