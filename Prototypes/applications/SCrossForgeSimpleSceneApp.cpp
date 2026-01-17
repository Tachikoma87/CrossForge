#include <crossforge/graphics/OpenGLHeader.h>
#include "SCrossForgeSimpleSceneApp.h"

#include <crossforge/assetio/controller/TriangleMeshController.h>
#include <crossforge/graphics/controller/WindowController.h>
#include <crossforge/graphics/systems/WindowSystem.h>
#include <crossforge/input/systems/KeyboardInputSystem.h>
#include <crossforge/input/systems/MouseInputSystem.h>
#include <crossforge/assetio/controller/VideoController.h>
#include <crossforge/input/controller/InputDeviceController.h>

#include <crossforge/math/CrossForgeMath.h>

#include "../Graphics/controllers/RenderingController.h"
#include "../scene/controllers/CameraEntityController.h"
#include "../Graphics/controllers/ActorPrefabEntityController.h"
#include "../scene/components/ChildObjectsComponent.h"
#include "../scene/controllers/SceneObjectEntityController.h"
#include "../scene/components/PrefabComponent.h"
#include "../miscellaneous/MiscUtility.hpp"
#include "../utility/GraphicsUtility.h"
#include "../scene/components/SphericalTransformationComponent.h"
#include "../miscellaneous/controllers/MiscellaneousController.h"
#include "../Graphics/provider/STextureProvider.h"


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

		if (!WindowController::initOpenGLWindow(m_pMainWin)) {
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
		m_pSystemManager->addSystem(std::make_shared<MouseInputSystem>());
		m_pInputDevice = std::make_shared<InputDeviceEntity>();
		m_pEntityManager->registerEntity(m_pInputDevice);
		AssociatedWindowComponentPtr pWindowComponent = std::make_shared<AssociatedWindowComponent>();
		pWindowComponent->associatedWindow() = m_pMainWin;
		m_pInputDevice->addComponent(pWindowComponent);
		m_pSystemManager->getSystem<KeyboardInputSystem>()->registerEntity(m_pInputDevice);
		m_pSystemManager->getSystem<MouseInputSystem>()->registerEntity(m_pInputDevice);
		
		m_pAssetIO = AssetIOProvider::instance();

		// initialize duck prefab
		std::string modelFilepath = "./Assets/ExampleScenes/Duck/Duck.gltf";
		//modelFilepath = "./Assets/ExampleScenes/Sponza/Sponza.gltf";
		//modelFilepath = "./Assets/Antilope/scene.gltf";

		TriangleMeshEntityPtr pDuckTriangleMesh = std::make_shared<TriangleMeshEntity>();
		if (!m_pAssetIO->loadMesh(pDuckTriangleMesh, modelFilepath)) {
			LogError("Failed to load duck mesh!");
		}
		else {
			if (!TriangleMeshController::recomputeVertexNormals(pDuckTriangleMesh)) LogError("Recomputing vertex normals failed.");
			//if (!TriangleMeshController::recomputeVertexTangents(pDuckTriangleMesh)) LogError("Recomputing vertex tangents failed.");
			m_pDuckActorPrefab = std::make_shared<ActorPrefabEntity>();
			if (!ActorPrefabEntityController::buildStaticActor(m_pDuckActorPrefab, pDuckTriangleMesh)) {
				LogError("Failed to build duck static actor!");
			}
			else {
				m_pEntityManager->registerEntity(m_pDuckActorPrefab);
				LogInfo("Duck prefab actor build successfully.");
			}
		}
		// initialize helmet prefab
		modelFilepath = "./Assets/ExampleScenes/Helmet/DamagedHelmet.gltf";
		TriangleMeshEntityPtr pHelmetTriangleMesh = std::make_shared<TriangleMeshEntity>();
		if (!m_pAssetIO->loadMesh(pHelmetTriangleMesh, modelFilepath)) {
			LogError("Failed to load helmet mesh!");
		}
		else {
			if (!TriangleMeshController::recomputeVertexNormals(pHelmetTriangleMesh)) LogError("Recomputing vertex normals failed.");
			if (!TriangleMeshController::recomputeVertexTangents(pHelmetTriangleMesh)) LogError("Recomputing vertex tangents failed.");
			m_pHelmetActorPrefab = std::make_shared<ActorPrefabEntity>();
			if (!ActorPrefabEntityController::buildStaticActor(m_pHelmetActorPrefab, pHelmetTriangleMesh)) {
				LogError("Failed to build helmet static actor.");
			}
			else {
				m_pEntityManager->registerEntity(m_pHelmetActorPrefab);
			}
		}



		// initialize ground plane prefab
		auto pPlaneTriangleMesh = std::make_shared<TriangleMeshEntity>();
		//TriangleMeshController::plane(pPlaneTriangleMesh, Eigen::Vector2f(20.0f, 20.0f), Eigen::Vector2i(5, 5));
		/*TriangleMeshController::circle(pPlaneTriangleMesh, Eigen::Vector2f(20.0f, 20.0f), 20, 5.0f, false);*/
		//TriangleMeshController::cuboid(pPlaneTriangleMesh, Eigen::Vector3f(10.0f, 5.0f, 5.0f), Eigen::Vector3i(6, 6, 6));
		TriangleMeshController::uvSphere(pPlaneTriangleMesh, Eigen::Vector3f(10.0f, 10.0f, 10.0f), 10, 10);
		//TriangleMeshController::doubleCone(pPlaneTriangleMesh, Eigen::Vector4f(5.0f, 5.0f, 5.0f, 4.0f), 10);
		//TriangleMeshController::cylinder(pPlaneTriangleMesh, Eigen::Vector2f(8, 2), Eigen::Vector2f(16, 8), 15, 10, Eigen::Vector2f(-2.0f, -2.0f));
		//TriangleMeshController::torus(pPlaneTriangleMesh, 3.0f, 1.0f, 15, 15);
		if (!TriangleMeshController::recomputeVertexNormals(pPlaneTriangleMesh)) LogError("Recomputing vertex normals failed.");
		m_pGroundPlanePrefab = std::make_shared<ActorPrefabEntity>();
		m_pEntityManager->registerEntity(m_pGroundPlanePrefab);
		ActorPrefabEntityController::buildStaticActor(m_pGroundPlanePrefab, pPlaneTriangleMesh);
		//m_pGroundPlanePrefab->getPBRMaterialsComponent()->getMaterial(0)->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO) = TextureProvider::instance()->getTexture(0);

		auto pMaterialComp = m_pGroundPlanePrefab->getPBRMaterialsComponent();
		for (uint32_t i = 0; i < pMaterialComp->getMaterialCount(); ++i) {
			auto pMat = pMaterialComp->pbrMaterial(i);
			GraphicsUtility::createDefaultMaterial(pMat, GraphicsUtility::METAL_COPPER);
			pMat->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO) = TextureProvider::instance()->getTexture(TextureProvider::BASIC_TEXTURE_8X8_WHITE);
			pMat->updateUbo();
		}
		

		// initialize camera
		m_pCameraEntity = std::make_shared<CameraEntity>();
		m_pCameraEntity->getUboCameraDataComponent(true);
		m_pCameraEntity->getTransformation3DComponent(true);
		CameraEntityController::computePerspectiveProjectionMatrix(m_pCameraEntity, m_pMainCanvas);
		auto pCameraTransform = m_pCameraEntity->getTransformation3DComponent();
		pCameraTransform->localPosition() = Eigen::Vector3f(0.0f, 0.0f, 40.0f);
		//pCameraTransform->globalPosition() = Eigen::Vector3f(0.0f, 100.0f, 250.0f);
		pCameraTransform->globalPosition() = Eigen::Vector3f(0.0f, 0.0f, 25.0f);
	
		m_pCameraEntity->addComponent(std::make_shared<SphericalTransformationComponent>());
		m_pCameraEntity->getComponent<SphericalTransformationComponent>()->rho() = 5.0f;
		m_pCameraEntity->getComponent<SphericalTransformationComponent>()->theta() = EIGEN_PI / 2.0f;

		m_pCameraSystem = std::make_shared<CameraSystem>();
		m_pCameraSystem->initialize(m_pMainWin);

		m_pSystemManager->addSystem(m_pCameraSystem);
		m_pCameraSystem->registerEntity(m_pCameraEntity);

		

		// initialize duck actor instance
		m_pDuckActorInstance = std::make_shared<ActorInstanceEntity>();
		m_pDuckActorInstance->addComponent(std::make_shared<PrefabComponent>());
		m_pDuckActorInstance->getComponent<PrefabComponent>()->actorPrefab()= m_pDuckActorPrefab;
		//m_pDuckActorInstance->getMovement3DComponent()->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(-5.0f), Eigen::Vector3f::UnitZ());
		m_pDuckActorInstance->getTransformation3DComponent(true)->localScale() = Eigen::Vector3f(0.01f, 0.01f, 0.01f);
		m_pDuckActorInstance->addComponent(std::make_shared<UBOTransformationDataComponent>());
		m_pEntityManager->registerEntity(m_pDuckActorInstance);

		m_pDuckActorInstance2 = std::make_shared<ActorInstanceEntity>();
		m_pDuckActorInstance2->addComponent(std::make_shared<PrefabComponent>());
		m_pDuckActorInstance2->getComponent<PrefabComponent>()->actorPrefab() = m_pDuckActorPrefab;
		m_pDuckActorInstance2->getTransformation3DComponent(true)->localPosition() = Eigen::Vector3f(-5.0f, -2.0f, -5.0f);
		m_pDuckActorInstance2->getMovement3DComponent()->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(2.5f), Eigen::Vector3f::UnitY());
		m_pDuckActorInstance2->getTransformation3DComponent(true)->localScale() = Eigen::Vector3f(0.0025f, 0.0025f, 0.0025f);
		m_pDuckActorInstance2->addComponent(std::make_shared<UBOTransformationDataComponent>());
		m_pEntityManager->registerEntity(m_pDuckActorInstance2);

		
		m_pHelmetActorInstance = std::make_shared<ActorInstanceEntity>();
		m_pHelmetActorInstance->addComponent(std::make_shared<PrefabComponent>());
		m_pHelmetActorInstance->getComponent<PrefabComponent>()->actorPrefab() = m_pHelmetActorPrefab;
		m_pHelmetActorInstance->getTransformation3DComponent(true)->localPosition() = Eigen::Vector3f(2.0f, 4.0f, 1.0f);
		m_pHelmetActorInstance->getMovement3DComponent(true)->positionDelta() = Eigen::Vector3f(0.0f, 0.05f, 0.0f);
		m_pHelmetActorInstance->addComponent(std::make_shared<UBOTransformationDataComponent>());
		m_pEntityManager->registerEntity(m_pHelmetActorInstance);


		m_pGroundPlaneInstance = std::make_shared<ActorInstanceEntity>();
		m_pGroundPlaneInstance->addComponent(std::make_shared<PrefabComponent>());
		m_pGroundPlaneInstance->getComponent<PrefabComponent>()->actorPrefab() = m_pGroundPlanePrefab;
		m_pGroundPlaneInstance->getTransformation3DComponent(true)->localPosition() = Eigen::Vector3f(0.0f, 5.0f, 5.0f);
		m_pGroundPlaneInstance->getTransformation3DComponent(true)->localScale() = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
		m_pGroundPlaneInstance->getMovement3DComponent(true)->rotationDelta() = Eigen::AngleAxisf(CrossForgeMath::degToRad(0.5f), Eigen::Vector3f::UnitX());
		m_pGroundPlaneInstance->addComponent(std::make_shared<UBOTransformationDataComponent>());
		m_pEntityManager->registerEntity(m_pGroundPlaneInstance);

		m_pCameraEntity->getTargetObjectComponent(true)->targetSceneObject() = m_pHelmetActorInstance;

		// initialize scene lights
		m_pSceneLights = std::make_shared<LightsEntity>();
		auto pLightsConfig = m_pSceneLights->getLightsConfigComponent(true);

		auto pDirLights = m_pSceneLights->getUBODirectionalLightsComponent(true);
		pDirLights->initialize(5);
		for (uint8_t i = 0; i < 5; ++i) {
			pDirLights->setColor(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 15.0f), i);
			pDirLights->setShadowId(-1, i);
		}


		pDirLights->setColor(Eigen::Vector4f(1.0f, 1.0f, 1.0f, 70.0f), 1);
		pDirLights->setColor(Eigen::Vector4f(0.0f, 0.0f, 1.0f, 5.0f), 2);
		pLightsConfig->directionalLightsSize() = 5;
		pLightsConfig->activeDirectionalLights() = 1;



		Eigen::Vector3f dir = Eigen::Vector3f::Zero() - Eigen::Vector3f(100.0f, 100.0f, 250.0f);
		pDirLights->setDirection(dir.normalized(), 0);
		dir = Eigen::Vector3f(0, -1.0, 0.0f);
		pDirLights->setDirection(dir.normalized(), 1);
		dir = Eigen::Vector3f::Zero() - Eigen::Vector3f(0.0f, -10.0f, 10.0f);
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
		m_pMovementSystem->registerEntity(m_pGroundPlaneInstance);
		m_pMovementSystem->registerEntity(m_pRootNode);

		auto pTransformComp = m_pRootNode->getTransformation3DComponent();
		auto pChildObjectsComp = m_pRootNode->getChildObjectsComponent();

		//pTransformComp->localRotation() = Eigen::AngleAxisf(CrossForgeMath::degToRad(90.0f), Eigen::Vector3f::UnitY());
		pChildObjectsComp->addChild(m_pDuckActorInstance);
		pChildObjectsComp->addChild(m_pDuckActorInstance2);
		pChildObjectsComp->addChild(m_pHelmetActorInstance);

		pChildObjectsComp->addChild(m_pGroundPlaneInstance);

		//m_pEntityManager->registerEntity(m_pCameraEntity);
		//if (!m_pGroundPlaneInstance->hasComponent(ChildObjectsComponent::identification)) m_pGroundPlaneInstance->addComponent(std::make_shared<ChildObjectsComponent>());
		//m_pGroundPlaneInstance->getChildObjectsComponent()->addChild(m_pCameraEntity);

		for (auto pObj : m_pActorInstances) pChildObjectsComp->addChild(pObj);
	
		m_pSceneEntity = std::make_shared<SceneEntity>();

		m_frameCount = 0;
		m_timestampLastFpsPrint = 0;

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);


		/*std::string imageFile = "./Assets/archangel_1920.jpg";
		Image2DEntityPtr pImgInput = std::make_shared<Image2DEntity>();
		AssetIOProvider::instance()->loadImage2D(pImgInput, imageFile);


		Image2DController::changeColorSpace(pImgInput, RawImage2DDataComponent::COLORSPACE_GRAYSCALE);
		AssetIOProvider::instance()->storeImage2D(pImgInput, "./Assets/ImgOutputRGBA.jpg");

		Image2DController::changeColorSpace(pImgInput, RawImage2DDataComponent::COLORSPACE_RGB);
		AssetIOProvider::instance()->storeImage2D(pImgInput, "./Assets/ImgOutputRGB.webp");
		Image2DController::changeColorSpace(pImgInput, RawImage2DDataComponent::COLORSPACE_RGBA);
		AssetIOProvider::instance()->storeImage2D(pImgInput, "./Assets/ImgOutputRGBA.webp");*/

	}
	void SCrossForgeSimpleSceneApp::update() {
		uint64_t startHighPrecision = GeneralUtility::timestampHighPrecision();

		auto pWinSystem = m_pSystemManager->getSystem<WindowSystem>();
		auto pKeyboardSystem = m_pSystemManager->getSystem<KeyboardInputSystem>();
		pWinSystem->update();
		pKeyboardSystem->update();

		// clear window buffer and activate canvas
		WindowController::clearBuffer(m_pMainWin);
		RenderingController::activateCanvas(m_pMainCanvas, true);

		auto pWindowMsgComp = m_pMainWin->getMessageComponent(true);
		while (!pWindowMsgComp->messageQueue().empty()) {
			auto pMsg = pWindowMsgComp->dequeu();
			if (pMsg->messageIdentifier().compare("size_changed") == 0) {
				int32_t width = m_pMainWin->getWindowPropertiesComponent()->width();
				int32_t height = m_pMainWin->getWindowPropertiesComponent()->height();

				// resize canvas
				m_pMainCanvas->getCanvasSettingsComponent()->viewportSize() = Eigen::Vector2i(width, height);
				// recompute projection matrix of camera
				CameraEntityController::computePerspectiveProjectionMatrix(m_pCameraEntity, m_pMainCanvas);
			}
		}


		// set the camera
		m_pMovementSystem->update();
		m_pCameraSystem->update();

		//SceneObjectEntityController::buildGlobalTransformation(m_pCameraEntity);
		SceneObjectEntityController::buildGlobalTransformation(m_pRootNode);

		/*auto sphericalTransform = m_pCameraEntity->getComponent<SphericalTransformationComponent>();	
		SceneObjectEntityController::lookAt(m_pCameraEntity, sphericalTransform->getPosition(), sphericalTransform->origin());
		CameraEntityController::computeCameraMatrixFromTransformation(m_pCameraEntity);*/

		
		// gather renderable models and draw them
		std::vector<SceneObjectEntityPtr> renderables;
		SceneObjectEntityController::gatherRenderableObjects(m_pRootNode, renderables);

		for (auto pObj : renderables) {
			if (pObj->isInstanceOf(ActorInstanceEntity::identification)) {
				try {
					SceneObjectEntityController::updateTransformationUbo(pObj);
					ActorInstanceEntityPtr pActor = std::static_pointer_cast<ActorInstanceEntity>(pObj);
					RenderingController::drawActor(ShaderProvider::RENDER_PASS_FORWARD, pActor, pObj->getComponent<PrefabComponent>()->actorPrefab(), m_pCameraEntity, m_pSceneLights);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
		}


		uint64_t highPrecionsTime = GeneralUtility::timestampHighPrecision() - startHighPrecision;

		pWinSystem->swapBuffers();
		m_frameCount++;


		auto pos = m_pHelmetActorInstance->getTransformation3DComponent()->localPosition();
		if (pos.y() < -5.0f || pos.y() > 5.0f) m_pHelmetActorInstance->getMovement3DComponent()->positionDelta() *= -1.0f;

		auto pKeyboard = m_pInputDevice->getKeyboardStateComponent(true);
		auto pMouse = m_pInputDevice->getMouseStateComponent(true);
		auto pCameraTransform = m_pCameraEntity->getTransformation3DComponent();
		auto pSphericalComponent = m_pCameraEntity->getComponent<SphericalTransformationComponent>();
		//if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_W)) {
		//	//SceneObjectEntityController::moveForward(m_pCameraEntity, 0.25f);
		//	pSphericalComponent->rho() += -0.05f;
		//}
		//if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_S)) {
		//	//SceneObjectEntityController::moveForward(m_pCameraEntity, -0.25f);
		//	pSphericalComponent->rho() += 0.05f;
		//}
		//if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_A)) {
		//	//SceneObjectEntityController::moveRight(m_pCameraEntity, -0.25f);
		//	pSphericalComponent->phi() += 0.01f;
		//}
		//if (pKeyboard->isKeyPressed(KeyboardDataComponent::KEY_D)) {
		//	//SceneObjectEntityController::moveRight(m_pCameraEntity, 0.25f);
		//	pSphericalComponent->phi() -= 0.01f;
		//}

		if (pKeyboard->isKeyReleased(KeyboardStateComponent::KEY_1)) {

			/*InputDeviceEntityPtr pInputDevice = std::make_shared<InputDeviceEntity>();
			auto pUserDialogComp = std::make_shared<UserDialogComponent>();
			pUserDialogComp->dialogType() = UserDialogComponent::DIALOG_TYPE_MESSAGE_BOX;
			pUserDialogComp->messageBoxType() = UserDialogComponent::MESSAGE_BOX_TYPE_YES_NO_CANCEL;
			pUserDialogComp->title() = "My first Message-Box";
			pUserDialogComp->message() = "This is a Message box test message - Hello User!";
			pUserDialogComp->iconType() = UserDialogComponent::ICON_TYPE_INFO;
			pInputDevice->addComponent(pUserDialogComp);
			InputDeviceController::conductUserDialog(pInputDevice);

			std::string output = "";
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			switch (pUserInput->userAnswer()) {
			case UserInputComponent::USER_ANSWER_OK: output = "User clicked ok"; break;
			case UserInputComponent::USER_ANSWER_YES: output = "User clicked yes"; break;
			case UserInputComponent::USER_ANSWER_NO: output = "User clicked no"; break;
			case UserInputComponent::USER_ANSWER_CANCEL: output = "User clicked cancel"; break;
			default: {
				output = "No idea what the user has done ...";
			}break;
			}
			LogDebug(output);*/

			auto pCameraPropComp = m_pCameraEntity->getCameraPropertiesComponent();
			if (!pCameraPropComp->cameraType() == CameraPropertiesComponent::FIRST_PERSON) pCameraPropComp->cameraType() = CameraPropertiesComponent::FIRST_PERSON;
			else pCameraPropComp->cameraType() = CameraPropertiesComponent::ORBITAL;


			pKeyboard->keyState(KeyboardStateComponent::KEY_1) = KeyboardStateComponent::KEY_STATE_OFF;
		}
		if (pKeyboard->isKeyReleased(KeyboardStateComponent::KEY_2)) {
			InputDeviceEntityPtr pInputDevice = std::make_shared<InputDeviceEntity>();
			auto pUserDialogComp = std::make_shared<UserDialogComponent>();
			pUserDialogComp->dialogType() = UserDialogComponent::DIALOG_TYPE_INPUT_PASSWORD;
			pUserDialogComp->title() = "My first Input-Böx";
			pUserDialogComp->message() = "Hello User - Please input something. I can handl Umläute ä ü ö ß";
			pUserDialogComp->iconType() = UserDialogComponent::ICON_TYPE_INFO;
			pInputDevice->addComponent(pUserDialogComp);
			InputDeviceController::conductUserDialog(pInputDevice);
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			LogDebug("User input was: " + pUserInput->string());

			pKeyboard->keyState(KeyboardStateComponent::KEY_2) = KeyboardStateComponent::KEY_STATE_OFF;
		}

		if (pKeyboard->isKeyReleased(KeyboardStateComponent::KEY_3)) {

			static InputDeviceEntityPtr pInputDevice = std::make_shared<InputDeviceEntity>();
			if (!pInputDevice->hasComponent(UserDialogComponent::identification)) {
				auto pUserDialogComp = std::make_shared<UserDialogComponent>();
				pUserDialogComp->dialogType() = UserDialogComponent::DIALOG_TYPE_COLOR_PICKER;
				pUserDialogComp->title() = "Select a color";
				pUserDialogComp->message() = "Hello User - Please input something. I can handl Umläute ä ü ö ß";
				pUserDialogComp->iconType() = UserDialogComponent::ICON_TYPE_INFO;
				pUserDialogComp->allowMultiselect() = false;
				pUserDialogComp->fileFilterPatterns().push_back("*.md");
				pInputDevice->addComponent(pUserDialogComp);
			}
			
			InputDeviceController::conductUserDialog(pInputDevice);
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();

			std::string outputMsg = "User selected the following file(s): ";
			for (std::string file : pUserInput->fileList()) {
				outputMsg += file + " ";
			}

			auto color = pUserInput->rgbColor();
			outputMsg = "User selected color: " + std::to_string(color.x()) + " | " + std::to_string(color.y()) + " | " + std::to_string(color.z());

			LogDebug(outputMsg);

			pKeyboard->keyState(KeyboardStateComponent::KEY_3) = KeyboardStateComponent::KEY_STATE_OFF;
		}

		//if (pMouse->buttonState(MouseDataComponent::BUTTON_LEFT) == MouseDataComponent::STATE_PRESSED) {
		//	//SceneObjectEntityController::rotate(m_pCameraEntity, -pMouse->positionDelta().x()/300.0f, Eigen::Vector3f::UnitY());
		//	//SceneObjectEntityController::pitch(m_pCameraEntity, -pMouse->positionDelta().y() / 300.0f);

		//	pSphericalComponent->phi() += pMouse->positionDelta().x() / 300.0f;
		//	pSphericalComponent->theta() += pMouse->positionDelta().y() / 300.0f;

		//}
		//pMouse->positionDelta() = Eigen::Vector2f::Zero();

		m_pGroundPlaneInstance->getTransformation3DComponent()->localPosition() += Eigen::Vector3f(0.0f, 0.0f, -0.1f);

		if (pKeyboard->isKeyPressed(KeyboardStateComponent::KEY_9)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->metallic() <= 1.0f) pMat->metallic() += 0.01f;
				else pMat->metallic() = 1.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardStateComponent::KEY_8)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->metallic() >= 0.0f) pMat->metallic() -= 0.01f;
				else pMat->metallic() = 0.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardStateComponent::KEY_7)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->roughness() <= 1.0f) pMat->roughness() += 0.01f;
				else pMat->roughness() = 1.0f;
				pMat->updateUbo();
			}
		}
		if (pKeyboard->isKeyPressed(KeyboardStateComponent::KEY_6)) {
			auto pMaterialsComp = m_pDuckActorPrefab->getPBRMaterialsComponent();
			for (auto pMat : pMaterialsComp->pbrMaterials()) {
				if (pMat->roughness() >= 0.0f) pMat->roughness() -= 0.01f;
				else pMat->roughness() = 0.0f;
				pMat->updateUbo();
			}
		}

		if (pKeyboard->isKeyReleased(KeyboardStateComponent::KEY_V)) {

			auto pWindowPropsComp = m_pMainWin->getWindowPropertiesComponent();
			pWindowPropsComp->vSyncInterval() += 1;
			if (pWindowPropsComp->vSyncInterval() > 5) pWindowPropsComp->vSyncInterval() = 0;
			WindowController::updateVerticalSynchronization(m_pMainWin);


			pKeyboard->keyState(KeyboardStateComponent::KEY_V) = KeyboardStateComponent::KEY_STATE_OFF;
		}

		if (pKeyboard->isKeyPressed(KeyboardStateComponent::KEY_5)) {
			auto pLightsConfig = m_pSceneLights->getLightsConfigComponent();
			pLightsConfig->activeDirectionalLights() = (pLightsConfig->activeDirectionalLights() + 1)%4;
			pKeyboard->keyState(KeyboardStateComponent::KEY_5) = KeyboardStateComponent::KEY_STATE_OFF;
		}

		#ifdef CROSSFORGE_OPTION_FFMPEG
		if (nullptr != m_pVideoRecorder) {
			Image2DEntityPtr pFrame = std::make_shared<Image2DEntity>();
			if (!GraphicsUtility::retrieveFrameBuffer(pFrame)) LogError("Failed to retrieve frame buffer");
			//else if (!Image2DController::resize(pFrame, 1920, 1080)) LogError("Failed to resize frame buffer");
			else if(!VideoController::addFrame(m_pVideoRecorder, pFrame)) LogError("Failed to add video frame.");
		}
		
		if (pKeyboard->isKeyReleased(KeyboardStateComponent::KEY_R)) {

			if (nullptr == m_pVideoRecorder) {
				m_pVideoRecorder = std::make_shared<VideoEntity>();
				auto pVideoDataComp = m_pVideoRecorder->getVideoDataComponent(true);


				pVideoDataComp->framerate() = 29;
				pVideoDataComp->width() = 1280;
				pVideoDataComp->height() = 720;
				pVideoDataComp->filename() = "./Assets/ScreenRecording.mp4";
				if (VideoController::startRecording(m_pVideoRecorder)) {
					LogInfo("Video recording started ...");
				}
				else {
					LogError("Starting video recording failed.");
				}
			}
			else {
				if (VideoController::stopRecording(m_pVideoRecorder)) LogInfo("Stopped video recording.");
				else LogError("Stopping video recording failed.");
				m_pVideoRecorder = nullptr;
			}

			pKeyboard->keyState(KeyboardStateComponent::KEY_R) = KeyboardStateComponent::KEY_STATE_OFF;
		}
		#endif

		static uint64_t highPrecionsTiming = 0;
		highPrecionsTiming += highPrecionsTime;
		
		if (GeneralUtility::getTimestamp() - m_timestampLastFpsPrint > 1000) {
			int32_t runtimeSeconds = (GeneralUtility::getTimestamp() - m_timestampStart) / 1000;
			//LogInfo("Application is running for " + std::to_string(runtimeSeconds) + " seconds now\n");
			if (nullptr == m_pMainWin->getWindowPropertiesComponent()->getGlfwWindowHandle()) {
				LogInfo("Main window was closed. Exiting application now!\n");
				this->stop();
			}

			if (m_pInputDevice->getKeyboardStateComponent(true)->isKeyReleased(KeyboardStateComponent::KEY_ESCAPE)) {
				this->stop();
			}

			

			float fps = m_frameCount / (float)((GeneralUtility::getTimestamp() - m_timestampLastFpsPrint) / 1000.0f);
			m_pMainWin->getWindowPropertiesComponent()->title() = "CrossForge Simple Test App [" + std::to_string(int32_t(std::round(fps))) + " fps | " + std::to_string(std::round(highPrecionsTiming/fps)/1000)  + " ms/frame]";
			WindowController::updateWindowTitle(m_pMainWin);
			m_frameCount = 0;
			highPrecionsTiming = 0.0;

			uint64_t timestamp = GeneralUtility::getTimestamp() % 100;
			m_pMainWin->getWindowPropertiesComponent()->clearColor() = Eigen::Vector4f(timestamp / 100.0f, timestamp / 50.0f, timestamp / 60.0f, 1.0f);
			m_timestampLastFpsPrint = GeneralUtility::getTimestamp();

			
			//LogInfo("Update took " + std::to_string(highPrecionsTime) + " microseconds");
			//testGraphicsUtility();

		}
	}

	void SCrossForgeSimpleSceneApp::testGraphicsUtility() {

		// retrieve color texture
		Image2DEntityPtr pImageEntity = std::make_shared<Image2DEntity>();
		Image2DEntityPtr pDepthImage = std::make_shared<Image2DEntity>();

		uint32_t helmetTex = m_pHelmetActorPrefab->getPBRMaterialsComponent()->pbrMaterial(0)->texture(PbrMaterial::TEXTURE_TYPE_ALBEDO)->getTexture2DComponent()->glTextureHandle();
		//GraphicsUtility::retrieveColorTexture(helmetTex, pImageEntity, 0);
		//AssetIOProvider::instance()->storeImage2D(pImageEntity, "./assets/helemetTex.webp");

		uint32_t gpuFreeMemAvail = GraphicsUtility::gpuFreeMemory();
		uint32_t gpuMemAvail = GraphicsUtility::gpuMemoryAvailable();

		GraphicsUtility::GPUTraits traits = GraphicsUtility::retrieveGPUTraits();
		LogInfo("GPU memory: " + std::to_string(gpuFreeMemAvail / 1000) + " mb free of " + std::to_string(gpuMemAvail / 1000) + "mb total.");
		
		GraphicsUtility::retrieveFrameBuffer(pImageEntity, pDepthImage, 0.1f, 1000.0f);
		AssetIOProvider::instance()->storeImage2D(pImageEntity, "./Assets/ColorBuffer.jpg");
		AssetIOProvider::instance()->storeImage2D(pDepthImage, "./Assets/DepthBuffer.jpg");


	}
}