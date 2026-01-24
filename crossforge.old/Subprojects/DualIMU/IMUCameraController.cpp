#include "IMUCameraController.h"
#include <crossforge/Math/CForgeMath.h>
#include <crossforge/Utility/CForgeUtility.h>

using namespace Eigen;

namespace CForge {
	IMUCameraController::IMUCameraController(void) {
		m_HeadOffset = 0.0f;
		m_CameraHeight = -1.0f;

		m_UserState = STATE_STANDING;

		m_TurnLeft = false;
		m_TurnRight = false;

		m_DataRecording = false;

		m_LastSearch = 0;
	}//Constructor

	IMUCameraController::~IMUCameraController(void) {
		clear();
	}//Destructor

	void IMUCameraController::init(uint16_t PortLeft, uint16_t PortRight, uint32_t AveragingTime) {
		clear();

		//m_SocketLeft.begin(UDPSocket::TYPE_SERVER, PortLeft);
		//m_SocketRight.begin(UDPSocket::TYPE_SERVER, PortRight);

		m_Socket.begin(PortLeft);
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
		//m_SocketLeft.end();
		//m_SocketRight.end();

		m_Socket.end();

		m_DataBufferLeft.clear();
		m_DataBufferRight.clear();
	}//clear

	void IMUCameraController::update(VirtualCamera* pCamera, float Scale) {

		//updateFoot(&m_SocketLeft, &m_DataBufferLeft);
		//updateFoot(&m_SocketRight, &m_DataBufferRight);

		updateFoot(&m_Socket, &m_DataBuffer);

		if (nullptr != pCamera) apply(pCamera, Scale);

		if (CForgeUtility::timestamp() - m_LastSearch > 500) {
			IMUWIP::IMUPackage SearchPackage;
			SearchPackage.Cmd = IMUWIP::IMUPackage::CMD_SEARCH;
			SearchPackage.setIP("192.168.1.206");

			uint8_t Buffer[64];
			if (m_LeftFootPort == 0) {
				SearchPackage.Port = m_Socket.port();
				uint32_t DataSize = SearchPackage.toStream(Buffer, sizeof(Buffer));
				m_Socket.sendData(Buffer, DataSize, "192.168.1.255", 10042);
				printf("Sending Search package (left)!\n");
			}
			if (m_RightFootPort == 0) {
				SearchPackage.Port = m_Socket.port();
				uint32_t DataSize = SearchPackage.toStream(Buffer, sizeof(Buffer));
				m_Socket.sendData(Buffer, DataSize, "192.168.1.255", 10042);
				printf("Sending Search package (right)!\n");
			}
			
			
			m_LastSearch = CForgeUtility::timestamp();
		}

	}//update

	void IMUCameraController::calibrate(void) {

		IMUWIP::IMUPackage Package;
		Package.Cmd = IMUWIP::IMUPackage::CMD_CALIBRATE;

		uint8_t Buffer[64];
		uint32_t DataSize = 0;
		DataSize = Package.toStream(Buffer, sizeof(Buffer));

		if (m_LeftFootPort != 0) m_Socket.sendData(Buffer, DataSize, m_LeftFootIP, m_LeftFootPort);
		if (m_RightFootPort != 0) m_Socket.sendData(Buffer, DataSize, m_RightFootIP, m_RightFootPort);

	}//calibrate

	void IMUCameraController::updateFoot(UDPSocket* pSock, std::list<IMUData>* pDataBuffer) {
		// fetch data
		uint8_t Buffer[64];
		uint32_t DataSize;

		std::string SenderIP;
		uint16_t SenderPort;

		while (pSock->recvData(Buffer, sizeof(Buffer), &DataSize, &SenderIP, &SenderPort)) {
			IMUWIP::IMUPackage Package;
			if (Package.checkMagicTag(Buffer)) {
				Package.fromStream(Buffer, DataSize);

				if (m_LeftFootPort == 0 /* && pSock == &m_SocketLeft*/ && Package.Type == IMUWIP::IMUPackage::DEVICE_TRACKER_LEFT) {
					m_LeftFootPort = SenderPort;
					m_LeftFootIP = SenderIP;
					printf("Found left tracker\n");
				}
				if (m_RightFootPort == 0 /* && pSock == &m_SocketRight*/ && Package.Type == IMUWIP::IMUPackage::DEVICE_TRACKER_RIGHT) {
					m_RightFootPort = SenderPort;
					m_RightFootIP = SenderIP;
					printf("Found right tacker\n");
				}

				//if (pSock == &m_SocketRight && Package.Cmd == IMUWIP::IMUPackage::CMD_AVG_DATA) {
				if (Package.Type == IMUWIP::IMUPackage::DEVICE_TRACKER_RIGHT && Package.Cmd == IMUWIP::IMUPackage::CMD_AVG_DATA) {
					m_CmdData.AveragedMovementRight.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
					m_CmdData.AveragedMovementRight.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);
				}
				//if (pSock == &m_SocketLeft && Package.Cmd == IMUWIP::IMUPackage::CMD_AVG_DATA) {
				if (Package.Type == IMUWIP::IMUPackage::DEVICE_TRACKER_LEFT && Package.Cmd == IMUWIP::IMUPackage::CMD_AVG_DATA) {
					m_CmdData.AveragedMovementLeft.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
					m_CmdData.AveragedMovementLeft.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);
				}
			}
		}//while[receive data]

		// kill data that is out of scope
		while (pDataBuffer->size() > 0 && (CForgeUtility::timestamp() - pDataBuffer->front().Timestamp) > m_AveragingTime) pDataBuffer->pop_front();
	}//updateFoot

	void IMUCameraController::apply(VirtualCamera* pCamera, float Scale) {

		if (m_CameraHeight < 0.0f) m_CameraHeight = pCamera->position().y();

		const float StepSpeed = m_CmdData.AveragedMovementLeft.Rotations.y() + m_CmdData.AveragedMovementRight.Rotations.y();

		float Speed = 0.0f;

		//const Vector2f WalkSpeed = Vector2f(3.0f, 5.0f);
		const float WalkSpeed = 2.5f;
		const float RunSpeed = 10.0f;

		static uint32_t RunCounter = 0;

		if (StepSpeed > 20.0f) {
			// walking

			m_TurnLeft = false;
			m_TurnRight = false;

			if (StepSpeed > 90.0f)	RunCounter++;
			else if (StepSpeed < 50.0f) RunCounter = 0;

			if (StepSpeed > 60.0f && RunCounter > 30) m_UserState = STATE_RUNNING;
			else m_UserState = STATE_WALKING;

			Speed = (m_UserState == STATE_WALKING) ? WalkSpeed : RunSpeed;

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

			const float TurnSpeed = CForgeMath::degToRad(45.0f / 60.0f); // 20 Degree per second
			if (m_TurnLeft) pCamera->rotY(TurnSpeed*Scale);
			if (m_TurnRight) pCamera->rotY(-TurnSpeed*Scale);

			// sidestep left or right?
			if (m_CmdData.AveragedMovementRight.Accelerations.y() > 0.2f) pCamera->right(m_CmdData.AveragedMovementRight.Accelerations.y()/60.0f * 10.5f * Scale);
			if (m_CmdData.AveragedMovementLeft.Accelerations.y() < -0.2f) pCamera->right(m_CmdData.AveragedMovementLeft.Accelerations.y() / 60.0f * 10.5f * Scale);
		}	

		if (Speed > 0.0f) {
			pCamera->forward(Speed * Scale / 50.0f);
			m_HeadOffset += Speed * Scale / 100.0f;
			Vector3f Pos = pCamera->position();
			Pos.y() = m_CameraHeight + 0.05f * std::sin(m_HeadOffset);
			pCamera->position(Pos);
		}
		

		if (m_DataRecording) {
			char Buffer[256];
			float Time = (CForgeUtility::timestamp() - m_RecordingStartTime) / 1000.0f;
			uint32_t MsgSize = sprintf(Buffer, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", Time, Speed,
				m_CmdData.AveragedMovementLeft.Accelerations.x(),
				m_CmdData.AveragedMovementRight.Accelerations.x(),
				m_CmdData.AveragedMovementLeft.Accelerations.y(),
				m_CmdData.AveragedMovementRight.Accelerations.y(),
				m_CmdData.AveragedMovementLeft.Accelerations.z(),
				m_CmdData.AveragedMovementRight.Accelerations.z(),
				m_CmdData.AveragedMovementLeft.Rotations.x(),
				m_CmdData.AveragedMovementRight.Rotations.x(),
				m_CmdData.AveragedMovementLeft.Rotations.y(),
				m_CmdData.AveragedMovementRight.Rotations.y(),
				m_CmdData.AveragedMovementLeft.Rotations.z(),
				m_CmdData.AveragedMovementRight.Rotations.z());

			m_DataFile.write(Buffer, MsgSize);
		}
	}//apply

	void IMUCameraController::recordData(std::string Filepath) {
		if (m_DataRecording) {
			m_DataFile.end();
			m_DataRecording = false;
		}
		else {
			m_DataFile.begin(Filepath, "wb");
			const char* Header = "Time,Speed,x-Pos-Left,x-Pos-Right,y-Pos-Left,y-Pos-Right,z-Pos-Left,z-Pos-Right,x-Rot-Left,x-Rot-Right,y-Rot-Left,y-Rot-Right,z-Rot-Left,z-Rot-Right\n";
			m_DataFile.write(Header, std::strlen(Header));
			m_DataRecording = true;
			m_RecordingStartTime = CForgeUtility::timestamp();
		}
	}//recordData

}