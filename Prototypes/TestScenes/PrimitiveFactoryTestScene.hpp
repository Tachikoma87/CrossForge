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
#ifndef __CFORGE_PRIMITIVEFACTORYTESTSCENE_HPP__
#define __CFORGE_PRIMITIVEFACTORYTESTSCENE_HPP__


#include "../../Examples/ExampleSceneBase.hpp"
#include <CForge/MeshProcessing/PrimitiveShapeFactory.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	class PrimitiveFactoryTestScene : public ExampleSceneBase {
	public:
		PrimitiveFactoryTestScene(void) {
			m_WindowTitle = "CrossForge Example - Primtive Factory Showroom";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~PrimitiveFactoryTestScene(void) {
			clear();
		}//Destructor

		void init() override{

			initWindowAndRenderDevice();
			initCameraAndLights();

#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif

			// load skydome and a textured cube
			T3DMesh<float> M;

			/*SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			m_Skydome.init(&M);
			M.clear();*/

			//SAssetIO::load("Assets/ExampleScenes/StarCoin/StarCoin.gltf", &M);
			//PrimitiveShapeFactory::plane(&M, Vector2f(5.0f, 5.0f), Vector2i(1, 1));
			//PrimitiveShapeFactory::circle(&M, Vector2f(3.0f, 3.0f), 20, 0.0f);
			PrimitiveShapeFactory::cuboid(&M, Vector3f(4, 4, 4), Vector3i(3, 3, 3));
			//PrimitiveShapeFactory::uvSphere(&M, Vector3f(2, 2, 2), 25, 25);		
			//PrimitiveShapeFactory::doubleCone(&M, Vector4f(1.5f, 1.5f, 1.5f, 0.0f), 5);
			//PrimitiveShapeFactory::cone(&M, Vector3f(1.0f, 2.0f, 1.0f), 8);
			//PrimitiveShapeFactory::cylinder(&M, Vector2f(2.0f, 2.0f), Vector2f(2.0f, 2.0f), 3.0f, 20, Vector2f(0.0f, 0.0f));
			//PrimitiveShapeFactory::Torus(&M, 2.0f, 0.5f, 30, 20);

			//M.getMaterial(0)->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			//M.getMaterial(0)->Color = Vector4f(0xcc, 0xac, 0x00, 0xff) / 255.0f;
			/*M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/ground14.jpg";
			M.getMaterial(0)->TexNormal = "Assets/ExampleScenes/ground14n.jpg";*/

			/*M.getMaterial(0)->TexAlbedo = "MyAssets/Textures/metal06.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/Textures/metal06n.jpg";

			M.getMaterial(0)->TexAlbedo = "MyAssets/Textures/stone03.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/Textures/stone03n.jpg";

			M.getMaterial(0)->TexAlbedo = "MyAssets/Textures/misc11.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/Textures/misc11n.jpg";*/

			//M.getMaterial(0)->TexAlbedo = "MyAssets/Textures/ground13.jpg";
			//M.getMaterial(0)->TexNormal = "MyAssets/Textures/ground13n.jpg";

			//M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/tex/rp_eric_rigged_001_dif.jpg";
			//M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/StarCoin/MaterialStar_baseColor.jpg";

			setMeshShader(&M, 0.2f, 0.04f);

			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::METAL_GOLD);

			//M.changeUVTiling(Vector3f(5.0f, 5.0f, 1.0f));
			std::string ErrorMsg;
			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("Not handled OpenGL error occurred during initialization of Ducky: " + ErrorMsg, "Ducky", SLogger::LOGTYPE_ERROR);
			}

			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Duck.init(&M);
			M.clear();

			BoundingVolume BV;
			m_Duck.boundingVolume(BV);

			// build scene graph
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			// add cube
			m_DuckTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 2.5f, 0.0f));
			m_DuckSGN.init(&m_DuckTransformSGN, &m_Duck);
			//m_DuckSGN.scale(Vector3f(0.02f, 0.02f, 0.02f));

			// rotate about the y-axis at 45 degree every second
			Quaternionf R = Quaternionf::Identity();
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			m_DuckTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("Not handled OpenGL error occurred during initialization of GBuffer: " + ErrorMsg, "GBuffer", SLogger::LOGTYPE_ERROR);
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

			//m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			//m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);
			
			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			//glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_RenderWin.swapBuffers();

			updateFPS();

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				SGNGeometry::Visualization Vis = m_DuckSGN.visualization();
				m_DuckSGN.visualization((Vis == SGNGeometry::VISUALIZATION_WIREFRAME) ? SGNGeometry::VISUALIZATION_FILL : SGNGeometry::VISUALIZATION_WIREFRAME);

			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_UP, true)) {
				float Metal = m_Duck.material(0)->metallic();
				m_Duck.material(0)->metallic(Metal + 0.05f);
				printf("Metallic now: %.2f\n", m_Duck.material(0)->metallic());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_DOWN, true)) {
				float Metal = m_Duck.material(0)->metallic();
				m_Duck.material(0)->metallic(Metal - 0.05f);
				printf("Metallic now: %.2f\n", m_Duck.material(0)->metallic());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT, true)) {
				float Roughness = m_Duck.material(0)->roughness();
				m_Duck.material(0)->roughness(Roughness - 0.05f);
				printf("Roughness now: %.2f\n", m_Duck.material(0)->roughness());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_RIGHT, true)) {
				float Roughness = m_Duck.material(0)->roughness();
				m_Duck.material(0)->roughness(Roughness + 0.05f);
				printf("Roughness now: %.2f\n", m_Duck.material(0)->roughness());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_M, true)) {
				m_CurrentMaterial++;
				//if (m_CurrentMaterial < uint32_t(CForgeUtility::PLASTIC_WHITE)) m_CurrentMaterial = uint32_t(CForgeUtility::PLASTIC_WHITE);
				if (m_CurrentMaterial > uint32_t(CForgeUtility::DEFAULT_MATERIAL_COUNT)) m_CurrentMaterial = 0;
				CForgeUtility::defaultMaterial(m_Duck.material(0), (CForgeUtility::DefaultMaterial)m_CurrentMaterial);
			}

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			std::string ErrorMsg;
			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}
		}//mainLoop

		void run(void) override{
			while (!m_RenderWin.shutdown()) {
				mainLoop();

			}//while[main loop]
		}//run

	protected:

		//void changeUVTiling(T3DMesh<float> *pMesh, Eigen::Vector3f Factor) {

		//	std::vector<Vector3f> UVWs;
		//	for (uint32_t i = 0; i < pMesh->textureCoordinatesCount(); ++i)
		//		UVWs.push_back(pMesh->textureCoordinate(i).cwiseProduct(Factor));
		//	pMesh->textureCoordinates(&UVWs);
		//	if (pMesh->tangentCount() > 0) pMesh->computePerVertexTangents();

		//}//changeUVTiling


		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Duck;

		uint32_t m_CurrentMaterial = 0;
	};//ExampleMinimumGraphicsSetup

}//name space

#endif