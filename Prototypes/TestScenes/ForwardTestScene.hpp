/*****************************************************************************\
*                                                                           *
* File(s): ForwardTestScene.hpp                                            *
*                                                                           *
* Content:    *
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
#ifndef __CFORGE_FORWARDTESTSCENE_HPP__
#define __CFORGE_FORWARDTESTSCENE_HPP__


#include "Examples/ExampleSceneBase.hpp"
#include <CForge/MeshProcessing/PrimitiveShapeFactory.h>

#include <CForge/Graphics/Font/Font.h>
#include <CForge/Graphics/Font/LineOfText.h>
#include <CForge/Graphics/Font/SFontManager.h>
#include <cuchar>

using namespace Eigen;
using namespace std;

namespace CForge {


	class ForwardTestScene : public ExampleSceneBase {
	public:
		ForwardTestScene(void) {
			m_WindowTitle = "CrossForge Test Scene - Forward Rendering Test";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ForwardTestScene(void) {
			clear();
		}//Destructor

		void init() override{

			initWindowAndRenderDevice(true);
			initCameraAndLights();
#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("MyAssets/Technique_Evaluation/OldModel.gltf", &M);
			M.clearSkeleton();
			setMeshShader(&M, 0.1f, 0.04f);
			float ModelScale = 0.05f;
			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_WHITE);

			M.computePerVertexNormals();
			//M.computePerVertexTangents();
			m_Cube.init(&M);
			M.clear();


			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube
			m_CubeTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_CubeSGN.init(&m_CubeTransformSGN, &m_Cube);
			m_CubeSGN.scale(Vector3f(ModelScale, ModelScale, ModelScale));

			// rotate about the y-axis at 45 degree every second
			Quaternionf R;
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			m_CubeTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			initText();

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) override{
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void mainLoop(void) override {
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			//m_CubeSGN.enable(true, false);
			
			if (m_Deferred) {
				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);
			}
			else {
				m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, true);
				m_SG.render(&m_RenderDev);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//to allow overlapping widget backgrounds
				glDisable(GL_DEPTH_TEST);
				m_Text.render(&m_RenderDev);

				std::string FPSS = "FPS: " + std::to_string(int32_t(std::floor(m_FPS)));
					
				m_Text2.canvasSize(m_RenderWin.width(), m_RenderWin.height());
				//m_Text2.setPosition(20, 50);
				m_Text2.text(FPSS);

				uint32_t Val = CForgeUtility::timestamp() % 100000;
				float Alpha = (std::sin(0.25f*Val/1000) + 1.0f) / 2.0f;
				m_Text2.color(Vector4f(1.0f, 0.0f, 0.0f, Alpha));

				m_Text2.render(&m_RenderDev);
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}


			m_RenderWin.swapBuffers();

			updateFPS();

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_5, true)) {
				T2DImage<uint8_t> DepthImg;
				m_Sun.retrieveDepthBuffer(&DepthImg);
				SAssetIO::store("SunDepth.jpg", &DepthImg);
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_6, true)) {
				m_Deferred = !m_Deferred;
			}

		}//mainLoop

	protected:

		void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) override {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");


				pMat->VertexShaderForwardPass.push_back("Shader/ForwardPassPBS.vert");
				pMat->FragmentShaderForwardPass.push_back("Shader/ForwardPassPBS.frag");

				pMat->Metallic = Metallic;
				pMat->Roughness = Roughness;
			}//for[materials]
		}//setMeshShader

		void changeUVTiling(T3DMesh<float> *pMesh, Eigen::Vector3f Factor) {

			std::vector<Vector3f> UVWs;
			for (uint32_t i = 0; i < pMesh->textureCoordinatesCount(); ++i)
				UVWs.push_back(pMesh->textureCoordinate(i).cwiseProduct(Factor));
			pMesh->textureCoordinates(&UVWs);
			if (pMesh->tangentCount() > 0) pMesh->computePerVertexTangents();

		}//changeUVTiling

		template<typename T>
		T nextPowerOfTwo(const T Value) {
			T a = std::log2(Value);
			return (std::pow(T(2), a) == Value) ? Value : std::pow(T(2), a + 1);
		}//nextPowerOfTwo

		void initText(void) {

			m_pFontManager = FontManager::instance();

			//m_pFont = m_pFontManager->createFont("Assets/fonts/SourceSansPro/SourceSansPro-SemiBold.ttf", 24);
			m_pFont = CForgeUtility::defaultFont(CForgeUtility::FONTTYPE_SANSERIF, 18, false, false);


			// init text
			m_Text.init(m_pFont, nullptr);
			m_Text.canvasSize(1280, 720);
			
			m_Text.position(0, 0);
			m_Text.color(0.0f, 0.0f, 0.0f, 1.0f);
			m_Text.text("Hello World! My old friend.");

			m_Text2.init(m_pFont, m_pTextShader);
			m_Text2.canvasSize(1280, 720);

			m_Text2.position(0, 50);
			m_Text2.color(1.0f, 0.4f, 0.4f);
			m_Text2.text("FPS: 60.0");
			int32_t XOffset = m_Text2.computeStringWidth("FPS: 60.0");
			m_Text2.position(1280 - XOffset, 700);
			
		}//initText

		
		LineOfText m_Text;
		LineOfText m_Text2;
		GLShader *m_pTextShader;
		Font* m_pFont;

		FontManager* m_pFontManager;

		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_CubeSGN;
		SGNTransformation m_CubeTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Cube;


		bool m_Deferred;
	};//ForwardTestScene

}//name space

#endif