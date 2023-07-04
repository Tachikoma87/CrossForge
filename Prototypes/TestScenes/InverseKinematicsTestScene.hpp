#ifndef __CFORGE_INVERSEKINEMATICSTESTSCENE_HPP__
#define __CFORGE_INVERSEKINEMATICSTESTSCENE_HPP__

#include "../InverseKinematics/IKSkeletalActor.h"
#include "../InverseKinematics/IKStickFigureActor.h"
#include "../../crossforge/MeshProcessing/PrimitiveShapeFactory.h"

#include "../../Examples/ExampleSceneBase.hpp"

#include <igl/unproject_ray.h>
#include <igl/ray_box_intersect.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	class InverseKinematicsTestScene : public ExampleSceneBase {
	public:
		InverseKinematicsTestScene(void) {
			m_WindowTitle = "CrossForge Prototype - Inverse Kinematics Test Scene";
		}//Constructor

		~InverseKinematicsTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{

			initWindowAndRenderDevice();
			gladLoadGL();			
			initCameraAndLights();		
			
			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);
			initFPSLabel();
			initSkybox();
			initCharacter(); // initialize character & animation controller
			initEndEffectorMarkers(); // initialize end-effector & target markers
			initTestActors();
			initRotationTest();
			
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement:(Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			m_HelpTexts.push_back(pKeybindings);
			pKeybindings->color(0.0f, 0.0f, 0.0f, 1.0f);
			m_DrawHelpTexts = true;

			// check whether a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			m_SelectedEffectorTarget = -1;
			m_LMBDownLastFrame = false;
		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();

			for (auto& i : m_EndEffectors) if (nullptr != i) delete i;
			m_EndEffectors.clear();

			for (auto& EffTransforms : m_EffectorTransformSGNs) {
				for (auto pSGN : EffTransforms.second) if (pSGN != nullptr) delete pSGN;
			}
			m_EffectorTransformSGNs.clear();

			for (auto& EffGeoms : m_EffectorGeomSGNs) {
				for (auto pSGN : EffGeoms.second) if (pSGN != nullptr) delete pSGN;
			}
			m_EffectorGeomSGNs.clear();

			for (auto& TargetTransforms : m_TargetTransformSGNs) {
				for (auto pSGN : TargetTransforms.second) if (pSGN != nullptr) delete pSGN;
			}
			m_TargetTransformSGNs.clear();

			for (auto& TargetGeoms : m_TargetGeomSGNs) {
				for (auto pSGN : TargetGeoms.second) if (pSGN != nullptr) delete pSGN;
			}
			m_TargetGeomSGNs.clear();
		}

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			m_CharacterController.update(60.0f / m_FPS);
			updateEndEffectorMarkers();
			
			// testing
			testRotations();
			
			if (m_RenderWin.mouse()->buttonState(Mouse::BTN_LEFT)) {
				if (!m_LMBDownLastFrame) pickTarget();
				if (m_SelectedEffectorTarget > -1) dragTarget();
			}
			else {
				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			}
			
			m_LMBDownLastFrame = m_RenderWin.mouse()->buttonState(Mouse::BTN_LEFT);

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				bool Enabled = true;
				m_CharacterSGN.enabled(nullptr, &Enabled);
				if (Enabled) {
					m_CharacterSGN.enable(true, false);
					m_CharacterStickSGN.enable(true, true);
				}
				else {
					m_CharacterSGN.enable(true, true);
					m_CharacterStickSGN.enable(true, false);
				}
			}

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			showMarkers(false);
			m_SG.render(&m_RenderDev);
			showMarkers(true);
			
			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			
			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
			m_SkyboxSG.render(&m_RenderDev);
			if (m_FPSLabelActive) m_FPSLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();
			
			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}//mainLoop

	protected:
		void pickTarget(void) {
			m_SelectedEffectorTarget = -1; // assume nothing will be picked
			
			const Vector4f Viewport = Vector4f(0.0f, 0.0f, float(m_RenderWin.width()), float(m_RenderWin.height())); 
			const Vector2f CursorPos = Vector2f(m_RenderWin.mouse()->position().x(), Viewport(3) - m_RenderWin.mouse()->position().y());
			const Matrix4f View = m_Cam.cameraMatrix();
			const Matrix4f Projection = m_Cam.projectionMatrix();
			
			Vector3f RayOrigin, RayDirection;
			igl::unproject_ray(CursorPos, View, Projection, Viewport, RayOrigin, RayDirection);
			RayDirection.normalize();

			for (int32_t i = 0; i < m_EndEffectors.size(); ++i) {
				auto* pEndEffector = m_EndEffectors[i];
				auto& TargetTransforms = m_TargetTransformSGNs.at(pEndEffector->Segment);

				int32_t AABBIndex = (pEndEffector->Segment == InverseKinematicsController::HEAD || pEndEffector->Segment == InverseKinematicsController::SPINE) ? 0 : pEndEffector->TargetPoints.cols();
				Vector3f AABBPos = TargetTransforms[AABBIndex]->translation();
				AlignedBox3f TranslatedAABB = AlignedBox3f(m_TargetMarkerAABB.min() + AABBPos, m_TargetMarkerAABB.max() + AABBPos);

				const float T0 = 0.0f;
				const float T1 = m_Cam.farPlane();
				float TMin, TMax; // minimum and maximum of interval of overlap within [T0, T1] -> not actually used here, but required by function

				if (igl::ray_box_intersect(RayOrigin, RayDirection, TranslatedAABB, T0, T1, TMin, TMax)) {
					m_SelectedEffectorTarget = i;

					Vector3f DragPlaneNormal = m_Cam.dir();
					float IntersectionDist = (AABBPos - RayOrigin).dot(DragPlaneNormal) / RayDirection.dot(DragPlaneNormal);
					m_DragStart = RayOrigin + (RayDirection * IntersectionDist);

					break;
				}
			}
		}//pickTarget

		void dragTarget(void) {
			auto* pEndEffector = m_EndEffectors[m_SelectedEffectorTarget];
			auto& TargetTransforms = m_TargetTransformSGNs.at(pEndEffector->Segment);
			const Vector4f Viewport = Vector4f(0.0f, 0.0f, float(m_RenderWin.width()), float(m_RenderWin.height()));
			const Vector2f CursorPos = Vector2f(m_RenderWin.mouse()->position().x(), Viewport(3) - m_RenderWin.mouse()->position().y());
			const Matrix4f View = m_Cam.cameraMatrix();
			const Matrix4f Projection = m_Cam.projectionMatrix();

			Vector3f RayOrigin, RayDirection;
			igl::unproject_ray(CursorPos, View, Projection, Viewport, RayOrigin, RayDirection);
			RayDirection.normalize();

			int32_t AABBIndex = (pEndEffector->Segment == InverseKinematicsController::HEAD || pEndEffector->Segment == InverseKinematicsController::SPINE) ? 0 : pEndEffector->TargetPoints.cols();
			Vector3f AABBPos = TargetTransforms[AABBIndex]->translation();

			Vector3f DragPlaneNormal = m_Cam.dir();
			float IntersectionDist = (AABBPos - RayOrigin).dot(DragPlaneNormal) / RayDirection.dot(DragPlaneNormal);
			Vector3f DragEnd = RayOrigin + (RayDirection * IntersectionDist);
			Vector3f DragTranslation = DragEnd - m_DragStart;
			m_DragStart = DragEnd;

			// apply translation to target marker transformation SGNs
			for (int32_t i = 0; i < TargetTransforms.size(); ++i) {
				if (TargetTransforms[i] == nullptr) continue;
				TargetTransforms[i]->translation(DragTranslation + TargetTransforms[i]->translation());
			}
			
			// apply translation to target points in character controller
			m_CharacterController.translateTarget(pEndEffector->Segment, DragTranslation);
		}//dragTarget

		void initCharacter(void) {
			T3DMesh<float> M;
			SAssetIO::load("MyAssets/Scaled.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_CharacterController.init(&M, "MyAssets/SkeletonConfig.json");
			m_Character.init(&M, &m_CharacterController);
			m_CharacterStick.init(&M, &m_CharacterController);
			M.clear();

			m_CharacterSGN.init(&m_RootSGN, &m_Character);
			m_CharacterStickSGN.init(&m_RootSGN, &m_CharacterStick);
			m_CharacterStickSGN.enable(true, false);
		}//initActors

		void initEndEffectorMarkers(void) {
			m_EndEffectors = m_CharacterController.retrieveEndEffectors();

			T3DMesh<float> M;

			// end-effector actors
			PrimitiveShapeFactory::uvSphere(&M, Vector3f(0.05f, 0.05f, 0.05f), 8, 8);
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
				pMat->Metallic = 0.3f;
				pMat->Roughness = 0.2f;
				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");
				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");
				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");
			}
			M.computePerVertexNormals();
			m_EffectorPos.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
			}
			m_EffectorX.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
			}
			m_EffectorY.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			}
			m_EffectorZ.init(&M);
			M.clear();

			// target actors
			PrimitiveShapeFactory::cuboid(&M, Vector3f(0.05f, 0.05f, 0.05f), Vector3i(1, 1, 1));
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 0.4f, 0.9f, 1.0f);
				pMat->Metallic = 0.3f;
				pMat->Roughness = 0.2f;
				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");
				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");
				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");
			}
			M.computePerVertexNormals();
			m_TargetPos.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
			}
			m_TargetX.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
			}
			m_TargetY.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			}
			m_TargetZ.init(&M);
			M.clear();

			// target aabb actor
			PrimitiveShapeFactory::cuboid(&M, Vector3f(0.5f, 0.5f, 0.5f), Vector3i(1, 1, 1));
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				pMat->Metallic = 0.3f;
				pMat->Roughness = 0.2f;
				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");
				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");
				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");
			}
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			m_TargetAABB.init(&M);
			m_TargetMarkerAABB = AlignedBox3f(M.aabb().Min, M.aabb().Max);
			M.clear();

			// scene graph nodes
			m_EffectorVis.init(&m_RootSGN);
			m_TargetVis.init(&m_RootSGN);

			for (int32_t i = 0; i < m_EndEffectors.size(); ++i) {
				m_EffectorTransformSGNs.try_emplace(m_EndEffectors[i]->Segment);
				m_EffectorGeomSGNs.try_emplace(m_EndEffectors[i]->Segment);
				m_TargetTransformSGNs.try_emplace(m_EndEffectors[i]->Segment);
				m_TargetGeomSGNs.try_emplace(m_EndEffectors[i]->Segment);

				auto& EffectorTransforms = m_EffectorTransformSGNs.at(m_EndEffectors[i]->Segment);
				auto& EffectorGeoms = m_EffectorGeomSGNs.at(m_EndEffectors[i]->Segment);
				auto& TargetTransforms = m_TargetTransformSGNs.at(m_EndEffectors[i]->Segment);
				auto& TargetGeoms = m_TargetGeomSGNs.at(m_EndEffectors[i]->Segment);

				EffectorTransforms = { nullptr };
				EffectorGeoms = { nullptr };
				TargetTransforms = { nullptr };
				TargetGeoms = { nullptr };

				for (int32_t j = 0; j < m_EndEffectors[i]->EndEffectorPoints.cols(); ++j) {
					EffectorTransforms[j] = new SGNTransformation();
					EffectorGeoms[j] = new SGNGeometry();
					EffectorTransforms[j]->init(&m_EffectorVis, m_EndEffectors[i]->EndEffectorPoints.col(j));
				}

				if (m_EndEffectors[i]->Segment == InverseKinematicsController::SkeletalSegment::HEAD) {
					EffectorGeoms[0]->init(EffectorTransforms[0], &m_EffectorPos);

					TargetTransforms[0] = new SGNTransformation();
					TargetTransforms[0]->init(&m_TargetVis, m_EndEffectors[i]->TargetPoints.col(0));

					TargetGeoms[0] = new SGNGeometry();
					TargetGeoms[1] = new SGNGeometry();

					TargetGeoms[0]->init(TargetTransforms[0], &m_TargetPos);
					TargetGeoms[1]->init(TargetTransforms[0], &m_TargetAABB);
					TargetGeoms[1]->visualization(SGNGeometry::Visualization::VISUALIZATION_WIREFRAME);
				}
				else if (m_EndEffectors[i]->Segment == InverseKinematicsController::SkeletalSegment::SPINE) {
					for (int32_t j = 0; j < m_EndEffectors[i]->EndEffectorPoints.cols(); ++j) {
						TargetTransforms[j] = new SGNTransformation();
						TargetGeoms[j] = new SGNGeometry();
						TargetTransforms[j]->init(&m_TargetVis, m_EndEffectors[i]->TargetPoints.col(j));
					}

					EffectorGeoms[0]->init(EffectorTransforms[0], &m_EffectorPos);
					EffectorGeoms[1]->init(EffectorTransforms[1], &m_EffectorZ);
					EffectorGeoms[2]->init(EffectorTransforms[2], &m_EffectorZ);
					EffectorGeoms[3]->init(EffectorTransforms[3], &m_EffectorX);
					EffectorGeoms[4]->init(EffectorTransforms[4], &m_EffectorX);
					EffectorGeoms[5]->init(EffectorTransforms[5], &m_EffectorY);
					EffectorGeoms[6]->init(EffectorTransforms[6], &m_EffectorY);

					TargetGeoms[0]->init(TargetTransforms[0], &m_TargetPos);
					TargetGeoms[1]->init(TargetTransforms[1], &m_TargetZ);
					TargetGeoms[2]->init(TargetTransforms[2], &m_TargetZ);
					TargetGeoms[3]->init(TargetTransforms[3], &m_TargetX);
					TargetGeoms[4]->init(TargetTransforms[4], &m_TargetX);
					TargetGeoms[5]->init(TargetTransforms[5], &m_TargetY);
					TargetGeoms[6]->init(TargetTransforms[6], &m_TargetY);

					int32_t AABBIndex = m_EndEffectors[i]->EndEffectorPoints.cols();

					TargetGeoms[AABBIndex] = new SGNGeometry();
					TargetGeoms[AABBIndex]->init(TargetTransforms[0], &m_TargetAABB);
					TargetGeoms[AABBIndex]->visualization(SGNGeometry::Visualization::VISUALIZATION_WIREFRAME);
				}
				else {
					for (int32_t j = 0; j < m_EndEffectors[i]->EndEffectorPoints.cols(); ++j) {
						TargetTransforms[j] = new SGNTransformation();
						TargetGeoms[j] = new SGNGeometry();
						EffectorGeoms[j]->init(EffectorTransforms[j], &m_EffectorPos);
						TargetTransforms[j]->init(&m_TargetVis, m_EndEffectors[i]->TargetPoints.col(j));
						TargetGeoms[j]->init(TargetTransforms[j], &m_TargetPos);
					}

					int32_t AABBIndex = m_EndEffectors[i]->EndEffectorPoints.cols();

					TargetTransforms[AABBIndex] = new SGNTransformation();
					TargetGeoms[AABBIndex] = new SGNGeometry();

					Vector3f AABBPos = m_EndEffectors[i]->TargetPoints.rowwise().sum();
					AABBPos /= m_EndEffectors[i]->TargetPoints.cols();
					
					TargetTransforms[AABBIndex]->init(&m_TargetVis, AABBPos);
					TargetGeoms[AABBIndex]->init(TargetTransforms[AABBIndex], &m_TargetAABB);
					TargetGeoms[AABBIndex]->visualization(SGNGeometry::Visualization::VISUALIZATION_WIREFRAME);
				}
			}
		}//initEndEffectorMarkers

		void updateEndEffectorMarkers(void) {
			m_CharacterController.updateEndEffectorValues(&m_EndEffectors);

			for (int32_t i = 0; i < m_EndEffectors.size(); ++i) {
				auto& EffectorTransforms = m_EffectorTransformSGNs.at(m_EndEffectors[i]->Segment);

				for (int32_t j = 0; j < m_EndEffectors[i]->EndEffectorPoints.cols(); ++j) {
					EffectorTransforms[j]->translation(m_EndEffectors[i]->EndEffectorPoints.col(j));
				}
			}
		}//updateEndEffectorMarkers

		void initTestActors(void) {
			T3DMesh<float> M;
			SAssetIO::load("MyAssets/JointMarker.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_CoordAxes.init(&M);
			M.clear();

			PrimitiveShapeFactory::cylinder(&M, Vector2f(0.1f, 0.1f), Vector2f(0.1f, 0.1f), 3.0f, 8, Vector2f(0.0f, 0.0f));
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");
				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");
				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");
				CForgeUtility::defaultMaterial(pMat, CForgeUtility::STONE_YELLOW);
			}
			M.computePerVertexNormals();
			m_Bone.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				CForgeUtility::defaultMaterial(pMat, CForgeUtility::STONE_WHITE);
			}
			m_BoneDefault.init(&M);
			
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				CForgeUtility::defaultMaterial(pMat, CForgeUtility::STONE_BLUE);
			}
			m_BoneMin.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				CForgeUtility::defaultMaterial(pMat, CForgeUtility::STONE_GREEN);
			}
			m_BoneMax.init(&M);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				CForgeUtility::defaultMaterial(pMat, CForgeUtility::STONE_RED);
			}
			m_BoneHinge.init(&M);

			M.clear();
		}//initDebugActor

		void initRotationTest(void) {
			m_AxesTransform[0].init(&m_RootSGN, Vector3f(0.0f, 3.0f, 20.0f));
			m_AxesTransform[1].init(&m_AxesTransform[0], Vector3f(-3.0f, 0.5f, 0.0f));
			m_AxesTransform[2].init(&m_AxesTransform[0], Vector3f(0.0f, 0.5f, 0.0f));
			m_AxesTransform[3].init(&m_AxesTransform[0], Vector3f(3.0f, 0.5f, 0.0f));
			m_AxesTransform[4].init(&m_AxesTransform[0], Vector3f(0.0f, -0.5f, 0.0f));

			m_AxesTransform[1].rotationDelta(Quaternionf(AngleAxisf(CForgeMath::degToRad(1.0f), Vector3f::UnitX())));
			m_AxesTransform[2].rotationDelta(Quaternionf(AngleAxisf(CForgeMath::degToRad(1.0f), Vector3f::UnitY())));
			m_AxesTransform[3].rotationDelta(Quaternionf(AngleAxisf(CForgeMath::degToRad(1.0f), Vector3f::UnitZ())));
			m_AxesTransform[4].rotationDelta(Quaternionf(AngleAxisf(CForgeMath::degToRad(1.0f), -Vector3f::UnitY())));

			m_AxesGeom[0].init(&m_AxesTransform[1], &m_CoordAxes);
			m_AxesGeom[1].init(&m_AxesTransform[2], &m_CoordAxes);
			m_AxesGeom[2].init(&m_AxesTransform[3], &m_CoordAxes);
			m_AxesGeom[3].init(&m_AxesTransform[4], &m_CoordAxes);
			m_AxesGeom[3].rotation(Quaternionf(AngleAxisf(CForgeMath::degToRad(180.0f), Vector3f::UnitZ())));
		}//initRotationTest

		void testRotations(void) {				



		}//testRotations

		void showMarkers(bool Visible) {
			m_EffectorVis.enable(true, Visible);
			m_TargetVis.enable(true, Visible);
		}//showMarkers

		SGNTransformation m_RootSGN;

		// character & controller
		IKSkeletalActor m_Character;
		IKStickFigureActor m_CharacterStick;
		InverseKinematicsController m_CharacterController;
		SGNGeometry m_CharacterSGN;
		SGNGeometry m_CharacterStickSGN;
		
		// end-effector & target markers
		std::vector<InverseKinematicsController::SkeletalEndEffector*> m_EndEffectors;

		SGNTransformation m_EffectorVis;
		std::map<InverseKinematicsController::SkeletalSegment, std::array<SGNTransformation*, 7>> m_EffectorTransformSGNs;
		std::map<InverseKinematicsController::SkeletalSegment, std::array<SGNGeometry*, 7>> m_EffectorGeomSGNs;
		StaticActor m_EffectorPos, m_EffectorX, m_EffectorY, m_EffectorZ;

		SGNTransformation m_TargetVis;
		std::map<InverseKinematicsController::SkeletalSegment, std::array<SGNTransformation*, 7>> m_TargetTransformSGNs;
		std::map<InverseKinematicsController::SkeletalSegment, std::array<SGNGeometry*, 8>> m_TargetGeomSGNs;
		StaticActor m_TargetPos, m_TargetX, m_TargetY, m_TargetZ, m_TargetAABB;
		AlignedBox3f m_TargetMarkerAABB;

		int32_t m_SelectedEffectorTarget; // index into m_EndEffectors vector; NOT id of joint inside m_CharacterController
		bool m_LMBDownLastFrame;
		Vector3f m_DragStart;

		// for debugging / testing
		StaticActor m_CoordAxes;
		StaticActor m_Bone;
		StaticActor m_BoneDefault;
		StaticActor m_BoneMin;
		StaticActor m_BoneMax;
		StaticActor m_BoneHinge;
		std::array<SGNTransformation, 5> m_AxesTransform;
		std::array<SGNGeometry, 4> m_AxesGeom;
	};//InverseKinematicsTestScene

	

}

#endif