#include "../OpenGLHeader.h"
#include "../../Utility/CForgeUtility.h"
#include "../../Core/SLogger.h"
#include "GLShader.h"

namespace CForge {

	uint32_t GLShader::attribArrayIndex(Attribute Attrib) {
		uint32_t Rval = 0;
		switch (Attrib) {
		case ATTRIB_POSITION:		Rval = 0; break;
		case ATTRIB_NORMAL:			Rval = 1; break;
		case ATTRIB_TANGENT:		Rval = 2; break;
		case ATTRIB_UVW:			Rval = 3; break;	
		case ATTRIB_BONE_INDICES:	Rval = 4; break;
		case ATTRIB_BONE_WEIGHTS:	Rval = 5; break;
		case ATTRIB_COLOR:			Rval = 6; break;
		case ATTRIB_SPARE:			Rval = 7; break;
		default: {
			throw CForgeExcept("Invalid vertex attribute specified!");
		}break;
		}//switch[Attribute]

		return Rval;
	}//attribArrayIndex


	GLShader::GLShader(void): CForgeObject("GLShader") {
		m_VertexShaderCodes.clear();
		m_FragmentShaderCodes.clear();
		m_ComputeShaderCodes.clear();

		m_ShaderType = SHADERTYPE_UNKNOWN;

		m_ShaderProgram = GL_INVALID_INDEX;

		m_BindingPoints.clear();

		CForgeUtility::memset(&m_DefaultUBOBindingPoints[0], GL_INVALID_INDEX, DEFAULTUBO_COUNT);
		CForgeUtility::memset(&m_DefaultTextureLocations[0], -1, DEFAULTTEX_COUNT);
	}//Constructor

	GLShader::~GLShader(void) {
		clear();
	}//Destructor

	void GLShader::init(void) {
		clear();
	}//initialize

	void GLShader::clear(void) {
		m_VertexShaderCodes.clear();
		m_FragmentShaderCodes.clear();
		m_ComputeShaderCodes.clear();

		if (glIsProgram(m_ShaderProgram)) glDeleteProgram(m_ShaderProgram);
		m_ShaderProgram = GL_INVALID_INDEX;
		m_ShaderType = SHADERTYPE_UNKNOWN;

		m_BindingPoints.clear();
	

		CForgeUtility::memset(&m_DefaultUBOBindingPoints[0], GL_INVALID_INDEX, DEFAULTUBO_COUNT);
		CForgeUtility::memset(&m_DefaultTextureLocations[0], -1, DEFAULTTEX_COUNT);
	}//clear

	void GLShader::bind(void) {
		if(m_ShaderProgram != GL_INVALID_INDEX) glUseProgram(m_ShaderProgram);
	}//bind

	void GLShader::unbind(void) {
		glUseProgram(0);
	}//unbind

	void GLShader::addVertexShader(const std::string Code) {
		if (Code.empty()) throw CForgeExcept("Empty vertex shader code specified!");
		m_VertexShaderCodes.push_back(Code);
	}//addVertexShader

	void GLShader::addFragmentShader(const std::string Code) {
		if (Code.empty()) throw CForgeExcept("Empty fragment shader code specified!");
		m_FragmentShaderCodes.push_back(Code);
	}//addFragmentShader

    void GLShader::addComputeShader(const std::string Code) {
        if (Code.empty()) throw CForgeExcept("Empty compute shader code specified!");
        m_ComputeShaderCodes.push_back(Code);
    }//addFragmentShader

