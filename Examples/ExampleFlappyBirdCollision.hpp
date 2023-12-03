
#ifndef __CFORGE_EXAMPLEFLAPPYBIRDCOLLISION_HPP__
#define __CFORGE_EXAMPLEFLAPPYBIRDCOLLISION_HPP__

#include "ExampleSceneBase.hpp"
#include "../crossforge/Graphics/Actors/SkyboxActor.h"
#include "../crossforge/MeshProcessing/PrimitiveShapeFactory.h"

#include "fcl/narrowphase/collision_object.h"
#include "fcl/narrowphase/distance.h"




using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleFlappyBirdCollision : public ExampleSceneBase {
	public:
		ExampleFlappyBirdCollision(void) {
			m_WindowTitle = "CrossForge Example - Bird";


		}//Constructor

		~ExampleFlappyBirdCollision(void) {
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
			SAssetIO::load("MyAssets/Ground/cloud.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			for (uint8_t i = 0; i < 4; ++i) M.textureCoordinate(i) *= 50.0f;
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Ground.init(&M);
			M.clear();
			//SAssetIO::load("MyAssets/Bird/bird.fbx", &M);
			//setMeshShader(&M, 0.1f, 0.04f);
			//M.computePerVertexNormals();
			//m_Bird.init(&M);
			//T3DMesh<float> M;
			SAssetIO::load("MyAssets/Kolibri/Kolibri.gltf", &M);
			setMeshShader(&M, 0.7f, 0.04f);
			M.computePerVertexNormals();
			m_BipedController.init(&M);
			m_Bird.init(&M, &m_BipedController);

			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB birdAABB = M.aabb();
			float birdBBSphereR = birdAABB.Min.norm();
			birdBBSphereR = birdBBSphereR > birdAABB.Min.norm() ? birdBBSphereR : birdAABB.Max.norm();

			M.clear();

			// calculate AABB for the bird
			//M.computeAxisAlignedBoundingBox();
			//T3DMesh<float>::AABB birdAABB = M.aabb();
			//float birdBBSphereR = birdAABB.Min.norm();
			//birdBBSphereR = birdBBSphereR > birdAABB.Min.norm() ? birdBBSphereR : birdAABB.Max.norm();
			//M.clear();

			m_RepeatAnimation = true;
			Quaternionf Rot;
			Rot = AngleAxisf(CForgeMath::degToRad(90.0f), Vector3f::UnitY());
			//m_BirdTransformSGN.rotation(Rot);
			// raven
			m_BirdTransformSGN.init(&m_RootSGN, m_startPosition);
			m_BirdTransformSGN.scale(Vector3f(6.0f, 6.0f, 6.0f));


			//m_BirdTransformSGN.rotation(Rot);
			m_BirdPitchSGN.init(&m_BirdTransformSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_BirdRollSGN.init(&m_BirdPitchSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_BirdYawSGN.init(&m_BirdRollSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_BirdYawSGN.rotation(Rot);
			//m_BirdTransformSGN.rotation(Rot);

			//Quaternionf To_Y1;
			//To_Y1 = AngleAxis(CForgeMath::degToRad(180.0f), Vector3f::UnitY());
			//m_BirdTransformSGN.rotation(Rot);
			m_BirdSGN.init(&m_BirdRollSGN, &m_Bird, Eigen::DenseBase<Eigen::Vector3f>::Zero(), Rot);
			m_BirdSGN.scale(Vector3f(0.05f, 0.05f, 0.05f)); // 
			m_birdSphere = m_BirdSGN.actor()->boundingVolume().boundingSphere();

			//different birds
			//SAssetIO::load("MyAssets/Bird/bird.fbx", &BirdMesh1);


			//SAssetIO::load("MyAssets/Eagle_Animated/EagleFlapFINAL/EagleFlap.gltf", &BirdMesh2); 
			


			// load buildings
			//weiß hoch
			SAssetIO::load("MyAssets/Buildings/building_06/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			T3DMesh<float>::AABB buildingAABBs = M.aabb();
			//setBuildingAABB(buildingAABB, 0);
			m_Buildings[0].init(&M);
			building_1.init(&M);
			M.clear();

			//weiß hoch mit kante
			SAssetIO::load("MyAssets/Buildings/building_07/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			buildingAABBs = M.aabb();
			//setBuildingAABB(buildingAABBs, 1);
			m_Buildings[1].init(&M);
			building_2.init(&M);
			M.clear();

			//rot klein
			//SAssetIO::load("MyAssets/Buildings/building_08/scene.gltf", &M);
			SAssetIO::load("MyAssets/Buildings/building_07/scene.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			buildingAABBs = M.aabb();
			//setBuildingAABB(buildingAABBs, 2);
			m_Buildings[2].init(&M);
			building_3.init(&M);
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


			//Textures for Skybox City 1

			/// gather textures for the skyboxes
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/right.bmp");
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/left.bmp");
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/up.bmp");
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/down.bmp");
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/back.bmp");
			m_City1.push_back("MyAssets/FlappyAssets/skybox1/front.bmp");

			//Textures for Skybox City 2
			m_City2.push_back("MyAssets/FlappyAssets/skybox/right.bmp");
			m_City2.push_back("MyAssets/FlappyAssets/skybox/left.bmp");
			m_City2.push_back("MyAssets/FlappyAssets/skybox/up.bmp");
			m_City2.push_back("MyAssets/FlappyAssets/skybox/down.bmp");
			m_City2.push_back("MyAssets/FlappyAssets/skybox/back.bmp");
			m_City2.push_back("MyAssets/FlappyAssets/skybox/front.bmp");



			// create actor and initialize
			m_Skybox.init(m_City1[0], m_City1[1], m_City1[2], m_City1[3], m_City1[4], m_City1[5]);

			// set initialize color adjustment values
			m_Skybox.brightness(1.15f);
			m_Skybox.contrast(1.1f);
			m_Skybox.saturation(1.2f);

			// create scene graph for the Skybox
			m_SkyboxTransSGN.init(nullptr);
			m_SkyboxGeomSGN.init(&m_SkyboxTransSGN, &m_Skybox);
			m_SkyboxSG.init(&m_SkyboxTransSGN);

			//weiß hoch
			//building_1_SGN.init(&m_RootSGN);
			//building_1_geo.init(&building_1_SGN, &building_1);

			//weiß hoch mit kante
			//building_2_SGN.init(&m_RootSGN);
			//building_2_geo.init(&building_2_SGN, &building_2);

			//rot klein
			//building_3_SGN.init(&m_RootSGN);
			//building_3_geo.init(&building_3_SGN, &building_3);

			vector<StaticActor*> buildings;
			buildings.push_back(&building_1);
			buildings.push_back(&building_2);
			buildings.push_back(&building_3);

			// Erstellen der Start-Area
			createStartingArea(-100.0f);
			createStartingArea(-50.0f);
			// Erstellen von Gebäude-Reihen
			for (int row = 0; row < 5; ++row) {
				float xOffset = row * 50.0f; // Abstand zwischen den Reihen
				createBuildingRow(xOffset);   // Methode zum Erstellen einer Gebäude-Reihe aufrufen
			}
			float AnimationSpeed = 1000 / 60.0f;
			SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
			m_Bird.activeAnimation(pAnim);

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: Up,Down,Left,Right  | Pause/Unpause: P | F3: Toggle help text");
			pKeybindings->color(1.0f, 1.0f, 1.0f, 1.0f);
			m_HelpTexts.push_back(pKeybindings);
			m_DrawHelpTexts = true;

			// create the Score label
			// position upper middle
			uint32_t FontSize = 30;
			//LineOfText* pText = nullptr;
			Font* pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 24, true, false);
			scoreLabel.init(pFont, "Score:");
			Vector2f LabelPos;
			LabelPos.x() = m_RenderWin.width() / 2 - pFont->computeStringWidth("Score: XXX");
			LabelPos.y() = 34;
			scoreLabel.position(LabelPos);

			Font* pFont2 = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 30, true, false);
			DeadLabel.init(pFont2, "Puhhh that was a close one, almost deadly...");
			DeadLabel.color(1.0f, 0.0f, 0.0f, 1.0f);
			Vector2f Label2Pos;
			Label2Pos.x() = m_RenderWin.width() / 2 - pFont2->computeStringWidth("Puhhh that was a close one, almost deadly...") / 2;
			Label2Pos.y() = 80;
			DeadLabel.position(Label2Pos);

			Font* pFontC = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 24, true, false);
			ContinueLabel.init(pFontC, "Press P to continue!");
			//RestartLabel.color(1.0f, 0.0f, 0.0f, 1.0f);
			Vector2f CLabelPos;
			CLabelPos.x() = m_RenderWin.width() / 2 - pFontC->computeStringWidth("Press P to continue!") / 2;
			CLabelPos.y() = (m_RenderWin.height() / 5) * 4;
			ContinueLabel.position(CLabelPos);

			Font* pFont3 = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 32, true, false);
			GameOverLabel.init(pFont3, "PLATSCH that is one bird less on the planet...");
			GameOverLabel.color(1.0f, 0.0f, 0.0f, 1.0f);
			Vector2f Label3Pos;
			Label3Pos.x() = m_RenderWin.width() / 2 - pFont3->computeStringWidth("PLATSCH that is one bird less on the planet...") / 2;
			Label3Pos.y() = 80;
			GameOverLabel.position(Label3Pos);

			Font* pFont4 = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 24, true, false);
			RestartLabel.init(pFont4, "Press SPACE to restart!");
			//RestartLabel.color(1.0f, 0.0f, 0.0f, 1.0f);
			Vector2f Label4Pos;
			Label4Pos.x() = m_RenderWin.width() / 2 - pFont4->computeStringWidth("Press SPACE to restart!") / 2;
			Label4Pos.y() = (m_RenderWin.height() / 5) * 4;
			RestartLabel.position(Label4Pos);

			m_CPCollisonCurrent = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void setBuildingAABB(T3DMesh<float>::AABB buildingAABB, int i) {
			/*m_buildingDiag[i] = buildingAABB.diagonal();
			//m_buildingOrigin[i] = buildingAABB.Min + (m_buildingDiag[i] * 0.5f);
			//m_buildingOrigin[i].y() = m_buildingOrigin[i].y() + 5.0f;
			/* m_buildingOrigin[i] = Eigen::Vector3f(
				(buildingAABB.Min.x() + buildingAABB.Max.x()) * 0.5f,
				(buildingAABB.Min.y() + buildingAABB.Max.y()) * 0.5f,
				(buildingAABB.Min.z() + buildingAABB.Max.z()) * 0.5f
			);
			m_buildingOrigin[i] = Eigen::Vector3f(
				buildingAABB.Min.x() + m_buildingDiag[i].x() * 0.5f,
				buildingAABB.Min.y() + m_buildingDiag[i].y() * 0.5f - 0.5f,
				buildingAABB.Min.z() + m_buildingDiag[i].z() * 0.5f -1.0f
			);
			
			// Optionally, adjust the origin if needed (e.g., add height)
			//m_buildingOrigin[i].y() += 1.0f; // Adjusted for height
			m_box_building[i] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->z()); */

			m_buildingDiag[i] = buildingAABB.diagonal();

			// Calculate the translation vector to shift the AABB origin
			Eigen::Vector3f translationVector = buildingAABB.Min;
			
			// Update the translation part of the AABB's transformation matrix
			if (i == 2) {
				m_buildingOrigin[i] = Eigen::Vector3f(
					-0.5,
					1.1,
					m_buildingDiag[i].z() * 0.5f
				);
				
			}
			else if (i == 3) {
				m_buildingOrigin[i] = Eigen::Vector3f(
					-1,
					-0.5,
					m_buildingDiag[i].z() * 0.5f
				);
				m_buildingDiag[3].x() -= 1.5f;
				m_buildingDiag[3].z() -= 1.5f;
			}
			else {
				m_buildingOrigin[i] = Eigen::Vector3f(
					-0.55,
					1,
					m_buildingDiag[i].z() * 0.5f
				);
			}
			
			
			m_box_building[i] = std::make_shared<fcl::Box<float>>(m_buildingDiag->x(), m_buildingDiag->y(), m_buildingDiag->z());
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
			const Vector3<float> half_size_building9{ m_buildingDiag[3] * 0.5 };

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
			bMat.translate(BirdPos);
			bMat.rotate(BirdRot);

			m_birdTestCollision = bMat.matrix();
			bird_collision_geometry.setTransform(bMat);

			m_col = false;
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

			// vector<SGNTransformation*> m_BuildingSGNs; // List to hold building SGNs
			// vector<SGNGeometry*> m_BuildingGeoSGNs;

			int buildingAssetsIdx = 0;
			for (size_t i = 0; i < m_BuildingGeoModels.size(); i++) {

				///////////////////////////////////////////////
				// TODO: specify model in order to get origin//
				///////////////////////////////////////////////
				int model = std::get<1>(m_BuildingGeoModels[i]);


				// get transformnode - m_BuildingTransformationSGNs[i];
				auto buildingTSGN = std::get<0>(m_BuildingGeoModels[i]);

				Eigen::Vector3f posBuilding;
				Eigen::Quaternionf rotBuilding;
				Eigen::Vector3f scaleBuilding;

				buildingTSGN->buildTansformation(&posBuilding, &rotBuilding, &scaleBuilding);
				auto buildingAABB = m_BuildingGeoSGNs[i]->actor()->boundingVolume().aabb();

				Eigen::Vector3f buildingDiag = buildingAABB.diagonal().cwiseProduct(scaleBuilding);
				Eigen::Vector3f buildingOrigin = buildingAABB.min().cwiseProduct(scaleBuilding) + (buildingDiag * 0.5f);
				auto box_building = std::make_shared<fcl::Box<float>>(buildingDiag.x(), buildingDiag.y(), buildingDiag.z());

				fcl::Transform3<float> X_WB = fcl::Transform3<float>::Identity();
				fcl::CollisionObject<float> building_collisoin_geometry(box_building, X_WB);

				fcl::Transform3f mat = fcl::Transform3f::Identity();
				mat.translate(posBuilding + m_buildingOrigin[model].cwiseProduct(scaleBuilding));
				rotBuilding = Quaternionf(rotBuilding.w(), rotBuilding.x(), rotBuilding.y(), rotBuilding.z());
				mat.rotate(rotBuilding);

				// can be used as l-value because it is a reference
				std::get<2>(m_BuildingGeoModels[i]) = mat.matrix() * CForgeMath::scaleMatrix(Vector3f(buildingDiag.x(), buildingDiag.y(), buildingDiag.z()));
				building_collisoin_geometry.setTransform(mat);

				evaluate_collision(&bird_collision_geometry, &building_collisoin_geometry, &m_col);
			}
		}

		void ExampleFlappyBirdCollision::createStartingArea(float xOffset) {
			////////////////////////////////////////////////////////////////////////////////
			// Erstelle Boden-Transformation-SGN
			SGNTransformation* GroundTransformSGN = new SGNTransformation();
			GroundTransformSGN->init(&m_RootSGN);
			GroundTransformSGN->translation(Vector3f(0.0f, -0.5f, xOffset));
			// ... Weitere Transformationsoperationen für die linke Seitenwand ...
			m_GroundBuildingSGNs.push_back(GroundTransformSGN);

			// Boden-Geometrie-SGN 
			SGNGeometry* GroundGeoSGN = new SGNGeometry();
			GroundGeoSGN->init(GroundTransformSGN, &m_Ground);
			GroundGeoSGN->scale(Vector3f(0.75f, 0.75f, 0.835f)); // Nur Skalierung entlang der z-Achse
			m_GroundBuildingGeoSGNs.push_back(GroundGeoSGN);

			////////////////////////////////////////////////////////////////////////////////
			// Erstelle linke Seitenwand-Transformation-SGN
			SGNTransformation* leftSidewallTransformSGN = new SGNTransformation();
			leftSidewallTransformSGN->init(&m_RootSGN);
			leftSidewallTransformSGN->translation(Vector3f(17.0f, 0.0f, xOffset));
			// ... Weitere Transformationsoperationen für die linke Seitenwand ...
			m_SideBuildingSGNs.push_back(leftSidewallTransformSGN);

			// Geometrie-SGN für die linke Seitenwand
			SGNGeometry* leftSidewallGeoSGN = new SGNGeometry();
			leftSidewallGeoSGN->init(leftSidewallTransformSGN, &building_2);
			leftSidewallGeoSGN->scale(Vector3f(5.0f, 5.0f, 50.0f)); // Nur Skalierung entlang der z-Achse
			m_SideBuildingGeoSGNs.push_back(leftSidewallGeoSGN);

			////////////////////////////////////////////////////////////////////////////////
			// Erstelle rechte Seitenwand-Transformation-SGN
			SGNTransformation* rightSidewallTransformSGN = new SGNTransformation();
			rightSidewallTransformSGN->init(&m_RootSGN);
			rightSidewallTransformSGN->translation(Vector3f(-11.0f, 0.0f, xOffset));
			// ... Weitere Transformationsoperationen für die rechte Seitenwand ...
			m_SideBuildingSGNs.push_back(rightSidewallTransformSGN);

			// Geometrie-SGN für die rechte Seitenwand
			SGNGeometry* rightSidewallGeoSGN = new SGNGeometry();
			rightSidewallGeoSGN->init(rightSidewallTransformSGN, &building_2);
			rightSidewallGeoSGN->scale(Vector3f(5.0f, 5.0f, 50.0f)); // Nur Skalierung entlang der z-Achse
			m_SideBuildingGeoSGNs.push_back(rightSidewallGeoSGN);
		}

		void ExampleFlappyBirdCollision::createBuildingRow(float xOffset) {
			float buildingSpacing = 5.0f; // Abstand zwischen den Gebäuden in einer Reihe
			float buildingWidth = 3.0f; // Breite eines Gebäudes

			// Calculate total row width
			float totalRowWidth = 3 * buildingWidth + 2 * buildingSpacing;

			// Zufällig auswählen, welche Position das Gebäude vom Typ building_2 haben soll
			int building2Position = rand() % 3;

			////////////////////////////////////////////////////////////////////////////////
			// Erstelle Boden-Transformation-SGN
			SGNTransformation* GroundTransformSGN = new SGNTransformation();
			GroundTransformSGN->init(&m_RootSGN);
			GroundTransformSGN->translation(Vector3f(0.0f, -0.5f, xOffset));
			// ... Weitere Transformationsoperationen für die linke Seitenwand ...
			m_GroundBuildingSGNs.push_back(GroundTransformSGN);

			// Boden-Geometrie-SGN 
			SGNGeometry* GroundGeoSGN = new SGNGeometry();
			GroundGeoSGN->init(GroundTransformSGN, &m_Ground);
			GroundGeoSGN->scale(Vector3f(0.75f, 0.75f, 0.835f)); // Nur Skalierung entlang der z-Achse
			m_GroundBuildingGeoSGNs.push_back(GroundGeoSGN);

			////////////////////////////////////////////////////////////////////////////////
			// Erstelle linke Seitenwand-Transformation-SGN
			SGNTransformation* leftSidewallTransformSGN = new SGNTransformation();
			leftSidewallTransformSGN->init(&m_RootSGN);
			leftSidewallTransformSGN->translation(Vector3f(17.0f, 0.0f, xOffset));
			// ... Weitere Transformationsoperationen für die linke Seitenwand ...
			m_SideBuildingSGNs.push_back(leftSidewallTransformSGN);

			// Geometrie-SGN für die linke Seitenwand
			SGNGeometry* leftSidewallGeoSGN = new SGNGeometry();
			leftSidewallGeoSGN->init(leftSidewallTransformSGN, &building_2);
			leftSidewallGeoSGN->scale(Vector3f(5.0f, 5.0f, 50.0f)); // Nur Skalierung entlang der z-Achse
			m_SideBuildingGeoSGNs.push_back(leftSidewallGeoSGN);

			////////////////////////////////////////////////////////////////////////////////
			// Erstelle rechte Seitenwand-Transformation-SGN
			SGNTransformation* rightSidewallTransformSGN = new SGNTransformation();
			rightSidewallTransformSGN->init(&m_RootSGN);
			rightSidewallTransformSGN->translation(Vector3f(-11.0f, 0.0f, xOffset));
			// ... Weitere Transformationsoperationen für die rechte Seitenwand ...
			m_SideBuildingSGNs.push_back(rightSidewallTransformSGN);

			// Geometrie-SGN für die rechte Seitenwand
			SGNGeometry* rightSidewallGeoSGN = new SGNGeometry();
			rightSidewallGeoSGN->init(rightSidewallTransformSGN, &building_2);
			rightSidewallGeoSGN->scale(Vector3f(5.0f, 5.0f, 50.0f)); // Nur Skalierung entlang der z-Achse
			m_SideBuildingGeoSGNs.push_back(rightSidewallGeoSGN);


			for (int i = 0; i < 3; ++i) {
				// Hier Gebäude für eine Reihe erstellen und in der Liste speichern
				SGNTransformation* buildingTransformSGN = new SGNTransformation();
				buildingTransformSGN->init(&m_RootSGN);
				//buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 5.0f, 0.0f, xOffset));
				bool querTest = false;
				if (i == building2Position) {
					int Version = rand() % 3;
					
					if (Version == 1) {
						Quaternionf querRot;
						querRot = AngleAxisf(CForgeMath::degToRad(90.0f), Vector3f::UnitZ());
						buildingTransformSGN->scale(Vector3f(4.0f, 4.0f, 4.0f));
						buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 3.0, 5.0f, xOffset));
						buildingTransformSGN->rotation(querRot);
						querTest = true;

					}
					else if (Version == 2 && (building2Position == 2 || building2Position == 1 || building2Position == 0)) {
						Quaternionf querRot;
						querRot = AngleAxisf(CForgeMath::degToRad(90.0f), Vector3f::UnitZ());
						buildingTransformSGN->scale(Vector3f(0.75f, 0.75f, 0.75f).cwiseProduct(Vector3f(5.0f, 5.0f, 5.0f)));
						buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 4.0, 12.0f, xOffset));
						buildingTransformSGN->rotation(querRot);
						querTest = true;
						
					}
					else if (Version == 2) {
						Quaternionf querRot;
						querRot = AngleAxisf(CForgeMath::degToRad(90.0f), Vector3f::UnitZ());
						buildingTransformSGN->scale(Vector3f(0.75f, 0.75f, 0.75f).cwiseProduct(Vector3f(5.0f, 5.0f, 5.0f)));
						buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 5.0, 12.0f, xOffset));
						buildingTransformSGN->rotation(querRot);
						querTest = true;
					}
					else {
						buildingTransformSGN->scale(Vector3f(5.0f, 5.0f, 5.0f));
						buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 5.0f, -10.0f, xOffset));
					}


				}
				else {
					buildingTransformSGN->scale(Vector3f(5.0f, 5.0f, 5.0f));
					buildingTransformSGN->translation(Vector3f(i * (buildingWidth + buildingSpacing) - 5.0f, 0.0f, xOffset));
				}

				//m_BuildingSGNs.push_back(buildingTransformSGN);
				// 

				// Füge die Transformationen zur aktuellen Gebäudereihe hinzu
				m_BuildingSGNs.push_back(buildingTransformSGN);


				// Hier die Geometrie-SGN für das Gebäude erstellen und in der Liste speichern
				SGNGeometry* buildingGeoSGN = new SGNGeometry();
				//buildingGeoSGN->init(buildingTransformSGN, &building_1);

				// Entscheiden, ob das Gebäude building_2 ist oder nicht
				if (i == building2Position) {
					if (querTest) {
						buildingGeoSGN->init(buildingTransformSGN, &building_3);
						//buildingTransformSGN->translation(Vector3f(0.0f, -5.0f, 0.0f));
						m_BuildingGeoModels.push_back({ buildingTransformSGN,3, Eigen::Matrix4f() });
					}
					else {
						buildingGeoSGN->init(buildingTransformSGN, &building_2);
						//buildingTransformSGN->translation(Vector3f(0.0f, -5.0f, 0.0f));
						m_BuildingGeoModels.push_back({ buildingTransformSGN,2, Eigen::Matrix4f() });
					}

				}
				else {
					buildingGeoSGN->init(buildingTransformSGN, &building_1);
					m_BuildingGeoModels.push_back({ buildingTransformSGN,1, Eigen::Matrix4f() });
				}

				buildingGeoSGN->scale(Vector3f(1.0f, 1.0f, 1.0f)); // Keine Skalierung

				// Geometrie-SGN zur Liste hinzufügen
				//m_BuildingSGNs.push_back(buildingTransformSGN);
				m_BuildingGeoSGNs.push_back(buildingGeoSGN);
				// Setze AABB-Informationen für das aktuelle Gebäude
				auto buildingAABB = buildingGeoSGN->actor()->getAABB();
				if (querTest) {
					setBuildingAABB(buildingAABB, 3);
				}
				else {
					setBuildingAABB(buildingAABB, i);
				}
				//setBuildingAABB(buildingAABB, i);
				


			}
		}

		void ExampleFlappyBirdCollision::removePassedBuildingRow() {

		}

		void clear(void) override {
			ExampleSceneBase::clear();

			for (SGNGeometry* buildingGeoSGN : m_BuildingGeoSGNs) {
				delete buildingGeoSGN;
			}
			m_BuildingGeoSGNs.clear();

			for (SGNTransformation* buildingTransformSGN : m_BuildingSGNs) {
				delete buildingTransformSGN;
			}
			m_BuildingSGNs.clear();
			for (SGNGeometry* buildingGeoSGN : m_SideBuildingGeoSGNs) {
				delete buildingGeoSGN;
			}
			m_SideBuildingGeoSGNs.clear();

			for (SGNTransformation* buildingTransformSGN : m_SideBuildingSGNs) {
				delete buildingTransformSGN;
			}
			m_SideBuildingSGNs.clear();
			for (SGNGeometry* buildingGeoSGN : m_GroundBuildingGeoSGNs) {
				delete buildingGeoSGN;
			}
			m_GroundBuildingGeoSGNs.clear();

			for (SGNTransformation* buildingTransformSGN : m_GroundBuildingSGNs) {
				delete buildingTransformSGN;
			}
			m_GroundBuildingSGNs.clear();
			// GUI-Aufräumen (Kopiere den entsprechenden Code aus GUITestScene)
			m_RenderWin.closeWindow();

		}//clear

		void translate_bird(Vector3f move) {
			m_BirdTransformSGN.translation(Vector3f(m_BirdTransformSGN.translation().x() + move.x(), m_BirdTransformSGN.translation().y() + move.y(), m_BirdTransformSGN.translation().z() + move.z()));
		}

		void checkCollisionBuilding() {
			// penalty for colding with building
			if (!m_colLastFrame && m_col) {
				// first instance of collision
				m_colLastFrame = true;
				if (m_CPCollisonCurrent < 2) {
					
					m_CPCollisonCurrent += 1;
					m_paused = !m_paused;
					m_collision = !m_collision;
					if (m_paused != true) m_speed.z() = oldSpeed + 0.01f;
					translate_bird(Vector3f(0.0f, 0.0f, -25.0f));

				}
				else {
					//Game Over
					m_paused = !m_paused;
					m_gameOver = !m_gameOver;
					translate_bird(Vector3f(0.0f, 0.0f, -25.0f));
					//cout << "PLATSCH, thats one bird less on the planet :(" << endl;
				}
				//m_CPCollisonCurrent += 1;
				//score -= 2;
				//cout << "Ouch that hurts :("<< endl;
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

		enum CameraMode {
			Bird,
			Default
		};

		CameraMode currentCameraMode = CameraMode::Bird;

		void updateCamera(Keyboard* keyBoard) {
			Keyboard* pKeyboard = keyBoard;

			Vector3f pos;
			Vector3f xzdir;
			Quaternionf rot;
			Matrix3f m3;
			Vector3f dir;
			// Bird is rotated in the direction where it is looking
			m3 = m_BirdTransformSGN.rotation().toRotationMatrix();
			dir.x() = m3(0, 0) * m_speed.x() + m3(0, 2) * m_speed.z();
			dir.y() = m_speed.y();
			dir.z() = m3(2, 0) * m_speed.x() + m3(2, 2) * m_speed.z();

			xzdir = Vector3f(dir.x(), 0, dir.z()).normalized();

			//Quaternionf rotate_left = AngleAxis(CForgeMath::degToRad(2.5f), dir);

			// in translation there is the postion
			//printf("%f - %f - %f | %f\n", m_BirdTransformSGN.translation().x(), m_BirdTransformSGN.translation().y(), m_BirdTransformSGN.translation().z(), speed.y());
			m_BirdTransformSGN.translationDelta(dir);

			// Check for the key to toggle camera mode
			if (pKeyboard->keyPressed(Keyboard::KEY_L, true)) {
				// Toggle camera mode here
				if (currentCameraMode == CameraMode::Default) {
					currentCameraMode = CameraMode::Bird;
				}
				else if (currentCameraMode == CameraMode::Bird) {
					currentCameraMode = CameraMode::Default;
				}
			}

			if (currentCameraMode == CameraMode::Bird) {
				defaultCameraUpdateBird(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), dir, m_BirdTransformSGN.translation(), Vector3f(0.0f, 1.0f, 0.0f), m3);
			}
			else if (currentCameraMode == CameraMode::Default) {
				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			}
		}

		void restartGame(void) {
			
			// Erstellen der Start-Area
			//createStartingArea(-100.0f);
			//createStartingArea(-50.0f);
			// Erstellen von Gebäude-Reihen
			//for (int row = 0; row < 5; ++row) {
			//	float xOffset = row * 50.0f; // Abstand zwischen den Reihen
			//	createBuildingRow(xOffset);   // Methode zum Erstellen einer Gebäude-Reihe aufrufen
			//}
			score = 0;
			m_BirdTransformSGN.translation(Vector3f(0.0f, 10.0f, m_BirdTransformSGN.translation().z()));
			m_CPCollisonCurrent = 0;
			m_gameOver = false;
			m_speed.z() = 0.3;
			
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

			checkCollisionBuilding();
			//Animation
			static uint64_t lastFrameTime = CForgeUtility::timestamp();

			// this will progress all active skeletal animations for this controller
			m_BipedController.update(60.0f / m_FPS);
			if (m_RepeatAnimation && nullptr != m_Bird.activeAnimation()) {
				auto* pAnim = m_Bird.activeAnimation();
				if (pAnim->t >= pAnim->Duration) pAnim->t -= pAnim->Duration;
			}

			// handle input for the skybox
			Keyboard* pKeyboard = m_RenderWin.keyboard();
			//float AnimationSpeed = 1000 / 60.0f;
			float Step = (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) ? -0.05f : 0.05f;

			//if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_G, true)) {
			//	SkeletalAnimationController::Animation* pAnim = m_BipedController.createAnimation(0, AnimationSpeed, 0.0f);
			//	m_Bird.activeAnimation(pAnim);

			//}
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
			//Toggle Help Text
			if (pKeyboard->keyPressed(Keyboard::KEY_F3, true)) {
				m_DrawHelpTexts = !m_DrawHelpTexts;
			}
			//CAMERA SWITCH 
			updateCamera(pKeyboard);
			//SKYBOX SWITCH
			if (pKeyboard->keyPressed(Keyboard::KEY_F1, true)) m_Skybox.init(m_City1[0], m_City1[1], m_City1[2], m_City1[3], m_City1[4], m_City1[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_F2, true)) m_Skybox.init(m_City2[0], m_City2[1], m_City2[2], m_City2[3], m_City2[4], m_City2[5]);



			// Handle left and right movement
			if (pKeyboard->keyPressedConst(Keyboard::KEY_LEFT)) {
				m_speed.x() += 0.01f; // Move left
			}
			else if (pKeyboard->keyPressedConst(Keyboard::KEY_RIGHT)) {
				m_speed.x() -= 0.01f; // Move right
			}
			else {
				m_speed.x() = 0.0f; // Stop horizontal movement when no keys are pressed
			}



			// Calculate and limit roll angle based on speed
			float rollAngle = -m_speed.x() * 40.0f; // Negative sign for correct roll direction
			rollAngle = std::clamp(rollAngle, -maxRollAngle, maxRollAngle);

			// Apply roll rotation
			Quaternionf rollRotation;
			rollRotation = AngleAxis(CForgeMath::degToRad(rollAngle), Vector3f::UnitZ());
			m_BirdRollSGN.rotation(rollRotation);

			// Handle up and down movement
			if (pKeyboard->keyPressedConst(Keyboard::KEY_DOWN)) {
				m_speed.y() -= 0.01f; // Move down
			}
			else if (pKeyboard->keyPressedConst(Keyboard::KEY_UP)) {
				m_speed.y() += 0.03f; // Move up
			}
			else {
				if (m_BirdTransformSGN.translation().y() <= maxVerticalPosition) {
					m_speed.y() = 0.0f; // Apply a small upward force when not pressing up
				}

				if (m_BirdTransformSGN.translation().y() >= maxVerticalPosition && m_speed.y() > 0.0f) {
					m_speed.y() = 0.0f; // Stop upward movement at the top
				}
				// Gradually reduce downward speed
				else if (m_speed.y() < -0.01f) {
					m_speed.y() += 0.02f;
				}
			}

			// Limit the vertical speed
			//m_speed.y() = std::clamp(m_speed.y(), -0.2f, 0.2f);
			// Limit the vertical speed
			m_speed.y() = std::clamp(m_speed.y(), -0.2f, 0.2f); // Beispielwerte für die vertikale Geschwindigkeit
			// Limit the horizontal speed
			m_speed.x() = std::clamp(m_speed.x(), -0.2f, 0.2f); // Beispielwerte für die horizontale Geschwindigkeit



			// Calculate and limit pitch angle based on speed
			float pitchAngle = m_speed.y() * 40.0f;


			// Calculate additional pitch angle based on bird's forward speed
			float forwardPitchAngle = m_speed.z() * -20.0f; // Adjust the multiplier as needed

			// Combine both pitch angles
			//pitchAngle += forwardPitchAngle;

			// Limit the pitch angle if the bird is moving very fast
			if (m_speed.y() > 10.0f) {
				pitchAngle = std::clamp(pitchAngle, -maxPitchAngle, maxPitchAngle);
			}

			// Apply pitch rotation
			Quaternionf pitchRotation;
			pitchRotation = AngleAxis(-CForgeMath::degToRad(pitchAngle), Vector3f::UnitX());
			m_BirdPitchSGN.rotation(pitchRotation);


			// Calculate the new horizontal position based on the speed
			float newHorizontalPosition = m_BirdTransformSGN.translation().x() + m_speed.x();

			// Check if the new horizontal position exceeds boundaries
			if (newHorizontalPosition > maxHorizontalPosition) {
				newHorizontalPosition = maxHorizontalPosition;
				if (m_speed.x() > 0.0f) {
					m_speed.x() = 0.0f; // Stop horizontal movement only if moving to the right
				}
			}
			else if (newHorizontalPosition < minHorizontalPosition) {
				newHorizontalPosition = minHorizontalPosition;
				if (m_speed.x() < 0.0f) {
					m_speed.x() = 0.0f; // Stop horizontal movement only if moving to the left
				}
			}

			// Update the bird's horizontal position
			m_BirdTransformSGN.translation(Vector3f(newHorizontalPosition, m_BirdTransformSGN.translation().y(), m_BirdTransformSGN.translation().z()));

			float newVerticalPosition = m_BirdTransformSGN.translation().y() + m_speed.y();

			// Check if the new vertical position exceeds boundaries
			if (newVerticalPosition > maxVerticalPosition) {
				newVerticalPosition = maxVerticalPosition;
				if (m_speed.y() > 0.0f) {
					m_speed.y() = 0.0f; // Stop vertical movement only if moving upwards
				}
			}
			else if (newVerticalPosition < minVerticalPosition) {
				newVerticalPosition = minVerticalPosition;
				if (m_speed.y() < 0.0f) {
					m_speed.y() = 0.0f; // Stop vertical movement only if moving downwards
				}
			}

			// Update the bird's vertical position
			m_BirdTransformSGN.translation(Vector3f(m_BirdTransformSGN.translation().x(), newVerticalPosition, m_BirdTransformSGN.translation().z()));
			if (pKeyboard->keyPressed(Keyboard::KEY_P, true) && !m_gameOver) {
				m_paused = !m_paused;
				m_collision = false;
				if (m_paused != true) m_speed.z() = oldSpeed + 0.01f;
			}
			if (m_paused) {
				m_speed.z() = pauseSpeed;
			}
			if (!m_paused) {
				// Handle speed adjustments
				if (pKeyboard->keyPressed(Keyboard::KEY_SPACE) && m_speed.z() <= 1.0f) {
					m_speed.z() += 0.01f; // Increase speed
				}
				if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_CONTROL) && m_speed.z() >= 0.05f) {
					m_speed.z() -= 0.01f; // Decrease speed
				}
				oldSpeed = m_speed.z();
				// Sink during normal flight
				if (m_speed.y() > -0.01f) {
					m_speed.y() -= 0.02f;
				}
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_SPACE) && m_gameOver) {
				restartGame();
			}


			////
			//Gebäude Check und Score anpassen
			////
			if (m_BuildingSGNs.size() > 0 && m_GroundBuildingSGNs.size() > 0 && m_SideBuildingSGNs.size() > 0) {
				float birdZ = m_BirdTransformSGN.translation().z();
				float firstBuildingZ = m_GroundBuildingSGNs[0]->translation().z();
				//float firstBuildingZ = m_SideBuildingSGNs[0]->translation().z();
				//float firstBuildingZ = m_BuildingSGNs[0]->translation().z();

				// Überprüfen, ob der Vogel die erste Reihe passiert hat
				if (birdZ > firstBuildingZ + 105.0f) {
					score++;
					//cout << "Score: " << score << endl;
					if (m_speed.z() < 0.80 && !m_paused) m_speed.z() += 0.01f;
					cout << "Aktueller Speed: " << m_speed.z() << endl;

					// Löschen der ersten Reihe
					if (birdZ > m_BuildingSGNs[0]->translation().z() + 105.0f) {
						for (int i = 0; i < 3; ++i) {
							delete m_BuildingGeoSGNs.front();
							delete m_BuildingSGNs.front();
							m_BuildingGeoSGNs.erase(m_BuildingGeoSGNs.begin());
							m_BuildingSGNs.erase(m_BuildingSGNs.begin());
						}
					}

					for (int j = 0; j < 2; ++j) {
						delete m_SideBuildingGeoSGNs.front();
						delete m_SideBuildingSGNs.front();
						m_SideBuildingGeoSGNs.erase(m_SideBuildingGeoSGNs.begin());
						m_SideBuildingSGNs.erase(m_SideBuildingSGNs.begin());
					}

					delete m_GroundBuildingGeoSGNs.front();
					delete m_GroundBuildingSGNs.front();
					m_GroundBuildingGeoSGNs.erase(m_GroundBuildingGeoSGNs.begin());
					m_GroundBuildingSGNs.erase(m_GroundBuildingSGNs.begin());

					m_BuildingGeoModels.erase(m_BuildingGeoModels.begin(), m_BuildingGeoModels.begin() + 3); // the fancy way


					// Hinzufügen von zwei neuen Reihen hinten
					//float lastBuildingZ = m_BuildingSGNs.back()->translation().z();
					float lastBuildingZ = m_GroundBuildingSGNs.back()->translation().z();

					float newRowsStartZ = lastBuildingZ + 50.0f; // Abstand zwischen den neuen Reihen
					createBuildingRow(newRowsStartZ);
					//createBuildingRow(newRowsStartZ + 50.0f); // Abstand zwischen den neuen Reihen
				}
			}



			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
			m_SG.render(&m_RenderDev);
			//m_CheckpointsSG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);




			//gui.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
