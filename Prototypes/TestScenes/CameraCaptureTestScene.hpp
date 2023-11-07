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
#ifndef __CFORGE_CAMERACAPTURETESTSCENE_HPP__
#define __CFORGE_CAMERACAPTURETESTSCENE_HPP__

#include "../Multimedia/SMediaDeviceManager.h"
#include "../../Examples/ExampleSceneBase.hpp"
#include "../Camera/CameraCapture.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class CameraCaptureTestScene : public ExampleSceneBase {
	public:
		CameraCaptureTestScene(void) {
			m_WindowTitle = "CrossForge Example - Camera Capture Test Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~CameraCaptureTestScene(void) {
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

			m_pMediaDevMan = SMediaDeviceManager::instance();
			
			if (m_pMediaDevMan->cameraCount() > 0) {
				m_pCamDevice = m_pMediaDevMan->camera(0);
				std::vector<int32_t> Formats;
				m_pCamDevice->findOptimalCaptureFormats(1280, 720, &Formats);
				m_pCamDevice->changeCaptureFormat(Formats[0]);

				printf("Found %d suitable capture formats:\n", Formats.size());
				for (auto i : Formats) {
					CameraDevice::CaptureFormat F = m_pCamDevice->captureFormat(i);
					printf("\t%dx%d - %s\n", F.FrameSize.x(), F.FrameSize.y(), F.DataFormat.c_str());
				}
			}

		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
			if(nullptr != m_pMediaDevMan) m_pMediaDevMan->release();
			m_pMediaDevMan = nullptr;
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

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
				T2DImage<uint8_t> Img;
				m_pCamDevice->retrieveImage(&Img);

				if (Img.width() > 0) {
					static int32_t c = 0;
					std::string Filename = "MyAssets/Webcam/" + std::to_string(c++) + ".jpg";
					SAssetIO::store(Filename, &Img);
				}
				
			}

		}//mainLoop

	protected:

		void initCamera() {
			m_CameraCapture.init();
			m_CameraCapture.clear();
		}//initCamera

		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		CameraCapture m_CameraCapture;
		SMediaDeviceManager* m_pMediaDevMan;
		CameraDevice* m_pCamDevice;

	};//CameraCaptureTestScene

}//name space

#endif