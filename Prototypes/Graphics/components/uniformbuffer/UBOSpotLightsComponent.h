/*****************************************************************************\
*                                                                           *
* File(s): UBOSpotLightsComponent.h and UBOSpotLightsComponent.cpp                      *
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
#ifndef __CROSSFORGE_UBOSPOTLIGHTSCOMPONENT_H__
#define __CROSSFORGE_UBOSPOTLIGHTSCOMPONENT_H__

#include "UniformBufferComponent.h"

namespace crossforge {
	class UBOSpotLightsComponent : public UniformBufferComponent {
	public:
		static inline std::string identification = "UBOSpotLightsComponent";

		UBOSpotLightsComponent();
		~UBOSpotLightsComponent();

		bool initialize(uint32_t lightCount);
		void clear();

		void setPosition(const Eigen::Vector3f position, uint32_t lightIndex);
		void setDirection(const Eigen::Vector3f direction, uint32_t lightIndex);
		void setOuterCutoff(const float outerCutoff, uint32_t lightIndex);
		void setColor(const Eigen::Vector3f color, uint32_t lightIndex);
		void setIntensity(const float intensity, uint32_t lightIndex);
		void setAttenuation(const Eigen::Vector3f attenuation, uint32_t lightIndex);
		void setInnerCutoff(const float innerCutoff, uint32_t lightIndex);
		void setLightSpaceMatrix(const Eigen::Matrix4f lightSpaceMatrix, uint32_t lightIndex);
		void setShadowIndex(const int32_t shadowIndex, uint32_t lightIndex);

	protected:
		UBOSpotLightsComponent(const std::string childIdentification);

		uint32_t m_lightCount;
		std::vector<uint32_t> m_positionOffsets;
		std::vector<uint32_t> m_directionOffsets;
		std::vector<uint32_t> m_outerCutoffOffsets;
		std::vector<uint32_t> m_colorOffsets;
		std::vector<uint32_t> m_intensityOffsets;
		std::vector<uint32_t> m_attenuationOffsets;
		std::vector<uint32_t> m_innerCutoffOffsets;
		std::vector<uint32_t> m_lightSpaceMatrixOffsets;
		std::vector<uint32_t> m_shadowIdOffsets;
	};

	typedef std::shared_ptr<UBOSpotLightsComponent> UBOSpotLightsComponentPtr;
}

//vec4 Position[SpotLightCount];
//vec4 Direction[SpotLightCount]; // Direction.w is outer cutoff
//vec4 Color[SpotLightCount];	// Color.w is intensity 
//vec4 Attenuation[SpotLightCount]; // Attenuation.w is inner cutoff
//mat4 LightSpaceMatrices[SpotLightCount];
//ivec4 ShadowIDs[SpotLightCount];

#endif 