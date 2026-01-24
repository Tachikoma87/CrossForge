#ifdef __WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include <crossforge/graphics/OpenGLHeader.h>
#include <crossforge/utility/GeneralUtility.hpp>
#include "SCrossForgeTestApp.h"

#include <crossforge/scene/controllers/RenderingController.h>
#include <crossforge/graphics/entities/CanvasEntity.h>
#include <crossforge/graphics/controllers/WindowEntityController.h>
#include <crossforge/assetio/SAssetIOProvider.h>
#include <crossforge/math/CrossForgeMath.h>
#include <crossforge/graphics/controllers/ActorPrefabEntityController.h>
#include <crossforge/graphics/entities/ActorPrefabEntity.h>
#include <crossforge/graphics/controllers/ShaderEntityController.h>
#include <crossforge/graphics/providers/SShaderProvider.h>

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
#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif
		}
		else {
			LogError("Something went wrong creating the main OpenGl window!");
		}

		// create rendering system
		m_startTimestamp = GeneralUtility::getTimestamp();
		m_lastPrint = GeneralUtility::getTimestamp();
		this->setSleepInterval(0);


		// bottom left
		CanvasEntityPtr pScene = std::make_shared<CanvasEntity>();
		CanvasSettingsComponentPtr pSceneSettings = std::make_shared<CanvasSettingsComponent>();

		pSceneSettings->viewportPosition() = Eigen::Vector2i(0, 0);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pSceneSettings->clearColor() = Eigen::Vector4f(0.5f, 0.5f, 0.0f, 1.0f);
		pScene->addComponent(pSceneSettings);
		m_pEntityManager->registerEntity(pScene);
		m_canvases.push_back(pScene);


		// top left
		pScene = std::make_shared<CanvasEntity>();
		pSceneSettings = std::make_shared<CanvasSettingsComponent>();

		pSceneSettings->viewportPosition() = Eigen::Vector2i(0, 720 / 2);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pSceneSettings->clearColor() = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		pScene->addComponent(pSceneSettings);
		m_pEntityManager->registerEntity(pScene);
		m_canvases.push_back(pScene);

		// top right
		pScene = std::make_shared<CanvasEntity>();
		pSceneSettings = std::make_shared<CanvasSettingsComponent>();

		pSceneSettings->viewportPosition() = Eigen::Vector2i(1280/2, 720/2);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		pSceneSettings->clearColor() = Eigen::Vector4f(0.0f, 0.0f, 1.0f, 1.0f);
		pScene->addComponent(pSceneSettings);
		m_pEntityManager->registerEntity(pScene);
		m_canvases.push_back(pScene);

		// bottom right
		pScene = std::make_shared<CanvasEntity>();
		pSceneSettings = std::make_shared<CanvasSettingsComponent>();
		pSceneSettings->viewportPosition() = Eigen::Vector2i(1280/2, 0);
		pSceneSettings->viewportSize() = Eigen::Vector2i(1280 / 2, 720 / 2);
		//pColor->color() = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
		pScene->addComponent(pSceneSettings);
		m_pEntityManager->registerEntity(pScene);
		m_canvases.push_back(pScene);

		// add input stuff
		m_pInputDevice = std::make_shared<InputDeviceEntity>();
		m_pEntityManager->registerEntity(m_pInputDevice);
		auto pKeyboardData = m_pInputDevice->getKeyboardStateComponent(true);
		auto pAssociatedWindow = m_pInputDevice->getAssociatedWindowComponent(true);
		pAssociatedWindow->associatedWindow() = m_pMainWin;

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
		
		//testFileIO();
		//testImageIO();
		//testTriangleMeshIO();
		//testRandom();
		//testShader();
		//testActorCreation();
		//testShaderGeneration();
		/*for (uint32_t i = 0; i < 10; ++i) {
			uint64_t timestampStart = GeneralUtility::getTimestamp();
			testShaderProvider();
			uint64_t time = GeneralUtility::getTimestamp() - timestampStart;
			LogInfo("Shader test run took " + std::to_string(time) + " milliseconds");
		}*/

	}

	

	void SCrossForgeTestApp::testShaderProvider() {
		ShaderPropertiesComponentPtr pShaderPropComp = std::make_shared<ShaderPropertiesComponent>();
		pShaderPropComp->addFeatures(ShaderPropertiesComponent::SHADER_FEATURE_DIRECTIONAL_LIGHTS);
		pShaderPropComp->directionalLightsSize() = 5;
		pShaderPropComp->activeDirectionalLightsCount() = 1;

		ShaderEntityPtr pShader = ShaderProvider::instance()->getShader(SShaderProvider::RENDER_PASS_FORWARD, pShaderPropComp);
		if (nullptr == pShader) LogError("Failed to get shader from shader provider!");
		else LogInfo("Seems like shader provider is working.");

		pShaderPropComp->pointLightsSize() = 5;
		pShaderPropComp->activePointLightsCount() = 3;
		pShaderPropComp->addFeatures(ShaderPropertiesComponent::SHADER_FEATURE_POINT_LIGHTS);
		pShader = ShaderProvider::instance()->getShader(SShaderProvider::RENDER_PASS_FORWARD, pShaderPropComp);
		if (nullptr == pShader) LogError("Failed to get shader from shader provider!");
		else LogInfo("Seems like shader provider is working.");


	}

	void SCrossForgeTestApp::testShaderGeneration() {

		ShaderEntityPtr pShader = std::make_shared<ShaderEntity>();
		pShader->addComponent(std::make_shared<ShaderPropertiesComponent>());
		auto pShaderSource = pShader->getShaderSourceComponent(true);
		auto pShaderProperties = pShader->getComponent<ShaderPropertiesComponent>(true);

		FileEntityPtr pFileEntity = std::make_shared<FileEntity>();
		AssetIOProvider::instance()->loadFile(pFileEntity, "./Assets/Shader/ForwardPassPBS.vert", false);
		pShaderSource->vertexShaderSources().push_back(pFileEntity->getStringComponent()->stringData());
		AssetIOProvider::instance()->loadFile(pFileEntity, "./Assets/Shader/ForwardPassPBS.frag", false);
		pShaderSource->fragmentShaderSources().push_back(pFileEntity->getStringComponent()->stringData());

		pShaderProperties->addFeatures(ShaderPropertiesComponent::SHADER_FEATURE_DIRECTIONAL_LIGHTS | ShaderPropertiesComponent::SHADER_FEATURE_NORMAL_MAPPING);

		try {
			ShaderEntityController::configureShaderSource(pShader);
		}
		catch (CrossForgeException& e) {
			LogError("Exception while configuring shader source: " + e.getMessage());
		}

		// store shader files
		pFileEntity->getStringComponent()->stringData() = pShader->getShaderSourceComponent()->vertexShaderSources()[0];
		AssetIOProvider::instance()->storeFile(pFileEntity, "./Assets/ConfiguredShader.vert", false);
		pFileEntity->getStringComponent()->stringData() = pShader->getShaderSourceComponent()->fragmentShaderSources()[0];
		AssetIOProvider::instance()->storeFile(pFileEntity, "./Assets/ConfiguredShader.frag", false);

	}


	void SCrossForgeTestApp::testImageIO() {
		LogInfo("Starting ImageIO test");
		uint64_t start = GeneralUtility::getTimestamp();

		std::string imageFileIn = "./Assets/Archangel_1920.png";
		std::vector<std::string> filesOut;
		filesOut.push_back("./Assets/Archangel.jpg");
		//filesOut.push_back("./Assets/Archangel.png");
		filesOut.push_back("./Assets/Archangel.webp");

		AssetIOProviderPtr pAssetIO = AssetIOProvider::instance();

		for (int32_t i = 0; i < 1; ++i) {
			Image2DEntityPtr pImageEntity = std::make_shared<Image2DEntity>();
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

		uint64_t runtime = GeneralUtility::getTimestamp() - start;
		LogInfo("Image test IO took " + std::to_string(runtime / 1000.0f) + " seconds.");

	}

	void SCrossForgeTestApp::testFileIO() {
		LogInfo("Starting to test AssetIO!");

		uint64_t start = GeneralUtility::getTimestamp();
		AssetIOProviderPtr pAssetIO = AssetIOProvider::instance();
		
		std::string shaderFileOut = "./Assets/Temp.txt";
		for (uint32_t i = 0; i < 1; ++i) {
			FileEntityPtr pEntity = std::make_shared<FileEntity>();
			std::string shaderFile = "./Assets/Shader/BasicGeometryPass.frag";
		
			if (!pAssetIO->loadFile(pEntity, shaderFile, false)) {
				LogError("Failed to load file " + shaderFile);
			}
			
			pAssetIO->storeFile(pEntity, shaderFileOut, false);

			//LogInfo("Testing of AssetIO finished!");

			pEntity->initialize();
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


		TriangleMeshEntityPtr pMeshEntity = std::make_shared<TriangleMeshEntity>();
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

	void SCrossForgeTestApp::testRandom() {

		std::unordered_map<uint64_t, uint32_t> rndCounter;
		uint64_t start = GeneralUtility::getTimestamp();

		uint64_t generationCount = 50000000;

		for (uint64_t i = 0; i < generationCount; ++i) {
			uint64_t rndNumber = CrossForgeMath::rand<uint16_t>();

			if (rndCounter.end() == rndCounter.find(rndNumber)) {
				rndCounter.insert(std::pair(rndNumber, 1));
			}
			else {
				rndCounter[rndNumber]++;
			}
		}

		uint64_t time = GeneralUtility::getTimestamp() - start;
		printf("Generating random numbers took: %d ms\n", (int32_t)time);

		int32_t expectedOccurrence = generationCount / std::numeric_limits<uint16_t>::max();

		int32_t overCounter = 0;
		int32_t underCounter = 0;
		for (auto x : rndCounter) {
			if (x.second > expectedOccurrence*1.1) {
				overCounter++;
			}
			else if (x.second < expectedOccurrence*0.9) {
				underCounter++;
			}
		}
		printf("Expected occurence: %d. Over counter is at %d and unter counter at %d\n",expectedOccurrence, overCounter, underCounter);
	}

	void SCrossForgeTestApp::testShader() {

		ShaderEntityPtr pShader = std::make_shared<ShaderEntity>();
		pShader->addComponent(std::make_shared<ShaderSourcesComponent>());
		auto pShaderSource = pShader->getComponent<ShaderSourcesComponent>();

		FileEntityPtr pShaderFile = std::make_shared<FileEntity>();
		if (AssetIOProvider::instance()->loadFile(pShaderFile, "./Assets/Shader/BasicGeometryPass.vert", false)) {
			pShaderSource->vertexShaderSources().push_back(pShaderFile->getStringComponent()->stringData());
		}
		if (AssetIOProvider::instance()->loadFile(pShaderFile, "./Assets/Shader/BasicGeometryPass.frag", false)) {
			pShaderSource->fragmentShaderSources().push_back(pShaderFile->getStringComponent()->stringData());
		}

		if (ShaderEntityController::buildRenderingShader(pShader)) {
			LogInfo("Shader build successfully!");
		}
		else {
			LogInfo("Shader building gone wrong");
		}



	}

	void SCrossForgeTestApp::testActorCreation() {

		// load model
		TriangleMeshEntityPtr pDuckMesh = std::make_shared<TriangleMeshEntity>();
		AssetIOProvider::instance()->loadMesh(pDuckMesh, "./Assets/ExampleScenes/Duck/Duck.gltf");

		ActorPrefabEntityPtr pDuckPrefabActor = std::make_shared<ActorPrefabEntity>();

		ActorPrefabEntityController::buildStaticActor(pDuckPrefabActor, pDuckMesh);

	}

	void SCrossForgeTestApp::update() {

		auto pWinSys = m_pSystemManager->getSystem<WindowSystem>();
		pWinSys->update();
		WindowEntityController::clearBuffer(m_pMainWin);

		for (auto pCanvas : m_canvases) RenderingController::activateCanvas(pCanvas);
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

		if (m_pInputDevice->getKeyboardStateComponent()->isKeyReleased(KeyboardStateComponent::KEY_ESCAPE)) this->stop();

		MouseStateComponentPtr pMouseData = m_pInputDevice->getMouseStateComponent();
		std::string mousePos = "x: " + std::to_string(pMouseData->position().x()) + " | y:" + std::to_string(pMouseData->position().y());
		if (pMouseData->buttonState(MouseStateComponent::BUTTON_MIDDLE) == MouseStateComponent::BUTTON_STATE_RELEASED) {
			LogInfo("Middle mouse button released at position: " + mousePos);
			pMouseData->buttonState(MouseStateComponent::BUTTON_MIDDLE) = MouseStateComponent::BUTTON_STATE_OFF;
		}
		if (pMouseData->buttonState(MouseStateComponent::BUTTON_MIDDLE) == MouseStateComponent::BUTTON_STATE_PRESSED) {
			LogInfo("Middle mouse button pressed at position: " + mousePos);
			pMouseData->buttonState(MouseStateComponent::BUTTON_MIDDLE) = MouseStateComponent::BUTTON_STATE_OFF;
		}
	}

	

	SCrossForgeTestApp::~SCrossForgeTestApp() {

	}

	

	SCrossForgeTestApp::SCrossForgeTestApp() {
		m_startTimestamp = 0;
		m_lastPrint = 0;
	}
}