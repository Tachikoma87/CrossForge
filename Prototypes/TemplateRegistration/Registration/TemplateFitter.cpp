#include "TemplateFitter.h"

#include "../../CForge/Core/CrossForgeException.h"
#include "../../CForge/AssetIO/SAssetIO.h"
#include "../../CForge/AssetIO/T3DMesh.hpp"

#include <cmath>

#include <igl/volume.h>

#include "Dataset/DatasetGeometryData.h"

namespace TempReg {

	TemplateFitter::TemplateFitter(void) {
		m_TemplateActive = m_TargetActive = false;
		m_TemplateGeometryType = m_TargetGeometryType = GeometryType::MESH;
		m_CurrentErrorCoarse = m_ErrorDeltaCoarse = 0.0f;
		m_MaxItFine = 0;
		m_CurrentErrorFine = m_ErrorDeltaFine = 0.0f;
	}//Constructor

	TemplateFitter::~TemplateFitter() {

	}//Destructor

	void TemplateFitter::loadTemplate(GeometryType GeomType, std::string Filepath) {		
		m_Template.init(Filepath, true);
		m_FittingResult.init(m_Template);	
		m_TemplateGeometryType = GeomType;

		// initialize correspondence storage
		m_Correspondences.clear();
		m_Correspondences = std::vector<Correspondence>(m_Template.vertexCount(), Correspondence());
		m_TemplateActive = true;
	}//loadTemplate
	
	void TemplateFitter::loadTarget(GeometryType GeomType, std::string Filepath) {		
		m_Target.init(Filepath, (GeomType == GeometryType::MESH) ? true : false);		
		m_TargetGeometryType = GeomType;
		m_TargetActive = true;
	}//loadTarget

	const DatasetGeometryData& TemplateFitter::getTemplate(void) const {
		return m_Template;
	}//getTemplate

	const DatasetGeometryData& TemplateFitter::getTarget(void) const {
		return m_Target;
	}//getTarget

	const DatasetGeometryData& TemplateFitter::getFittingResult(void) const {
		return m_FittingResult;
	}//getFittingResult

	const TemplateFitter::GeometryType TemplateFitter::templateGeometryType(void) const {
		return m_TemplateGeometryType;
	}//templateGeometryType
	
	const TemplateFitter::GeometryType TemplateFitter::targetGeometryType(void) const {
		return m_TargetGeometryType;
	}//targetGeometryType
	
	const bool TemplateFitter::templateActive(void) const {
		return m_TemplateActive;
	}//templateActive
	
	const bool TemplateFitter::targetActive(void) const {
		return m_TargetActive;
	}//targetActive

	void TemplateFitter::createCorrespondenceToPCL(CorrespondenceType CT, uint32_t TemplateVertex, uint32_t TargetPoint) {
		if (TemplateVertex > m_Correspondences.size()) throw IndexOutOfBoundsExcept("TemplateVertex");
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");

		// set correspondence data
		m_Correspondences[TemplateVertex].TemplateVertex = TemplateVertex;
		m_Correspondences[TemplateVertex].TargetPoint = TargetPoint;
		m_Correspondences[TemplateVertex].TargetPointPos = m_Target.vertexPosition(TargetPoint);
		m_Correspondences[TemplateVertex].TargetPointNormal = Vector3f::Zero(); //TODO: point normals for point clouds?
		m_Correspondences[TemplateVertex].Type = CT;
		m_Correspondences[TemplateVertex].DistanceEuclidean = (m_FittingResult.vertexPosition(TemplateVertex) - m_Target.vertexPosition(TargetPoint)).norm();

		// add to quick access
		if (CT == CorrespondenceType::AUTOMATIC) m_AutoCorrQuickAccess.insert(TemplateVertex);
		else m_FeatCorrQuickAccess.insert(TemplateVertex);
	}//createCorrespondenceToPCL

