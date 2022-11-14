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

		uint32_t getMatchingVertex(uint32_t RedundantVertexID, std::vector<std::pair<uint32_t, uint32_t>> *pRedundantVertices) {
			uint32_t Rval = 0;

			for (auto i : (*pRedundantVertices)) {
				if (i.second == RedundantVertexID) {
					Rval = i.first;
					break;
				}
			}
			return Rval;
		}//getMatchingVertex

		void mergeRedundantVertices(T3DMesh<float>* pMesh) {
			float Epsilon = 0.00025f;
 
			std::vector<std::pair<uint32_t, uint32_t>> RedundantVertices;
			std::vector<bool> IsRedundant;
			std::vector <uint32_t> VertexMapping;
			for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
				IsRedundant.push_back(false);
				VertexMapping.push_back(i);
			}

			for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
				if (IsRedundant[i]) continue;
				auto v1 = pMesh->vertex(i);
				printf("Checking vertex %d/%d\r", i, pMesh->vertexCount());

				for (uint32_t k = i+1; k < pMesh->vertexCount(); ++k) {
					auto v2 = pMesh->vertex(k);

					if ((v2 - v1).dot(v2 - v1) < Epsilon) {
						RedundantVertices.push_back(std::pair<uint32_t, uint32_t>(i,k));
						IsRedundant[k] = true;
						break;
					}

				}//for[all remaining vertices]
			}//for[all vertices]

			printf("Found %d double vertices                             \n", uint32_t(RedundantVertices.size()));

			// rebuild vertices, normals, tangents
			std::vector<Eigen::Vector3f> Vertices;
			std::vector<Eigen::Vector3f> Normals;
			std::vector<Eigen::Vector3f> Tangents;


			for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
				VertexMapping[i] = Vertices.size();
				if (!IsRedundant[i]) {
					Vertices.push_back(pMesh->vertex(i));
				}
			}
			for (uint32_t i = 0; i < pMesh->normalCount(); ++i) {
				if (!IsRedundant[i]) Normals.push_back(pMesh->normal(i));
			}
			for (uint32_t i = 0; i < pMesh->tangentCount(); ++i) {
				if (!IsRedundant[i]) Tangents.push_back(pMesh->tangent(i));
			}

			// replace indices in faces
			for (uint32_t i = 0; i < pMesh->submeshCount(); ++i) {
				auto* pM = pMesh->getSubmesh(i);
				for (auto& f : pM->Faces) {
					if (IsRedundant[f.Vertices[0]]) f.Vertices[0] = getMatchingVertex(f.Vertices[0], &RedundantVertices);
					if (IsRedundant[f.Vertices[1]]) f.Vertices[1] = getMatchingVertex(f.Vertices[1], &RedundantVertices);
					if (IsRedundant[f.Vertices[2]]) f.Vertices[2] = getMatchingVertex(f.Vertices[2], &RedundantVertices);

					// and remapp
					f.Vertices[0] = VertexMapping[f.Vertices[0]];
					f.Vertices[1] = VertexMapping[f.Vertices[1]];
					f.Vertices[2] = VertexMapping[f.Vertices[2]];

				}//for[faces of submesh]
			}//for[submeshes]

			// replace vertex weights
			for (uint32_t i = 0; i < pMesh->boneCount(); ++i) {
				auto* pBone = pMesh->getBone(i);

				// collect data
				std::vector<int32_t> Influences;
				std::vector<float> Weights;

				for (uint32_t k = 0; k < pBone->VertexInfluences.size(); ++k) {
					uint32_t ID = pBone->VertexInfluences[k];
					if (IsRedundant[ID]) pBone->VertexInfluences[k] = getMatchingVertex(ID, &RedundantVertices);

					pBone->VertexInfluences[k] = VertexMapping[pBone->VertexInfluences[k]];
				}

			}//for[all bones]

			// replace mesh data
			if (Vertices.size() > 0) pMesh->vertices(&Vertices);
			if (Normals.size() > 0) pMesh->normals(&Normals);
			if (Tangents.size() > 0) pMesh->tangents(&Tangents);

		}//mergeDoubleVertices

		SkelAnimTestScene(void) {

		}//Constructor

		~SkelAnimTestScene(void) {

		}//Destructor

		void init(void) {
			initWindowAndRenderDevice();
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-15.0f, 15.0f, 25.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(30.0f, 30.0f, 0.1f, 1000.0f));
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);


			m_Cam.position(Vector3f(15.0f, 5.0f, 35.0f));
			m_Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());

			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();


			//SAssetIO::load("MyAssets/WalkingSittingEve.glb", &M);
			//SAssetIO::load("MyAssets/WalkingSittingEve.glb", &M);
			//setMeshShader(&M, 0.6f, 0.04f);
			//
			//printf("Vertex count before: %d\n", M.vertexCount());
			//mergeRedundantVertices(&M);
			//printf("Vertex count after: %d\n", M.vertexCount());

			//M.computePerVertexNormals();
			//m_ControllerCaptured.init(&M);
			//m_Captured.init(&M, &m_ControllerCaptured);

			//M.bones(nullptr);
			//m_ComparisonModel.init(&M);

			////AssetIO::store("MyAssets/TestModel_After.obj", &M);

			//M.clear();

			

			T3DMesh<float> AnimData;
			/*T3DMesh<float> M2;
			SAssetIO::load("Assets/tmp/MuscleManPosed.glb", &M2);
			M2.computePerFaceNormals();*/

			//// initialize skeletal actor (Eric) and its animation controller
			//SAssetIO::load("MyAssets/DoubleCaptured.glb", &M);
			SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &M);
			//SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/AnimationFramework.glb", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			// male textures
			/*M.getMaterial(0)->TexAlbedo = "Assets/tmp/MHTextures/young_lightskinned_male_diffuse2.png";
			M.getMaterial(1)->TexAlbedo = "Assets/tmp/MHTextures/brown_eye.png";
			M.getMaterial(2)->TexAlbedo = "Assets/tmp/MHTextures/male_casualsuit04_diffuse.png";
			M.getMaterial(3)->TexAlbedo = "Assets/tmp/MHTextures/shoes06_diffuse.png";*/

			//// female textures
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";	
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";

			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";

			M.computePerVertexNormals();
			M.computePerVertexTangents();

			SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &AnimData);
			M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
			AnimData.clear();

			//SAssetIO::load("")

			m_ControllerCaptured.init(&M);
			m_Captured.init(&M, &m_ControllerCaptured);
			M.clear();

			// load more animation data
			
			//if(M.skeletalAnimationCount() > 0) m_ControllerCaptured.addAnimationData(M.getSkeletalAnimation(0));
			SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";

			SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &AnimData);
			M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
			AnimData.clear();

			m_ControllerSynth.init(&M);
			m_Synth.init(&M, &m_ControllerSynth);
			M.clear();

			//SAssetIO::load("MyAssets/DoubleStylized.glb", &M);
			//setMeshShader(&M, 0.7f, 0.04f);
			//M.computePerVertexNormals();
			//M.computePerVertexTangents();
			//M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			//M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.png";
			//M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			//M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";
			//M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.png";
			//m_ControllerStyle.init(&M);
			//m_Style.init(&M, &m_ControllerStyle);
			//M.clear();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add skeletal actor to scene graph (Eric)
			float Sc = 0.05f;

			Quaternionf Rot;
			Rot = Quaternionf::Identity();


			m_ComparisonModelSGN.init(&m_RootSGN, &m_ComparisonModel, Vector3f(-5.0f, 0.0f, 0.0f), Rot, Vector3f(Sc, Sc, Sc));

			Vector3f ModelPos = Vector3f(0.0f, 0.0f, 0.0f);
			Vector3f Offset = Vector3f(0.0f, 0.0f, -5.0f);

			m_CapturedTransformSGN.init(&m_RootSGN, ModelPos + 2 * Offset, Rot, Vector3f(Sc, Sc, Sc));
			m_CapturedSGN.init(&m_CapturedTransformSGN, &m_Captured);

			/*m_StyleTransformSGN.init(&m_RootSGN, ModelPos + Offset, Rot, Vector3f(Sc, Sc, Sc));
			m_StyleSGN.init(&m_StyleTransformSGN, &m_Style);*/

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
					if (m_ControllerCaptured.animationCount() > 5) m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(5, AnimationSpeed, 0.0f));
					if (m_ControllerSynth.animationCount() > 5) m_Synth.activeAnimation(m_ControllerSynth.createAnimation(5, AnimationSpeed, 0.0f));
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
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(1, AnimationSpeed, 0.0f));
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_7, true)) {
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(2, AnimationSpeed, 0.0f));
				}
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_8, true)) {
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(3, AnimationSpeed, 0.0f));
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

		StaticActor m_ComparisonModel;
		SGNGeometry m_ComparisonModelSGN;

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