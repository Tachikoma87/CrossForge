#include "../Math/CForgeMath.h"
#include "../Core/SLogger.h"
#include "VirtualCamera.h"

using namespace Eigen;

namespace CForge {

	VirtualCamera::VirtualCamera(void): CForgeObject("VirtualCamera") {
		m_Rotation = Quaternionf::Identity();
		m_Position = Vector3f(0.0f, 0.0f, 0.0f);
	}//Constructor

	VirtualCamera::~VirtualCamera(void) {
		clear();
	}//Destructor

	void VirtualCamera::init(Eigen::Vector3f Position, Eigen::Vector3f Up) {
		clear();
		m_Position = Position;
		m_ViewFrustum.init(this);
	}//initialize

	void VirtualCamera::clear(void) {
		m_Rotation = Quaternionf::Identity();
		m_Position = Vector3f::Zero();
		m_ViewFrustum.clear();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//clear

	void VirtualCamera::resetToOrigin(void) {
		m_Rotation = Quaternionf(1, 0, 0, 0);
		m_Position = Vector3f(0, 0, 0);
		m_ViewFrustum.update();

		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);	
	}//resetToORigin

	void VirtualCamera::lookAt(const Eigen::Vector3f Position, const Eigen::Vector3f Target, const Eigen::Vector3f Up) {
		Vector3f Z = (Target - Position).normalized();
		Vector3f X = (Z.cross(Up)).normalized();
		Vector3f Y = X.cross(Z);
		Z = -Z;

		Matrix3f Rot;
		Rot(0, 0) = X.x();
		Rot(0, 1) = X.y();
		Rot(0, 2) = X.z();
		Rot(1, 0) = Y.x();
		Rot(1, 1) = Y.y();
		Rot(1, 2) = Y.z();
		Rot(2, 0) = Z.x();
		Rot(2, 1) = Z.y();
		Rot(2, 2) = Z.z();
		Rot.transposeInPlace();
		m_Rotation = Rot;
		m_Position = Position;
		m_ViewFrustum.update();

		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//lookAt

	Matrix4f VirtualCamera::cameraMatrix(void)const {
		Matrix4f Rval;
		Rval.setIdentity();

		const Quaternion q = m_Rotation.normalized().conjugate();
		const Matrix3f R = q.toRotationMatrix();
		const Eigen::Vector3f T = -(q * m_Position);
		
		for (uint8_t i = 0; i < 3; ++i) {
			for (uint8_t k = 0; k < 3; ++k) {
				Rval(i, k) = R(i, k);
			}
		}
		Rval(0, 3) = T.x();
		Rval(1, 3) = T.y();
		Rval(2, 3) = T.z();

		return Rval;
	}//cameraMatrix


	Vector3f VirtualCamera::dir(void)const {
		Vector3f Rval = -(m_Rotation * Vector3f::UnitZ());
		return Rval.normalized();
	}//direction

	Vector3f VirtualCamera::up(void)const {
		Vector3f Rval = m_Rotation * Vector3f::UnitY();
		return Rval.normalized();
	}//up

	Vector3f VirtualCamera::right(void)const {
		Vector3f Rval = m_Rotation * Vector3f::UnitX();
		return Rval.normalized();
	}//right

	Vector3f VirtualCamera::position(void)const {
		return m_Position;
	}//position

	void VirtualCamera::forward(float Speed) {
		m_Position += Speed * dir();
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//forward

	void VirtualCamera::right(float Speed) {
		m_Position += Speed * right();
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//right

	void VirtualCamera::up(float Speed) {
		m_Position += Speed * up();
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//up

	void VirtualCamera::position(Eigen::Vector3f Pos) {
		m_Position = Pos;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//position

	void VirtualCamera::projectionMatrix(uint32_t ViewportWidth, uint32_t ViewportHeight, float FieldOfView, float NearPlane, float FarPlane) {
		m_Projection = CForgeMath::perspectiveProjection(ViewportWidth, ViewportHeight, FieldOfView, NearPlane, FarPlane);
		m_FOV = FieldOfView;
		m_ViewportWidth = ViewportWidth;
		m_ViewportHeight = ViewportHeight;
		m_NearPlane = NearPlane;
		m_FarPlane = FarPlane;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::PROJECTION_CHANGED);
	}//projectionMatrix

	void VirtualCamera::orthographicProjection(float Left, float Right, float Bottom, float Top, float Near, float Far) {
		m_Projection = CForgeMath::orthographicProjection(Left, Right, Bottom, Top, Near, Far);
		m_FOV = 0.0f;
		m_ViewportWidth = Right - Left;
		m_ViewportHeight = Top - Bottom;
		m_NearPlane = Near;
		m_FarPlane = Far;

		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::PROJECTION_CHANGED);
	}//orthographicProjection

	Matrix4f VirtualCamera::projectionMatrix(void)const {
		return m_Projection;
	}//projectionMatrix


	void VirtualCamera::yaw(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, up());
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//yaw

	void VirtualCamera::roll(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, dir());
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//roll

	void VirtualCamera::pitch(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, right());
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//pitch

	void VirtualCamera::rotX(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitX()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotX

	void VirtualCamera::rotY(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitY()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotY

	void VirtualCamera::rotZ(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitZ()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		m_ViewFrustum.update();
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotZ

	void VirtualCamera::notifyListeners(VirtualCameraMsg::MsgCode Code) {
		VirtualCameraMsg Msg(this);
		Msg.Code = Code;
		broadcast(Msg);
	}//notifyListeners

	float VirtualCamera::getFOV() {
		return m_FOV;
	}

	uint32_t VirtualCamera::viewportWidth(void)const {
		return m_ViewportWidth;
	}//viewportWidth

	uint32_t VirtualCamera::viewportHeight(void)const {
		return m_ViewportHeight;
	}//viewportHeight

	float VirtualCamera::fieldOfView(void)const {
		return m_FOV;
	}//fieldOfView

	float VirtualCamera::nearPlane(void)const {
		return m_NearPlane;
	}//nearPlane

	float VirtualCamera::farPlane(void)const {
		return m_FarPlane;
	}//farPlane

	const ViewFrustum* VirtualCamera::viewFrustum(void)const {
		return &m_ViewFrustum;
	}//viewFrustum

}//name space
