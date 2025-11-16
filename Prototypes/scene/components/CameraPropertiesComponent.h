/*****************************************************************************\
*                                                                           *
* File(s): CameraPropertiesComponent.h and CameraPropertiesComponent.cpp     *
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
#ifndef __CROSSFORGE_CAMERAPROPERTIESCOMPONENT_H__
#define __CROSSFORGE_CAMERAPROPERTIESCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class CameraPropertiesComponent : public ComponentBase {
	public:
		static inline std::string identification = "CameraPropertiesComponent";

		enum CameraType : int8_t {
			UNKNOWN = -1,
			FIRST_PERSON = 0,
			ORBITAL,
			COUNT
		};

		CameraPropertiesComponent();
		~CameraPropertiesComponent();

		void initialize();
		void clear()override;

		Eigen::Matrix4f& projectionMatrix();
		Eigen::Matrix4f& viewMatrix();
		CameraType& cameraType();

	protected:
		CameraPropertiesComponent(const std::string childIdentification);

		Eigen::Matrix4f m_projectionMatrix;
		Eigen::Matrix4f m_viewMatrix;
		CameraType m_cameraType;
	};
	typedef std::shared_ptr<CameraPropertiesComponent> CameraPropertiesComponentPtr;
}

#endif 