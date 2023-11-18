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
#ifndef __CFORGE_EXAMPLEBIRDSKELETALTESTANIMATION_HPP__
#define __CFORGE_EXAMPLEBIRDSKELETALTESTANIMATION_HPP__

#include "../crossforge/Graphics/Actors/SkeletalActor.h"
#include "../crossforge/Graphics/Actors/SkyboxActor.h"
#include "../crossforge/MeshProcessing/PrimitiveShapeFactory.h"

#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleBirdSkeletalTestAnimation : public ExampleSceneBase {
	public:
		ExampleBirdSkeletalTestAnimation(void) {
			m_WindowTitle = "CrossForge Example - Skeletal Animation";
		}//Constructor

		~ExampleBirdSkeletalTestAnimation(void) {
			clear();
		}//Destructor

		void init(void) override{

			initWindowAndRenderDevice();
			initCameraAndLights();
			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);
			initFPSLabel();
			initSkybox();

			// load skydome
			T3DMesh<float> M;
			// initialize skeletal actor (Eric) and its animation controller
			//SAssetIO::load("Assets/ExampleScenes/CesiumMan/CesiumMan.gltf", &M); 
			
			//WorkingAnimationButCannotEditOrImportInBlenderSinceItWillNotWorkAfterwards
			//SAssetIO::load("MyAssets/Crow_Animated/NotWorkingInFramework/CrowSimple/Crow.gltf", &M);
	
			//WORKING EAGLE
			SAssetIO::load("MyAssets/Eagle_Animated/EagleFlapSym/EagleFlap.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_BipedController.init(&M);
			m_CesiumMan.init(&M, &m_BipedController);
			M.clear();

			//SAssetIO::load("MyAssets/Eagle_Animated/EagleFallFinal/EagleFall.gltf", &M);
			SAssetIO::load("MyAssets/Eagle_Animated/EagleFallSymHalf/EagleFall.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			//m_BipedController.init(&M);
			M.computePerVertexNormals();
			//m_EagleFall.init(&M, &m_BipedController);
			m_BipedController.addAnimationData(M.getSkeletalAnimation(0)),
			M.clear();

			// add skeletal actor to scene graph (Eric)			 
			m_CesiumManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 1.0f, 0.0f));
			m_CesiumManOffsetSGN.init(&m_CesiumManTransformSGN, Vector3f(0.0f, 0.0f, -4.0f));
			m_CesiumManSGN.init(&m_CesiumManOffsetSGN, &m_CesiumMan);
			m_CesiumManSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			//m_EagleFallSGN.init(&m_CesiumManOffsetSGN, &m_EagleFall);
			//m_EagleFallSGN.scale(Vector3f(3.0f, 3.0f, 3.0f));
			//m_EagleFallSGN.enable(true, true);
			
			Quaternionf Rot;
			Rot = AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX());
			//m_CesiumManSGN.rotation(Rot);

			LineOfText* pKeybindings = new LineOfText();
			LineOfText* pAnimationControls = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement:(Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			pAnimationControls->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "1: Start Animation | Shift + 1: Fast Animation | Ctrl + 1: Slow Animation | R: Toggle Repeat Animation | 2: Toggle Surface Model/Stick-Figure");
			m_HelpTexts.push_back(pKeybindings);
			m_HelpTexts.push_back(pAnimationControls);
			pKeybindings->color(0.0f, 0.0f, 0.0f, 1.0f);
			pAnimationControls->color(0.0f, 0.0f, 0.0f, 1.0f);
			m_DrawHelpTexts = true;

			// check whether a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			m_RepeatAnimation = false;

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_8, true));
		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			bool isKey1Pressed = false;
			bool isKey2Pressed = false;
			bool isKey3Pressed = false;
			bool isKey4Pressed = false;
			bool isKey5Pressed = false;
			bool isKey6Pressed = false;
			bool isKey7Pressed = false;
			// this will progress all active skeletal animations for this controller
			m_BipedController.update(60.0f / m_FPS);
			if (m_RepeatAnimation && nullptr != m_CesiumMan.activeAnimation()) {
				auto* pAnim = m_CesiumMan.activeAnimation();
				if (pAnim->t >= pAnim->Duration) pAnim->t -= pAnim->Duration;
			}
			/*if (nullptr != m_CesiumMan.activeAnimation()) {
				//Quaternionf Q;
				if (m_CesiumMan.activeAnimation()->AnimationID == 1 && m_CesiumMan.activeAnimation()->t > m_CesiumMan.activeAnimation()->Duration - 1.0f)
				{
					if (m_CesiumMan.activeAnimation()->Speed != 0.0f) {
						//m_CesiumMan.pauseActiveAnimation();
						//m_CesiumMan.reverseActiveAnimation();
					}
					m_CesiumMan.reverseActiveAnimation();
				}
				
				//float Deg = m_CesiumMan.activeAnimation()->Speed / 50.0f;
				//Q = AngleAxisf(CForgeMath::degToRad(-Deg), Vector3f::UnitY());
				//m_CesiumManTransformSGN.rotation(Q * m_CesiumManTransformSGN.rotation());
			}
			*/

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			//bool isKey6Pressed = false;
			// if user hits key 1, animation will be played
			// if user also presses shift, animation speed is doubled
			float AnimationSpeed = 1000 / 60;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed *= 0.25f;

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				isKey1Pressed = true;
				SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
				m_CesiumMan.activeAnimation(pAnim);
				
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2)) {
				isKey2Pressed = true;
				SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(1, AnimationSpeed, 0.0f);
				m_CesiumMan.activeAnimation(pAnim);
				
				//m_CesiumMan.pauseActiveAnimation();
				//m_EagleFall.activeAnimation(pAnim);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5)) {
				printf("Key5");
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
				printf("Key6True");
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_7, false)) {
				printf("Key7False");
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_8)) {
				isKey2Pressed = true;
				SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(1, AnimationSpeed, 0.0f);
				m_CesiumMan.activeAnimation(pAnim);

				//m_CesiumMan.pauseActiveAnimation();
				//m_EagleFall.activeAnimation(pAnim);
			}
			if (m_RenderWin.keyboard()->keyReleased(Keyboard::KEY_8, true)) {
				
				printf("Key8");
				m_CesiumMan.reverseActiveAnimation();
				//cout << "Key6COUT " << endl; 
				
			}
			
			
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F1, true)) {
				m_CesiumMan.pauseActiveAnimation();
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				m_CesiumMan.resumeActiveAnimation();
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F3, true)) {
				m_CesiumMan.reverseActiveAnimation();
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_R, true)) {
				m_RepeatAnimation = !m_RepeatAnimation;
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_E, true)) {
				bool Enabled = true;
				m_CesiumManSGN.enabled(nullptr, &Enabled);
				if (Enabled) {
					m_CesiumManSGN.enable(true, false);
					//m_EagleFallSGN.enable(true, true);
				}
				else {
					m_CesiumManSGN.enable(true, true);
					//m_EagleFallSGN.enable(true, false);
				}
			}

			if (nullptr != m_CesiumMan.activeAnimation()) {
				//Quaternionf Q;
				if (m_CesiumMan.activeAnimation()->AnimationID == 1 && m_CesiumMan.activeAnimation()->t > m_CesiumMan.activeAnimation()->Duration - 15.0f)
				{
					if (m_CesiumMan.activeAnimation()->Speed != 0.0f) {
						//m_CesiumMan.pauseActiveAnimation();
						//m_CesiumMan.reverseActiveAnimation();
					}
					//m_CesiumMan.reverseActiveAnimation();
					m_CesiumMan.pauseActiveAnimation();

				}
				
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
					m_CesiumMan.reverseActiveAnimation();
				}
				if (m_RenderWin.keyboard()->keyReleased(Keyboard::KEY_8, true)) {

					
					m_CesiumMan.reverseActiveAnimation();
					//cout << "Key6COUT " << endl; 

				}

				//float Deg = m_CesiumMan.activeAnimation()->Speed / 50.0f;
				//Q = AngleAxisf(CForgeMath::degToRad(-Deg), Vector3f::UnitY());
				//m_CesiumManTransformSGN.rotation(Q * m_CesiumManTransformSGN.rotation());
			}

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

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
		StaticActor m_Skydome;
		SkeletalActor m_CesiumMan;
		SkeletalActor m_EagleFall;
		SkeletalAnimationController m_BipedController;

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CesiumManSGN;
		SGNGeometry m_EagleFallSGN;
		SGNTransformation m_CesiumManTransformSGN;
		SGNTransformation m_CesiumManOffsetSGN;

		bool m_RepeatAnimation;
		


	};//ExampleBirdSkeletalTestAnimation

	

}

#endif