/*****************************************************************************\
*                                                                           *
* File(s): FrustumCullingTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to use the scene graph to create    *
*          dynamic scene descriptions.                                      *
*                                                                           *
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
#ifndef __CFORGE_FRUSTUMCULLINGTESTSCENE_HPP__
#define __CFORGE_FRUSTUMCULLINGTESTSCENE_HPP__

#include "../../Examples/ExampleSceneBase.hpp"
#include <CForge/Graphics/Camera/ViewFrustum.h>

namespace CForge {
	class FrustumCullingTestScene : public ExampleSceneBase {
	public:
		FrustumCullingTestScene(void) {

		}//Constructor

		~FrustumCullingTestScene(void) {

		}//Destructor

		void init(void) {
			initWindowAndRenderDevice();
			initCameraAndLights();

			initSkybox();

			m_RootSGN.init(nullptr);
			m_SG.rootNode(&m_RootSGN);

			// initialize ground plane
			T3DMesh<float> M;

			// load the ground model
			SAssetIO::load("Assets/ExampleScenes/TexturedGround.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 50.0f;
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			M.clear();

			// initialize ground transformation and geometry scene graph node
			m_GroundTransformSGN.init(&m_RootSGN);
			m_GroundSGN.init(&m_GroundTransformSGN, &m_Ground);
			m_GroundSGN.scale(Vector3f(15.0f, 15.0f, 15.0f));

			// load the tree models
			SAssetIO::load("Assets/ExampleScenes/Trees/LowPolyTree_01.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			scaleAndOffsetModel(&M, 0.5f);
			m_Trees[0].init(&M);
			M.computeAxisAlignedBoundingBox();
			
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Trees/LowPolyTree_02.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Trees[1].init(&M);
			M.computeAxisAlignedBoundingBox();
			
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Trees/LowPolyTree_03.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			scaleAndOffsetModel(&M, 5.0f, Vector3f(0.0f, 0.25f, 0.0f));
			m_Trees[2].init(&M);
			M.computeAxisAlignedBoundingBox();
			
			M.clear();

			AssetIO::load("Assets/ExampleScenes/StarCoin/StarCoin.gltf", &M);
			setMeshShader(&M, 0.2f, 0.2f);
			M.computePerVertexTangents();
			m_Coin.init(&M);
			M.computeAxisAlignedBoundingBox();
			
			// this way we can turn off frustum culling
			/*BoundingVolume BV;
			m_Trees[0].boundingVolume(BV);
			m_Trees[1].boundingVolume(BV);
			m_Trees[2].boundingVolume(BV);
			m_Coin.boundingVolume(BV);*/

			// sceen graph node that holds our forest
			m_TreeGroupSGN.init(&m_RootSGN);
			m_CoinGroupSGN.init(&m_RootSGN);

			float Area = 1500.0f;	// square area [-Area, Area] on the xz-plane, where trees are planted
			float TreeCount = 7000;	// number of trees to create
			float CoinCount = 3000; // number of coins to create

			for (uint32_t i = 0; i < TreeCount; ++i) { 
				// create the scene graph nodes
				SGNGeometry* pGeomSGN = nullptr;
				SGNTransformation* pTransformSGN = nullptr;

				// initialize position and scaling of the tree
				pTransformSGN = new SGNTransformation();
				pTransformSGN->init(&m_TreeGroupSGN);

				float TreeScale = CoreUtility::randRange(0.1f, 3.0f);
				//TreeScale = 1.0f;

				Vector3f TreePos = Vector3f::Zero();
				TreePos.x() = CoreUtility::randRange(-Area, Area);
				TreePos.z() = CoreUtility::randRange(-Area, Area);

				pTransformSGN->translation(TreePos);
				pTransformSGN->scale(Vector3f(TreeScale, TreeScale, TreeScale));

				// initialize geometry
				// choose one of the trees randomly
				pGeomSGN = new SGNGeometry();
				uint8_t TreeType = CoreUtility::rand() % 3;
				pGeomSGN->init(pTransformSGN, &m_Trees[TreeType]);

				/*BoundingVolume BV;
				BV.init(AABBs[TreeType]);
				pGeomSGN->boundingVolume(&BV);
				pGeomSGN->boundingSphere(BS[TreeType]);
				pGeomSGN->aabb(AABBs[TreeType]);*/

			}//for[TreeCount]

			for (uint32_t i = 0; i < CoinCount; ++i) {
				SGNGeometry* pGeomSGN = nullptr;
				SGNTransformation* pTransformSGN = nullptr;

				pTransformSGN = new SGNTransformation();
				Vector3f CoinPos = Vector3f::Zero();
				CoinPos.y() = 10.0f + CoreUtility::randRange(-5.0f, 5.0f);
				CoinPos.x() = CoreUtility::randRange(-Area, Area);
				CoinPos.z() = CoreUtility::randRange(-Area, Area);

				float CoinScale = CoreUtility::randRange(0.25f, 5.0f);

				Quaternionf RotDelta = Quaternionf::Identity();
				Quaternionf RotDeltaY, RotDeltaZ;
				float Angle = GraphicsUtility::degToRad(CoreUtility::randRange(2.0f, 10.0f));
				RotDeltaY = AngleAxisf(CoreUtility::randRange(-Angle, Angle), Vector3f::UnitY());
				RotDeltaZ = AngleAxisf(CoreUtility::randRange(-Angle / 5.0f, Angle / 5.0f), Vector3f::UnitZ());
				RotDelta = RotDeltaY;

				pTransformSGN->init(&m_CoinGroupSGN, CoinPos, Quaternionf::Identity(), Vector3f(CoinScale, CoinScale, CoinScale));
				pTransformSGN->rotationDelta(RotDelta);

				pGeomSGN = new SGNGeometry();
				pGeomSGN->init(pTransformSGN, &m_Coin);

				/*BoundingVolume BV;
				BV.init(AABBs[3]);
				pGeomSGN->boundingVolume(&BV);

				pGeomSGN->boundingSphere(BS[3]);
				pGeomSGN->aabb(AABBs[3]);*/


			}//for[CoinCount]

			// change sun settings to cover this large area
			m_Sun.position(Vector3f(100.0f, 1000.0f, 500.0f));
			m_Sun.direction(-m_Sun.position().normalized());
			m_Sun.initShadowCasting(2048 * 2, 2048 * 2, Vector2i(1000, 1000), 1.0f, 5000.0f);

			/*m_RenderDev.cameraUBO()->position(m_Sun.position());
			m_RenderDev.cameraUBO()->projectionMatrix(m_Sun.projectionMatrix());
			m_RenderDev.cameraUBO()->viewMatrix(m_Sun.viewMatrix());*/

		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();

			for (auto& i : m_TreeTransformSGNs) delete i;
			for (auto& i : m_TreeSGNs) delete i;
		}//clear

		void run(void) {
			bool Fly = false;
			bool Orthographic = false;
			uint64_t LastPrint = CoreUtility::timestamp();

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), 0.1f * 60.0f / m_FPS, 1.0f, 8.0f);
				// make sure to always walk on the ground if not flying
				if (!Fly) {
					Vector3f CamPos = m_Cam.position();
					CamPos.y() = 1.0f;
					m_Cam.position(CamPos);
				}

				m_SkyboxSG.update(60.0f / m_FPS);
				m_SG.update(60.0f / m_FPS);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
				m_SG.render(&m_RenderDev);

				// culled in shadow pass
				//uint32_t CulledShadowPass = SGNCullingGeom::culled();

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_RenderDev.activeCamera(&m_Cam);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.render(&m_RenderDev);

				m_RenderWin.swapBuffers();

				/*uint32_t CulledRenderPass = SGNCullingGeom::culled();

				if (CoreUtility::timestamp() - LastPrint > 1000) {
					printf("Culled - Shadow Pass (%d) | Render Pass (%d)\n", CulledShadowPass, CulledRenderPass);
					LastPrint = CoreUtility::timestamp();
				}*/

				updateFPS();

				// change between flying and walking mode
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F, true)) Fly = !Fly;

				defaultKeyboardUpdate(m_RenderWin.keyboard());

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F6, true)) {
					T2DImage<uint8_t> Img;
					m_Sun.retrieveDepthBuffer(&Img);
					AssetIO::store("SunDepth.jpg", &Img);
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F7, true)) {
					if (!Orthographic) {
						float S = 5.0f;
						m_Cam.orthographicProjection(-8 * S, 8 * S, -4.5f*S, 4.5f * S, 0.01f, 1500.0f);
						Orthographic = true;
					}
					else {
						m_Cam.projectionMatrix(m_RenderWin.width(), m_RenderWin.height(), GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
						Orthographic = false;
					}
					m_RenderDev.activeCamera(nullptr);
					m_RenderDev.activeCamera(&m_Cam);
					
				}

			}//while[run]
		}//run

	protected:

		void scaleAndOffsetModel(T3DMesh<float>* pModel, float Factor, Vector3f Offset = Vector3f::Zero()) {
			Matrix3f Sc = Matrix3f::Identity();
			Sc(0, 0) = Factor;
			Sc(1, 1) = Factor;
			Sc(2, 2) = Factor;
			for (uint32_t i = 0; i < pModel->vertexCount(); ++i) pModel->vertex(i) = Sc * pModel->vertex(i) - Offset;
		}//scaleModel

		

		SGNTransformation m_RootSGN;

		StaticActor m_Ground;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_GroundTransformSGN;

		StaticActor m_Trees[3];
		std::vector<SGNTransformation*> m_TreeTransformSGNs;
		std::vector<SGNGeometry*> m_TreeSGNs;

		StaticActor m_Coin;
		std::vector<SGNTransformation*> m_CoinTransformSGNs;
		std::vector<SGNGeometry*> m_CoinSGNs;

		SGNTransformation m_TreeGroupSGN;
		SGNTransformation m_CoinGroupSGN;

	};//ExampleSceneGraph

}//name space

#endif 