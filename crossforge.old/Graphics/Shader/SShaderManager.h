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
* MIT License without any warranty or guaranty to work properly.            *
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
	* \todo Write concept page on how shaders are created and used in CrossForge.
	*/
	class CFORGE_API SShaderManager: CForgeObject {
	public:

		enum DEFAULT_SHADER_SOURCE: int8_t {
			DEF_VS_GEOMETRY_PASS = 0,
			DEF_FS_GEOMETRY_PASS,
			DEF_VS_SHADOW_PASS,
			DEF_FS_SHADOW_PASS,
			DEF_VS_FORWARD_PASS,
			DEF_FS_FORWARD_PASS,
			DEF_VS_TEXT,
			DEF_FS_TEXT,
			DEF_SS_COUNT,
		};

		static SShaderManager* instance(void);
		void release(void);

		void reset(void);

		// Code can be path or actual GLSL code
		ShaderCode* createShaderCode(std::string Code, std::string VersionTag, uint8_t ConfigOptions, std::string precisionTag);
//		GLShader* buildShader(const std::vector<std::string>* pVSSources, const std::vector<std::string>* pFSSources, std::string *pErrorLog = nullptr);
		GLShader* buildShader(std::vector<ShaderCode*>* pVSSources, std::vector<ShaderCode*> *pFSSources, std::string* pErrorLog);
        GLShader* buildComputeShader(std::vector<ShaderCode*>* pCSSources, std::string* pErrorLog);
		uint32_t shaderCount(void)const;

		void configShader(ShaderCode::LightConfig LC);
		void configShader(ShaderCode::PostProcessingConfig PPC);

		std::vector<std::string> defaultShaderSources(DEFAULT_SHADER_SOURCE Type);

	protected:
		SShaderManager(void);
		virtual ~SShaderManager(void);

		void init(void);
		void clear(void);

	private:
		static SShaderManager* m_pInstance;
		static uint32_t m_InstanceCount;

		struct Shader {
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

		std::vector<std::string> m_DefVSGeometry;
		std::vector<std::string> m_DefFSGeometry;
		std::vector<std::string> m_DefVSShadow;
		std::vector<std::string> m_DefFSShadow;
		std::vector<std::string> m_DefVSForward;
		std::vector<std::string> m_DefFSForward;
		std::vector<std::string> m_DefVSText;
		std::vector<std::string> m_DefFSText;
		

	};//SShaderManager
}


#endif