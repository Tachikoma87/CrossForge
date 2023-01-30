#include "../../AssetIO/File.h"
#include "../../AssetIO/SAssetIO.h"
#include "ShaderCode.h"


using namespace std;

namespace CForge {

	ShaderCode::ShaderCode(void): CForgeObject("ShaderCode") {
		m_ConfigOptions = 0;
	}//Constructor

	ShaderCode::~ShaderCode(void) {

	}//Destructor

	void ShaderCode::init(std::string ShaderCode, std::string VersionTag, uint8_t ConfigOptions, std::string PrecisionTag) {
		if (ShaderCode.empty()) throw CForgeExcept("Empty shader code specified!");

		if (ShaderCode[0] == '#') {
			// already is shader code
			m_Code = ShaderCode;		
		}
		else if (!File::exists(ShaderCode)) {
			throw CForgeExcept("Shader Code file " + ShaderCode + " could not be found!");
		}
		else {
			m_Code = AssetIO::readTextFile(ShaderCode);
		}
		m_OrigCode = ShaderCode;

		m_VersionTag = VersionTag;
		m_PrecisionTag = PrecisionTag;

		changeVersionTag(VersionTag, PrecisionTag);


		m_ConfigOptions = ConfigOptions;

	}//initialize

	void ShaderCode::changeVersionTag(const std::string VersionTag, const std::string PrecisionTag) {
		// set version tag
		uint32_t Pos = m_Code.find("#version");
		// overwrite whole line with whitespaces
		uint32_t P = Pos;
		while (m_Code[P] != '\n') m_Code[P++] = ' ';
		string Tag = "#version " + VersionTag + "\n";
		m_Code.insert(Pos, Tag);
		m_InsertPosition = Pos + Tag.length();

		if (!PrecisionTag.empty()) {
			Tag = "precision " + PrecisionTag + " float;\n";
			m_Code.insert(m_InsertPosition, Tag);
			m_InsertPosition += Tag.length();
		}
	}//changeVersionTag


	void ShaderCode::clear(void) {

	}//clear

	void ShaderCode::config(LightConfig* pConfig) {
		if (nullptr == pConfig) throw NullpointerExcept("pConfig");

		m_LightConfig = (*pConfig);

		// light stuff
		if (m_LightConfig.DirLightCount == 0) {
			removeDefine("DIRECTIONAL_LIGHTS");		
		}
		else {
			addDefine("DIRECTIONAL_LIGHTS");
			changeConst("const uint DirLightCount", to_string(m_LightConfig.DirLightCount) + "U");
		}

		if (m_LightConfig.PointLightCount == 0) {
			removeDefine("POINT_LIGHTS");
		}
		else {
			addDefine("POINT_LIGHTS");
			changeConst("const uint PointLightCount", to_string(m_LightConfig.PointLightCount) + "U");
		}

		if (m_LightConfig.SpotLightCount == 0) {
			removeDefine("SPOT_LIGHTS");
		}
		else {
			addDefine("SPOT_LIGHTS");
			changeConst("const uint SpotLightCount", to_string(m_LightConfig.SpotLightCount) + "U");
		}

		// shadow stuff
		if (pConfig->PCFSize == 0) {
			removeDefine("PCF_SHADOWS");
		}
		else {
			addDefine("PCF_SHADOWS");	
			changeConst("const int PCFFilterSize", to_string(pConfig->PCFSize));
		}

		if (pConfig->ShadowMapCount > 1) {
			// requies at least version 400
			changeVersionTag("400 core", "highp float");
			addDefine("MULTIPLE_SHADOWS");
		}
		else {
			removeDefine("MULTIPLE_SHADOWS");
		}

		changeConst("const uint ShadowMapCount", to_string(pConfig->ShadowMapCount) + "U");
		changeConst("const float ShadowBias", to_string(pConfig->ShadowBias));

	}//configure

	void ShaderCode::config(PostProcessingConfig* pConfig) {
		if (nullptr == pConfig) throw NullpointerExcept("pConfig");

		m_PostProcessingConfig = (*pConfig);
		changeConst("const float Exposure", to_string(pConfig->Exposure));
		changeConst("const float Gamma", to_string(pConfig->Gamma));
		changeConst("const float Saturation", to_string(pConfig->Saturation));
		changeConst("const float Brightness", to_string(pConfig->Brightness));
		changeConst("const float Contrast", to_string(pConfig->Contrast));

	}//configure

	void ShaderCode::config(SkeletalAnimationConfig* pConfig) {
		if (nullptr == pConfig) throw NullpointerExcept("pConfig");
		
		m_SkeletalAnimationConfig = (*pConfig);

		addDefine("SKELETAL_ANIMATION");
		changeConst("const uint BoneCount", to_string(pConfig->BoneCount) + "U");
	}//configure

	void ShaderCode::config(MorphTargetAnimationConfig* pConfig) {
		if (nullptr == pConfig) throw NullpointerExcept("pConfig");

		m_MorphTargetAnimationConfig = (*pConfig);

		addDefine("MORPHTARGET_ANIMATION");
	}//config

	void ShaderCode::config(uint8_t ConfigOptions) {
		if (ConfigOptions & CONF_LIGHTING) config(&m_LightConfig);
		if (ConfigOptions & CONF_POSTPROCESSING) config(&m_PostProcessingConfig);
		if (ConfigOptions & CONF_SKELETALANIMATION) config(&m_SkeletalAnimationConfig);
		if (ConfigOptions & CONF_MORPHTARGETANIMATION) config(&m_MorphTargetAnimationConfig);
		if (ConfigOptions & CONF_VERTEXCOLORS) addDefine("VERTEX_COLORS");
		if (ConfigOptions & CONF_NORMALMAPPING) addDefine("NORMAL_MAPPING");
	}//config

	std::string ShaderCode::code(void)const {
		return m_Code;
	}//code

	void ShaderCode::removeDefine(std::string Define) {
		// find tag
		size_t Pos = m_Code.find("#define " + Define);

		if (Pos != std::string::npos) {
			size_t P = Pos;
			while (m_Code[P] != '\n') m_Code[P++] = ' ';
		}
		
	}//removeDefine

	void ShaderCode::addDefine(std::string Define) {
		size_t Pos = m_Code.find("#define " + Define);
		if (Pos != string::npos) return; //already defined

		string Tag = "#define " + Define + "\n";
		m_Code.insert(m_InsertPosition, Tag);
		m_InsertPosition += Tag.length();

	}//addDefine

	void ShaderCode::changeConst(std::string Const, std::string Value) {
		size_t Pos = m_Code.find(Const);
		if (string::npos != Pos) {
			// find "="
			while (m_Code[Pos] != '=') Pos++;
			Pos++;
			size_t P = Pos;
			while (m_Code[P] != '\n') m_Code[P++] = ' ';
			m_Code.insert(Pos, " " + Value + ";");
		}
	}//changeConst

	bool ShaderCode::requiresConfig(uint8_t ConfigOptions) {
		return (m_ConfigOptions & ConfigOptions);
	}//requiresConfig

	string ShaderCode::originalCode(void)const {
		return m_OrigCode;
	}//originalCode

	std::string ShaderCode::versionTag(void)const {
		return m_VersionTag;
	}//versionTag

	uint8_t ShaderCode::configOptions(void)const {
		return m_ConfigOptions;
	}//configOptions

	std::string ShaderCode::precisionTag(void)const {
		return m_PrecisionTag;;
	}//floatPrecisionTag

	

}//name-space