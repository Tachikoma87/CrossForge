#include "DatasetGeometryData.h"

#include "../../../CForge/AssetIO/SAssetIO.h"

#include <igl/unproject_ray.h>
#include <igl/embree/unproject_onto_mesh.h>

namespace TempReg {

	DatasetGeometryData::DatasetGeometryData(void) {

	}//Constructor

	DatasetGeometryData::~DatasetGeometryData() {

	}//Destructor

	void DatasetGeometryData::init(std::string Filepath, bool IsMesh, uint32_t MaxBVHDepth, float PointBoundingRadius) {
		m_IsMesh = IsMesh;

		CForge::T3DMesh<float> M;
		CForge::SAssetIO::load(Filepath, &M);

		m_Positions.resize(M.vertexCount(), 3);
		for (int32_t i = 0; i < M.vertexCount(); ++i) m_Positions.row(i) = M.vertex(i);

		computeVertexCentroid();
		
		if (m_IsMesh) {
			M.computePerVertexNormals();
			M.computePerFaceNormals();

			m_VertexNormals.resize(M.normalCount(), 3);
			for (int32_t i = 0; i < M.normalCount(); ++i) m_VertexNormals.row(i) = M.normal(i);
			
			size_t FaceCount = 0;
			for (size_t i = 0; i < M.submeshCount(); ++i) FaceCount += M.getSubmesh(i)->Faces.size();

			m_Faces.resize(FaceCount, 3);
			m_FaceNormals.resize(FaceCount, 3);

			size_t FaceOffset = 0;
			for (size_t i = 0; i < M.submeshCount(); ++i) {
				auto S = M.getSubmesh(i);

				for (size_t i = 0; i < S->FaceNormals.size(); ++i) 
					m_FaceNormals.row(i + FaceOffset) = S->FaceNormals[i];
				
				for (size_t i = 0; i < S->Faces.size(); ++i) 
					m_Faces.row(i + FaceOffset) << S->Faces[i].Vertices[0], S->Faces[i].Vertices[1], S->Faces[i].Vertices[2];

				FaceOffset += S->Faces.size();
			}

			computeSurfaceCentroid();

			m_EI.init(m_Positions, m_Faces);
			//initMeshClpBVH(MaxBVHDepth);
		}
		else {
			m_PclBVH.init(m_Positions, PointBoundingRadius, MaxBVHDepth);
		}
	}//init

	void DatasetGeometryData::init(const DatasetGeometryData& Other) {
		m_IsMesh = Other.m_IsMesh;
		m_Positions = Other.m_Positions;
		m_VertexNormals = Other.m_VertexNormals;
		m_FaceNormals = Other.m_FaceNormals;
		m_Faces = Other.m_Faces;
		m_VertexCentroid = Other.m_VertexCentroid;

		if (Other.m_IsMesh) {
			m_EI.init(m_Positions, m_Faces);
			//initMeshClpBVH(MaxBVHDepth);
		}
		else {
			m_PclBVH.init(Other.m_PclBVH);
		}
	}//init

	void DatasetGeometryData::clear(void) {
		m_Positions.resize(0, 0);
		m_VertexNormals.resize(0, 0);
		m_FaceNormals.resize(0, 0);
		m_Faces.resize(0, 0);
		m_VertexCentroid = Vector3f::Zero();
		m_SurfaceCentroid = Vector3f::Zero();

		if (m_IsMesh) m_EI.deinit();
		if (!m_IsMesh) m_PclBVH.clear();
		m_IsMesh = false;
	}//clear

	void DatasetGeometryData::applyTransformation(const Matrix3f& M) {
		m_Positions *= M.transpose();
			
		computeVertexCentroid();

		if (m_IsMesh) {
			computeFaceNormals();
			computeVertexNormals();
			computeSurfaceCentroid();
			m_EI.deinit();
			m_EI.init(m_Positions, m_Faces);
		}
		else {
			m_PclBVH.generate(m_Positions);
		}
	}//applyTransformation
	
	void DatasetGeometryData::applyTransformation(const Vector3f& T) {
		m_Positions.rowwise() += T.transpose();

		computeVertexCentroid();

		if (m_IsMesh) {
			computeFaceNormals();
			computeVertexNormals();
			computeSurfaceCentroid();
			m_EI.deinit();
			m_EI.init(m_Positions, m_Faces);
		}
		else {
			m_PclBVH.generate(m_Positions);
		}
	}//applyTransformation
	
	void DatasetGeometryData::applyTransformation(const Matrix3f& R, const Vector3f& T) {
		m_Positions *= R.transpose();
		m_Positions.rowwise() += T.transpose();

		computeVertexCentroid();

		if (m_IsMesh) {
			computeFaceNormals();
			computeVertexNormals();
			computeSurfaceCentroid();
			m_EI.deinit();
			m_EI.init(m_Positions, m_Faces);
		}
		else {
			m_PclBVH.generate(m_Positions);
		}
	}//applyTransformation
	
	void DatasetGeometryData::applyTransformation(const Matrix3f& S, const Matrix3f& R, const Vector3f& T) {
		m_Positions *= S.transpose();
		m_Positions *= R.transpose();
		m_Positions.rowwise() += T.transpose();
		
		computeVertexCentroid();

		if (m_IsMesh) {
			computeFaceNormals();
			computeVertexNormals();
			computeSurfaceCentroid();
			m_EI.deinit();
			m_EI.init(m_Positions, m_Faces);
		}
		else {
			m_PclBVH.generate(m_Positions);
		}
	}//applyTransformation

	void DatasetGeometryData::computeNormals(void) {
		if (m_IsMesh) {
			computeFaceNormals();
			computeVertexNormals();
		}
	}//computeNormals

	void DatasetGeometryData::computeVertexCentroid(void) {
		m_VertexCentroid = m_Positions.colwise().sum();
		m_VertexCentroid /= m_Positions.rows();
	}//computeVertexCentroid

	void DatasetGeometryData::computeSurfaceCentroid(void) {
		float AreaSum = 0.0f;
		m_SurfaceCentroid = Vector3f::Zero();

		for (uint32_t i = 0; i < m_Faces.rows(); ++i) {
			Vector3f P0 = m_Positions.row(m_Faces.row(i)(0));
			Vector3f P1 = m_Positions.row(m_Faces.row(i)(1));
			Vector3f P2 = m_Positions.row(m_Faces.row(i)(2));
			Vector3f TriCenter = (P0 + P1 + P2) / 3.0f;
			float TriArea = ((P1 - P0).cross(P2 - P0)).norm() * 0.5f;
			m_SurfaceCentroid += TriArea * TriCenter;
			AreaSum += TriArea;
		}

		m_SurfaceCentroid /= AreaSum;
	}//computeSurfaceCentroid

	void DatasetGeometryData::updateActiveBVHs(void) {
		if (m_IsMesh) {
			m_EI.deinit();
			m_EI.init(m_Positions, m_Faces);

			//TODO: update MeshClpBVH
		}
		else { // dataset is a point cloud
			m_PclBVH.generate(m_Positions);
		}
	}//updateBVH

	const Vector3f DatasetGeometryData::vertexPosition(uint32_t Idx) const {
		Vector3f Vertex = m_Positions.row(Idx);
		return Vertex;
	}//vertexPosition

	const MatrixXf& DatasetGeometryData::vertexPositions(void) const {
		return m_Positions;
	}//vertexPositions

	void DatasetGeometryData::vertexPositions(const MatrixXf& Vertices) {
		m_Positions.resize(Vertices.rows(), Vertices.cols());
		m_Positions = Vertices;
	}//vertexPositions

	const int32_t DatasetGeometryData::closestVertex(const uint32_t Face, const Vector3f SurfacePoint, float Range) const {
		Vector3i F = face(Face);
		Vector3f V0 = m_Positions.row(F(0));
		Vector3f V1 = m_Positions.row(F(1));
		Vector3f V2 = m_Positions.row(F(2));

		float DistToV0 = (V0 - SurfacePoint).norm();
		float DistToV1 = (V1 - SurfacePoint).norm();
		float DistToV2 = (V2 - SurfacePoint).norm();

		float DistToClosest = DistToV0;
		size_t ClosestVertex = F(0);

		if (DistToV1 < DistToClosest) {
			DistToClosest = DistToV1;
			ClosestVertex = F(1);
		}

		if (DistToV2 < DistToClosest) {
			DistToClosest = DistToV2;
			ClosestVertex = F(2);
		}

		int32_t PickedVertex = -1;

		if (DistToClosest < Range)
			PickedVertex = ClosestVertex;

		return PickedVertex;
	}//closestVertex

	size_t DatasetGeometryData::vertexCount(void) const {
		return m_Positions.rows();
	}//vertexCount

