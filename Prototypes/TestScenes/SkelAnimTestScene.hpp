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

// chose only one!
#define ART_HUMAN_2_TEST
//#define MAKE_HUMAN_TEST

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
			T3DMesh<float> AnimData;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			// 0 is Blender exported models
			// 1 is Blender exported models but switched animation data
			// 2 is Models with applied animation data and recomputed inverse bind pose matrices
			// 3 is models with transferred animation data rotations only, than recompute inverse rest pose matrix
			// 4 is models with transferred animations data rotations only, that recomputed inverse rest pose matrix and scaled movement speed
#ifdef MAKE_HUMAN_TEST
			const uint32_t Presentation = 4;

			

			float Model1YOffset = 0.0f;
			float Model2YOffset = 0.0f;

			// Initialize model with new skeleton definition
			Vector4f ModelHeightOrig[3];
			Vector4f ModelHeightNew[3];

			for (uint8_t i = 0; i < 3; ++i) {
				ModelHeightOrig[i] = Eigen::Vector4f::Zero();
				ModelHeightNew[i] = Eigen::Vector4f::Zero();
			}

			float ModelYOffset = 0.0f;

			SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &M);
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

			switch (Presentation) {
			case 0: {
				// actually nothing to do here
			}break;
			case 1: {
				SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &AnimData);
				M.clearSkeletalAnimations();
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				AnimData.clear();
			}break;
			case 2: {
				SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &AnimData);

				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);
				M.clearSkeletalAnimations();
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);
				recomputeInverseBindPoseMatrix(&M, false);
				AnimData.clear();

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model1YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();

			}break;
			case 3: {
				SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &AnimData);		
				T3DMesh<float>::SkeletalAnimation SkelAnim;
				mergeAnimationData(M.getSkeletalAnimation(0), AnimData.getSkeletalAnimation(0), &SkelAnim, M.rootBone()->Name);

				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);
				M.clearSkeletalAnimations();
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);
				M.clearSkeletalAnimations();

				AnimData.clear();
				M.addSkeletalAnimation(&SkelAnim);
				
				recomputeInverseBindPoseMatrix(&M, false);

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model1YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();
			}break;
			case 4: {
				SAssetIO::load("MyAssets/Model_Comparison/Old Skeleton/ModelOldSkeleton.gltf", &AnimData);
				T3DMesh<float>::SkeletalAnimation SkelAnim;
				mergeAnimationData(M.getSkeletalAnimation(0), AnimData.getSkeletalAnimation(0), &SkelAnim, M.rootBone()->Name);

				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);
				M.clearSkeletalAnimations();
				
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);

				M.clearSkeletalAnimations();
				AnimData.clear();
				M.addSkeletalAnimation(&SkelAnim);

				recomputeInverseBindPoseMatrix(&M, false);
				float s = (ModelHeightOrig[1].w() - ModelHeightOrig[1].z()) / (ModelHeightNew[1].w() - ModelHeightNew[1].z());

				// scale movement speed
				auto* pSkelAnim = M.getSkeletalAnimation(0);
				for (auto& i : pSkelAnim->Keyframes[0]->Positions) i.z() = s * i.z();

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model1YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();

			}break;
			default: break;
			}

			m_ControllerCaptured.init(&M);
			m_Captured.init(&M, &m_ControllerCaptured);
			M.clear();


			// Initialize model with old skeleton definition	
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

			M.computePerVertexNormals();
			M.computePerVertexTangents();

			switch (Presentation) {
			case 0: {
				// actually nothing to do here
			}break;
			case 1: {
				SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &AnimData);
				M.clearSkeletalAnimations();
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				AnimData.clear();
			}break;
			case 2: {
				SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &AnimData);
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);
				M.clearSkeletalAnimations();
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);
				recomputeInverseBindPoseMatrix(&M, false);
				AnimData.clear();

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model2YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();
			}break;
			case 3: {
				SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &AnimData);
				T3DMesh<float>::SkeletalAnimation SkelAnim;
				mergeAnimationData(M.getSkeletalAnimation(0), AnimData.getSkeletalAnimation(0), &SkelAnim, "Hips");
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);
				M.clearSkeletalAnimations();

				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);
				M.clearSkeletalAnimations();

				AnimData.clear();
				M.addSkeletalAnimation(&SkelAnim);	
				recomputeInverseBindPoseMatrix(&M, false);

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model2YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();
			}break;
			case 4: {
				SAssetIO::load("MyAssets/Model_Comparison/New Skeleton/ModelNewSkeleton.gltf", &AnimData);
				T3DMesh<float>::SkeletalAnimation SkelAnim;
				mergeAnimationData(M.getSkeletalAnimation(0), AnimData.getSkeletalAnimation(0), &SkelAnim, M.rootBone()->Name);

				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightOrig, true);

				M.clearSkeletalAnimations();

				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0));
				transformSkeleton(M.rootBone(), Eigen::Matrix4f::Identity(), &M.getSkeletalAnimation(0)->Keyframes, ModelHeightNew, true);

				M.clearSkeletalAnimations();
				AnimData.clear();
				M.addSkeletalAnimation(&SkelAnim);

				recomputeInverseBindPoseMatrix(&M, false);
				float s = (ModelHeightOrig[1].w() - ModelHeightOrig[1].z()) / (ModelHeightNew[1].w() - ModelHeightNew[1].z());

				// scale movement speed
				auto* pSkelAnim = M.getSkeletalAnimation(0);
				for (auto& i : pSkelAnim->Keyframes[0]->Positions) i.z() = s * i.z();

				float OrigHeight = ModelHeightOrig[1].w() - ModelHeightOrig[1].z();
				float NewHeight = ModelHeightNew[1].w() - ModelHeightNew[1].z();
				Model2YOffset = OrigHeight - NewHeight;
				ModelHeightOrig[1] = ModelHeightNew[1] = Eigen::Vector4f::Zero();
			}break;
			default: break;
			}
	

			m_ControllerSynth.init(&M);
			m_Synth.init(&M, &m_ControllerSynth);
			M.clear();
