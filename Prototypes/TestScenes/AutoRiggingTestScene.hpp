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
#ifndef __CFORGE_AUTORIGGINGTESTSCENE_HPP__
#define __CFORGE_AUTORIGGINGTESTSCENE_HPP__

#include "../../crossforge/AssetIO/SAssetIO.h"
#include "../../crossforge/Graphics/Shader/SShaderManager.h"
#include "../../crossforge/Graphics/STextureManager.h"

#include "../../crossforge/Graphics/GLWindow.h"
#include "../../crossforge/Math/CForgeMath.h"
#include "../../crossforge/Graphics/RenderDevice.h"

#include "../../crossforge/Graphics/Lights/DirectionalLight.h"
#include "../../crossforge/Graphics/Lights/PointLight.h"

#include "../../crossforge/Graphics/SceneGraph/SceneGraph.h"
#include "../../crossforge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../crossforge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../crossforge/Graphics/Actors/StaticActor.h"
#include "../../crossforge/Graphics/Actors/SkeletalActor.h"

#include "../../Examples/exampleSceneBase.hpp"

//#include "../AutoRig.hpp"

#include "../../thirdparty/Pinocchio/PinocchioTools.hpp"
#include "../Assets/GLTFIO/GLTFIO.hpp"

#include "Prototypes/SkeletonConvertion.hpp"

#include <filesystem>
#include <stdio.h>
#include <iostream>

namespace nsPiT = nsPinocchioTools;

using namespace Eigen;
using namespace std;

namespace CForge {
	
