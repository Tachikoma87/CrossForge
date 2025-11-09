#include "SceneObjectEntityController.h"

#include "../components/ChildObjectsComponent.h"
#include "../components/PrefabComponent.h"
#include "../../Graphics/components/uniformbuffer/UBOTransformationDataComponent.h"
#include <crossforge/math/CrossForgeMath.h>

using namespace Eigen;
namespace crossforge {

	SceneObjectEntityController::SceneObjectEntityController(const std::string childIdentification): ControllerBase(SceneObjectEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	SceneObjectEntityController::~SceneObjectEntityController() {

	}


	void SceneObjectEntityController::buildGlobalTransformation(SceneObjectEntityPtr pSceneObject, Transformation3DComponentPtr pParentTransform, uint32_t depthLevel) {
		if (nullptr == pSceneObject) return;
		if (depthLevel > 1000) throw CrossForgeExcept("Recursion depth level exceeded 1000. Seems like you produced a circular scene graph. Please fix that!");

		auto pObjTransformComp = pSceneObject->getComponent<Transformation3DComponent>();
		auto pChildObjectsComp = pSceneObject->getComponent<ChildObjectsComponent>();

		// apply transformation
		if (nullptr == pParentTransform) {
			pObjTransformComp->globalPosition() = pObjTransformComp->localPosition();
			pObjTransformComp->globalRotation() = pObjTransformComp->localRotation();
			pObjTransformComp->globalScale() = pObjTransformComp->localScale();
		}
		else {
			//pObjTransformComp->globalPosition() = (pParentTransform->globalRotation().toRotationMatrix() * pObjTransformComp->localPosition()) + pParentTransform->globalPosition();
			pObjTransformComp->globalPosition() = (pParentTransform->globalRotation() * pObjTransformComp->localPosition()) + pParentTransform->globalPosition();
			pObjTransformComp->globalRotation() = pParentTransform->globalRotation() * pObjTransformComp->localRotation();
			pObjTransformComp->globalScale() = pParentTransform->globalScale().cwiseProduct(pObjTransformComp->localScale());
		}

		// recursion
		if (nullptr != pChildObjectsComp) {
			for (auto pEntity : pChildObjectsComp->childSceneObjects()) {
				try {
					buildGlobalTransformation(pEntity, pObjTransformComp, depthLevel + 1);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
		}
	}

	void SceneObjectEntityController::gatherRenderableObjects(SceneObjectEntityPtr pSceneObject, std::vector<SceneObjectEntityPtr>& visibleObjects) {
		if (nullptr == pSceneObject) return;

		if (pSceneObject->hasComponent(PrefabComponent::identification)) visibleObjects.push_back(pSceneObject);
		auto pChildren = pSceneObject->getChildObjectsComponent();
		if (nullptr != pChildren) {
			for (auto pChild : pChildren->childSceneObjects()) gatherRenderableObjects(pChild, visibleObjects);
		}
	}


	void SceneObjectEntityController::updateTransformationUbo(SceneObjectEntityPtr pObj) {
		if (nullptr == pObj) throw NullpointerExcept("pActor");

		auto pTransformation3DComp = pObj->getTransformation3DComponent();
		auto pUboTransformation = pObj->getComponent<UBOTransformationDataComponent>();

		if (nullptr == pTransformation3DComp) throw MissingComponentException(Transformation3DComponent::identification);
		if (nullptr == pUboTransformation) throw MissingComponentException(UBOTransformationDataComponent::identification);

		const Eigen::Matrix4f rotMatrix = CrossForgeMath::buildRotationMatrix(pTransformation3DComp->globalRotation());
		const Eigen::Matrix4f posMatrix = CrossForgeMath::buildTranslationMatrix(pTransformation3DComp->globalPosition());
		const Eigen::Matrix4f scaleMatrix = CrossForgeMath::buildScaleMatrix(pTransformation3DComp->globalScale());
		Eigen::Matrix4f modelMatrix = posMatrix * rotMatrix * scaleMatrix;
		pUboTransformation->setModelMatrix(modelMatrix);
		pUboTransformation->setNormalMatrix(modelMatrix.inverse().transpose());

	}


	void SceneObjectEntityController::rotate(SceneObjectEntityPtr pSceneObjectEntity, const Eigen::Quaternionf rotation) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pCameraEntity");
		auto pTransform = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransform) throw MissingComponentException(Transformation3DComponent::identification);
		pTransform->localRotation() = rotation * pTransform->localRotation();
	}

	void SceneObjectEntityController::moveForward(SceneObjectEntityPtr pSceneObjectEntity, const float delta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f dir = -(pTransformComp->localRotation() * Eigen::Vector3f::UnitZ()).normalized();
		pTransformComp->localPosition() += delta * dir;
	}
	void SceneObjectEntityController::moveRight(SceneObjectEntityPtr pSceneObjectEntity, const float delta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f right = (pTransformComp->localRotation() * Eigen::Vector3f::UnitX()).normalized();
		pTransformComp->localPosition() += delta * right;
	}
	void SceneObjectEntityController::moveUp(SceneObjectEntityPtr pSceneObjectEntity, const float delta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f up = (pTransformComp->localRotation() * Eigen::Vector3f::UnitY()).normalized();
		pTransformComp->localPosition() += delta * up;
	}
	void SceneObjectEntityController::yaw(SceneObjectEntityPtr pSceneObjectEntity, const float theta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f up = (pTransformComp->localRotation() * Eigen::Vector3f::UnitY()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, up);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}
	void SceneObjectEntityController::roll(SceneObjectEntityPtr pSceneObjectEntity, const float theta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f dir = -(pTransformComp->localRotation() * Eigen::Vector3f::UnitZ()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, dir);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}
	void SceneObjectEntityController::pitch(SceneObjectEntityPtr pSceneObjectEntity, const float theta) {
		if (nullptr == pSceneObjectEntity) throw NullpointerExcept("pSceneObjectEntity");
		auto pTransformComp = pSceneObjectEntity->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f right = (pTransformComp->localRotation() * Eigen::Vector3f::UnitX()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, right);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}
}