/*****************************************************************************\
*                                                                           *
* File(s): ShaderUtility.h and ShaderUtility.cpp                               *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SHADERCODE_H__
#define __CFORGE_SHADERCODE_H__

#include "../../Core/CForgeObject.h"


namespace CForge {
	class CFORGE_API ShaderCode: public CForgeObject {
	public:
		
		struct LightConfig {
			uint16_t DirLightCount;
			uint16_t PointLightCount;
			uint16_t SpotLightCount;

			float ShadowBias;
			uint16_t ShadowMapCount;
			uint16_t PCFSize;

			LightConfig(void) {
				DirLightCount = 1;
				PointLightCount = 2;
				SpotLightCount = 0;

				ShadowBias = 0.005f;
				ShadowMapCount = 1;
				PCFSize = 0;
			}

		};
		
		struct PostProcessingConfig {
			float Exposure;
			float Gamma;
			float Saturation;
			float Brightness;
			float Contrast;

			PostProcessingConfig(void) {
				Exposure = 1.0f;
				Gamma = 2.2f;
				Saturation = 1.0f;
				Brightness = 1.0f;
				Contrast = 1.0f;
			}
		};

		struct SkeletalAnimationConfig {
			uint32_t BoneCount;

			SkeletalAnimationConfig(void) {
				BoneCount = 0;
			}
		};

		struct MorphTargetAnimationConfig {
			uint32_t Stub; // no configuration yet
		};

		enum ConfigOptions: uint8_t {
			CONF_LIGHTING				= 0x01,
			CONF_POSTPROCESSING			= 0x02,
			CONF_SKELETALANIMATION		= 0x04,
			CONF_MORPHTARGETANIMATION	= 0x08,
			CONF_VERTEXCOLORS			= 0x10,
			CONF_NORMALMAPPING			= 0x20,
		};

		ShaderCode(void);
		~ShaderCode(void);

		void init(std::string ShaderCode, std::string VersionTag, uint8_t ConfigOptions, std::string PrecisionTag);
		void clear(void);

		void config(LightConfig* pConfig);
		void config(PostProcessingConfig* pConfig);
		void config(SkeletalAnimationConfig* pConfig);
		void config(MorphTargetAnimationConfig* pConfig);
		void config(uint8_t ConfigOptions);

		std::string code(void)const;

		bool requiresConfig(uint8_t ConfigOptions);

		std::string originalCode(void)const;
		std::string versionTag(void)const;
		uint8_t configOptions(void)const;
		std::string precisionTag(void)const;

	protected:
		std::string m_Code;
		std::string m_OrigCode;

		std::string m_VersionTag;
		std::string m_PrecisionTag;

		uint8_t m_ConfigOptions;

		LightConfig m_LightConfig;
		PostProcessingConfig m_PostProcessingConfig;
		SkeletalAnimationConfig m_SkeletalAnimationConfig;
		MorphTargetAnimationConfig m_MorphTargetAnimationConfig;

	private:

		void removeDefine(std::string Define);
		void addDefine(std::string Define);

		void changeConst(std::string Const, std::string Value);
		void changeVersionTag(const std::string VersionTag, const std::string PrecisionTag);

		uint32_t m_InsertPosition;
	};//ShaderCode
}


#endif 