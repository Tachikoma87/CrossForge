/*****************************************************************************\
*                                                                           *
* File(s): CameraEntity.h and CameraEntity.cpp                        *
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
#ifndef __CROSSFORGE_CAMERAENTITY_H__
#define __CROSSFORGE_CAMERAENTITY_H__

#include "SceneObjectEntity.h"
#include "../../Graphics/components/uniformbuffer/UBOCameraDataComponent.h"
#include "../components/Transformation3DComponent.h"
#include "../components/CameraPropertiesComponent.h"
#include "../components/TargetObjectComponent.h"

namespace crossforge {
	class CameraEntity : public SceneObjectEntity {
	public:
		static inline std::string identification = "CameraEntity";

		enum CameraEntityComponents: uint8_t {
			COMPONENT_UBO_CAMERA_DATA	= 0x01,
			COMPONENT_TRANSFORMATION_3D = 0x02,
			COMPONENT_CAMERA_PROPERTIES = 0x04,
			COMPONENT_TARGET_OBJECT		= 0x08,
			COMPONENTS_ALL				= 0xFF,
		};

		CameraEntity(uint8_t componentsBitmask);
		~CameraEntity();

		void initialize(uint8_t componentsBitmask);
		void clear();

		UBOCameraDataComponentPtr getUboCameraDataComponent();
		CameraPropertiesComponentPtr getCameraPropertiesComponent();
		TargetObjectComponentPtr getTargetObjectComponentPtr();


	protected:
		CameraEntity(const std::string childIdentification);
	};

	typedef std::shared_ptr<CameraEntity> CameraEntityPtr;
}

#endif