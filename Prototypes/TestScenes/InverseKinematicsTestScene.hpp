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
			m_WindowTitle = "CrossForge Example - Inverse Kinematics Test Scene";
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
			
			// Initialize scene graph and actors:
			
			// -> character
			// Initialize character animation controller
			Vector3f CharacterPosition = Vector3f::Zero();
			Quaternionf CharacterRotation = Quaternionf::Identity();
			Vector3f CharacterScaling = Vector3f(0.025f, 0.025f, 0.025f);

			T3DMesh<float> M;
			SAssetIO::load("MyAssets/NewModel.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_CharacterController.init(&M, "MyAssets/JointConfig.json", CharacterPosition, CharacterRotation, CharacterScaling); // CharacterPosition, CharacterRotation, CharacterScaling are used to compute global joint positions
			m_Character.init(&M, &m_CharacterController);
			m_CharacterStick.init(&M, &m_CharacterController);
			M.clear();

			m_CharacterTransformSGN.init(&m_RootSGN, CharacterPosition, CharacterRotation, CharacterScaling);
			m_CharacterSGN.init(&m_CharacterTransformSGN, &m_Character);
			m_CharacterStickSGN.init(&m_CharacterTransformSGN, &m_CharacterStick);
			m_CharacterStickSGN.enable(true, false);

			// -> markers for end-effector target positions
			PrimitiveShapeFactory::uvSphere(&M, Vector3f(0.1f, 0.1f, 0.1f), 8, 8);
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
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
			m_Target.init(&M);
			M.clear();

			PrimitiveShapeFactory::cuboid(&M, Vector3f(0.3f, 0.3f, 0.3f), Vector3i(1, 1, 1));
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				auto* pMat = M.getMaterial(i);
				pMat->Color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
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
			m_TargetMarker.init(&M);
			m_TargetMarkerAABB = AlignedBox3f(M.aabb().Min, M.aabb().Max);
			M.clear();
			
			m_EndEffectors = m_CharacterController.retrieveEndEffectors();

			m_TargetVisSGN.init(&m_RootSGN);

			for (int32_t i = 0; i < m_EndEffectors.size(); ++i) {
				SGNTransformation* pTargetTransformSGN = new SGNTransformation();
				SGNGeometry* pTargetSGN = new SGNGeometry();
				SGNGeometry* pMarkerSGN = new SGNGeometry();

				pTargetTransformSGN->init(&m_TargetVisSGN, m_EndEffectors[i]->Target);
				pTargetSGN->init(pTargetTransformSGN, &m_Target);
				pMarkerSGN->init(pTargetTransformSGN, &m_TargetMarker);
				pMarkerSGN->visualization(SGNGeometry::Visualization::VISUALIZATION_WIREFRAME);
				
				
				m_TargetTransformSGNs.push_back(pTargetTransformSGN);
				m_TargetSGNs.push_back(pTargetSGN);
				m_TargetMarkerSGNs.push_back(pMarkerSGN);
			}

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
			for (auto& i : m_TargetTransformSGNs) if (nullptr != i) delete i;
			for (auto& i : m_TargetSGNs) if (nullptr != i) delete i;
			for (auto& i : m_TargetMarkerSGNs) if (nullptr != i) delete i;
			m_EndEffectors.clear();
			m_TargetTransformSGNs.clear();
			m_TargetSGNs.clear();
			m_TargetMarkerSGNs.clear();
		}

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			m_CharacterController.update(60.0f / m_FPS);
			m_CharacterController.updateEndEffectorValues(&m_EndEffectors);
			
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
			m_TargetVisSGN.enable(true, false);
			m_SG.render(&m_RenderDev);
			m_TargetVisSGN.enable(true, true);

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

				//const AlignedBox3f TranslatedAABB = m_TargetMarkerAABB.translated(pEndEffector->Target); // according to emscripten 3.1.31 "translated" is not a member of AlignedBox<float, 3>
				const AlignedBox3f TranslatedAABB = AlignedBox3f(m_TargetMarkerAABB.min() + pEndEffector->Target, m_TargetMarkerAABB.max() + pEndEffector->Target);
				
				const float T0 = 0.0f;
				const float T1 = m_Cam.farPlane();
				float TMin, TMax; // minimum and maximum of interval of overlap within [T0, T1] -> not actually used here, but required by function
				
				if (igl::ray_box_intersect(RayOrigin, RayDirection, TranslatedAABB, T0, T1, TMin, TMax)) {
					m_SelectedEffectorTarget = i;
								
					Vector3f DragPlaneNormal = m_Cam.dir();
					float IntersectionDist = (pEndEffector->Target - RayOrigin).dot(DragPlaneNormal) / RayDirection.dot(DragPlaneNormal);
					m_DragStart = RayOrigin + (RayDirection * IntersectionDist);

					break;
				}
			}
		}//pickTarget

		void dragTarget(void) {
			auto* pEndEffector = m_EndEffectors[m_SelectedEffectorTarget];
			auto* pTargetTransformSGN = m_TargetTransformSGNs[m_SelectedEffectorTarget];
			
			const Vector4f Viewport = Vector4f(0.0f, 0.0f, float(m_RenderWin.width()), float(m_RenderWin.height()));
			const Vector2f CursorPos = Vector2f(m_RenderWin.mouse()->position().x(), Viewport(3) - m_RenderWin.mouse()->position().y());
			const Matrix4f View = m_Cam.cameraMatrix();
			const Matrix4f Projection = m_Cam.projectionMatrix();

			Vector3f RayOrigin, RayDirection;
			igl::unproject_ray(CursorPos, View, Projection, Viewport, RayOrigin, RayDirection);
			RayDirection.normalize();

			Vector3f DragPlaneNormal = m_Cam.dir();
			float IntersectionDist = (pEndEffector->Target - RayOrigin).dot(DragPlaneNormal) / RayDirection.dot(DragPlaneNormal);
			Vector3f DragEnd = RayOrigin + (RayDirection * IntersectionDist);
			Vector3f DragTranslation = DragEnd - m_DragStart;
			m_DragStart = DragEnd;

			// apply translation to target		
			pTargetTransformSGN->translation(DragTranslation + pTargetTransformSGN->translation());
			m_CharacterController.endEffectorTarget(pEndEffector->Segment, DragTranslation + pEndEffector->Target);
		}//dragTarget

		SGNTransformation m_RootSGN;

		IKSkeletalActor m_Character;
		IKStickFigureActor m_CharacterStick;
		InverseKinematicsController m_CharacterController;
		SGNGeometry m_CharacterSGN;
		SGNGeometry m_CharacterStickSGN;
		SGNTransformation m_CharacterTransformSGN;

		std::vector<InverseKinematicsController::SkeletalEndEffector*> m_EndEffectors;
		SGNTransformation m_TargetVisSGN;
		std::vector<SGNTransformation*> m_TargetTransformSGNs;
		std::vector<SGNGeometry*> m_TargetSGNs;
		std::vector<SGNGeometry*> m_TargetMarkerSGNs;
		StaticActor m_Target;
		StaticActor m_TargetMarker;
		AlignedBox3f m_TargetMarkerAABB;
				
		int32_t m_SelectedEffectorTarget; // index into m_Targets vector; NOT id of joint inside m_CharacterController
		bool m_LMBDownLastFrame;
		Vector3f m_DragStart;
	};//InverseKinematicsTestScene

	

}

#endif