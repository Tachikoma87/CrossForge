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

#include "fcl/narrowphase/collision_object.h"
#include "fcl/narrowphase/distance.h"

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
			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);
			// load skydome and a textured cube
			T3DMesh<float> M;
			SAssetIO::load("MyAssets/Bird/bird.fbx", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Bird.init(&M);

			// calculate AABB for the bird -->
			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB birdAABB = M.aabb();
			float birdBBSphereR = birdAABB.Min.norm();
			birdBBSphereR = birdBBSphereR > birdAABB.Min.norm() ? birdBBSphereR : birdAABB.Max.norm();
			M.clear();
			// # Todo: make the same for the buildings (all 3 types -> get the size of them) 

			/*SAssetIO::load("Assets/ExampleScenes/Bird/mountain_range_01.glb", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Mountain.init(&M);
			M.clear();*/

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
			m_BirdTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 10.0f, 0.0f));
			m_BirdTransformSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			m_BirdPitchSGN.init(&m_BirdTransformSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_BirdRollSGN.init(&m_BirdPitchSGN, Vector3f(0.0f, 0.0f, 0.0f));

			m_MountainTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 5.0f, 0.0f));
			//m_MountainTransformSGN.scale(Vector3f(1.5f, 1.5f, 1.5f));

			Quaternionf To_Y;
			To_Y = AngleAxis(CForgeMath::degToRad(90.0f), Vector3f::UnitY());
			m_BirdTransformSGN.rotation(To_Y);
			m_BirdSGN.init(&m_BirdRollSGN, &m_Bird);



			// rotate about the y-axis at 45 degree every second and about the X axis to make it a bit more interesting
			/*Quaternionf RY, RX;
			RY = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			RX = AngleAxisf(CForgeMath::degToRad(-25.0f / 60.0f), Vector3f::UnitZ());
			m_BirdTransformSGN.rotationDelta(RX * RY);*/

			/*
			T3DMesh<float> M;
			SAssetIO::load("MyAssets/Bird/bird.fbx", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Bird.init(&M);

			// calculate AABB for the bird -->
			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB birdAABB = M.aabb();
			float birdBBSphereR = birdAABB.Min.norm();
			birdBBSphereR = birdBBSphereR > birdAABB.Min.norm() ? birdBBSphereR : birdAABB.Max.norm();
			M.clear();
			*/

			// load buildings
			SAssetIO::load("MyAssets/Buildings/building_06/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();

			T3DMesh<float>::AABB buildingAABB = M.aabb();
			M.computeAxisAlignedBoundingBox();
			m_buildingDiag[0] = buildingAABB.diagonal();
			m_buildingOrigin[0] = buildingAABB.Min + (m_buildingDiag[0] * 0.5f);
			m_box_building[0] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->y());

			m_Buildings[0].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Buildings/building_07/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();

			M.computeAxisAlignedBoundingBox();
			m_buildingDiag[1] = buildingAABB.diagonal();
			m_buildingOrigin[1] = buildingAABB.Min + (m_buildingDiag[1] * 0.5f);
			m_box_building[1] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->y());

			m_Buildings[1].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Buildings/building_08/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();

			M.computeAxisAlignedBoundingBox();
			m_buildingDiag[2] = buildingAABB.diagonal();
			m_buildingOrigin[2] = buildingAABB.Min + (m_buildingDiag[2] * 0.5f);
			m_box_building[2] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->y());

			/*	float diagx = buildingAABB.diagonal().x();
			float diagy = buildingAABB.diagonal().y();
			float diagz = buildingAABB.diagonal().z();
			float tmp = buildingAABB.Max.z() - buildingAABB.Min.z();*/
			m_Buildings[2].init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Sphere.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Sphere.init(&M);
			M.clear();

			m_BuildingGroupSGN.init(&m_RootSGN);

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

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());



		}//initialize

		void clear(void) override {
			ExampleSceneBase::clear();
		}//clear

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

		void translate_bird(Vector3f move) {
			m_BirdTransformSGN.translation(Vector3f(m_BirdTransformSGN.translation().x() + move.x(), m_BirdTransformSGN.translation().y() + move.y(), m_BirdTransformSGN.translation().z() + move.z()));
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

		void mainLoop(void)override {
			if (!glLoaded) {
				gladLoadGL();
				glLoaded = true;
			}

			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			// setup for collision detections
			auto solver_type = fcl::GJKSolverType::GST_LIBCCD;
			using fcl::Vector3;
			using Real = typename fcl::constants<float>::Real;
			const Real eps = fcl::constants<float>::eps();

			// set the AABBs for the buildings 
			const Vector3<float> half_size_building6{ m_buildingDiag[0] * 0.5 };
			const Vector3<float> half_size_building7{ m_buildingDiag[1] * 0.5 };
			const Vector3<float> half_size_building8{ m_buildingDiag[2] * 0.5 };

			// collision setup for bird
			float m_bird_diameter = 19.207;
			const Real r = m_bird_diameter * 0.5; // this is the diameter of the bird -> check aabb!

			Eigen::Vector3f BirdPos;
			Eigen::Quaternionf BirdRot;
			Eigen::Vector3f BirdScale;
			m_BirdTransformSGN.buildTansformation(&BirdPos, &BirdRot, &BirdScale);

			auto birbSphere = m_BirdSGN.actor()->boundingVolume().boundingSphere();
			float max_scale_birb = std::max(std::max(BirdScale.x(), BirdScale.y()), BirdScale.z());

			auto bird_collision = std::make_shared<fcl::Sphere<float>>(birbSphere.radius() * max_scale_birb);
			fcl::Transform3<float> X_WS = fcl::Transform3<float>::Identity();
			fcl::CollisionObject<float> bird_collision_geometry(bird_collision, X_WS);

			fcl::CollisionRequest<float> collision_request(1 /* num contacts */,
				true /* enable_contact */);
			collision_request.gjk_solver_type = solver_type;

			fcl::Transform3f bMat;
			bMat.rotate(BirdRot);
			bMat.translate(BirdPos + birbSphere.center());
			bird_collision_geometry.setTransform(bMat);

			auto evaluate_collision = [&](
				const fcl::CollisionObject<float>* s1, const fcl::CollisionObject<float>* s2) {
					// Compute collision.
					fcl::CollisionResult<float> collision_result;
					std::size_t contact_count = fcl::collide(s1, s2, collision_request, collision_result);

					// Test answers
					if (contact_count == collision_request.num_max_contacts) {
						std::vector<fcl::Contact<float>> contacts;
						collision_result.getContacts(contacts);
						//GTEST_ASSERT_EQ(contacts.size(), collision_request.num_max_contacts);
						m_col = true;
					}
					else {
						m_col = false;
					}
			};


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


				auto buildingAABB = m_BuildingSGNs[i]->actor()->boundingVolume().aabb();

				Eigen::Vector3f pos;
				Eigen::Quaternionf rot;
				Eigen::Vector3f scale;

				m_BuildingSGNs[i]->buildTansformation(&pos, &rot, &scale);

				Eigen::Vector3f buildingDiag = buildingAABB.diagonal().cwiseProduct(scale);
				Eigen::Vector3f buildingOrigin = buildingAABB.min().cwiseProduct(scale) + (buildingDiag * 0.5f);
				auto box_building = std::make_shared<fcl::Box<float>>(buildingDiag.x(), buildingDiag.y(), buildingDiag.y());

				fcl::Transform3<float> X_WB = fcl::Transform3<float>::Identity();
				fcl::CollisionObject<float> box(box_building, X_WB);

				fcl::Transform3f mat;
				mat.rotate(rot.toRotationMatrix());
				mat.translate(pos + buildingOrigin);
				box.setTransform(mat);

				evaluate_collision(&bird_collision_geometry, &box);
			}


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

			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT)) {
				if (rollSpeed < 3.0f) rollSpeed += 1.0f;
			}
			else {
				if (pKeyboard->keyPressed(Keyboard::KEY_RIGHT)) {
					if (rollSpeed > -3.0f) rollSpeed -= 1.0f;
				}
				else {
					if (rollSpeed < 0.0f) rollSpeed += 1.0f;
					if (rollSpeed > 0.0f) rollSpeed -= 1.0f;
				}
			}
			Quaternionf To_Y;
			To_Y = AngleAxis(CForgeMath::degToRad(rollSpeed / (10.0f * speed.z())), Vector3f::UnitY());
			m_BirdTransformSGN.rotation(m_BirdTransformSGN.rotation() * To_Y);

			Quaternionf To_Z;
			To_Z = AngleAxis(CForgeMath::degToRad(-rollSpeed * 4.0f), Vector3f::UnitZ());
			m_BirdRollSGN.rotation(To_Z);

			Vector3f pos;
			Vector3f xzdir;
			Quaternionf rot;
			Matrix3f m3;
			Vector3f dir;

			// gain and loose speed
			if (pKeyboard->keyPressed(Keyboard::KEY_UP) && speed.z() <= 0.5f)speed.z() += 0.01f;
			if (pKeyboard->keyPressed(Keyboard::KEY_DOWN) && speed.z() >= 0.3f)speed.z() -= 0.01f;

			// the bird sinks during normal flight and gains altitude when pressed space
			if (pKeyboard->keyPressed(Keyboard::KEY_SPACE, true)) speed.y() += 0.2;
			if (speed.y() > -0.01f) speed.y() -= 0.03f;

			// bird to near the ground -> remains altitude
			if (m_BirdTransformSGN.translation().y() < 0.05) speed.y() += 0.1f;

			// dive
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL)) speed.y() -= 0.01f;
			else if (speed.y() < -0.01f) speed.y() += 0.02f;

			if (speed.y() < -0.01f) {
				Quaternionf To_X;
				float pitchAngle = -speed.y() * 40.0f; if (pitchAngle > 80.0f) pitchAngle = 80.0f;
				To_X = AngleAxis(CForgeMath::degToRad(pitchAngle), Vector3f::UnitX());
				m_BirdPitchSGN.rotation(To_X);
			}

			// Bird is rotated in the direction where it is looking
			m3 = m_BirdTransformSGN.rotation().toRotationMatrix();
			dir.x() = m3(0, 0) * speed.x() + m3(0, 2) * speed.z();
			dir.y() = speed.y();
			dir.z() = m3(2, 0) * speed.x() + m3(2, 2) * speed.z();

			xzdir = Vector3f(dir.x(), 0, dir.z()).normalized();

			//Quaternionf rotate_left = AngleAxis(CForgeMath::degToRad(2.5f), dir);

			// in translation there is the postion
			printf("%f - %f - %f | %f\n", m_BirdTransformSGN.translation().x(), m_BirdTransformSGN.translation().y(), m_BirdTransformSGN.translation().z(), speed.y());

			m_BirdTransformSGN.translationDelta(dir);

			//defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			defaultCameraUpdateBird(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), dir, m_BirdTransformSGN.translation(), Vector3f(0.0f, 1.0f, 0.0f), m3);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			if (m_col)
				glColorMask(false, true, false, true);
			else
				glColorMask(true, false, false, true);

			Eigen::Vector3f posBird;
			Eigen::Quaternionf rotBird;
			Eigen::Vector3f scaleBird;
			m_BirdTransformSGN.buildTansformation(&posBird, &rotBird, &scaleBird);

			Eigen::Matrix4f posMatrix = CForgeMath::translationMatrix(posBird + birbSphere.center());
			Eigen::Matrix4f rotMatrix = CForgeMath::rotationMatrix(rotBird);
			Eigen::Matrix4f scaleMatrix = CForgeMath::scaleMatrix(Eigen::Vector3f(max_scale_birb, max_scale_birb, max_scale_birb));
			m_matSphere = posMatrix * rotMatrix * scaleMatrix;
			m_RenderDev.modelUBO()->modelMatrix(m_matSphere);
			m_Sphere.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());
			glColorMask(true, true, true, true);
			glDisable(GL_BLEND);

			// Skybox should be last thing to render
			m_SkyboxSG.render(&m_RenderDev);

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}

	protected:

		StaticActor m_Bird;
		SGNTransformation m_RootSGN;
		StaticActor m_Mountain;

		vector<string> m_ClearSky;
		vector<string> m_EmptySpace;
		vector<string> m_Techno;
		vector<string> m_BlueCloud;

		SkyboxActor m_Skybox;

		SGNGeometry m_BirdSGN;
		SGNTransformation m_BirdTransformSGN;
		SGNTransformation m_BirdRollSGN;
		SGNTransformation m_BirdPitchSGN;

		SGNGeometry m_MountainSGN;
		SGNTransformation m_MountainTransformSGN;
		SGNTransformation m_MountainTranslateSGN;

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

		//Speed für Vogel
		Vector3f speed = Vector3f(0.0f, 0.0f, 0.3f);
		float rollSpeed = 0.0f;


		// vectors for the buildings - check whether it works or not! 
		int m_building_asset[BUILDING_COUNT] = { -1, 1, 1, 0, 2, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 1, 0, 0, 2, 0, 2, 2, 0, 2 };
		Vector3f m_buildingOrigin[3];
		Vector3f m_buildingDiag[3];
		std::shared_ptr<fcl::Box<float>> m_box_building[3];

		StaticActor m_Sphere;
		Eigen::Matrix4f m_matSphere = Eigen::Matrix4f::Identity();

		bool m_col = false;
		bool glLoaded = false;

	};//ExampleBird

}

#endif