#endif

#ifdef ART_HUMAN_2_TEST
			SAssetIO::load("MyAssets/RiggedForART2.glb", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.computePerVertexNormals();
			
			// take other animation
			// not working since the coordinate systems between rigged model and whatever ART uses do not match
			// requires further investigation how the data gets imported

			/*SAssetIO::load("MyAssets/Aufnahme2_ARTHumanV2.bvh", &AnimData);
			M.clearSkeletalAnimations();
			for (uint32_t i = 0; i < AnimData.skeletalAnimationCount(); ++i) {
				M.addSkeletalAnimation(AnimData.getSkeletalAnimation(i));
			}*/

			/*
			// Also not working
			T3DMesh<float>::SkeletalAnimation Merged;
			mergeAnimationData(M.getSkeletalAnimation(0), AnimData.getSkeletalAnimation(0), &Merged, "pelvis");

			M.clearSkeletalAnimations();
			M.addSkeletalAnimation(&Merged);
			//recomputeInverseBindPoseMatrix(&M, false);*/

			printf("Avaialable skeletal Animations: %d\n", M.skeletalAnimationCount());

			m_ControllerSynth.init(&M);
			m_Synth.init(&M, &m_ControllerSynth);
			M.clear();
			AnimData.clear();



#endif


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
#ifdef MAKE_HUMAN_TEST
			m_ComparisonModelSGN.init(&m_RootSGN, &m_ComparisonModel, Vector3f(-5.0f, 0.0f, 0.0f), Rot, Vector3f(Sc, Sc, Sc));

			Vector3f ModelPos = Vector3f(0.0f, 0.0f, 0.0f);
			Vector3f Offset = Vector3f(-2.0f, 0.0f, 0.0f);

			m_CapturedTransformSGN.init(&m_RootSGN, ModelPos + Vector3f(0.0f, Model1YOffset*Sc, 0.0f) , Rot, Vector3f(Sc, Sc, Sc));
			m_CapturedSGN.init(&m_CapturedTransformSGN, &m_Captured);

			/*m_StyleTransformSGN.init(&m_RootSGN, ModelPos + Offset, Rot, Vector3f(Sc, Sc, Sc));
			m_StyleSGN.init(&m_StyleTransformSGN, &m_Style);*/

			m_SynthTransformSGN.init(&m_RootSGN, ModelPos + 2 * Offset + Vector3f(0.0f, Model2YOffset*Sc, 0.0f), Rot, Vector3f(Sc, Sc, Sc));
			m_SynthSGN.init(&m_SynthTransformSGN, &m_Synth);

#endif

#ifdef ART_HUMAN_2_TEST
			Sc = 0.01f;
			Rot = AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX());
			Vector3f ModelPos = Vector3f(0.0f, 0.0f, 0.0f);
			Vector3f Offset = Vector3f(0.0f, 0.0f, 0.0f);
			m_SynthTransformSGN.init(&m_RootSGN, Vector3f::Zero(), Rot, Vector3f(Sc, Sc, Sc));
			m_SynthSGN.init(&m_SynthTransformSGN, &m_Synth);
