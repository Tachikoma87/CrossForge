#include "IMUCameraController.h"
#include "../../CForge/Core/CoreUtility.hpp"
#include "../../CForge/Graphics/GraphicsUtility.h"

using namespace Eigen;

namespace CForge {
	IMUCameraController::IMUCameraController(void) {

	}//Constructor

	IMUCameraController::~IMUCameraController(void) {
		clear();
	}//Destructor

	void IMUCameraController::init(uint16_t PortLeft, uint16_t PortRight, uint32_t ForwardInterpolation, uint32_t RotationInterpolation) {
		clear();

		m_SocketLeft.begin(UDPSocket::TYPE_SERVER, PortLeft);
		m_SocketRight.begin(UDPSocket::TYPE_SERVER, PortRight);
		m_ForwardInterpolation = ForwardInterpolation;
		m_RotationInterpolation = RotationInterpolation;

		m_StepStarted = 0;
		m_StepEnded = 0;
		m_StepSpeed = 0.0f;

		m_LeftFootPort = 0;
		m_RightFootPort = 0;

		m_CameraHeight = -1.0f;
		m_HeadOffset = 0.0f;
	}//initialize

	void IMUCameraController::clear(void) {
		m_SocketLeft.end();
		m_SocketRight.end();
		m_DataBufferLeft.clear();
		m_DataBufferRight.clear();
	}//clear

	void IMUCameraController::update(VirtualCamera* pCamera, float Scale) {

		updateFoot(&m_SocketLeft, &m_DataBufferLeft);
		updateFoot(&m_SocketRight, &m_DataBufferRight);

		interpolateData(&m_CmdData);

		if (nullptr != pCamera) apply(pCamera, Scale);

	}//update

	void IMUCameraController::calibrate(void) {

		ArduForge::IMUPackage Package;
		Package.Cmd = ArduForge::IMUPackage::CMD_CALIBRATE;

		uint8_t Buffer[64];
		uint32_t DataSize = 0;
		DataSize = Package.toStream(Buffer, sizeof(Buffer));

		if (m_LeftFootPort != 0) m_SocketLeft.sendData(Buffer, DataSize, m_LeftFootIP, m_LeftFootPort);
		if (m_RightFootPort != 0) m_SocketRight.sendData(Buffer, DataSize, m_RightFootIP, m_RightFootPort);

	}//calibrate

	void IMUCameraController::updateFoot(UDPSocket* pSock, std::list<IMUData>* pDataBuffer) {
		// fetch data
		uint8_t Buffer[64];
		uint32_t DataSize;

		std::string SenderIP;
		uint16_t SenderPort;

		while (pSock->recvData(Buffer, &DataSize, &SenderIP, &SenderPort)) {
			ArduForge::IMUPackage Package;
			if (Package.checkMagicTag(Buffer)) {
				Package.fromStream(Buffer, DataSize);

				IMUData D;
				D.Timestamp = CoreUtility::timestamp();
				D.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
				D.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);

				pDataBuffer->push_back(D);

				if (m_LeftFootPort == 0 && pSock == &m_SocketLeft) {
					m_LeftFootPort = SenderPort;
					m_LeftFootIP = SenderIP;
				}
				if (m_RightFootPort == 0 && pSock == &m_SocketRight) {
					m_RightFootPort = SenderPort;
					m_RightFootIP = SenderIP;
				}

				//static uint64_t Stamp = CoreUtility::timestamp();
				//static Vector3f Rots = Vector3f::Zero();
				//static Vector3f Accels = Vector3f::Zero();

				//if (CoreUtility::timestamp() - Stamp > 1000) {
				//	Stamp = CoreUtility::timestamp();

				//	printf("IMU: %.2f %.2f %.2f | %.2f %.2f %.2f\n", Rots.x(), Rots.y(), Rots.z(), Accels.x(), Accels.y(), Accels.z());
				//	Rots = Vector3f::Zero();
				//	Accels = Vector3f::Zero();
				//}
				//else {
				//	Accels = D.Accelerations;
				//	Rots.x() += std::abs(D.Rotations.x());
				//	Rots.y() += std::abs(D.Rotations.y());
				//	Rots.z() += std::abs(D.Rotations.z());

				//	/*Rots.x() += (D.Rotations.x());
				//	Rots.y() += (D.Rotations.y());
				//	Rots.z() += (D.Rotations.z());*/
				//}		

			}
		}//while[receive data]

