/*****************************************************************************\
*                                                                           *
* File(s): exampleSceneGraph.hpp                                            *
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
#ifndef __CFORGE_EXAMPLESCENEGRAPH_HPP__
#define __CFORGE_EXAMPLESCENEGRAPH_HPP__

#include "exampleSceneBase.hpp"

namespace CForge {
	class ExampleSceneGraph : public ExampleSceneBase {
	public:
		ExampleSceneGraph(void) {

		}//Constructor

		~ExampleSceneGraph(void) {

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
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Trees/LowPolyTree_02.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Trees[1].init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Trees/LowPolyTree_03.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			scaleAndOffsetModel(&M, 5.0f, Vector3f(0.0f, 0.25f, 0.0f));
			m_Trees[2].init(&M);
			M.clear();

			// sceen graph node that holds our forest
			m_TreeGroupSGN.init(&m_RootSGN);

			float Area = 500.0f;	// square area [-Area, Area] on the xz-plane, where trees are planted
			float TreeCount = 900;	// number of trees to create

			for (uint32_t i = 0; i < TreeCount; ++i) {
				// create the scene graph nodes
				SGNGeometry* pGeomSGN = nullptr;
				SGNTransformation* pTransformSGN = nullptr;

				// initialize position and scaling of the tree
				pTransformSGN = new SGNTransformation();
				pTransformSGN->init(&m_TreeGroupSGN);
				
				float TreeScale = CoreUtility::randRange(0.1f, 3.0f);

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

			}//for[TreeCount]

			// change sun settings to cover this large area
			m_Sun.position(Vector3f(100.0f, 1000.0f, 500.0f));
			m_Sun.initShadowCasting(2048, 2048, Vector2i(1000, 1000), 1.0f, 5000.0f);
		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();
		}//clear

		void run(void) {
			bool Fly = false;

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), 0.1f * 60.0f/m_FPS, 1.0f, 8.0f);
				// make sure to always walk on the ground if not flying
				if (!Fly) {
					Vector3f CamPos = m_Cam.position();
					CamPos.y() = 1.0f;
					m_Cam.position(CamPos);
				}

				m_SkyboxSG.update(60.0f/m_FPS);
				m_SG.update(60.0f / m_FPS);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
				m_SkyboxSG.render(&m_RenderDev);

				m_RenderWin.swapBuffers();

				updateFPS();

				// change between flying and walking mode
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F, true)) Fly = !Fly;

				defaultKeyboardUpdate(m_RenderWin.keyboard());
				
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
		
		SGNTransformation m_TreeGroupSGN;

	};//ExampleSceneGraph

}//name space

#endif 