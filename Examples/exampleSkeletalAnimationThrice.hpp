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
#ifndef __CFORGE_EXAMPLESKELETALANIMATIONTHRICE_HPP__
#define __CFORGE_EXAMPLESKELETALANIMATIONTHRICE_HPP__

#include "../CForge/Graphics/Actors/SkeletalActor.h"

#include "exampleSceneBase.hpp"
#include "../Prototypes/Assets/AnimationIO.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSkeletalAnimationThrice : public ExampleSceneBase {
	public:
		ExampleSkeletalAnimationThrice(void) {

		}//Constructor

		~ExampleSkeletalAnimationThrice(void) {
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
			setMeshShader(&M, 0.7f, 0.04f);
			M.sortBones(8);
			M.computePerVertexNormals();
			Controller.init(&M);
			Eric.init(&M, &Controller);
			M.clear();

			//Eric 2
			SAssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			setMeshShader(&M, 0.7f, 0.04f);

			M.sortBones(8);
			M.clearSkeletalAnimations();

			AnimationIO::loadSkeletalAnimation("Assets/ExampleScenes/Version3_2It_Initial_BSpline.dat", &M);

			M.computePerVertexNormals();
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				M.getMaterial(i)->Color = Vector4f(1.0f, 0.6f, 0.6f, 1.0f);
				M.getMaterial(i)->TexAlbedo = "";
			}

			Controller2.init(&M);
			Eric2.init(&M, &Controller2);

			M.clear();

			//Eric 3
			// initialize skeletal actor (Eric) and its animation controller
			SAssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			setMeshShader(&M, 0.7f, 0.04f);

			M.sortBones(8);
			M.clearSkeletalAnimations();

			AnimationIO::loadSkeletalAnimation("Assets/ExampleScenes/Version5_2It_Initial_BSpline.dat", &M);

			M.computePerVertexNormals();
			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				M.getMaterial(i)->Color = Vector4f(0.6f, 0.6f, 1.0f, 1.0f);
				M.getMaterial(i)->TexAlbedo = "";
			}
			Controller3.init(&M);
			Eric3.init(&M, &Controller3);

			M.clear();


			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add skeletal actor to scene graph (Eric)			
			EricTransformSGN.init(&m_RootSGN, Vector3f(-3.50f, 0.0f, 0.0f));
			EricSGN.init(&EricTransformSGN, &Eric);
			EricSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			EricSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			EricTransformSGN2.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			EricSGN2.init(&EricTransformSGN2, &Eric2);
			EricSGN2.scale(Vector3f(0.05f, 0.05f, 0.05f));
			EricSGN2.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			EricTransformSGN3.init(&m_RootSGN, Vector3f(3.50f, 0.0f, 0.0f));
			EricSGN3.init(&EricTransformSGN3, &Eric3);
			EricSGN3.scale(Vector3f(0.05f, 0.05f, 0.05f));
			EricSGN3.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

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

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1.0f;

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = Controller.createAnimation(7, AnimationSpeed, 0.0f);
					//SkeletalAnimationController::Animation* pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
					Eric.activeAnimation(pAnim);

					SkeletalAnimationController::Animation* pAnim2 = Controller2.createAnimation(0, AnimationSpeed, 0.0f);
					Eric2.activeAnimation(pAnim2);

					SkeletalAnimationController::Animation* pAnim3 = Controller3.createAnimation(0, AnimationSpeed, 0.0f);
					Eric3.activeAnimation(pAnim3);

				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					Controller.update(m_FPS / 60.0f);
					Controller2.update(m_FPS / 60.0f);
					Controller3.update(m_FPS / 60.0f);
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
					for (int i = 0; i < 450; i++) {
						Controller.update(m_FPS / 60.0f);
						Controller2.update(m_FPS / 60.0f);
						Controller3.update(m_FPS / 60.0f);
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
		SkeletalActor Eric;
		SkeletalAnimationController Controller;

		SkeletalActor Eric2;
		SkeletalAnimationController Controller2;

		SkeletalActor Eric3;
		SkeletalAnimationController Controller3;


		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry EricSGN;
		SGNTransformation EricTransformSGN;
		SGNGeometry EricSGN2;
		SGNTransformation EricTransformSGN2;
		SGNGeometry EricSGN3;
		SGNTransformation EricTransformSGN3;
	};//ExampleSkeletalAnimation

	void exampleSkeletalAnimationThrice(void) {

		ExampleSkeletalAnimationThrice Ex;
		Ex.init();
		Ex.run();
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif