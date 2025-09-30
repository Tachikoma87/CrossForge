#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <crossforge/utility/GeneralUtility.hpp>
#include "SCrossForgeTestApp.h"
#include "../Graphics/Systems/RenderingSystem.h"
#include <crossforge/graphics/entities/SceneEntity.h>
#include "../ECS/PrototypeComponents/PositionComponent2D.h"
#include <crossforge/graphics/controller/WindowEntityController.h>

#include "../AssetIO/controller/FileIOSystemController.h"
#include "../AssetIO/controller/Image2DIOStbController.h"
#include "../AssetIO/controller/TriangleMeshIOAssimpController.h"

#include "../AssetIO/SAssetIOProvider.h"

namespace crossforge {
	std::shared_ptr<SCrossForgeTestApp> SCrossForgeTestApp::m_pInstance = nullptr;

	std::shared_ptr<SCrossForgeTestApp> SCrossForgeTestApp::instance() {
		if (nullptr == m_pInstance) {
			SCrossForgeTestApp* pApp = new SCrossForgeTestApp();
			m_pInstance = std::make_shared<SCrossForgeTestApp>(*pApp);
		}
		return m_pInstance;
	}
	void SCrossForgeTestApp::destory() {
		m_pInstance = nullptr;
	}

	void SCrossForgeTestApp::initialize() {
		

		// create window system
		WindowSystemPtr pWindowSystem = std::make_shared<WindowSystem>();
		m_pSystemManager->addSystem(pWindowSystem);
		pWindowSystem->initialize();

		m_pMainWin = std::make_shared<WindowEntity>();
		m_pEntityManager->registerEntity(m_pMainWin);
		WindowPropertiesComponentPtr pWinProps = std::make_shared<WindowPropertiesComponent>();
		pWinProps->width() = 1280;
		pWinProps->height() = 720;
		pWinProps->position() = Eigen::Vector2i(200, 200);
		m_pMainWin->addComponent(pWinProps);
		
		if (WindowEntityController::initOpenGLWindow(m_pMainWin)) {
			LogInfo("Successfully created main window with OpenGL functionality!");
			pWindowSystem->registerEntity(m_pMainWin);
			gladLoadGL();
		}
		else {
			LogError("Something went wrong creating the main OpenGl window!");
		}

		// create rendering system

		m_startTimestamp = GeneralUtility::getTimestamp();
		m_lastPrint = GeneralUtility::getTimestamp();
		this->setSleepInterval(0);

		RenderingSystemPtr pRenderSys = std::make_shared<RenderingSystem>();
		m_pSystemManager->addSystem(pRenderSys);

		// bottom left
		SceneEntityPtr pScene = std::make_shared<SceneEntity>();
		SceneSettingsComponentPtr pSceneSettings = std::make_shared<SceneSettingsComponent>();
		PositionComponent2DPtr pPos = std::make_shared<PositionComponent2D>();
		pSceneSettings->viewportPosition() = Eigen::Vector2i(0, 0);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pPos->setPosition(Eigen::Vector2f(0.25, 0.5));
		pScene->addComponent(pSceneSettings);
		pScene->addComponent(pPos);
		m_pEntityManager->registerEntity(pScene);
		pRenderSys->registerEntity(pScene);

		// top left
		pScene = std::make_shared<SceneEntity>();
		pSceneSettings = std::make_shared<SceneSettingsComponent>();
		pPos = std::make_shared<PositionComponent2D>();
		pSceneSettings->viewportPosition() = Eigen::Vector2i(0, 720 / 2);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pPos->setPosition(Eigen::Vector2f(0.8, 0.0));
		pScene->addComponent(pSceneSettings);
		pScene->addComponent(pPos);
		m_pEntityManager->registerEntity(pScene);
		pRenderSys->registerEntity(pScene);

		// top right
		pScene = std::make_shared<SceneEntity>();
		pSceneSettings = std::make_shared<SceneSettingsComponent>();
		pPos = std::make_shared<PositionComponent2D>();
		pSceneSettings->viewportPosition() = Eigen::Vector2i(1280/2, 720/2);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pPos->setPosition(Eigen::Vector2f(0.15, 0.25));
		pScene->addComponent(pSceneSettings);
		pScene->addComponent(pPos);
		m_pEntityManager->registerEntity(pScene);
		pRenderSys->registerEntity(pScene);

		// bottom right
		pScene = std::make_shared<SceneEntity>();
		pSceneSettings = std::make_shared<SceneSettingsComponent>();
		pPos = std::make_shared<PositionComponent2D>();
		pSceneSettings->viewportPosition() = Eigen::Vector2i(1280/2, 0);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pPos->setPosition(Eigen::Vector2f(0.0, 0.8));
		pScene->addComponent(pSceneSettings);
		pScene->addComponent(pPos);
		m_pEntityManager->registerEntity(pScene);
		pRenderSys->registerEntity(pScene);

		// add input stuff
		m_pInputDevice = std::make_shared<InputDeviceEntity>(InputDeviceEntity::KEYBOARD_DATA_COMPONENT | InputDeviceEntity::ASSOCIATED_WINDOW_COMPONENT | InputDeviceEntity::MOUSE_DATA_COMPONENT);
		m_pEntityManager->registerEntity(m_pInputDevice);
		auto pKeyboardData = m_pInputDevice->getKeyboardDataComponent();
		auto pAssociatedWindow = m_pInputDevice->getAssociatedWindowComponent();
		pAssociatedWindow->windowEntityId() = m_pMainWin->getEntityId();

		KeyboardInputSystemPtr pKeyboardSys = std::make_shared<KeyboardInputSystem>();
		m_pSystemManager->addSystem(pKeyboardSys);
		if (!pKeyboardSys->registerEntity(m_pInputDevice)) {
			LogError("Failed to register input device entity for keyboard input!");
		}
		else {
			LogInfo("Registered input device entity for keyboard input.");
		}

		MouseInputSystemPtr pMouseSys = std::make_shared<MouseInputSystem>();
		pMouseSys->registerEntity(m_pInputDevice);
		m_pSystemManager->addSystem(pMouseSys);
		
		testFileIO();
		testImageIO();
		testTriangleMeshIO();

	}

	void SCrossForgeTestApp::testImageIO() {
		LogInfo("Starting ImageIO test");
		uint64_t start = GeneralUtility::getTimestamp();

		std::string imageFileIn = "./Assets/ExampleScenes/Duck/DuckCM.png";
		std::vector<std::string> filesOut;
		filesOut.push_back("./Assets/Duck.jpg");
		filesOut.push_back("./Assets/Duck.png");

		AssetIOProviderPtr pAssetIO = AssetIOProvider::instance();

		for (int32_t i = 0; i < 100; ++i) {

			Image2DEntityPtr pImageEntity = std::make_shared<Image2DEntity>(Image2DEntity::COMPONENTS_ALL);

			if (!pAssetIO->loadImage2D(pImageEntity, imageFileIn)) {
				LogError("Failed to load image " + imageFileIn);
				return;
			}

			for (std::string fileOut : filesOut) {
				try {
					if (!pAssetIO->storeImage2D(pImageEntity, fileOut)) LogError("Failed to store file " + fileOut);
				}
				catch (CrossForgeException e) {
					LogError(e.message());
				}
			}
		}

		uint64_t duration = GeneralUtility::getTimestamp() - start;
		LogInfo("Image IO test took " + std::to_string(duration) + " milliseconds.");

	}

	void SCrossForgeTestApp::testFileIO() {
		LogInfo("Starting to test AssetIO!");

		uint64_t start = GeneralUtility::getTimestamp();
		AssetIOProviderPtr pAssetIO = AssetIOProvider::instance();
		
		std::string shaderFileOut = "./Assets/Temp.txt";


		for (uint32_t i = 0; i < 10; ++i) {
			FileEntityPtr pEntity = std::make_shared<FileEntity>(FileEntity::ALL_COMPONENTS);
			std::string shaderFile = "./Assets/Shader/BasicGeometryPass.frag";
		
			if (!pAssetIO->loadFile(pEntity, shaderFile, false)) {
				LogError("Failed to load file " + shaderFile);
			}
			
			pAssetIO->storeFile(pEntity, shaderFileOut, false);

			//LogInfo("Testing of AssetIO finished!");

			pEntity->initialize(FileEntity::ALL_COMPONENTS);
			std::string shaderFile2 = "./Assets/Shader/DrLightingPassPBS.frag";
			pAssetIO->loadFile(pEntity, shaderFile2, true);
			pAssetIO->storeFile(pEntity, shaderFileOut, true);
		}
		uint64_t duration = GeneralUtility::getTimestamp() - start;
		LogInfo("Test reading and writing files took " + std::to_string(duration) + " milliseconds.");
	}

