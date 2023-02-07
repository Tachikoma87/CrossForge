/*****************************************************************************\
*                                                                           *
* File(s): SurfaceSamplerTestScene.hpp                                            *
*                                                                           *
* Content:    *
*                       *
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
#ifndef __CFORGE_SURFACESAMPLERTESTSCENE_HPP__
#define __CFORGE_SURFACESAMPLERTESTSCENE_HPP__

#include "../../Examples/ExampleSceneBase.hpp"
#include "../MeshProcessing/SurfaceSampler.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class SurfaceSamplerTestScene : public ExampleSceneBase {
	public:
		SurfaceSamplerTestScene(void) {
			m_WindowTitle = "CForge Prototype - Surface Sampler";
			m_SampleCount = 10;
		}//Constructor

		~SurfaceSamplerTestScene(void) {
			clear();
		}//Destructor

		void init(void) override{
			initWindowAndRenderDevice();
			initCameraAndLights();

			T3DMesh<float> M;
			

			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.fbx", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("Assets/tmp/Sphere.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			for (uint32_t i = 0; i < M.materialCount(); ++i) M.getMaterial(i)->Color = Vector4f(0.0f, 0.0f, 1.0f, 1.0f); // red spheres
			m_Sphere.init(&M);
			M.clear();

			SAssetIO::load("Assets/tmp/MuscleManPosed.obj", &m_ModelData);
			setMeshShader(&m_ModelData, 0.1f, 0.04f);
			m_ModelData.computePerVertexNormals();
			m_Surface.init(&m_ModelData);


			// build scene graph	
			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			// add skydome
			m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			m_SkydomeSGN.scale(Vector3f(5.0f, 5.0f, 5.0f));

			// add muscle man		
			m_MuscleManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 3.0f, 0.0f));
			m_MuscleManTransformSGN.scale(Vector3f(1.0f / 40.0f, 1.0f / 40.0f, 1.0f / 40.0f));
			m_MuscleManSGN.init(&m_MuscleManTransformSGN, &m_Surface);

			// rotate about the y-axis at 45 degree every second
			Quaternionf R = Quaternionf::Identity();
			R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());

			m_MuscleManTransformSGN.rotationDelta(R);

			// prepare node to add markers		
			m_MarkerGroupSGN.init(&m_MuscleManTransformSGN); // markers will always belong to this object for the purpose of this demo

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

			// setup for the surface sampler
			
			// we use MuscleMan data already stored in M
			m_SSampler.init(&m_ModelData);

			m_FPS = 60.0f;
			m_FPSCount = 0;
			m_LastFPSPrint = CForgeUtility::timestamp();

		}//initialize

		void clear(void) override{
			ExampleSceneBase::clear();
		}//clear

		void mainLoop(void) override {

			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);


			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_0, true)) {
				float MaxDist = 0.5f;
				std::vector<int32_t> Samples;
				m_SSampler.sampleEquidistant(MaxDist, m_SampleCount, &Samples);

				// add marker at position of sample point
				for (auto i : Samples) {
					SGNTransformation* pMarkerSGN = new SGNTransformation();
					SGNTransformation* pMarkerSGN2 = new SGNTransformation();
					SGNGeometry* pMarkerGeomSGN = new SGNGeometry();

					pMarkerSGN->init(&m_MarkerGroupSGN, m_ModelData.vertex(i).cwiseProduct(m_MuscleManTransformSGN.scale()));
					pMarkerSGN2->init(pMarkerSGN, Vector3f::Zero(), m_MuscleManTransformSGN.rotation(), m_MuscleManTransformSGN.scale(), Vector3f::Zero(), m_MuscleManTransformSGN.rotationDelta());
					pMarkerGeomSGN->init(pMarkerSGN2, &m_Sphere, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(25.0f, 25.0f, 25.0f));
				}
			}

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}//while[main loop]

	protected:
		StaticActor m_Skydome;
		StaticActor m_Surface;
		SkeletalAnimationController m_MuscleManController;
		StaticActor m_Sphere;

		SurfaceSampler m_SSampler;

		SGNTransformation m_RootSGN;

		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_MuscleManSGN;
		SGNTransformation m_MuscleManTransformSGN;

		SGNTransformation m_MarkerGroupSGN;

		T3DMesh<float> m_ModelData;

		uint32_t m_SampleCount;
	};//SurfaceSamplerTestScene



	

}

#endif