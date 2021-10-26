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

				if (UsedMaterial == -1 && k.Material != -1) {
					UsedMaterial = k.Material;
				}
			}//for[faces]

			pRG->Range.y() = BufferPointer;

			// initialize shader
			if (UsedMaterial != -1) {	
				//pRG->pShader = pSMan->buildShader(&pMesh->getMaterial(UsedMaterial)->VertexShaderSources, &pMesh->getMaterial(UsedMaterial)->FragmentShaderSources);	

				std::vector<ShaderCode*> VSSources;
				std::vector<ShaderCode*> FSSources;
				std::string ErrorLog;
				const T3DMesh<float>::Material* pMat = pMesh->getMaterial(UsedMaterial);

				try {
					for (auto k : pMat->VertexShaderSources) {
						uint8_t ConfigOptions = 0;

						// requires skeletal animation?
						if (pMesh->boneCount() > 0) {
							ConfigOptions |= ShaderCode::CONF_SKELETALANIMATION;
						}

						ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp", "highp");

						if (pMesh->boneCount() > 0) {
							ShaderCode::SkeletalAnimationConfig SKConfig;
							SKConfig.BoneCount = pMesh->boneCount();
							pC->config(&SKConfig);
						}

						VSSources.push_back(pC);
					}

					for (auto k : pMat->FragmentShaderSources) {
						uint8_t ConfigOptions = 0;
						ShaderCode* pC = pSMan->createShaderCode(k, "330 core", ConfigOptions, "highp", "highp");
						FSSources.push_back(pC);
					}

					pRG->pShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);

					if (!ErrorLog.empty()) SLogger::log("Building shader failed:\n" + ErrorLog);
				}
				catch (const CrossForgeException& e) {
					SLogger::log("Building shader failed!\n");
				}
				

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

	std::vector<RenderGroupUtility::RenderGroup*> RenderGroupUtility::renderGroups(void) {
		return m_RenderGroups;
	}//renderGroups

	uint32_t RenderGroupUtility::renderGroupCount(void)const {
		return m_RenderGroups.size();
	}//renderGroupCount


}//name space