/*****************************************************************************\
*                                                                           *
* File(s): GLShader.h and GLShader.cpp                                    *
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
#ifndef __CFORGE_GLSHADER_H__
#define __CFORGE_GLSHADER_H__

#include "../../Core/CForgeObject.h"
#include "../GLTexture2D.h"

namespace CForge {

	/**
	* \brief Compiles, builds, and binds an OpenGL shader. Also provides binding points for common attributes defined by the CrossForge engine (textures, vertex attributes, UBOs).
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API GLShader: public CForgeObject {
	public:
		enum Attribute: int8_t {
			ATTRIB_UNKNOWN = -1,
			ATTRIB_POSITION = 0,
			ATTRIB_NORMAL,
			ATTRIB_TANGENT,
			ATTRIB_UVW,
			ATTRIB_BONE_INDICES,
			ATTRIB_BONE_WEIGHTS,
			ATTRIB_COLOR,
			ATTRIB_SPARE,
		};

		enum ShaderType : int8_t {
			SHADERTYPE_UNKNOWN = -1,
			SHADERTYPE_DRAW = 0,
			SHADERTYPE_COMPUTE
		};

		enum DefaultUBO : int8_t {
			DEFAULTUBO_UNKNOWN = -1,
			DEFAULTUBO_CAMERADATA = 0,
			DEFAULTUBO_DIRECTIONALLIGHTSDATA,
			DEFAULTUBO_POINTLIGHTSDATA,
			DEFAULTUBO_SPOTLIGHTSDATA,
			DEFAULTUBO_MATERIALDATA,
			DEFAULTUBO_MODELDATA,
			DEFAULTUBO_BONEDATA,
			DEFAULTUBO_MORPHTARGETDATA,
			DEFAULTUBO_COLORADJUSTMENT,
			DEFAULTUBO_INSTANCE,
			DEFAULTUBO_COUNT,
		};

		enum DefaultTex : int8_t {
			DEFAULTTEX_UNKNOWN = -1,
			DEFAULTTEX_ALBEDO = 0,
			DEFAULTTEX_NORMAL,
			DEFAULTTEX_DEPTH,
			DEFAULTTEX_SHADOW0,
			DEFAULTTEX_SHADOW1,
			DEFAULTTEX_SHADOW2,
			DEFAULTTEX_SHADOW3,
			DEFAULTTEX_MORPHTARGETDATA,
			DEFAULTTEX_COUNT,
		};

		const std::string UBOCameraDataName = "CameraData";
		const std::string UBODirectionalLightsDataName = "DirectionalLightsData";
		const std::string UBOPointLightsDataName = "PointLightsData";
		const std::string UBOSpotLightsDataName = "SpotLightsData";
		const std::string UBOMaterialDataName = "MaterialData";
		const std::string UBOModelDataName = "ModelData";
		const std::string UBOBoneDataName = "BoneData";
		const std::string UBOMorphTargetDataName = "MorphTargetData";
		const std::string UBOColorAdjustmentDataName = "ColorAdjustmentData";
		const std::string UBOInstancedDataName = "InstancedData";

		const std::string TextureAlbedoName = "TexAlbedo";
		const std::string TextureNormalName = "TexNormal";
		const std::string TextureDepthName = "TexDepth";
		const std::string TextureShadow0Name = "TexShadow[0]";
		const std::string TextureShadow1Name = "TexShadow[1]";
		const std::string TextureShadow2Name = "TexShadow[2]";
		const std::string TextureShadow3Name = "TexShadow[3]";
		const std::string TextureMorphTargetDataName = "MorphTargetDataBuffer";

		static uint32_t attribArrayIndex(Attribute Attrib);

		GLShader(void);
		~GLShader(void);

		void init(void);
		void clear(void);

		void bind(void);
		void unbind(void);

		void addVertexShader(const std::string Code);
		void addFragmentShader(const std::string Code);
        void addComputeShader(const std::string Code);

		void build(std::string *pErrorLog);

		ShaderType type(void)const;

		uint32_t uboBindingPoint(std::string Name);
		uint32_t uboBindingPoint(DefaultUBO Name);

		int32_t uniformLocation(std::string Name);
		int32_t uniformLocation(DefaultTex Tex);

		void bindTexture(DefaultTex TexType, GLTexture2D* pTex);

		uint32_t uniformBlockIndex(const std::string BlockName);
		void uniformBlockBinding(uint32_t BlockIndex, uint32_t BlockBinding);

	protected:
		void compileShader(uint32_t ShaderID, std::vector<std::string>* pShaderSources, std::string* pErrorLog);
		std::string infoLog(uint32_t ObjectID, bool Shader);

		std::vector<std::string> m_VertexShaderCodes;
		std::vector<std::string> m_FragmentShaderCodes;
		std::vector<std::string> m_ComputeShaderCodes;

		ShaderType m_ShaderType;

		uint32_t m_ShaderProgram;

		std::vector<std::pair<std::string, uint32_t>> m_BindingPoints;

		uint32_t m_DefaultUBOBindingPoints[DEFAULTUBO_COUNT];
		int32_t m_DefaultTextureLocations[DEFAULTTEX_COUNT];

	private:

	};//GLShader

}//name space

#endif 