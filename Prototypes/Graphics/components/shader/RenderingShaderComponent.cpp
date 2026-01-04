#include <glad/glad.h>
#include "RenderingShaderComponent.h"

namespace crossforge {
	const std::string RenderingShaderComponent::getBaseTextureName(BaseTexture baseTex) {
		std::string result = "";
		switch (baseTex) {
		case BASE_TEX_ALBEDO:	result = "TexAlbedo"; break;
		case BASE_TEX_NORMAL:	result = "TexNormal"; break;
		case BASE_TEX_DEPTH:	result = "TesDepth"; break;
		case BASE_TEX_SHADOW0:	result = "TexShadow[0]"; break;
		case BASE_TEX_SHADOW1:	result = "TexShadow[1]"; break;
		case BASE_TEX_SHADOW2:	result = "TexShadow[2]"; break;
		case BASE_TEX_SHADOW3:	result = "TexShadow[3]"; break;
		case BASE_TEX_MORPHTARGETDATA: result = "MorphTargetDataBuffer"; break;
		default: {
			LogWarning("Not handled base texture enumeration encountered: " + std::to_string(baseTex));
		}break;
		}
		return result;
	}
	const std::string RenderingShaderComponent::getBaseUBOName(BaseUBO baseUbo) {
		std::string result = "";
		switch (baseUbo) {
		case BASE_UBO_CAMERADATA:			result = "CameraData"; break;
		case BASE_UBO_DIRECTIONALLIGHTSDATA:result = "DirectionalLightsData"; break;
		case BASE_UBO_POINTLIGHTSDATA:		result = "PointLightsData"; break;
		case BASE_UBO_SPOTLIGHTSDATA:		result = "SpotLightsData"; break;
		case BASE_UBO_MATERIALDATA_PBR:		result = "MaterialData"; break;
		case BASE_UBO_MODELDATA:			result = "ModelData"; break;
		case BASE_UBO_BONEDATA:				result = "BoneData"; break;
		case BASE_UBO_MORPHTARGETDATA:		result = "MorphTargetData"; break;
		case BASE_UBO_TEXTDATA:				result = "TextData"; break;
		case BASE_UBO_COLORADJUSTMENT:		result = "ColorAdjustmentData"; break;
		case BASE_UBO_INSTANCE:				result = "InstancedData"; break;
		default: {
			LogWarning("Not handled base UBO enumeration encountered: " + std::to_string(baseUbo));
		}break;
		}
		return result;
	}
	const int32_t RenderingShaderComponent::getAttributeArrayIndex(AttributeArray attribArray) {
		int32_t result = -1;
		switch (attribArray) {
		case ATTRIB_ARRAY_POSITION: result = 0; break;
		case ATTRIB_ARRAY_NORMAL: result = 1; break;
		case ATTRIB_ARRAY_TANGENT: result = 2; break;
		case ATTRIB_ARRAY_UVW: result = 3; break;
		case ATTRIB_ARRAY_BONE_INDICES: result = 4; break;
		case ATTRIB_ARRAY_BONE_WEIGHTS: result = 5; break;
		case ATTRIB_ARRAY_COLOR: result = 6; break;
		case ATTRIB_ARRAY_SPARE: result = 7; break;
		default: {
			LogWarning("Not handled attribute array enumeration encountered: " + std::to_string(attribArray));
		}
		}
		return result;
	}

	RenderingShaderComponent::RenderingShaderComponent() : ComponentBase(RenderingShaderComponent::identification) {
		m_shaderProgram = GL_INVALID_INDEX;
		clear();
	}
	RenderingShaderComponent::RenderingShaderComponent(const std::string childIdentification): ComponentBase(RenderingShaderComponent::identification) {
		m_inheritance.push_back(childIdentification);
		m_shaderProgram = GL_INVALID_INDEX;
		initialize();
	}
	RenderingShaderComponent::~RenderingShaderComponent() {
		clear();
	}

	void RenderingShaderComponent::initialize(const std::shared_ptr<const RenderingShaderComponent> pRef) {
		if (this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_shaderProgram = pRef->getShaderProgram();
			for (int8_t i = 0; i < BASE_UBO_COUNT; ++i) m_baseUBOBindingPoints[i] = pRef->getBaseUboBindingPoint(BaseUBO(i));
			for (int8_t i = 0; i < BASE_TEX_COUNT; ++i) m_baseTextureLocations[i] = pRef->getBaseTextureLocation(BaseTexture(i));
		}
	}
	void RenderingShaderComponent::clear() {
		GeneralUtility::memset(m_baseUBOBindingPoints, GL_INVALID_INDEX, BASE_UBO_COUNT);
		GeneralUtility::memset(m_baseTextureLocations, GL_INVALID_INDEX, BASE_TEX_COUNT);
		if (glIsProgram(m_shaderProgram)) {
			LogWarning("The shader program with id " + std::to_string(m_shaderProgram) + " was not properly deleted.");
			glDeleteProgram(m_shaderProgram);
		}
		m_shaderProgram = 0;
	}


	uint32_t& RenderingShaderComponent::shaderProgram() {
		return m_shaderProgram;
	}
	uint32_t& RenderingShaderComponent::baseUboBindingPoint(BaseUBO baseUbo) {
		if (0 > baseUbo || baseUbo >= BASE_UBO_COUNT) throw IndexOutOfBoundsExcept("baseUbo");
		return m_baseUBOBindingPoints[baseUbo];
	}
	uint32_t& RenderingShaderComponent::baseTextureLocation(BaseTexture baseTex) {
		if (0 > baseTex || baseTex >= BASE_TEX_COUNT) throw IndexOutOfBoundsExcept("baseTex");
		return m_baseTextureLocations[baseTex];
	}


	const uint32_t RenderingShaderComponent::getShaderProgram()const {
		return m_shaderProgram;
	}
	const uint32_t RenderingShaderComponent::getBaseUboBindingPoint(BaseUBO baseUbo)const {
		if (BASE_UBO_UNKNOWN >= baseUbo || baseUbo >= BASE_UBO_COUNT) throw IndexOutOfBoundsExcept("baseUbo");
		return m_baseUBOBindingPoints[baseUbo];
	}
	const uint32_t RenderingShaderComponent::getBaseTextureLocation(BaseTexture baseTex)const {
		if (BASE_TEX_UNKNOWN >= baseTex || baseTex >= BASE_TEX_COUNT) throw IndexOutOfBoundsExcept("baseTex");
		return m_baseTextureLocations[baseTex];
	}

	void RenderingShaderComponent::setShaderProgram(const uint32_t program) {
		m_shaderProgram = program;
	}
	void RenderingShaderComponent::setBaseUboBindingPoint(const uint32_t bindingPoint, BaseUBO baseUbo) {
		if (BASE_UBO_UNKNOWN >= baseUbo || baseUbo >= BASE_UBO_COUNT) throw IndexOutOfBoundsExcept("baseUbo");
		m_baseUBOBindingPoints[baseUbo] = bindingPoint;
	}
	void RenderingShaderComponent::setBaseTextureLocation(const uint32_t location, BaseTexture baseTexture) {
		if (BASE_TEX_UNKNOWN >= baseTexture || baseTexture >= BASE_TEX_COUNT) throw IndexOutOfBoundsExcept("baseTexture");
		m_baseTextureLocations[baseTexture] = location;
	}
}