#

			//glEnable(GL_BLEND);
			// debug bird

			//Eigen::Vector3f posBird;
			//Eigen::Quaternionf rotBird;
			//Eigen::Vector3f scaleBird;
			//m_BirdTransformSGN.buildTansformation(&posBird, &rotBird, &scaleBird);

			//Eigen::Matrix4f scaleMatrix = CForgeMath::scaleMatrix(Eigen::Vector3f(m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius())));

			/*
			if (pKeyboard->keyPressed(Keyboard::KEY_9)) {
				// bird
				glDisable(GL_DEPTH_TEST);
				glBlendFunc(GL_ONE, GL_ONE);

				if (!m_col)
					glColorMask(true, false, false, true);
				else
					glColorMask(false, true, false, true);

				m_RenderDev.modelUBO()->modelMatrix(m_birdTestCollision * scaleMatrix);
				m_Sphere.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());




			}*/

			/*if (m_colCP)
				glColorMask(false, false, true, true);
			else
				glColorMask(false, true, false, true);
			*/

			glEnable(GL_BLEND);
			//glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_ONE, GL_ONE);

			Eigen::Vector3f posBird;
			Eigen::Quaternionf rotBird;
			Eigen::Vector3f scaleBird;
			m_BirdTransformSGN.buildTansformation(&posBird, &rotBird, &scaleBird);

			Eigen::Matrix4f scaleMatrix = CForgeMath::scaleMatrix(Eigen::Vector3f(m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius()), m_max_scale_bird * (2 * m_birdSphere.radius())));

			if (m_col)
				glColorMask(true, false, false, true);
			else
				glColorMask(false, true, false, true);

			m_RenderDev.modelUBO()->modelMatrix(m_birdTestCollision * scaleMatrix);
			m_Sphere.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());

			int model = 0;
			int buildingCollisionIdx = 0;
			for (size_t i = 0; i < m_BuildingGeoModels.size(); i++)
			{
				model = std::get<1>(m_BuildingGeoModels[i]); //m_BuildingGeoModels[i].second;
				buildingCollisionIdx++;

				//auto building = std::get<0>(m_BuildingGeoModels[i]);

				m_RenderDev.modelUBO()->modelMatrix(std::get<2>(m_BuildingGeoModels[i]));
				m_Cube.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());

			}

			glColorMask(true, true, true, true);
			//glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			// Skybox should be last thing to render
			m_SkyboxSG.render(&m_RenderDev);

			// update and draw FPS label
			std::string LabelText = "Score: " + std::to_string(int32_t(score));
			scoreLabel.text(LabelText);
			scoreLabel.render(&m_RenderDev);
			if (m_paused) {

			std::string DeadText = "Puhhh that was a close one, almost deadly...";
			std::string DeadText2 = "Another one and you probably end up as a trophy on someones building...";
			DeadLabel.text(DeadText2);
			if (m_CPCollisonCurrent == 1) {
				DeadLabel.text(DeadText);
			}
			std::string ContinueText = "Press P to continue!";
			ContinueLabel.text(ContinueText);

			if (m_collision) {
				DeadLabel.render(&m_RenderDev);
				ContinueLabel.render(&m_RenderDev);
			}

			DeadLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
			// reposition Dead Label
			Vector2f DLabelPos;
			DLabelPos.x() = m_RenderWin.width() / 2 - DeadLabel.font()->computeStringWidth("Another one and you probably end up as a trophy on someones building...") / 2;
			if (m_CPCollisonCurrent == 1) {
				DLabelPos.x() = m_RenderWin.width() / 2 - DeadLabel.font()->computeStringWidth("Puhhh that was a close one, almost deadly...") / 2;
			}
			DLabelPos.y() = 80;
			DeadLabel.position(DLabelPos);

			std::string GameOverText = "PLATSCH that is one bird less on the planet...";
			GameOverLabel.text(GameOverText);
			std::string RestartText = "Press SPACE to restart!";
			RestartLabel.text(RestartText);
			if (m_gameOver) {
				GameOverLabel.render(&m_RenderDev);
				RestartLabel.render(&m_RenderDev);
			}
		}

			//DeadLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();

			wchar_t text_wstring[10] = { 0 };
			int charcount = swprintf(text_wstring, 10, L"score: %d\nZw", score);
			std::u32string text;
			//ugly cast to u32string from wchar[]
			for (int i = 0; i < charcount; i++) {
				text.push_back((char32_t)text_wstring[i]);
			}

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}

		virtual void listen(GLWindowMsg Msg) override {
			ExampleSceneBase::listen(Msg);

			// we have to notify the labels if canvas size changes
			if (GLWindowMsg::MC_RESIZE == Msg.Code) {

				scoreLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				// reposition score Label
				Vector2f SLabelPos;
				SLabelPos.x() = m_RenderWin.width() / 2 - scoreLabel.font()->computeStringWidth("Score: XXX");
				SLabelPos.y() = 34;
				scoreLabel.position(SLabelPos);

				DeadLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				// reposition Dead Label
				Vector2f DLabelPos;
				DLabelPos.x() = m_RenderWin.width() / 2 - DeadLabel.font()->computeStringWidth("Another one and you probably end up as a trophy on someones building...") / 2;
				if (m_CPCollisonCurrent == 1) {
					DLabelPos.x() = m_RenderWin.width() / 2 - DeadLabel.font()->computeStringWidth("Puhhh that was a close one, almost deadly...") / 2;
				}
				DLabelPos.y() = 80;
				DeadLabel.position(DLabelPos);

				ContinueLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				// reposition Continue Label
				Vector2f CLabelPos;
				CLabelPos.x() = m_RenderWin.width() / 2 - ContinueLabel.font()->computeStringWidth("Press P to continue!") / 2;
				CLabelPos.y() = (m_RenderWin.height() / 5) * 4;
				ContinueLabel.position(CLabelPos);

				GameOverLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				// reposition GameOver Label
				Vector2f GOLabelPos;
				GOLabelPos.x() = m_RenderWin.width() / 2 - GameOverLabel.font()->computeStringWidth("PLATSCH that is one bird less on the planet...") / 2;
				GOLabelPos.y() = 80;
				GameOverLabel.position(GOLabelPos);

				RestartLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				// reposition Restart Label
				Vector2f RLabelPos;
				RLabelPos.x() = m_RenderWin.width() / 2 - RestartLabel.font()->computeStringWidth("Press SPACE to restart!") / 2;
				RLabelPos.y() = (m_RenderWin.height() / 5) * 4;
				RestartLabel.position(RLabelPos);
			}

		}//listen[GLWindow]

	protected:

		SkeletalActor m_Bird;
		T3DMesh<float> BirdMesh1;
		T3DMesh<float> BirdMesh2;
		SkeletalAnimationController m_BipedController;
		SGNTransformation m_RootSGN;

		vector<string> m_City1;
		vector<string> m_City2;

		SkyboxActor m_Skybox;

		SGNGeometry m_BirdSGN;
		SGNTransformation m_BirdTransformSGN;
		SGNTransformation m_BirdRollSGN;
		SGNTransformation m_BirdPitchSGN;
		SGNTransformation m_BirdYawSGN;

		StaticActor m_Ground;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_GroundTransformSGN;

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;

		StaticActor building_1;
		SGNGeometry building_1_geo;
		SGNTransformation building_1_SGN;

		StaticActor building_2;
		SGNGeometry building_2_geo;
		SGNTransformation building_2_SGN;

		StaticActor building_3;
		SGNGeometry building_3_geo;
		SGNTransformation building_3_SGN;

		vector<SGNTransformation*> m_BuildingSGNs; // List to hold building SGNs
		vector<SGNGeometry*> m_BuildingGeoSGNs;   // List to hold building geometry SGNs

		vector<SGNTransformation*> m_SideBuildingSGNs; // List to hold building SGNs
		vector<SGNGeometry*> m_SideBuildingGeoSGNs;   // List to hold building geometry SGNs

		vector<SGNTransformation*> m_GroundBuildingSGNs; // List to hold building SGNs
		vector<SGNGeometry*> m_GroundBuildingGeoSGNs;   // List to hold building geometry SGNs

		bool m_paused = true;
		bool m_RepeatAnimation = true;
		bool m_collision = false;
		bool m_gameOver = false;

		bool glLoaded = false;


		//Speed für Vogel
		float oldSpeed = 0.3f;
		float pauseSpeed = 0.0f;

		Vector3f m_speed = Vector3f(0.0f, 0.0f, 0.1f);
		Vector3f m_startPosition = Vector3f(0.0f, 10.0f, -100.0f);

		int score = 0; // Anfangswert des Scores
		LineOfText scoreLabel;
		LineOfText DeadLabel;
		LineOfText ContinueLabel;
		LineOfText GameOverLabel;
		LineOfText RestartLabel;

		const float maxHorizontalPosition = 10.0f; // Adjust as needed
		const float minHorizontalPosition = -10.0f; // Adjust as needed

		const float maxVerticalPosition = 14.0f; // Adjust as needed
		const float minVerticalPosition = 1.0f; // Adjust as needed

		float m_rollSpeed = 0.0f;
		float m_pitchSpeed = 0.0f;
		// Limit the pitch angle to a reasonable range
		const float maxPitchAngle = 25.0f; // Adjust as needed
		const float maxRollAngle = 20.0f; // Adjust as needed
		const float maxRollSpeed = 5.0f; // Max roll speed (adjust as needed)

		const float rollDecayRate = 0.05f; // Rate at which roll speed decays (adjust as needed)

		// --- Collision --- //
		StaticActor m_Buildings[4]; // theoretisch kann das auch weglassen
		Vector3f m_buildingOrigin[4];
		Vector3f m_buildingDiag[4];
		std::shared_ptr<fcl::Box<float>> m_box_building[4];

		StaticActor m_Cube;
		StaticActor m_Sphere;
		CForge::Sphere m_birdSphere;
		float m_max_scale_bird;

		Eigen::Matrix4f m_birdTestCollision = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_matSphere = Eigen::Matrix4f::Identity();
		bool m_col = false;
		bool m_colLastFrame = false;
		int m_CPCollisonCurrent = 0;
		//std::vector<UINT32> m_BuildingGeoModels;
		//std::vector<std::pair<SGNTransformation*, UINT32>> m_BuildingGeoModels;
		std::vector<std::tuple<SGNTransformation*, UINT32, Eigen::Matrix4f>> m_BuildingGeoModels;

	};//ExampleFlappyBird

}

#endif