#endif

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
				//m_ControllerCaptured.update(60.0f / m_FPS);
				m_ControllerSynth.update(60.0f / m_FPS);
				//m_ControllerStyle.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 16.666f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed /= 20.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					//m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, AnimationSpeed, 0.0f));
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, AnimationSpeed, 0.0f));
					//m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, AnimationSpeed, 0.0f));
					LastAnimSpeed = AnimationSpeed;
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					if (m_ControllerCaptured.animationCount() > 1) m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(1, AnimationSpeed, 0.0f));
					if (m_ControllerSynth.animationCount() > 1) m_Synth.activeAnimation(m_ControllerSynth.createAnimation(1, AnimationSpeed, 0.0f));
				}


				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
					RepeatAnim = !RepeatAnim;
				}

				if (RepeatAnim && (m_Style.activeAnimation() == nullptr || m_Captured.activeAnimation() == nullptr || m_Synth.activeAnimation() == nullptr)) {
					m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, LastAnimSpeed, 0.0f));
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, LastAnimSpeed, 0.0f));
					m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, LastAnimSpeed, 0.0f));
				}

#ifdef ART_HUMAN_2_TEST
				auto* pKeyboard = m_RenderWin.keyboard();
				if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(2, AnimationSpeed, 0.0f));
				}
				if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) {
					m_Synth.activeAnimation(m_ControllerSynth.createAnimation(3, AnimationSpeed, 0.0f));
				}

#endif

