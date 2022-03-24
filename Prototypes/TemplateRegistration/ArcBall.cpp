#include "ArcBall.h"

#include "../../CForge/Graphics/GraphicsUtility.h"

namespace TempReg
{
	ArcBall::ArcBall() :
		m_DragStart(Vector3f::Zero()), m_DragEnd(Vector3f::Zero()), m_DragDir(Vector3f::Zero()), m_CursorLast(Vector2i::Zero()), m_RotationAxis(Vector3f::Zero()), m_Rotation(Quaternionf::Identity()) {
	
	}

	ArcBall::~ArcBall() {

	}

	void ArcBall::startRotation(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize) {
		m_DragStart = mapToSphere(CursorX, CursorY, VPSize);
	}

	Quaternionf ArcBall::updateRotation(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize) {
		m_DragEnd = mapToSphere(CursorX, CursorY, VPSize);

		m_DragDir = m_DragEnd - m_DragStart;

		m_RotationAxis = m_DragStart.cross(m_DragEnd);
		m_RotationAxis.normalize();
		
		m_Rotation = AngleAxisf(CForge::GraphicsUtility::degToRad(m_DragDir.norm() * 50.0f), m_RotationAxis);
		
		m_CursorLast = Vector2i(CursorX, CursorY);

		m_DragStart = m_DragEnd;

		return m_Rotation;
	}

	// TODO take viewport offset into account! => cursor position relative to viewport origin
	Vector3f ArcBall::mapToSphere(uint32_t CursorX, uint32_t CursorY, Vector2i VPSize) {
		
		float Radius = 1.0f;
	
		float MappedX = 2.0f * (float)CursorX / (float)VPSize.x() - 1.0f;
		float MappedY = 1.0f - 2.0f * (float)CursorY / (float)VPSize.y();
		float MappedZ;

		float LengthSquared = (MappedX * MappedX) + (MappedY * MappedY);

		if (LengthSquared <= Radius * Radius / 2.0f) {
			MappedZ = std::sqrtf((Radius * Radius) - LengthSquared);
		} 
		else {
			MappedZ = ((Radius * Radius) / 2.0f) / std::sqrtf(LengthSquared);
		}

		Vector3f MappedPos = Vector3f(MappedX, MappedY, MappedZ);

		return MappedPos;
	}
}