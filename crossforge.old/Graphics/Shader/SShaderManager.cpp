#include "SShaderManager.h"
#include "../../Core/SLogger.h"
#include "../../AssetIO/SAssetIO.h"

namespace CForge {

	SShaderManager* SShaderManager::m_pInstance = nullptr;
	uint32_t SShaderManager::m_InstanceCount = 0;

	SShaderManager* SShaderManager::instance(void) {
		if (nullptr == m_pInstance) {
			m_pInstance = new SShaderManager();
			m_pInstance->init();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SShaderManager::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (m_InstanceCount == 0) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}

	}//release

	void SShaderManager::reset(void) {
		for (auto& i : m_Shader) {
			delete i;
			i = nullptr;
		}
		for (auto& i : m_ShaderCodes) {
			delete i;
			i = nullptr;
		}

		m_Shader.clear();
		m_ShaderCodes.clear();
	}//reset

	ShaderCode* SShaderManager::createShaderCode(std::string Code, std::string VersionTag, uint8_t ConfigOptions, std::string PrecisionTag) {
		ShaderCode* pRval = nullptr;

		// do not create repeatedly if same object definition
		for (auto i : m_ShaderCodes) {
			if (i->originalCode().compare(Code) != 0) continue;
			if (i->versionTag().compare(VersionTag) != 0) continue;
			if (i->configOptions() != ConfigOptions) continue;
			if (i->precisionTag().compare(PrecisionTag) != 0) continue;

			pRval = i;
			break;
		}//for[all shader codes]


		if (nullptr == pRval) {
			pRval = new ShaderCode();
			pRval->init(Code, VersionTag, ConfigOptions, PrecisionTag);
			m_ShaderCodes.push_back(pRval);
		}
		return pRval;
	}//creatShaderCode

	GLShader* SShaderManager::buildShader(std::vector<ShaderCode*>* pVSSources, std::vector<ShaderCode*> *pFSSources, std::string* pErrorLog) {
		GLShader* pRval = nullptr;

		// does this shader already exist?
		for (auto i : m_Shader) {
			if (i == nullptr) continue;
			if (i->FSSources.size() != pFSSources->size() || i->VSSources.size() != pVSSources->size()) continue;

			pRval = i->pShader;

			for (auto k : (*pVSSources)) {
				if (!find(k, &i->VSSources)) {
					pRval = nullptr;
					break;
				}
			}//for[VSSources]

			if (nullptr == pRval) continue;
			
			for (auto k : (*pFSSources)) {
				if (!find(k, &i->FSSources)) {
					pRval = nullptr;
					break;
				}
			}//for[all FS sources]

			if (nullptr != pRval) break;

		}//for[all known shader]

		if (nullptr == pRval) {
			Shader* pS = new Shader();
			pS->VSSources = (*pVSSources);
			pS->FSSources = (*pFSSources);
			configAndCompile(pS);
			pRval = pS->pShader;

			m_Shader.push_back(pS);
		}//if[create new shader object]

		return pRval;
	}//buildShader

	GLShader* SShaderManager::buildComputeShader(std::vector<ShaderCode*>* pCSSources, std::string* pErrorLog) {
		GLShader* pRval = nullptr;

		// does this shader already exist?
		for (auto i : m_Shader) {
			if (i == nullptr) continue;
			if (i->CSSources.size() != pCSSources->size()) continue;

			pRval = i->pShader;

			for (auto k : (*pCSSources)) {
				if (!find(k, &i->CSSources)) {
					pRval = nullptr;
					break;
				}
			}//for[CSSources]

			if (nullptr != pRval) break;

		}//for[all known shader]

		if (nullptr == pRval) {
			Shader* pS = new Shader();
			pS->CSSources = (*pCSSources);
			configAndCompile(pS);
			pRval = pS->pShader;

			m_Shader.push_back(pS);
		}//if[create new shader object]

		return pRval;
	}


	uint32_t SShaderManager::shaderCount(void)const {
		return m_Shader.size();
	}//shaderCount

	SShaderManager::SShaderManager(void): CForgeObject("SShaderManager") {

	}//Constructor

	SShaderManager::~SShaderManager(void) {
		clear();
	}//Destructor

	void SShaderManager::init(void) {
		// nothing to do
		m_DefVSShadow.push_back("Shader/ShadowPassShader.vert");
		m_DefFSShadow.push_back("Shader/ShadowPassShader.frag");
		m_DefVSGeometry.push_back("Shader/BasicGeometryPass.vert");
		m_DefFSGeometry.push_back("Shader/BasicGeometryPass.frag");
		m_DefVSForward.push_back("Shader/ForwardPassPBS.vert");
		m_DefFSForward.push_back("Shader/ForwardPassPBS.frag");
		m_DefVSText.push_back("Shader/CFText.vert");
		m_DefFSText.push_back("Shader/CFText.frag");
	}//initialize

	void SShaderManager::clear(void) {
		for (auto& i : m_Shader) {
			delete i;
			i = nullptr;
		}
		for (auto& i : m_ShaderCodes) {
			delete i;
			i = nullptr;
		}

		m_Shader.clear();
		m_ShaderCodes.clear();

		m_LightConfig = ShaderCode::LightConfig();
		m_PostProcessingConfig = ShaderCode::PostProcessingConfig();

		m_DefVSGeometry.clear();
		m_DefFSGeometry.clear();
		m_DefVSShadow.clear();
		m_DefFSShadow.clear();
		m_DefVSForward.clear();
		m_DefFSForward.clear();
		m_DefVSText.clear();
		m_DefFSText.clear();
	}//clear

