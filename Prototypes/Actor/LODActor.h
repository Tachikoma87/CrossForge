/*****************************************************************************\
*                                                                           *
* File(s): LODActor.h and LODActor.cpp                                *
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
#ifndef __CFORGE_LODACTOR_H__
#define __CFORGE_LODACTOR_H__

#include "../../CForge/Graphics/Actors/IRenderableActor.h"
#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/GLBuffer.h"
#include "../../CForge/Graphics/GLVertexArray.h"
#include "../../CForge/Graphics/Shader/GLShader.h"
#include "../../CForge/Graphics/RenderMaterial.h"
#include "../../CForge/Graphics/Actors/VertexUtility.h"
#include "../../CForge/Graphics/Actors/RenderGroupUtility.h"
#include "../SLOD.h"

#include "../UBOInstancedData.h"

namespace CForge {
	/**
	* \brief Actor that can be placed inside the world and does not do anything besides being rendered or moved rigidly.
	*
	* \todo Do full documentation
	*/
	class LODActor : public IRenderableActor {
	public:
		LODActor(void);
		~LODActor(void);

		void init(T3DMesh<float>* pMesh, bool isInstanced = false, bool manualyInstanced = false);
		void init(T3DMesh<float>* pMesh, const std::vector<float> &LODStages, bool isInstanced = false, bool manualyInstanced = false);
		void clear(void);
		void release(void);

		void render(RenderDevice* pRDev);
		void renderInstanced(RenderDevice* pRDev, UBOInstancedData* pInstanceUBO);

		uint32_t materialCount(void)const;
		RenderMaterial* material(uint32_t level);
		
		void bindLODLevel(uint32_t index);
		uint32_t getLODLevel();
		bool isTranslucent();
		bool isVisible();

		T3DMesh<float>::AABB getAABB();
		void testAABBvis(class RenderDevice* pRDev, Eigen::Matrix4f sgMat);
		void renderAABB(class RenderDevice* pRDev);
		void queryAABB(RenderDevice* pRDev, Eigen::Matrix4f transform);
		
		void initInstancing(bool manualInstanced);
		void addInstance(Eigen::Matrix4f matrix);
		void clearInstances();
		void changeInstance(uint32_t index, Eigen::Matrix4f mat);
		void setInstanceMatrices(std::vector<Eigen::Matrix4f> matrices);
		const std::vector<Eigen::Matrix4f>* getInstanceMatrices(uint32_t level);
		
		std::vector<float> getLODStages();
		void evaluateQueryResult(Eigen::Matrix4f mat, uint32_t pixelCount);
		
		void setFaceCulling(bool state);
		
	protected:

	private:
		void initiateBuffers(uint32_t level);
		bool fovCulling(RenderDevice* pRDev, Eigen::Matrix4f* mat);
		
		SLOD* m_pSLOD;
		bool m_initialized = false;
		
		std::vector<T3DMesh<float>*> m_LODMeshes;
		std::vector<float> m_LODStages;
		uint32_t m_LODLevel;
		
		bool m_translucent = true;
		bool m_visible = true;
		bool m_faceCulling = true;
		uint32_t m_pixelCount;

		std::vector<uint16_t> m_VertexProperties;
		std::vector<RenderGroupUtility*> m_RenderGroupUtilities;
		std::vector<uint8_t*> m_VertexBuffers;
		std::vector<uint32_t> m_VertexBufferSizes;
		std::vector<uint8_t*> m_ElementBuffers;
		std::vector<uint32_t> m_ElementBufferSizes;
		
		// AABB
		T3DMesh<float>::AABB m_aabb;
		void initAABB();
		GLVertexArray m_AABBvertArray;
		GLBuffer m_AABBvertBuffer;
		GLBuffer m_AABBindexBuffer;
		GLShader* m_AABBshader;
		void updateAABB();

		// Instanced Rendering
		std::vector<Eigen::Matrix4f> m_instancedMatrices;
		std::vector<std::vector<Eigen::Matrix4f>*> m_instancedMatRef;
		
	};//LODActor

}//name space

#endif