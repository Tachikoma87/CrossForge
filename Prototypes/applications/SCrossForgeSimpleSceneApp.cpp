#include <glad/glad.h>
#include "SCrossForgeSimpleSceneApp.h"

#include <crossforge/graphics/controller/WindowEntityController.h>
#include <crossforge/graphics/systems/WindowSystem.h>
#include <crossforge/input/systems/KeyboardInputSystem.h>
#include "../Graphics/controllers/RenderingController.h"

#include "../scene/controllers/CameraEntityController.h"
#include "../Graphics/controllers/ActorPrefabEntityController.h"
#include "../Graphics/controllers/ShaderEntityController.h"

#include "../Graphics/controllers/TextureEntityController.h"

#include "../Graphics/controllers/Image2DController.h"

#include "../Graphics/controllers/TriangleMeshController.h"

#include <crossforge/math/CrossForgeMath.h>
#include "../scene/components/ChildObjectsComponent.h"

#include "../scene/controllers/SceneObjectEntityController.h"
#include "../scene/components/PrefabComponent.h"

namespace crossforge {

	std::shared_ptr<SCrossForgeSimpleSceneApp> SCrossForgeSimpleSceneApp::m_pInstance = nullptr;

	std::shared_ptr<SCrossForgeSimpleSceneApp> SCrossForgeSimpleSceneApp::instance() {
		if (nullptr == m_pInstance) {
			SCrossForgeSimpleSceneApp* pApp = new SCrossForgeSimpleSceneApp();
			m_pInstance = std::make_shared<SCrossForgeSimpleSceneApp>(*pApp);
		}
		return m_pInstance;
	}
	void SCrossForgeSimpleSceneApp::destroy() {
		m_pInstance = nullptr;
	}

	SCrossForgeSimpleSceneApp::SCrossForgeSimpleSceneApp() {


	}

	SCrossForgeSimpleSceneApp::~SCrossForgeSimpleSceneApp() {

	}


