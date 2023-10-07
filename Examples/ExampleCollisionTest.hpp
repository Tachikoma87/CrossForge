/*****************************************************************************\
*                                                                           *
* File(s): exampleMinimumGraphicsSetup.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
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
#pragma once 

#include "ExampleSceneBase.hpp"
#include "fcl/narrowphase/collision_object.h"
#include "fcl/narrowphase/distance.h"

using namespace Eigen;
using namespace std;

enum RotationCube {
	UNITX,
	UNITY,
	UNITZ,
};

namespace CForge {

	class ExampleCollisionTest : public ExampleSceneBase {
	public:
		ExampleCollisionTest(void) {
			m_WindowTitle = "CrossForge Example - Minimum Collision Test";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ExampleCollisionTest(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/cylinder.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Cube.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Sphere.obj", &M);
			setMeshShader(&M, 0.1f, 0.04f);
			M.computePerVertexNormals();
			m_Sphere.init(&M);
			M.clear();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add cube
			m_CubeTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 1.5f, 0.0f));
			m_CubeSGN.init(&m_CubeTransformSGN, &m_Cube);

			m_SphereTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 2.6f, 0.0f));
			m_SphereSGN.init(&m_SphereTransformSGN, &m_Sphere);

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			//R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			//m_DuckTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}

		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear


		void mainLoop(void)override {
			if (!glLoaded) {
				gladLoadGL();
				glLoaded = true;
			}

			auto solver_type = fcl::GJKSolverType::GST_LIBCCD;

			using fcl::Vector3;
			using Real = typename fcl::constants<float>::Real;
			const Real eps = fcl::constants<float>::eps();

			const Real w = 1.;
			const Real h = 1.;
			const Real d = 1.;
			const Vector3<float> half_size{ w / 2, d / 2, h / 2 };
			const Real r = .5;
			auto sphere_geometry = std::make_shared<fcl::Sphere<float>>(r);

			const Real r_c = 0.5f;
			const Real h_c = 1.0f;
			auto box_geometry = std::make_shared<fcl::Cylinder<float>>(r_c, h_c);
			//auto box_geometry = std::make_shared<fcl::Box<float>>(w, d, h);

			fcl::Transform3<float> X_WB = fcl::Transform3<float>::Identity();
			fcl::Transform3<float> X_WS = fcl::Transform3<float>::Identity();

			fcl::CollisionObject<float> sphere(sphere_geometry, X_WS);
			fcl::CollisionObject<float> box(box_geometry, X_WB);

			fcl::CollisionRequest<float> collision_request(1 /* num contacts */,
				true /* enable_contact */);
			collision_request.gjk_solver_type = solver_type;

			//for (Real sx : x_values) {
			//  for (Real sy : y_values ) {
			// Repose the sphere.
			float sx, sy, sz;
			sx = m_birdTransform.data()[12], sy = m_birdTransform.data()[13], sz = m_birdTransform.data()[14];
			//X_WS.translation() << sx, sy, sz;

			float bx, by, bz;
			float deltaX = 0.f, deltaY = 0.f, deltaZ = 0.f;
			bx = m_matCube.data()[12], by = m_matCube.data()[13], bz = m_matCube.data()[14];
			//X_WB.translation() << bx, by, bz;

			//box.setTranslation(Eigen::Vector3f(bx, by, bz));
			//box.setTransform(m_matCube.block<3, 3>(0, 0), Eigen::Vector3f(bx, by, bz));
			box.setRotation(m_matCube.block<3, 3>(0, 0));
			box.setTranslation(Eigen::Vector3f(bx, by, bz));
			sphere.setTransform(m_birdTransform.block<3, 3>(0, 0), Eigen::Vector3f(sx, sy, sz));

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
						col = true;
					}
					else {
						col = false;
					}
			};

			evaluate_collision(&sphere, &box);


			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			globalTimer += 1.f / m_FPS * 0.6;
			deltaTime = 1 / m_FPS; // gleich schnell zum rotieren

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			//m_SG.render(&m_RenderDev);

			float doRot = 0.0f;

			Keyboard* pKeyboard = m_RenderWin.keyboard();
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) m_rotationCube = UNITX;
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) m_rotationCube = UNITY;
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) m_rotationCube = UNITZ;
			
			if (pKeyboard->keyPressed(Keyboard::KEY_T, true)) doRot = deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_Z, true)) doRot = -deltaTime;

			if (pKeyboard->keyPressed(Keyboard::KEY_UP, true)) deltaZ = -deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_DOWN, true)) deltaZ = deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_RIGHT, true)) deltaX = -deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT, true)) deltaX = deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_R, true)) deltaY = -deltaTime;
			if (pKeyboard->keyPressed(Keyboard::KEY_E, true)) deltaY = deltaTime;

			switch (m_rotationCube)
			{
			case UNITX:
				m_rot = Vector3f::UnitX();
				break;
			case UNITY:
				m_rot = Vector3f::UnitY();
				break;
			case UNITZ:
				m_rot = Vector3f::UnitZ();
				break;
			default:
				break;
			}


			m_matCubeRot *= CForgeMath::rotationMatrix((Quaternionf)AngleAxisf(CForgeMath::degToRad(doRot) * 8, m_rot));
			m_matCubeTrans *= CForgeMath::translationMatrix(Eigen::Vector3f(deltaX, deltaY, deltaZ));
			m_matCube = m_matCubeRot * m_matCubeTrans;

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			m_RenderDev.modelUBO()->modelMatrix(m_matCube);
			m_Cube.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());

			if (col)
				glColorMask(true, false, false, true);
			else
				glColorMask(true, true, true, true);

			m_RenderDev.modelUBO()->modelMatrix(m_birdTransform);
			m_Sphere.render(&m_RenderDev, Eigen::Quaternionf(), Eigen::Vector3f(), Eigen::Vector3f());
			glColorMask(true, true, true, true);

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}
		}



	protected:
		bool glLoaded = false;
		bool col = false;
		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CubeSGN;
		SGNGeometry m_SphereSGN;
		SGNTransformation m_CubeTransformSGN;
		SGNTransformation m_SphereTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Cube;

		Eigen::Matrix4f m_matCube = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_matCubeTrans = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_matCubeRot = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f m_birdTransform = Eigen::Matrix4f::Identity();

		StaticActor m_Sphere;
		RotationCube m_rotationCube; 
		Vector3f m_rot = Vector3f::UnitX();

		float globalTimer = 0.f;
		float deltaTime = 0.f;

	};//ExampleMinimumGraphicsSetup

}//name space


