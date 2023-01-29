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
#include <CForge/Graphics/Actors/StickFigureActor.h>
#include "../../Examples/ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

// chose only one!
//#define ART_HUMAN_2_TEST
//#define MAKE_HUMAN_TEST
#define TECHNIQUE_EVALUATION

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
			m_Pause = false;
			m_ScreenshotExtension = "jpg";
		}//Constructor

		~SkelAnimTestScene(void) {

		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key light) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-15.0f, 15.0f, 25.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
			m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			//m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(30.0f, 30.0f, 0.1f, 1000.0f));
			m_Sun.initShadowCasting(1024, 1024, Vector2i(30, 30), 01.f, 1000.0f);
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
	
#ifdef MAKE_HUMAN_TEST
			// 0 is Blender exported models
			// 1 is Blender exported models but switched animation data
			// 2 is Models with applied animation data and recomputed inverse bind pose matrices
			// 3 is models with transferred animation data rotations only, than recompute inverse rest pose matrix
			// 4 is models with transferred animations data rotations only, than recomputed inverse rest pose matrix and scaled movement speed
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
			case 5: {

			}break;
			case 6: {

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

#ifdef TECHNIQUE_EVALUATION
			AssetIO::load("MyAssets/Technique_Evaluation/OldModel.gltf", &M);
			
			// mesh 1 (captured mesh)
			setMeshShader(&M, 0.6f, 0.04f);
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";

			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";

			M.computePerVertexNormals();
			M.computePerVertexTangents();
			//recomputeInverseBindPoseMatrix(&M, false);
			
			m_ControllerCaptured.init(&M);
			m_Captured.init(&M, &m_ControllerCaptured);
			m_CapturedStick.init(&M, &m_ControllerCaptured);

			m_CapturedMeshOrig.init(&M);
			m_CapturedMeshTransformed.init(&M);

			// Mesh2 (Modified Skeleton from Blender)
			AssetIO::load("MyAssets/Technique_Evaluation/OldModel_Modified.gltf", &M);
			setMeshShader(&M, 0.6f, 0.04f);
			M.getMaterial(0)->TexAlbedo = "MyAssets/MHTextures/young_lightskinned_female_diffuse.png";
			M.getMaterial(1)->TexAlbedo = "MyAssets/MHTextures/female_casualsuit01_diffuse.png";
			M.getMaterial(1)->TexNormal = "MyAssets/MHTextures/female_casualsuit01_normal.png";

			M.getMaterial(2)->TexAlbedo = "MyAssets/MHTextures/brown_eye.jpg";
			M.getMaterial(3)->TexAlbedo = "MyAssets/MHTextures/shoes06_diffuse.jpg";

			M.computePerVertexNormals();
			M.computePerVertexTangents();

			
			m_ControllerSynth.init(&M);
			m_Synth.init(&M, &m_ControllerSynth);
			m_SynthStick.init(&M, &m_ControllerSynth);

			m_SynthMeshOrig.init(&M);
			m_SynthMeshTransformed.init(&M);

			// Mesh3 (Modified Skeleton from Blender) and recomputed inverse bind pose matrix
			/*recomputeInverseBindPoseMatrix(&M, false, false);
			m_ControllerStyle2.init(&M);
			m_Style2.init(&M, &m_ControllerStyle2);

			m_Style2MeshOrig.init(&M);
			m_Style2MeshTransformed.init(&M);*/

			
			// Mesh 4 (modified skeleton from bvh and recomputed inverse bind pose matrix)

			AnimData.clear();
			AssetIO::load("MyAssets/Technique_Evaluation/OldGait.bvh", &AnimData);
			M.clearSkeletalAnimations();
			M.addSkeletalAnimation(AnimData.getSkeletalAnimation(0), true);
			
			// copy timeline from captured mesh
			auto* pAnim = m_CapturedMeshOrig.getSkeletalAnimation(0);
			auto* pAnimStyle = M.getSkeletalAnimation(0);
			pAnimStyle->Duration = pAnim->Duration;
			pAnimStyle->Speed = pAnim->Speed;
			for (uint32_t i = 0; i < pAnimStyle->Keyframes.size(); ++i) {		
				for (uint32_t k = 0; k < pAnimStyle->Keyframes[i]->Timestamps.size(); ++k) {
					pAnimStyle->Keyframes[i]->Timestamps[k] = (k+1) * 1000.0f/60.0f;//pAnim->Keyframes[i]->Timestamps[k];
				}
			}

			m_ControllerStyle.init(&M);
			m_Style.init(&M, &m_ControllerStyle);
			m_StyleStick.init(&M, &m_ControllerStyle);

			m_StyleMeshOrig.init(&M);
			m_StyleMeshTransformed.init(&M);

			recomputeInverseBindPoseMatrix(&M, false, true);
			m_ControllerStyle2.init(&M);
			m_Style2.init(&M, &m_ControllerStyle2);
			m_Style2Stick.init(&M, &m_ControllerStyle2);
			m_Style2MeshOrig.init(&M);
			m_Style2MeshTransformed.init(&M);


			M.clear();

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
			m_SkydomeSGN.scale(Vector3f(159.0f, 159.0f, 159.0f));

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

#ifdef TECHNIQUE_EVALUATION
			Vector3f ModelPos = Vector3f(-10.0f, 0.0f, 0.0f);
			Vector3f ModelOffset = Vector3f(6.0f, 0.0f, 0.0f);
			m_CapturedTransformSGN.init(&m_RootSGN, ModelPos - ModelOffset, Rot, Vector3f(Sc, Sc, Sc));
			m_CapturedSGN.init(&m_CapturedTransformSGN, &m_Captured);
			m_CapturedStickSGN.init(&m_CapturedTransformSGN, &m_CapturedStick);

			m_SynthTransformSGN.init(&m_RootSGN, ModelPos, Rot, Vector3f(Sc, Sc, Sc));
			m_SynthSGN.init(&m_SynthTransformSGN, &m_Synth);
			m_SynthStickSGN.init(&m_SynthTransformSGN, &m_SynthStick);

			m_StyleTransformSGN.init(&m_RootSGN, ModelPos + ModelOffset, Rot, Vector3f(Sc, Sc, Sc));
			m_StyleSGN.init(&m_StyleTransformSGN, &m_Style);
			m_StyleStickSGN.init(&m_StyleTransformSGN, &m_StyleStick);

			m_Style2TransformSGN.init(&m_RootSGN, ModelPos + 2.0f * ModelOffset, Rot, Vector3f(Sc, Sc, Sc));
			m_Style2SGN.init(&m_Style2TransformSGN, &m_Style2);
			m_Style2StickSGN.init(&m_Style2TransformSGN, &m_Style2Stick);


			m_VisualizeSkeleton = false;

			m_StyleStickSGN.enable(true, false);
			m_CapturedStickSGN.enable(true, false);
			m_SynthStickSGN.enable(true, false);
			m_Style2StickSGN.enable(true, false);

			/*m_StyleSGN.enable(true, false);
			m_CapturedSGN.enable(true, false);
			m_SynthSGN.enable(true, false);
			m_Style2SGN.enable(true, false);*/


			/*Vector4f JointColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
			Vector4f BoneColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);

			m_CapturedStick.jointColor(JointColor);
			m_CapturedStick.boneColor(BoneColor);
			m_SynthStick.jointColor(JointColor);
			m_SynthStick.boneColor(BoneColor);
			m_StyleStick.jointColor(JointColor);
			m_StyleStick.boneColor(BoneColor);
			m_Style2Stick.jointColor(JointColor);
			m_Style2Stick.boneColor(BoneColor);*/

#endif

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}//clear

		void run(void) override{
			SkeletalAnimationController::Animation* pAnimFront = nullptr;
			SkeletalAnimationController::Animation* pAnimBack = nullptr;

			bool RepeatAnim = false;
			float LastAnimSpeed = 16.666f;

			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				// this will progress all active skeletal animations for this controller
				if (!m_Pause) {
					m_ControllerCaptured.update(60.0f / m_FPS);
					m_ControllerSynth.update(60.0f / m_FPS);
					m_ControllerStyle.update(60.0f / m_FPS);
					m_ControllerStyle2.update(60.0f / m_FPS);
				}
				
				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1000.0f/60.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed *= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_CONTROL)) AnimationSpeed /= 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					if(m_ControllerCaptured.animationCount() > 0) m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, AnimationSpeed, 0.0f));
					if(m_ControllerSynth.animationCount() > 0) m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, AnimationSpeed, 0.0f));
					if(m_ControllerStyle.animationCount() > 0) m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, AnimationSpeed, 0.0f));
					if (m_ControllerStyle2.animationCount() > 0) m_Style2.activeAnimation(m_ControllerStyle2.createAnimation(0, AnimationSpeed, 0.0f));
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
					m_Style2.activeAnimation(m_ControllerStyle2.createAnimation(0, LastAnimSpeed, 0.0f));
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

