/*****************************************************************************\
*                                                                           *
* File(s): StickFigureTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to use skeletal animation.          *
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
#ifndef __CFORGE_STICKFIGURETESTSCENE_HPP__
#define __CFORGE_STICKFIGURETESTSCENE_HPP__

#include <crossforge/Graphics/Actors/SkeletalActor.h>
#include <crossforge/Graphics/Actors/StickFigureActor.h>

#include "../../Examples/ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class StickFigureTestScene : public ExampleSceneBase {
	public:
		StickFigureTestScene(void) {
			m_WindowTitle = "CrossForge Example - Stick Figure Test Scene";
		}//Constructor

		~StickFigureTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			m_Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			//SAssetIO::load("Assets/ExampleScenes/CesiumMan/CesiumMan.gltf", &M);
			//AssetIO::load("MyAssets/pearl.fbx", &M);
			//AssetIO::load("MyAssets/Pearl/Pearl.glb", &M);
			//SAssetIO::load("MyAssets/WalkingSittingEve.glb", &M);
			SAssetIO::load("MyAssets/Technique_Evaluation/OldModel.gltf", &M);
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";

			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";


			//T3DMesh<float> Anim;
			//SAssetIO::load("MyAssets/Technique_Evaluation/OldGait_Modified.bvh", &Anim);
			//M.clearSkeletalAnimations();
			//M.addSkeletalAnimation(Anim.getSkeletalAnimation(0));

			//SAssetIO::load("MyAssets/Gehen_Sitzen_1-005.glb", &M);

			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			M.computeAxisAlignedBoundingBox();
			m_BipedController.init(&M);
			m_CesiumMan.init(&M, &m_BipedController);
			m_StickFigure.init(&M, &m_BipedController);
			M.clear();

			BoundingVolume BV;
			m_CesiumMan.boundingVolume(BV);
			m_StickFigure.boundingVolume(BV);

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(150.0f, 150.0f, 150.0f));

			float Scale = 0.035f;
			//Scale = 5.0f;

			// add skeletal actor to scene graph (Eric)			
			m_CesiumManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_CesiumManSGN.init(&m_CesiumManTransformSGN, &m_CesiumMan);
			m_CesiumManSGN.scale(Vector3f(Scale, Scale, Scale));
			//m_CesiumManSGN.visualization(SGNGeometry::VISUALIZATION_WIREFRAME);

			m_StickFigureTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_StickFigureSGN.init(&m_StickFigureTransformSGN, &m_StickFigure);
			m_StickFigureSGN.scale(Vector3f(Scale, Scale, Scale));
		

			Quaternionf Rot = Quaternionf::Identity();
			//Rot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_CesiumManSGN.rotation(Rot);
			m_StickFigureSGN.rotation(Rot);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, 1000.0f / 60.0f, 0.0f);
			m_CesiumMan.activeAnimation(pAnim);
			m_StickFigure.activeAnimation(pAnim);

			m_RepeatAnimation = true;
			m_VisualizeSkeleton = false;
			m_StickFigureSGN.enable(true, false);
			m_CesiumManSGN.enable(true, true);

			Vector3f Pos = Vector3f(13.0f, 4.0f, 15.0f);
			m_Cam.position(Pos);
			m_Cam.lookAt(Pos, Vector3f(0.0f, 2.8f, 10.0f));
		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			// this will progress all active skeletal animations for this controller
			m_BipedController.update(60.0f / m_FPS);
			if (m_RepeatAnimation && nullptr != m_CesiumMan.activeAnimation()) {
				auto* pAnim = m_CesiumMan.activeAnimation();
				if (pAnim->t >= pAnim->Duration) {
					pAnim->t -= pAnim->Duration; // pAnim->Duration;
					m_BipedController.update(60.0f / m_FPS);
				}

			}

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			// if user hits key 1, animation will be played
			// if user also presses shift, animation speed is doubled
			float AnimationSpeed = 1000.0f / 60.0f;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
				m_CesiumMan.activeAnimation(pAnim);
				m_StickFigure.activeAnimation(pAnim);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_R, true)) {
				m_RepeatAnimation = !m_RepeatAnimation;
			}

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderWin.swapBuffers();

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_O, true)) {
				T3DMesh<float>::Material M;
				CForgeUtility::defaultMaterial(&M, CForgeUtility::PLASTIC_RED);
				m_StickFigure.jointMaterial(M);
				CForgeUtility::defaultMaterial(&M, CForgeUtility::PLASTIC_WHITE);
				m_StickFigure.boneMaterial(M);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
				if (m_VisualizeSkeleton) {
					m_StickFigureSGN.enable(true, false);
					m_CesiumManSGN.enable(true, true);
				}
				else {
					m_StickFigureSGN.enable(true, true);
					m_CesiumManSGN.enable(true, false);
				}
				m_VisualizeSkeleton = !m_VisualizeSkeleton;
			}

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}

	protected:
		StaticActor m_Skydome;
		SkeletalActor m_CesiumMan;
		StickFigureActor m_StickFigure;
		SkeletalAnimationController m_BipedController;

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CesiumManSGN;
		SGNTransformation m_CesiumManTransformSGN;

		SGNGeometry m_StickFigureSGN;
		SGNTransformation m_StickFigureTransformSGN;

		bool m_RepeatAnimation;
		bool m_VisualizeSkeleton;

	};//StickFigureTestScene



}

#endif