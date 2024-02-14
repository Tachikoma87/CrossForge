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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_FILEDIALOGTESTSCENE_HPP__
#define __CFORGE_FILEDIALOGTESTSCENE_HPP__


#include <Examples/ExampleSceneBase.hpp>

#include <crossforge/AssetIO/UserDialog.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	class FileDialogTestScene : public ExampleSceneBase {
	public:
		FileDialogTestScene(void) {
			m_WindowTitle = "CrossForge Example - File Dialog Test Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~FileDialogTestScene(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice();
			initCameraAndLights();
			initFPSLabel();

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// load skydome and a textured cube
			T3DMesh<float> M;

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);

			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			for (uint32_t i = 0; i < M.materialCount(); ++i) CForgeUtility::defaultMaterial(M.getMaterial(i), CForgeUtility::PLASTIC_YELLOW);
			M.computePerVertexNormals();
			m_Duck.init(&M);
			M.clear();


			// add cube
			m_DuckTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 1.5f, 0.0f));
			m_DuckSGN.init(&m_DuckTransformSGN, &m_Duck);
			m_DuckSGN.scale(Vector3f(0.02f, 0.02f, 0.02f));

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			m_DuckTransformSGN.rotationDelta(R);

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			m_HelpTexts.push_back(pKeybindings);
			m_DrawHelpTexts = true;

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
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			m_FPSLabel.render(&m_RenderDev);
			if (m_DrawHelpTexts) drawHelpTexts();

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) {
				UserDialog::NotifyPopup("Notification", "You have been served!", UserDialog::ICON_WARNING);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_2, true)) {
				UserDialog::UserAnswer A = UserDialog::MessagePopup("Message Box", "This is a message maybe it is too short!", UserDialog::DIALOG_YESNO, UserDialog::ICON_QUESTION, false);

				switch (A) {
				case UserDialog::ANSWER_OK: printf("User: Ok\n"); break;
				case UserDialog::ANSWER_CANCEL: printf("user:: cancel\n"); break;
				case UserDialog::ANSWER_YES: printf("User: yes\n"); break;
				case UserDialog::ANSWER_NO: printf("User:: no\n"); break;
				default: printf("Unhandled return value\n");
				}
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_3, true)) {
				std::string Name = UserDialog::PasswordBox("Give me a name", "Enter your name!");
				printf("User input: %s\n", Name.c_str());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_4, true)) {
				std::vector<std::string> Patterns;
				Patterns.push_back("*.jpg");
				Patterns.push_back("*.png");
				std::string Path = UserDialog::SaveFile("Save image", Patterns, "");
				printf("User input: %s\n", Path.c_str());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
				std::vector<std::string> Patterns;
				Patterns.push_back("*.jpg");
				Patterns.push_back("*.png");
				std::string Path = UserDialog::OpenFile("Select an image", Patterns, "");
				printf("User input: %s\n", Path.c_str());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
				std::vector<std::string> Patterns;
				Patterns.push_back("*.jpg");
				Patterns.push_back("*.png");
				std::vector<std::string> Paths = UserDialog::OpenFiles("Select images", Patterns, "");

				printf("User inputs: %d files\n", Paths.size());
				for (auto i : Paths) {
					printf("\t%s, \n", i.c_str());
				}
				
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_7, true)) {
				std::string Path = UserDialog::SelectFolder("Select a temporary folder");
				printf("User Input: %s\n", Path.c_str());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_8, true)) {
				Eigen::Vector3i Col = UserDialog::ColorPicker("Select a color", Eigen::Vector3i(255, 255, 255));
				printf("User Input: %d %d %d\n", Col[0], Col[1], Col[2]);

				Vector4f NewCol = Vector4f(Col[0], Col[1], Col[2], 255);
				NewCol /= 255.0f;

				m_Duck.material(0)->color(NewCol);
			}

			
		}//mainLoop

	protected:
		

		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif