#include "GraphicsUtility.h"
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
	}//initialize

	void VirtualCamera::clear(void) {

	}//clear

	void VirtualCamera::resetToOrigin(void) {
		m_Rotation = Quaternionf(1, 0, 0, 0);
		m_Position = Vector3f(0, 0, 0);

		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
		
	}//resetToORigin

	Matrix4f VirtualCamera::cameraMatrix(void)const {
		Matrix4f Rval;
		Rval.setIdentity();

		const Quaternion q = m_Rotation.conjugate();
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
		return -(m_Rotation * Vector3f::UnitZ());
	}//direction

	Vector3f VirtualCamera::up(void)const {
		return m_Rotation * Vector3f::UnitY();
	}//up

	Vector3f VirtualCamera::right(void)const {
		return m_Rotation * Vector3f::UnitX();
	}//right

	Vector3f VirtualCamera::position(void)const {
		return m_Position;
	}//position

	void VirtualCamera::forward(float Speed) {
		m_Position += Speed * dir();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//forward

	void VirtualCamera::right(float Speed) {
		m_Position += Speed * right();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//right

	void VirtualCamera::up(float Speed) {
		m_Position += Speed * up();
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//up

	void VirtualCamera::position(Eigen::Vector3f Pos) {
		m_Position = Pos;
		notifyListeners(VirtualCameraMsg::POSITION_CHANGED);
	}//position



	void VirtualCamera::projectionMatrix(uint32_t ViewportWidth, uint32_t ViewportHeight, float FieldOfView, float NearPlane, float FarPlane) {
		m_Projection = GraphicsUtility::perspectiveProjection(ViewportWidth, ViewportHeight, FieldOfView, NearPlane, FarPlane);
		notifyListeners(VirtualCameraMsg::PROJECTION_CHANGED);
	}//projectionMatrix

	Matrix4f VirtualCamera::projectionMatrix(void)const {
		return m_Projection;
	}//projectionMatrix


	void VirtualCamera::yaw(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, up());
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//yaw

	void VirtualCamera::roll(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, dir());
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//roll

	void VirtualCamera::pitch(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, right());
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//pitch

	void VirtualCamera::rotX(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitX()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotX

	void VirtualCamera::rotY(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitY()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotY

	void VirtualCamera::rotZ(float Theta) {
		Quaternionf q;
		q = AngleAxisf(Theta, Eigen::Vector3f::UnitZ()).toRotationMatrix();
		m_Rotation = q * m_Rotation;
		notifyListeners(VirtualCameraMsg::ROTATION_CHANGED);
	}//rotZ

	void VirtualCamera::notifyListeners(VirtualCameraMsg::MsgCode Code) {
		VirtualCameraMsg Msg(this);
		Msg.Code = Code;
		broadcast(Msg);
	}//notifyListeners

}//name space
