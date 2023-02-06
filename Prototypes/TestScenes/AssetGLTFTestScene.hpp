/*****************************************************************************\
*                                                                           *
* File(s): AssetGLTFTestScene.hpp                                            *
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
#ifndef __CFORGE_ASSETGLTFTESTSCENE_HPP__
#define __CFORGE_ASSETGLTFTESTSCENE_HPP__

#ifndef __unix__

#include <Examples/ExampleSceneBase.hpp>
#include <CForge/MeshProcessing/PrimitiveShapeFactory.h>
#include "../Assets/GLTFIO.h"


#include <filesystem>
#include <iostream>

using namespace Eigen;
using namespace std;

namespace CForge {

	class AssetGLTFTestScene : public ExampleSceneBase {
	public:
		AssetGLTFTestScene(void) {
			m_WindowTitle = "CrossForge - GLTF Test Scene";
		}//Constructor

		~AssetGLTFTestScene(void) {
			clear();
		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			m_RenderWin.title(m_WindowTitle);

			initCameraAndLights();
			initSkybox();

			m_Sun.position(Vector3f(-50.0f, 100.0f, 200.0f));
			m_Sun.direction(-m_Sun.position().normalized());
			m_Sun.initShadowCasting(4096, 4096, Vector2i(100, 100), 0.5f, 1000.0f);

			// initialize scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			T3DMesh<float> M;

			// create ground plane
			PrimitiveShapeFactory::plane(&M, Vector2f(150.0f, 150.0f), Vector2i(1, 1));
			setMeshShader(&M, 0.1f, 0.0f);
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_WHITE);
			M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/Metal06.jpg";
			M.getMaterial(0)->TexNormal = "Assets/ExampleScenes/Metal06n.jpg";
			changeUVTiling(&M, Vector3f(50.0f, 50.0f, 1.0f));
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_GroundPlaneActor.init(&M);
			M.clear();
			m_GroundPlaneSGN.init(&m_RootSGN, &m_GroundPlaneActor, Vector3f(0.0f, -5.0f, 0.0f), Quaternionf::Identity(), Vector3f(3.0f, 3.0f, 3.0f));


			// define models to load and write
			GLTFIO gltfio;
			std::vector<std::string> FilesIn;
			std::vector<std::string> FilesOut;
			std::string BasePath = "MyAssets/Conversion/";

			Vector3f Position = Vector3f(-25.0f, 0.0f, 0.0f);
			float XOffset = 5.0f;
			float YOffset = 5.0f;

			FilesIn.push_back("BarramundiFish.glb");
			FilesIn.push_back("CesiumMan.gltf");
			FilesIn.push_back("pearl.fbx");
			FilesIn.push_back("RiggedFigure.gltf");
			FilesIn.push_back("ToyCar.gltf");
			FilesIn.push_back("WalkingSittingEve.fbx");
			FilesIn.push_back("MaterialsVariantsShoe.gltf");
			FilesIn.push_back("SheenChair.gltf");
			//FilesIn.push_back("Manuelgltf.gltf"); // that model kills performance, also does not work correclty after it has been saved and loaded again

			for (auto i : FilesIn) {
				FilesOut.push_back("Out/" + std::filesystem::path(i).replace_extension("gltf").string());
			}
			for (uint32_t i = 0; i < FilesIn.size(); ++i) {
				M.clear();
				SAssetIO::load(BasePath + FilesIn[i], &M);
				gltfio.store(BasePath + FilesOut[i], &M);
			}

			// first row are models loaded with AssImp
			for (auto i : FilesIn) {
				createObject(BasePath + i, Position, false);
				Position.x() += XOffset;
			}

			Position.x() = -25.0f;
			Position.z() -= 10.0f;

			// load models with gltfio (if gltf/glb)
			for (auto i : FilesIn) {
				if (gltfio.accepted(BasePath + i, I3DMeshIO::OP_LOAD)) {
					createObject(BasePath + i, Position, true);
				}
				Position.x() += XOffset;
			}

			// store models as gltf and load again
			for (uint32_t i = 0; i < FilesIn.size(); ++i) {
				std::string Filepath = BasePath + FilesIn[i];
				AssetIO::load(Filepath, &M);
				gltfio.store(BasePath + FilesOut[i], &M);
			}

			Position.x() = -25.0f;
			Position.z() -= 10.0f;

			// load models with gltfio again and place in world
			for (auto i : FilesOut) {
				createObject(BasePath + i, Position, true);
				Position.x() += XOffset;
			}

			m_VisWireframe = false;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());
		}//initialize

		void clear(void) {

		}//clear

		void release(void) {
			delete this;
		}//release

		void mainLoop(void) override {

			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			for (auto i : m_Controller) {
				if (nullptr != i) i->update(60.0f / m_FPS);
			}


			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, false);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);	
			m_SkyboxSG.render(&m_RenderDev);

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_P, true)) {
				for (uint32_t i = 0; i < m_Controller.size(); ++i) {
					if (nullptr != m_Controller[i]) {
						auto *pAnim = m_Controller[i]->createAnimation(0, 1.0f, 0.0f);
						SkeletalActor* pActor = static_cast<SkeletalActor*>(m_Actors[i]);
						pActor->activeAnimation(pAnim);
					}
				}
			}
		
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
				SGNGeometry::Visualization Vis = (m_VisWireframe) ? SGNGeometry::VISUALIZATION_FILL : SGNGeometry::VISUALIZATION_WIREFRAME;
				m_VisWireframe = !m_VisWireframe;
				for (auto i : m_ObjectSGNs) if (nullptr != i) i->visualization(Vis);
			}


		}//run

	protected:
		void changeUVTiling(T3DMesh<float>* pMesh, Eigen::Vector3f Factor) {

			std::vector<Vector3f> UVWs;
			for (uint32_t i = 0; i < pMesh->textureCoordinatesCount(); ++i)
				UVWs.push_back(pMesh->textureCoordinate(i).cwiseProduct(Factor));
			pMesh->textureCoordinates(&UVWs);
			if (pMesh->tangentCount() > 0) pMesh->computePerVertexTangents();

		}//changeUVTiling

		void createObject(const std::string Filepath, const Vector3f Position, bool UseGLTFIO) {
			try {
				T3DMesh<float> M;
				GLTFIO gltfio;
				(UseGLTFIO) ? gltfio.load(Filepath, &M) : AssetIO::load(Filepath, &M);
				M.computeAxisAlignedBoundingBox();
				M.computePerVertexNormals();
				setMeshShader(&M, 0.1f, 0.04f);

				IRenderableActor* pActor = nullptr;
				SkeletalAnimationController* pC = nullptr;
				if (M.boneCount() > 0) {
					SkeletalActor* pA = new SkeletalActor();
					pC = new SkeletalAnimationController();
					pC->init(&M, true);
					pA->init(&M, pC);
					pActor = pA;
				}
				else {
					StaticActor* pA = new StaticActor();
					pA->init(&M);
					pActor = pA;
				}

				SGNGeometry* pGeom = new SGNGeometry();
				SGNTransformation* pTrans = new SGNTransformation();

				pTrans->init(&m_RootSGN);
				pGeom->init(pTrans, pActor);

				pTrans->translation(Position);

				float Scale = 5.0f / M.aabb().diagonal().norm();
				pGeom->scale(Vector3f(Scale, Scale, Scale));

				m_Actors.push_back(pActor);
				m_Controller.push_back(pC);
				m_ObjectSGNs.push_back(pGeom);
				m_ObjectTransformSGNs.push_back(pTrans);

			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
			}
			catch (...) {
				printf("Exception occurred during loading of %s\n", Filepath.c_str());
			}
		}//createObject

		SceneGraph m_SG;
		SGNTransformation m_RootSGN;

		StaticActor m_GroundPlaneActor;
		SGNGeometry m_GroundPlaneSGN;

		std::vector<IRenderableActor*> m_Actors;
		std::vector<SGNGeometry*> m_ObjectSGNs;
		std::vector<SGNTransformation*> m_ObjectTransformSGNs;
		std::vector<SkeletalAnimationController*> m_Controller;

		bool m_VisWireframe;
	};//AssetGLTFTestScene

}

#endif
#endif