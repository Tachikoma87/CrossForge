#include "IMUCameraController.h"
#include "../../CForge/Core/CoreUtility.hpp"
#include "../../CForge/Graphics/GraphicsUtility.h"

using namespace Eigen;

namespace CForge {
	IMUCamera::IMUCamera(void) {

	}//Constructor

	IMUCamera::~IMUCamera(void) {
		clear();
	}//Destructor

	void IMUCamera::init(uint16_t PortLeft, uint16_t PortRight, uint32_t ForwardInterpolation, uint32_t RotationInterpolation) {
		clear();

		m_SocketLeft.begin(UDPSocket::TYPE_SERVER, PortLeft);
		m_SocketRight.begin(UDPSocket::TYPE_SERVER, PortRight);
		m_ForwardInterpolation = ForwardInterpolation;
		m_RotationInterpolation = RotationInterpolation;

		m_StepStarted = 0;
		m_StepEnded = 0;
		m_StepSpeed = 0.0f;

	}//initialize

	void IMUCamera::clear(void) {
		m_SocketLeft.end();
		m_SocketRight.end();
		m_DataBufferLeft.clear();
		m_DataBufferRight.clear();
	}//clear

	void IMUCamera::update(VirtualCamera* pCamera, float Scale) {

		updateFoot(&m_SocketLeft, &m_DataBufferLeft);
		updateFoot(&m_SocketRight, &m_DataBufferRight);

		interpolateData(&m_CmdData);

		if (nullptr != pCamera) apply(pCamera, Scale);

	}//update

	void IMUCamera::updateFoot(UDPSocket* pSock, std::list<IMUData>* pDataBuffer) {
		// fetch data
		uint8_t Buffer[64];
		uint32_t DataSize;
		while (pSock->recvData(Buffer, &DataSize, nullptr, nullptr)) {
			ArduForge::IMUPackage Package;
			if (Package.checkMagicTag(Buffer)) {
				Package.fromStream(Buffer, DataSize);

				IMUData D;
				D.Timestamp = CoreUtility::timestamp();
				D.Accelerations = Vector3f(Package.AccelX, Package.AccelY, Package.AccelZ);
				D.Rotations = Vector3f(Package.GyroX, Package.GyroY, Package.GyroZ);
				pDataBuffer->push_back(D);
			}
		}//while[receive data]

		// kill data that is out of scope
		uint32_t T = std::max(m_ForwardInterpolation, m_RotationInterpolation);
		while (pDataBuffer->size() > 0 && (CoreUtility::timestamp() - pDataBuffer->front().Timestamp) > T) pDataBuffer->pop_front();
	}//updateFoot

	void IMUCamera::interpolateData(InterpolatedControllerData* pData) {
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

	void IMUCamera::apply(VirtualCamera* pCamera, float Scale) {

		if (m_CmdData.ForwardLeft > 1.5f || m_CmdData.ForwardRight > 1.5f) {
			m_StepSpeed += m_CmdData.ForwardLeft + m_CmdData.ForwardRight;
		}
		
		m_StepSpeed /= 2.0f;
		
		if (m_StepSpeed > 15.0f) pCamera->forward(std::min(7.5f, m_StepSpeed) / 50.0f);

		printf("Step Speed: %.2f\n", m_StepSpeed);

		if (std::abs(m_CmdData.TiltLeft) > 0.25f) pCamera->rotY(GraphicsUtility::degToRad(-5.0f * m_CmdData.TiltLeft));
		if (std::abs(m_CmdData.TiltRight) > 0.25f) pCamera->rotY(GraphicsUtility::degToRad(-5.0f * m_CmdData.TiltRight));


	}//apply


}