#ifdef MAKE_HUMAN_TEST
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

				if (m_Recording) {

					if (m_Captured.activeAnimation() == nullptr) {
						m_RecordingFile.end();
						m_Recording = false;

						m_ErrorsSynth.z() /= float(m_RecordingCounter);
						m_ErrorsStyle.z() /= float(m_RecordingCounter);
						m_ErrorsStyle2.z() /= float(m_RecordingCounter);

						printf("Recording finished\n");
						printf("\t Errors Synth (Max | Min | Avg): %.5f %.5f %.5f\n", m_ErrorsSynth.x(), m_ErrorsSynth.y(), m_ErrorsSynth.z());
						printf("\t Errors Style (Max | Min | Avg): %.5f %.5f %.5f\n", m_ErrorsStyle.x(), m_ErrorsStyle.y(), m_ErrorsStyle.z());
						printf("\t Errors Style2 (Max | Min | Avg): %.5f %.5f %.5f\n", m_ErrorsStyle2.x(), m_ErrorsStyle2.y(), m_ErrorsStyle2.z());
					}
					else {
						std::vector<Eigen::Matrix4f> SkinningMats;

						//// deform capture mesh
						m_ControllerCaptured.applyAnimation(m_Captured.activeAnimation(), true);
						m_ControllerCaptured.retrieveSkinningMatrices(&SkinningMats);
						transformSkinnedMesh(&m_CapturedMeshOrig, &m_CapturedMeshTransformed, &SkinningMats);
						SkinningMats.clear();

						// deform synth mesh
						m_ControllerSynth.applyAnimation(m_Synth.activeAnimation(), true);
						m_ControllerSynth.retrieveSkinningMatrices(&SkinningMats);
						transformSkinnedMesh(&m_SynthMeshOrig, &m_SynthMeshTransformed, &SkinningMats);
						SkinningMats.clear();

						//// deform Style2 mesh
						//m_ControllerStyle2.applyAnimation(m_Style2.activeAnimation(), true);
						//m_ControllerStyle2.retrieveSkinningMatrices(&SkinningMats);
						//transformSkinnedMesh(&m_Style2MeshOrig, &m_Style2MeshTransformed, &SkinningMats);
						//SkinningMats.clear();

						// deform style mesh
						m_ControllerStyle.applyAnimation(m_Style.activeAnimation(), true);
						m_ControllerStyle.retrieveSkinningMatrices(&SkinningMats);
						transformSkinnedMesh(&m_StyleMeshOrig, &m_StyleMeshTransformed, &SkinningMats);
						SkinningMats.clear();

						// compute mesh deviations
						float Dev1 = computeMeshToMeshRSME(&m_CapturedMeshTransformed, &m_SynthMeshTransformed);
						if (Dev1 > m_ErrorsSynth.x()) m_ErrorsSynth.x() = Dev1;
						if (Dev1 < m_ErrorsSynth.y()) m_ErrorsSynth.y() = Dev1;
						m_ErrorsSynth.z() += Dev1;

						/*float Dev2 = computeMeshToMeshRSME(&m_CapturedMeshTransformed, &m_Style2MeshTransformed);
						if (Dev2 > m_ErrorsStyle2.x()) m_ErrorsStyle2.x() = Dev2;
						if (Dev2 < m_ErrorsStyle2.y()) m_ErrorsStyle2.y() = Dev2;
						m_ErrorsStyle2.z() += Dev2;*/

						float Dev3 = computeMeshToMeshRSME(&m_CapturedMeshTransformed, &m_StyleMeshTransformed);
						if (Dev3 > m_ErrorsStyle.x()) m_ErrorsStyle.x() = Dev3;
						if (Dev3 < m_ErrorsStyle.y()) m_ErrorsStyle.y() = Dev3;
						m_ErrorsStyle.z() += Dev3;

						string Msg = std::to_string(m_RecordingCounter) + ", " + std::to_string(Dev1) + ", " + std::to_string(Dev3) + "\n";
						m_RecordingFile.write(Msg.c_str(), Msg.length());
						m_RecordingCounter++;
					}
				}//if[recording]

				// print current deformed mesh
				if (m_RenderWin.keyboard()->keyState(Keyboard::KEY_LEFT_SHIFT) == Keyboard::KEY_PRESSED && m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_M, true)) {

					
					if(m_Recording){
						
					}
					else {
						m_RecordingFile.begin("MyAssets/Technique_Evaluation/Out/Recording.csv", "w"); 
						string Msg = "Frame Number, Error M1, Error M2\n";
						m_RecordingFile.write(Msg.c_str(), Msg.length());

						if (m_ControllerCaptured.animationCount() > 0) m_Captured.activeAnimation(m_ControllerCaptured.createAnimation(0, AnimationSpeed/2.0f, 0.0f));
						if (m_ControllerSynth.animationCount() > 0) m_Synth.activeAnimation(m_ControllerSynth.createAnimation(0, AnimationSpeed/2.0f, 0.0f));
						if (m_ControllerStyle.animationCount() > 0) m_Style.activeAnimation(m_ControllerStyle.createAnimation(0, AnimationSpeed/2.0f, 0.0f));
						//if (m_ControllerStyle2.animationCount() > 0) m_Style2.activeAnimation(m_ControllerStyle.createAnimation(0, AnimationSpeed/2.0f, 0.0f));

						float t = 1.0f * 1000.0f / 60.0f;

						m_Captured.activeAnimation()->t = t;
						m_Synth.activeAnimation()->t = t;
						m_Style.activeAnimation()->t = t;
						//m_Style2.activeAnimation()->t = t;

						m_Recording = true;
						m_RecordingCounter = 0;

						m_ErrorsStyle = Vector3f::Zero();
						m_ErrorsSynth = Vector3f::Zero();
						//m_ErrorsStyle2 = Vector3f::Zero();

						m_ErrorsStyle.y() = std::numeric_limits<float>::max();
						m_ErrorsSynth.y() = std::numeric_limits<float>::max();
						//m_ErrorsStyle2.y() = std::numeric_limits<float>::max();

						printf("Recording started\n");

					}
					

					//printf("t: %.4f | %.4f | %.4f\n", m_Captured.activeAnimation()->t, m_Synth.activeAnimation()->t, m_Style.activeAnimation()->t);

				}//if[print deformed mesh]
				else if( m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_M, true)) {
					std::vector<Eigen::Matrix4f> SkinningMats;


					// deform capture mesh
					m_ControllerCaptured.applyAnimation(m_Captured.activeAnimation(), true);
					m_ControllerCaptured.retrieveSkinningMatrices(&SkinningMats);
					transformSkinnedMesh(&m_CapturedMeshOrig, &m_CapturedMeshTransformed, &SkinningMats);
					SkinningMats.clear();

					// deform synth mesh
					m_ControllerSynth.applyAnimation(m_Synth.activeAnimation(), true);
					m_ControllerSynth.retrieveSkinningMatrices(&SkinningMats);
					transformSkinnedMesh(&m_SynthMeshOrig, &m_SynthMeshTransformed, &SkinningMats);
					SkinningMats.clear();

					// deform style mesh
					m_ControllerStyle.applyAnimation(m_Style.activeAnimation(), true);
					m_ControllerStyle.retrieveSkinningMatrices(&SkinningMats);
					transformSkinnedMesh(&m_StyleMeshOrig, &m_StyleMeshTransformed, &SkinningMats);
					SkinningMats.clear();


					AssetIO::store("MyAssets/Technique_Evaluation/Out/Captured.obj", &m_CapturedMeshTransformed);
					AssetIO::store("MyAssets/Technique_Evaluation/Out/Synth.obj", &m_SynthMeshTransformed);
					AssetIO::store("MYAssets/Technique_Evaluation/Out/Style.obj", &m_StyleMeshTransformed);

					// compute mesh deviations
					float Dev1 = computeMeshToMeshRSME(&m_CapturedMeshTransformed, &m_SynthMeshTransformed);
					float Dev2 = computeMeshToMeshRSME(&m_CapturedMeshTransformed, &m_StyleMeshTransformed);

					printf("Deviations: %.4f | %.4f\n", Dev1, Dev2);

					//printf("t: %.4f | %.4f | %.4f\n", m_Captured.activeAnimation()->t, m_Synth.activeAnimation()->t, m_Style.activeAnimation()->t);

				}//if[print deformed mesh]

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F5, true)) {
					if (m_VisualizeSkeleton) {
						m_StyleSGN.visualization(SGNGeometry::VISUALIZATION_FILL);
						m_CapturedSGN.visualization(SGNGeometry::VISUALIZATION_FILL);
						m_SynthSGN.visualization(SGNGeometry::VISUALIZATION_FILL);
						m_Style2SGN.visualization(SGNGeometry::VISUALIZATION_FILL);

						m_StyleStickSGN.enable(true, false);
						m_CapturedStickSGN.enable(true, false);
						m_SynthStickSGN.enable(true, false);
						m_Style2StickSGN.enable(true, false);

						m_StyleSGN.enable(true, true);
					}
					else {
						m_StyleSGN.visualization(SGNGeometry::VISUALIZATION_WIREFRAME);
						m_CapturedSGN.visualization(SGNGeometry::VISUALIZATION_WIREFRAME);
						m_SynthSGN.visualization(SGNGeometry::VISUALIZATION_WIREFRAME);
						m_Style2SGN.visualization(SGNGeometry::VISUALIZATION_WIREFRAME);

						m_StyleStickSGN.enable(true, true);
						m_CapturedStickSGN.enable(true, true);
						m_SynthStickSGN.enable(true, true);
						m_Style2StickSGN.enable(true, true);

						m_StyleSGN.enable(true, false);
					}
					m_VisualizeSkeleton = !m_VisualizeSkeleton;

				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_P, true)) m_Pause = !m_Pause;

				//if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F7, true)) {
				//	// take screenshot
				//	T2DImage<uint8_t> Img;
				//	GraphicsUtility::retrieveFrameBuffer(&Img);
				//	std::string Filepath = "Screenshots/Screenshot" + to_string(m_ScreenshotCounter++) + ".jpg";
				//	AssetIO::store(Filepath, &Img);
				//}

			}//while[main loop]

		}//run

	protected:
		

		void transformSkeleton(T3DMesh<float>::Bone* pBone, Eigen::Matrix4f ParentTransform, std::vector<T3DMesh<float>::BoneKeyframes*>* pKeyframes, Eigen::Vector4f *pMinMax, bool ConsiderTranslationOnly, bool DryRun) {
			// compute local transform
			T3DMesh<float>::BoneKeyframes* pFrame = nullptr;
			
			for (auto i : (*pKeyframes)) {
				if (i->BoneName.compare(pBone->Name) == 0) pFrame = i;
			}

			Eigen::Matrix4f JointTransform = Matrix4f::Identity();
			if (pFrame != nullptr) {
				Matrix4f R = (pFrame->Rotations.size() > 0) ? CForgeMath::rotationMatrix(pFrame->Rotations[0]) : Matrix4f::Identity();
				Matrix4f T = (pFrame->Positions.size() > 0) ? CForgeMath::translationMatrix(pFrame->Positions[0]) : Matrix4f::Identity();
				Matrix4f S = (pFrame->Scalings.size() > 0) ? CForgeMath::scaleMatrix(pFrame->Scalings[0]) : Matrix4f::Identity();

				if (ConsiderTranslationOnly) {
					S = R = Matrix4f::Identity();
				}

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
			for (auto i : pBone->Children) transformSkeleton(i, LocalTransform, pKeyframes, pMinMax, ConsiderTranslationOnly, DryRun);

		}//transformSkeleton

		void recomputeInverseBindPoseMatrix(T3DMesh<float>* pMesh, bool ScaleRootAnimation, bool ConsiderTranslationOnly) {
			if (nullptr == pMesh->rootBone()) throw CForgeExcept("Mesh contains no root bone!");
			if (0 == pMesh->skeletalAnimationCount()) throw CForgeExcept("Mesh contains no animations!");

			auto *pRoot = pMesh->rootBone();
			auto* pAnim = pMesh->getSkeletalAnimation(0);

			Eigen::Vector4f MinMax[3] = { Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero() };

			//for(auto i: pRoot->Children) transformSkeleton(i, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, false);
			transformSkeleton(pRoot, Eigen::Matrix4f::Identity(), &pAnim->Keyframes, MinMax, ConsiderTranslationOnly, false);

		
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
					pMerged->Keyframes[i]->Timestamps.push_back(float(k)); // pKeyframeNew->Timestamps[k]);
					
				}//for[rotation data]

				// copy root bone data
				if (pMerged->Keyframes[i]->BoneName.compare(RootBone) == 0) {
					pMerged->Keyframes[i]->Positions = pKeyframeNew->Positions;
				}	
			}//for[all keyframes]

		}//copyRotationData

		void transformSkinnedMesh(T3DMesh<float> *pOriginalMesh, T3DMesh<float>* pTransformedMesh, std::vector<Eigen::Matrix4f> *pSkinningMatrices) {

			std::vector<Eigen::Vector3f> TransformedVertices;
			for (uint32_t i = 0; i < pOriginalMesh->vertexCount(); ++i) TransformedVertices.push_back(Vector3f::Zero());

			for (uint32_t i = 0; i < pOriginalMesh->boneCount(); ++i) {
				auto* pBone = pOriginalMesh->getBone(i);
				Eigen::Matrix4f SkinMat = pSkinningMatrices->at(i);

				for (uint32_t k = 0; k < pBone->VertexInfluences.size(); ++k) {
					int32_t VertexID = pBone->VertexInfluences[k];
					float VertexWeight = pBone->VertexWeights[k];
					Eigen::Vector4f V = VertexWeight * SkinMat * Eigen::Vector4f(pOriginalMesh->vertex(VertexID).x(), pOriginalMesh->vertex(VertexID).y(), pOriginalMesh->vertex(VertexID).z(), 1.0f);
					TransformedVertices[VertexID] += Eigen::Vector3f(V.x(), V.y(), V.z());
				}//for[all influences]

			}//for[all bones]

			pTransformedMesh->vertices(&TransformedVertices);

		}//transformMesh

		float computeMeshToMeshRSME(const T3DMesh<float>* pOrigin, const T3DMesh<float>* pTarget) {
			float Rval = 0.0f;
			if (pOrigin->vertexCount() != pTarget->vertexCount()) throw CForgeExcept("Origin and target differ in vertex count. Can not compute RSME");


			for (uint32_t i = 0; i < pOrigin->vertexCount(); ++i) {
				const float Dist = (pTarget->vertex(i) - pOrigin->vertex(i)).norm();
				Rval += Dist * Dist;
			}

			return std::sqrt(Rval/pOrigin->vertexCount());
		}//computeMeshToMeshRSME

		StaticActor m_Skydome;
		SkeletalActor m_Captured;
		SkeletalActor m_Synth;
		SkeletalActor m_Style;
		SkeletalActor m_Style2;

		StickFigureActor m_CapturedStick;
		StickFigureActor m_SynthStick;
		StickFigureActor m_StyleStick;
		StickFigureActor m_Style2Stick;

		SkeletalAnimationController m_ControllerCaptured;
		SkeletalAnimationController m_ControllerSynth;
		SkeletalAnimationController m_ControllerStyle;
		SkeletalAnimationController m_ControllerStyle2;

		StaticActor m_ComparisonModel;
		SGNGeometry m_ComparisonModelSGN;

		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;

		SGNGeometry m_CapturedSGN;
		SGNGeometry m_StyleSGN;
		SGNGeometry m_SynthSGN;
		SGNGeometry m_Style2SGN;

		SGNGeometry m_CapturedStickSGN;
		SGNGeometry m_StyleStickSGN;
		SGNGeometry m_SynthStickSGN;
		SGNGeometry m_Style2StickSGN;

		SGNTransformation m_CapturedTransformSGN;
		SGNTransformation m_StyleTransformSGN;
		SGNTransformation m_SynthTransformSGN;
		SGNTransformation m_Style2TransformSGN;


		T3DMesh<float> m_CapturedMeshOrig;
		T3DMesh<float> m_SynthMeshOrig;
		T3DMesh<float> m_StyleMeshOrig;
		T3DMesh<float> m_Style2MeshOrig;
		T3DMesh<float> m_CapturedMeshTransformed;
		T3DMesh<float> m_SynthMeshTransformed;
		T3DMesh<float> m_StyleMeshTransformed;
		T3DMesh<float> m_Style2MeshTransformed;

		bool m_Recording;
		File m_RecordingFile;
		uint32_t m_RecordingCounter;
		Eigen::Vector3f m_ErrorsSynth;
		Eigen::Vector3f m_ErrorsStyle;
		Eigen::Vector3f m_ErrorsStyle2;

		bool m_VisualizeSkeleton;
		bool m_Pause;

	};//SkelAnimTestScene

	void skelAnimTestScene(void) {

		SkelAnimTestScene Scene;
		Scene.init();
		Scene.run();
		Scene.clear();

	}//exampleMinimumGraphicsSetup

}

#endif