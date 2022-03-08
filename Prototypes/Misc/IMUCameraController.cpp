#include "IMUCameraController.h"
#include "../../CForge/Core/CoreUtility.hpp"
#include "../../CForge/Graphics/GraphicsUtility.h"

using namespace Eigen;

namespace CForge {
	IMUCameraController::IMUCameraController(void) {
		m_HeadOffset = 0.0f;
		m_CameraHeight = -1.0f;

		m_UserState = STATE_STANDING;

		m_TurnLeft = false;
		m_TurnRight = false;
	}//Constructor

	IMUCameraController::~IMUCameraController(void) {
		clear();
	}//Destructor

	void IMUCameraController::init(uint16_t PortLeft, uint16_t PortRight, uint32_t AveragingTime) {
		clear();

		m_SocketLeft.begin(UDPSocket::TYPE_SERVER, PortLeft);
		m_SocketRight.begin(UDPSocket::TYPE_SERVER, PortRight);
		m_AveragingTime = AveragingTime;

		m_LeftFootPort = 0;
		m_RightFootPort = 0;

		m_CameraHeight = -1.0f;
		m_HeadOffset = 0.0f;

		m_UserState = STATE_STANDING;
		m_TurnRight = false;
		m_TurnLeft = false;
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

		//interpolateData(&m_CmdData);

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

				/*IMUData D;
				D.Timestamp = CoreUtility::timestamp();
				D.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
				D.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);

				pDataBuffer->push_back(D);*/

				if (m_LeftFootPort == 0 && pSock == &m_SocketLeft) {
					m_LeftFootPort = SenderPort;
					m_LeftFootIP = SenderIP;
				}
				if (m_RightFootPort == 0 && pSock == &m_SocketRight) {
					m_RightFootPort = SenderPort;
					m_RightFootIP = SenderIP;
				}

				if (pSock == &m_SocketRight && Package.Cmd == ArduForge::IMUPackage::CMD_AVG_DATA) {
					m_CmdData.AveragedMovementRight.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
					m_CmdData.AveragedMovementRight.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);
				}
				if (pSock == &m_SocketLeft && Package.Cmd == ArduForge::IMUPackage::CMD_AVG_DATA) {
					m_CmdData.AveragedMovementLeft.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
					m_CmdData.AveragedMovementLeft.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);
				}
			}
		}//while[receive data]

		// kill data that is out of scope
		while (pDataBuffer->size() > 0 && (CoreUtility::timestamp() - pDataBuffer->front().Timestamp) > m_AveragingTime) pDataBuffer->pop_front();
	}//updateFoot

	void IMUCameraController::interpolateData(InterpolatedControllerData* pData) {
		//// reset data
		//pData->ForwardLeft = 0.0f;
		//pData->ForwardRight = 0.0f;
		//
		//uint64_t Stamp = CoreUtility::timestamp();
		//m_CmdData.AveragedMovementLeft.clear();
		//m_CmdData.AveragedMovementRight.clear();
		//m_CmdData.AveragedAbsMovementLeft.clear();
		//m_CmdData.AveragedAbsMovementRight.clear();

		//// left foot
		//for (auto i : m_DataBufferLeft) {
		//	
		//		m_CmdData.AveragedMovementLeft.Accelerations += i.Accelerations;
		//		m_CmdData.AveragedMovementLeft.Rotations += i.Rotations;
		//		for (uint8_t k = 0; k < 3; ++k) {
		//			m_CmdData.AveragedAbsMovementLeft.Rotations[k] += std::abs(i.Rotations[k]);
		//			m_CmdData.AveragedAbsMovementLeft.Accelerations[k] += std::abs(i.Accelerations[k]);
		//		}
		//}
		//
		//if (m_DataBufferLeft.size() > 1.0f) {
		//	float Scale = float(m_DataBufferLeft.size());
		//	m_CmdData.AveragedMovementLeft.Accelerations /= Scale;
		//	m_CmdData.AveragedMovementLeft.Rotations /= Scale;
		//	m_CmdData.AveragedAbsMovementLeft.Accelerations /= Scale;
		//	m_CmdData.AveragedAbsMovementLeft.Rotations /= Scale;
		//}

		//
		//// right foot
		//for (auto i : m_DataBufferRight) {

		//	m_CmdData.AveragedMovementRight.Accelerations += i.Accelerations;
		//	m_CmdData.AveragedMovementRight.Rotations += i.Rotations;
		//	for (uint8_t k = 0; k < 3; ++k) {
		//		m_CmdData.AveragedAbsMovementRight.Accelerations[k] += std::abs(i.Accelerations[k]);
		//		m_CmdData.AveragedAbsMovementRight.Rotations[k] += std::abs(i.Rotations[k]);
		//	}
		//}
		//
		//if (m_DataBufferRight.size() > 1.0f) {
		//	float Scale = float(m_DataBufferRight.size());
		//	m_CmdData.AveragedMovementRight.Accelerations /= Scale;
		//	m_CmdData.AveragedMovementRight.Rotations /= Scale;
		//	m_CmdData.AveragedAbsMovementRight.Accelerations /= Scale;
		//	m_CmdData.AveragedAbsMovementRight.Rotations /= Scale;
		//}

	}//interpolateData

	void IMUCameraController::apply(VirtualCamera* pCamera, float Scale) {

		if (m_CameraHeight < 0.0f) m_CameraHeight = pCamera->position().y();

		const float StepSpeed = m_CmdData.AveragedMovementLeft.Rotations.y() + m_CmdData.AveragedMovementRight.Rotations.y();
		
		if (StepSpeed > 25.0f) {
			const float WalkSpeed = 5.0f;
			const float RunSpeed = 10.0f;

			float Alpha = std::clamp( (StepSpeed-20.0f) / 200.0f, 0.0f, 1.0f);
		
			float Speed = Alpha * RunSpeed + (1 - Alpha) * WalkSpeed;
			//printf("\tSpeed: %.2f | %.2f\n", Speed, m_CmdData.TiltLeft);

			if (Speed > 3.51f) {
				pCamera->forward(Speed * Scale / 50.0f);
				m_HeadOffset += Speed * Scale / 100.0f;
				Vector3f Pos = pCamera->position();
				Pos.y() = m_CameraHeight + 0.05f*std::sin(m_HeadOffset);
				pCamera->position(Pos);
			}
			m_UserState = STATE_WALKING;

			m_TurnLeft = false;
			m_TurnRight = false;
		}
		else {
			// state Standing
			m_UserState = STATE_STANDING;

			// rotate left or right?
			if (m_CmdData.AveragedMovementRight.Rotations.x() > 50.0f) {
				m_TurnRight = true;
			}
			else if (m_CmdData.AveragedMovementRight.Rotations.x() < -20.0f) {
				m_TurnRight = false;
			}
			if (m_CmdData.AveragedMovementLeft.Rotations.x() < -50.0f) {
				m_TurnLeft = true;
			}
			else if (m_CmdData.AveragedMovementLeft.Rotations.x() > 20.0f) {
				m_TurnLeft = false;
			}

			const float TurnSpeed = GraphicsUtility::degToRad(45.0f / 60.0f); // 20 Degree per second
			if (m_TurnLeft) pCamera->rotY(TurnSpeed*Scale);
			if (m_TurnRight) pCamera->rotY(-TurnSpeed*Scale);

			// sidestep left or right?
			if (m_CmdData.AveragedMovementRight.Accelerations.y() > 0.2f) pCamera->right(m_CmdData.AveragedMovementRight.Accelerations.y()/60.0f * 10.5f * Scale);
			if (m_CmdData.AveragedMovementLeft.Accelerations.y() < -0.2f) pCamera->right(m_CmdData.AveragedMovementLeft.Accelerations.y() / 60.0f * 10.5f * Scale);

		}
		
	}//apply


}