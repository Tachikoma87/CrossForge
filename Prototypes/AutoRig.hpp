
#ifndef __CFORGE_AUTORIGGER_H__
#define __CFORGE_AUTORIGGER_H__
#pragma once

#include "../CForge/Core/CForgeObject.h"
#include "../CForge/AssetIO/T3DMesh.hpp"
#include <glad/glad.h>

#include "../CForge/Graphics/Actors/StaticActor.h"
#include "../CForge/Graphics/RenderDevice.h"
#include "../CForge/AssetIO/SAssetIO.h"
#include "../CForge/Graphics/GraphicsUtility.h"

namespace CForge {
	class AutoRigger : public CForgeObject {
	public:
		AutoRigger();
		~AutoRigger();
		void init(T3DMesh<float>* targetMesh, T3DMesh<float>::Bone* targetSkeleton);
		void process();
		void clear();
		
		// target mesh must be in the same pose as the skeleton
		// helpful if feature joints have a large angle
		
		// endeffectors are determined by geometry furthest away from the root (point in mesh with most depth)
		
		//settings
		float gridScale = 1.25f;
		Eigen::Vector3i m_gridDim = (Eigen::Vector3f(50.0f,5.0f,50.0f)*gridScale).cast<int>(); // dimentions of the sdf grid
		// sphere size while forming graph, larger size causes more connected graph
		float sphereScaleWhileMarking = 1.25f; // smaller grid requires larger scale
		uint32_t sdfChecks = 4; // amount of checked Triangles for sdf approximation, higher values can fix errors but decrease sphere accuracy
		
		// visualization stuff ///////////////////////////////////////////////////
		void renderGrid(RenderDevice* pRDev, Eigen::Matrix4f transf, Eigen::Matrix4f scale);
		StaticActor cube;
		StaticActor sphere;
		float counter = 0.0;
		void computeSDFGrid(uint32_t start, uint32_t end);
	private:
		uint32_t AR_CORES = 7;
		struct Face {
			Eigen::Vector3f vert[3];
		};
		
		Eigen::Vector3f m_cell_dimension;
		struct GridCell {
			Eigen::Vector3f pos;
			float closestTriDist;
			int32_t closestTri;
			
			float sdf; // sdf at the cell
			int32_t sdfTri; // triangle to which distance is smallest
			
			std::vector<int32_t> VertexInfluences;
			std::vector<float>   VertexWeights;
		};
		
		std::vector<GridCell> m_grid;

		// graph connecting stuff /////////////////////////////////////////////////
		struct SphereNode {
			Eigen::Vector3f pos;
			float radius;
			std::vector<SphereNode*> neighbours;
			uint32_t visited = 0;
			bool endeffector = false;
			T3DMesh<float>::Bone* cBone = nullptr;
			
			//Tree stuff
			SphereNode* pParent;
		};
		std::vector<SphereNode> m_nodes;

		void MarkDFS(SphereNode* root);
		void MarkReset();
		void MarkBFS(SphereNode* root, uint32_t color);
		SphereNode* closestNonVisitedNode();
		
		// skeleton embedding stuff ///////////////////////////////////////////////
		std::vector<T3DMesh<float>::Bone*> getTSKEF(T3DMesh<float>::Bone* bone);
		// returns angle between 2 bones with connection b in cosine
		float Angle2Bones(Eigen::Vector3f a,Eigen::Vector3f b,Eigen::Vector3f c);
		// sets pParent
		void BFSPath(SphereNode* root);
		
		
		//grid stuff //////////////////////////////////////////////////////////////
		void buildGrid();
		
		// sdf approximation using center
		inline float sdfTriApprox(Face face, Eigen::Vector3f point);
		// distance from triangle center to point
		inline float distTriApprox(Face face, Eigen::Vector3f point);
		inline bool insideSphere(Eigen::Vector3f c, float r, Eigen::Vector3f p);
		
		T3DMesh<float>* m_targetMesh;
		T3DMesh<float>::Bone* m_targetSkeleton;
		
		static bool cmpSDFGridCell(const GridCell &a, const GridCell &b);

		virtual void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");

				pMat->Metallic = Metallic;
				pMat->Roughness = Roughness;
			}//for[materials]
		};//setMeshShader
	};
}
#endif