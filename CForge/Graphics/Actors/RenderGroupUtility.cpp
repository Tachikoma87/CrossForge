#include "../../Core/SLogger.h"
#include "RenderGroupUtility.h"
#include "../Shader/SShaderManager.h"

namespace CForge {

	RenderGroupUtility::RenderGroupUtility(void): CForgeObject("RenderGroupUtiliy") {
		m_RenderGroups.clear();
	}//Constructor

	RenderGroupUtility::~RenderGroupUtility(void) {
		clear();
	}//Destructor

	void RenderGroupUtility::init(const T3DMesh<float>* pMesh, void **ppBuffer, uint32_t *pBufferSize) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (pMesh->submeshCount() == 0) throw CForgeExcept("Mesh does not contain any submeshes");

		clear();

		for (uint32_t i = 0; i < pMesh->submeshCount(); ++i) {
			m_RenderGroups.push_back(new RenderGroup());
		}//for[all submeshes]

		if (nullptr != ppBuffer && nullptr != pBufferSize) buildIndexArray(pMesh, ppBuffer, pBufferSize);
	}//initialize

	void RenderGroupUtility::clear(void) {
		for (auto& i : m_RenderGroups) delete i;
		m_RenderGroups.clear();
	}//clear

	void RenderGroupUtility::buildIndexArray(const T3DMesh<float>* pMesh, void** ppBuffer, uint32_t* pBufferSize) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		if (nullptr == ppBuffer) throw NullpointerExcept("ppBuffer");
		if (nullptr == pBufferSize) throw NullpointerExcept("pBufferSize");
		if (pMesh->submeshCount() == 0) throw CForgeExcept("Mesh contains no submeshes. Building index array not possible!");
		if (m_RenderGroups.size() == 0) throw CForgeExcept("Class was not initialized!");

		uint32_t IndexCount = 0;
		uint32_t* pBuffer = nullptr;

		// compute buffer size
		for (uint32_t i = 0; i < pMesh->submeshCount(); ++i) {
			const T3DMesh<float>::Submesh *pSM = pMesh->getSubmesh(i);
			IndexCount += pSM->Faces.size() * 3;
		}//for[submeshes]

		// allocate buffer
		pBuffer = new uint32_t[IndexCount];
		if (nullptr == pBuffer) throw OutOfMemoryExcept("pBuffer");
		uint32_t BufferPointer = 0;

		// fill buffer and build render groups
		SShaderManager* pSMan = SShaderManager::instance();
		for (uint32_t i = 0; i < m_RenderGroups.size(); ++i) {
			const T3DMesh<float>::Submesh* pSM = pMesh->getSubmesh(i);
			RenderGroup* pRG = m_RenderGroups[i];

			// set start
			pRG->Range.x() = BufferPointer;

			int32_t UsedMaterial = -1;

			for (auto const k : pSM->Faces) {
				pBuffer[BufferPointer + 0] = k.Vertices[0];
				pBuffer[BufferPointer + 1] = k.Vertices[1];
				pBuffer[BufferPointer + 2] = k.Vertices[2];
				BufferPointer += 3;

				if (UsedMaterial == -1 && pSM->Material != -1) UsedMaterial = pSM->Material;
			}//for[faces]

			pRG->Range.y() = BufferPointer;

			// initialize shader
			if (UsedMaterial != -1) {	
				
				const T3DMesh<float>::Material* pMat = pMesh->getMaterial(UsedMaterial);

				std::vector<std::string> VSSources;
				std::vector<std::string> FSSources;

				// geometry pass
				VSSources = (pMat->VertexShaderGeometryPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_VS_GEOMETRY_PASS) : pMat->VertexShaderGeometryPass;
				FSSources = (pMat->FragmentShaderGeometryPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_FS_GEOMETRY_PASS) : pMat->FragmentShaderGeometryPass;
				pRG->pShaderGeometryPass = (VSSources.empty() || FSSources.empty()) ? nullptr : createShader(pMesh, pMat, VSSources, FSSources);

				// shadow pass
				VSSources = (pMat->VertexShaderShadowPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_VS_SHADOW_PASS) : pMat->VertexShaderShadowPass;
				FSSources = (pMat->FragmentShaderShadowPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_FS_SHADOW_PASS) : pMat->FragmentShaderShadowPass;
				pRG->pShaderShadowPass = (VSSources.empty() || FSSources.empty()) ? nullptr : createShader(pMesh, pMat, VSSources, FSSources);


				// forward pass
				VSSources = (pMat->VertexShaderForwardPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_VS_FORWARD_PASS) : pMat->VertexShaderForwardPass;
				FSSources = (pMat->FragmentShaderForwardPass.empty()) ? pSMan->defaultShaderSources(SShaderManager::DEF_FS_FORWARD_PASS) : pMat->FragmentShaderForwardPass;
				pRG->pShaderForwardPass = (VSSources.empty() ||FSSources.empty()) ? nullptr : createShader(pMesh, pMat, VSSources, FSSources);
				
			}

			// initialize material
			if (UsedMaterial != -1) {
				try {
					pRG->Material.init(pMesh->getMaterial(UsedMaterial));
				}
				catch (CrossForgeException& e) {
					SLogger::logException(e);
					pRG->Material.clear();
				}
			}//if[initialize material]

		}//for[render groups]

		pSMan->release();
		pSMan = nullptr;

		// set return values
		(*ppBuffer) = pBuffer;
		(*pBufferSize) = IndexCount * sizeof(uint32_t);

	}//buildIndexArray

	GLShader* RenderGroupUtility::createShader(const T3DMesh<float>* pMesh, const T3DMesh<float>::Material* pMat, std::vector<std::string> VSSources, std::vector<std::string> FSSources) {
		GLShader* pRval = nullptr;

		SShaderManager* pSMan = SShaderManager::instance();
		std::vector<ShaderCode*> VSCodes;
		std::vector<ShaderCode*> FSCodes;

		try {

			for (auto k : VSSources) {
				uint8_t ConfigOptions = 0;

				// requires skeletal animation?
				if (pMesh->boneCount() > 0) {
					ConfigOptions |= ShaderCode::CONF_SKELETALANIMATION;
				}
				// requires morph target animation?
				if (pMesh->morphTargetCount() > 0) {
					ConfigOptions |= ShaderCode::CONF_MORPHTARGETANIMATION;
				}
				// requires per vertex colors
				if (pMesh->colorCount() > 0) {
					ConfigOptions |= ShaderCode::CONF_VERTEXCOLORS;
				}

				//requires normal mapping
				if (pMesh->tangentCount() > 0 && !pMat->TexNormal.empty()) {
					ConfigOptions |= ShaderCode::CONF_NORMALMAPPING;
				}

				// requires lighting? (currently using name of shader, that is not very clean and clever, change later)
				if (k.find("Shadow") != std::string::npos) {
					ConfigOptions |= ShaderCode::CONF_LIGHTING;
				}

				ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp");

				if (pMesh->boneCount() > 0) {
					ShaderCode::SkeletalAnimationConfig SKConfig;
					SKConfig.BoneCount = pMesh->boneCount();
					pC->config(&SKConfig);
				}

				if (pMesh->morphTargetCount() > 0) {
					ShaderCode::MorphTargetAnimationConfig MTConfig;

					pC->config(&MTConfig);
				}

				VSCodes.push_back(pC);
			}

			for (auto k : FSSources) {
				uint8_t ConfigOptions = 0;

				if (pMesh->colorCount() > 0) {
					ConfigOptions |= ShaderCode::CONF_VERTEXCOLORS;
				}
				if (pMesh->tangentCount() > 0 && !pMat->TexNormal.empty()) {
					ConfigOptions |= ShaderCode::CONF_NORMALMAPPING;
				}

				ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp");
				FSCodes.push_back(pC);
			}
		}
		catch (const CrossForgeException& e) {
			SLogger::logException(e);
		}
		try {
			std::string ErrorLog;
			pRval = pSMan->buildShader(&VSCodes, &FSCodes, &ErrorLog);
			if (!ErrorLog.empty()) SLogger::log("Building shader failed:\n" + ErrorLog);
		}
		catch (const CrossForgeException& e) {
			SLogger::logException(e);
		}
		
		return pRval;
	}//createShader


	std::vector<RenderGroupUtility::RenderGroup*> RenderGroupUtility::renderGroups(void) {
		return m_RenderGroups;
	}//renderGroups

	uint32_t RenderGroupUtility::renderGroupCount(void)const {
		return m_RenderGroups.size();
	}//renderGroupCount


}//name space