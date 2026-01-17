#include <glad/glad.h>
#include "ShaderEntityController.h"
#include "../components/shader/ShaderSourceComponent.h"
#include "../components/shader/RenderingShaderComponent.h"
#include "../../utility/GraphicsUtility.h"
#include "../components/shader/ShaderPropertiesComponent.h"
#include "../../miscellaneous/MiscUtility.hpp"
#include <crossforge/assetio/SAssetIOProvider.h>

namespace crossforge {

	ShaderEntityController::ShaderEntityController(const std::string childIdentification): ControllerBase(ShaderEntityController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	ShaderEntityController::~ShaderEntityController() {

	}

	std::string ShaderEntityController::retrieveInfoLog(uint32_t ObjectID, bool Shader) {
		char* pMsg = new char[512];
		int32_t MsgLength = 0;
		(Shader) ? glGetShaderInfoLog(ObjectID, 512, &MsgLength, pMsg) : glGetProgramInfoLog(ObjectID, 512, &MsgLength, pMsg);
		std::string Rval = std::string(pMsg);
		delete[] pMsg;
		return Rval;
	}//shaderInfoLog

	bool ShaderEntityController::compileShader(uint32_t shaderId, const std::vector<std::string> &shaderSources, std::string &errorLog) {
		if (!glIsShader(shaderId)) throw CrossForgeExcept("Specified shader object is not valid!");

		char** ppSources = nullptr;
		int32_t* pSourcesLength = nullptr;
		int32_t Status = 0;


		// build shader (set code, build, clean)
		ppSources = new char* [shaderSources.size()];
		pSourcesLength = new int32_t[shaderSources.size()];
		for (uint32_t i = 0; i < shaderSources.size(); ++i) {
			const std::string code = shaderSources[i];
			ppSources[i] = new char[code.length()];
			memcpy(ppSources[i], code.c_str(), code.length());
			pSourcesLength[i] = (int32_t)code.length();
		}

		glShaderSource(shaderId, shaderSources.size(), ppSources, pSourcesLength);
		glCompileShader(shaderId);
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Status);
		if (!Status) {
			errorLog = retrieveInfoLog(shaderId, true);
		}
		//clean up
		for (uint32_t i = 0; i < shaderSources.size(); ++i) {
			delete[] ppSources[i];
			ppSources[i] = nullptr;
		}
		delete[] ppSources;
		delete[] pSourcesLength;
		ppSources = nullptr;
		pSourcesLength = nullptr;

		return (0 != Status);
	}//compile shader

	ShaderEntityPtr ShaderEntityController::buildRenderingShader(std::vector<std::string> vertexShaderFiles, std::vector<std::string> fragmentShaderFiles) {
		ShaderEntityPtr pResult = nullptr;
		if (vertexShaderFiles.empty()) LogError("Vertex shader file list is empty.");
		else if (fragmentShaderFiles.empty()) LogError("Fragment shader files list is empty.");
		else {
			// try to load specified shader files
			pResult = std::make_shared<ShaderEntity>();
			auto pFileEntity = std::make_shared<FileEntity>();
			ShaderSourceComponentPtr pShaderSourceComp = pResult->getShaderSourceComponent(true);
			for (auto filepath : vertexShaderFiles) {
				if (AssetIOProvider::instance()->loadFile(pFileEntity, filepath, false)) {
					pShaderSourceComp->vertexShaderSources().push_back(pFileEntity->getStringComponent()->stringData());
				}
				else {
					LogError("Failed to load file: " + filepath);
				}
			}
			for (auto filepath : fragmentShaderFiles) {
				if (AssetIOProvider::instance()->loadFile(pFileEntity, filepath, false)) {
					pShaderSourceComp->fragmentShaderSources().push_back(pFileEntity->getStringComponent()->stringData());
				}
				else {
					LogError("Failed to load file: " + filepath);
				}
			}
			pFileEntity = nullptr;


			if (ShaderEntityController::buildRenderingShader(pResult)) {
				LogInfo("Successfully created rendering shader with name using " + std::to_string(vertexShaderFiles.size()) + " vertex shader and " + std::to_string(fragmentShaderFiles.size()) + " fragment shader files.");
			}
			else {
				LogError("Building shader failed.");
				pResult = nullptr;
			}

		}
		return pResult;
	}
	ShaderEntityPtr ShaderEntityController::buildRenderingShader(std::string vertexShaderFile, std::string fragmentShaderFile) {
		std::vector<std::string> vertexShaderFiles = { vertexShaderFile };
		std::vector<std::string> fragmentShaderFiles = { fragmentShaderFile };
		return buildRenderingShader(vertexShaderFiles, fragmentShaderFiles);
	}

