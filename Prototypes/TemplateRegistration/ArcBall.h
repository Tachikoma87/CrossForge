#ifndef __TEMPREG_ARCBALL_H__
#define __TEMPREG_ARCBALL_H__

#include <Eigen/Eigen>

#include "../../CForge/Graphics/RenderDevice.h"

using namespace Eigen;

namespace TempReg {
	class Arcball {
	public:

		Arcball(void);
		~Arcball();

		Quaternionf rotate(bool FirstClick, uint32_t CursorX, uint32_t CursorY, const CForge::RenderDevice::Viewport& VP, const class CForge::VirtualCamera* pCam);

	private:
		Vector3f m_Start, m_End, m_DragDir, m_RotationAxis;
		Vector2i m_CursorLast;
		Quaternionf m_CamRotation;
		Quaternionf m_ModelRotation;

		Vector3f mapToSphereHyperbolic(uint32_t CursorX, uint32_t CursorY, Vector2i VPOffset, Vector2i VPSize);
	};
}

#endif