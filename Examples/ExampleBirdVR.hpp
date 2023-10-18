/*****************************************************************************\
*                                                                           *
* File(s): ExampleBirdVR		                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable    *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_EXAMPLEBIRD_HPP__
#define __CFORGE_EXAMPLEBIRD_HPP__

#include "ExampleSceneBase.hpp"
#include "../CForge/Graphics/Actors/SkyboxActor.h"
#include "../CForge/MeshProcessing/PrimitiveShapeFactory.h"

#include "fcl/narrowphase/collision_object.h"
#include "fcl/narrowphase/distance.h"

#include <chrono>

#define BUILDING_ROW_COUNT 5
#define BUILDING_COLUMN_COUNT 5
#define BUILDING_COUNT (BUILDING_ROW_COUNT * BUILDING_COLUMN_COUNT)

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleBird : public ExampleSceneBase {
	public:
		ExampleBird(void) {
			m_WindowTitle = "CrossForge Example - Bird";
		}//Constructor

		~ExampleBird(void) {
			clear();
		}//Destructor

		void init(void) override {
			initWindowAndRenderDevice();
			initCameraAndLights();

			srand(static_cast <unsigned> (time(0)));

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);
	
			T3DMesh<float> M;
			
			// load Bird
			SAssetIO::load("MyAssets/Bird/bird.fbx", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Bird.init(&M);

			// calculate AABB for the bird
			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB birdAABB = M.aabb();
			float birdBBSphereR = birdAABB.Min.norm();
			birdBBSphereR = birdBBSphereR > birdAABB.Min.norm() ? birdBBSphereR : birdAABB.Max.norm();
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/TexturedGround.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 50.0f;
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			BoundingVolume BV;
			m_Ground.boundingVolume(BV);
			M.clear();
			m_GroundTransformSGN.init(&m_RootSGN);
			m_GroundSGN.init(&m_GroundTransformSGN, &m_Ground);
			m_GroundSGN.scale(Vector3f(15.0f, 15.0f, 15.0f));

			// raven
			m_BirdTransformSGN.init(&m_RootSGN, m_startPosition);
			m_BirdTransformSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			m_BirdPitchSGN.init(&m_BirdTransformSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_BirdRollSGN.init(&m_BirdPitchSGN, Vector3f(0.0f, 0.0f, 0.0f));

			// Quaternionf To_Y;
			Quaternionf To_Y = (Quaternionf)AngleAxis(CForgeMath::degToRad(90.0f), Vector3f::UnitY());
			m_BirdTransformSGN.rotation(To_Y);
			m_BirdSGN.init(&m_BirdRollSGN, &m_Bird);
			m_birdSphere = m_BirdSGN.actor()->boundingVolume().boundingSphere();

			// load buildings
			SAssetIO::load("MyAssets/Buildings/building_06/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB buildingAABB = M.aabb();
			setBuildingAABB(buildingAABB, 0);
			m_Buildings[0].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Buildings/building_07/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			buildingAABB = M.aabb();
			setBuildingAABB(buildingAABB, 1);
			m_Buildings[1].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Buildings/building_08/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals(); 
			M.computeAxisAlignedBoundingBox();
			buildingAABB = M.aabb();
			setBuildingAABB(buildingAABB, 2);
			m_Buildings[2].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Sphere.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Sphere.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/cube.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Cube.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/cylinder.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Cylinder.init(&M);
			M.clear();

			m_BuildingGroupSGN.init(&m_RootSGN);

			// TODO: change to vector
			for (uint32_t i = 0; i < BUILDING_COUNT; i++)
			{
				uint32_t x = i / BUILDING_COLUMN_COUNT;
				uint32_t y = i % BUILDING_COLUMN_COUNT;

				uint32_t building = m_building_asset[x * BUILDING_COLUMN_COUNT + y];

				if (building != -1)
				{
					set_building(x * 60, y * 60, 5.0f, building, i);
				}
			}

			//Checkpoints
			initCheckpoints();

			/// gather textures for the skyboxes
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_right.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_left.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_up.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_down.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_back.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_front.png");


			// create actor and initialize
			m_Skybox.init(m_ClearSky[0], m_ClearSky[1], m_ClearSky[2], m_ClearSky[3], m_ClearSky[4], m_ClearSky[5]);

			// set initialize color adjustment values
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.1f);
			m_Skybox.saturation(1.2f);

			// create scene graph for the Skybox
			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxGeomSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);

			// start time
			m_timeStart = std::chrono::high_resolution_clock::now();

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override {
			ExampleSceneBase::clear();
			m_RenderWin.closeWindow();
		}//clear

		void setBuildingAABB(T3DMesh<float>::AABB buildingAABB, int i) {
			m_buildingDiag[i] = buildingAABB.diagonal();
			m_buildingOrigin[i] = buildingAABB.Min + (m_buildingDiag[i] * 0.5f);
			m_box_building[i] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->z());
		}

		void set_building(float x, float y, float z, int model, int i) {
			SGNTransformation* pTransformSGN = nullptr;
			SGNGeometry* pGeomSGN = nullptr;

			pTransformSGN = new SGNTransformation();
			pTransformSGN->init(&m_BuildingGroupSGN);

			// set to other vector
			pTransformSGN->translation(Vector3f(x, z, y));
			pTransformSGN->scale(m_building_scale);

			pGeomSGN = new SGNGeometry();
			pGeomSGN->init(pTransformSGN, &m_Buildings[model]);

			m_BuildingTransformationSGNs.push_back(pTransformSGN);
			m_BuildingSGNs.push_back(pGeomSGN);

		}

		void initRandomCheckpoints(void) {

			m_CPPosVec.push_back(m_startPosition + Vector3f(0.0f, 0.0f, 60.0f));

			// 2D Grid movements
			vector <Vector3f> actionRoom = { Vector3f(0.0f, 0.0f, 60.0f), Vector3f(0.0f, 0.0f, -60.0f), Vector3f(60.0f, 0.0f, 0.0f), Vector3f(-60.0f, 0.0f, 0.0f) };
			vector <Vector3f> activeActionRoom;

			Vector3f prevAction = Vector3f(0.0f, 0.0f, 60.0f);

			for (int i = 1; i < m_CPCount; i++) {
				float heightChange = 2.0f * (float)(rand() / (float)RAND_MAX) - 2.0f;		// random heightchange from -2.0f to 2.0f
				activeActionRoom.clear();
				for (auto j : actionRoom) {
					if (j != -prevAction) activeActionRoom.push_back(j);
				}
				prevAction = activeActionRoom[rand() % 3];
				m_CPPosVec.push_back(m_CPPosVec[i - 1] + prevAction + Vector3f(0.0f, heightChange, 0.0f));
				if (m_CPPosVec[i].y() < m_minHeight) m_CPPosVec[i].y() = m_minHeight;
				else if (m_CPPosVec[i].y() > m_maxHeight) m_CPPosVec[i].y() = m_maxHeight;

			}

			buildCheckpoints(m_CPPosVec);

		}

		void initCheckpoints(void) {
			//TODO create checkpoints positions manually
			m_CPPosVec.push_back(m_startPosition + Vector3f(30.0f, 0.0f, 0.0f));
			m_CPPosVec.push_back(m_CPPosVec[0] + Vector3f(30.0f, 5.0f, 0.0f));
			m_CPPosVec.push_back(m_CPPosVec[1] + Vector3f(15.0f, 5.0f, 15.0f));
			m_CPPosVec.push_back(m_CPPosVec[2] + Vector3f(15.0f, 5.0f, 15.0f));
			m_CPPosVec.push_back(m_CPPosVec[3] + Vector3f(0.0f, 5.0f, 30.0f));
			m_CPPosVec.push_back(m_CPPosVec[4] + Vector3f(0.0f, 0.0f, 30.0f));
			m_CPPosVec.push_back(m_CPPosVec[5] + Vector3f(10.0f, -5.0f, 20.0f));
			m_CPPosVec.push_back(m_CPPosVec[6] + Vector3f(20.0f, -5.0f, 10.0f));
			m_CPPosVec.push_back(m_CPPosVec[7] + Vector3f(20.0f, -5.0f, -10.0f));
			m_CPPosVec.push_back(m_CPPosVec[8] + Vector3f(10.0f, 0.0f, -20.0f));
			m_CPPosVec.push_back(m_CPPosVec[9] + Vector3f(-10.0f, 0.0f, -20.0f));
			m_CPPosVec.push_back(m_CPPosVec[10] + Vector3f(-20.0f, 0.0f, -10.0f));
			m_CPPosVec.push_back(m_CPPosVec[11] + Vector3f(-30.0f, 0.0f, 0.0f));
			m_CPPosVec.push_back(m_CPPosVec[12] + Vector3f(-30.0f, 5.0f, 0.0f));
			m_CPPosVec.push_back(m_CPPosVec[13] + Vector3f(-20.0f, 5.0f, -10.0f));
			m_CPPosVec.push_back(m_CPPosVec[14] + Vector3f(-10.0f, 5.0f, -20.0f));
			m_CPPosVec.push_back(m_CPPosVec[15] + Vector3f(0.0f, 5.0f, -30.0f));
			m_CPPosVec.push_back(m_CPPosVec[16] + Vector3f(10.0f, 5.0f, -20.0f));
			m_CPPosVec.push_back(m_CPPosVec[17] + Vector3f(20.0f, 0.0f, -10.0f));
			m_CPPosVec.push_back(m_CPPosVec[18] + Vector3f(15.0f, 0.0f, 15.0f));
			m_CPPosVec.push_back(m_CPPosVec[19] + Vector3f(15.0f, 0.0f, 15.0f));
			m_CPPosVec.push_back(m_CPPosVec[20] + Vector3f(30.0f, 0.0f, 30.0f));
			m_CPPosVec.push_back(m_CPPosVec[21] + Vector3f(30.0f, -5.0f, 30.0f));
			m_CPPosVec.push_back(m_CPPosVec[22] + Vector3f(30.0f, -5.0f, 0.0f));
			m_CPPosVec.push_back(m_CPPosVec[23] + Vector3f(30.0f, -5.0f, 0.0f));
			buildCheckpoints(m_CPPosVec);

		}

		void buildCheckpoints(vector <Vector3f> positions) {

			//if (positions == ([] || NULL)) return;

			m_CheckpointsSG.clear();

			//add to scenegraph
			T3DMesh <float> M;
			PrimitiveShapeFactory::Torus(&M, m_CPRadius, m_CPHeight, 15, 15); //alt.: load
			//SAssetIO::load("MyAssets/Torus/scene.gltf", &M);

			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();

			//set Color for Mesh

			for (uint32_t i = 0; i < M.materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = M.getMaterial(i);

				//PrimitiveShape
				pMat->Color = Vector4f(0.0f, 0.0f, 0.9f, 0.2f);

				//Asset
				//pMat->Color.w() = 0.7f;
			}


			m_Checkpoint.init(&M);
			M.clear();

			//base rotation for torus
			Quaternionf Q;
			Q = AngleAxis(CForgeMath::degToRad(90.0f), Vector3f::UnitZ());

			//int checkpointNum = positions.length();

			m_CPGroupSGN.init(nullptr);
			for (size_t i = 0; i < positions.size(); i++) {
				SGNTransformation* pTransformSGN = nullptr;
				SGNGeometry* pGeomSGN = nullptr;

				pTransformSGN = new SGNTransformation();
				pTransformSGN->init(&m_CPGroupSGN);

				pTransformSGN->translation(positions[i]);

				//rotation
				Vector3f change = Vector3f(0.0f, 0.0f, 0.0f);
				if (i != 0) change += positions[i] - positions[i - 1];
				if (i + 1 < positions.size()) change += positions[i + 1] - positions[i];
				Vector3f changeNorm = change.normalized();
				Vector3f changeNoY = Vector3f(change.x(), 0.0f, change.z());
				changeNoY.normalize();
				float yAngle = acos(changeNoY.dot(Vector3f::UnitX()));
				float xzAngle = acos(changeNorm.dot(changeNoY));
				// get correct sign of yAngle
				if (changeNoY.z() > 0) yAngle = -yAngle;
				//printf("y: %f, xz: %f\n", yAngle, xzAngle);
				//rotate so that checkpoint points in change direction
				Quaternionf Y;
				Y = AngleAxis(yAngle, Vector3f::UnitY());
				Quaternionf XZ;
				XZ = AngleAxis(xzAngle, (changeNoY.cross(changeNorm)).normalized());
				//XZ = AngleAxis(xzAngle, Vector3f::UnitX());
				pTransformSGN->rotation(XZ * Y * Q);

				pGeomSGN = new SGNGeometry();
				pGeomSGN->init(pTransformSGN, &m_Checkpoint);
				pGeomSGN->scale(Vector3f(1.0f, 1.0f, 1.0f));

				m_CPSGNs.push_back(pGeomSGN);
				m_CPTransformationSGNs.push_back(pTransformSGN);
			}

			m_CheckpointsSG.init(&m_CPGroupSGN);
			return;
		}

		void CollisionTest() {

			// setup for collision detections
			auto solver_type = fcl::GJKSolverType::GST_LIBCCD;
			using fcl::Vector3;
			using Real = typename fcl::constants<float>::Real;
			const Real eps = fcl::constants<float>::eps();

			// set the AABBs for the buildings 
			const Vector3<float> half_size_building6{ m_buildingDiag[0] * 0.5 };
			const Vector3<float> half_size_building7{ m_buildingDiag[1] * 0.5 };
			const Vector3<float> half_size_building8{ m_buildingDiag[2] * 0.5 };

			Eigen::Vector3f BirdPos;
			Eigen::Quaternionf BirdRot;
			Eigen::Vector3f BirdScale;
			m_BirdTransformSGN.buildTansformation(&BirdPos, &BirdRot, &BirdScale);

			auto birdSphere = m_BirdSGN.actor()->boundingVolume().boundingSphere();
			m_max_scale_bird = std::max(std::max(BirdScale.x(), BirdScale.y()), BirdScale.z());

			auto bird_collision = std::make_shared<fcl::Sphere<float>>(birdSphere.radius() * m_max_scale_bird);
			fcl::Transform3<float> X_WS = fcl::Transform3<float>::Identity();
			fcl::CollisionObject<float> bird_collision_geometry(bird_collision, X_WS);

			fcl::CollisionRequest<float> collision_request(1 /* num contacts */,
				true /* enable_contact */);
			collision_request.gjk_solver_type = solver_type;

			fcl::Transform3f bMat = fcl::Transform3f::Identity();
			bMat.translate(BirdPos); // no further offset is needed
			bMat.rotate(BirdRot); // scale can not be set! -> can just do it above with the m_max_scale_bird

			m_birdTestCollision = bMat.matrix();
			bird_collision_geometry.setTransform(bMat);

			m_col = false;
			m_colCP = false;
			auto evaluate_collision = [&](
				const fcl::CollisionObject<float>* s1, const fcl::CollisionObject<float>* s2, bool* col) {
					// Compute collision.
					fcl::CollisionResult<float> collision_result;
					std::size_t contact_count = fcl::collide(s1, s2, collision_request, collision_result);

					// Test answers
					if (contact_count >= collision_request.num_max_contacts) {
						std::vector<fcl::Contact<float>> contacts;
						collision_result.getContacts(contacts);
						//GTEST_ASSERT_EQ(contacts.size(), collision_request.num_max_contacts);
						*col = true;
					}
			};

			// set ring to the next, if its the last then we end

			if (m_CPCollisonCurrent < m_CPSGNs.size()) {
				auto CheckPointTSGN = m_CPSGNs[m_CPCollisonCurrent]->parent();

				Eigen::Vector3f posCheckPoint;
				Eigen::Quaternionf rotCheckPoint;
				Eigen::Vector3f scaleCheckPoint;

				CheckPointTSGN->buildTansformation(&posCheckPoint, &rotCheckPoint, &scaleCheckPoint);

				auto cylinder_checkpoint = std::make_shared<fcl::Cylinder<float>>(m_CPRadius, m_CPHeight);

				fcl::Transform3<float> X_WB = fcl::Transform3<float>::Identity();
				fcl::CollisionObject<float> cylinder_collisoin_geometry(cylinder_checkpoint, X_WB);

				fcl::Transform3f mat = fcl::Transform3f::Identity();
				mat.translate(posCheckPoint);
				mat.rotate(rotCheckPoint);
				mat.rotate((Quaternionf)AngleAxisf(CForgeMath::degToRad(90.0f), Vector3f::UnitX()));

				m_cylinderTestCollision = mat.matrix() * CForgeMath::scaleMatrix(Vector3f(1.0f, 1.0f, 1.0f));
				cylinder_collisoin_geometry.setTransform(mat);

				evaluate_collision(&bird_collision_geometry, &cylinder_collisoin_geometry, &m_colCP);
				if (m_colCP) m_CPCollisonCurrent++; // needs to be a frame after that
			}
			if (m_CPCollisonCurrent == m_CPSGNs.size() && !m_timePrinted) {
				m_timeEnd = std::chrono::high_resolution_clock::now();
				m_totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(m_timeEnd - m_timeStart);
				auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(m_totalTime);
				auto remainingMilliseconds = m_totalTime - totalSeconds;

				std::cout << "You did it in: " << std::chrono::duration_cast<std::chrono::seconds>(totalSeconds).count() 
					<< "." << std::chrono::duration_cast<std::chrono::milliseconds>(remainingMilliseconds).count() 
					<< " seconds - you hit " << m_hitCounter << " buildings" << std::endl;
				m_timePrinted = true;

				m_totalTime = std::chrono::milliseconds::zero();
			}

			

			int buildingAssetsIdx = 0;
			for (size_t i = 0; i < m_BuildingSGNs.size(); i++)
			{
				int model = m_building_asset[buildingAssetsIdx];
				buildingAssetsIdx++;

				while (model == -1) {
					buildingAssetsIdx++;

					if (buildingAssetsIdx >= m_BuildingSGNs.size()) throw CForgeExcept("Index out of bounds");

					model = m_building_asset[buildingAssetsIdx];
				}

				// get transformnode - m_BuildingTransformationSGNs[i];
				auto buildingTSGN = m_BuildingSGNs[i]->parent();

				Eigen::Vector3f posBuilding;
				Eigen::Quaternionf rotBuilding;
				Eigen::Vector3f scaleBuilding;

				buildingTSGN->buildTansformation(&posBuilding, &rotBuilding, &scaleBuilding);

				auto buildingAABB = m_BuildingSGNs[i]->actor()->boundingVolume().aabb();

				Eigen::Vector3f buildingDiag = buildingAABB.diagonal().cwiseProduct(scaleBuilding);
				Eigen::Vector3f buildingOrigin = buildingAABB.min().cwiseProduct(scaleBuilding) + (buildingDiag * 0.5f);
				auto box_building = std::make_shared<fcl::Box<float>>(buildingDiag.x(), buildingDiag.y(), buildingDiag.z());

				fcl::Transform3<float> X_WB = fcl::Transform3<float>::Identity();
				fcl::CollisionObject<float> building_collisoin_geometry(box_building, X_WB);

				fcl::Transform3f mat = fcl::Transform3f::Identity();
				mat.rotate(rotBuilding);
				mat.translate(posBuilding + m_buildingOrigin[model].cwiseProduct(scaleBuilding));
				m_buildingTestCollision[i] = mat.matrix() * CForgeMath::scaleMatrix(Vector3f(buildingDiag.x(), buildingDiag.y(), buildingDiag.z()));
				building_collisoin_geometry.setTransform(mat);

				evaluate_collision(&bird_collision_geometry, &building_collisoin_geometry, &m_col);
			}
		}

		void checkCollisionBuilding() {
			// penalty for colding with building
			if (!m_colLastFrame && m_col) {
				// first instance of collision
				using namespace std::literals::chrono_literals;
				m_totalTime += 1000ms;
				m_colLastFrame = true;
				m_hitCounter += 1;
			}
			if (m_colLastFrame && !m_col) {
				// do not collide anymore
				m_colLastFrame = false; 
			}
		}

		void defaultCameraUpdateBird(VirtualCamera* pCamera, Keyboard* pKeyboard, Mouse* pMouse, Vector3f m, Vector3f posBird, Vector3f up, Matrix3f bird) {
			if (nullptr == pCamera) throw NullpointerExcept("pCamera");
			if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");
			if (nullptr == pMouse) throw NullpointerExcept("pMouse");

			//float S = 1.0f;
			//if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) S = SpeedScale;
			Vector3f mm;
			Vector3f posC = Vector3f(0.0f, 3.0f, -10.0f); // position for the camera

			// eigen matrix3f -> (row, col)
			mm.x() = bird(0, 0) * posC.x() + bird(0, 2) * posC.z();
			mm.y() = posC.y();
			mm.z() = bird(2, 0) * posC.x() + bird(2, 2) * posC.z();
			pCamera->position(posBird + mm);
			pCamera->lookAt(pCamera->position(), posBird, up);

			/*if (pMouse->buttonState(Mouse::BTN_RIGHT)) {
				if (m_CameraRotation) {
					const Eigen::Vector2f MouseDelta = pMouse->movement();
					pCamera->rotY(CForgeMath::degToRad(-0.1f * 1 * MouseDelta.x()));
					pCamera->pitch(CForgeMath::degToRad(-0.1f * 1 * MouseDelta.y()));

				}
				else {
					m_CameraRotation = true;

				}
				pMouse->movement(Eigen::Vector2f::Zero());
			}
			else {
				m_CameraRotation = false;
			}*/
		}//defaultCameraUpdate

		void timeUpdate() {
			// paused is started
			if (!m_pausedLastFrame && m_paused) {
				m_pausedLastFrame = true;

				m_timeEnd = std::chrono::high_resolution_clock::now();
				m_totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(m_timeEnd - m_timeStart);
			}
			// pause is ended
			if (m_pausedLastFrame && !m_paused) {
				m_pausedLastFrame = false;

				// start again
				m_timeStart = std::chrono::high_resolution_clock::now();
			}

		}

		void mainLoop(void)override {
			if (!glLoaded) {
				gladLoadGL();
				glLoaded = true;
			}

			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			CollisionTest();

			// give penalty if building was hit
			checkCollisionBuilding();

			// handle input for the skybox
			Keyboard* pKeyboard = m_RenderWin.keyboard();
			float Step = (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) ? -0.05f : 0.05f;
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) m_Skybox.brightness(m_Skybox.brightness() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) m_Skybox.saturation(m_Skybox.saturation() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) m_Skybox.contrast(m_Skybox.contrast() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) && pKeyboard->keyPressed(Keyboard::KEY_R)) {
				m_SkyboxTransSGN.rotationDelta(Quaternionf::Identity());
			}
			else if (pKeyboard->keyPressed(Keyboard::KEY_R, true)) {
				Quaternionf RDelta;
				RDelta = AngleAxisf(CForgeMath::degToRad(-2.5f / 60.0f), Vector3f::UnitY());
				m_SkyboxTransSGN.rotationDelta(RDelta);
			}

			// reset
			if (pKeyboard->keyPressed(Keyboard::KEY_BACKSPACE)) {
				m_BirdTransformSGN.translation(m_startPosition);
				m_BirdTransformSGN.rotation(Quaternionf::Identity() * (Quaternionf)AngleAxis(CForgeMath::degToRad(90.0f), Vector3f::UnitY()));
				m_CPCollisonCurrent = 0;
				m_timeStart = chrono::high_resolution_clock::now();
				m_timePrinted = false;
				m_totalTime = std::chrono::milliseconds::zero();
				m_hitCounter = 0;
			}

			//TODO input for resetting Checkpoints
			if (pKeyboard->keyPressed(Keyboard::KEY_P, true)) m_paused = !m_paused;
			if (m_paused) {
				m_BirdTransformSGN.translationDelta(Vector3f(0.0f, 0.0f, 0.0f));
				timeUpdate();
			}
			if (!m_paused)
			{
				timeUpdate();
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT)) {
					if (m_rollSpeed < 3.0f) m_rollSpeed += 1.0f;
				}
				else {
					if (pKeyboard->keyPressed(Keyboard::KEY_RIGHT)) {
						if (m_rollSpeed > -3.0f) m_rollSpeed -= 1.0f;
					}
					else {
						if (m_rollSpeed < 0.0f) m_rollSpeed += 1.0f;
						if (m_rollSpeed > 0.0f) m_rollSpeed -= 1.0f;
					}
				}
				Quaternionf To_Y;
				To_Y = AngleAxis(CForgeMath::degToRad(m_rollSpeed / (8.0f * m_speed.z())), Vector3f::UnitY());
				m_BirdTransformSGN.rotation(m_BirdTransformSGN.rotation() * To_Y);

				Quaternionf To_Z;
				To_Z = AngleAxis(CForgeMath::degToRad(-m_rollSpeed * 4.0f), Vector3f::UnitZ());
				m_BirdRollSGN.rotation(To_Z);

				Vector3f pos;
				Vector3f xzdir;
				Quaternionf rot;
				Matrix3f m3;
				Vector3f dir;

				// gain and loose speed
				if (pKeyboard->keyPressed(Keyboard::KEY_UP) && m_speed.z() <= 0.5f)m_speed.z() += 0.01f;
				if (pKeyboard->keyPressed(Keyboard::KEY_DOWN) && m_speed.z() >= 0.3f)m_speed.z() -= 0.01f;

				// the bird sinks during normal flight and gains altitude when pressed space
				if (pKeyboard->keyPressed(Keyboard::KEY_SPACE, true)) m_speed.y() += 0.3;
				if (m_speed.y() > -0.01f) m_speed.y() -= 0.03f;

				// bird to near the ground -> remains altitude
				if (m_BirdTransformSGN.translation().y() < 0.05) m_speed.y() += 0.1f;

				// dive
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) m_speed.y() -= 0.01f;
				else if (m_speed.y() < -0.01f) m_speed.y() += 0.02f;

				if (m_speed.y() < -0.01f) {
					Quaternionf To_X;
					float pitchAngle = -m_speed.y() * 40.0f; if (pitchAngle > 80.0f) pitchAngle = 80.0f;
					To_X = AngleAxis(CForgeMath::degToRad(pitchAngle), Vector3f::UnitX());
					m_BirdPitchSGN.rotation(To_X);
				}

				// Bird is rotated in the direction where it is looking
				m3 = m_BirdTransformSGN.rotation().toRotationMatrix();
				dir.x() = m3(0, 0) * m_speed.x() + m3(0, 2) * m_speed.z();
				dir.y() = m_speed.y();
				dir.z() = m3(2, 0) * m_speed.x() + m3(2, 2) * m_speed.z();

				xzdir = Vector3f(dir.x(), 0, dir.z()).normalized();

				//Quaternionf rotate_left = AngleAxis(CForgeMath::degToRad(2.5f), dir);

				// in translation there is the postion
				//printf("%f - %f - %f | %f\n", m_BirdTransformSGN.translation().x(), m_BirdTransformSGN.translation().y(), m_BirdTransformSGN.translation().z(), m_speed.y());

				m_BirdTransformSGN.translationDelta(dir);


				//defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
				defaultCameraUpdateBird(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), dir, m_BirdTransformSGN.translation(), Vector3f(0.0f, 1.0f, 0.0f), m3);
			}
			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
			m_SG.render(&m_RenderDev);
			m_CheckpointsSG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);
			m_CheckpointsSG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);


			// while pressing key 9 you can enable the debug mode
			// there you can see the bounding boxes of the bird and the buildings

			glEnable(GL_BLEND);

			// debug bird

			Eigen::Vector3f posBird;
			Eigen::Quaternionf rotBird;
			Eigen::Vector3f scaleBird;
			m_BirdTransformSGN.buildTansformation(&posBird, &rotBird, &scaleBird);

			Eigen::Matrix4f scaleMatrix = CForgeMath::scaleMatrix(Eigen::Vector3f(m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius())));

			// debug building
			int buildingCollisionIdx = 0;
			int model = 0;
			
 			if (pKeyboard->keyPressed(Keyboard::KEY_9)) {
				// bird
				glDisable(GL_DEPTH_TEST);
				glBlendFunc(GL_ONE, GL_ONE);

				if (m_col)
					glColorMask(true, false, false, true);
				else
					glColorMask(false, true, false, true);

				m_RenderDev.modelUBO()->modelMatrix(m_birdTestCollision * scaleMatrix);
				m_Sphere.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());

				// building
				glColorMask(true, true, true, true);

				for (size_t i = 0; i < m_BuildingSGNs.size(); i++)
				{
					model = m_building_asset[buildingCollisionIdx];
					buildingCollisionIdx++;

					while (model == -1) {
						buildingCollisionIdx++;

						if (buildingCollisionIdx >= m_BuildingSGNs.size()) throw CForgeExcept("Index out of bounds");

						model = m_building_asset[buildingCollisionIdx];
					}
					auto building = m_BuildingTransformationSGNs[i];

					m_RenderDev.modelUBO()->modelMatrix(m_buildingTestCollision[i]);
					m_Cube.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());

				}
			}

			if (m_colCP)
				glColorMask(false, false, true, true);
			else
				glColorMask(false, true, false, true);

			if (m_CPCollisonCurrent < m_CPSGNs.size()) {
				m_cylinderTestCollision *= CForgeMath::scaleMatrix(Eigen::Vector3f(m_CPRadius * 2, m_CPRadius * 2, m_CPHeight));
				m_RenderDev.modelUBO()->modelMatrix(m_cylinderTestCollision);
				m_Cylinder.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());
			}
			
			glColorMask(true, true, true, true);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);

			// Skybox should be last thing to render
			m_SkyboxSG.render(&m_RenderDev);

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}

	protected:

		StaticActor m_Bird;
		SGNTransformation m_RootSGN;

		vector<string> m_ClearSky;
		vector<string> m_EmptySpace;
		vector<string> m_Techno;
		vector<string> m_BlueCloud;

		SkyboxActor m_Skybox;

		SGNGeometry m_BirdSGN;
		SGNTransformation m_BirdTransformSGN;
		SGNTransformation m_BirdRollSGN;
		SGNTransformation m_BirdPitchSGN;

		StaticActor m_Ground;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_GroundTransformSGN;

		// Building
		StaticActor m_Buildings[3];
		SGNTransformation m_BuildingGroupSGN;
		std::vector<SGNTransformation*> m_BuildingTransformationSGNs;
		std::vector<SGNGeometry*> m_BuildingSGNs;
		const Vector3f m_building_scale = Vector3f(10.0f, 10.0f, 10.0f);

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;


		// vectors for the buildings - check whether it works or not! 
		int m_building_asset[BUILDING_COUNT] = { -1, 1, 1, 0, 2, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 1, 0, 0, 2, 0, 2, 2, 0, 2 };
		Vector3f m_buildingOrigin[3];
		Vector3f m_buildingDiag[3];
		std::shared_ptr<fcl::Box<float>> m_box_building[3];

		StaticActor m_Sphere;
		StaticActor m_Cube;
		StaticActor m_Cylinder;
		Eigen::Matrix4f m_birdTransform = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_birdTransformfclPure = Eigen::Matrix4f::Identity();
		CForge::Sphere m_birdSphere;
		float m_max_scale_bird;

		std::vector<Eigen::Matrix4f> m_buildingTestCollision = std::vector<Eigen::Matrix4f>(BUILDING_COUNT, Eigen::Matrix4f::Identity());
		Eigen::Matrix4f m_birdTestCollision = Eigen::Matrix4f::Identity();

		bool m_col = false;
		bool m_colLastFrame = false; 
		bool m_colCP = false;
		bool glLoaded = false;
		uint16_t m_hitCounter = 0; 

		Vector3f m_speed = Vector3f(0.0f, 0.0f, 0.3f);
		float m_rollSpeed = 0.0f;
		Vector3f m_startPosition = Vector3f(0.0f, 10.0f, 30.0f);

		//Checkpoints
		SceneGraph m_CheckpointsSG;
		StaticActor m_Checkpoint;
		SGNTransformation m_CPGroupSGN;
		std::vector<SGNTransformation*> m_CPTransformationSGNs;
		std::vector<SGNGeometry*> m_CPSGNs;
		int m_CPCount = 20;
		int m_CPCollisonCurrent = 0;
		const float m_CPHeight = 0.2f;
		const float m_CPRadius = 2.0f;
		Eigen::Matrix4f m_cylinderTestCollision;
		vector<Vector3f> m_CPPosVec;
		float m_minHeight = 2.0f;
		float m_maxHeight = 30.0f;
		

		bool m_paused = true;
		bool m_pausedLastFrame = false;
		std::chrono::steady_clock::time_point m_timeStart;
		std::chrono::steady_clock::time_point m_timeEnd;
		std::chrono::milliseconds m_totalTime = std::chrono::milliseconds::zero();
		bool m_timePrinted = false;

	};//ExampleBird

}

#endif
