/*****************************************************************************\
*                                                                           *
* File(s): exampleMinimumGraphicsSetup.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_IMUINPUTDEVICETESTSCENE_HPP__
#define __CFORGE_IMUINPUTDEVICETESTSCENE_HPP__

#include "../../Examples/ExampleSceneBase.hpp"
#include "IMUCameraController.h"
#include <crossforge/Graphics/Actors/SkyboxActor.h>


using namespace Eigen;
using namespace std;

namespace CForge {

	

	class IMUInputDeviceTestScene : public ExampleSceneBase {
	public:
		struct Rect {
			Vector2f Min;
			Vector2f Max;

			inline bool pointInside(Vector2f P) const {
				bool Rval = true;
				if (P.x() > Max.x()) Rval = false;
				if (P.y() > Max.y()) Rval = false;
				if (P.x() < Min.x()) Rval = false;
				if (P.y() < Min.y()) Rval = false;
				return Rval;
			}
		};//Rect

		struct Sphere {
			Vector3f Position;
			float Radius2;

			inline bool pointInside(Vector3f P) const {
				const Vector3f Vec = (P - Position);
				return Vec.dot(Vec) < Radius2;
			}
		};//Sphere

		IMUInputDeviceTestScene(void) {
			m_WindowTitle = "CForge - IMU Input Device Test Scene";

			m_TreeCount = 100;
			m_CoinCount = 100;
			m_ForestDimension = 100;

			m_pCoinNodes = nullptr;
			m_pCoinTransNodes = nullptr;
			m_pTreeNodes = nullptr;
			m_pTreeTransNodes = nullptr;
			m_pTreeSpheres = nullptr;
			m_pCoinBS = nullptr;

		}//Constructor

		~IMUInputDeviceTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();
			
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 0.25f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key lights) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-15.0f, 625.0f, 500.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 10.0f, -500.0f);
			m_Sun.init(SunPos, (Vector3f(0.0f, 0.0f, 0.0f) - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 15.0f);
			// sun will cast shadows
			uint32_t ShadowMapSize = 4 * 1024;
			//m_Sun.initShadowCasting(ShadowMapSize, ShadowMapSize, GraphicsUtility::orthographicProjection(275.0f, 275.0f, 5.0f, 1500.0f));
			m_Sun.initShadowCasting(ShadowMapSize, ShadowMapSize, Vector2i(275, 275), 5.0f, 1500.0f);
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);

			// load ground
			T3DMesh<float> M;
			
			SAssetIO::load("MyAssets/TexturedGround.fbx", &M);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 15.0f;
			setMeshShader(&M, 0.0f, 0.04f);
			M.getMaterial(0)->Color = 1.0f * Vector4f(0.75f, 0.85f, 0.75f, 1.0f);

			M.getMaterial(0)->TexAlbedo = "MyAssets/ground14.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/ground14n.jpg";

			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			M.clear();

			initSkybox();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

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

			AssetIO::load("MyAssets/StarCoin.glb", &M);
			setMeshShader(&M, 0.3f, 0.25f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				M.getMaterial(i)->TexAlbedo = "MyAssets/MaterialStar_baseColor.jpeg";
				M.getMaterial(i)->TexNormal = "MyAssets/MaterialStar_normal.jpeg";
			}
			m_Coin.init(&M);
			M.clear();

			// add ground
			m_GroundTransformSGN.init(&m_RootSGN);
			m_GroundSGN.init(&m_GroundTransformSGN, &m_Ground);
			m_GroundSGN.scale(Vector3f(600.0f, 600.0f, 1.0f));
			Quaternionf R = Quaternionf::Identity();
			R = AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX());
			m_GroundSGN.rotation(R);



			// set random seed
			CForgeMath::randSeed(CForgeUtility::timestamp());

			// generat forest
			generateForest(m_TreeCount, m_ForestDimension);
			
			// generate coins
			generateCoins(m_CoinCount, m_ForestDimension);

		


			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			uint64_t LastMessage = CForgeUtility::timestamp();

			uint8_t Buffer[256];
			uint32_t MsgLength;

			std::string Sender;
			uint16_t Port;

			m_IMUCam.init(25001, 25000, 200);
	
		}//init

		void clear(void) override {
			ExampleSceneBase::clear();
			m_IMUCam.clear();

			if(nullptr != m_pCoinNodes) delete[] m_pCoinNodes;
			if(nullptr != m_pCoinTransNodes) delete[] m_pCoinTransNodes;
			if(nullptr != m_pTreeNodes) delete[] m_pTreeNodes;;
			if(nullptr != m_pTreeTransNodes) delete[] m_pTreeTransNodes;
			if(nullptr != m_pTreeSpheres) delete[] m_pTreeSpheres;
			if(nullptr != m_pCoinBS) delete[] m_pCoinBS;

			m_pCoinNodes = nullptr;
			m_pCoinTransNodes = nullptr;
			m_pTreeNodes = nullptr;
			m_pTreeTransNodes = nullptr;
			m_pTreeSpheres = nullptr;
			m_pCoinBS = nullptr;

		}//clear


		void mainLoop(void) override{
			
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) Flying = !Flying;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) m_IMUCam.calibrate();

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) m_IMUCam.recordData("Assets/IMUData.csv");
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) m_IMUCam.recordData();

			Vector3f Pos = m_Cam.position();
			Pos.x() = std::max(-m_ForestDimension, Pos.x());
			Pos.x() = std::min(m_ForestDimension, Pos.x());
			if (!Flying) Pos.y() = 0.85f;
			Pos.z() = std::max(-m_ForestDimension, Pos.z());
			Pos.z() = std::min(m_ForestDimension, Pos.z());

			m_Cam.position(Pos);

			m_IMUCam.update(&m_Cam, 60.0f / m_FPS);

			Pos = m_Cam.position();

			// player collision with tree?
			for (uint32_t i = 0; i < m_TreeCount; ++i) {
				if (m_pTreeSpheres[i].pointInside(Pos)) {
					// set player outside sphere
					Vector3f V = m_pTreeSpheres[i].Position + std::sqrt(m_pTreeSpheres[i].Radius2) * (Pos - m_pTreeSpheres[i].Position).normalized();
					Pos.x() = V.x();
					Pos.z() = V.z();
					m_Cam.position(Pos);
				}
			}

			// player within coin range?
			for (uint32_t i = 0; i < m_CoinCount; ++i) {
				bool En;
				m_pCoinNodes[i].enabled(&En, nullptr);
				if (En && m_pCoinBS[i].pointInside(Pos)) {
					m_pCoinNodes[i].enable(false, false);
					PlayerScore++;
					printf("Player Score: %d\n", PlayerScore);
				}
			}


			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			m_SkyboxSG.render(&m_RenderDev);

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
			
		}//run
	protected:

		void generateForest(uint32_t TreeCount, float Dimensions) {
			// generate a forest	
			m_pTreeNodes = new SGNGeometry[TreeCount];
			m_pTreeTransNodes = new SGNTransformation[TreeCount];
			m_pTreeSpheres = new Sphere[TreeCount];

			float MinPlane = -Dimensions;
			float MaxPlane = Dimensions;

			for (uint32_t i = 0; i < TreeCount; ++i) {
				// placement in world
				Vector3f Pos;
				Pos.x() = CForgeMath::randRange(MinPlane, MaxPlane);
				Pos.y() = 0.0f;
				Pos.z() = CForgeMath::randRange(MinPlane, MaxPlane);

				float Scaling = CForgeMath::randRange(3.8f, 8.4f);

				Quaternionf RotationY;
				RotationY = AngleAxisf(CForgeMath::degToRad(CForgeMath::randRange(0.0f, 360.0f)), Vector3f::UnitY());

				m_pTreeTransNodes[i].init(&m_RootSGN);
				m_pTreeTransNodes[i].translation(Pos);
				m_pTreeTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
				m_pTreeTransNodes[i].rotation(RotationY);

				if (CForgeMath::rand() % 5 != 0) {
					Vector3f StaticOffset = Vector3f(0.0f, 1.8f * Scaling, 0.0f);
					m_pTreeNodes[i].init(&m_pTreeTransNodes[i], &m_Tree1, StaticOffset);
					m_pTreeSpheres[i].Position = Pos + Vector3f(0.0f, 1.0f, 0.0f);;
					const float r = 2.0f + Scaling / 8.0f;;
					m_pTreeSpheres[i].Radius2 = r * r;
				}
				else {
					Vector3f StaticOffset = Vector3f(0.0f, 0.0f * Scaling, 0.0f);
					Vector3f StaticScale = Vector3f(0.15f, 0.15f, 0.15f);
					m_pTreeNodes[i].init(&m_pTreeTransNodes[i], &m_Tree2, StaticOffset, Quaternionf::Identity(), StaticScale);
					m_pTreeSpheres[i].Position = Pos + Vector3f(0.0f, 1.0f, 0.0f);
					const float r = 2.5f;
					m_pTreeSpheres[i].Radius2 = r * r;
				}

			}//for[generate trees]
		}//generateForest

		void generateCoins(uint32_t CoinCount, float Dimension) {
			
			m_pCoinNodes = new SGNGeometry[CoinCount];
			m_pCoinTransNodes = new SGNTransformation[CoinCount];
			m_pCoinBS = new Sphere[CoinCount];

			float MinPlane = -Dimension;
			float MaxPlane = Dimension;

			float CoinScale = 1.25f;
			Quaternionf CoinRotDelta;
			CoinRotDelta = AngleAxisf(CForgeMath::degToRad(180.0f / 60.0f), Vector3f::UnitY());

			for (uint32_t i = 0; i < CoinCount; ++i) {
				// placement in world
				Vector3f Pos;
				Pos.x() = CForgeMath::randRange(MinPlane, MaxPlane);
				Pos.y() = 1.5f;
				Pos.z() = CForgeMath::randRange(MinPlane, MaxPlane);

				Quaternionf RotationY;
				RotationY = AngleAxisf(CForgeMath::degToRad(CForgeMath::randRange(0.0f, 360.0f)), Vector3f::UnitY());

				m_pCoinTransNodes[i].init(&m_RootSGN);
				m_pCoinTransNodes[i].translation(Pos);
				//pCoinTransNodes[i].scale(Vector3f(Scaling, Scaling, Scaling));
				m_pCoinTransNodes[i].rotation(RotationY);
				m_pCoinTransNodes[i].rotationDelta(CoinRotDelta);

				m_pCoinNodes[i].init(&m_pCoinTransNodes[i], &m_Coin, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(CoinScale, CoinScale, CoinScale));

				m_pCoinBS[i].Position = Pos;
				const float r = 2.0f;
				m_pCoinBS[i].Radius2 = r * r;
			}
		}//generateCoins

		StaticActor m_Ground;
		SGNTransformation m_RootSGN;

		SGNGeometry m_GroundSGN;
		SGNTransformation m_GroundTransformSGN;

		SGNGeometry* m_pCoinNodes;
		SGNTransformation* m_pCoinTransNodes;
		SGNGeometry* m_pTreeNodes;
		SGNTransformation* m_pTreeTransNodes;
		Sphere* m_pTreeSpheres;
		Sphere* m_pCoinBS;

		StaticActor m_Tree1;
		StaticActor m_Tree2;
		StaticActor m_Coin;

		IMUCameraController m_IMUCam;

		uint32_t m_TreeCount;
		uint32_t m_CoinCount;
		float m_ForestDimension;

		uint64_t LastMessage = CForgeUtility::timestamp();

		uint8_t Buffer[256];
		uint32_t MsgLength;

		std::string Sender;
		uint16_t Port;

		bool Flying = false;

		uint32_t PlayerScore = 0;

	};// IMUInputDeviceTestScene



}

#endif