/*****************************************************************************\
*                                                                           *
* File(s): exampleMultiViewport.hpp                                            *
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
#ifndef __CFORGE_EXAMPLEMULTIVIEWPORT_HPP__
#define __CFORGE_EXAMPLEMULTIVIEWPORT_HPP__

#include "exampleSceneBase.hpp"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleMultiViewport : public ExampleSceneBase {
	public:
		ExampleMultiViewport(void) {
			m_WindowTitle = "CrossForge Example - Multiple Viewports";
			m_RenderBufferScale = 2; // only half resolution of render buffer (GBuffer)
		}//Constructor

		~ExampleMultiViewport(void) {
			clear();
		}//Destructor

		void init(void) {

			initWindowAndRenderDevice();
			initCameraAndLights();

			// load skydome and a textured cube
			T3DMesh<float> M;
			
			SAssetIO::load("Assets/ExampleScenes/SimpleSkydome.glb", &M);
			setMeshShader(&M, 0.8f, 0.04f);
			M.computePerVertexNormals();
			m_Skydome.init(&M);
			M.clear();

			SAssetIO::load("Assets/ExampleScenes/Helmet/DamagedHelmet.gltf", &M);
			setMeshShader(&M, 0.2f, 0.24f);
			M.computePerVertexNormals();
			M.computePerVertexTangents();
			m_Helmet.init(&M);
			M.clear();

			// build scene graphs (one for every viewport)

			float Speed = 10.0f; // Degree per second
			Quaternionf HelmetRotationDeltas[4];
			HelmetRotationDeltas[0] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitX());
			HelmetRotationDeltas[1] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitY());
			HelmetRotationDeltas[2] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f::UnitZ());
			HelmetRotationDeltas[3] = AngleAxisf(GraphicsUtility::degToRad(Speed / 60.0f), Vector3f(1.0f, 1.0f, 1.0f).normalized());

			for (uint8_t i = 0; i < 4; ++i) {
				m_RootSGNs[i].init(nullptr);
				m_SGs[i].init(&m_RootSGNs[i]);

				// add skydome
				m_DomeGeomSGNs[i].init(&m_RootSGNs[i], &m_Skydome, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(50.0f, 50.0f, 50.0f));
				// add helmet
				m_HelmetTransSGNs[i].init(&m_RootSGNs[i], Vector3f(0.0f, 3.5f, 0.0f));
				m_HelmetGeomSGNs[i].init(&m_HelmetTransSGNs[i], &m_Helmet, Vector3f::Zero(), Quaternionf::Identity(), Vector3f(3.0f, 3.0f, 3.0f));

				// let the helmets spin
				m_HelmetTransSGNs[i].rotationDelta(HelmetRotationDeltas[i]);
			}

			// we need one viewport for the GBuffer		
			m_GBufferVP.Position = Vector2i(0, 0);	
			m_GBufferVP.Size = Vector2i(m_RenderDev.gBuffer()->width(), m_RenderDev.gBuffer()->height());
			m_RenderDev.viewport(RenderDevice::RENDERPASS_GEOMETRY, m_GBufferVP);

			updateViewportsAndCamera();

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CoreUtility::timestamp();
			int32_t FPSCount = 0;

			std::string GLError = "";
			GraphicsUtility::checkGLError(&GLError);
			if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());
		}//initialize

		void clear(void) {
			ExampleSceneBase::clear();
		}//clear

		void run(void) {
			while (!m_RenderWin.shutdown()) {
				m_RenderWin.update();
				defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

				// perform rendering for the 4 viewports
				for (uint8_t i = 0; i < 4; ++i) {
					m_SGs[i].update(60.0f / m_FPS);

					// render scene as usual
					//m_RenderDev.viewport(RenderDevice::RENDERPASS_GEOMETRY, m_GBufferVP);
					m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
					m_SGs[i].render(&m_RenderDev);
					m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
					m_SGs[i].render(&m_RenderDev);

					// set viewport and perform lighting pass
					// this will produce the correct tile in the final output window (backbuffer to be specific)
					m_RenderDev.viewport(RenderDevice::RENDERPASS_LIGHTING, m_VPs[i]);
					m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING, nullptr, (i == 0) ? true : false);
				}

				m_RenderWin.swapBuffers();

				updateFPS();
				defaultKeyboardUpdate(m_RenderWin.keyboard());

			}//while[main loop]
		}//run
	protected:

		void updateViewportsAndCamera(void) {
			uint32_t RenderWinWidth = m_RenderWin.width();
			uint32_t RenderWinHeight = m_RenderWin.height();

			uint32_t Margin = 14;
			Vector2i VPSize = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) - 2 * Vector2i(Margin, Margin) + Vector2i(Margin / 2, Margin / 2);

			// Top left
			m_VPs[0].Position = Vector2i(0, RenderWinHeight / 2) + Vector2i(Margin, Margin / 2);
			// top right
			m_VPs[1].Position = Vector2i(RenderWinWidth / 2, RenderWinHeight / 2) + Vector2i(Margin / 2, Margin / 2);
			// bottom left
			m_VPs[2].Position = Vector2i(Margin, Margin);
			// bottom right
			m_VPs[3].Position = Vector2i(RenderWinWidth / 2, 0) + Vector2i(Margin / 2, Margin);
			for (uint8_t i = 0; i < 4; ++i) m_VPs[i].Size = VPSize;

			m_Cam.projectionMatrix(m_VPs[0].Size[0], m_VPs[0].Size[1], GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

			m_GBufferVP.Size = Vector2i(m_RenderDev.gBuffer()->width(), m_RenderDev.gBuffer()->height());
			m_RenderDev.viewport(RenderDevice::RENDERPASS_GEOMETRY, m_GBufferVP);

			// viewport for forward pass (required for correct screenshots)
			RenderDevice::Viewport VFPass;
			VFPass.Position = Vector2i(0, 0);
			VFPass.Size = Vector2i(RenderWinWidth, RenderWinHeight);
			m_RenderDev.viewport(RenderDevice::RENDERPASS_FORWARD, VFPass);

		}//updateViewports

		void listen(GLWindowMsg Msg) {
			ExampleSceneBase::listen(Msg);

			updateViewportsAndCamera();
		}

		StaticActor m_Skydome;
		StaticActor m_Helmet;

		SceneGraph m_SGs[4];
		SGNTransformation m_RootSGNs[4];

		SGNTransformation m_HelmetTransSGNs[4];
		SGNGeometry m_DomeGeomSGNs[4];
		SGNGeometry m_HelmetGeomSGNs[4];

		// required viewports
		RenderDevice::Viewport m_GBufferVP;
		RenderDevice::Viewport m_VPs[4];
	};//ExampleMultiViewport


	

}

#endif