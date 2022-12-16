/*****************************************************************************\
*                                                                           *
* File(s): exampleSkeletalAnimation.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to use skeletal animation.          *
*                       *
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
#ifndef __CFORGE_EXAMPLESKELETALCOLLISIONRESOLVER_HPP__
#define __CFORGE_EXAMPLESKELETALCOLLISIONRESOLVER_HPP__

#include "../CForge/Graphics/Actors/SkeletalActor.h"

#include "exampleSceneBase.hpp"
#include "../Prototypes/Assets/AnimationIO.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSkeletonCollisionResolver : public ExampleSceneBase {
	public:
		ExampleSkeletonCollisionResolver(void) {

		}//Constructor

		~ExampleSkeletonCollisionResolver(void) {
			clear();
		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			
			//SAssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			//SAssetIO::load("Assets/tmp/WalkingSittingEve2.fbx", &M);
			SAssetIO::load("Assets/tmp/MuscleMan3.fbx", &M);
			
			
			//M.mergeRedundantVertices();
			//M.clearSkeletalAnimations();
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/NewEveV6+3_3It_IB.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EricNoIGWristElbow.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MM4V6+3_2+3It_IB_HalfLearning.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EveV6+3_3It_IB_HalfLearning.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EricV3_5It_NoIG_HalfLearning.dat", &M);
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				M.getMaterial(i)->Color = Vector4f(1.0f, 0.6f, 0.6f, 1.0f);
				M.getMaterial(i)->TexAlbedo = "";
			}
			setMeshShader(&M, 0.7f, 0.04f);
			
			//M.sortBones(1);
			//M.printOutWeights();

			//uint64_t Start = CoreUtility::timestamp();
			//M.resolveCollisions(BoneIDs1, BoneIDs2, 0, 815, 816, 1, 0, 6, 1);
			//printf("Collision Resolved in %d ms\n", uint32_t(CoreUtility::timestamp() - Start));

			//AnimationIO::storeSkeletalAnimation("EricNoIGWrist4It.dat", &M, 7, 7);

			M.computePerVertexNormals();
			Controller.init(&M);
			Eric.init(&M, &Controller);
			M.clear();

			//SAssetIO::load("Assets/tmp/WalkingSittingEve2.fbx", &M);
			SAssetIO::load("Assets/tmp/MuscleMan3.fbx", &M);
			//SAssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			
			M.clearSkeletalAnimations();
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MM4V6+3_2+3It_IB.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MM4V6+3_2+3It_IB_HalfLearning.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/NewEveV6+3_3It_IB.dat", &M); 

			AnimationIO::loadSkeletalAnimation("Assets/Animations/MuscleManNoIG.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MuscleManBest.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EveBest.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EricBest.dat", &M); 
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EricNoIGWristElbow.dat", &M); 

			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EveV6+3_265-310V6_4It_IB.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EricNoIGWristElbow.dat", &M);
			setMeshShader(&M, 0.7f, 0.04f);

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				M.getMaterial(i)->Color = Vector4f(0.6f, 0.6f, 1.0f, 1.0f);
				M.getMaterial(i)->TexAlbedo = "";
			}

			M.computePerVertexNormals();
			ControllerOriginal.init(&M);
			EricOriginal.init(&M, &ControllerOriginal);
			M.clear();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add skeletal actor to scene graph (Eric)			
			EricTransformSGN.init(&m_RootSGN, Vector3f(0.0f, -0.06f, 0.0f));
			EricSGN.init(&EricTransformSGN, &Eric);
			EricSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			EricSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			// add skeletal actor to scene graph (Eric)			
			EricOriginalTransformSGN.init(&m_RootSGN, Vector3f(0.0f, -0.06f, 0.0f));
			EricOriginalSGN.init(&EricOriginalTransformSGN, &EricOriginal);
			EricOriginalSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			EricOriginalSGN.position(Vector3f(-3.0f, 0.0f, 0.0f));
			EricOriginalSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

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
		}

		void run(void) {
			bool paused = true;
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				if (paused == false) {
					Controller.update(m_FPS / 60.0f);
					ControllerOriginal.update(m_FPS / 60.0f);
				}

				// this will progress all active skeletal animations for this controller
				//m_BipedController.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				//float AnimationSpeed = 1000 / 60.0f;
				float AnimationSpeed = 0.25f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
				/*if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
					m_MuscleMan.activeAnimation(pAnim);
				}*/
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
					Eric.activeAnimation(pAnim);
					SkeletalAnimationController::Animation* pAnimOriginal = ControllerOriginal.createAnimation(0, AnimationSpeed, 0.0f);
					EricOriginal.activeAnimation(pAnimOriginal);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					paused = true;
					Controller.update(m_FPS / 60.0f);
					ControllerOriginal.update(m_FPS / 60.0f);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
					paused = true;
					for (int i = 0; i < 540; i++) {
						Controller.update(m_FPS / 60.0f);
						ControllerOriginal.update(m_FPS / 60.0f);
					}
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
					paused = !paused;
				}

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderWin.swapBuffers();

				updateFPS();
				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run
	protected:
		StaticActor m_Skydome;
		//SkeletalActor m_MuscleMan;
		//SkeletalAnimationController m_BipedController;

		SkeletalActor Eric;
		SkeletalAnimationController Controller;
		SkeletalActor EricOriginal;
		SkeletalAnimationController ControllerOriginal;
		int32_t BoneIDs1[4] = { 9, 10, 13, 14 }; //10=RightWrist, 14=LeftWrist (After Bones were Sorted)
		int32_t BoneIDs2[4] = { 15, 15, 19, 19 }; //15=RightHip, 19=LeftHip
		//int32_t BoneIDs2[4] = { 15, 15, 18, 18 }; //15=RightHip, 19=LeftHip

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry EricSGN;
		SGNTransformation EricTransformSGN; 
		SGNGeometry EricOriginalSGN;
		SGNTransformation EricOriginalTransformSGN;
		//SGNGeometry m_MuscleManSGN;
		//SGNTransformation m_MuscleManTransformSGN;

	};//ExampleSkeletonCollisionResolver

	void exampleSkeletonCollisionResolver(void) {

		ExampleSkeletonCollisionResolver Ex;
		Ex.init();
		Ex.run();
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif