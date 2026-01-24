/*****************************************************************************\
*                                                                           *
* File(s): exampleMinimumGraphicsSetup.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_IMUCAMERACONTROLLER_H__
#define __CFORGE_IMUCAMERACONTROLLER_H__

#include <list>
#include "IMUPackage.hpp"
#include <crossforge/Graphics/Camera/VirtualCamera.h>
#include <crossforge/Network/UDPSocket.h>
#include <crossforge/AssetIO/File.h>

namespace CForge {
	class IMUCameraController {
	public:
		enum UserState: int8_t {
			STATE_UNKNOWN = -1,
			STATE_WALKING,
			STATE_RUNNING,
			STATE_STANDING,
		};

		IMUCameraController(void);
		~IMUCameraController(void);

		void init(uint16_t PortLeft, uint16_t PortRight, uint32_t AveragingTime);
		void clear(void);

		void update(VirtualCamera *pCamera, float Scale);
		void apply(VirtualCamera* pCamera, float Scale);

		void calibrate(void);

		void recordData(std::string Filepath = std::string());

	protected:
		struct IMUData {
			uint64_t Timestamp;
			Eigen::Vector3f Rotations;
			Eigen::Vector3f Accelerations;

			IMUData(void) {
				Timestamp = 0;
				Rotations = Eigen::Vector3f::Zero();
				Accelerations = Eigen::Vector3f::Zero();
			}//Constructor

			void clear(void) {
				Timestamp = 0;
				Rotations = Eigen::Vector3f::Zero();
				Accelerations = Eigen::Vector3f::Zero();
			}
		};

		struct AveragedControllerData {
			IMUData AveragedMovementLeft;
			IMUData AveragedMovementRight;
			IMUData AveragedAbsMovementLeft;
			IMUData AveragedAbsMovementRight;

			float ForwardLeft;
			float ForwardRight;	

			AveragedControllerData(void) {
				ForwardLeft = 0.0f;
				ForwardRight = 0.0f;
			}//Constructor
		};

		void updateFoot(UDPSocket *pSock, std::list<IMUData>* pDataBuffer);

		UDPSocket m_Socket;
		std::list<IMUData> m_DataBuffer;

		//UDPSocket m_SocketLeft;
		//UDPSocket m_SocketRight;
		std::list<IMUData> m_DataBufferLeft;
		std::list<IMUData> m_DataBufferRight;

		uint32_t m_AveragingTime;

		AveragedControllerData m_CmdData;

		std::string m_LeftFootIP;
		std::string m_RightFootIP;
		uint16_t m_LeftFootPort;
		uint16_t m_RightFootPort;

		float m_HeadOffset;
		float m_CameraHeight;

		UserState m_UserState;

		bool m_TurnLeft;
		bool m_TurnRight;

		bool m_DataRecording;
		File m_DataFile;
		uint64_t m_RecordingStartTime;

		uint64_t m_LastSearch;

	};//IMUCamera

}//name space


#endif 