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

#include "ExampleSceneBase.hpp"

#include "../Prototypes/Assets/GLTFIO.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSkeletalAnimation : public ExampleSceneBase {
	public:
		ExampleSkeletalAnimation(void) {
			m_WindowTitle = "CrossForge Example - Skeletal Animation";
		}//Constructor

		~ExampleSkeletalAnimation(void) {
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
			SAssetIO::load("Assets/ExampleScenes/CesiumMan/CesiumMan.gltf", &M);

			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_BipedController.init(&M);
			m_CesiumMan.init(&M, &m_BipedController);
			M.clear();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add skeletal actor to scene graph (Eric)			
			m_CesiumManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_CesiumManSGN.init(&m_CesiumManTransformSGN, &m_CesiumMan);
			m_CesiumManSGN.scale(Vector3f(3.0f, 3.0f, 3.0f));

			Quaternionf Rot;
			Rot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			m_CesiumManSGN.rotation(Rot);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			m_RepeatAnimation = false;
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
				if (m_RepeatAnimation && nullptr != m_CesiumMan.activeAnimation()) {
					auto* pAnim = m_CesiumMan.activeAnimation();
					if (pAnim->t >= pAnim->Duration) pAnim->t -= pAnim->Duration;
				}

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1000 / 60.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
					m_CesiumMan.activeAnimation(pAnim);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_R, true)) {
					m_RepeatAnimation = !m_RepeatAnimation;
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
		SkeletalActor m_CesiumMan;
		SkeletalAnimationController m_BipedController;

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CesiumManSGN;
		SGNTransformation m_CesiumManTransformSGN;

		bool m_RepeatAnimation;

	};//ExampleSkeletalAnimation

	

}

#endif