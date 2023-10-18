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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_EXAMPLESKYBOX_HPP__
#define __CFORGE_EXAMPLESKYBOX_HPP__

#include "ExampleSceneBase.hpp"
#include <crossforge/Graphics/Actors/SkyboxActor.h>


using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleSkybox : public ExampleSceneBase {
	public:
		ExampleSkybox(void) {
			m_WindowTitle = "CrossForge Example - Skybox";
		}//Constructor

		~ExampleSkybox(void) {
			clear();
		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();
			initCameraAndLights();
			initFPSLabel();

			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 15.0f, 3.0f);
			

			// Add the duck
			T3DMesh<float> M;
			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			for (uint32_t i = 0; i < M.materialCount(); ++i) CForgeUtility::defaultMaterial(M.getMaterial(i), CForgeUtility::PLASTIC_YELLOW);
			M.computePerVertexNormals();
			m_Duck.init(&M);
			M.clear();

			m_DuckTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 1.0f, 0.0f));
			m_DuckTransformSGN.scale(Vector3f(0.02f, 0.02f, 0.02f));
			m_DuckSGN.init(&m_DuckTransformSGN, &m_Duck);

			// rotate about the y-axis at 45 degree every second and about the X axis to make it a bit more interesting
			Quaternionf RY, RX;
			RY = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			RX = AngleAxisf(CForgeMath::degToRad(-25.0f / 60.0f), Vector3f::UnitZ());
			m_DuckTransformSGN.rotationDelta(RX * RY);

			/// gather textures for the skyboxes
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_right.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_left.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_up.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_down.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_back.png");
			m_ClearSky.push_back("Assets/ExampleScenes/skybox/vz_clear_front.png");

			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_right.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_left.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_up.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_down.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_back.png");
			m_EmptySpace.push_back("Assets/ExampleScenes/skybox/vz_empty_space_front.png");

			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_right.png");
			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_left.png");
			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_up.png");
			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_down.png");
			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_back.png");
			m_Techno.push_back("Assets/ExampleScenes/skybox/vz_techno_front.png");

			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_rt.jpg");
			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_lf.jpg");
			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_up.jpg");
			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_dn.jpg");
			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_ft.jpg");
			m_BlueCloud.push_back("Assets/ExampleScenes/skybox/bluecloud_bk.jpg");

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


			// create help text
			LineOfText* pKeybindings = new LineOfText();
			LineOfText* pSkyboxControls = new LineOfText();
			LineOfText* pSkyboxControls2 = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			pSkyboxControls->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "(Shift) + B: +/- Brightness | (Shift) + C: +/- Contrast | (Shift) + V: +/- Saturation");
			pSkyboxControls2->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "1 - 4: Change skybox textures | R: increase skybox rotation | Shift + R: stop skybox rotation");
			m_HelpTexts.push_back(pKeybindings);
			m_HelpTexts.push_back(pSkyboxControls);
			m_HelpTexts.push_back(pSkyboxControls2);
			m_DrawHelpTexts = true;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void)override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_SkyboxSG.update(60.0f / m_FPS);

			// handle input for the skybox
			Keyboard* pKeyboard = m_RenderWin.keyboard();
			float Step = (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) ? -0.05f : 0.05f;
			if (pKeyboard->keyPressed(Keyboard::KEY_B, true)) m_Skybox.brightness(m_Skybox.brightness() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_V, true)) m_Skybox.saturation(m_Skybox.saturation() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_C, true)) m_Skybox.contrast(m_Skybox.contrast() + Step);
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT) && pKeyboard->keyPressed(Keyboard::KEY_R)) {
				m_SkyboxTransSGN.rotationDelta(Quaternionf::Identity());
			}
			else if (pKeyboard->keyPressed(Keyboard::KEY_R, true)) {
				Quaternionf RDelta = m_SkyboxTransSGN.rotationDelta();
				auto R = AngleAxisf(CForgeMath::degToRad(-2.5f / 60.0f), Vector3f::UnitY());
				m_SkyboxTransSGN.rotationDelta(R * RDelta);
			}

			if (pKeyboard->keyPressed(Keyboard::KEY_1, true)) m_Skybox.init(m_ClearSky[0], m_ClearSky[1], m_ClearSky[2], m_ClearSky[3], m_ClearSky[4], m_ClearSky[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_2, true)) m_Skybox.init(m_EmptySpace[0], m_EmptySpace[1], m_EmptySpace[2], m_EmptySpace[3], m_EmptySpace[4], m_EmptySpace[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_3, true)) m_Skybox.init(m_Techno[0], m_Techno[1], m_Techno[2], m_Techno[3], m_Techno[4], m_Techno[5]);
			if (pKeyboard->keyPressed(Keyboard::KEY_4, true)) m_Skybox.init(m_BlueCloud[0], m_BlueCloud[1], m_BlueCloud[2], m_BlueCloud[3], m_BlueCloud[4], m_BlueCloud[5]);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

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
			m_FPSLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}

	protected:
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;
		
		vector<string> m_ClearSky;
		vector<string> m_EmptySpace;
		vector<string> m_Techno;
		vector<string> m_BlueCloud;
		SkyboxActor m_Skybox;

		SceneGraph m_SkyboxSG;
		SGNTransformation m_SkyboxTransSGN;
		SGNGeometry m_SkyboxGeomSGN;

	};//ExampleSkybox

}

#endif