	void SCrossForgeSimpleSceneApp::initialize() {
		m_timestampStart = GeneralUtility::getTimestamp();

		// create window system
		WindowSystemPtr pWindowSystem = std::make_shared<WindowSystem>();
		pWindowSystem->initialize();
		m_pSystemManager->addSystem(pWindowSystem);

		// create main window
		m_pMainWin = std::make_shared<WindowEntity>();
		m_pEntityManager->registerEntity(m_pMainWin);
		WindowPropertiesComponentPtr pWindowProperties = std::make_shared<WindowPropertiesComponent>();
		pWindowProperties->position() = Eigen::Vector2i((1920 - 1280) / 2, (1080 - 720)*(1.0f/3.0f));
		pWindowProperties->height() = 720;
		pWindowProperties->width() = 1280;
		m_pMainWin->addComponent(pWindowProperties);

		if (!WindowEntityController::initOpenGLWindow(m_pMainWin)) {
			LogError("Initializing OpenGL window failed!");
			return;
		}
		else {
			LogInfo("Successfully initialized main window!");
			pWindowSystem->registerEntity(m_pMainWin);
			gladLoadGL();

		}

		// initialize
		m_pMainCanvas = std::make_shared<CanvasEntity>();
		CanvasSettingsComponentPtr pCanvasSettings = std::make_shared<CanvasSettingsComponent>();
		pCanvasSettings->viewportPosition() = Eigen::Vector2i(0, 0);
		pCanvasSettings->viewportSize() = Eigen::Vector2i(1280, 720);
		m_pMainCanvas->addComponent(pCanvasSettings);

		// initialize input
		m_pSystemManager->addSystem(std::make_shared<KeyboardInputSystem>());
		m_pInputDevice = std::make_shared<InputDeviceEntity>();
		m_pEntityManager->registerEntity(m_pInputDevice);
		AssociatedWindowComponentPtr pWindowComponent = std::make_shared<AssociatedWindowComponent>();
		pWindowComponent->windowEntityId() = m_pMainWin->getEntityId();
		m_pInputDevice->addComponent(pWindowComponent);
		m_pSystemManager->getSystem<KeyboardInputSystem>()->registerEntity(m_pInputDevice);
		
		m_pAssetIO = AssetIOProvider::instance();

		// initialize duck prefab
		std::string modelFilepath = "./Assets/ExampleScenes/Duck/Duck.gltf";
		
		//modelFilepath = "./Assets/ExampleScenes/Sponza/Sponza.gltf";

		TriangleMeshEntityPtr pDuckTriangleMesh = std::make_shared<TriangleMeshEntity>();
		if (!m_pAssetIO->loadMesh(pDuckTriangleMesh, modelFilepath)) {
			LogError("Failed to load duck mesh!");
		}
		else {
			if (!TriangleMeshController::recomputeVertexNormals(pDuckTriangleMesh)) LogError("Recomputing vertex normals failed.");
			m_pDuckActorPrefab = std::make_shared<ActorPrefabEntity>(0);
			if (!ActorPrefabEntityController::buildStaticActor(m_pDuckActorPrefab, pDuckTriangleMesh)) {
				LogError("Failed to build duck static actor!");
			}
			else {
				m_pEntityManager->registerEntity(m_pDuckActorPrefab);
				LogInfo("Duck prefab actor build successfully.");
			}
		}

		modelFilepath = "./Assets/ExampleScenes/Helmet/DamagedHelmet.gltf";
		TriangleMeshEntityPtr pHelmetTriangleMesh = std::make_shared<TriangleMeshEntity>();
		if (!m_pAssetIO->loadMesh(pHelmetTriangleMesh, modelFilepath)) {
			LogError("Failed to load helmet mesh!");
		}
		else {
			if (!TriangleMeshController::recomputeVertexNormals(pHelmetTriangleMesh)) LogError("Recomputing vertex normals failed.");
			m_pHelmetActorPrefab = std::make_shared<ActorPrefabEntity>();
			if (!ActorPrefabEntityController::buildStaticActor(m_pHelmetActorPrefab, pHelmetTriangleMesh)) {
				LogError("Failed to build helmet static actor.");
			}
			else {
				m_pEntityManager->registerEntity(m_pHelmetActorPrefab);
			}
		}

		
		// initialize camera
		m_pCameraEntity = std::make_shared<CameraEntity>(CameraEntity::COMPONENTS_ALL);
		CameraEntityController::computePerspectiveProjectionMatrix(m_pCameraEntity, m_pMainCanvas);
		auto pCameraTransform = m_pCameraEntity->getTransformation3DComponent();
		pCameraTransform->localPosition() = Eigen::Vector3f(0.0f, 0.0f, 40.0f);
		//pCameraTransform->globalPosition() = Eigen::Vector3f(0.0f, 100.0f, 250.0f);
		pCameraTransform->globalPosition() = Eigen::Vector3f(0.0f, 0.0f, 5.0f);
	

		

		// initialize duck actor instance
		m_pDuckActorInstance = std::make_shared<ActorInstanceEntity>(ActorInstanceEntity::COMPONENTS_ALL);
		m_pDuckActorInstance->addComponent(std::make_shared<PrefabComponent>());
		m_pDuckActorInstance->getComponent<PrefabComponent>()->actorPrefab()= m_pDuckActorPrefab;
		m_pDuckActorInstance->getMovement3DComponent()->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(-5.0f), Eigen::Vector3f::UnitZ());
		m_pDuckActorInstance->getTransformation3DComponent()->localScale() = Eigen::Vector3f(0.01f, 0.01f, 0.01f);
		m_pEntityManager->registerEntity(m_pDuckActorInstance);

