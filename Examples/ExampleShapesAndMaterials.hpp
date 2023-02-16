/*****************************************************************************\
*                                                                           *
* File(s): ExampleShapesAndMaterials.hpp                                            *
*                                                                           *
* Content:           *
*                       *
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
#ifndef __CFORGE_EXAMPLESHAPESANDMATERIALS_HPP__
#define __CFORGE_EXAMPLESHAPESANDMATERIALS_HPP__

#include "ExampleSceneBase.hpp"
#include <crossforge/MeshProcessing/PrimitiveShapeFactory.h>

namespace CForge {
	class ExampleShapesAndMaterials : public ExampleSceneBase {
	public:
		ExampleShapesAndMaterials(void) {

		}//Constructor

		~ExampleShapesAndMaterials(void) {

		}//Destructor

		void init(void)override {
			initWindowAndRenderDevice();
			initCameraAndLights();
			initSkybox();
			initFPSLabel();

			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);


			// create ground plane
			T3DMesh<float> M;
			PrimitiveShapeFactory::plane(&M, Vector2f(150.0f, 150.0f), Vector2i(1, 1));
			setMeshShader(&M, 0.4f, 0.04f);
			M.changeUVTiling(Vector3f(20.0f, 20.0f, 1.0f));
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_WHITE);
			M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/Textures/Tiles107/Tiles107_1K_Color.webp";
			M.getMaterial(0)->TexNormal = "Assets/ExampleScenes/Textures/Tiles107/Tiles107_1K_NormalGL.webp";
			BoundingVolume BV;
			m_GroundPlane.init(&M);
			m_GroundPlane.boundingVolume(BV);
			m_GroundPlaneSGN.init(&m_RootSGN, &m_GroundPlane);
			M.clear();

			m_ModelType = 0;
			m_MaterialType = 0;
			m_Wireframe = false;

			// create models
			createObjects(m_ModelType, CForgeUtility::DefaultMaterial(m_MaterialType));

			// and let them rotate about the y-axis (90 degree per second)
			m_ObjectRotation = AngleAxisf(CForgeMath::degToRad(90.0f / 60.0f), Vector3f::UnitY());
			letObjectRotate(m_ObjectRotation);

			m_Sun.position(Vector3f(-50.0f, 100.0f, 200.0f));
			m_Sun.direction(-m_Sun.position().normalized());
			m_Sun.initShadowCasting(4096, 4096, Vector2i(50, 50), 0.5f, 1000.0f);

			m_Cam.position(Vector3f(-9.0f, 7.0f, -9.0f));
			m_Cam.lookAt(m_Cam.position(), Vector3f(0.0f, 3.0f, 0.0f));

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			LineOfText* pKeybindings2 = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			pKeybindings2->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "1: Cycle through shapes | M: Cycle through materials | V: Wireframe shapes");
			pKeybindings->color(0.0f, 0.0f, 0.0f, 1.0f);
			pKeybindings2->color(0.0f, 0.0f, 0.0f, 1.0f);
			m_HelpTexts.push_back(pKeybindings);
			m_HelpTexts.push_back(pKeybindings2);
			m_DrawHelpTexts = true;


		}//initialize

		void clear(void)override {

			ExampleSceneBase::clear();
		}//clear

		void release(void)override {
			delete this;
		}//release

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, true);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);
			m_SkyboxSG.render(&m_RenderDev);
			if (m_FPSLabelActive) m_FPSLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				m_ModelType = (m_ModelType + 1) % 7;
				createObjects(m_ModelType, CForgeUtility::DefaultMaterial(m_MaterialType));
				letObjectRotate(m_ObjectRotation);
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_M, true)) {
				m_MaterialType = (m_MaterialType+ 1) % CForgeUtility::DEFAULT_MATERIAL_COUNT;
				changeMaterial(CForgeUtility::DefaultMaterial(m_MaterialType));
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_V, true)) {
				m_Wireframe = !m_Wireframe;
				updateVisualization();
			}

		}//mainLoop

	protected:

		void updateVisualization(void) {
			for (auto i : m_ObjectSGNs) {
				if (nullptr == i) continue;
				i->visualization((m_Wireframe) ? SGNGeometry::VISUALIZATION_WIREFRAME : SGNGeometry::VISUALIZATION_FILL);
			}
		}//updateVisualization

		void letObjectRotate(const Eigen::Quaternionf Speed) {
			for (auto i : m_ObjectTransformSGNs) if (nullptr != i) i->rotationDelta(Speed);
		}//letObjectsRotate

		void changeMaterial(CForgeUtility::DefaultMaterial Mat) {
			for (uint32_t x = 0; x < 10; ++x) {
				for (uint32_t z = 0; z < 10; ++z) {
					uint32_t Index = (x * 10) + z;
					if (m_Objects.size() > Index) {
						CForgeUtility::defaultMaterial(m_Objects[Index]->material(0), Mat);
						m_Objects[Index]->material(0)->roughness(x / 10.0f);
						m_Objects[Index]->material(0)->metallic(z / 10.0f);
					}
				}//for[z]
			}//for[x]

			// last object separately
			if(m_Objects.size() > 0) CForgeUtility::defaultMaterial(m_Objects[m_Objects.size() - 1]->material(0), Mat);

		}//changeMaterial

		void destroyObjects(void) {
			for (auto& i : m_ObjectSGNs) if (nullptr != i) delete i;
			for (auto& i : m_ObjectTransformSGNs) if (nullptr != i) delete i;
			for (auto& i : m_Objects) if (nullptr != i) delete i;

			m_ObjectSGNs.clear();
			m_ObjectTransformSGNs.clear();
			m_Objects.clear();
		}//destroyObjects

		void createObjects(int32_t Object, CForgeUtility::DefaultMaterial Mat) {
			// clean up first
			destroyObjects();

			// create the object to render
			T3DMesh<float> M;
			switch (Object) {
			case 0: PrimitiveShapeFactory::uvSphere(&M, Vector3f(2.0f, 2.0f, 2.0f), 15, 15); break;
			case 1: PrimitiveShapeFactory::plane(&M, Vector2f(2.0f, 2.0f), Vector2i(1.0f, 1.0f)); break;
			case 2: PrimitiveShapeFactory::circle(&M, Vector2f(2.0f, 2.0f), 20, 0.0f); break;
			case 3: PrimitiveShapeFactory::cuboid(&M, Vector3f(2.0f, 2.0f, 2.0f), Vector3i(1, 1, 1)); break;
			case 4: PrimitiveShapeFactory::Torus(&M, 1.0f, 0.5f, 15, 15); break;
			case 5: PrimitiveShapeFactory::cone(&M, Vector3f(2.0f, 2.0f, 2.0f), 20); break;
			case 6: PrimitiveShapeFactory::cylinder(&M, Vector2f(2.0f, 2.0f), Vector2f(2.0f, 2.0f), 2.0f, 20, Vector2f(0.0f, 0.0f)); break;
			default: break;
			}

			setMeshShader(&M, 0.4f, 0.04f);
			M.computePerVertexNormals();
			CForgeUtility::defaultMaterial(M.getMaterial(0), Mat);


			// create 10 x 10 objects with variation of roughness along the x axis and metallic along the z axis
			for (uint32_t x = 0; x < 10; ++x) {
				for (uint32_t z = 0; z < 10; ++z) {
					StaticActor* pActor = new StaticActor();
					SGNGeometry* pGeomSGN = new SGNGeometry();
					SGNTransformation* pTransSGN = new SGNTransformation();

					pActor->init(&M);
					pTransSGN->init(&m_RootSGN, Vector3f(x*3.5f, 2.0f, z*3.5f));
					pGeomSGN->init(pTransSGN, pActor);

					m_Objects.push_back(pActor);
					m_ObjectSGNs.push_back(pGeomSGN);
					m_ObjectTransformSGNs.push_back(pTransSGN);

					pActor->material(0)->roughness(x / 10.0f);
					pActor->material(0)->metallic(z / 10.0f);
				}//for[y]
			}//for[x]

			// at last create one object that shows the original material
			StaticActor* pActor = new StaticActor();
			SGNGeometry* pGeomSGN = new SGNGeometry();
			SGNTransformation* pTransSGN = new SGNTransformation();

			pActor->init(&M);
			pTransSGN->init(&m_RootSGN, Vector3f(-4.5f, 4.0f, -4.5f));
			pTransSGN->scale(Vector3f(2.0f, 2.0f, 2.0f));
			pGeomSGN->init(pTransSGN, pActor);

			m_Objects.push_back(pActor);
			m_ObjectSGNs.push_back(pGeomSGN);
			m_ObjectTransformSGNs.push_back(pTransSGN);

			updateVisualization();

		}//createObjects

		StaticActor m_GroundPlane;
		SGNGeometry m_GroundPlaneSGN;

		SGNTransformation m_RootSGN;

		std::vector<StaticActor*> m_Objects;
		std::vector<SGNGeometry*> m_ObjectSGNs;
		std::vector<SGNTransformation*> m_ObjectTransformSGNs;

		int32_t m_ModelType;
		int32_t m_MaterialType;
		Eigen::Quaternionf m_ObjectRotation;
		bool m_Wireframe;
	};//ExampleShapesAndMaterials

}//namespace


#endif 