/*****************************************************************************\
*                                                                           *
* File(s): SShaderManager.h and SShaderManager.cpp                           *
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
#ifndef __CFORGE_SSHADERMANAGER_H__
#define __CFORGE_SSHADERMANAGER_H__

#include "../../Core/CForgeObject.h"
#include "ShaderCode.h"
#include "GLShader.h"

namespace CForge {
	/**
	* \brief Manages shaders and prevents redundant building.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API SShaderManager: CForgeObject {
	public:
		static SShaderManager* instance(void);
		void release(void);

		// Code can be path or actual GLSL code
		ShaderCode* createShaderCode(std::string Code, std::string VersionTag, uint8_t ConfigOptions, std::string FloatPrecisionTag, std::string IntegerPrecisionTag);
//		GLShader* buildShader(const std::vector<std::string>* pVSSources, const std::vector<std::string>* pFSSources, std::string *pErrorLog = nullptr);
		GLShader* buildShader(std::vector<ShaderCode*>* pVSSources, std::vector<ShaderCode*> *pFSSources, std::string* pErrorLog);
        GLShader* buildComputeShader(std::vector<ShaderCode*>* pCSSources, std::string* pErrorLog);
		uint32_t shaderCount(void)const;

		void configShader(ShaderCode::LightConfig LC);
		void configShader(ShaderCode::PostProcessingConfig PPC);

	protected:
		SShaderManager(void);
		virtual ~SShaderManager(void);

		void init(void);
		void clear(void);

	private:
		static SShaderManager* m_pInstance;
		static uint32_t m_InstanceCount;

		struct Shader {
			//std::vector<std::string> Sources;
			std::vector<ShaderCode*> VSSources;
			std::vector<ShaderCode*> FSSources;
            std::vector<ShaderCode*> CSSources;
			GLShader* pShader;
			uint32_t ReferenceCount;

			Shader(void) {
				VSSources.clear();
				FSSources.clear();
                CSSources.clear();
				pShader = nullptr;
				ReferenceCount = 0;
			}//Constructor

			~Shader(void) {
				clear();
			}//Destructor

			void clear(void) {
				VSSources.clear();
				FSSources.clear();
                CSSources.clear();
				delete pShader;
				pShader = nullptr;
			}
		};//Shader

		void configAndCompile(Shader *pShader);

		//bool findShaderSource(std::string Source, Shader* pShader);
		bool find(ShaderCode* pSC, std::vector<ShaderCode*>* pCodes);

		std::vector<Shader*> m_Shader;
		std::vector<ShaderCode*> m_ShaderCodes;

		ShaderCode::LightConfig m_LightConfig;
		ShaderCode::PostProcessingConfig m_PostProcessingConfig;

	};//SShaderManager
}


#endif