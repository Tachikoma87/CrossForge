#include "TriangleMeshController.h"

using namespace Eigen;

namespace crossforge {

	TriangleMeshController::TriangleMeshController(const std::string childIdentification): ControllerBase(TriangleMeshController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	TriangleMeshController::~TriangleMeshController() {

	}

	bool TriangleMeshController::recomputeVertexNormals(TriangleMeshEntityPtr pTriangleMesh) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pTriangleMesh");

		auto pPositions = pTriangleMesh->getPositionDataComponent();
		auto pMeshDefinitions = pTriangleMesh->getMeshDefinitionsComponent();

		if (nullptr == pPositions) throw MissingComponentException(PositionDataComponent::identification);
		if (nullptr == pMeshDefinitions) throw MissingComponentException(MeshDefinitionsComponent::identification);

		// compute per face normals
		std::vector<Vector3f> faceNormals;
		for (uint32_t i = 0; i < pMeshDefinitions->getMeshDefinitionsCount(); ++i) {
			auto pSubMesh = pMeshDefinitions->getMeshDefinition(i);

			for (uint32_t k = 0; k < pSubMesh->getFaceCount(); ++k) {
				const Vector3i face = pSubMesh->getFace(k);
				const Vector3f a = pPositions->getPosition(face.x()) - pPositions->getPosition(face.y());
				const Vector3f b = pPositions->getPosition(face.z()) - pPositions->getPosition(face.y());
				const Eigen::Vector3f n = a.cross(b);
				faceNormals.push_back(n.normalized());
			}
		}

		// compute per vertex normals
		std::vector<Vector3f> vertexNormals;
		vertexNormals.resize(pPositions->getPositionCount());
		for (uint32_t i = 0; i < vertexNormals.size(); ++i) vertexNormals[i] = Eigen::Vector3f::Zero();

		uint32_t faceIndex = 0;
		for (uint32_t i = 0; i < pMeshDefinitions->getMeshDefinitionsCount(); ++i) {
			auto pSubMesh = pMeshDefinitions->getMeshDefinition(i);

			for (uint32_t k = 0; k < pSubMesh->getFaceCount(); ++k) {
				const Vector3i face = pSubMesh->getFace(k);
				vertexNormals[face.x()] += faceNormals[faceIndex];
				vertexNormals[face.y()] += faceNormals[faceIndex];
				vertexNormals[face.z()] += faceNormals[faceIndex];
				faceIndex++;
			}
		}

		// normalize normals
		for (auto& normal : vertexNormals) normal.normalize();

		if (!pTriangleMesh->hasComponent(NormalDataComponent::identification)) pTriangleMesh->addComponent(std::make_shared<NormalDataComponent>());
		auto pNormalDataComponent = pTriangleMesh->getNormalDataComponent();
		pNormalDataComponent->setNormals(vertexNormals);
		return true;
		


		// sum normals
		//for (auto i : m_Submeshes) {
		//	for (uint32_t k = 0; k < i->Faces.size(); ++k) {
		//		Face* pF = &(i->Faces[k]);
		//		m_Normals[pF->Vertices[0]] = m_Normals[pF->Vertices[0]] + i->FaceNormals[k];
		//		m_Normals[pF->Vertices[1]] = m_Normals[pF->Vertices[1]] + i->FaceNormals[k];
		//		m_Normals[pF->Vertices[2]] = m_Normals[pF->Vertices[2]] + i->FaceNormals[k];
		//	}//for[faces]
		//}//for[sub meshes

		//// normalize normals
		//for (auto& i : m_Normals) i.normalize();



		//for (auto i : m_Submeshes) {
		//	i->FaceNormals.clear();
		//	for (auto k : i->Faces) {
		//		const Eigen::Vector3f a = m_Positions[k.Vertices[0]] - m_Positions[k.Vertices[2]];
		//		const Eigen::Vector3f b = m_Positions[k.Vertices[1]] - m_Positions[k.Vertices[2]];
		//		const Eigen::Vector3f n = a.cross(b);
		//		i->FaceNormals.push_back(n.normalized());
		//	}//for[faces]
		//}//for[submeshes]
	}
}