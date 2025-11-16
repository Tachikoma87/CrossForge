/*****************************************************************************\
*                                                                           *
* File(s): RenderingShaderComponent.h and RenderingShaderComponent.cpp                       *
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
#ifndef __CROSSFORGE_RENDERINGSHADERCOMPONENT_H__
#define __CROSSFORGE_RENDERINGSHADERCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class RenderingShaderComponent : public ComponentBase {
	public:
		static inline std::string identification = "RenderingShaderComponent";

		enum AttributeArray : int8_t {
			ATTRIB_ARRAY_UNKNOWN = -1,
			ATTRIB_ARRAY_POSITION = 0,
			ATTRIB_ARRAY_NORMAL,
			ATTRIB_ARRAY_TANGENT,
			ATTRIB_ARRAY_UVW,
			ATTRIB_ARRAY_BONE_INDICES,
			ATTRIB_ARRAY_BONE_WEIGHTS,
			ATTRIB_ARRAY_COLOR,
			ATTRIB_ARRAY_SPARE,
			ATTRIB_ARRAY_COUNT
		};

		enum BaseUBO : int8_t {
			BASE_UBO_UNKNOWN = -1,
			BASE_UBO_CAMERADATA = 0,
			BASE_UBO_DIRECTIONALLIGHTSDATA,
			BASE_UBO_POINTLIGHTSDATA,
			BASE_UBO_SPOTLIGHTSDATA,
			BASE_UBO_MATERIALDATA_PBR,
			BASE_UBO_MODELDATA,
			BASE_UBO_BONEDATA,
			BASE_UBO_MORPHTARGETDATA,
			BASE_UBO_TEXTDATA,
			BASE_UBO_COLORADJUSTMENT,
			BASE_UBO_INSTANCE,
			BASE_UBO_COUNT,
		};

		enum BaseTexture : int8_t {
			BASE_TEX_UNKNOWN = -1,
			BASE_TEX_ALBEDO = 0,
			BASE_TEX_NORMAL,
			BASE_TEX_DEPTH,
			BASE_TEX_SHADOW0,
			BASE_TEX_SHADOW1,
			BASE_TEX_SHADOW2,
			BASE_TEX_SHADOW3,
			BASE_TEX_MORPHTARGETDATA,
			BASE_TEX_COUNT,
		};

		static const std::string getBaseTextureName(BaseTexture baseTex);
		static const std::string getBaseUBOName(BaseUBO baseUbo);
		static const int32_t getAttributeArrayIndex(AttributeArray attribArray);

		RenderingShaderComponent();
		~RenderingShaderComponent();

		void initialize();
		void clear() override;

		
		uint32_t& shaderProgram();
		uint32_t &baseUboBindingPoint(BaseUBO baseUbo);
		uint32_t &baseTextureLocation(BaseTexture baseTex);

	protected:
		RenderingShaderComponent(const std::string childIdentification);

		uint32_t m_shaderProgram;
		uint32_t m_baseUBOBindingPoints[BASE_UBO_COUNT];
		uint32_t m_baseTextureLocations[BASE_TEX_COUNT];
	};

	typedef std::shared_ptr<RenderingShaderComponent> RenderingShaderComponentPtr;

}

#endif