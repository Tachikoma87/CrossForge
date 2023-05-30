/*****************************************************************************\
*                                                                           *
* File(s): VideoPlayerTestScene.hpp                                            *
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
#ifndef __CFORGE_VIDEOPLAYERTESTSCENE_HPP__
#define __CFORGE_VIDEOPLAYERTESTSCENE_HPP__


#include <Examples/ExampleSceneBase.hpp>
#include "../Camera/VideoPlayer.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class VideoPlayerTestScene : public ExampleSceneBase {
	public:
		VideoPlayerTestScene(void) {
			m_WindowTitle = "CrossForge Example - Vidoe Player Test Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~VideoPlayerTestScene(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice();
			gladLoadGL();
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

			m_VideoPlayer.init(Vector2f(0.125f, 0.125f), Vector2f(0.75f, 0.75f));

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
			m_VideoPlayer.update();

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

			m_VideoPlayer.render(&m_RenderDev);

			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true)) m_VideoPlayer.play("MyAssets/Study_Videos/01_1.mp4");

		}//mainLoop

	protected:

		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		VideoPlayer m_VideoPlayer;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif