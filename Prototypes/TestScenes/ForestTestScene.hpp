/*****************************************************************************\
*                                                                           *
* File(s): VertexColorTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
\****************************************************************************/
#ifndef __CFORGE_FORESTTESTSCENE_HPP__
#define __CFORGE_FORESTTESTSCENE_HPP__

#include "../../CForge/Graphics/Actors/SkyboxActor.h"
#include "../../Examples/exampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ForestTestScene : public ExampleSceneBase {
	public:
		ForestTestScene(void) {
			m_WindowTitle = "CrossForge Prototype - Forest Test Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ForestTestScene(void) {
			clear();
		}//Destructor

		void init(void) {
			initWindowAndRenderDevice();

			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-55.0f, 50.0f, 50.0f);
			Vector3f SunLookAt = Vector3f(0.0f, 0.0f, -30.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
			m_Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 14.0f);
			// sun will cast shadows
			m_Sun.initShadowCasting(2048, 2048, GraphicsUtility::orthographicProjection(70.0f, 70.0f, 0.1f, 1000.0f));

			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 13.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			T3DMesh<float> M;

			SAssetIO::load("MyAssets/TexturedGround.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 15.0f;
			M.getMaterial(0)->TexAlbedo = "MyAssets/ground14.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/ground14n.jpg";
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_TexturedGround.init(&M);
			M.clear();


			AssetIO::load("Assets/tmp/lowpolytree.obj", &M);
			setMeshShader(&M, 0.7f, 0.94f);
			M.computePerVertexNormals();
			m_Tree1.init(&M);
			M.clear();

			AssetIO::load("Assets/tmp/Lowpoly_tree_sample.obj", &M);
			setMeshShader(&M, 0.7f, 0.94f);
			M.computePerVertexNormals();
			m_Tree2.init(&M);
			M.clear();


			vector<string> ClearSky;
			ClearSky.push_back("MyAssets/Cloudy/graycloud_rt.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_lf.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_dn.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_up.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_ft.jpg");
			ClearSky.push_back("MyAssets/Cloudy/graycloud_bk.jpg");


			m_Skybox.init(ClearSky[0], ClearSky[1], ClearSky[2], ClearSky[3], ClearSky[4], ClearSky[5]);

			// set initialize color adjustment values
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.1f);
			m_Skybox.saturation(1.2f);

			// create scene graph for the Skybox
			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxGeomSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);

			Quaternionf Rot;
			Rot = AngleAxisf(GraphicsUtility::degToRad(-2.5f / 60.0f), Vector3f::UnitY());
			m_SkyboxTransSGN.rotationDelta(Rot);

			// build scene graph
			
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add ground
			
			m_GroundSGN.init(&m_RootSGN, &m_TexturedGround);
			m_GroundSGN.scale(Vector3f(500.0f, 500.0f, 500.0f));
			Rot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_GroundSGN.rotation(Rot);

			// generate a forest
			const uint32_t TreeCount = 200;
			SGNGeometry* pTreeNodes = new SGNGeometry[TreeCount];
			SGNTransformation* pTreeTransNodes = new SGNTransformation[TreeCount];

			float MinPlane = -40.0f;
			float MaxPlane = 40.0f;

			for (uint32_t i = 0; i < TreeCount; ++i) {

				// placement in world
				Vector3f Pos;
				Pos.x() = CoreUtility::randRange(MinPlane, MaxPlane);
				Pos.y() = 0.0f;
				Pos.z() = CoreUtility::randRange(MinPlane, MaxPlane);

				float Scaling = CoreUtility::randRange(0.8f, 1.4f);

				Quaternionf RotationY;
				RotationY = AngleAxisf(GraphicsUtility::degToRad(CoreUtility::randRange(0.0f, 360.0f)), Vector3f::UnitY());


				pTreeTransNodes[i].init(&m_RootSGN);
				pTreeTransNodes[i].translation(Pos);
				pTreeTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
				pTreeTransNodes[i].rotation(RotationY);


				if (CoreUtility::rand() % 5 != 0) {
					Vector3f StaticOffset = Vector3f(0.0f, 1.8f * Scaling, 0.0f);
					pTreeNodes[i].init(&pTreeTransNodes[i], &m_Tree1, StaticOffset);
				}
				else {
					Vector3f StaticOffset = Vector3f(0.0f, 0.0f * Scaling, 0.0f);
					Vector3f StaticScale = Vector3f(0.15f, 0.15f, 0.15f);
					pTreeNodes[i].init(&pTreeTransNodes[i], &m_Tree2, StaticOffset, Quaternionf::Identity(), StaticScale);
				}


			}//for[generate trees]

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();
		}//clear

		void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				/*m_RenderDev.cameraUBO()->position(m_Sun.position());
				m_RenderDev.cameraUBO()->projectionMatrix(m_Sun.projectionMatrix());
				m_RenderDev.cameraUBO()->viewMatrix(m_Sun.viewMatrix());*/

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
				m_SkyboxSG.render(&m_RenderDev);

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
					static int32_t ScreenshotCount = 0;
					T2DImage<uint8_t> Img;
					T2DImage<uint8_t> DepthBuffer;
					GraphicsUtility::retrieveFrameBuffer(&Img, &DepthBuffer, 0.1f, 200.0f);
					AssetIO::store("../../Screenshot_" + std::to_string(ScreenshotCount) + ".jpg", &Img);
					AssetIO::store("../../DepthBuffer_" + std::to_string(ScreenshotCount) + ".jpg", &DepthBuffer);

					Img.clear();

					ScreenshotCount++;
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F3, true)) {
					T2DImage<uint8_t> Img;
					m_Sun.retrieveDepthBuffer(&Img);
					AssetIO::store("../../SunDepthbuffer.jpg", &Img);
				}

				m_RenderWin.swapBuffers();

				updateFPS();
				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run
	protected:
		StaticActor m_TexturedGround;
		StaticActor m_Tree1;
		StaticActor m_Tree2;
		SkyboxActor m_Skybox;

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_GroundSGN;
	};//ForestTestScene

	void forestTestScene(void) {

		ForestTestScene Scene;
		Scene.init();
		Scene.run();
		Scene.clear();

	}//exampleMinimumGraphicsSetup

}

#endif