		// kill data that is out of scope
		uint32_t T = std::max(m_ForwardInterpolation, m_RotationInterpolation);
		while (pDataBuffer->size() > 0 && (CoreUtility::timestamp() - pDataBuffer->front().Timestamp) > T) pDataBuffer->pop_front();
	}//updateFoot

	void IMUCameraController::interpolateData(InterpolatedControllerData* pData) {
		// reset data
		pData->ForwardLeft = 0.0f;
		pData->ForwardRight = 0.0f;
		pData->RotationLeft = 0.0f;
		pData->RotationRight = 0.0f;
		pData->TiltLeft = 0.0f;
		pData->TiltRight = 0.0f;

		uint64_t Stamp = CoreUtility::timestamp();
		float RotationScale = 0.0f;
		float ForwardScale = 0.0f;

		m_CmdData.AveragedMovementLeft.clear();
		m_CmdData.AveragedMovementRight.clear();
		m_CmdData.AveragedRotationLeft.clear();
		m_CmdData.AveragedRotationRight.clear();

		// left foot
		for (auto i : m_DataBufferLeft) {
			if (Stamp - i.Timestamp < m_ForwardInterpolation) {
				pData->ForwardLeft += std::abs(i.Rotations.y());
				ForwardScale += 1.0f;

			}
			if (Stamp - i.Timestamp < m_RotationInterpolation) {
				pData->RotationLeft += -i.Rotations.x();
				pData->TiltLeft += i.Accelerations.y();
				RotationScale += 1.0f;
			}

		}
		if (RotationScale > 1.0f) {
			pData->RotationLeft /= RotationScale;
			pData->TiltLeft /= RotationScale;
		}
		if (ForwardScale > 1.0f) pData->ForwardLeft /= ForwardScale;

		ForwardScale = 0.0f;
		RotationScale = 0.0f;
		// right foot
		for (auto i : m_DataBufferRight) {
			if (Stamp - i.Timestamp < m_ForwardInterpolation) {
				pData->ForwardRight += std::abs(i.Rotations.y());
				ForwardScale += 1.0f;
			}
			if (Stamp - i.Timestamp < m_RotationInterpolation) {
				pData->RotationRight += -i.Rotations.x();
				pData->TiltRight += i.Accelerations.y();
				RotationScale += 1.0f;
			}

		}
		if (RotationScale > 1.0f) {
			pData->RotationRight /= RotationScale;
			pData->TiltRight /= RotationScale;
		}
		if (ForwardScale > 1.0f) pData->ForwardRight /= ForwardScale;

	}//interpolateData

	void IMUCameraController::apply(VirtualCamera* pCamera, float Scale) {

		if (m_CameraHeight < 0.0f) m_CameraHeight = pCamera->position().y();

		m_StepSpeed = m_CmdData.ForwardLeft + m_CmdData.ForwardRight;

		//printf("StepSpeed: %.2f\n", m_StepSpeed);
		
		if (m_StepSpeed > 30.0f) {
			const float WalkSpeed = 3.5f;
			const float RunSpeed = 15.0f;

			float Alpha = std::clamp( (m_StepSpeed-30.0f) / 125.0f, 0.0f, 1.0f);
		
			float Speed = Alpha * RunSpeed + (1 - Alpha) * WalkSpeed;
			//printf("\tSpeed: %.2f | %.2f\n", Speed, m_CmdData.TiltLeft);

			if (Speed > 3.51f) {
				pCamera->forward(Speed / 50.0f);
				m_HeadOffset += Speed / 100.0f;
				Vector3f Pos = pCamera->position();
				Pos.y() = m_CameraHeight + 0.05f*std::sin(m_HeadOffset);
				pCamera->position(Pos);
			}
		}
		

		if (std::abs(m_CmdData.TiltLeft) > 0.2f) pCamera->rotY(GraphicsUtility::degToRad(-5.0f * m_CmdData.TiltLeft));
		if (std::abs(m_CmdData.TiltRight) > 0.2f) pCamera->rotY(GraphicsUtility::degToRad(-5.0f * m_CmdData.TiltRight));


	}//apply


}