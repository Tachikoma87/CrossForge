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

#include <crossforge/scene/entities/SceneNodeEntity.h>
#include "../components/TargetSceneNodeComponent.h"
#include <crossforge/graphics/components/ubos/UboCameraDataComponent.h>
#include <crossforge/scene/components/CameraPropertiesComponent.h>

namespace crossforge {
	class CameraEntity : public SceneNodeEntity {
	public:
		static inline std::string identification = "CameraEntity";

		CameraEntity();
		~CameraEntity();

		void initialize();
		void clear();

		UboCameraDataComponentPtr getUboCameraDataComponent(const bool createIfNotExists = false);
		CameraPropertiesComponentPtr getCameraPropertiesComponent(const bool createIfNotExists = false);
		TargetSceneNodeComponentPtr getTargetObjectComponent(const bool createIfNotExists = false);

	protected:
		CameraEntity(const std::string childIdentification);
	};

	using CameraEntityPtr = std::shared_ptr<CameraEntity>;
	using CameraEntityCPtr = std::shared_ptr<const CameraEntity>;
}

#endif