#if MAKE_HUMAN_TEST
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
#endif

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
		

		void transformSkeleton(T3DMesh<float>::Bone* pBone, Eigen::Matrix4f ParentTransform, std::vector<T3DMesh<float>::BoneKeyframes*>* pKeyframes, Eigen::Vector4f *pMinMax, bool DryRun) {
			// compute local transform
			T3DMesh<float>::BoneKeyframes* pFrame = nullptr;
			
			for (auto i : (*pKeyframes)) {
				if (i->BoneName.compare(pBone->Name) == 0) pFrame = i;

			}

			Eigen::Matrix4f JointTransform = Matrix4f::Identity();
			if (pFrame != nullptr) {
				Matrix4f R = (pFrame->Rotations.size() > 0) ? GraphicsUtility::rotationMatrix(pFrame->Rotations[0]) : Matrix4f::Identity();
				Matrix4f T = (pFrame->Positions.size() > 0) ? GraphicsUtility::translationMatrix(pFrame->Positions[0]) : Matrix4f::Identity();
				Matrix4f S = (pFrame->Scalings.size() > 0) ? GraphicsUtility::scaleMatrix(pFrame->Scalings[0]) : Matrix4f::Identity();

				R = Matrix4f::Identity();

				if (pBone->pParent == nullptr) {
					JointTransform = R * S;
				}
				else {
					JointTransform = T * R * S;
				}
			}
			
			Matrix4f LocalTransform = ParentTransform * JointTransform;
			float x = LocalTransform(0, 3);
			float y = LocalTransform(1, 3);
			float z = LocalTransform(2, 3);
			// new y values
			if (x < pMinMax[0].z()) pMinMax[0].z() = x;
			if (y < pMinMax[1].z()) pMinMax[1].z() = y;
			if (z < pMinMax[2].z()) pMinMax[2].z() = z;

			if (pBone->pParent == nullptr) {
				pMinMax[0].w() = x;
				pMinMax[1].w() = y;
				pMinMax[2].w() = z;
			}

			// old y values
			x = pBone->OffsetMatrix.inverse()(0, 3);
			y = pBone->OffsetMatrix.inverse()(1, 3);
			z = pBone->OffsetMatrix.inverse()(2, 3);

			if (x < pMinMax[0].x()) pMinMax[0].x() = x;
			if (y < pMinMax[1].x()) pMinMax[1].x() = y;
			if (z < pMinMax[2].x()) pMinMax[2].x() = z;

			if (pBone->pParent == nullptr) {
				pMinMax[0].y() = x;
				pMinMax[1].y() = y;
				pMinMax[2].y() = z;
			}

			if(!DryRun)	pBone->OffsetMatrix = LocalTransform.inverse();

			// recursion
			for (auto i : pBone->Children) transformSkeleton(i, LocalTransform, pKeyframes, pMinMax, DryRun);

		}//transformSkeleton

		void recomputeInverseBindPoseMatrix(T3DMesh<float>* pMesh, bool ScaleRootAnimation) {
			if (nullptr == pMesh->rootBone()) throw CForgeExcept("Mesh contains no root bone!");
			if (0 == pMesh->skeletalAnimationCount()) throw CForgeExcept("Mesh contains no animations!");

			auto *pRoot = pMesh->rootBone();
			auto* pAnim = pMesh->getSkeletalAnimation(0);

			Eigen::Vector4f MinMax[3] = { Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero() };
			//for(auto i: pRoot->Children) transformSkeleton(i, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, false);

			transformSkeleton(pRoot, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, false);

		
			if (ScaleRootAnimation) {
				// scale vertices of mesh
				std::vector<Eigen::Vector3f> Vertices;
				Eigen::Matrix3f ScaleMatrix = Matrix3f::Identity();
				ScaleMatrix(0, 0) = (MinMax[0].w() - MinMax[0].z()) / (MinMax[0].y() - MinMax[0].x());
				ScaleMatrix(1, 1) = (MinMax[1].w() - MinMax[1].z()) / (MinMax[1].y() - MinMax[1].x());
				ScaleMatrix(2, 2) = (MinMax[2].w() - MinMax[2].z()) / (MinMax[2].y() - MinMax[2].x());

				printf("Scale factors are %.2f %.2f %.2ff\n", ScaleMatrix(0, 0), ScaleMatrix(1, 1), ScaleMatrix(2, 2));

				for (uint32_t i = 0; i < pMesh->vertexCount(); ++i) {
					auto v = ScaleMatrix * pMesh->vertex(i);
					Vertices.push_back(v);
				}
				//pMesh->vertices(&Vertices);

				float s = (MinMax[1].y() - MinMax[1].x()) / (MinMax[1].w() - MinMax[1].z());
				ScaleMatrix(0, 0) = s;
				ScaleMatrix(1, 1) = s;
				ScaleMatrix(2, 2) = s;


				for (uint32_t i = 0; i < pMesh->skeletalAnimationCount(); ++i) {
					auto* pAnim = pMesh->getSkeletalAnimation(i);

					for (uint32_t k = 0; k < pAnim->Keyframes.size(); ++k) {
						if (pAnim->Keyframes[k]->BoneName.compare(pMesh->rootBone()->Name) == 0) {
							for (auto& j : pAnim->Keyframes[i]->Positions) j = ScaleMatrix * j;
						}
					}

				}
			}//ScaleRootAnimation

		}//recomputeInverseBindPoseMatrix


		void mergeAnimationData(T3DMesh<float>::SkeletalAnimation *pAnimOrig, T3DMesh<float>::SkeletalAnimation *pAnimNew, T3DMesh<float>::SkeletalAnimation *pMerged, std::string RootBone) {

			pMerged->Duration = pAnimNew->Duration;
			pMerged->Keyframes.clear();
			pMerged->Speed = pAnimNew->Speed;
			pMerged->Name = pAnimNew->Name;

			for (uint32_t i = 0; i < pAnimOrig->Keyframes.size(); ++i) {
				
				pMerged->Keyframes.push_back(new T3DMesh<float>::BoneKeyframes());
				pMerged->Keyframes[i]->ID = i;
				pMerged->Keyframes[i]->BoneID = pAnimOrig->Keyframes[i]->BoneID;
				pMerged->Keyframes[i]->BoneName = pAnimOrig->Keyframes[i]->BoneName;

				// we use position and scale of first keyframe (rest pose)
				Vector3f Pos = pAnimOrig->Keyframes[i]->Positions[0];
				Vector3f Scale = pAnimOrig->Keyframes[i]->Scalings[0];

				// find matching keyframe in new animation
				T3DMesh<float>::BoneKeyframes* pKeyframeNew = nullptr;
				for (auto j: pAnimNew->Keyframes) {
					if (pAnimOrig->Keyframes[i]->BoneName.compare(j->BoneName) == 0) {
						pKeyframeNew = j;
						break;
					}
				}

				if (nullptr == pKeyframeNew) {
					printf("Did not find a valid match for bone %s\n", pMerged->Keyframes[i]->BoneName.c_str());
					continue;
				}
		
				for (uint32_t k = 0; k < pKeyframeNew->Rotations.size(); ++k) {
					pMerged->Keyframes[i]->Positions.push_back(Pos);
					pMerged->Keyframes[i]->Scalings.push_back(Scale);
					pMerged->Keyframes[i]->Rotations.push_back(pKeyframeNew->Rotations[k]);
					pMerged->Keyframes[i]->Timestamps.push_back(k); // pKeyframeNew->Timestamps[k]);
					
				}//for[rotation data]

				// copy root bone data
				if (pMerged->Keyframes[i]->BoneName.compare(RootBone) == 0) {
					pMerged->Keyframes[i]->Positions = pKeyframeNew->Positions;
				}	
			}//for[all keyframes]

		}//copyRotationData

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