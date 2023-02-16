/*****************************************************************************\
*                                                                           *
* File(s): ExampleTextRendering.hpp                                         *
*                                                                           *
* Content: Showcases the default fonts and how to render them on screen.    *
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
#ifndef __CFORGE_EXAMPLETEXTRENDERING_HPP__
#define __CFORGE_EXAMPLETEXTRENDERING_HPP__

#include <crossforge/Graphics/Font/LineOfText.h>
#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleTextRendering : public ExampleSceneBase {
	public:
		ExampleTextRendering(void) {
			m_WindowTitle = "CrossForge Example - Text Rendering";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ExampleTextRendering(void) {
			clear();
		}//Destructor

		void init() override {

			initWindowAndRenderDevice(false);
			initCameraAndLights();
			initSkybox();
			initFPSLabel();

#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif


			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);
			T3DMesh<float> M;

			// create ground plane
			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);

			SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			setMeshShader(&M, 0.1f, 0.04f);
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

			initText();

			// create help text
			LineOfText* pKeybindings = new LineOfText();
			pKeybindings->init(CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18), "Movement: (Shift) + W,A,S,D  | Rotation: LMB/RMB + Mouse | F1: Toggle help text");
			pKeybindings->color(0.0f, 0.0f, 0.0f, 1.0f);
			m_HelpTexts.push_back(pKeybindings);
			m_DrawHelpTexts = true;
			m_FPSLabelActive = true;

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

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);
			m_SkyboxSG.render(&m_RenderDev);
			renderText();

			if (m_FPSLabelActive) {
				// update and draw FPS label
				std::string LabelText = "FPS: " + std::to_string(int32_t(m_FPS));
				m_FPSLabel.text(LabelText);
				m_FPSLabel.render(&m_RenderDev);
			}
			if (m_DrawHelpTexts) drawHelpTexts();
	
			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

		}//mainLoop

	protected:

		void initText(void) {
			uint32_t FontSize = 18;
			LineOfText *pText = nullptr;

			// most well known sample sentence to test fonts
			std::string SampleSentence = "The quick brown fox jumps over the lazy dog.";

			// starting position
			Vector2f Position = Vector2f(10, 40);

			// Monotype
			Font* pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_MONO, FontSize, false, false);
			pText = new LineOfText();
			pText->init(pFont, "Monotype Font (UbunutMono)");
			float XOffset = pFont->computeStringWidth(SampleSentence) - pFont->computeStringWidth("Monotype Font (UbuntuMono)");
			pText->position(Position + Vector2f(XOffset/2.0f, 0.0f));
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);	

			// regular
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);;
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_MONO, FontSize, true, false);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_MONO, FontSize, false, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_MONO, FontSize, true, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);


			Position.x() += pFont->computeStringWidth(SampleSentence) + 40;
			Position.y() = 40.0f;

			// Sanserif 
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, FontSize, false, false);
			pText = new LineOfText();
			pText->init(pFont, "Sanserif Font (Source Sans Pro)");
			XOffset = std::abs(pFont->computeStringWidth(SampleSentence) - pFont->computeStringWidth("Sanserif Font (Source Sans Pro)"));
			pText->position(Position + Vector2f(XOffset / 2.0f, 0.0f));
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// regular
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);;
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, FontSize, true, false);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, FontSize, false, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, FontSize, true, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			Position += Vector2f(0.0f, FontSize * 1.5f);

			// handwriting font
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_HANDWRITING, FontSize*1.5, false, false);
			pText = new LineOfText();
			pText->init(pFont, "Handwriting Font (Rouge Script)");
			XOffset = std::abs(pFont->computeStringWidth(SampleSentence) - pFont->computeStringWidth("Handwriting Font (Rouge Script)"));
			pText->position(Position + Vector2f(XOffset / 2.0f, 0.0f));
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// only one style
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);;
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);


			Position.x() += pFont->computeStringWidth(SampleSentence) + 40;
			Position.y() = 40.0f;

			// serif 
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SERIF, FontSize, false, false);
			pText = new LineOfText();
			pText->init(pFont, "Serif Font (Noto Serif)");
			XOffset = std::abs(pFont->computeStringWidth(SampleSentence) - pFont->computeStringWidth("Serif Font (Noto Serif)"));
			pText->position(Position + Vector2f(XOffset / 2.0f, 0.0f));
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// regular
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);;
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SERIF, FontSize, true, false);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SERIF, FontSize, false, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);

			// bold italic
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SERIF, FontSize, true, true);
			pText = new LineOfText();
			pText->init(pFont, SampleSentence);
			pText->position(Position);
			m_Texts.push_back(pText);
			Position += Vector2f(0.0f, FontSize * 1.5f);


			// set canvas size for all texts
			for (auto i : m_Texts) {
				i->canvasSize(m_RenderWin.width(), m_RenderWin.height());
				i->color(0.0f, 0.0f, 0.0f, 1.0f);
			}


			// create the FPS label
			// position right bottom of the screen
			pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 24, true, false);
			m_FPSLabel.init(pFont, "FPS: 60");
			Vector2f LabelPos;
			LabelPos.x() = m_RenderWin.width() - pFont->computeStringWidth("FPS: 60") - 20;
			LabelPos.y() = m_RenderWin.height() - 34;
			m_FPSLabel.position(LabelPos);

		}//initText

		void renderText(void) {
			for (auto i : m_Texts) if (nullptr != i) i->render(&m_RenderDev);
		}//renderText

		virtual void listen(GLWindowMsg Msg) override {
			ExampleSceneBase::listen(Msg);

			// we have to notify the LinesOfText if canvas size changes
			if (GLWindowMsg::MC_RESIZE == Msg.Code) {
				for (auto i : m_Texts) i->canvasSize(m_RenderWin.width(), m_RenderWin.height());
				m_FPSLabel.canvasSize(m_RenderWin.width(), m_RenderWin.height());

				// reposition FPS Label
				Vector2f LabelPos;
				LabelPos.x() = m_RenderWin.width() - m_FPSLabel.font()->computeStringWidth("FPS: 60") - 20;
				LabelPos.y() = m_RenderWin.height() - 34;
				m_FPSLabel.position(LabelPos);
			}

		}//listen[GLWindow]

		// Scene Graph
		SGNTransformation m_RootSGN;
		
		StaticActor m_GroundPlane;
		SGNGeometry m_GroundPlaneSGN;
		SGNTransformation m_GroundPlaneTransSGN;
		
		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		LineOfText m_FPSLabel;

		std::vector<LineOfText*> m_Texts;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif