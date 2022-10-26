#ifndef __CFORGE_SHAPEDEFORMER_H__
#define __CFORGE_SHAPEDEFORMER_H__

#include "../../CForge/Core/CForgeObject.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	class ShapeDeformer : public CForgeObject {
	public:
		ShapeDeformer(void);
		~ShapeDeformer(void);

		void init(T3DMesh<float>* pMesh);
		void clear(void);
		void release(void);
		int binarySearch(int start, int end, int value, std::vector<int32_t> pVec);
		void setSamplePoints(void);
		void setFaceMatrix(void);
		void sortSamplePoints(Eigen::VectorXi bones);
		void prepareShapeDeformation(Eigen::VectorXi importantBones, int32_t AnimationID, int32_t startFrame, int32_t endFrame);
		Eigen::Vector2f getOptimalCapsuleRadius(T3DMesh<float>::Line Capsule1, float capsuleRadius1, T3DMesh<float>::Line Capsule2, float capsuleRadius2);
		float getAccurateCapsuleRadius(T3DMesh<float>::Line Capsule, int32_t BoneID, Eigen::Vector3f pVertex, int frame);
		Eigen::Vector3f getcloseVert(T3DMesh<float>::Line Capsule, int32_t BoneID, Eigen::Vector3f pVertex, int frame);
		int findInUnsortedVector(std::vector<int32_t> vector, int32_t value);
		std::vector<int32_t> findAdditionalVertices(T3DMesh<float>::Submesh* Mesh, std::vector<int32_t> goodVertices, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> trianglesToCheck);
		std::vector<std::vector<int32_t>> findAffectedControlPoints(T3DMesh<float>::Submesh* Submesh1, T3DMesh<float>::Submesh* Submesh2, std::vector<bool> checkMesh1, std::vector<bool> checkMesh2, std::vector<Eigen::Vector2i> affectedTriangles, int frame);
		Eigen::Vector3f bestLocationSamplePoint(T3DMesh<float>::Line Capsule, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame);
		Eigen::Vector3f bestLocationSamplePoint2(T3DMesh<float>::Line Capsule1, T3DMesh<float>::Line Capsule2, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame);
		Eigen::Vector3f bestLocationSamplePoint3(T3DMesh<float>::Line Capsule, int32_t vertexID, int32_t TriIDs[3], int32_t frame);
		Eigen::MatrixXf collisionTestShapeDeformation(int Submesh1Index, int Submesh2Index, int frame, int AnimationID, Eigen::MatrixXf U_bc);
		Eigen::MatrixXf ShapeDeformation(int32_t frame, Eigen::MatrixXf U_bc);
		void resolveCollisionsShapeDeformation(int Submesh1Index, int Submesh2Index, int32_t startFrame, int32_t endFrame, int32_t AnimationID);

	private:
		T3DMesh<float>* m_pMesh;		///< The mesh to process.

		std::vector<Eigen::MatrixXf> mat_Vertices; // Vertex data
		Eigen::MatrixXi mat_Faces; // Face data
		Eigen::VectorXi m_SamplePoints;
		std::vector<std::vector<int32_t>> m_SamplePointsInBones;
	};//SurfaceSampler


}//name space


#endif 