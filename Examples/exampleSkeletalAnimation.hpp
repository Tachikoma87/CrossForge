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
#ifndef __CFORGE_EXAMPLESKELETALANIMATION_HPP__
#define __CFORGE_EXAMPLESKELETALANIMATION_HPP__

#include "../CForge/Graphics/Actors/SkeletalActor.h"

#include "exampleSceneBase.hpp"
#include "../Prototypes/MeshProcessing/ShapeDeformer.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSkeletalAnimation : public ExampleSceneBase {
	public:
		ExampleSkeletalAnimation(void) {

		}//Constructor

		~ExampleSkeletalAnimation(void) {
			clear();
		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome
			T3DMesh<float> M;

			T3DMesh<float> M2;
			
			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			SAssetIO::load("Assets/tmp/MuscleManSittingWalking.glb", &M);
			SAssetIO::load("Assets/tmp/MuscleMan3.glb", &M2);
			//SAssetIO::load("Assets/tmp/WalkingSittingEve.glb", &M);
			M.sortBones(1);
			M2.sortBones(1);
			M.mergeRedundantVertices();
			setMeshShader(&M, 0.7f, 0.04f);
			
			M.computePerVertexNormals();

			m_BipedController.init(&M);
			m_MuscleMan.init(&M, &m_BipedController);
			M.clear();
			M2.clear();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add skeletal actor to scene graph (Eric)			
			m_MuscleManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, -0.06f, 0.0f));
			m_MuscleManSGN.init(&m_MuscleManTransformSGN, &m_MuscleMan);
			m_MuscleManSGN.scale(Vector3f(0.025f, 0.025f, 0.025f));
			m_MuscleManSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

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
				m_BipedController.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				//float AnimationSpeed = 1000 / 60.0f;
				float AnimationSpeed = 0.5f;
				int currentFrame = 0;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
					m_MuscleMan.activeAnimation(pAnim);
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
		SkeletalActor m_MuscleMan;
		SkeletalAnimationController m_BipedController;
		
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_MuscleManSGN;
		SGNTransformation m_MuscleManTransformSGN;

	};//ExampleSkeletalAnimation

	void exampleSkeletalAnimation(void) {

		ExampleSkeletalAnimation Ex;
		Ex.init();
		Ex.run();
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif