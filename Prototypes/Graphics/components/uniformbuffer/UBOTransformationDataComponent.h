/*****************************************************************************\
*                                                                           *
* File(s): UBOTransformationDataComponent and UBOTransformationDataComponent.cpp                       *
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
#ifndef __CROSSFORGE_UBOTRANSFORMATIONDATACOMPONENT_H__
#define __CROSSFORGE_UBOTRANSFORMATIONDATACOMPONENT_H__

#include "UniformBufferComponent.h"

namespace crossforge {
	class UBOTransformationDataComponent : public UniformBufferComponent {
	public:
		static inline std::string identification = "UBOTransformationDataComponent";

		UBOTransformationDataComponent();
		~UBOTransformationDataComponent();

		void setModelMatrix(const Eigen::Matrix4f modelMatrix);
		void setNormalMatrix(const Eigen::Matrix4f normalMatrix);

	protected:
		UBOTransformationDataComponent(const std::string childIdentification);

		void initialize();
	
		uint32_t m_modelMatrixOffset;
		uint32_t m_normalMatrixOffset;
		uint32_t m_bufferSize;
	};

	typedef std::shared_ptr<UBOTransformationDataComponent> UBOTransformationDataComponentPtr;
}

#endif