	void GLShader::build(std::string* pErrorLog) {
		if (nullptr == pErrorLog) throw NullpointerExcept("pErrorLog");
		(*pErrorLog) = std::string();

		if (!m_ComputeShaderCodes.empty()) {
			// this is a compute shader
			m_ShaderType = SHADERTYPE_COMPUTE;

			uint32_t ComputeShader = glCreateShader(GL_COMPUTE_SHADER);
			int32_t Status = 0;

			// compile compute shader
			try {
				compileShader(ComputeShader, &m_ComputeShaderCodes, pErrorLog);
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
				(*pErrorLog) = "Exception occurred during compute shader compilation!";
				return;
			}
			if (!pErrorLog->empty()) {
				glDeleteShader(ComputeShader);
				(*pErrorLog) += "\n" + m_ComputeShaderCodes[0];
				return;
			}

			// now link the thing
			m_ShaderProgram = glCreateProgram();
			glAttachShader(m_ShaderProgram, ComputeShader);
			glLinkProgram(m_ShaderProgram);

			glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &Status);
			if (!Status) {
				(*pErrorLog) = infoLog(m_ShaderProgram, false);
				glDeleteProgram(m_ShaderProgram);
				m_ShaderProgram = 0;
			}

			// delete shader
			glDeleteShader(ComputeShader);

			return;
			// throw CForgeExcept("Compute shader not implemented yet. Sorry, Sorry!");
		}
		else if (m_VertexShaderCodes.empty() || m_FragmentShaderCodes.empty()) {
			throw CForgeExcept("Empty fragment shader codes and/or empty vertex shader codes specified! Unable to build shader.");
		}
		else {
			m_ShaderType = SHADERTYPE_DRAW;
			uint32_t VertexShader = glCreateShader(GL_VERTEX_SHADER);;
			uint32_t FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			int32_t Status = 0;

			
			// compile vertex shader
			try {
				compileShader(VertexShader, &m_VertexShaderCodes, pErrorLog);
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
				(*pErrorLog) = "Exception occurred during vertex shader compilation!";
				return;
			}
			if (!pErrorLog->empty()) {
				glDeleteShader(VertexShader);
				glDeleteShader(FragmentShader);
				(*pErrorLog) += "\n";
				for (auto i : m_VertexShaderCodes) (*pErrorLog) += i;
				return;
			}

			// compile fragment shader
			try {
				compileShader(FragmentShader, &m_FragmentShaderCodes, pErrorLog);
			}
			catch (CrossForgeException& e) {
				SLogger::logException(e);
				(*pErrorLog) = "Exception occurred during fragment shader compilation!";
				return;
			}
			if (!pErrorLog->empty()) {
				glDeleteShader(VertexShader);
				glDeleteShader(FragmentShader);
				(*pErrorLog) += "\n";
				for (auto i : m_VertexShaderCodes) (*pErrorLog) += i;
				return;
			}

			// now link the thing
			m_ShaderProgram = glCreateProgram();
			glAttachShader(m_ShaderProgram, VertexShader);
			glAttachShader(m_ShaderProgram, FragmentShader);
			glLinkProgram(m_ShaderProgram);
			
			glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &Status);
			if (!Status) {
				(*pErrorLog) = infoLog(m_ShaderProgram, false);
				glDeleteProgram(m_ShaderProgram);
				m_ShaderProgram = 0;
			}
			
