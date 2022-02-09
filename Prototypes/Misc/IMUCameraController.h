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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_IMUCAMERACONTROLLER_H__
#define __CFORGE_IMUCAMERACONTROLLER_H__

#include <list>
#include "../Internet/IMUPackage.hpp"
#include "../../CForge/Graphics/VirtualCamera.h"
#include "../Internet/UDPSocket.h"

namespace CForge {
	class IMUCameraController {
	public:
		IMUCameraController(void);
		~IMUCameraController(void);

		void init(uint16_t PortLeft, uint16_t PortRight, uint32_t ForwardInterpolation, uint32_t RotationInterpolation);
		void clear(void);

		void update(VirtualCamera *pCamera, float Scale);
		void apply(VirtualCamera* pCamera, float Scale);

		void calibrate(void);

	protected:
		struct IMUData {
			uint64_t Timestamp;
			Eigen::Vector3f Rotations;
			Eigen::Vector3f Accelerations;

			void clear(void) {
				Timestamp = 0;
				Rotations = Eigen::Vector3f::Zero();
				Accelerations = Eigen::Vector3f::Zero();
			}
		};

		struct InterpolatedControllerData {
			IMUData AveragedMovementLeft;
			IMUData AveragedMovementRight;
			IMUData AveragedRotationLeft;
			IMUData AveragedRotationRight;

			float ForwardLeft;
			float ForwardRight;
			float RotationLeft;
			float RotationRight;
			float TiltLeft;
			float TiltRight;

			InterpolatedControllerData(void) {
				ForwardLeft = 0.0f;
				ForwardRight = 0.0f;
				RotationLeft = 0.0f;
				RotationRight = 0.0f;
				TiltLeft = 0.0f;
				TiltRight = 0.0f;
			}
		};

		void updateFoot(UDPSocket *pSock, std::list<IMUData>* pDataBuffer);
		void interpolateData(InterpolatedControllerData* pData);

		UDPSocket m_SocketLeft;
		UDPSocket m_SocketRight;
		std::list<IMUData> m_DataBufferLeft;
		std::list<IMUData> m_DataBufferRight;
		uint32_t m_ForwardInterpolation;
		uint32_t m_RotationInterpolation;

		float m_StepSpeed;

		uint64_t m_StepStarted;
		uint64_t m_StepEnded;

		InterpolatedControllerData m_CmdData;

		std::string m_LeftFootIP;
		std::string m_RightFootIP;
		uint16_t m_LeftFootPort;
		uint16_t m_RightFootPort;

		float m_HeadOffset;
		float m_CameraHeight;

	};//IMUCamera

}//name space


#endif 