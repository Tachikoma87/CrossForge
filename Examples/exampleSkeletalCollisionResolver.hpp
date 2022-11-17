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
			SAssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			
			//SAssetIO::load("Assets/tmp/WalkingSittingEve2.fbx", &M);
			
			setMeshShader(&M, 0.7f, 0.04f);

			//Versions:
			//1: calculate all Verts (goodVerts) of Mesh1 inside Mesh2, 
			//   calculate Ray based off average Normal of colliding Mesh2 Triangles
			//   calculate max_distance / TargetPosition based off Ray-Plane Intersection goodVerts, Normal & Mesh2
			//2: calculate Ray based off average Normal of colliding Mesh2 Triangles
			//   calculate max_distance / TargetPosition based off Ray-Plane Intersection Verts Mesh1, Normal & Mesh2
			//-----Best One----
			//3: calculate Ray based off average Normal of colliding Mesh1 Triangles
			//   calculate max_distance / TargetPosition based off Ray-Plane Intersection Verts Mesh1, Normal & Mesh2
			//-----------------
			//4: calculate all Verts (goodVerts) of Mesh1 inside Mesh2, 
			//   calculate Ray based off average Normal of goodVerts
			//   calculate max_distance / TargetPosition based off Ray-Plane Intersection goodVerts, Normal & Mesh2

			//Adjust numIterations to higher number, if collisions aren't supposed to be there at all
			// 5 = some collisions are still there
			// 15 = almost all resolved

			M.resolveCollisions(BoneIDs1, BoneIDs2, 0, 50, 816, 8, 7, 3, 3);

			//AnimationIO::storeSkeletalAnimation("MyAssets/EricVersion3_3It_Initial_BSpline.dat", &M, 7, 7);

			M.computePerVertexNormals();
			Controller.init(&M);
			Eric.init(&M, &Controller);
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
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// this will progress all active skeletal animations for this controller
				//m_BipedController.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				//float AnimationSpeed = 1000 / 60.0f;
				float AnimationSpeed = 1.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
				/*if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
					m_MuscleMan.activeAnimation(pAnim);
				}*/
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = Controller.createAnimation(7, AnimationSpeed, 0.0f);
					Eric.activeAnimation(pAnim);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					Controller.update(m_FPS / 60.0f);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
					for (int i = 0; i < 540; i++) {
						Controller.update(m_FPS / 60.0f);
					}
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
		int32_t BoneIDs1[4] = { 9, 10, 13, 14 }; //10=RightWrist, 14=LeftWrist (After Bones were Sorted)
		int32_t BoneIDs2[4] = { 15, 15, 19, 19 }; //15=RightHip, 19=LeftHip
		//int32_t BoneIDs2[4] = { 15, 15, 18, 18 }; //15=RightHip, 19=LeftHip

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry EricSGN;
		SGNTransformation EricTransformSGN;
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