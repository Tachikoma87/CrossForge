#pragma once

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {
	class ArcBall {
	public:

		ArcBall();

		~ArcBall();

		void startRotation(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize);

		Quaternionf updateRotation(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize);

	private:
		Vector3f m_DragStart, m_DragEnd, m_DragDir, m_RotationAxis;
		Vector2i m_CursorLast;
		Quaternionf m_Rotation;

		Vector3f mapToSphere(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize);
	};
}