	void SCrossForgeTestApp::testTriangleMeshIO() {
		std::string fileIn = "./Assets/ExampleScenes/Helmet/DamagedHelmet.gltf";
		std::string fileOut = "./Assets/Helmet.obj";

		LogInfo("Starting Triangle Mesh IO test");
		uint64_t start = GeneralUtility::getTimestamp();


		TriangleMeshEntityPtr pMeshEntity = std::make_shared<TriangleMeshEntity>(TriangleMeshEntity::ALL_BASIC_COMPONENTS);
		AssetIOProviderPtr pAssetIO = AssetIOProvider::instance();

		if (pAssetIO->loadMesh(pMeshEntity, fileIn)) {
			LogInfo("Successfully loaded mesh " + fileIn);
		}
		else {
			LogError("Failed to load mesh " + fileIn);
			return;
		}
		uint64_t durationLoad = GeneralUtility::getTimestamp() - start;
		LogInfo("Loading took " + std::to_string(durationLoad) + " milliseconds");

		if (pAssetIO->storeMesh(pMeshEntity, fileOut)) {
			LogInfo("Successfully stored mesh " + fileOut);
		}
		else {
			LogError("Failed to store mesh " + fileOut);
		}

		uint64_t duration = GeneralUtility::getTimestamp() - start;
		LogInfo("Finished Triangle Mesh IO test in " + std::to_string(duration) + " milliseconds.");
	}



	void SCrossForgeTestApp::update() {

		auto pWinSys = m_pSystemManager->getSystem<WindowSystem>();
		auto pRenderSys = m_pSystemManager->getSystem<RenderingSystem>();
		pWinSys->update();
		WindowEntityController::clearBuffer(m_pMainWin);
		pRenderSys->update();
		pWinSys->swapBuffers();


		if (GeneralUtility::getTimestamp() - m_lastPrint > 1000) {
			int32_t runtimeSeconds = (GeneralUtility::getTimestamp() - m_startTimestamp)/1000;
			//LogInfo("Application is running for " + std::to_string(runtimeSeconds) + " seconds now\n");
			if (nullptr == m_pMainWin->getWindowPropertiesComponent()->getGlfwWindowHandle()) {
				LogInfo("Main window was closed. Exiting application now!\n");
				this->stop();
			}
			uint64_t timestamp = GeneralUtility::getTimestamp()%100;
			m_pMainWin->getWindowPropertiesComponent()->clearColor() = Eigen::Vector4f(timestamp / 100.0f, timestamp / 50.0f, timestamp / 60.0f, 1.0f);
			m_lastPrint = GeneralUtility::getTimestamp();
		}

		if (m_pInputDevice->getKeyboardDataComponent()->isKeyRelease(KeyboardDataComponent::KEY_ESCAPE)) this->stop();

		MouseDataComponentPtr pMouseData = m_pInputDevice->getMouseDataComponent();
		std::string mousePos = "x: " + std::to_string(pMouseData->position().x()) + " | y:" + std::to_string(pMouseData->position().y());
		if (pMouseData->buttonState(MouseDataComponent::BUTTON_MIDDLE) == MouseDataComponent::STATE_RELEASED) {
			LogInfo("Middle mouse button released at position: " + mousePos);
			pMouseData->buttonState(MouseDataComponent::BUTTON_MIDDLE) = MouseDataComponent::STATE_OFF;
		}
		if (pMouseData->buttonState(MouseDataComponent::BUTTON_MIDDLE) == MouseDataComponent::STATE_PRESSED) {
			LogInfo("Middle mouse button pressed at position: " + mousePos);
			pMouseData->buttonState(MouseDataComponent::BUTTON_MIDDLE) = MouseDataComponent::STATE_OFF;
		}
	}

	

	SCrossForgeTestApp::~SCrossForgeTestApp() {

	}

	

	SCrossForgeTestApp::SCrossForgeTestApp() {
		m_startTimestamp = 0;
		m_lastPrint = 0;
	}
}