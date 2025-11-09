/*****************************************************************************\
*                                                                           *
* File(s): CameraEntityController.h and CameraEntityController.cpp                       *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_CAMERAENTITYCONTROLLER_H__
#define __CROSSFORGE_CAMERAENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/CameraEntity.h"
#include <crossforge/graphics/entities/CanvasEntity.h>

namespace crossforge {
	class CameraEntityController : public ControllerBase {
	public:
		static inline std::string identification = "CameraEntityController";

		static void computePerspectiveProjectionMatrix(CameraEntityPtr pCameraEntity, CanvasEntityPtr pCanvasEntity, float fieldOfView = 45.0f, float near = 0.1f, float far = 1000.0f);
		static void computeCameraMatrixFromTransformation(CameraEntityPtr pCameraEntity);
		static void computeCameraMatrixLookAt(CameraEntityPtr pCameraEntity, Eigen::Vector3f targetPosition);

		

		~CameraEntityController();
	protected:
		CameraEntityController();
		CameraEntityController(const std::string childIdentification);

	};

	typedef std::shared_ptr<CameraEntityController> CameraEntityControllerPtr;
}


#endif