	bool ShaderEntityController::buildRenderingShader(ShaderEntityPtr pShaderEntity) {
		if (nullptr == pShaderEntity) throw NullpointerExcept("pErrorLog");
		if(!pShaderEntity->hasComponent(ShaderSourceComponent::identification)) throw MissingComponentException(ShaderSourceComponent::identification);

		// retrieve vertex shader and fragment shader codes
		auto pShaderSourceComp = pShaderEntity->getComponent<ShaderSourceComponent>();

		if (pShaderSourceComp->vertexShaderSources().size() == 0) {
			LogError("Shader has no vertex shader sources. Can not compile.");
			return false;
		}
		if (pShaderSourceComp->fragmentShaderSources().size() == 0) {
			LogError("Shader has no fragment shader sources. Can not compile!");
			return false;
		}
	
		uint32_t VertexShader = glCreateShader(GL_VERTEX_SHADER);;
		uint32_t FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		uint32_t shaderProgram = 0;
		int32_t Status = 0;
		std::string errorLog = "";

		// compile vertex shader
		try {
			if (!compileShader(VertexShader, pShaderSourceComp->vertexShaderSources(), errorLog)) {
				LogError("Compiling vertex shader failed with error: " + errorLog);
			}
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			errorLog = "Exception occurred during vertex shader compilation!";
			return false;
		}
		if (!errorLog.empty()) {
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);
			return false;
		}

