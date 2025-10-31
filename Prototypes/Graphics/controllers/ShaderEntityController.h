/*****************************************************************************\
*                                                                           *
* File(s): ShaderEntityController.h and ShaderEntityController.cpp                        *
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
#ifndef __CROSSFORGE_SHADERENTITYCONTROLLER_H__
#define __CROSSFORGE_SHADERENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/ShaderEntity.h"

#include "../components/uniformbuffer/UBOCameraDataComponent.h"
#include "../components/uniformbuffer/UBOTransformationDataComponent.h"
#include "../entities/TextureEntity.h"

namespace crossforge {
	class ShaderEntityController : public ControllerBase {
	public:
		static inline std::string identification = "ShaderEntityController";

		static bool configureShaderSource(ShaderEntityPtr pShaderEntity);

		static ShaderEntityPtr buildRenderingShader(std::vector<std::string> vertexShaderFiles, std::vector<std::string> fragmentShaderFiles);
		static ShaderEntityPtr buildRenderingShader(std::string vertexShaderFile, std::string fragmentShaderFile);
		static bool buildRenderingShader(ShaderEntityPtr pShaderEntity);
		static bool bindRenderingShader(ShaderEntityPtr pShaderEntity);

		static bool bindBaseUbo(ShaderEntityPtr pShaderEntity, UniformBufferComponentPtr pUniformBuffer, RenderingShaderComponent::BaseUBO baseUbo);
		static bool bindCameraDataUBO(ShaderEntityPtr pShaderEntity, UBOCameraDataComponentPtr pUBOCameraDataComp);
		static bool bindTransformationDataUBO(ShaderEntityPtr pShaderEntity, UBOTransformationDataComponentPtr pUBOTransformData);

		static bool bindTexture(ShaderEntityPtr pShader, TextureEntityPtr pTexture, RenderingShaderComponent::BaseTexture textureType);
		
		~ShaderEntityController();

	protected:
		ShaderEntityController();
		ShaderEntityController(const std::string childIdentification);

		static std::string retrieveInfoLog(uint32_t ObjectID, bool Shader);
		static bool compileShader(uint32_t shaderId, const std::vector<std::string>& shaderSources, std::string& errorLog);

	};
	typedef std::shared_ptr<ShaderEntityController> ShaderEntityControllerPtr;
}

#endif 