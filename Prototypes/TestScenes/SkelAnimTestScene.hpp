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
#ifndef __CFORGE_SKELANIMTESTSCENE_HPP__
#define __CFORGE_SKELANIMTESTSCENE_HPP__

#include "../../CForge/Graphics/Actors/SkeletalActor.h"
#include "../../Examples/exampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class SkelAnimTestScene : public ExampleSceneBase {
	public:
		SkelAnimTestScene(void) {

		}//Constructor

		~SkelAnimTestScene(void) {

		}//Destructor

		void init(void) {
			initWindowAndRenderDevice();
			initCameraAndLights();

			m_Cam.position(Vector3f(15.0f, 5.0f, 35.0f));
			m_Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());

			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			SAssetIO::load("MyAssets/DoubleCaptured.glb", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			// male textures
			//M.getMaterial(0)->TexAlbedo = "Assets/tmp/MHTextures/young_lightskinned_male_diffuse2.png";
			//M.getMaterial(1)->TexAlbedo = "Assets/tmp/MHTextures/brown_eye.png";
			//M.getMaterial(2)->TexAlbedo = "Assets/tmp/MHTextures/male_casualsuit04_diffuse.png";
			//M.getMaterial(3)->TexAlbedo = "Assets/tmp/MHTextures/shoes06_diffuse.png";

			// female textures
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";

			M.computePerVertexNormals();
			//M.computePerVertexTangents();
			m_ControllerCaptured.init(&M);
			m_Captured.init(&M, &m_ControllerCaptured);
			M.clear();

			SAssetIO::load("MyAssets/DoubleSynth.glb", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";
			m_ControllerSynth.init(&M);
			m_Synth.init(&M, &m_ControllerSynth);
			M.clear();

			SAssetIO::load("MyAssets/DoubleStylized.glb", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";
			m_ControllerStyle.init(&M);
			m_Style.init(&M, &m_ControllerStyle);
			M.clear();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add skeletal actor to scene graph (Eric)
			

			Vector3f ModelPos = Vector3f(0.0f, 0.0f, 0.0f);
			Vector3f Offset = Vector3f(0.0f, 0.0f, -5.0f);

			float Sc = 0.05f;

			Quaternionf Rot;
			Rot = Quaternionf::Identity();


			m_CapturedTransformSGN.init(&m_RootSGN, ModelPos + 2 * Offset, Rot, Vector3f(Sc, Sc, Sc));
			m_CapturedSGN.init(&m_CapturedTransformSGN, &m_Captured);

			m_StyleTransformSGN.init(&m_RootSGN, ModelPos + Offset, Rot, Vector3f(Sc, Sc, Sc));
			m_StyleSGN.init(&m_StyleTransformSGN, &m_Style);

			m_SynthTransformSGN.init(&m_RootSGN, ModelPos, Rot, Vector3f(Sc, Sc, Sc));
			m_SynthSGN.init(&m_SynthTransformSGN, &m_Synth);

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
		}//clear

		void run(void) {
			SkeletalAnimationController::Animation* pAnimFront = nullptr;
			SkeletalAnimationController::Animation* pAnimBack = nullptr;

			bool RepeatAnim = false;
			float LastAnimSpeed = 16.666f;

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// this will progress all active skeletal animations for this controller
				m_ControllerCaptured.update(60.0f / m_FPS);
				m_ControllerSynth.update(60.0f / m_FPS);
				m_ControllerStyle.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 16.666f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed /= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, AnimationSpeed, 0.0f));
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, AnimationSpeed, 0.0f));
					m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, AnimationSpeed, 0.0f));
					LastAnimSpeed = AnimationSpeed;
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					RepeatAnim = !RepeatAnim;
				}

				if (RepeatAnim && (m_Style.activeAnimation() == nullptr || m_Captured.activeAnimation() == nullptr || m_Synth.activeAnimation() == nullptr)) {
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, LastAnimSpeed, 0.0f));
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, LastAnimSpeed, 0.0f));
					m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, LastAnimSpeed, 0.0f));
				}


				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
					bool En;
					m_SynthSGN.enabled(nullptr, &En);
					m_SynthSGN.enable(true, !En);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
					bool En;
					m_StyleSGN.enabled(nullptr, &En);
					m_StyleSGN.enable(true, !En);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
					bool En;
					m_CapturedSGN.enabled(nullptr, &En);
					m_CapturedSGN.enable(true, !En);
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {

				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_7, true)) {

				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_8, true)) {

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
		SkeletalActor m_Captured;
		SkeletalActor m_Synth;
		SkeletalActor m_Style;
		SkeletalAnimationController m_ControllerCaptured;
		SkeletalAnimationController m_ControllerSynth;
		SkeletalAnimationController m_ControllerStyle;

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CapturedSGN;
		SGNGeometry m_StyleSGN;
		SGNGeometry m_SynthSGN;
		SGNTransformation m_CapturedTransformSGN;
		SGNTransformation m_StyleTransformSGN;
		SGNTransformation m_SynthTransformSGN;
	};//SkelAnimTestScene

	void skelAnimTestScene(void) {

		SkelAnimTestScene Scene;
		Scene.init();
		Scene.run();
		Scene.clear();

	}//exampleMinimumGraphicsSetup

}

#endif