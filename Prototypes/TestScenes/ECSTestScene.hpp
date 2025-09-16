/*****************************************************************************\
*                                                                           *
* File(s): ECSTestScene.hpp                                            *
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
#ifndef __CFORGE_ECSTESTSCENE_HPP__
#define __CFORGE_ECSTESTSCENE_HPP__


#include "../../Examples/ExampleSceneBase.hpp"

#include "../ECS/SEntityManager.h"
#include "../ECS/EntityBase.h"
#include "../ECS/ComponentBase.h"
#include "../ECS/SystemBase.h"

#include "../ECS/PrototypeComponents/PositionComponent2D.h"
#include "../ECS/PrototypeComponents/MovementSystem.h"

#include "../ECS/SSystemManager.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ECSTestScene : public ExampleSceneBase {
	public:
		ECSTestScene(void) {
			m_WindowTitle = "CrossForge - Entity Component Test Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~ECSTestScene(void) {
			clear();
		}//Destructor

		void initECS() {

			EntityManagerPtr entityMan = EntityManager::Instance();

			uint64_t startTimestamp = CForgeUtility::timestamp();

			for (int i = 0; i < 50; ++i) {
				EntityBasePtr pEntity = std::make_shared<EntityBase>(EntityBase::identification);
				pEntity->SetEntityId(entityMan->RegisterEntity(pEntity));

				if ( (i % 20) != 0) {
					PositionComponent2DPtr pPos = std::make_shared<PositionComponent2D>();
					pEntity->AddComponent(pPos);
				}
				
			}

			auto entityList = entityMan->GetEntities<EntityBase>();

			for (EntityBasePtr pEntity : entityList) {
				std::string message = "I found entity with id " + std::to_string(pEntity->GetEntityId()) + " of type " + pEntity->GetIdentification() + "\n";
				//printf(message.c_str());

				PositionComponent2DPtr pPos = pEntity->GetComponent<PositionComponent2D>(PositionComponent2D::identification);
				if (nullptr != pPos) {
					message = "Entity " + std::to_string(pEntity->GetEntityId()) + " has position component\n";
					//printf(message.c_str());
				}
			}

			// create movement system
			MovementSystemPtr movementSystem = std::make_shared<MovementSystem>();
			for (EntityBasePtr pEntity : entityList) {
				if (movementSystem->IsEntityValid(pEntity)) {
					movementSystem->AddEntity(pEntity);
				}
			}

			SystemManagerPtr pSysMan = SystemManager::GetInstance();
			pSysMan->AddSystem(movementSystem);

			std::string msg = "Method took " + std::to_string(CForgeUtility::timestamp() - startTimestamp) + "ms to complete.\n";
			printf(msg.c_str());
		}

		void init() override {

			initWindowAndRenderDevice();
			initCameraAndLights();
			initFPSLabel();

			initECS();

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


			// update ECS
			uint64_t timestamp = CForgeUtility::timestamp();
			MovementSystemPtr pMovementSystem = SystemManager::GetInstance()->GetSystem<MovementSystem>(MovementSystem::identification);
			pMovementSystem->Update();

			auto entityList = EntityManager::Instance()->GetEntities<EntityBase>();

			for (EntityBasePtr pEntity : entityList) {
				if (pEntity->HasComponent(PositionComponent2D::identification) && pEntity->GetEntityId() == 9) {
					auto pos = pEntity->GetComponent<PositionComponent2D>(PositionComponent2D::identification)->GetPosition();

					std::string msg = "Entity " + std::to_string(pEntity->GetEntityId()) + ": " +
						std::to_string(pos.x()) + "   " + std::to_string(pos.y()) + "\n";
					printf(msg.c_str());

				}
			}

			std::string msg = "Movement system update took " + std::to_string(CForgeUtility::timestamp() - timestamp) + " ms\n";
			//printf(msg.c_str());

		}//mainLoop

	protected:

		// Scene Graph
		SGNTransformation m_RootSGN;

		StaticActor m_Duck;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

	};//ExampleMinimumGraphicsSetup

}//name space

#endif