			// delete shader
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);
			

			// get default ubo binding points
			bind();
			m_DefaultUBOBindingPoints[DEFAULTUBO_CAMERADATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOCameraDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_MODELDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOModelDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_DIRECTIONALLIGHTSDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBODirectionalLightsDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_POINTLIGHTSDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOPointLightsDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_SPOTLIGHTSDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOSpotLightsDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_MATERIALDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOMaterialDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_BONEDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOBoneDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_MORPHTARGETDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOMorphTargetDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_TEXTDATA] = glGetUniformBlockIndex(m_ShaderProgram, UBOTextDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_COLORADJUSTMENT] = glGetUniformBlockIndex(m_ShaderProgram, UBOColorAdjustmentDataName.c_str());
			m_DefaultUBOBindingPoints[DEFAULTUBO_INSTANCE] = glGetUniformBlockIndex(m_ShaderProgram, UBOInstancedDataName.c_str());

			// retrieve default texture bindign points
			m_DefaultTextureLocations[DEFAULTTEX_ALBEDO] = uniformLocation(TextureAlbedoName);
			m_DefaultTextureLocations[DEFAULTTEX_NORMAL] = uniformLocation(TextureNormalName);
			m_DefaultTextureLocations[DEFAULTTEX_DEPTH] = uniformLocation(TextureDepthName);
			m_DefaultTextureLocations[DEFAULTTEX_SHADOW0] = uniformLocation(TextureShadow0Name);
			m_DefaultTextureLocations[DEFAULTTEX_SHADOW1] = uniformLocation(TextureShadow1Name);
			m_DefaultTextureLocations[DEFAULTTEX_SHADOW2] = uniformLocation(TextureShadow2Name);
			m_DefaultTextureLocations[DEFAULTTEX_SHADOW3] = uniformLocation(TextureShadow3Name);
			m_DefaultTextureLocations[DEFAULTTEX_MORPHTARGETDATA] = uniformLocation(TextureMorphTargetDataName);

			// bind shader and uniform blocks together
			for (uint8_t i = 0; i < DEFAULTUBO_COUNT; ++i) {
				if (GL_INVALID_INDEX != m_DefaultUBOBindingPoints[i]) {
					glUniformBlockBinding(m_ShaderProgram, m_DefaultUBOBindingPoints[i], m_DefaultUBOBindingPoints[i]);
				}
			}
			unbind();
		}//if[build draw shader]

		std::string ErrorMsg;
		if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			SLogger::log("Not handled OpenGL error occurred during building shader: " + ErrorMsg, "GLShader", SLogger::LOGTYPE_ERROR);
		}

	}//build

	uint32_t GLShader::uniformBlockIndex(const std::string BlockName) {
		return glGetUniformBlockIndex(m_ShaderProgram, BlockName.c_str());
	}//uniformBlockIndex

	void GLShader::compileShader(uint32_t ShaderID, std::vector<std::string>* pShaderSources, std::string* pErrorLog) {
		if (!glIsShader(ShaderID)) throw CForgeExcept("Specified shader is not valid!");
		if (nullptr == pShaderSources) throw NullpointerExcept("pShaderSources");
		if (nullptr == pErrorLog) throw NullpointerExcept("pErrorLog");

		char** ppSources = nullptr;
		int32_t* pSourcesLength = nullptr;
		int32_t Status = 0;

		// build shader (set code, build, clean)
		ppSources = new char* [pShaderSources->size()];
		pSourcesLength = new int32_t[pShaderSources->size()];
		for (uint32_t i = 0; i < pShaderSources->size(); ++i) {
			const std::string Code = pShaderSources->at(i);
			ppSources[i] = new char[Code.length()];
			memcpy(ppSources[i], Code.c_str(), Code.length());
			pSourcesLength[i] = Code.length();
		}

		glShaderSource(ShaderID, pShaderSources->size(), ppSources, pSourcesLength);
		glCompileShader(ShaderID);
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Status);
		if (!Status) {
			(*pErrorLog) = infoLog(ShaderID, true);
		}

		//clean up
		for (uint32_t i = 0; i < pShaderSources->size(); ++i) delete[] ppSources[i];
		delete[] ppSources;
		delete[] pSourcesLength;
	}//compile shader

	std::string GLShader::infoLog(uint32_t ObjectID, bool Shader) {
		char* pMsg = new char[512];
		int32_t MsgLength = 0;
		(Shader) ? glGetShaderInfoLog(ObjectID, 512, &MsgLength, pMsg) : glGetProgramInfoLog(ObjectID, 512, &MsgLength, pMsg);
		std::string Rval = std::string(pMsg);
		delete[] pMsg;
		return Rval;
	}//shaderInfoLog

	GLShader::ShaderType GLShader::type(void)const {
		return m_ShaderType;
	}//type

	uint32_t GLShader::uboBindingPoint(std::string Name) {
		if (Name.empty()) return 0;

		for (auto i : m_BindingPoints) {
			if (i.first.compare(Name) == 0) return i.second;
		}//for[known binding points]

		// binding point not known yet, look for it
		std::pair<std::string, uint32_t> Entry;
		Entry.first = Name;
		Entry.second = glGetUniformBlockIndex(m_ShaderProgram, Name.c_str());
		uniformBlockBinding(Entry.second, Entry.second);
		m_BindingPoints.push_back(Entry);
		return Entry.second;
	}//uboBindingPoint

	uint32_t GLShader::uboBindingPoint(DefaultUBO Name) {
		if (Name >= DEFAULTUBO_COUNT) throw IndexOutOfBoundsExcept("Name");
		return m_DefaultUBOBindingPoints[Name];
	}//uboBindingPoint

	void GLShader::uniformBlockBinding(uint32_t BlockIndex, uint32_t BlockBinding) {
		glUniformBlockBinding(m_ShaderProgram, BlockIndex, BlockBinding);
	}//blockBinding

	int32_t GLShader::uniformLocation(std::string Name) {
		if (Name.empty()) throw CForgeExcept("Empty uniform name specified!");
		int32_t Rval = -1;
		Rval = glGetUniformLocation(m_ShaderProgram, Name.c_str());
		return Rval;
	}//uniformLocation

	int32_t GLShader::uniformLocation(DefaultTex Tex) {
		if (Tex < 0 || Tex >= DEFAULTTEX_COUNT) throw IndexOutOfBoundsExcept("Tex");
		return m_DefaultTextureLocations[Tex];
	}//uniformLocation

	void GLShader::bindTexture(DefaultTex TexType, GLTexture2D* pTex) {
		if (TexType < 0 || TexType >= DEFAULTTEX_COUNT) throw IndexOutOfBoundsExcept("TexType");
		if (nullptr == pTex) throw NullpointerExcept("pTex");

		if (-1 != m_DefaultTextureLocations[TexType]) {
			glActiveTexture(GL_TEXTURE0 + m_DefaultTextureLocations[TexType]);
			pTex->bind();
			glUniform1i(m_DefaultTextureLocations[TexType], m_DefaultTextureLocations[TexType]);
		}

	}//bindTexture

}//name space