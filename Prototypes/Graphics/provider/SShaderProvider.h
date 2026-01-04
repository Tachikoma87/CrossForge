/*****************************************************************************\
*                                                                           *
* File(s): SShaderProvider.h and SShaderProvider.cpp     *
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
#ifndef __CROSSFORGE_SSHADERPROVIDER_H__
#define __CROSSFORGE_SSHADERPROVIDER_H__

#include <crossforge/core/CoreDefinitions.h>
#include <crossforge/assetio/entities/FileEntity.h>
#include "../entities/ShaderEntity.h"
#include "../components/shader/ShaderPropertiesComponent.h"

namespace crossforge {
	class SShaderProvider {
	public:
		enum RenderPass : int8_t {
			RENDER_PASS_UNKNOWN = -1,
			RENDER_PASS_FORWARD = 0,
			RENDER_PASS_SHADOW,
			RENDER_PASS_GEOMETRY,
			RENDER_PASS_LIGHTING,
			RENDER_PASS_COUNT
		};

		static inline std::string identification = "SShaderProvider";
		static std::shared_ptr<SShaderProvider> instance();
		static void destroy();

		bool registerShader(std::string shaderName, ShaderEntityPtr pShader);
		bool isShaderRegistered(std::string shaderName)const;
		
		ShaderEntityPtr getShader(std::string shaderName);
		ShaderEntityPtr getShader(uint32_t index);
		ShaderEntityPtr getShader(RenderPass type, ShaderPropertiesComponentPtr pShaderProperties);
		uint32_t getShaderCount()const;

		~SShaderProvider();
	protected:
		static std::shared_ptr<SShaderProvider> m_pInstance;

		SShaderProvider();

		std::map<std::string, ShaderEntityPtr> m_shaderMap;
	};

	using ShaderProvider = SShaderProvider;
	using ShaderProviderPtr = std::shared_ptr<SShaderProvider>;
	using ShaderProviderCPtr = std::shared_ptr<const SShaderProvider>;
}

#endif 