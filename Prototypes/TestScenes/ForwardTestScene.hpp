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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_FORWARDTESTSCENE_HPP__
#define __CFORGE_FORWARDTESTSCENE_HPP__


#include "Examples/ExampleSceneBase.hpp"
#include <CForge/MeshProcessing/PrimitiveShapeFactory.h>

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

			// load skydome and a textured cube
			T3DMesh<float> M;

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.gltf", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();



			SAssetIO::load("MyAssets/Technique_Evaluation/OldModel.gltf", &M);
			M.clearSkeleton();
			//PrimitiveShapeFactory::uvSphere(&M, Vector3f(1.0f, 1.0f, 1.0f), 20, 20);
			//PrimitiveShapeFactory::cuboid(&M, Vector3f(2, 2, 2), Vector3i(2, 2, 2));
			//PrimitiveShapeFactory::plane(&M, Vector2f(10, 10), Vector2i(1, 1));
			//changeUVTiling(&M, Vector3f(3.0f, 3.0f, 1.0f));
			
			setMeshShader(&M, 0.1f, 0.04f);

			float ModelScale = 0.05f;

			/*M.getMaterial(0)->FragmentShaderForwardPass.clear();
			M.getMaterial(0)->VertexShaderForwardPass.clear();*/
			/*M.getMaterial(0)->VertexShaderGeometryPass.clear();
			M.getMaterial(0)->VertexShaderShadowPass.clear();
			M.getMaterial(0)->FragmentShaderGeometryPass.clear();
			M.getMaterial(0)->FragmentShaderShadowPass.clear();*/

			/*M.getMaterial(0)->FragmentShaderForwardPass[0] ="Shader/CrippledShader.frag";
			M.getMaterial(0)->VertexShaderForwardPass[0] = "Shader/CrippledShader.vert";*/

			CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::PLASTIC_WHITE);

			/*M.getMaterial(0)->TexAlbedo = "MyAssets/Textures/ground13.jpg";
			M.getMaterial(0)->TexNormal = "MyAssets/Textures/ground13n.jpg";*/

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