/*****************************************************************************\
*                                                                           *
* File(s): UBOPointLightsComponent.h and UBOPointLighsComponent.cpp                       *
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
#ifndef __CROSSFORGE_UBOPOINTLIGHTSCOMPONENT_H__
#define __CROSSFORGE_UBOPOINTLIGHTSCOMPONENT_H__

#include <crossforge/graphics/components/ubos/UboBaseComponent.h>

namespace crossforge {
	class UBOPointLightsComponent : public UboBaseComponent {
	public:
		static inline std::string identification = "UBOPointLightsComponent";

		UBOPointLightsComponent();
		~UBOPointLightsComponent();

		bool initialize(uint32_t lightCount);
		void clear();

		void setPosition(const Eigen::Vector3f position, uint32_t lightIndex);
		void setColor(const Eigen::Vector3f color, uint32_t lightIndex);
		void setIntensity(const float intensity, uint32_t lightIndex);
		void setDirection(const Eigen::Vector3f direction, uint32_t lightIndex);
		void setLightSpaceMatrix(const Eigen::Matrix4f lightSpaceMatrix, uint32_t lightIndex);
		void setShadowIndex(const int32_t shadowIndex, uint32_t lightIndex);

	protected:
		UBOPointLightsComponent(const std::string childIdentification);

		uint32_t m_lightCount;
		std::vector<uint32_t> m_positionOffsets;
		std::vector<uint32_t> m_colorOffsets;
		std::vector<uint32_t> m_intensityOffsets;
		std::vector<uint32_t> m_attenuationOffsets;
		std::vector<uint32_t> m_directionOffsets;
		std::vector<uint32_t> m_lightSpaceMatrixOffsets;
		std::vector<uint32_t> m_shadowIndexOffsets;
	};

	using UBOPointLightsComponentPtr = std::shared_ptr<UBOPointLightsComponent>;
	using UBOPointLightsComponentCPtr = std::shared_ptr<const UBOPointLightsComponent>;
}


#endif 