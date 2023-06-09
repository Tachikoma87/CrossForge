/*****************************************************************************\
*                                                                           *
* File(s): exampleSkybox.hpp                                            *
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
#ifndef __CFORGE_EXAMPLEBIRD_HPP__
#define __CFORGE_EXAMPLEBIRD_HPP__

#include "ExampleSceneBase.hpp"
#include "../CForge/Graphics/Actors/SkyboxActor.h"


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
			M.clear();

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

			// add cube
			m_BirdTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 10.0f, 0.0f));
			m_BirdTransformSGN.scale(Vector3f(0.1f, 0.1f, 0.1f));
			//m_BirdTranslateSGN.init(&m_BirdTransformSGN, Vector3f(0.0f, 0.0f, 0.0f));

			m_MountainTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 5.0f, 0.0f));
			//m_MountainTransformSGN.scale(Vector3f(1.5f, 1.5f, 1.5f));
			
			Quaternionf To_Y;
			To_Y = AngleAxis(CForgeMath::degToRad(90.0f), Vector3f::UnitY());
			m_BirdTransformSGN.rotation(To_Y);
			m_BirdSGN.init(&m_BirdTransformSGN, &m_Bird);
			

			// rotate about the y-axis at 45 degree every second and about the X axis to make it a bit more interesting
			/*Quaternionf RY, RX;
			RY = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			RX = AngleAxisf(CForgeMath::degToRad(-25.0f / 60.0f), Vector3f::UnitZ());
			m_BirdTransformSGN.rotationDelta(RX * RY);*/

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
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

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

			Quaternionf To_Y;
			Quaternionf To_YN;
			Quaternionf To_Z;
			To_Y = AngleAxis(CForgeMath::degToRad(5.0f), Vector3f::UnitY());
			To_YN = AngleAxis(CForgeMath::degToRad(-5.0f), Vector3f::UnitY());
			To_Z = AngleAxis(CForgeMath::degToRad(-5.0f), Vector3f::UnitZ());
			
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT)) { 
				m_BirdTransformSGN.rotation(m_BirdTransformSGN.rotation() * To_Y);
			}
			if (pKeyboard->keyPressed(Keyboard::KEY_RIGHT))m_BirdTransformSGN.rotation(m_BirdTransformSGN.rotation() * To_YN);
			//if (pKeyboard->keyPressed(Keyboard::KEY_DOWN))m_BirdTransformSGN.rotation(m_BirdTransformSGN.rotation() * To_Z);

			Vector3f pos;
			Vector3f dir;
			Quaternionf rot;
			Matrix3f m3;
			Vector3f m;
			
			// gain and loose speed
			if (pKeyboard->keyPressed(Keyboard::KEY_UP) && speed.z() <= 1.5f)speed.z() += 0.01f;
			if (pKeyboard->keyPressed(Keyboard::KEY_DOWN) && speed.z() >= 0.1f)speed.z() -= 0.01f;

			// the bird sinks during normal flight and gains altitude when pressed enter
			if (pKeyboard->keyPressed(Keyboard::KEY_SPACE)) speed.y() += 0.03;
			if (speed.y() > -0.01f) speed.y() -= 0.01f;


			m3 = m_BirdTransformSGN.rotation().toRotationMatrix();
			m.x() = m3(0, 0) * speed.x() + m3(0, 2) * speed.z();
			m.y() = speed.y();
			m.z() = m3(2, 0) * speed.x() + m3(2, 2) * speed.z();

			dir = Vector3f(m.x(), 0, m.z()).normalized();

			//Quaternionf rotate_left = AngleAxis(CForgeMath::degToRad(2.5f), dir);

			// in translation there is the postion
			printf("%f - %f - %f | %f\n", m_BirdTransformSGN.translation().x(), m_BirdTransformSGN.translation().y(), m_BirdTransformSGN.translation().z(), speed.y());
			
			if (m_BirdTransformSGN.translation().y() < 0.05) speed.y() += 0.1f;


			m_BirdTransformSGN.translationDelta(m); 

			//defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());
			defaultCameraUpdateBird(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse(), m, m_BirdTransformSGN.translation(), Vector3f(0.0f, 1.0f, 0.0f), m3);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera((VirtualCamera*)m_Sun.camera());
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
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
		SGNTransformation m_BirdTranslateSGN;
		SGNTransformation m_BirdRollSGN;

		SGNGeometry m_MountainSGN;
		SGNTransformation m_MountainTransformSGN;
		SGNTransformation m_MountainTranslateSGN;

		StaticActor m_Ground;
		SGNGeometry m_GroundSGN;
		SGNTransformation m_GroundTransformSGN;

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;

		//Speed für Vogel
		Vector3f speed = Vector3f(0.0f, 0.0f, 0.01f);

	};//ExampleBird

}

#endif