	bool SShaderManager::find(ShaderCode* pSC, std::vector<ShaderCode*>* pCodes) {
		if (nullptr == pSC) throw NullpointerExcept("pSC");
		if(nullptr == pCodes) throw NullpointerExcept("pCodes");

		bool Rval = false;
		for (auto i : (*pCodes)) {
			if (i->objectID() == pSC->objectID()) {
				Rval = true;
				break;
			}
		}//for[all codes]
		return Rval;
	}//find

	void SShaderManager::configAndCompile(Shader* pShader) {
		if (nullptr == pShader) throw NullpointerExcept("pShader");

		if(nullptr == pShader->pShader) pShader->pShader = new GLShader();
		pShader->pShader->init();

		// gather vertex shader 
		for (auto i : pShader->VSSources) {
			if (i->requiresConfig(ShaderCode::CONF_LIGHTING)) i->config(&m_LightConfig);
			if (i->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) i->config(&m_PostProcessingConfig);
			if (i->requiresConfig(ShaderCode::CONF_SKELETALANIMATION)) i->config(ShaderCode::CONF_SKELETALANIMATION);
			if (i->requiresConfig(ShaderCode::CONF_VERTEXCOLORS)) i->config(ShaderCode::CONF_VERTEXCOLORS);
			if (i->requiresConfig(ShaderCode::CONF_NORMALMAPPING)) i->config(ShaderCode::CONF_NORMALMAPPING);
			pShader->pShader->addVertexShader(i->code());
		}//for[VS sources]

		// gather fragment shader
		for (auto i : pShader->FSSources) {
			if (i->requiresConfig(ShaderCode::CONF_LIGHTING)) i->config(&m_LightConfig);
			if (i->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) i->config(&m_PostProcessingConfig);
			if (i->requiresConfig(ShaderCode::CONF_VERTEXCOLORS)) i->config(ShaderCode::CONF_VERTEXCOLORS);
			if (i->requiresConfig(ShaderCode::CONF_NORMALMAPPING)) i->config(ShaderCode::CONF_NORMALMAPPING);
			pShader->pShader->addFragmentShader(i->code());
		}//for[FS sources]


		try {
			std::string ErrorLog;
			pShader->pShader->build(&ErrorLog);

			if (!ErrorLog.empty()) {
				SLogger::log("Shader compilation failed: " + ErrorLog);
				delete pShader->pShader;
				pShader->pShader = nullptr;
			}
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			delete pShader->pShader;
			pShader->pShader = nullptr;
		}
		catch (...) {
			SLogger::log("Something went wrong during shader building!");
			delete pShader->pShader;
			pShader->pShader = nullptr;
		}
		
	}//configAndCompile


	void SShaderManager::configShader(ShaderCode::LightConfig LC) {
		m_LightConfig = LC;
		for (auto i : m_Shader) {
			bool Rebuild = false;
			for (auto k : i->VSSources) {
				if (k->requiresConfig(ShaderCode::CONF_LIGHTING)) {
					Rebuild = true;
					break;
				}
			}//for[VSSources]

			for (auto k : i->FSSources) {
				if (k->requiresConfig(ShaderCode::CONF_LIGHTING)) {
					Rebuild = true;
					break;
				}
			}//for[FSSources]

			if (Rebuild) configAndCompile(i);	
		}
	}//configShader

	void SShaderManager::configShader(ShaderCode::PostProcessingConfig PPC) {
		m_PostProcessingConfig = PPC;
		for (auto i : m_Shader) {
			bool Rebuild = false;
			for (auto k : i->VSSources) {
				if (k->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) {
					Rebuild = true;
					break;
				}
			}//for[VSSources]

			for (auto k : i->FSSources) {
				if (k->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) {
					Rebuild = true;
					break;
				}
			}//for[FSSources]

			if (Rebuild) configAndCompile(i);
		}
	}//configShader

	std::vector<std::string> SShaderManager::defaultShaderSources(DEFAULT_SHADER_SOURCE Type) {
		std::vector<std::string> Rval;

		switch (Type) {
		case DEF_VS_GEOMETRY_PASS: Rval = m_DefVSGeometry; break;
		case DEF_FS_GEOMETRY_PASS: Rval = m_DefFSGeometry; break;
		case DEF_VS_SHADOW_PASS: Rval = m_DefVSShadow; break;
		case DEF_FS_SHADOW_PASS: Rval = m_DefFSShadow; break;
		case DEF_VS_FORWARD_PASS: Rval = m_DefVSForward; break;
		case DEF_FS_FORWARD_PASS: Rval = m_DefFSForward; break;
		case DEF_VS_TEXT: Rval = m_DefVSText; break;
		case DEF_FS_TEXT: Rval = m_DefFSText; break;
		default: {
			throw CForgeExcept("Invalid default shader source type specified!");
		}break;
		}//switch[shader source type]

		return Rval;
	}//defaultShaderSources

}//name space