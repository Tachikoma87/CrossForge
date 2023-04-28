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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_LODACTOR_H__
#define __CFORGE_LODACTOR_H__

#include "../../crossforge/Graphics/Actors/IRenderableActor.h"
#include "../../crossforge/AssetIO/T3DMesh.hpp"
#include "../../crossforge/Graphics/GLBuffer.h"
#include "../../crossforge/Graphics/GLVertexArray.h"
#include "../../crossforge/Graphics/Shader/GLShader.h"
#include "../../crossforge/Graphics/RenderMaterial.h"
#include "../../crossforge/Graphics/Actors/VertexUtility.h"
#include "../../crossforge/Graphics/Actors/RenderGroupUtility.h"
#include "../SLOD.h"

#include "../UBOInstancedData.h"
#include "../../crossforge/Graphics/VirtualCamera.h"

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
		
		// TODO houston we need some cleanup

		uint32_t getLODLevel();
		std::vector<T3DMesh<float>*> getLODMeshes();
		std::vector<float> getLODStages();
		std::vector<float> getLODPercentages();

		void setFaceCulling(bool state);
		bool m_castShadows = true;
		/**
		* manual LOD generation without LODHandler
		*/
		void generateLODModells();
		bool isTranslucent();
		bool isVisible();

		void bindLODLevel(uint32_t index);

		T3DMesh<float>::AABB getAABB();
		void testAABBvis(class RenderDevice* pRDev, Eigen::Matrix4f sgMat);
		void renderAABB(class RenderDevice* pRDev);
		void queryAABB(RenderDevice* pRDev, Eigen::Matrix4f transform);
		void evaluateQueryResult(Eigen::Matrix4f mat, uint32_t pixelCount);
		
		// instancing
		void initInstancing(bool manualInstanced);
		void addInstance(Eigen::Matrix4f matrix);
		void clearInstances();
		void changeInstance(uint32_t index, Eigen::Matrix4f mat);
		void setInstanceMatrices(std::vector<Eigen::Matrix4f> matrices);
		const std::vector<Eigen::Matrix4f>* getInstanceMatrices(uint32_t level);
		void clearMatRef();
		
		void setLODmeshes(std::vector<T3DMesh<float>*> meshes);
		void setLODStages(std::vector<float> vec);
		void freeLODMeshes();
		void calculateLODPercentages();
		void initiateLODBuffers();
		
		//bool frustumCulling(RenderDevice* pRDev, const Eigen::Matrix4f* mat);
	protected:

	private:
		bool storeOnVRAM = true;
		GLVertexArray* m_pVertexArray; ///< the vertex array
		std::vector<GLVertexArray*> m_pVertexArrays;
		std::vector<GLBuffer*> m_pVertexBuffers;
		std::vector<GLBuffer*> m_pElementBuffers;
		
		void setBufferData(void);
		
		void initiateBuffers(uint32_t level);
		
		SLOD* m_pSLOD;
		bool m_initialized = false;
		
		std::vector<T3DMesh<float>*> m_LODMeshes;
		std::vector<float> m_LODStages;
		std::vector<float> m_LODPercentages;
		int32_t m_LODLevel = -1;
		
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
		
		// frustum culling
		//inline bool AABBonPlan(const T3DMesh<float>::AABB* aabb, const VirtualCamera::FrustumPlane* plan);
	};//LODActor

}//name space

#endif