		m_pDuckActorInstance2 = std::make_shared<ActorInstanceEntity>(ActorInstanceEntity::COMPONENTS_ALL);
		m_pDuckActorInstance2->addComponent(std::make_shared<PrefabComponent>());
		m_pDuckActorInstance2->getComponent<PrefabComponent>()->actorPrefab() = m_pDuckActorPrefab;
		m_pDuckActorInstance2->getTransformation3DComponent()->localPosition() = Eigen::Vector3f(-5.0f, -2.0f, 0.0f);
		m_pDuckActorInstance2->getMovement3DComponent()->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(2.5f), Eigen::Vector3f::UnitY());
		m_pDuckActorInstance2->getTransformation3DComponent()->localScale() = Eigen::Vector3f(0.0025f, 0.0025f, 0.0025f);
		m_pEntityManager->registerEntity(m_pDuckActorInstance2);

		
		m_pHelmetActorInstance = std::make_shared<ActorInstanceEntity>(ActorInstanceEntity::COMPONENTS_ALL);
		m_pHelmetActorInstance->addComponent(std::make_shared<PrefabComponent>());
		m_pHelmetActorInstance->getComponent<PrefabComponent>()->actorPrefab() = m_pHelmetActorPrefab;
		m_pHelmetActorInstance->getTransformation3DComponent()->localPosition() = Eigen::Vector3f(2.0f, 4.0f, 1.0f);
		m_pHelmetActorInstance->getMovement3DComponent()->positionDelta() = Eigen::Vector3f(0.0f, 0.05f, 0.0f);
		m_pEntityManager->registerEntity(m_pHelmetActorInstance);




		// initialize scene lights
		m_pSceneLights = std::make_shared<LightsEntity>(LightsEntity::COMPONENTS_ALL);
		auto pLightsConfig = m_pSceneLights->getLightsConfigComponent();

		auto pDirLights = m_pSceneLights->getUBODirectionalLightsComponent();
		pDirLights->initialize(5);
		for (uint8_t i = 0; i < 5; ++i) {
			pDirLights->setColor(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 5.0f), i);
			pDirLights->setShadowId(-1, i);
		}

		pDirLights->setColor(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 3.0f), 1);
		pDirLights->setColor(Eigen::Vector4f(0.0f, 0.0f, 1.0f, 5.0f), 2);
		pLightsConfig->directionalLightsUBOSize() = 5;
		pLightsConfig->activeDirectionalLights() = 3;

	

		Eigen::Vector3f dir = Eigen::Vector3f::Zero() - Eigen::Vector3f(0.0f, 50.0f, -150.0f);
		pDirLights->setDirection(dir.normalized(), 0);
		dir = Eigen::Vector3f::Zero() - Eigen::Vector3f(-25.0f, -25.0f, -200.0f);
		pDirLights->setDirection(dir.normalized(), 1);
		dir = Eigen::Vector3f::Zero() - Eigen::Vector3f(0.0f, 10.0f, -10.0f);
		pDirLights->setDirection(dir.normalized(), 2);


		m_pRootNode = std::make_shared<SceneObjectEntity>();
		m_pEntityManager->registerEntity(m_pRootNode);
		m_pRootNode->addComponent(std::make_shared<Transformation3DComponent>());
		m_pRootNode->addComponent(std::make_shared<Movement3DComponent>());
		m_pRootNode->addComponent(std::make_shared<ChildObjectsComponent>());
		//m_pRootNode->getComponent<Movement3DComponent>()->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(1.0f), Eigen::Vector3f::UnitY());

		m_pMovementSystem = std::make_shared<MovementSystem>();
		m_pMovementSystem->registerEntity(m_pDuckActorInstance);
		m_pMovementSystem->registerEntity(m_pDuckActorInstance2);
		m_pMovementSystem->registerEntity(m_pHelmetActorInstance);
		m_pMovementSystem->registerEntity(m_pRootNode);

		auto pTransformComp = m_pRootNode->getTransformation3DComponent();
		auto pChildObjectsComp = m_pRootNode->getChildObjectsComponent();

		//pTransformComp->localRotation() = Eigen::AngleAxisf(CrossForgeMath::degToRad(90.0f), Eigen::Vector3f::UnitY());
		pChildObjectsComp->addChild(m_pDuckActorInstance);
		pChildObjectsComp->addChild(m_pDuckActorInstance2);
		pChildObjectsComp->addChild(m_pHelmetActorInstance);

		for (auto pObj : m_pActorInstances) pChildObjectsComp->addChild(pObj);
	

		m_pSceneEntity = std::make_shared<SceneEntity>(SceneEntity::COMPONENTS_ALL);


		m_frameCount = 0;
		m_timestampLastFpsPrint = 0;

	}
	void SCrossForgeSimpleSceneApp::update() {

		auto pWinSystem = m_pSystemManager->getSystem<WindowSystem>();
		auto pKeyboardSystem = m_pSystemManager->getSystem<KeyboardInputSystem>();
		pWinSystem->update();
		pKeyboardSystem->update();

		// clear window buffer and activate canvas
		WindowEntityController::clearBuffer(m_pMainWin);
		RenderingController::activateCanvas(m_pMainCanvas, true);


		// set the camera
		CameraEntityController::computeCameraMatrixFromTransformation(m_pCameraEntity);

		m_pMovementSystem->update();
	
		SceneObjectEntityController::buildGlobalTransformation(m_pRootNode);

		
		// gather renderable models and draw them
		std::vector<SceneObjectEntityPtr> renderables;
		SceneObjectEntityController::gatherRenderableObjects(m_pRootNode, renderables);

		for (auto pObj : renderables) {
			if (pObj->isInstanceOf(ActorInstanceEntity::identification)) {
				SceneObjectEntityController::updateTransformationUbo(pObj);
				ActorInstanceEntityPtr pActor = std::static_pointer_cast<ActorInstanceEntity>(pObj);
				RenderingController::drawActor(ShaderProvider::RENDER_PASS_FORWARD, pActor, pObj->getComponent<PrefabComponent>()->actorPrefab(), m_pCameraEntity, m_pSceneLights);
			}
		}

	
		pWinSystem->swapBuffers();
		m_frameCount++;


		auto pos = m_pHelmetActorInstance->getTransformation3DComponent()->localPosition();
		if (pos.y() < -5.0f || pos.y() > 5.0f) m_pHelmetActorInstance->getMovement3DComponent()->positionDelta() *= -1.0f;

		auto pKeyboard = m_pInputDevice->getKeyboardDataComponent();
		auto pCameraTransform = m_pCameraEntity->getTransformation3DComponent();
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_W)) {
			pCameraTransform->globalPosition() += Eigen::Vector3f(0.0f, 0.0f, -0.25f);
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_S)) {
			pCameraTransform->globalPosition().z() += 0.25f;
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_A)) {
			pCameraTransform->globalPosition().x() -= 0.25f;
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_D)) {
			pCameraTransform->globalPosition().x() += 0.25f;
		}

		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_9)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->metallic() <= 1.0f) pMat->metallic() += 0.01f;
				else pMat->metallic() = 1.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_8)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->metallic() >= 0.0f) pMat->metallic() -= 0.01f;
				else pMat->metallic() = 0.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_7)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->roughness() <= 1.0f) pMat->roughness() += 0.01f;
				else pMat->roughness() = 1.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_6)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->roughness() >= 0.0f) pMat->roughness() -= 0.01f;
				else pMat->roughness() = 0.0f;
				pMat->updateUbo();
			}
		}

		if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_5)) {
			auto pLightsConfig = m_pSceneLights->getLightsConfigComponent();
			pLightsConfig->activeDirectionalLights() = (pLightsConfig->activeDirectionalLights() + 1)%4;
			pKeyboard->keyState(KeyboardDataComponent::KEY_5) = KeyboardDataComponent::KEYSTATE_OFF;
		}

		
		if (GeneralUtility::getTimestamp() - m_timestampLastFpsPrint > 1000) {
			int32_t runtimeSeconds = (GeneralUtility::getTimestamp() - m_timestampStart) / 1000;
			//LogInfo("Application is running for " + std::to_string(runtimeSeconds) + " seconds now\n");
			if (nullptr == m_pMainWin->getWindowPropertiesComponent()->getGlfwWindowHandle()) {
				LogInfo("Main window was closed. Exiting application now!\n");
				this->stop();
			}

			if (m_pInputDevice->getKeyboardDataComponent()->isKeyRelease(KeyboardDataComponent::KEY_ESCAPE)) {
				this->stop();
			}

			float FPS = m_frameCount / (float)((GeneralUtility::getTimestamp() - m_timestampLastFpsPrint) / 1000.0f);

			LogInfo("FPS: " + std::to_string(FPS));
			m_frameCount = 0;

			uint64_t timestamp = GeneralUtility::getTimestamp() % 100;
			m_pMainWin->getWindowPropertiesComponent()->clearColor() = Eigen::Vector4f(timestamp / 100.0f, timestamp / 50.0f, timestamp / 60.0f, 1.0f);
			m_timestampLastFpsPrint = GeneralUtility::getTimestamp();
		}
		

	}
}