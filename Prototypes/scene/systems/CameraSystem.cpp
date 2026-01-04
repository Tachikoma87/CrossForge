#include "CameraSystem.h"
#include "../entities/CameraEntity.h"
#include "../components/SphericalTransformationComponent.h"
#include "../controllers/SceneObjectEntityController.h"
#include "../controllers/CameraEntityController.h"



namespace crossforge {

	CameraSystem::CameraSystem(): SystemBase(CameraSystem::identification){

	}
	CameraSystem::CameraSystem(const std::string childIdentification): SystemBase(CameraSystem::identification) {
		m_inheritance.push_back(childIdentification);
	}
	CameraSystem::~CameraSystem() {
		m_entities.clear();
	}

	void CameraSystem::initialize() {
		
	}

	void CameraSystem::initialize(WindowEntityPtr pAssociatedWindow) {
		if (nullptr == pAssociatedWindow) throw NullpointerExcept("pAssociatedWindow");
		clear();

		try {
			m_pSystemManager = SystemManager::instance();

			if (!m_pSystemManager->hasSystem(KeyboardInputSystem::identification)) m_pSystemManager->addSystem(std::make_shared<KeyboardInputSystem>());
			if (!m_pSystemManager->hasSystem(MouseInputSystem::identification)) m_pSystemManager->addSystem(std::make_shared<MouseInputSystem>());

			auto pKeyboardInputSystem = m_pSystemManager->getSystem<KeyboardInputSystem>();
			auto pMouseInputSystem = m_pSystemManager->getSystem<MouseInputSystem>();


			m_pInputDevice = std::make_shared<InputDeviceEntity>();
			auto pWindowComp = m_pInputDevice->getAssociatedWindowComponent(true);
			pWindowComp->associatedWindow() = pAssociatedWindow;
			pKeyboardInputSystem->registerEntity(m_pInputDevice);
			pMouseInputSystem->registerEntity(m_pInputDevice);
		}
		catch (CrossForgeException& e) {
			Logger::logException(e);
		}

	}
	void CameraSystem::clear() {
		
	}
	void CameraSystem::update() {

		auto pMouseData = m_pInputDevice->getMouseStateComponent(true);
		auto pKeyboardData = m_pInputDevice->getKeyboardStateComponent(true);

		float moveDir = 0.0f;
		float moveRight = 0.0f;
		float moveUp = 0.0f;
		float rotYaw = 0.0f;
		float rotPitch = 0.0f;
		float rotRoll = 0.0f;

		if (pKeyboardData->isKeyPressed(KeyboardStateComponent::KEY_W)) {
			moveDir = 1.0f;
		}
		if (pKeyboardData->isKeyPressed(KeyboardStateComponent::KEY_S)) {
			moveDir = -1.0f;
		}
		if (pKeyboardData->isKeyPressed(KeyboardStateComponent::KEY_A)) {
			moveRight = -1.0f;
		}
		if (pKeyboardData->isKeyPressed(KeyboardStateComponent::KEY_D)) {
			moveRight = 1.0f;
		}

		if (pMouseData->buttonState(MouseStateComponent::BUTTON_LEFT) == MouseStateComponent::BUTTON_STATE_PRESSED) {
			rotYaw = pMouseData->positionDelta().x();
			rotPitch = pMouseData->positionDelta().y();
		}
		pMouseData->positionDelta() = Eigen::Vector2f::Zero();



		for (auto pEntity : m_entities) {
			auto pCamera = std::static_pointer_cast<CameraEntity>(pEntity);

			auto pCameraPropertiesComp = pCamera->getCameraPropertiesComponent(true);
			auto pTransform3DComp = pCamera->getTransformation3DComponent(true);
			auto pSphericalTransformationComp = pCamera->getComponent<SphericalTransformationComponent>(true);
			auto pTargetObjectComp = pCamera->getTargetObjectComponent(true);

			switch (pCameraPropertiesComp->cameraType()) {
			case CameraPropertiesComponent::ORBITAL: {
				pSphericalTransformationComp->rho() += -0.05f * moveDir;
				pSphericalTransformationComp->phi() += rotYaw / 300.0f;
				pSphericalTransformationComp->theta() -= rotPitch / 300.0f;

				if (nullptr != pTargetObjectComp && pTargetObjectComp->targetSceneObject() != nullptr && pTargetObjectComp->targetSceneObject()->hasComponent(Transformation3DComponent::identification)) {
					pSphericalTransformationComp->origin() = pTargetObjectComp->targetSceneObject()->getTransformation3DComponent()->globalPosition();
				}

				SceneObjectEntityController::lookAt(pCamera, pSphericalTransformationComp->getPosition(), pSphericalTransformationComp->origin());
			}break;
			default: {
				// first person camera
				SceneObjectEntityController::moveForward(pCamera, moveDir*0.25f);
				SceneObjectEntityController::moveRight(pCamera, moveRight * 0.25f);
				SceneObjectEntityController::rotate(pCamera, -rotYaw / 300.0f, Eigen::Vector3f::UnitY());
				SceneObjectEntityController::pitch(pCamera, -rotPitch / 300.0f);
			}break;
			}

			SceneObjectEntityController::buildGlobalTransformation(pCamera);
			CameraEntityController::computeCameraMatrixFromTransformation(pCamera);
			
		}

	}
	bool CameraSystem::isEntityValid(EntityBasePtr pEntity)const {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");

		bool result = true;
		if (!pEntity->isInstanceOf(CameraEntity::identification)) result = false;

		return result;
	}
}