	void TemplateFitter::createCorrespondenceToMesh(CorrespondenceType CT, uint32_t TemplateVertex, uint32_t TargetFace, Vector3f TargetPointPos, Vector3f TargetPointNormal) {
		if (TemplateVertex > m_Correspondences.size()) throw IndexOutOfBoundsExcept("TemplateVertex");
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");

		// add correspondence data
		m_Correspondences[TemplateVertex].TemplateVertex = TemplateVertex;
		m_Correspondences[TemplateVertex].TargetPoint = -1;
		m_Correspondences[TemplateVertex].TargetPointPos = TargetPointPos;
		m_Correspondences[TemplateVertex].TargetPointNormal = TargetPointNormal;
		m_Correspondences[TemplateVertex].Type = CT;
		m_Correspondences[TemplateVertex].DistanceEuclidean = (m_FittingResult.vertexPosition(TemplateVertex) - TargetPointPos).norm();

		// add to quick access
		if (CT == CorrespondenceType::AUTOMATIC) m_AutoCorrQuickAccess.insert(TemplateVertex);
		else m_FeatCorrQuickAccess.insert(TemplateVertex);
	}//createCorrespondenceToMesh

	void TemplateFitter::deleteCorrespondence(uint32_t TemplateVertex) {
		if (m_Correspondences[TemplateVertex].Type == CorrespondenceType::NONE) return;

		if (m_Correspondences[TemplateVertex].Type == CorrespondenceType::AUTOMATIC) m_AutoCorrQuickAccess.erase(TemplateVertex);
		else m_FeatCorrQuickAccess.erase(TemplateVertex);

		m_Correspondences[TemplateVertex].TemplateVertex = -1;
		m_Correspondences[TemplateVertex].TargetPoint = -1;
		m_Correspondences[TemplateVertex].TargetPointPos = Vector3f::Zero();
		m_Correspondences[TemplateVertex].TargetPointNormal = Vector3f::Zero();
		m_Correspondences[TemplateVertex].Type = CorrespondenceType::NONE;
		m_Correspondences[TemplateVertex].DistanceEuclidean = FLT_MAX;
	}//deleteCorrespondence

	void TemplateFitter::clearCorrespondenceStorage(void) {
		for (auto& C : m_Correspondences) {
			C.TemplateVertex = -1;
			C.TargetPoint = -1;
			C.TargetPointPos = Vector3f::Zero();
			C.TargetPointNormal = Vector3f::Zero();
			C.Type = CorrespondenceType::NONE;
			C.DistanceEuclidean = FLT_MAX;
		}

		m_AutoCorrQuickAccess.clear();
		m_FeatCorrQuickAccess.clear();
	}//clearCorrespondenceStorage

	void TemplateFitter::clearCorrespondenceStorage(CorrespondenceType CT) {
		if (CT == CorrespondenceType::NONE) throw CForgeExcept("Invalid value for CT");

		auto& Corrs = (CT == CorrespondenceType::AUTOMATIC) ? m_AutoCorrQuickAccess : m_FeatCorrQuickAccess;
		for (auto& C : Corrs) {
			m_Correspondences[C].TemplateVertex = -1;
			m_Correspondences[C].TargetPoint = -1;
			m_Correspondences[C].TargetPointPos = Vector3f::Zero();
			m_Correspondences[C].TargetPointNormal = Vector3f::Zero();
			m_Correspondences[C].Type = CorrespondenceType::NONE;
			m_Correspondences[C].DistanceEuclidean = FLT_MAX;
		}

		Corrs.clear();
	}//clearCorrespondenceStorage

	bool TemplateFitter::hasCorrespondence(uint32_t TemplateVertex) const {
		return m_Correspondences[TemplateVertex].Type != CorrespondenceType::NONE;
	}//hasCorrespondence

	const std::set<uint32_t>& TemplateFitter::automaticCorrespondences(void) const {
		return m_AutoCorrQuickAccess;
	}//automaticCorrespondences

	const std::set<uint32_t>& TemplateFitter::featureCorrespondences(void) const {
		return m_FeatCorrQuickAccess;
	}//featureCorrespondences

