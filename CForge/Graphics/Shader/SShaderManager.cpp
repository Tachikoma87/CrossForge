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

	ShaderCode* SShaderManager::createShaderCode(std::string Code, std::string VersionTag, uint8_t ConfigOptions, std::string FloatPrecisionTag, std::string IntegerPrecisionTag) {
		ShaderCode* pRval = nullptr;

		for (auto i : m_ShaderCodes) {
			/*if (i != nullptr && i->originalCode().compare(Code) == 0) {
				pRval = i;
				break;
			}*/
		}//for[all shader codes]


		if (nullptr == pRval) {
			pRval = new ShaderCode();
			pRval->init(Code, VersionTag, ConfigOptions, FloatPrecisionTag, IntegerPrecisionTag);
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

			/*pS->pShader->init();
			for (auto k : pS->VSSources) pS->pShader->addVertexShader(k->code());
			for (auto k : pS->FSSources) pS->pShader->addFragmentShader(k->code());
			std::string ErrorLog;
			try {
				pS->pShader->build(&ErrorLog);
			}
			catch (const CrossForgeException& e) {
				delete pS->pShader;
				delete pS;
				SLogger::logException(e);
				return nullptr;
			}

			if (!ErrorLog.empty() && nullptr != perror) {
				delete pS->pShader;
				(*pErrorLog) = ErrorLog;
			}
			else {
				pRval = pS->pShader;
				m_Shader.push_back(pS);
			}*/
			

		}//if[create new shader object]

		return pRval;
	}//buildShader

    GLShader *SShaderManager::buildComputeShader(std::vector<ShaderCode *> *pCSSources, std::string *pErrorLog) {
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

	//GLShader* SShaderManager::buildShader(const std::vector<std::string>* pVSSources, const std::vector<std::string>* pFSSources, std::string *pErrorLog) {
	//	if (nullptr == pVSSources) throw NullpointerExcept("pVSrouces");
	//	if (nullptr == pFSSources) throw NullpointerExcept("pFSSources");

	//	if (pVSSources->empty() || pFSSources->empty()) return nullptr;

	//	GLShader* pRval = nullptr;

	//	// search for shader with same sources
	//	for (auto i : m_Shader) {
	//		if (i->Sources.size() != pVSSources->size() + pFSSources->size()) continue;

	//		pRval = i->pShader;

	//		for (auto k : (*pVSSources)) {
	//			if (!findShaderSource(k, i)) {
	//				pRval = nullptr;
	//				break;
	//			}
	//		}//for[VSources]

	//		if (nullptr == pRval) break;

	//		for (auto k : (*pFSSources)) {
	//			if (!findShaderSource(k, i)) {
	//				pRval = nullptr;
	//				break;
	//			}
	//		}//for[FSources]

	//		if (nullptr != pRval) {
	//			i->ReferenceCount++;
	//			return pRval;
	//		}
	//	}//for[shader]

	//	
	//	// need to build shader
	//	pRval = new GLShader();
	//	pRval->init();
	//	
	//	std::string Code;
	//	for (auto i : (*pVSSources)) {
	//		if (File::exists(i)) {
	//			Code = AssetIO::readTextFile(i);
	//			pRval->addVertexShader(Code);
	//		}
	//		else {
	//			SLogger::log("Unable to find VS code " + i);
	//		}
	//		
	//	}
	//	for (auto i : (*pFSSources)) {
	//		if (File::exists(i)) {
	//			Code = AssetIO::readTextFile(i);
	//			pRval->addFragmentShader(Code);
	//		}
	//		else {
	//			SLogger::log("Unable to find FS code " + i);
	//		}	
	//	}

	//	try {
	//		std::string ErrorLog;
	//		pRval->build(&ErrorLog);

	//		if (!ErrorLog.empty()) {
	//			SLogger::log("Shader compilation " + pVSSources->at(0) + " | " + pFSSources->at(0) + "failed: " + ErrorLog);
	//			delete pRval;
	//			pRval = nullptr;

	//			if (nullptr != pErrorLog) (*pErrorLog) = ErrorLog;
	//		}
	//	}
	//	catch (CrossForgeException& e) {
	//		SLogger::logException(e);
	//		delete pRval;
	//		pRval = nullptr;
	//	}
	//	catch (...) {
	//		SLogger::log("Something went wrong during shader building!");
	//		delete pRval;
	//		pRval = nullptr;
	//	}

	//	if (nullptr != pRval) {
	//		Shader* pS = new Shader();
	//		pS->Sources = (*pVSSources);
	//		for (auto i : (*pFSSources)) pS->Sources.push_back(i);
	//		pS->pShader = pRval;
	//		pS->ReferenceCount = 1;
	//		m_Shader.push_back(pS);
	//	}

	//	return pRval;
	//}//buildShader

	//bool SShaderManager::findShaderSource(std::string Source, Shader* pShader) {
	//	bool Rval = false;
	//	for (auto const k : pShader->Sources) {
	//		if (0 == k.compare(Source)) {
	//			Rval = true;
	//			break;
	//		}
	//	}
	//	return Rval;
	//}//findShaderSource

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
			pShader->pShader->addVertexShader(i->code());
		}//for[VS sources]

		// gather fragment shader
		for (auto i : pShader->FSSources) {
			if (i->requiresConfig(ShaderCode::CONF_LIGHTING)) i->config(&m_LightConfig);
			if (i->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) i->config(&m_PostProcessingConfig);
			pShader->pShader->addFragmentShader(i->code());
		}//for[FS sources]


        // gather compute shader
        for (auto i : pShader->CSSources) {
            if (i->requiresConfig(ShaderCode::CONF_LIGHTING)) i->config(&m_LightConfig);
            if (i->requiresConfig(ShaderCode::CONF_POSTPROCESSING)) i->config(&m_PostProcessingConfig);
            pShader->pShader->addComputeShader(i->code());
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
}//name space