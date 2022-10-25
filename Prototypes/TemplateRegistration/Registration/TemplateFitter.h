#ifndef __TEMPREG_TEMPLATEFITTER_H__
#define __TEMPREG_TEMPLATEFITTER_H__

#include <Eigen/Eigen>

#include <cstdint>
#include <set>
#include <array>
#include <string>

#include "Dataset/DatasetGeometryData.h"

using namespace Eigen;

namespace TempReg {

	class TemplateFitter {
	public:
		enum class GeometryType {
			MESH,
			POINTCLOUD
		};

		enum class CorrespondenceType {
			NONE,
			FEATURE,
			AUTOMATIC
		};

		enum class VolumeComputation {
			DISABLED,
			FEATURE_AABBS,
			//FEATURE_CONVEX_HULLS	//TODO?
		};

		struct Correspondence {
			CorrespondenceType Type;
			float DistanceEuclidean;
			int32_t TemplateVertex;
			int32_t TargetPoint;
			Vector3f TargetPointPos;
			Vector3f TargetPointNormal;

			Correspondence() {
				Type = CorrespondenceType::NONE;
				DistanceEuclidean = FLT_MAX;
				TemplateVertex = TargetPoint = -1;
				TargetPointPos = Vector3f::Zero();
				TargetPointNormal = Vector3f::Zero();
			}
		};

		TemplateFitter(void);
		~TemplateFitter();

		void loadTemplate(GeometryType GeomType, std::string Filepath);
		void loadTarget(GeometryType GeomType, std::string Filepath);
		const DatasetGeometryData& getTemplate(void) const;
		const DatasetGeometryData& getTarget(void) const;
		const DatasetGeometryData& getFittingResult(void) const;
		const GeometryType templateGeometryType(void) const;
		const GeometryType targetGeometryType(void) const;
		const bool templateActive(void) const;
		const bool targetActive(void) const;

		void createCorrespondenceToPCL(CorrespondenceType CT, uint32_t TemplateVertex, uint32_t TargetPoint);
		void createCorrespondenceToMesh(CorrespondenceType CT, uint32_t TemplateVertex, uint32_t TargetFace, Vector3f TargetPointPos, Vector3f TargetPointNormal);
		void deleteCorrespondence(uint32_t TemplateVertex);
		void clearCorrespondenceStorage(void);
		void clearCorrespondenceStorage(CorrespondenceType CT);
		bool hasCorrespondence(uint32_t TemplateVertex) const;
		const std::set<uint32_t>& automaticCorrespondences(void) const;
		const std::set<uint32_t>& featureCorrespondences(void) const;
		const Correspondence& correspondenceData(uint32_t TemplateVertex) const;

		void clear(void);
		void clearTemplate(void);
		void clearTarget(void);
		void clearFineFittingConfig(void);
		void maxIterationsFineFitting(uint32_t It);

		void computeCoarseFitting(VolumeComputation VolCompute);
		void computeFineFitting(void);
		void computeFineFittingIteration(void);
	private:
		void computeRigidTransformation(const MatrixXf& Source, const MatrixXf& Target, Matrix3f& R, Vector3f& T);

		Vector3f computePointCentroidFromColMajor(const MatrixXf& Points);
		Vector3f computePointCentroidFromRowMajor(const MatrixXf& Points);
		//void computeConvexHull(const MatrixXf& Points, ...); //TODO?
		
		float computeAABBVolume(const MatrixXf& Points);
		//float computeConvexHullVolume(...); //TODO?
		
		float computeResidualError(const MatrixXf& Source, const MatrixXf& Target);
		float computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& S);
		float computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& R, const Vector3f& T);
		float computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& S, const Matrix3f& R, const Vector3f& T);
		//float computeResidualError(/*fine fitting parameters*/); //TODO

		bool m_TemplateActive;
		bool m_TargetActive;
		
		GeometryType m_TemplateGeometryType;
		GeometryType m_TargetGeometryType;

		DatasetGeometryData m_Template;
		DatasetGeometryData m_Target;
		DatasetGeometryData m_FittingResult;
		
		std::vector<Correspondence> m_Correspondences;
		std::set<uint32_t> m_FeatCorrQuickAccess;
		std::set<uint32_t> m_AutoCorrQuickAccess;

		float m_CurrentErrorCoarse;
		float m_ErrorDeltaCoarse;

		uint32_t m_MaxItFine;
		float m_CurrentErrorFine;
		float m_ErrorDeltaFine;
	};
}

#endif