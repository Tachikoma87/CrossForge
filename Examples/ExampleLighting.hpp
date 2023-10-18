/*****************************************************************************\
*                                                                           *
* File(s): ExampleLighting.hpp                                            *
*                                                                           *
* Content: Example scene that showcases the different light sources.   *
*                        *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_EXAMPLELIGHTING_HPP__
#define __CFORGE_EXAMPLELIGHTING_HPP__

#include <crossforge/Graphics/Lights/SpotLight.h>
#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleLighting : public ExampleSceneBase {
	public:
		enum PointLights : int8_t {
			PL_SIDEWAY_1 = 0,
			PL_SIDEWAY_2,
			PL_MANAWELL_1,
			PL_MANAWELL_2,
			PL_CHANDELIER_1,
			PL_CHANDELIER_2,
			PL_COUNT,
		};

		ExampleLighting(void) {
			m_WindowTitle = "CrossForge Example - Lighting";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor


		~ExampleLighting(void) {
			clear();
		}//Destructor

		void init() override {
			
			initWindowAndRenderDevice(true, 2, static_cast<int>(PL_COUNT+1), 1);
			initCameraAndLights(true);
			initFPSLabel();
			m_FPSLabel.color(1.0f, 1.0f, 1.0f, 1.0f);

			m_Sun.initShadowCasting(2048, 2048, Vector2i(50, 50), 0.5f, 1000.0f);

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			m_Cam.position(Vector3f(0.0f, 2.0f, 0.0f));
			m_Cam.lookAt(Vector3f(0.0f, 2.0f, 0.0f), Vector3f(5.0f, 2.0f, 0.0f));

			// load skydome and a textured cube
			T3DMesh<float> M;

			initSkybox();

			SAssetIO::load("Assets/ExampleScenes/Sponza/Sponza.gltf", &M);
			for (uint32_t i = 0; i < M.materialCount(); ++i) CForgeUtility::defaultMaterial(M.getMaterial(i), CForgeUtility::PLASTIC_WHITE);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Sponza.init(&M);
			M.clear();


			// add sponza scene graph
			m_SponzaTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f));
			m_SponzaSGN.init(&m_SponzaTransformSGN, &m_Sponza);
			m_SponzaSGN.scale(Vector3f(4.0f, 4.0f, 4.0f));

			// create Sun actor
			PrimitiveShapeFactory::uvSphere(&M, Vector3f(5.0f, 5.0f, 5.0f), 20, 20);
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::METAL_GOLD);
			M.computePerVertexNormals();
			m_SunActor.init(&M);
			M.clear();

			m_SunTransSGN.init(&m_RootSGN);
			m_SunSGN.init(&m_SunTransSGN, &m_SunActor);

			// reposition sun
			m_SunDistance = 200.0f;
			m_SunAzimuth = CForgeMath::degToRad(0.0f);
			m_SunPolar = CForgeMath::degToRad(5.0f);

			m_SunAmbientLight.init(Vector3f::Zero(), Vector3f::UnitY(), Vector3f::Ones(), 4.0f);

			// initialize point light actor
			PrimitiveShapeFactory::doubleCone(&M, Vector4f(0.75f, 0.35f, 0.75f, 0.35f), 10);
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::METAL_STEEL);
			M.computePerVertexNormals();
			m_PointLightActor.init(&M);
			
			initPointLights();

			float Inner = CForgeMath::degToRad(2.0f);
			float Outer = CForgeMath::degToRad(15.f);
			m_Flashlight.init(m_Cam.position(), m_Cam.dir(), Vector3f(1.0f, 0.35f, 0.35f), 10.0f, Vector3f(0.0f, 0.75f, 0.01f), Vector2f(Inner, Outer));

			m_RenderDev.addLight(&m_SunAmbientLight);
			for (auto i : m_PointLights) m_RenderDev.addLight(&i->PL);
			m_RenderDev.addLight(&m_Flashlight);
			updateSun();

			m_LastPointLightUpdate = CForgeUtility::timestamp();

			m_IsSunActive = true;

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			LineOfText* pLightControls = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			pLightControls->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "1: Toggle Alley Lights| 2: Toggle Mana Well | 3: Toggle Main Room Lights| 4: Toggle Sun | 5,F: Toggle Flashlight");
			m_HelpTexts.push_back(pKeybindings);
			m_HelpTexts.push_back(pLightControls);
			m_DrawHelpTexts = true;


			std::string ErrorMsg;
			if (0 != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}
			m_IsFlashlightActive = false;
			m_Flashlight.intensity(0.0f);
		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear


		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			// sun simulation
			float Azimuth = (CForgeUtility::timestamp() % 10000000) / 1000.0f;
			Azimuth *= 0.2f;
			m_SunAzimuth = Azimuth - std::floor(Azimuth / (2.0f * EIGEN_PI)) * (2.0f * EIGEN_PI);
			updateSun();

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			if (m_IsFlashlightActive) {
				m_Flashlight.position(m_Cam.position());
				m_Flashlight.direction(m_Cam.dir());
			}

			if (CForgeUtility::timestamp() - m_LastPointLightUpdate > 200) {
				updatePointLights();
				m_LastPointLightUpdate = CForgeUtility::timestamp();
			}
			
			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
			m_SkyboxSG.render(&m_RenderDev);
			if(m_FPSLabelActive) m_FPSLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			// turn light sources on and off
			Keyboard* pKeyboard = m_RenderWin.keyboard();
			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) {
				if (m_PointLights[PL_SIDEWAY_1]->PL.intensity() > 0.0f) {
					m_PointLights[PL_SIDEWAY_1]->PL.intensity(0.0f);
					m_PointLights[PL_SIDEWAY_2]->PL.intensity(0.0f);
				}
				else {
					m_PointLights[PL_SIDEWAY_1]->PL.intensity(8.0f);
					m_PointLights[PL_SIDEWAY_2]->PL.intensity(8.0f);
				}
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) {
				if (m_PointLights[PL_MANAWELL_1]->PL.intensity() > 0.0f) {
					m_PointLights[PL_MANAWELL_1]->PL.intensity(0.0f);
					m_PointLights[PL_MANAWELL_2]->PL.intensity(0.0f);
				}
				else {
					m_PointLights[PL_MANAWELL_1]->PL.intensity(50.0f);
					m_PointLights[PL_MANAWELL_2]->PL.intensity(50.0f);
				}
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) {
				if (m_PointLights[PL_CHANDELIER_1]->PL.intensity() > 0.0f) {
					m_PointLights[PL_CHANDELIER_1]->PL.intensity(0.0f);
					m_PointLights[PL_CHANDELIER_2]->PL.intensity(0.0f);
				}
				else {
					m_PointLights[PL_CHANDELIER_1]->PL.intensity(40.0f);
					m_PointLights[PL_CHANDELIER_2]->PL.intensity(40.0f);
				}
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) {
				m_IsSunActive = !m_IsSunActive;
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_5, true) || pKeyboard->keyPressed(Keyboard::KEY_F, true)) {
				m_IsFlashlightActive = !m_IsFlashlightActive;
				if (m_IsFlashlightActive) m_Flashlight.intensity(20.0f);
				else m_Flashlight.intensity(0.0f);
			}

		}//mainLoop

	protected:
		struct PointLightStruct {
			PointLight PL;
			SGNGeometry PLSGN;
			SGNTransformation PLTransSGN;
		};

		void updateSun(void) {
			
			Vector3f SunPos;
			double Phi = m_SunAzimuth;
			double Theta = m_SunPolar;
			SunPos.x() = std::sin(Phi) * std::cos(Theta);
			SunPos.y() = std::cos(Phi);
			SunPos.z() = std::sin(Phi) * std::sin(Theta);
			SunPos = SunPos.cwiseProduct(0.5f * Vector3f(m_SunDistance, m_SunDistance, m_SunDistance));

			Vector3f SunDir = -SunPos.normalized();

			m_Sun.position(SunPos);
			m_Sun.direction(SunDir);
			m_SunTransSGN.translation(SunPos - SunDir * 7.0f);

			m_SunAmbientLight.position(SunPos);
			m_SunAmbientLight.direction(SunDir);

			if (SunPos.y() < 0.0f || !m_IsSunActive) {
				m_SunAmbientLight.intensity(0.0f);
				m_Sun.intensity(0.0f);
			}
			else if (SunPos.y() < 50.0f) {
				float Intensity = CForgeMath::lerp(0.0f, 2.0f, SunPos.y() / 50.0f);
				m_SunAmbientLight.intensity(Intensity);
				m_Sun.intensity(Intensity*2.0f);
			}
			else {
				m_SunAmbientLight.intensity(2.0f);
				m_Sun.intensity(4.0f);
			}


			m_BGLight.intensity(0);

		}//updateSun

		void initPointLights(void) {
			// the 2 ceiling lights in the sideways
			Vector3f PLPos = Vector3f(-2.0f, 10.5f, 13.0f);
			PointLightStruct* pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(1.0f, 1.0f, 0.0f), 8.0f, Vector3f(1.0f, 3.0f, 0.1f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);

			PLPos = Vector3f(-2.0f, 10.5f, -13.0f);
			pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(1.0f, 1.0f, 0.0f), 8.0f, Vector3f(1.0f, 3.0f, 0.1f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);

			// the two mana well lights
			PLPos = Vector3f(36.0f, 5.0f, -14.0f);
			pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(0.25f, 0.25f, 1.0f), 50.0f, Vector3f(0.0f, 2.0f, 0.0f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);


			PLPos = Vector3f(36.0f, 5.0f, 14.0f);
			pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(0.25f, 0.25f, 1.0f), 50.0f, Vector3f(0.0f, 2.0f, 0.0f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);

			// two two chandeliers
			PLPos = Vector3f(-25.0f, 15.0f, 0.0f);
			pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(1.0f, 1.0f, 0.7f), 40.0f, Vector3f(0.0f, 2.0f, 0.0f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);

			PLPos = Vector3f(25.0f, 15.0f, 0.0f);
			pPLS = new PointLightStruct();
			pPLS->PL.init(PLPos, Vector3f::UnitY(), Vector3f(1.0f, 1.0f, 0.7f), 40.0f, Vector3f(0.0f, 2.0f, 0.0f));
			pPLS->PLTransSGN.init(&m_RootSGN, PLPos + Vector3f(0.0f, 1.0f, 0.0f));
			pPLS->PLSGN.init(&pPLS->PLTransSGN, &m_PointLightActor);
			m_PointLights.push_back(pPLS);

		}//initPointLights

		void updatePointLights(void) {
			if (m_PointLights[PL_MANAWELL_1]->PL.intensity() < 0.1f) return; // mana wells are turend off

			float Value = CForgeUtility::timestamp() % 1000000 / 1000.0f;
			Value = std::sin(Value*2.0f);
			m_PointLights[PL_MANAWELL_1]->PL.intensity(50 + (Value * 35.0f));
			m_PointLights[PL_MANAWELL_2]->PL.intensity(50 + (Value * 35.0f));
		}//updatePointLights


		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Sponza;
		SGNGeometry m_SponzaSGN;
		SGNTransformation m_SponzaTransformSGN;

		float m_SunDistance;
		float m_SunAzimuth;
		float m_SunPolar;

		StaticActor m_SunActor;
		StaticActor m_PointLightActor;
		SGNGeometry m_SunSGN;
		SGNTransformation m_SunTransSGN;

		DirectionalLight m_SunAmbientLight;
		SpotLight m_Flashlight;
		

		std::vector<PointLightStruct*> m_PointLights;

		uint32_t m_LastPointLightUpdate;

		bool m_IsSunActive;
		bool m_IsFlashlightActive;

	};//ExampleLighting

}//name space

#endif