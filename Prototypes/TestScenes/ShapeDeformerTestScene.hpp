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

		void init(int32_t startFrame, int32_t endFrame) {

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

			//std::string filepath("Assets/tmp/FBXsamplePoints1000.txt"); //for MuscleMan
			std::string filepath("Assets/tmp/EveSamplePoints1000.txt");
			int SamplePoints;
			int numSamplePoints;
			std::ifstream myfile(filepath);

			if (!myfile.is_open()) {
				std::cout << "Unable to open file\n";
			}
			else {
				myfile >> numSamplePoints;
				for (int i = 0; i < numSamplePoints; i++)
				{
					myfile >> SamplePoints;
					ControlPoints.push_back(SamplePoints);
					SGNGeometry temp1;
					SGNTransformation temp2;
					SGNTransformation temp3;

					markerSGN1.push_back(temp2);
					markerSGN2.push_back(temp3);
					markerSphere.push_back(temp1);
				}

				myfile.close();

			}

			// load shape model
			// start first 1 frame, 0 is T-Pose
			M.clear();
			Eigen::MatrixXf V;
			Eigen::MatrixXi F;
			Eigen::MatrixXf V2;
			Eigen::MatrixXi F2;
			
			//igl::readOBJ("Assets/Animations/MuscleMan/OriginalMuscleMan" + std::to_string(startFrame) + ".obj", V2, F2);
			//igl::readOBJ("Assets/Animations/MuscleManBest/MuscleManBest" + std::to_string(startFrame) + ".obj", V2, F2);
			//igl::readOBJ("Assets/Animations/MuscleManBest/MuscleManBestDeformed" + std::to_string(startFrame) + ".obj", V, F);
			
			//igl::readOBJ("Assets/Animations/Eve/OriginalEve" + std::to_string(startFrame) + ".obj", V2, F2);
			igl::readOBJ("Assets/Animations/EveBest/EveBest" + std::to_string(startFrame) + ".obj", V2, F2);
			igl::readOBJ("Assets/Animations/EveBest/EveBestDeformed" + std::to_string(startFrame) + ".obj", V, F);
			
			toCForgeMesh(&m_ModelData, V, F);
			toCForgeMesh(&m_ModelDataOriginal, V2, F2);
			setMeshShader(&m_ModelData, 0.5f, 0.04f);
			setMeshShader(&m_ModelDataOriginal, 0.5f, 0.04f);
			m_ModelData.computePerVertexNormals();
			m_ModelDataOriginal.computePerVertexNormals();
			// build the morph targets
			//buildMTModel(&m_ModelData, "MuscleManBestDeformed", startFrame, endFrame);
			//buildMTModel(&m_ModelDataOriginal, "MuscleManBest", startFrame, endFrame);
			//buildMTModel(&m_ModelDataOriginal, "OriginalMuscleMan", startFrame, endFrame);
			
			buildMTModel(&m_ModelData, "EveBestDeformed", startFrame, endFrame);
			buildMTModel(&m_ModelDataOriginal, "EveBest", startFrame, endFrame);
			//buildMTModel(&m_ModelDataOriginal, "OriginalEve", startFrame, endFrame);

			// initialize morph target controller and actor
			m_MTController.init(&m_ModelData);
			buildMTSequences(&m_MTController);
			m_Shape.init(&m_ModelData, &m_MTController);

			m_MTControllerOriginal.init(&m_ModelDataOriginal);
			buildMTSequences(&m_MTControllerOriginal);
			m_ShapeOriginal.init(&m_ModelDataOriginal, &m_MTControllerOriginal);
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
			m_ShapeSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			m_ShapeTransformSGNOriginal.init(&m_RootSGN, Vector3f(-0.5f, 3.0f, 0.0f));
			m_ShapeSGNOriginal.init(&m_ShapeTransformSGNOriginal, &m_ShapeOriginal);
			m_ShapeSGNOriginal.scale(Vector3f(0.01f, 0.01f, 0.01f));
			m_ShapeSGNOriginal.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			// prepare node to add markers		
			m_MarkerGroupSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f)); // markers will always belong to this object for the purpose of this demo
			m_MarkerGroupSGN.rotation(Eigen::Quaternionf(-0.7071068, 0.7071068, 0, 0));

			SAssetIO::load("Assets/tmp/Sphere.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			for (uint32_t i = 0; i < M.materialCount(); ++i) M.getMaterial(i)->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f); // red spheres
			m_Sphere.init(&M);
			M.clear();

			int current = 0;
			T3DMesh<float>::MorphTarget* current_MorphTarget = m_ModelData.getMorphTarget(0);
			for (int i = 0; i < ControlPoints.size(); i++) {
				int32_t vert = ControlPoints.at(i);
				if (vert == current_MorphTarget->VertexIDs.at(current)) {
					Eigen::Vector3f position = (m_ModelData.vertex(vert) + current_MorphTarget->VertexOffsets.at(current)) / 100.0f;
					markerSGN1.at(i).init(&m_MarkerGroupSGN, position.cwiseProduct(m_ShapeTransformSGN.scale()));
					current++;
				}
				else {
					markerSGN1.at(i).init(&m_MarkerGroupSGN, (m_ModelData.vertex(vert) / 100.0f).cwiseProduct(m_ShapeTransformSGN.scale()));
				}
				markerSGN2.at(i).init(&markerSGN1.at(i), Vector3f::Zero(), m_ShapeTransformSGN.rotation(), m_ShapeTransformSGN.scale(), Vector3f::Zero(), m_ShapeTransformSGN.rotationDelta());
				markerSphere.at(i).init(&markerSGN2.at(i), &m_Sphere, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(0.003f, 0.003f, 0.003f));
			}


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
			ControlPoints.clear();
			markerSGN1.clear();
			markerSGN2.clear();
			markerSphere.clear();
			m_ModelData.clear();
			m_ModelDataOriginal.clear();
		}//clear

		void run(void) {

			MorphTargetAnimationController::ActiveAnimation* pAnim = nullptr;  
			MorphTargetAnimationController::ActiveAnimation* pAnim2 = nullptr;
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// progres morph target animations
				m_MTController.update(60.0f / m_FPS);
				m_MTControllerOriginal.update(60.0f / m_FPS);

				Keyboard* pKeyboard = m_RenderWin.keyboard();

				float CamSpeed = 0.1f;

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), CamSpeed, 1.0f, 0.25f);
				
				// scale animation speed
				float MTAnimationSpeed = 1.0f;
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) {
					MTAnimationSpeed = 0.5f;
				}
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) {
					MTAnimationSpeed = 0.25f;
				}

				// play animation
				if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
					if (nullptr == pAnim) {
						pAnim = m_MTController.play(0, 0);
						m_Shape.addAnimation(pAnim);

						pAnim2 = m_MTControllerOriginal.play(0, 0);
						m_ShapeOriginal.addAnimation(pAnim2);
					}
					else {
						pAnim->Finished = false;
						pAnim->Speed = MTAnimationSpeed;
						pAnim->t = 0.0f;
						pAnim->CurrentSquenceIndex = 0;
						pAnim->SequenceStartTimestamp = CoreUtility::timestamp();
						
						pAnim2->Finished = false;
						pAnim2->Speed = MTAnimationSpeed;
						pAnim2->t = 0.0f;
						pAnim2->CurrentSquenceIndex = 0;
						pAnim2->SequenceStartTimestamp = CoreUtility::timestamp();
					}
				}

				// toggle pause animation
				if (pKeyboard->keyPressed(Keyboard::KEY_P, true)) {
					if (pAnim != nullptr) {
						if (pAnim->Speed > 0.0f) pAnim->Speed = 0.0f;
						else pAnim->Speed = MTAnimationSpeed;
						if (pAnim2->Speed > 0.0f) pAnim2->Speed = 0.0f;
						else pAnim2->Speed = MTAnimationSpeed;
					}
					
				}

				// single step
				if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
					if (nullptr == pAnim) {
						pAnim = m_MTController.play(0, 0.0f);
						m_Shape.addAnimation(pAnim);

						pAnim2 = m_MTControllerOriginal.play(0, 0.0f);
						m_ShapeOriginal.addAnimation(pAnim2);
					}
					if (pAnim->Speed > 0.0f) {
						pAnim->Speed = 0.0f;
						pAnim2->Speed = 0.0f;
					}
					
					pAnim->CurrentSquenceIndex++;
					pAnim2->CurrentSquenceIndex++;
					if (pAnim->CurrentSquenceIndex > m_ModelData.morphTargetCount() - 1) {
						pAnim->CurrentSquenceIndex = 0;
						pAnim2->CurrentSquenceIndex = 0;
					}

					if (pAnim != nullptr) {
						T3DMesh<float>::MorphTarget* current_MorphTarget = m_ModelData.getMorphTarget(pAnim->CurrentSquenceIndex);
						int current = 0;
						for (int i = 0; i < current_MorphTarget->VertexIDs.size(); i++) {
							int32_t vert = current_MorphTarget->VertexIDs.at(i);
							if (current < ControlPoints.size()) {
								if (vert == ControlPoints.at(current)) {
									Eigen::Vector3f position = (current_MorphTarget->VertexOffsets.at(i) + m_ModelData.vertex(vert)) / 100.0f;
									markerSGN1.at(current).translation(position);
									current++;
								}
							}

						}
					}
				}

				// single step
				if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
					if (nullptr == pAnim) {
						pAnim = m_MTController.play(0, 0.0f);
						m_Shape.addAnimation(pAnim);
						pAnim2 = m_MTControllerOriginal.play(0, 0.0f);
						m_ShapeOriginal.addAnimation(pAnim2);
					}
					if (pAnim->Speed > 0.0f) {
						pAnim->Speed = 0.0f;
						pAnim2->Speed = 0.0f;
					}
					pAnim->CurrentSquenceIndex--;
					pAnim2->CurrentSquenceIndex--;
					if (pAnim->CurrentSquenceIndex < 0) {
						pAnim->CurrentSquenceIndex = m_ModelData.morphTargetCount() - 1;
						pAnim2->CurrentSquenceIndex = m_ModelData.morphTargetCount() - 1;
					}
					if (pAnim != nullptr) {
						T3DMesh<float>::MorphTarget* current_MorphTarget = m_ModelData.getMorphTarget(pAnim->CurrentSquenceIndex);
						int current = 0;
						for (int i = 0; i < current_MorphTarget->VertexIDs.size(); i++) {
							int32_t vert = current_MorphTarget->VertexIDs.at(i);
							if (current < ControlPoints.size()) {
								if (vert == ControlPoints.at(current)) {
									Eigen::Vector3f position = (current_MorphTarget->VertexOffsets.at(i) + m_ModelData.vertex(vert)) / 100.0f;
									markerSGN1.at(current).translation(position);
									current++;
								}
							}

						}
					}
					
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
			Mat.Roughness = 0.1f;
			pM->addMaterial(&Mat, true);
			pM->addSubmesh(pSub, false);
			pM->vertices(&Vertices);
		}

		void buildMTModel(T3DMesh<float>* pBaseMesh, string modelName, int32_t startFrame, int32_t endFrame) {
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

			if (startFrame == 0) {
				startFrame = 2;
			}
			else {
				startFrame = startFrame + 1;
			}

			

			for (uint16_t i = startFrame; i < endFrame + 1; ++i) {
				sprintf(Name, "Frame %d", i);

				//Eve

				if (modelName.length() > 14) {
					sprintf(Path, "Assets/Animations/EveBest/EveBestDeformed%d.obj", i);
				}
				else if (modelName.length() > 9) {
					sprintf(Path, "Assets/Animations/Eve/OriginalEve%d.obj", i);
				}
				else {
					sprintf(Path, "Assets/Animations/EveBest/EveBest%d.obj", i);
				}


				//MuscleMan
				/*
				if (modelName.length() > 19) {
					sprintf(Path, "Assets/Animations/MuscleManBest/MuscleManBestDeformed%d.obj", i);
				}
				else if (modelName.length() > 14) {
					sprintf(Path, "Assets/Animations/MuscleMan/OriginalMuscleMan%d.obj", i);
				}
				else {
					sprintf(Path, "Assets/Animations/MuscleManBest/MuscleManBest%d.obj", i);
				}*/

				
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
		MorphTargetActor m_ShapeOriginal;
		MorphTargetAnimationController m_MTControllerOriginal;
		// scene graph nodes
		SGNGeometry m_ShapeSGN;
		SGNTransformation m_ShapeTransformSGN;
		SGNGeometry m_ShapeSGNOriginal;
		SGNTransformation m_ShapeTransformSGNOriginal;
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;

		SGNTransformation m_MarkerGroupSGN;
		std::vector<int32_t> ControlPoints;
		std::vector<SGNTransformation> markerSGN1;
		std::vector<SGNTransformation> markerSGN2;
		std::vector<SGNGeometry> markerSphere;
		T3DMesh<float> m_ModelData;
		T3DMesh<float> m_ModelDataOriginal;
		StaticActor m_Sphere;


	};//ExampleMorphTargetAnimation

	void shapeDeformerTest(int32_t startFrame, int32_t endFrame) {

		ShapeDeformerTestScene Ex;
		Ex.init(startFrame, endFrame);
		Ex.run();
		Ex.clear();

	}//exampleMorphTargetAnimation

}

#endif