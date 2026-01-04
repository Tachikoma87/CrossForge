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

		void initialize(const std::shared_ptr<const ShaderPropertiesComponent> pRef = nullptr);
		void clear() override;

		bool hasFeatures(uint16_t featureMask);
		void addFeatures(uint16_t featureMask);
		void removeFeatures(uint16_t featureMask);


		/** Accessor **/
		uint32_t& directionalLightsSize();
		uint32_t& pointLightsSize();
		uint32_t& spotLightsSize();
		uint32_t& activeDirectionalLightsCount();
		uint32_t& activePointLightsCount();
		uint32_t& activeSpotLightsCount();
		uint32_t& shadowMapCount();

		std::string& featureName(ShaderFeature feature);
		std::string& directionalLightsSizeConstDefinition();
		std::string& pointLightsSizeConstDefinition();
		std::string& spotLightsSizeConstDefinition();
		std::string& activeDirectionalLightsConstDefinition();
		std::string& activePointLightsConstDefinition();
		std::string& activeSpotLightsConstDefinition();

		uint16_t& featureMask();

		/** Getter **/
		const uint32_t getDirectionalLightsSize()const;
		const uint32_t getPointLightsSize()const;
		const uint32_t getSpotLightsSize()const;
		const uint32_t getActiveDirectionalLightsCount()const;
		const uint32_t getActivePointLightsCount()const;
		const uint32_t getActiveSpotLightsCount()const;
		const uint32_t getShadowMapCount()const;

		const std::string getFeatureName(const ShaderFeature feature)const;
		const std::string getDirectionalLightsSizeConstDefinition()const;
		const std::string getPointLightsSizeConstDefinition()const;
		const std::string getSpotLightsSizeConstDefinition()const;
		const std::string getActiveDirectionalLightsConstDefinition()const;
		const std::string getActivePointLightsConstDefinition()const;
		const std::string getActiveSpotLightsConstDefinition()const;

		const uint16_t getFeatureMask()const;

		/** Setter **/
		void setDirectionalLightsSize(const uint32_t size);
		void setPointLightsSize(const uint32_t size);
		void setSpotLightsSize(const uint32_t size);
		void setActiveDirectionalLightsCount(const uint32_t count);
		void setActivePointLightsCount(const uint32_t count);
		void setActiveSpotLightsCount(const uint32_t count);
		void setShadowMapCount(const uint32_t count);

		void setFeatureName(const std::string name, const ShaderFeature feature);
		void setDirectionalLightsSizeConstDefinition(const std::string definition);
		void setPointLightsSizeConstDefinition(const std::string definition);
		void setSpotLightsSizeConstDefinition(const std::string definition);
		void setActiveDirectionalLightsConstDefinition(const std::string definition);
		void setActivePointLightsConstDefinition(const std::string definition);
		void setActiveSpotLightsConstDefinition(const std::string definition);

		void setFeatureMask(uint16_t featureMask);

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

	using ShaderPropertiesComponentPtr = std::shared_ptr<ShaderPropertiesComponent>;
	using ShaderPropertiesComponentCPtr = std::shared_ptr<const ShaderPropertiesComponent>;
}

#endif 
