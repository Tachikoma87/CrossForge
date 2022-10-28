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

#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/STextureManager.h"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/GraphicsUtility.h"
#include "../../CForge/Graphics/RenderDevice.h"

#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"

#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "../../CForge/Graphics/Actors/StaticActor.h"
#include "../../CForge/Graphics/Actors/SkeletalActor.h"

#include "../../Examples/exampleSceneBase.hpp"

#include "../AutoRig.hpp"

#include "../../thirdparty/Pinocchio/pinocchioApi.h"
#include "../../thirdparty/PinocchioTools/PinocchioTools.hpp"

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
		void run(void) {
			SShaderManager* pSMan = SShaderManager::instance();

			std::string WindowTitle = "CForge - Skeletal Animation Example";
			float FPS = 60.0f;

			bool const LowRes = false;

			uint32_t WinWidth = 1280;
			uint32_t WinHeight = 720;

			if (LowRes) {
				WinWidth = 720;
				WinHeight = 576;
			}

			// create an OpenGL capable windows
			GLWindow RenderWin;
			RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), WindowTitle);
			gladLoadGL();

			// configure and initialize rendering pipeline
			RenderDevice RDev;
			RenderDevice::RenderDeviceConfig Config;
			Config.DirectionalLightsCount = 1;
			Config.PointLightsCount = 1;
			Config.SpotLightsCount = 0;
			Config.ExecuteLightingPass = true;
			Config.GBufferHeight = WinHeight;
			Config.GBufferWidth = WinWidth;
			Config.pAttachedWindow = &RenderWin;
			Config.PhysicallyBasedShading = true;
			Config.UseGBuffer = true;
			RDev.init(&Config);

			// configure and initialize shader configuration device
			ShaderCode::LightConfig LC;
			LC.DirLightCount = 1;
			LC.PointLightCount = 1;
			LC.SpotLightCount = 0;
			LC.PCFSize = 0;
			LC.ShadowBias = 0.00001f;
			LC.ShadowMapCount = 1;
			pSMan->configShader(LC);

			// initialize camera
			VirtualCamera Cam;
			Cam.init(Vector3f(20.0f, 5.0f, 45.0f), Vector3f::UnitY());
			Cam.lookAt(Vector3f(10.0f, 5.0f, 35.0f), Vector3f(0.0f, 4.0f, 25.0f), Vector3f::UnitY());
			Cam.projectionMatrix(WinWidth, WinHeight, GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			// initialize sun (key lights) and back ground light (fill light)
			Vector3f SunPos = Vector3f(-25.0f, 50.0f, -20.0f);
			Vector3f SunLookAt = Vector3f(0.0f, 0.0f, 30.0f);
			Vector3f BGLightPos = Vector3f(0.0f, 5.0f, 60.0f);
			DirectionalLight Sun;
			PointLight BGLight;
			Sun.init(SunPos, (SunLookAt - SunPos).normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
			// sun will cast shadows
			Sun.initShadowCasting(1024, 1024, GraphicsUtility::orthographicProjection(20.0f, 20.0f, 0.1f, 1000.0f));
			BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.5f, Vector3f(0.0f, 0.0f, 0.0f));

			// set camera and lights
			RDev.activeCamera(&Cam);
			RDev.addLight(&Sun);
			RDev.addLight(&BGLight);

			// load skydome and a textured cube
			T3DMesh<float> M;	
			StaticActor Skydome;
			SkeletalActor Eric;
			StaticActor Spock;
			SkeletalAnimationController Controller;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			Skydome.init(&M);
			M.clear();

			// initialize skeletal actor (Eric) and its animation controller
			SAssetIO::load("Assets/ExampleScenes/Eric_Anim.fbx", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			//AssetIO::store("AssetOut/out.fbx", &M);
			//return;
			
			T3DMesh<float> MT;
			//SAssetIO::load("Assets/muppetshow/Model1.obj", &MT);
			//SAssetIO::load("Assets/muppetshow/female_body.obj", &MT);
			SAssetIO::load("Assets/autorig/spock/spockR2.fbx", &MT);
			setMeshShader(&MT, 0.7f, 0.04f);
			mergeRedundantVertices(&MT);
			MT.computePerVertexNormals();
			Spock.init(&MT);

			nsPinocchio::Skeleton sklEric;
			nsPiT::CVScalingInfo cvsInfo;
			std::vector<nsPiT::BonePair> sym;
			std::vector<T3DMesh<float>::Bone*> foot;
			std::vector<T3DMesh<float>::Bone*> fat;
			std::vector<T3DMesh<float>::Bone*> bones;
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
			GraphicsUtility::rotationMatrix((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(-90.0f),Vector3f::UnitX())).block<3,3>(0,0), &joints);
			
			nsPinocchio::Mesh piM;// = new nsPinocchio::Mesh(); //("Assets/muppetshow/Model1.obj");
			nsPiT::convertMesh(&MT, &piM);
			//nsPinocchio::Mesh piM("Assets/autorig/eric.obj");
			vector<Vector3f> poss;
			vector<float> rads;
			nsPinocchioTools::autorigCust(sklEric, piM, &poss, &rads);
			//nsPinocchio::PinocchioOutput rig = nsPinocchio::autorig(sklEric, piM);
			nsPinocchio::PinocchioOutput rig = nsPiT::autorig(sklEric, &piM);
			
			MT.bones(&bones);
			MT.clearSkeletalAnimations();
			//MT.addSkeletalAnimation(M.getSkeletalAnimation(0));
			nsPiT::copyAnimation(&M,&MT,0);
			for (uint32_t i = 0; i < MT.boneCount(); i++) {
				MT.getBone(i)->VertexInfluences = std::vector<int32_t>();
				MT.getBone(i)->VertexWeights = std::vector<float>();
			}
			
			if (rig.attachment)
				nsPiT::applyWeights(&sklEric, &piM, &MT, cvsInfo, rig, MT.vertexCount());
			
			AutoRigger autoRigger;
			autoRigger.init(&MT,M.getBone(0));
			//autoRigger.process();
			Controller.init(&MT);
			Eric.init(&MT, &Controller);
			AssetIO::store("AssetOut/out.fbx", &MT);
			return;
			//Controller.init(&M);
			//Eric.init(&M, &Controller);
			M.clear();

			// build scene graph
			SceneGraph SG;
			SGNTransformation RootSGN;
			RootSGN.init(nullptr);
			SG.init(&RootSGN);

			// add skydome
			SGNGeometry SkydomeSGN;
			//SkydomeSGN.init(&RootSGN, &Skydome);
			//SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add skeletal actor to scene graph (Eric)
			SGNGeometry EricSGN;
			SGNTransformation EricTransformSGN;
			EricTransformSGN.init(&RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			EricSGN.init(&EricTransformSGN, &Eric);
			//EricSGN.init(&EricTransformSGN, &Spock);
			//EricSGN.scale(Vector3f(0.05f, 0.05f, 0.05f));
			//EricTransformSGN.scale(Eigen::Vector3f(100.0f,100.0f,100.0f));
			//EricTransformSGN.rotation((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX()));

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			// check wheter a GL error occurred
			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			// start main loop
			while (!RenderWin.shutdown()) {
				RenderWin.update();
				SG.update(60.0f/FPS);
				
				// this will progress all active skeletal animations for this controller
				Controller.update(60.0f/FPS);
				
				defaultCameraUpdate(&Cam, RenderWin.keyboard(), RenderWin.mouse(), 0.4f, 1.0f, 4.0f*60.0f/FPS);
				
				// if user hits key 1, animation will be played
				// if user also presses shift, animation speed is doubled
				float AnimationSpeed = 1.0f;
				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT_SHIFT)) AnimationSpeed = 2.0f;
				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
					SkeletalAnimationController::Animation *pAnim = Controller.createAnimation(0, AnimationSpeed, 0.0f);
					Eric.activeAnimation(pAnim);
				}
				
				//RDev.activePass(RenderDevice::RENDERPASS_SHADOW, &Sun);
				//SG.render(&RDev);

				RDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);

				//autoRigger.renderGrid(&RDev, GraphicsUtility::translationMatrix(Eigen::Vector3f(0.0,0.0,0.0)),
				//	GraphicsUtility::rotationMatrix((Quaternionf) AngleAxisf(GraphicsUtility::degToRad(-90.0f), Vector3f::UnitX()))
				//	*GraphicsUtility::scaleMatrix(Vector3f(0.05f, 0.05f, 0.05f)));

				//glEnable(GL_BLEND);
				//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
				SG.render(&RDev);
				//glDisable(GL_BLEND);
				//for (uint32_t i = 0; i < (uint32_t) rig.embedding.size(); i++) {
				//	Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(Eigen::Vector3f(rig.embedding[i][0],rig.embedding[i][1],rig.embedding[i][2]));
				//	float r = 0.1f;
				//	Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
				//	RDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
				//	autoRigger.sphere.render(&RDev);
				//}
				//for (uint32_t i = 0; i < (uint32_t) joints.size(); i++) {
				//	Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(joints[i]);
				//	float r = 0.1f;
				//	Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
				//	RDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
				//	autoRigger.sphere.render(&RDev);
				//}
				for (uint32_t i = 0; i < (uint32_t) poss.size(); i++) {
					Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(poss[i]);
					float r = rads[i];
					Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
					RDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
					//autoRigger.sphere.render(&RDev);
				}
				//glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
				//{
				//	Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(Eigen::Vector3f(1.0,1.0,1.0));
				//	float r = 0.1;
				//	Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
				//	RDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
				//	autoRigger.sphere.render(&RDev);
				//}
				//glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
				//{
				//	Eigen::Matrix4f cubeTransform = GraphicsUtility::translationMatrix(Eigen::Vector3f(-1.0,-1.0,-1.0));
				//	float r = 0.1;
				//	Eigen::Matrix4f cubeScale = GraphicsUtility::scaleMatrix(Eigen::Vector3f(r,r,r)*2.0f);
				//	RDev.modelUBO()->modelMatrix(cubeTransform*cubeScale);
				//	autoRigger.sphere.render(&RDev);
				//}
				//glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

				RDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				RenderWin.swapBuffers();

				FPSCount++;
				if (CoreUtility::timestamp() - LastFPSPrint > 1000U) {
					char Buf[64];
					sprintf(Buf, "FPS: %d\n", FPSCount);
					FPS = float(FPSCount);
					FPSCount = 0;
					LastFPSPrint = CoreUtility::timestamp();

					RenderWin.title(WindowTitle + "[" + std::string(Buf) + "]");
				}

				if (RenderWin.keyboard()->keyPressed(Keyboard::KEY_ESCAPE)) {
					RenderWin.closeWindow();
				}
			}//while[main loop]

			pSMan->release();

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
				printf("Checking vertex %d/%d\r", i, pMesh->vertexCount());

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
	};
	
	void autoRiggingTestScene(void) {
		AutoRiggingTestScene ts;
		ts.run();
	}
}

#endif