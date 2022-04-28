#include "Arcball.h"

#include "../../CForge/Graphics/VirtualCamera.h"
#include "../../CForge/Graphics/GraphicsUtility.h"

namespace TempReg
{
	Arcball::Arcball(void) :
		m_Start(Vector3f::Zero()), m_End(Vector3f::Zero()), m_DragDir(Vector3f::Zero()),
		m_CursorLast(Vector2i::Zero()), m_RotationAxis(Vector3f::Zero()), m_CamRotation(Quaternionf::Identity()), m_ModelRotation(Quaternionf::Identity()) {

	}//Constructor

	Arcball::~Arcball() {

	}//Destructor

	Quaternionf Arcball::rotate(bool FirstClick, uint32_t CursorX, uint32_t CursorY, const CForge::RenderDevice::Viewport& VP, const CForge::VirtualCamera* pCam) {
		
		if (FirstClick) {
			m_Start = mapToSphereHyperbolic(CursorX, CursorY, VP.Position, VP.Size);

			Matrix3f RotMat;
			RotMat.block<3, 3>(0, 0) = pCam->cameraMatrix().block<3, 3>(0, 0);
			m_CamRotation = Quaternionf(RotMat.inverse());
			
			// rotate m_Start with VirtualCamera
			m_Start = m_CamRotation * m_Start;
		}
		
		m_End = mapToSphereHyperbolic(CursorX, CursorY, VP.Position, VP.Size);

		// rotate m_End with VirtualCamera
		m_End = m_CamRotation * m_End;

		m_DragDir = m_End - m_Start;

		m_RotationAxis = m_Start.cross(m_End);
		m_RotationAxis.normalize();
		
		m_ModelRotation = AngleAxisf(CForge::GraphicsUtility::degToRad(m_DragDir.norm() * 65.0f), m_RotationAxis);
		
		m_CursorLast = Vector2i(CursorX, CursorY);

		m_Start = m_End;

		return m_ModelRotation;
	}//rotate

	Vector3f Arcball::mapToSphereHyperbolic(uint32_t CursorX, uint32_t CursorY, Vector2i VPOffset, Vector2i VPSize) {
		
		float Radius = 1.0f;
	
		float MappedX = 2.0f * ((float)CursorX - (float)VPOffset.x()) / (float)VPSize.x() - 1.0f;
		float MappedY = 1.0f - 2.0f * ((float)CursorY - (float)VPOffset.y()) / (float)VPSize.y();
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
	}//mapToSphere
}