	class AutoRiggingTestScene : public ExampleSceneBase {
	public:
		AutoRiggingTestScene() {
			
		}
		~AutoRiggingTestScene() {
			
		}
		virtual void initCameraAndLights(bool CastShadows = true) {
			// initialize camera
			m_Cam.init(Vector3f(0.0f, 3.0f, 8.0f), Vector3f::UnitY());
			m_Cam.projectionMatrix(m_WinWidth, m_WinHeight, CForgeMath::degToRad(45.0f), 0.1f, 1000.0f);

			Vector3f SunPos = Vector3f(-25.0f, 50.0f, -20.0f);
			Vector3f SunLookAt = Vector3f(0.0f, 0.0f, 30.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
			DirectionalLight Sun;
			PointLight BGLight;
			m_Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows

			//m_Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
			if (CastShadows)
				m_Sun.initShadowCasting(1024, 1024, Vector2i(10, 10), 0.1f, 1000.0f);
			m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			m_RenderDev.activeCamera(&m_Cam);
			m_RenderDev.addLight(&m_Sun);
			m_RenderDev.addLight(&m_BGLight);
		}//initCameraAndLights
		
		void mainLoop() {

			gladLoadGL();
			
			GLTFIO gltfio;
			
			// load skydome and a textured cube
			T3DMesh<float> M;
			StaticActor Skydome;
			SkeletalActor Eric;
			StaticActor Spock;
			SkeletalAnimationController Controller;
			
			SAssetIO::load("MyAssets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			Skydome.init(&M);
			M.clear();

			/////////////////////////////////////////////////////////////////

			SAssetIO::load("MyAssets/ExampleScenes/Eric_Anim.fbx", &M);
			//SAssetIO::load("AssetOut/outModel1.fbx", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			
			T3DMesh<float> MT;
			std::filesystem::path modelPath;
			//modelPath = std::filesystem::path("MyAssets/muppetshow/Model1.obj");
			modelPath = std::filesystem::path("MyAssets/muppetshow/Model2.obj");
			//modelPath = std::filesystem::path("MyAssets/muppetshow/Model3.obj");
			//modelPath = std::filesystem::path("MyAssets/muppetshow/Model6.obj");
			//modelPath = std::filesystem::path("MyAssets/muppetshow/female_body.obj");
			//modelPath = std::filesystem::path("MyAssets/autorig/spock/spockR2.fbx");
			//modelPath = std::filesystem::path("MyAssets/autorig/spock/spockR3.fbx");
			//modelPath = std::filesystem::path("MyAssets/autorig/Armadillo.fbx");
			std::string modelName = modelPath.stem().string();
			
			SAssetIO::load(modelPath.string(), &MT);
			setMeshShader(&MT, 0.7f, 0.04f);
			mergeRedundantVertices(&MT);
			MT.computePerVertexNormals();
			Spock.init(&MT);
			
			//*
			nsPinocchio::Skeleton sklEric;
			nsPiT::CVScalingInfo cvsInfo;
			std::vector<nsPiT::BonePair> sym;
			std::vector<T3DMesh<float>::Bone*> foot;
			std::vector<T3DMesh<float>::Bone*> fat;
			std::vector<T3DMesh<float>::Bone*> bones;
			
			// assign foot and symmetry for pinoccio
			for (uint32_t i = 0; i < M.boneCount(); i++) {
				T3DMesh<float>::Bone* cur = M.getBone(i);
				if (cur->Name.compare("RightToe")==0 || cur->Name.compare("LeftToe")==0)
					foot.push_back(cur);
				
				if (cur->Name.find("Left") != std::string::npos) {
					// find Left
					std::string sBoneType = cur->Name.substr(5);
					//std::cout << sBoneType << "\n";
					for (uint32_t j = 0; j < M.boneCount(); j++) {
						T3DMesh<float>::Bone* cur2 = M.getBone(j);
						//std::cout << "compare: " << cur2->Name << " with: " << "Right" + sBoneType << "\n";
						if (cur2->Name.compare("Right" + sBoneType)==0) {
							nsPiT::BonePair pair;
							pair.pair[0] = cur;
							pair.pair[1] = cur2;
							sym.push_back(pair);
							break;
						}
					}
				}
				// TODO fat
				
				bones.push_back(cur);
			}
			
			std::vector<Eigen::Vector3f> joints;
			nsPiT::convertSkeleton(M.rootBone(), &sklEric, &cvsInfo, sym, fat, foot,
			       CForgeMath::rotationMatrix((Quaternionf) AngleAxisf(CForgeMath::degToRad(-90.0f),Vector3f::UnitX())).block<3,3>(0,0),
			       &joints);
			
			nsPinocchio::Mesh piM;// = new nsPinocchio::Mesh(); //("MyAssets/muppetshow/Model1.obj");
			nsPiT::convertMesh(MT, &piM);
			
			vector<Vector3f> poss;
			vector<float> rads;
			nsPiT::autorigCust(sklEric, piM, &poss, &rads);
			//nsPinocchio::PinocchioOutput rig = nsPinocchio::autorig(sklEric, piM);
			nsPinocchio::PinocchioOutput rig = nsPiT::autorig(sklEric, &piM);
			
			MT.bones(&bones);
			MT.clearSkeletalAnimations();
			//MT.addSkeletalAnimation(M.getSkeletalAnimation(0));
			nsPiT::copyAnimation(M,&MT,0);
			for (uint32_t i = 0; i < MT.boneCount(); i++) {
				MT.getBone(i)->VertexInfluences = std::vector<int32_t>();
				MT.getBone(i)->VertexWeights = std::vector<float>();
			}
			
			if (rig.attachment)
				nsPiT::applyWeights(&sklEric, &piM, &MT, cvsInfo, rig, MT.vertexCount());
			
			/////////////////////////////////////////////////////////////////////////////
			//std::string outNameFBX = "MyAssets/AssetOut/out" + modelName + "3.fbx";
			//AssetIO::store(outNameFBX, &M);
			
			std::string outName = "MyAssets/AssetOut/out" + modelName + "3.gltf";
			gltfio.store(outName,&MT);
			//__debugbreak();
			exit(0);
			//std::string outNameEric = "MyAssets/AssetOut/eric.gltf";
			//gltfio.store(outNameEric,&M);
			//*/
			//AutoRigger autoRigger; // for sphere visualisation
			//autoRigger.init(&MT,M.getBone(0));
			//autoRigger.process();
			
			// build scene graph
			SceneGraph SG;
			SGNTransformation RootSGN;
			RootSGN.init(nullptr);
			SG.init(&RootSGN);

			// add skeletal actor to scene graph (Eric)
			SGNGeometry EricSGN;
			SGNTransformation EricTransformSGN;
			EricTransformSGN.init(&RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			EricSGN.init(&EricTransformSGN, &Eric);
			//EricSGN.init(&EricTransformSGN, &Spock);
			EricTransformSGN.scale(Eigen::Vector3f(10.0f,10.0f,10.0f));
			
			/**/
			Controller.init(&MT);
			Eric.init(&MT, &Controller);
			/**/
			/*/
			Controller.init(&M);
			Eric.init(&M, &Controller);
			EricSGN.scale(Vector3f(0.005f, 0.005f, 0.005f));
			//EricTransformSGN.rotation((Quaternionf) AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX()));
			/**/
			M.clear();

			// add skydome
			SGNGeometry SkydomeSGN;
			//SkydomeSGN.init(&RootSGN, &Skydome);
			//SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());
			
			T3DMesh<float> sphereMesh;
			StaticActor sphere;
			AssetIO::load("MyAssets/sphere.obj", &sphereMesh);
			setMeshShader(&sphereMesh, 0.8f, 0.04f);
			sphereMesh.computePerVertexNormals();
			sphere.init(&sphereMesh);
			
			T3DMesh<float> boneMesh;
			StaticActor boneVis;
			AssetIO::load("MyAssets/boneVis.obj", &boneMesh);
			setMeshShader(&boneMesh, 0.8f, 0.04f);
			boneMesh.computePerVertexNormals();
			boneVis.init(&boneMesh);

			SkeletonConverter sc;
			sc.OMtoRH(MT.rootBone());
			T3DMesh<float>::Bone* sklHroot = sc.getRoot();

			// start main loop
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				SG.update(60.0f/FPS);
				
				// this will progress all active skeletal animations for this controller
				Controller.update(60.0f/FPS);
				
				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), 0.4f, 1.0f, 4.0f*60.0f/FPS);
				
				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed = 2.0f;
				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation *pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
					Eric.activeAnimation(pAnim);
				}
				
				//m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				//SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);

				//autoRigger.renderGrid(&m_RenderDev, CForgeMath::translationMatrix(Eigen::Vector3f(0.0,0.0,0.0)),
				//	CForgeMath::rotationMatrix((Quaternionf) AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX()))
				//	*CForgeMath::scaleMatrix(Vector3f(0.05f, 0.05f, 0.05f)));

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				/*/ // render embedded joints
				for (uint32_t i = 0; i < (uint32_t) rig.embedding.size(); i++) {
					Eigen::Matrix4f cubeTransform = CForgeMath::translationMatrix(Eigen::Vector3f(rig.embedding[i][0],rig.embedding[i][1],rig.embedding[i][2]));
					float r = 0.02f;
					Eigen::Matrix4f cubeScale = CForgeMath::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
					Eigen::Matrix4f scale = CForgeMath::scaleMatrix(Eigen::Vector3f(10.0f,10.0f,10.0f));
					Eigen::Quaternionf rot;
					m_RenderDev.modelUBO()->modelMatrix(scale*cubeTransform*cubeScale);
					
					sphere.render(&m_RenderDev,rot,Eigen::Vector3f(),Eigen::Vector3f());
				}
				/**/
				
				// Render Offset Matrix
				glColorMask(true,false,false,true);
				std::vector<T3DMesh<float>::Bone*> MTbones;
				SkeletonConverter::collectBones(&MTbones,MT.rootBone());
				for (uint32_t i = 0; i < MTbones.size(); ++i) {
					Eigen::Quaternionf rot;
					m_RenderDev.modelUBO()->modelMatrix(CForgeMath::scaleMatrix(Eigen::Vector3f(10.,10.,10.))*MTbones[i]->OffsetMatrix.inverse()*CForgeMath::scaleMatrix(Eigen::Vector3f(.01,.1,.01)));
					boneVis.render(&m_RenderDev,rot,Eigen::Vector3f(),Eigen::Vector3f());
				}
				glColorMask(true,true,true,true);
				
				// Render Skeleton Hierarchy
				glColorMask(false,true,false,true);
				std::vector<T3DMesh<float>::Bone*> todoBones;
				todoBones.push_back(sklHroot);
				while (todoBones.size() > 0) {
					T3DMesh<float>::Bone* cb = todoBones[todoBones.size()-1];
					todoBones.pop_back();
					for (uint32_t i=0;i<cb->Children.size();++i)
						todoBones.push_back(cb->Children[i]);
					
					// accumulate bone mats to get offsetmat replica
					Eigen::Matrix4f rep = Eigen::Matrix4f::Identity();
					T3DMesh<float>::Bone* cbr = cb;
					while (cbr->pParent) {
						rep = rep*cbr->OffsetMatrix;
						cbr = cbr->pParent;
					}
					
					Eigen::Quaternionf rot;
					m_RenderDev.modelUBO()->modelMatrix(CForgeMath::scaleMatrix(Eigen::Vector3f(10.,10.,10.))*rep*CForgeMath::scaleMatrix(Eigen::Vector3f(.01,.1,.01)));
					boneVis.render(&m_RenderDev,rot,Eigen::Vector3f(),Eigen::Vector3f());
				}
				glColorMask(true,true,true,true);
				/**/ // render sphere packing
				//for (uint32_t i = 0; i < (uint32_t) poss.size(); i++) {
				//	Eigen::Matrix4f cubeTransform = CForgeMath::translationMatrix(poss[i]);
				//	float r = rads[i];
				//	Eigen::Matrix4f cubeScale = CForgeMath::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
				//	Eigen::Matrix4f scale = CForgeMath::scaleMatrix(Eigen::Vector3f(10.0f,10.0f,10.0f));
				//	m_RenderDev.modelUBO()->modelMatrix(scale*cubeTransform*cubeScale);
				//	autoRigger.sphere.render(&m_RenderDev);
				//}
				/**/
				
				
				/*/ // render joints
				for (uint32_t i = 0; i < (uint32_t) joints.size(); i++) {
					Eigen::Matrix4f cubeTransform = CForgeMath::translationMatrix(joints[i]);
					float r = 0.1f;
					Eigen::Matrix4f cubeScale = CForgeMath::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
					m_RenderDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
					autoRigger.sphere.render(&m_RenderDev);
				}
				/**/
				
				SG.render(&m_RenderDev);
				glDisable(GL_BLEND);
				
				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderWin.swapBuffers();

				FPSCount++;
				if (CForgeUtility::timestamp() - LastFPSPrint > 1000U) {
					char Buf[64];
					sprintf(Buf, "FPS: %d\n", FPSCount);
					FPS = float(FPSCount);
					FPSCount = 0;
					LastFPSPrint = CForgeUtility::timestamp();

					m_RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
				}

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
					m_RenderWin.closeWindow();
				}
			}//while[main loop]

			m_pShaderMan->release();

		}
		void mergeRedundantVertices(T3DMesh<float>* pMesh) {
			float Epsilon = /**/std::numeric_limits<float>::min();//*/0.00025f;//TODO

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
				//printf("Checking vertex %d/%d\r", i, pMesh->vertexCount());

				for (uint32_t k = i+1; k < pMesh->vertexCount(); ++k) {
					auto v2 = pMesh->vertex(k);

					if ((v2 - v1).dot(v2 - v1) < Epsilon) {
						RedundantVertices.push_back(std::pair<uint32_t, uint32_t>(i,k));
						IsRedundant[k] = true;
						//break; //TODO multple vertices can be redundant?
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
	private:
		std::string WindowTitle = "CForge - AutoRigging Test";
		float FPS = 60.0f;
	};
}

#endif