	const Vector3i DatasetGeometryData::face(uint32_t Idx) const {
		Vector3i Face;
		Face(0) = m_Faces(Idx, 0);
		Face(1) = m_Faces(Idx, 1);
		Face(2) = m_Faces(Idx, 2);
		return Face;
	}//face

	const MatrixXi& DatasetGeometryData::faces(void) const {
		return m_Faces;
	}//faces

	size_t DatasetGeometryData::faceCount(void) const {
		return m_Faces.rows();
	}//faceCount

	const Vector3f DatasetGeometryData::vertexNormal(uint32_t Idx) const {
		Vector3f Normal;
		Normal(0) = m_VertexNormals(Idx, 0);
		Normal(1) = m_VertexNormals(Idx, 1);
		Normal(2) = m_VertexNormals(Idx, 2);
		return Normal;
	}//vertexNormal

	const Vector3f DatasetGeometryData::faceNormal(uint32_t Idx) const {
		Vector3f Normal;
		Normal(0) = m_FaceNormals(Idx, 0);
		Normal(1) = m_FaceNormals(Idx, 1);
		Normal(2) = m_FaceNormals(Idx, 2);
		return Normal;
	}//faceNormal

	const Vector3f DatasetGeometryData::vertexCentroid(void) const {
		return m_VertexCentroid;
	}//vertexCentroid

	const Vector3f DatasetGeometryData::surfaceCentroid(void) const {
		return m_SurfaceCentroid;
	}//surfaceCentroid

	void DatasetGeometryData::raycastMeshPoint(const Vector2f& CursorPosOGL, const Vector4f& Viewport, const Matrix4f& Model, const Matrix4f& View, const Matrix4f& Projection, int32_t& IntersectedFace, Vector3f& IntersectionPoint, Vector3f& BaryCoords) const {
		IntersectedFace = -1;
		IntersectionPoint = Vector3f::Zero();
		BaryCoords = Vector3f::Zero();

		if (igl::embree::unproject_onto_mesh(CursorPosOGL, m_Faces, View * Model, Projection, Viewport, m_EI, IntersectedFace, BaryCoords)) {
			Vector3i Face = face(IntersectedFace);		
			Vector3f V0 = m_Positions.row(Face(0));
			Vector3f V1 = m_Positions.row(Face(1));
			Vector3f V2 = m_Positions.row(Face(2));
			IntersectionPoint = (BaryCoords.x() * V0) + (BaryCoords.y() * V1) + (BaryCoords.z() * V2);
		}
	}//raycastMeshPoint

	void DatasetGeometryData::raycastPclPoint(const Vector2f& CursorPosOGL, const Vector4f& Viewport, const Matrix4f& Model, const Matrix4f& View, const Matrix4f& Projection, int32_t& PickedPoint) const {
		// calculate world space ray, grab octree data
		Vector3f RayOrigin, RayDir;
		igl::unproject_ray(CursorPosOGL, View * Model, Projection, Viewport, RayOrigin, RayDir);
		RayDir.normalize();
		PickedPoint = m_PclBVH.raycastClosestPoint(RayOrigin, RayDir, m_Positions);
	}//raycastPclPoint

	void DatasetGeometryData::computeFaceNormals(void) {
		for (size_t i = 0; i < m_Faces.rows(); ++i) {
			Vector3f V0 = m_Positions.row(m_Faces(i, 0));
			Vector3f V1 = m_Positions.row(m_Faces(i, 1));
			Vector3f V2 = m_Positions.row(m_Faces(i, 2));
			const Eigen::Vector3f a = V0 - V2;
			const Eigen::Vector3f b = V1 - V2;
			const Eigen::Vector3f n = a.cross(b);
			m_FaceNormals.row(i) = n.normalized();
		}
	}//computeFaceNormals

	void DatasetGeometryData::computeVertexNormals(void) {
		// reset normals
		m_VertexNormals.setZero();

		// sum normals
		for (size_t i = 0; i < m_Faces.rows(); ++i) {
			m_VertexNormals.row(m_Faces(i, 0)) += m_FaceNormals.row(i);
			m_VertexNormals.row(m_Faces(i, 1)) += m_FaceNormals.row(i);
			m_VertexNormals.row(m_Faces(i, 2)) += m_FaceNormals.row(i);
		}

		// normalize normals
		m_VertexNormals.rowwise().normalize();
	}//computeVertexNormals
}