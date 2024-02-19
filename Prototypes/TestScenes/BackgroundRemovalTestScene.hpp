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
#ifndef __CFORGE_BACKGROUNDREMOVALTESTSCENE_HPP__
#define __CFORGE_BACKGROUNDREMOVALTESTSCENE_HPP__


#include "../../Examples/ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class BackgroundRemovalTestScene : public ExampleSceneBase {
	public:
		BackgroundRemovalTestScene(void) {
			m_WindowTitle = "CrossForge - Background Removal";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~BackgroundRemovalTestScene(void) {
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

			processBGImages();
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

		}//mainLoop

	protected:

		void processBGImages() {
			printf("Starting to process BG images ...\n");

			T2DImage<uint8_t> Img;
			T2DImage<uint8_t> Bg;
			T2DImage<uint8_t> Result;
			float Threshold = 30.0f;

			AssetIO::load("MyAssets/Timon_Mask/prefix_7_cam04540918.jpg", &Img);
			AssetIO::load("MyAssets/Timon_Mask/prefix_0_cam04540918.jpg", &Bg);

			removeBackground(&Img, &Bg, &Result, Threshold);

			AssetIO::store("MyAssets/Timon_Mask/BGremoval.jpg", &Result);

			printf("Finished processing BG images\n");
		}

		void removeBackground(const T2DImage<uint8_t>* pImg, const T2DImage<uint8_t>* pBg, T2DImage<uint8_t>* pResult, float Threshold) {
			if (pImg->width() != pBg->width() || pImg->height() != pBg->height()) throw CForgeExcept("Image dimensions do not match!");

			pResult->init(pImg->width(), pImg->height(), T2DImage<uint8_t>::COLORSPACE_RGB, nullptr);

			const uint8_t* pImgData = pImg->data();
			const uint8_t* pBgData = pBg->data();

			for (uint32_t i = 0; i < pImg->pixelCount()*3; i += 3) {
				Vector3f P = Vector3f(pImgData[i + 0], pImgData[i + 1], pImgData[i + 2]);
				Vector3f B = Vector3f(pBgData[i + 0], pBgData[i + 1], pBgData[i + 2]);
				Vector3f Diff = P - B;
				if (std::abs(Diff[0]) < Threshold && std::abs(Diff[1]) < Threshold && std::abs(Diff[2]) < Threshold) {
					pResult->data()[i + 0] = 255.0f;
					pResult->data()[i + 1] = 255.0f;
					pResult->data()[i + 2] = 255.0f;
				}
				else {
					pResult->data()[i + 0] = pImgData[i+0];
					pResult->data()[i + 1] = pImgData[i+1];
					pResult->data()[i + 2] = pImgData[i+2];
				}
			}

		}//removeBackground

		

		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif