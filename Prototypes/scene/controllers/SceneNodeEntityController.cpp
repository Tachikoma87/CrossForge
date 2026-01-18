#include "SceneNodeEntityController.h"

#include <crossforge/math/CrossForgeMath.h>

#include <crossforge/graphics/components/ubos/UboTransformationDataComponent.h>
#include <crossforge/scene/components/ActorPrefabComponent.h>

#include <crossforge/scene/components/SceneNodesComponent.h>

using namespace Eigen;
namespace crossforge {

	SceneNodeEntityController::SceneNodeEntityController(const std::string childIdentification): ControllerBase(SceneNodeEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	SceneNodeEntityController::~SceneNodeEntityController() {

	}


	void SceneNodeEntityController::buildGlobalTransformation(SceneNodeEntityPtr pSceneNode, Transformation3DComponentPtr pParentTransform, uint32_t depthLevel) {
		if (nullptr == pSceneNode) return;
		if (depthLevel > 1000) throw CrossForgeExcept("Recursion depth level exceeded 1000. Seems like you produced a circular scene graph. Please fix that!");

		auto pObjTransformComp = pSceneNode->getComponent<Transformation3DComponent>();
		auto pChildObjectsComp = pSceneNode->getComponent<SceneNodesComponent>();

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
			for (auto pEntity : pChildObjectsComp->sceneNodes()) {
				try {
					buildGlobalTransformation(pEntity, pObjTransformComp, depthLevel + 1);
				}
				catch (const CrossForgeException& e) {
					Logger::logException(e);
				}
			}
		}
	}

	void SceneNodeEntityController::gatherRenderableObjects(SceneNodeEntityPtr pSceneNode, std::vector<SceneNodeEntityPtr>& visibleObjects) {
		if (nullptr == pSceneNode) return;

		if (pSceneNode->hasComponent(ActorPrefabComponent::identification)) visibleObjects.push_back(pSceneNode);
		auto pChildren = pSceneNode->getSceneNodesComponent();
		if (nullptr != pChildren) {
			for (auto pChild : pChildren->sceneNodes()) gatherRenderableObjects(pChild, visibleObjects);
		}
	}


	void SceneNodeEntityController::updateTransformationUbo(SceneNodeEntityPtr pSceneNode) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pActor");

		auto pTransformation3DComp = pSceneNode->getTransformation3DComponent();
		auto pUboTransformation = pSceneNode->getComponent<UboTransformationDataComponent>();

		if (nullptr == pTransformation3DComp) throw MissingComponentException(Transformation3DComponent::identification);
		if (nullptr == pUboTransformation) throw MissingComponentException(UboTransformationDataComponent::identification);

		const Eigen::Matrix4f rotMatrix = CrossForgeMath::buildRotationMatrix(pTransformation3DComp->globalRotation());
		const Eigen::Matrix4f posMatrix = CrossForgeMath::buildTranslationMatrix(pTransformation3DComp->globalPosition());
		const Eigen::Matrix4f scaleMatrix = CrossForgeMath::buildScaleMatrix(pTransformation3DComp->globalScale());
		Eigen::Matrix4f modelMatrix = posMatrix * rotMatrix * scaleMatrix;
		pUboTransformation->setModelMatrix(modelMatrix);
		pUboTransformation->setNormalMatrix(modelMatrix.inverse().transpose());

	}


	void SceneNodeEntityController::rotate(SceneNodeEntityPtr pSceneNode, const Eigen::Quaternionf rotation) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransform = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransform) throw MissingComponentException(Transformation3DComponent::identification);
		pTransform->localRotation() = rotation * pTransform->localRotation();
	}

	void SceneNodeEntityController::rotate(SceneNodeEntityPtr pSceneNode, const float theta, const Eigen::Vector3f axis) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);
		Quaternionf rot;
		rot = AngleAxisf(theta, axis);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}

	void SceneNodeEntityController::moveForward(SceneNodeEntityPtr pSceneNode, const float delta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f dir = -(pTransformComp->localRotation() * Eigen::Vector3f::UnitZ()).normalized();
		pTransformComp->localPosition() += delta * dir;
	}
	void SceneNodeEntityController::moveRight(SceneNodeEntityPtr pSceneNode, const float delta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f right = (pTransformComp->localRotation() * Eigen::Vector3f::UnitX()).normalized();
		pTransformComp->localPosition() += delta * right;
	}
	void SceneNodeEntityController::moveUp(SceneNodeEntityPtr pSceneNode, const float delta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f up = (pTransformComp->localRotation() * Eigen::Vector3f::UnitY()).normalized();
		pTransformComp->localPosition() += delta * up;
	}
	void SceneNodeEntityController::yaw(SceneNodeEntityPtr pSceneNode, const float theta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f up = (pTransformComp->localRotation() * Eigen::Vector3f::UnitY()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, up);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}
	void SceneNodeEntityController::roll(SceneNodeEntityPtr pSceneNode, const float theta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f dir = -(pTransformComp->localRotation() * Eigen::Vector3f::UnitZ()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, dir);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}
	void SceneNodeEntityController::pitch(SceneNodeEntityPtr pSceneNode, const float theta) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransformComp = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransformComp) throw MissingComponentException(Transformation3DComponent::identification);

		const Vector3f right = (pTransformComp->localRotation() * Eigen::Vector3f::UnitX()).normalized();
		Quaternionf rot;
		rot = AngleAxisf(theta, right);
		pTransformComp->localRotation() = rot * pTransformComp->localRotation();
	}

	void SceneNodeEntityController::lookAt(SceneNodeEntityPtr pSceneNode, Eigen::Vector3f origin, Eigen::Vector3f target, Eigen::Vector3f up) {
		if (nullptr == pSceneNode) throw NullpointerExcept("pSceneNode");
		auto pTransform = pSceneNode->getTransformation3DComponent();
		if (nullptr == pTransform) throw MissingComponentException(Transformation3DComponent::identification);

		Vector3f z = (target - origin).normalized();
		Vector3f x = (z.cross(up)).normalized();
		Vector3f y = x.cross(z);
		z = -z;

		Matrix3f rot;
		rot(0, 0) = x.x();
		rot(0, 1) = x.y();
		rot(0, 2) = x.z();
		rot(1, 0) = y.x();
		rot(1, 1) = y.y();
		rot(1, 2) = y.z();
		rot(2, 0) = z.x();
		rot(2, 1) = z.y();
		rot(2, 2) = z.z();
		rot.transposeInPlace();

		pTransform->localPosition() = origin;
		pTransform->localRotation() = rot;
	}
}