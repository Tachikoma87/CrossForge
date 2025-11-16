/*****************************************************************************\
*                                                                           *
* File(s): ShaderFeaturesComponent.h and ShaderFeaturesComponent.cpp        *
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
#ifndef __CROSSFORGE_SHADERPROPERTIESCOMPONENT_H__
#define __CROSSFORGE_SHADERPROPERTIESCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class ShaderPropertiesComponent : public ComponentBase {
	public:
		static inline std::string identification = "ShaderPropertiesComponent";

		enum ShaderFeature : uint16_t {
			SHADER_FEATURE_DIRECTIONAL_LIGHTS	= 0x0001,
			SHADER_FEATURE_POINT_LIGHTS			= 0x0002,
			SHADER_FEATURE_SPOT_LIGHTS			= 0x0004,
			SHADER_FEATURE_MULTIPLE_SHADOWS		= 0x0008,
			SHADER_FEATURE_NORMAL_MAPPING		= 0x0010,
			SHADER_FEATURE_PCF_SHADOWS			= 0x0020,
			SHADER_FEATURE_VERTEX_COLORS		= 0x0040,
			SHADER_FEATURE_SKELETAL_ANIMATION	= 0x0080,
			SHADER_FEATURE_MORPH_TARGET_ANIMATION = 0x0100,
		};

		ShaderPropertiesComponent();
		~ShaderPropertiesComponent();

		void initialize(std::shared_ptr<ShaderPropertiesComponent> pRef = nullptr);
		void clear() override;

		uint32_t& directionalLightsSize();
		uint32_t& pointLightsSize();
		uint32_t& spotLightsSize();
		uint32_t& activeDirectionalLightsCount();
		uint32_t& activePointLightsCount();
		uint32_t& activeSpotLightsCount();
		uint32_t& shadowMapCount();

		bool hasFeatures(uint16_t featureMask);
		void addFeatures(uint16_t featureMask);
		void removeFeatures(uint16_t featureMask);

		std::string getFeatureName(ShaderFeature feature)const;
		std::string& directionalLightsSizeConstDefinition();
		std::string& pointLightsSizeConstDefinition();
		std::string& spotLightsSizeConstDefinition();

		std::string& activeDirectionalLightsConstDefinition();
		std::string& activePointLightsConstDefinition();
		std::string& activeSpotLightsConstDefinition();

		uint16_t& featureMask();

	protected:
		ShaderPropertiesComponent(const std::string childIdentification);

		uint32_t m_directionalLightsSize;
		uint32_t m_pointLightsSize;
		uint32_t m_spotLightsSize;
		uint32_t m_shadowMapCount;

		uint32_t m_activeDirectionalLightsCount;
		uint32_t m_activePointLightsCount;
		uint32_t m_activeSpotLightsCount;

		std::string m_directionalLightsSizeConstDefinition;
		std::string m_pointLightsSizeConstDefinition;
		std::string m_spotLightsSizeConstDefinition;
		std::string m_activeDirectionalLightsConstDefinition;
		std::string m_activePointLightsConstDefinition;
		std::string m_activeSpotLightsConstDefinition;

		uint16_t m_featureMask;

		std::unordered_map<ShaderFeature, std::string> m_featureNamesMap;
	};

	typedef std::shared_ptr<ShaderPropertiesComponent> ShaderPropertiesComponentPtr;
}

#endif 
