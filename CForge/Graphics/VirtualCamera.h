/*****************************************************************************\
*                                                                           *
* File(s): VirtualCamera.h and VirtualCamera.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_VIRTUALCAMERA_H__
#define __CFORGE_VIRTUALCAMERA_H__

#include "../Core/CForgeObject.h"
#include "../Core/ITCaller.hpp"

namespace CForge {
	/**
	* \brief Message object send by the VirtualCamera class.
	*
	* \todo Full documentation
	*/
	struct CFORGE_API VirtualCameraMsg {
		enum MsgCode: int8_t {
			UNKNOWN = -1,
			POSITION_CHANGED,
			ROTATION_CHANGED,
			PROJECTION_CHANGED,

		};

		VirtualCameraMsg(class VirtualCamera *pCaller) {
			this->pCaller = pCaller;
		}

		~VirtualCameraMsg(void) {

		}

		MsgCode Code;
		class VirtualCamera* pCaller;
	};//VirtualCameraMsg

	/**
	* \brief Camera object for moving around in a 3D virtual world.
	*
	* \todo Full documentation
	*/
	class CFORGE_API VirtualCamera:public CForgeObject, public ITCaller<VirtualCameraMsg> {
	public:
		VirtualCamera(void);
		~VirtualCamera(void);

		void init(Eigen::Vector3f Position, Eigen::Vector3f Up);
		void clear(void);
		void resetToOrigin(void);
		void lookAt(const Eigen::Vector3f Position, const Eigen::Vector3f Target, const Eigen::Vector3f Up = Eigen::Vector3f::UnitY());

		Eigen::Matrix4f cameraMatrix(void)const;
		Eigen::Matrix4f projectionMatrix(void)const;
		void projectionMatrix(uint32_t ViewportWidth, uint32_t ViewportHeight, float FieldOfView, float Near, float Far);

		Eigen::Vector3f dir(void)const;
		Eigen::Vector3f up(void)const;
		Eigen::Vector3f right(void)const;
		Eigen::Vector3f position(void)const;
		

		void forward(float Speed);
		void right(float Speed);
		void up(float Speed);
		void position(Eigen::Vector3f Pos);

		void yaw(float Theta);
		void roll(float Theta);
		void pitch(float Theta);
		void rotX(float Theta);
		void rotY(float Theta);
		void rotZ(float Theta);

		float getFOV();

	protected:
		void notifyListeners(VirtualCameraMsg::MsgCode Code);

		Eigen::Vector3f m_Position;
		Eigen::Quaternionf m_Rotation;
		Eigen::Matrix4f m_Projection;
		float m_FOV;
	};//VirtualCamera

}//name space

#endif