/*****************************************************************************\
*                                                                           *
* File(s): UBOLightData.h and UBOLightData.cpp                              *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_UBOLIGHTDATA_H__
#define __CFORGE_UBOLIGHTDATA_H__

#include "../Lights/ILight.h"
#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for light related data.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT UBOLightData: CForgeObject {
	public:
		UBOLightData(void);
		~UBOLightData(void);

		void init(uint32_t DirectionalLights, uint32_t PointLights, uint32_t SpotLights);
		void clear(void);

		void bind(uint32_t BindingPoint, ILight::LightType Type);
		uint32_t size(ILight::LightType Type);

		uint32_t lightCount(ILight::LightType Type);

		void position(Eigen::Vector3f Pos, ILight::LightType Type, uint32_t Index);
		void direction(Eigen::Vector3f Dir, ILight::LightType Type, uint32_t Index);
		void color(Eigen::Vector3f Color, ILight::LightType Type, uint32_t Index);
		void intensity(float Intensity, ILight::LightType Type, uint32_t Index);
		void attenuation(Eigen::Vector3f Attenuation, ILight::LightType Type, uint32_t Index);
		void cutOff(Eigen::Vector2f CutOff, ILight::LightType Type, uint32_t Index);
		void lightSpaceMatrix(Eigen::Matrix4f Mat, ILight::LightType Type, uint32_t Index);
		void shadowID(int32_t ID, ILight::LightType Type, uint32_t Index);
		void updateLight(ILight* pLight, uint32_t Index);

	protected:
		struct DirectionalLightData {	
			
			std::vector<uint32_t> DirectionOffsets;
			std::vector<uint32_t> ColorOffsets;
			std::vector<uint32_t> IntensityOffsets;
			std::vector<uint32_t> LightMatricesOffset;
			std::vector<uint32_t> ShadowIDOffsets;
			GLBuffer Buffer;

			void init(uint32_t LightCount);
			void clear(void);
			uint32_t lightCount(void)const;
			uint32_t size(void)const;
		};

		struct PointLightData {
			std::vector<uint32_t> PositionOffsets; ///< Position.xyz and Position.w is Attenuation
			std::vector<uint32_t> IntensityOffsets;
			std::vector<uint32_t> DirectionOffsets;
			std::vector<uint32_t> ColorOffsets;	
			std::vector<uint32_t> AttenuationOffsets;	
			std::vector<uint32_t> LightMatrixOffsets;
			std::vector<uint32_t> ShadowIDOffsets;
			GLBuffer Buffer;

			void init(uint32_t LightCount);
			void clear(void);
			uint32_t lightCount(void)const;
			uint32_t size(void)const;
		};

		struct SpotLightData {
			std::vector<uint32_t> PositionOffsets;
			std::vector<uint32_t> DirectionOffsets;
			std::vector<uint32_t> OuterCutOffOffsets;
			std::vector<uint32_t> InnerCutOffOffsets;
			std::vector<uint32_t> ColorOffsets;
			std::vector<uint32_t> IntensityOffsets;
			std::vector<uint32_t> AttenuationOffsets;
			std::vector<uint32_t> LightMatrixOffsets;
			std::vector<uint32_t> ShadowIDOffsets;
			GLBuffer Buffer;

			void init(uint32_t LightCount);
			void clear(void);
			uint32_t lightCount(void)const;
			uint32_t size(void)const;
		};

		DirectionalLightData m_DirLightsData;
		PointLightData m_PointLightsData;
		SpotLightData m_SpotLightsData;
	};//UBOLightData

}//name space

#endif 