	const TemplateFitter::Correspondence& TemplateFitter::correspondenceData(uint32_t TemplateVertex) const {
		if (TemplateVertex > m_Correspondences.size()) throw IndexOutOfBoundsExcept("CorrID");
		return m_Correspondences[TemplateVertex];
	}//correspondenceData

	void TemplateFitter::clear(void) {
		if (m_TemplateActive) {
			m_Template.clear();
			m_FittingResult.clear();
		}
		if (m_TargetActive) m_Target.clear();

		m_TemplateActive = false;
		m_TargetActive = false;

		m_Correspondences.clear();
		m_AutoCorrQuickAccess.clear();
		m_FeatCorrQuickAccess.clear();
		m_MaxItFine = 0;

		//TODO: clear config data

	}//clear

	void TemplateFitter::clearTemplate(void) {
		if (m_TemplateActive) {
			m_Template.clear();
			m_FittingResult.clear();
		}
		m_TemplateActive = false;
	}//clearTemplate

	void TemplateFitter::clearTarget(void) {
		if (m_TargetActive) m_Target.clear();
		m_TargetActive = false;
	}//clearTarget

	void TemplateFitter::clearFineFittingConfig(void) { //TODO
		m_MaxItFine = 0;

		//...

	}//clearFineFittingConfig

	void TemplateFitter::maxIterationsFineFitting(uint32_t It) {
		m_MaxItFine = It;
	}//maxIterationsFineFitting

	void TemplateFitter::computeCoarseFitting(VolumeComputation VolCompute) {
		if (m_FeatCorrQuickAccess.empty()) return; // nothing to do

		// collect feature correspondence points
		MatrixXf TemplateFeaturePoints(3, m_FeatCorrQuickAccess.size());
		MatrixXf TargetFeaturePoints(3, m_FeatCorrQuickAccess.size());

		uint32_t i = 0;
		for (const auto C : m_FeatCorrQuickAccess) {
			TemplateFeaturePoints.col(i) = m_FittingResult.vertexPosition(m_Correspondences[C].TemplateVertex);
			TargetFeaturePoints.col(i) = m_Correspondences[C].TargetPointPos;
			++i;
		}

		// compute volumetric scaling and rigid alignment

		Matrix3f S = Matrix3f::Identity();
		Matrix3f R = Matrix3f::Identity();
		Vector3f T = Vector3f::Zero();

		float ErrorStart = computeResidualError(TemplateFeaturePoints, TargetFeaturePoints, S, R, T);
		m_ErrorDeltaCoarse = -std::numeric_limits<float>::max();

		if (VolCompute != VolumeComputation::DISABLED) {
			float TemplateVol = 0.0f;
			float TargetVol = 0.0f;

			switch (VolCompute) {
			case VolumeComputation::FEATURE_AABBS: {
				TemplateVol = computeAABBVolume(TemplateFeaturePoints);
				TargetVol = computeAABBVolume(TargetFeaturePoints);
				break;
			}
			// other choices here...
			}

			float ScaleFactor = std::cbrtf(TargetVol / TemplateVol);

			S(0, 0) = ScaleFactor;
			S(1, 1) = ScaleFactor;
			S(2, 2) = ScaleFactor;

			TemplateFeaturePoints = S * TemplateFeaturePoints;
		}

		computeRigidTransformation(TemplateFeaturePoints, TargetFeaturePoints, R, T);

		m_CurrentErrorCoarse = computeResidualError(TemplateFeaturePoints, TargetFeaturePoints, S, R, T);
		m_ErrorDeltaCoarse = m_CurrentErrorCoarse - ErrorStart;

		// apply scaling and rigid alignment to vertices of m_FittingResult
		m_FittingResult.applyTransformation(S, R, T);

		for (auto& C : m_Correspondences) {
			if (C.Type == CorrespondenceType::NONE) continue;
			C.DistanceEuclidean = (m_FittingResult.vertexPosition(C.TemplateVertex) - C.TargetPointPos).norm();
		}
	}//computeCoarseFitting

