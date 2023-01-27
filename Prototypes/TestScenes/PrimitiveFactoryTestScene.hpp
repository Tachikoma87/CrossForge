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


#include "../../Examples/exampleSceneBase.hpp"
//#include "../MeshProcessing/PrimitiveShapeFactory.h"
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

		void init() {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			M.computeAxisAlignedBoundingBox();
			m_Skydome.init(&M);
			M.clear();

			//SAssetIO::load("Assets/ExampleScenes/Duck/Duck.gltf", &M);
			//PrimitiveFactory::plane(&M, Vector2f(4.0f, 4.0f), Vector2i(8, 3));
			//PrimitiveFactory::circle(&M, Vector2f(3.0f, 3.0f), 20, 0.0f);
			//PrimitiveFactory::cuboid(&M, Vector3f(4, 3, 2), Vector3i(10, 10, 10));
			//PrimitiveFactory::uvSphere(&M, Vector3f(2, 2, 2), 25, 25);		
			//PrimitiveFactory::doubleCone(&M, Vector4f(1.0f, 1.5f, 1.0f, 0.0f), 35);
			//PrimitiveFactory::cylinder(&M, Vector2f(2.0f, 2.0f), Vector2f(2.0f, 2.0f), 3.0f, 20, Vector2f(0.0f, 0.0f));
			PrimitiveShapeFactory::Torus(&M, 4.0f, 1.5f, 20, 10);
			

			//M.getMaterial(0)->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
			//M.getMaterial(0)->Color = Vector4f(0xcc, 0xac, 0x00, 0xff) / 255.0f;
			M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/ground14.jpg";
			//M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/tex/rp_eric_rigged_001_dif.jpg";
			//M.getMaterial(0)->TexAlbedo = "Assets/ExampleScenes/StarCoin/MaterialStar_baseColor.jpg";
			setMeshShader(&M, 0.2f, 0.04f);
			M.computePerVertexNormals();
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

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		}//initialize

		void clear(void) {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				m_SG.update(60.0f / m_FPS);

				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
				m_SG.render(&m_RenderDev);

				m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

				m_RenderWin.swapBuffers();

				updateFPS();

				if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
					SGNGeometry::Visualization Vis = m_DuckSGN.visualization();
					m_DuckSGN.visualization((Vis == SGNGeometry::VISUALIZATION_WIREFRAME) ? SGNGeometry::VISUALIZATION_FILL : SGNGeometry::VISUALIZATION_WIREFRAME);
					
				}

				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run

	protected:

		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Duck;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif