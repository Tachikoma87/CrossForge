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
#ifndef __CFORGE_SHAPEDEFORMERTESTSCENE_HPP
#define __CFORGE_SHAPEDEFORMERTESTSCENE_HPP

#include <CForge/Graphics/Actors/MorphTargetActor.h>
#include <CForge/MeshProcessing/Builder/MorphTargetModelBuilder.h>
#include <Examples/exampleSceneBase.hpp>
#include <glad/glad.h>
#include <igl/readOBJ.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	class ShapeDeformerTestScene : public ExampleSceneBase {
	public:
		ShapeDeformerTestScene(void) {
			m_WindowTitle = "CrossForge Test Scene - Shape Deformer";
			m_WinWidth = 1280;
			m_WinHeight = 720;

			m_ForceSmoothLighting = true;
		}//Constructor

		~ShapeDeformerTestScene(void) {

		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			initCameraAndLights();

			gladLoadGL();

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// load shape model
			// start first 1 frame, 0 is T-Pose
			M.clear();
			Eigen::MatrixXf V;
			Eigen::MatrixXi F;
			igl::readOBJ("MuscleMan1.obj", V, F);
			toCForgeMesh(&M, V, F);
			setMeshShader(&M, 0.5f, 0.04f);
			M.computePerVertexNormals();
			// build the morph targets
			buildMTModel(&M);
			// initialize morph target controller and actor
			m_MTController.init(&M);
			buildMTSequences(&m_MTController);
			m_Shape.init(&M, &m_MTController);
			M.clear();

			// build scene graph			
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome		
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add cube		
			m_ShapeTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_ShapeSGN.init(&m_ShapeTransformSGN, &m_Shape);
			m_ShapeSGN.scale(Vector3f(0.01f, 0.01f, 0.01f));

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) {
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void run(void) {

			MorphTargetAnimationController::ActiveAnimation* pAnim = nullptr;  

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// progres morph target animations
				m_MTController.update(60.0f / m_FPS);

				Keyboard* pKeyboard = m_RenderWin.keyboard();

				float CamSpeed = 0.1f;

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), CamSpeed, 1.0f, 0.25f);
				
				// scale animation speed
				float MTAnimationSpeed = 1.0f;
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) MTAnimationSpeed = 0.5f;
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) MTAnimationSpeed = 0.25f;

				// play animation
				if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
					if (nullptr == pAnim) {
						pAnim = m_MTController.play(0, MTAnimationSpeed);
						m_Shape.addAnimation(pAnim);
					}
					else {
						pAnim->Finished = false;
						pAnim->Speed = MTAnimationSpeed;
						pAnim->t = 0.0f;
						pAnim->CurrentSquenceIndex = 0;
						pAnim->SequenceStartTimestamp = CoreUtility::timestamp();
					}
				}

				// toggle pause animation
				if (pKeyboard->keyPressed(Keyboard::KEY_P, true)) {
					if (pAnim->Speed > 0.0f) pAnim->Speed = 0.0f;
					else pAnim->Speed = MTAnimationSpeed;
				}

				// single step
				if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
					if (pAnim->Speed > 0.0f) pAnim->Speed = 0.0f;
					pAnim->CurrentSquenceIndex++;
				}
				
				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				static bool Wireframe = false;
				if (pKeyboard->keyPressed(Keyboard::KEY_F2, true)) {
					Wireframe = !Wireframe;
				}

				if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderWin.swapBuffers();

				updateFPS();

				defaultKeyboardUpdate(m_RenderWin.keyboard());
			}//while[main loop]
		}//run
	protected:

		void toCForgeMesh(T3DMesh<float>* pM, Eigen::MatrixXf V, Eigen::MatrixXi Faces) {
			std::vector<Vector3f> Vertices;
			T3DMesh<float>::Submesh* pSub = new T3DMesh<float>::Submesh();
			pSub->Material = 0;

			for (uint32_t i = 0; i < V.rows(); i++) {
				Vector3f v = Vector3f(V(i, 0), V(i, 1), V(i, 2));
				Vertices.push_back(v);
			}

			for (uint32_t i = 0; i < Faces.rows(); ++i) {
				T3DMesh<float>::Face F;
				F.Vertices[0] = Faces(i, 0);
				F.Vertices[1] = Faces(i, 1);
				F.Vertices[2] = Faces(i, 2);
				pSub->Faces.push_back(F);
			}

			T3DMesh<float>::Material Mat;
			Mat.Color = Vector4f(0.7f, 0.7f, 0.7f, 1.0f);
			Mat.ID = 0;
			Mat.Metallic = 0.04f;
			Mat.Roughness = 0.1f
				;
			pM->addMaterial(&Mat, true);
			pM->addSubmesh(pSub, false);
			pM->vertices(&Vertices);
		}

		void buildMTModel(T3DMesh<float>* pBaseMesh) {
			if (nullptr == pBaseMesh) throw NullpointerExcept("pBaseMesh");

			printf("Building morph target model...");
			uint64_t Start = CoreUtility::timestamp();

			// create morph target build and initialize with base mesh
			MorphTargetModelBuilder MTBuilder;
			MTBuilder.init(pBaseMesh);

			// define models we want to add
			// models have to bin in full vertex correspondence (same number of vertices, each having the same meaning)
			vector<pair<string, string>> MTList;
			char Name[32];
			char Path[256];
			for (uint16_t i = 2; i < 150; ++i) {
				sprintf(Name, "Frame %d", i);
				sprintf(Path, "MuscleMan%d.obj", i);
				MTList.push_back(pair<string, string>(Name, Path));
			}//for[all frames]
			
			// load models and add to builder as targets
			T3DMesh<float> M;
			for (auto i : MTList) {
				M.clear();
				try {
					Eigen::MatrixXf V;
					Eigen::MatrixXi F;
					igl::readOBJ(i.second, V, F);

					T3DMesh<float> M;
					toCForgeMesh(&M, V, F);
		
					if (M.vertexCount() != pBaseMesh->vertexCount()) {
						printf("Model %s has different vertex count than base mesh: %d vs %d\n", i.second.c_str(), M.vertexCount(), pBaseMesh->vertexCount());
					}
					else {
						M.computePerVertexNormals();
						MTBuilder.addTarget(&M, i.first);
					}
					
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
				}
			}//for[all files]

			// build morph targets and retrieve them
			MTBuilder.build();
			MTBuilder.retrieveMorphTargets(pBaseMesh);

			printf(" finished int %d ms\n", uint32_t(CoreUtility::timestamp() - Start));
		}//buildMTModel

		void buildMTSequences(MorphTargetAnimationController* pController) {
			if (nullptr == pController) throw NullpointerExcept("pController");

			MorphTargetAnimationController::AnimationSequence Seq;

			// for every morph target we create a sequence
			Seq.clear();
			float Dur = 1.0f/60.0f; // every target visible for 1/60th of a frame
			float t = 0.0f;
			
			// first sequence activates targets
			for (uint32_t i = 0; i < pController->morphTargetCount(); ++i) {
				MorphTargetAnimationController::MorphTarget* pMT = pController->morphTarget(i);
				Seq.Targets.push_back(pMT->ID);
				Seq.Parameters.push_back(Vector3f(1.0f, 1.0f, Dur));
				Seq.Name = pMT->Name;		
			}//for[all morph targets
			pController->addAnimationSequence(&Seq);

		}//buildMTSequences

		bool m_ForceSmoothLighting;

		StaticActor m_Skydome;
		MorphTargetActor m_Shape;
		MorphTargetAnimationController m_MTController;

		// scene graph nodes
		SGNGeometry m_ShapeSGN;
		SGNTransformation m_ShapeTransformSGN;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;

	};//ExampleMorphTargetAnimation

	void shapeDeformerTest(void) {

		ShapeDeformerTestScene Ex;
		Ex.init();
		Ex.run();
		Ex.clear();

	}//exampleMorphTargetAnimation

}

#endif