	void TemplateFitter::computeFineFitting(void) { //TODO
		
	}//computeFineFitting

	void TemplateFitter::computeFineFittingIteration(void) { //TODO
		
	}//computeFineFittingIteration

	void TemplateFitter::computeRigidTransformation(const MatrixXf& Source, const MatrixXf& Target, Matrix3f& R, Vector3f& T) {
		if (Source.cols() != Target.cols()) throw CForgeExcept("Mismatch between number of source points and number of target points!");

		Vector3f SourceCoM = computePointCentroidFromColMajor(Source);
		Vector3f TargetCoM = computePointCentroidFromColMajor(Target);

		// compute matrix W
		Matrix3f W = (Target.colwise() - TargetCoM) * (Source.colwise() - SourceCoM).transpose();

		// compute singular value decomposition
		JacobiSVD<Matrix3f> SVD(W, ComputeFullU | ComputeFullV);
		Matrix3f U = SVD.matrixU();
		Matrix3f V = SVD.matrixV();

		// compute rotation and translation
		R = U * V.transpose();
		T = TargetCoM - R * SourceCoM;
	}//computeRigidTransformation

	Vector3f TemplateFitter::computePointCentroidFromColMajor(const MatrixXf& Points) {
		Vector3f CoM = Points.rowwise().sum();
		return CoM /= float(Points.cols());
	}//computePointCentroidFromColMajor

	Vector3f TemplateFitter::computePointCentroidFromRowMajor(const MatrixXf& Points) {
		Vector3f CoM = Points.colwise().sum();
		return CoM /= float(Points.rows());
	}//computePointCentroidFromRowMajor

	float TemplateFitter::computeAABBVolume(const MatrixXf& Points) {
		Vector3f PointsMin = Points.rowwise().minCoeff();
		Vector3f PointsMax = Points.rowwise().maxCoeff();

		float LengthX = PointsMax.x() - PointsMin.x();
		float LengthY = PointsMax.y() - PointsMin.y();
		float LengthZ = PointsMax.z() - PointsMin.z();

		return LengthX * LengthY * LengthZ;
	}//computeAABBVolume
	
	float TemplateFitter::computeResidualError(const MatrixXf& Source, const MatrixXf& Target) {
		if (Source.cols() != Target.cols()) throw CForgeExcept("Mismatch between number of source points and number of target points!");
		
		MatrixXf DistVecs = Target - Source;
		float Error = DistVecs.cwiseProduct(DistVecs).sum();
		Error /= float(Source.cols());

		return Error;
	}//computeResidualError

	float TemplateFitter::computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& S) {
		if (Source.cols() != Target.cols()) throw CForgeExcept("Mismatch between number of source points and number of target points!");

		MatrixXf DistVecs = Target - (S * Source);
		float Error = DistVecs.cwiseProduct(DistVecs).sum();
		Error /= float(Source.cols());

		return Error;
	}//computeResidualError

	float TemplateFitter::computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& R, const Vector3f& T) {
		if (Source.cols() != Target.cols()) throw CForgeExcept("Mismatch between number of source points and number of target points!");

		MatrixXf DistVecs = Target - ((R * Source).colwise() + T);
		float Error = DistVecs.cwiseProduct(DistVecs).sum();
		Error /= float(Source.cols());

		return Error;
	}//computeResidualError

	float TemplateFitter::computeResidualError(const MatrixXf& Source, const MatrixXf& Target, const Matrix3f& S, const Matrix3f& R, const Vector3f& T) {
		if (Source.cols() != Target.cols()) throw CForgeExcept("Mismatch between number of source points and number of target points!");

		MatrixXf DistVecs = Target - ((R * S * Source).colwise() + T);
		float Error = DistVecs.cwiseProduct(DistVecs).sum();
		Error /= float(Source.cols());

		return Error;
	}//computeResidualError
}