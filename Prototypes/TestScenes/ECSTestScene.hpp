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

#include "../AssetIO/Systems/AssimpMeshIOSystem.h"

#include "../Graphics/Systems/WindowSystem.h"

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

		bool isActive()override {
			return true;
		}

		void initECS() {

			EntityManagerPtr entityMan = EntityManager::instance();

			uint64_t startTimestamp = CForgeUtility::timestamp();

			for (int i = 0; i < 50; ++i) {
				EntityBasePtr pEntity = std::make_shared<EntityBase>(EntityBase::identification);
				pEntity->setEntityId(entityMan->registerEntity(pEntity));

				if ( (i % 20) != 0) {
					PositionComponent2DPtr pPos = std::make_shared<PositionComponent2D>();
					pEntity->addComponent(pPos);
				}
				
			}

			auto entityList = entityMan->getEntities<EntityBase>();

			for (EntityBasePtr pEntity : entityList) {
				std::string message = "I found entity with id " + std::to_string(pEntity->getEntityId()) + " of type " + pEntity->getIdentification() + "\n";
				//printf(message.c_str());

				PositionComponent2DPtr pPos = pEntity->getComponent<PositionComponent2D>(PositionComponent2D::identification);
				if (nullptr != pPos) {
					message = "Entity " + std::to_string(pEntity->getEntityId()) + " has position component\n";
					//printf(message.c_str());
				}
			}

			// create movement system
			MovementSystemPtr movementSystem = std::make_shared<MovementSystem>();
			for (EntityBasePtr pEntity : entityList) {
				if (movementSystem->isEntityValid(pEntity)) {
					movementSystem->addEntity(pEntity);
				}
			}

			SystemManagerPtr pSysMan = SystemManager::getInstance();
			pSysMan->addSystem(movementSystem);

			std::string msg = "Method took " + std::to_string(CForgeUtility::timestamp() - startTimestamp) + "ms to complete.\n";
			printf(msg.c_str());
		}

		void testSystems() {
			AssimpMeshIOSystemPtr pAssimp = std::make_shared<AssimpMeshIOSystem>();

			TriangleMeshEntityPtr pDuck = std::make_shared<TriangleMeshEntity>();

			pAssimp->loadMesh("./Assets/ExampleScenes/Duck/Duck.gltf", pDuck);
			pAssimp->storeMesh("./Assets/duck.obj", pDuck);

			printf("Loaded duck ...");

		}

		void init() override {

			// initialize manager
			m_pEntityManager = EntityManager::instance();

			// initialize systems
			m_pWindowSystem = std::make_shared<WindowSystem>();

			// create a windows
			WindowEntityPtr pMainWin = std::make_shared<WindowEntity>();
			WindowPropertiesComponentPtr pMainWinProps = std::make_shared<WindowPropertiesComponent>();
			pMainWin->addComponent(pMainWinProps);

			if (m_pWindowSystem->initOpenGLWindow(pMainWin)) {
				LogInfo("Successfully created main OpenGL window.", "");
				m_pWindowSystem->registerEntity(pMainWin);
			}
			else {
				LogError("Failed to create main OpenGL window.", "");
			}

			
			

		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear


		void mainLoop(void)override {
			
		
			m_pWindowSystem->update();
			m_pWindowSystem->swapBuffers();

			
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			

		}//mainLoop

	protected:

		EntityManagerPtr m_pEntityManager;
		WindowSystemPtr m_pWindowSystem;
		

	};//ExampleMinimumGraphicsSetup

}//name space

#endif