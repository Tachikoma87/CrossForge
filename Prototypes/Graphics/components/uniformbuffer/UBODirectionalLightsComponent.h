/*****************************************************************************\
*                                                                           *
* File(s): UBODirectialLightsComponent.h and UBODirectionalLightsComponent.cpp                       *
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
#ifndef __CROSSFORGE_UBODIRECTIONALLIGHTSCOMPONENT_H__
#define __CORSSFORGE_UBODIRECTIONALLIGHTSCOMPONENT_H__

#include "UniformBufferComponent.h"

namespace crossforge {
	class UBODirectionalLightsComponent : public UniformBufferComponent {
	public:
		static inline std::string identification = "UBODirectionalLightsComponent";

		UBODirectionalLightsComponent();
		~UBODirectionalLightsComponent();

		bool initialize(const uint32_t lightCount);
		void clear() override;

		const uint32_t getLightCount()const;

		void setDirection(const Eigen::Vector3f direction, uint32_t lightIndex);
		void setColor(const Eigen::Vector4f color, uint32_t lightIndex);
		void setLightSpaceMatrix(const Eigen::Matrix4f lighSpaceMatrix, uint32_t lightIndex);
		void setShadowId(int32_t shadowId, uint32_t lightIndex);


	protected:
		UBODirectionalLightsComponent(const std::string childIdentification);

		uint32_t m_lightCount;
		std::vector<uint32_t> m_directionOffsets;
		std::vector<uint32_t> m_colorOffsets;
		std::vector<uint32_t> m_lightSpaceMatrixOffsets;
		std::vector<uint32_t> m_shadowIdOffsets;

	};
	
	using UBODirectionalLightsComponentPtr = std::shared_ptr<UBODirectionalLightsComponent>;
	using UBODirectionalLightsComponentCPtr = std::shared_ptr<const UBODirectionalLightsComponent>;
}


#endif