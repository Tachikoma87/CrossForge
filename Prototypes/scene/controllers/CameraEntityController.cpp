#include "CameraEntityController.h"

#include <crossforge/math/CrossForgeMath.h>

using namespace Eigen;

namespace crossforge {

	CameraEntityController::CameraEntityController(): ControllerBase(CameraEntityController::identification) {

	}
	CameraEntityController::CameraEntityController(const std::string childIdentification) : ControllerBase(CameraEntityController::identification){
		m_inheritance.push_back(childIdentification);
	}
	CameraEntityController::~CameraEntityController() {

	}


	void CameraEntityController::computePerspectiveProjectionMatrix(CameraEntityPtr pCameraEntity, CanvasEntityPtr pCanvasEntity, float fieldOfView, float near, float far){
		if (nullptr == pCameraEntity) throw NullpointerExcept("pCameraEntity");
		if (nullptr == pCanvasEntity) throw NullpointerExcept("pCanvasEntity");

		auto pCanvasSettings = pCanvasEntity->getCanvasSettingsComponent();
		if (nullptr == pCanvasSettings) throw MissingComponentException(CanvasSettingsComponent::identification);

		Eigen::Matrix4f perspectiveMatrix = CrossForgeMath::perspectiveProjection(pCanvasSettings->viewportSize().x(), pCanvasSettings->viewportSize().y(), fieldOfView, near, far);
		if (!pCameraEntity->hasComponent(CameraPropertiesComponent::identification)) pCameraEntity->addComponent(std::make_shared<CameraPropertiesComponent>());
		pCameraEntity->getCameraPropertiesComponent()->projectionMatrix() = perspectiveMatrix;

		auto pCameraUbo = pCameraEntity->getUboCameraDataComponent();
		if (nullptr != pCameraUbo) pCameraUbo->setProjectionMatrix(perspectiveMatrix);

	}

	void CameraEntityController::computeCameraMatrixFromTransformation(CameraEntityPtr pCameraEntity) {
		if (nullptr == pCameraEntity) throw NullpointerExcept("pCameraEntity");

		auto pTransform3DComp = pCameraEntity->getTransformation3DComponent();
		auto pCameraPropertiesComp = pCameraEntity->getCameraPropertiesComponent();
		if (nullptr == pTransform3DComp) throw MissingComponentException(Transformation3DComponent::identification);
		if (nullptr == pCameraPropertiesComp) throw MissingComponentException(CameraPropertiesComponent::identification);

		Matrix4f viewMatrix = Matrix4f::Identity();

		const Quaternion q = pTransform3DComp->globalRotation().normalized().conjugate();
		const Matrix3f R = q.toRotationMatrix();
		const Eigen::Vector3f T = -(q * pTransform3DComp->globalPosition());

		for (uint8_t i = 0; i < 3; ++i) {
			for (uint8_t k = 0; k < 3; ++k) {
				viewMatrix(i, k) = R(i, k);
			}
		}
		viewMatrix(0, 3) = T.x();
		viewMatrix(1, 3) = T.y();
		viewMatrix(2, 3) = T.z();

		pCameraPropertiesComp->viewMatrix() = viewMatrix;
		auto pUBO = pCameraEntity->getUboCameraDataComponent();
		if (nullptr != pUBO) pUBO->setCameraMatrix(viewMatrix);
		

	}
	void CameraEntityController::computeCameraMatrixLookAt(CameraEntityPtr pCameraEntity, Eigen::Vector3f targetPosition) {
		if (nullptr == pCameraEntity) throw NullpointerExcept("pCameraEntity");
		throw CrossForgeExcept("Not implemented yet, sorry.");
	}

	

}