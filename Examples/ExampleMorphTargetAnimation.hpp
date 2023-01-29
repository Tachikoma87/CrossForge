/*****************************************************************************\
*                                                                           *
* File(s): exampleMorphTargetAnimation.hpp                                            *
*                                                                           *
* Content: Example scene that shows how to build, use, and visualize     *
*          a morph target animation.              *
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
#ifndef __CFORGE_EXAMPLEMORPHTARGETANIMATION_HPP__
#define __CFORGE_EXAMPLEMORPHTARGETANIMATION_HPP__

#include "../CForge/Graphics/Actors/MorphTargetActor.h"
#include "../CForge/MeshProcessing/Builder/MorphTargetModelBuilder.h"
#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleMorphTargetAnimation : public ExampleSceneBase {
	public:
		ExampleMorphTargetAnimation(void) {
			m_WindowTitle = "CrossForge Example - Morph Target Animation";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ExampleMorphTargetAnimation(void) {

		}//Destructor

		void init(void) override{

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M;
			
			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// load face model
			SAssetIO::load("Assets/ExampleScenes/FaceGenMale/MaleFace.obj", &M);
			setMeshShader(&M, 0.5f, 0.04f);
			M.computePerVertexNormals();
			// build the morph targets
			buildMTModel(&M);
			// initialize morph target controller and actor
			
			m_MTController.init(&M);
			buildMTSequences(&m_MTController);
			m_Face.init(&M, &m_MTController);
			M.clear();

			// build scene graph			
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube		
			m_FaceTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_FaceSGN.init(&m_FaceTransformSGN, &m_Face);
			m_FaceSGN.scale(Vector3f(0.01f, 0.01f, 0.01f));

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override{
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void run(void) override{
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// progres morph target animations
				m_MTController.update(60.0f/m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
				Keyboard* pKeyboard = m_RenderWin.keyboard();

				// if one of key 1 through 5 is pressed, animation played
				// key 0 is wildcard playing a random animation

				int32_t PlayMTAnimation = -1;
				float MTAnimationSpeed = 1.0f;
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) MTAnimationSpeed = 3.0f;
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) MTAnimationSpeed = 6.0f;

				if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) PlayMTAnimation = 0;
				if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) PlayMTAnimation = 1;
				if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) PlayMTAnimation = 2;
				if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) PlayMTAnimation = 3;
				if (pKeyboard->keyPressed(Keyboard::KEY_5, true)) PlayMTAnimation = 4;
				if (pKeyboard->keyPressed(Keyboard::KEY_6, true)) PlayMTAnimation = 5;
				if (pKeyboard->keyPressed(Keyboard::KEY_7, true)) PlayMTAnimation = 6;
				if (pKeyboard->keyPressed(Keyboard::KEY_8, true)) PlayMTAnimation = 7;
				if (pKeyboard->keyPressed(Keyboard::KEY_0, true)) PlayMTAnimation = CForgeMath::rand() % m_MTController.animationSequenceCount();


				if (-1 != PlayMTAnimation) {
					MorphTargetAnimationController::ActiveAnimation* pAnim = m_MTController.play(PlayMTAnimation, MTAnimationSpeed);
					m_Face.addAnimation(pAnim);
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

		void buildMTModel(T3DMesh<float>* pBaseMesh) {
			if (nullptr == pBaseMesh) throw NullpointerExcept("pBaseMesh");

			printf("Building morph target model...");
			uint64_t Start = CForgeUtility::timestamp();

			// create morph target build and initialize with base mesh
			MorphTargetModelBuilder MTBuilder;
			MTBuilder.init(pBaseMesh);

			// define models we want to add
			// models have to bin in full vertex correspondence (same number of vertices, each having the same meaning)
			vector<pair<string, string>> MTList;
			MTList.push_back(pair("Anger", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionAnger.obj"));
			MTList.push_back(pair("ChinRaised", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionChinRaised.obj"));
			MTList.push_back(pair("Disgust", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionDisgust.obj"));
			MTList.push_back(pair("Fear", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionFear.obj"));
			MTList.push_back(pair("Frown", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionFrown.obj"));
			MTList.push_back(pair("Kiss", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionKiss.obj"));
			MTList.push_back(pair("PuffCheeks", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionPuffCheeks.obj"));
			MTList.push_back(pair("Surprise", "Assets/ExampleScenes/FaceGenMale/MaleFace_ExpressionSurprise.obj"));

			// load models and add to builder as targets
			T3DMesh<float> M;
			for (auto i : MTList) {
				M.clear();
				try {
					AssetIO::load(i.second, &M);
					MTBuilder.addTarget(&M, i.first);
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
				}
			}//for[all files]

			// build morph targets and retrieve them
			MTBuilder.build();
			MTBuilder.retrieveMorphTargets(pBaseMesh);

			printf(" finished int %d ms\n", uint32_t(CForgeUtility::timestamp() - Start));
		}//buildMTModel

		void buildMTSequences(MorphTargetAnimationController* pController) {
			if (nullptr == pController) throw NullpointerExcept("pController");

			MorphTargetAnimationController::AnimationSequence Seq;

			// for every morph target we create a sequence
			// 1.25 seconds to activate the target, 0.5 seconds hold, and 1.25 seconds to go back to base mesh
			for (uint32_t i = 0; i < pController->morphTargetCount(); ++i) {
				Seq.clear();
				MorphTargetAnimationController::MorphTarget* pMT = pController->morphTarget(i);
				Seq.Name = pMT->Name;
				Seq.Targets.push_back(pMT->ID);
				Seq.Targets.push_back(pMT->ID);
				Seq.Targets.push_back(pMT->ID);
				Seq.Parameters.push_back(Vector3f(0.0f, 1.0f, 1.25f));
				Seq.Parameters.push_back(Vector3f(1.0f, 1.0f, 0.5f));
				Seq.Parameters.push_back(Vector3f(1.0f, 0.0f, 1.25f));
				pController->addAnimationSequence(&Seq);
			}//for[all morph targets

		}//buildMTSequences

		StaticActor m_Skydome;
		MorphTargetActor m_Face;
		MorphTargetAnimationController m_MTController;

		// scene graph nodes
		SGNGeometry m_FaceSGN;
		SGNTransformation m_FaceTransformSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;

	};//ExampleMorphTargetAnimation

	

}

#endif