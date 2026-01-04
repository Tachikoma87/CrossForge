/*****************************************************************************\
*                                                                           *
* File(s): UBOCameraDataComponent and UBOCameraDataComponent.cpp                       *
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
#ifndef __CROSSFORGE_UBOCAMERADATACOMPONENT_H__
#define __CROSSFORGE_UBOCAMERADATACOMPONENT_H__

#include "UniformBufferComponent.h"

namespace crossforge {
	class UBOCameraDataComponent : public UniformBufferComponent {
	public:
		static inline std::string identification = "UBOCameraDataComponent";

		UBOCameraDataComponent();
		~UBOCameraDataComponent();

		void initialize();

		void setCameraMatrix(const Eigen::Matrix4f cameraMatrix);
		void setProjectionMatrix(const Eigen::Matrix4f projectionMatrix);
		void setCameraPosition(const Eigen::Vector3f position);

	protected:
		UBOCameraDataComponent(const std::string childIdentification);

		uint32_t m_cameraMatrixOffset;
		uint32_t m_projectionMatrixOffset;
		uint32_t m_cameraPositionOffset;
	};
	using UBOCameraDataComponentPtr = std::shared_ptr<UBOCameraDataComponent>;
	using UBOCameraDataComponentCPtr = std::shared_ptr<const UBOCameraDataComponent>;
}

#endif 