		// compile fragment shader
		try {
			if (!compileShader(FragmentShader, pShaderSourceComp->fragmentShaderSources(), errorLog)) {
				LogError("Compiling fragment shader failed with error: " + errorLog);
			}
		}
		catch (CrossForgeException& e) {
			SLogger::logException(e);
			errorLog = "Exception occurred during fragment shader compilation!";
			return false;
		}
		if (!errorLog.empty()) {
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);
			return false;
		}

		// now link the thing
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, VertexShader);
		glAttachShader(shaderProgram, FragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Status);
		if (!Status) {
			errorLog = retrieveInfoLog(shaderProgram, false);
			glDeleteProgram(shaderProgram);
			shaderProgram = 0;
		}

		// delete shader
		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);

		if (0 == shaderProgram) return false;

		// retrieve data and fill Rendering Shader component
		if (!pShaderEntity->hasComponent(RenderingShaderComponent::identification)) pShaderEntity->addComponent(std::make_shared<RenderingShaderComponent>());
		auto pRenderShaderData = pShaderEntity->getRenderingShaderComponent();
		pRenderShaderData->clear();
		pRenderShaderData->shaderProgram() = shaderProgram;


		// retrieve ubo binding points
		for (int8_t i = 0; i < RenderingShaderComponent::BASE_UBO_COUNT; ++i) {
			const RenderingShaderComponent::BaseUBO baseUbo = RenderingShaderComponent::BaseUBO(i);
			pRenderShaderData->baseUboBindingPoint(baseUbo) = glGetUniformBlockIndex(shaderProgram, pRenderShaderData->getBaseUBOName(baseUbo).c_str());
		}
		// bind shader and uniform blocks together
		for (int8_t i = 0; i < RenderingShaderComponent::BASE_UBO_COUNT; ++i) {
			uint32_t bindingPoint = pRenderShaderData->baseUboBindingPoint(RenderingShaderComponent::BaseUBO(i));
			if (GL_INVALID_INDEX != bindingPoint) glUniformBlockBinding(shaderProgram, bindingPoint, bindingPoint);
		}
		// retrieve default texture locations
		for (int8_t i = 0; i < RenderingShaderComponent::BASE_TEX_COUNT; ++i) {
			const RenderingShaderComponent::BaseTexture baseTex = RenderingShaderComponent::BaseTexture(i);
			pRenderShaderData->baseTextureLocation(baseTex) = glGetUniformLocation(shaderProgram, pRenderShaderData->getBaseTextureName(baseTex).c_str());
		}

		std::string errorMsg;
		if (GL_NO_ERROR != GraphicsUtility::checkGLError(&errorMsg)) {
			LogError("Not handled OpenGL error occurred during building shader: " + errorMsg);
		} 

		return true;
	}

	bool ShaderEntityController::bindRenderingShader(ShaderEntityPtr pShaderEntity) {
		if (nullptr == pShaderEntity) glUseProgram(0);
		else {
			auto pRenderingShader = pShaderEntity->getComponent<RenderingShaderComponent>();
			if (nullptr != pRenderingShader && GL_INVALID_INDEX != pRenderingShader->shaderProgram()) {
				glUseProgram(pRenderingShader->shaderProgram());
			}
			else {
				LogError("Shader entity with id " + std::to_string(pShaderEntity->getEntityId()) + " has no rendering shader or no valid shader program attached.");
				return false;
			}
		}
		return true;

	}

	bool ShaderEntityController::bindBaseUbo(ShaderEntityPtr pShaderEntity, UboBaseComponentPtr pUniformBuffer, RenderingShaderComponent::BaseUBO baseUbo) {
		if (nullptr == pShaderEntity) throw NullpointerExcept("pShaderEntity");
		if (nullptr == pUniformBuffer) throw NullpointerExcept("pUBOCameraDataComp");

		auto pRenderingShader = pShaderEntity->getRenderingShaderComponent();
		if (nullptr == pRenderingShader) throw MissingComponentException(RenderingShaderComponent::identification);

		bool result = false;
		const uint32_t bindingPoint = pRenderingShader->baseUboBindingPoint(baseUbo);
		const uint32_t bufferHandle = pUniformBuffer->getGlBufferHandle();
		if (GL_INVALID_INDEX == bindingPoint) {
			LogError("Specified shader does not have a valid binding point for specified base ubo.");
		}
		else if (GL_INVALID_INDEX == bufferHandle) {
			LogError("Specified uniform buffer component does not have a valid OpenGL buffer handle.");
		}
		else {
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferHandle);
			result = true;
		}

		return result;
	}

	bool ShaderEntityController::bindCameraDataUBO(ShaderEntityPtr pShaderEntity, UBOCameraDataComponentPtr pUBOCameraDataComp) {
		bool result = false;
		try {
			if (bindBaseUbo(pShaderEntity, pUBOCameraDataComp, RenderingShaderComponent::BASE_UBO_CAMERADATA)) result = true;
			else LogError("Binding camera ubo failed!");
		}
		catch (CrossForgeException& e) {
			Logger::logException(e);
		}
		return result;
		
	}

	bool ShaderEntityController::bindTransformationDataUBO(ShaderEntityPtr pShaderEntity, UBOTransformationDataComponentPtr pUBOTransformData) {
		bool result = false;
		try {
			if(bindBaseUbo(pShaderEntity, pUBOTransformData, RenderingShaderComponent::BASE_UBO_MODELDATA))	result = true;
			else LogError("Binding camera ubo failed!");
		}
		catch (CrossForgeException& e) {
			Logger::logException(e);
		}
		return result;

	}


	bool ShaderEntityController::bindTexture(ShaderEntityPtr pShader, TextureEntityPtr pTexture, RenderingShaderComponent::BaseTexture textureType) {
		if (nullptr == pShader) throw NullpointerExcept("pShader");
		if (nullptr == pTexture) throw NullpointerExcept("pTexture");

		auto pTexture2DComp = pTexture->getTexture2DComponent();
		auto pRenderingShader = pShader->getRenderingShaderComponent();
		if (nullptr == pTexture2DComp) throw MissingComponentException("pTexture2DComp");
		if (nullptr == pRenderingShader) throw MissingComponentException("pRenderingShader");

		uint32_t location = pRenderingShader->baseTextureLocation(textureType);
		uint32_t textureHandle = pTexture2DComp->glTextureHandle();
		bool result = false;
		if (GL_INVALID_INDEX == location) {
			LogError("This shader does not support texture type " + std::to_string(textureType));
		}
		else if (GL_INVALID_INDEX == textureHandle) {
			LogError("Texture handle is invalid");
		}
		else {
			glActiveTexture(GL_TEXTURE0 + location);
			glBindTexture(GL_TEXTURE_2D, textureHandle);
			glUniform1i(location, location);
			result = true;
		}
		return result;
	}

	std::string configShaderCode(std::string code, std::vector<std::string> featureList, ShaderPropertiesComponentPtr pShaderProperties) {
		if (nullptr == pShaderProperties) throw NullpointerExcept("pShaderProperties");

		auto shaderLines = GeneralUtility::splitString(code);
		std::string result = "";
		bool featuresAdded = false;

		for (uint32_t i = 0; i < shaderLines.size(); ++i) {

			if (!featuresAdded && shaderLines[i].find("#define CROSSFORGE_FEATURE_CONFIG") != std::string::npos) {
				// skip all defines
				result += shaderLines[i] + "\n";
				while (i < shaderLines.size() && shaderLines[++i].find("#define") != std::string::npos);
				for (auto feature : featureList) result += "#define " + feature + "\n";
				result += "\n";
				featuresAdded = true;
			}
			else if (shaderLines[i].find(pShaderProperties->directionalLightsSizeConstDefinition()) != std::string::npos) {
				result += pShaderProperties->directionalLightsSizeConstDefinition() + " = " + std::to_string(pShaderProperties->directionalLightsSize()) + "U;\n";
			}
			else if (shaderLines[i].find(pShaderProperties->pointLightsSizeConstDefinition()) != std::string::npos) {
				result += pShaderProperties->pointLightsSizeConstDefinition() + " = " + std::to_string(pShaderProperties->pointLightsSize()) + "U;\n";
			}
			else if (shaderLines[i].find(pShaderProperties->spotLightsSizeConstDefinition()) != std::string::npos) {
				result += pShaderProperties->spotLightsSizeConstDefinition() + " = " + std::to_string(pShaderProperties->spotLightsSize()) + "U;\n";
			}
			else if (shaderLines[i].find(pShaderProperties->activeDirectionalLightsConstDefinition()) != std::string::npos) {
				result += pShaderProperties->activeDirectionalLightsConstDefinition() + " = " + std::to_string(pShaderProperties->activeDirectionalLightsCount()) + "U;\n";
			}
			else if (shaderLines[i].find(pShaderProperties->activePointLightsConstDefinition()) != std::string::npos) {
				result += pShaderProperties->activePointLightsConstDefinition() + " = " + std::to_string(pShaderProperties->activePointLightsCount()) + "U;\n";
			}
			else if (shaderLines[i].find(pShaderProperties->activeSpotLightsConstDefinition()) != std::string::npos) {
				result += pShaderProperties->activeSpotLightsConstDefinition() + " = " + std::to_string(pShaderProperties->activeSpotLightsCount()) + "U;\n";
			}
			else {
				result += shaderLines[i] + "\n";
			}
		}

		return result;
	}


	bool ShaderEntityController::configureShaderSource(ShaderEntityPtr pShaderEntity) {
		if (nullptr == pShaderEntity) throw NullpointerExcept("pShaderEntity");

		auto pShaderSourceComp = pShaderEntity->getShaderSourceComponent();
		auto pShaderPropComp = pShaderEntity->getComponent<ShaderPropertiesComponent>();
		if (nullptr == pShaderSourceComp) throw MissingComponentException(ShaderSourceComponent::identification);
		if (nullptr == pShaderPropComp) throw MissingComponentException(ShaderPropertiesComponent::identification);

		// gather the required defines
		std::vector<std::string> featureList;
		
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_DIRECTIONAL_LIGHTS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_DIRECTIONAL_LIGHTS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_POINT_LIGHTS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_POINT_LIGHTS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_SPOT_LIGHTS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_SPOT_LIGHTS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_MULTIPLE_SHADOWS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_MULTIPLE_SHADOWS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_NORMAL_MAPPING)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_NORMAL_MAPPING));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_PCF_SHADOWS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_PCF_SHADOWS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_VERTEX_COLORS)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_VERTEX_COLORS));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_SKELETAL_ANIMATION)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_SKELETAL_ANIMATION));
		if (pShaderPropComp->hasFeatures(ShaderPropertiesComponent::SHADER_FEATURE_MORPH_TARGET_ANIMATION)) featureList.push_back(pShaderPropComp->getFeatureName(ShaderPropertiesComponent::SHADER_FEATURE_MORPH_TARGET_ANIMATION));

		for (uint32_t i = 0; i < pShaderSourceComp->vertexShaderSources().size(); ++i) {
			std::string shaderContent = pShaderSourceComp->vertexShaderSources()[i];
			pShaderSourceComp->vertexShaderSources()[i] = configShaderCode(shaderContent, featureList, pShaderPropComp);
		}

		for (uint32_t i = 0; i < pShaderSourceComp->fragmentShaderSources().size(); ++i) {
			std::string shaderContent = pShaderSourceComp->fragmentShaderSources()[i];
			pShaderSourceComp->fragmentShaderSources()[i] = configShaderCode(shaderContent, featureList, pShaderPropComp);
			
		}

		return true;
	}
}