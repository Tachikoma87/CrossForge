#include "TriangleMeshController.h"
#include <crossforge/math/CrossForgeMath.h>
using namespace Eigen;

namespace crossforge {

	TriangleMeshController::TriangleMeshController(const std::string childIdentification): ControllerBase(TriangleMeshController::identification) {
		m_inheritance.push_back(childIdentification);
	}
	TriangleMeshController::~TriangleMeshController() {

	}

	bool TriangleMeshController::recomputeVertexNormals(TriangleMeshEntityPtr pTriangleMesh) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pTriangleMesh");

		auto pPositions = pTriangleMesh->getPositionsComponent();
		auto pMeshDefinitions = pTriangleMesh->getMeshDefinitionsComponent();

		if (nullptr == pPositions) throw MissingComponentException(PositionsComponent::identification);
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

		auto pNormalDataComponent = pTriangleMesh->getNormalsComponent(true);
		pNormalDataComponent->setNormals(vertexNormals);
		return true;
	}

	bool TriangleMeshController::recomputeVertexTangents(TriangleMeshEntityPtr pTriangleMesh) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pTriangleMesh");	
		auto pPositionsComp = pTriangleMesh->getPositionsComponent();
		auto pNormalsComp = pTriangleMesh->getNormalsComponent();
		auto pTexCoordsComp = pTriangleMesh->getTextureCoordinatesComponent();
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent();

		//// per face tangents
		//if (m_UVWs.size() == 0) throw CForgeExcept("No UVW coordinates. Can not compute tangents.");

		//for (auto i : m_Submeshes) {
		//	i->FaceTangents.clear();
		//	for (auto F : i->Faces) {
		//		const Eigen::Vector3f Edge1 = m_Positions[F.Vertices[1]] - m_Positions[F.Vertices[0]];
		//		const Eigen::Vector3f Edge2 = m_Positions[F.Vertices[2]] - m_Positions[F.Vertices[0]];
		//		const Eigen::Vector3f DeltaUV1 = m_UVWs[F.Vertices[1]] - m_UVWs[F.Vertices[0]];
		//		const Eigen::Vector3f DeltaUV2 = m_UVWs[F.Vertices[2]] - m_UVWs[F.Vertices[0]];

		//		float f = DeltaUV1.x() * DeltaUV2.y() - DeltaUV2.x() + DeltaUV1.y();
		//		f = (std::abs(f) > 0.0f) ? 1.0f / f : 1.0f;

		//		Eigen::Vector3f Tangent;
		//		Tangent.x() = f * (DeltaUV2.y() * Edge1.x() - DeltaUV1.y() * Edge2.x());
		//		Tangent.y() = f * (DeltaUV2.y() * Edge1.y() - DeltaUV1.y() * Edge2.y());
		//		Tangent.z() = f * (DeltaUV2.y() * Edge1.z() - DeltaUV1.y() * Edge2.z());
		//		i->FaceTangents.push_back(Tangent);
		//	}//for[all faces]
		//}//for[all submeshes]

		//// per vertex tangents
		//if (ComputePerFaceTangents) computePerFaceTangents();
		//m_Tangents.clear();
		//// create tangents
		//for (uint32_t i = 0; i < m_Positions.size(); ++i) m_Tangents.push_back(Eigen::Vector3f::Zero());

		//// sum tangents
		//for (auto i : m_Submeshes) {
		//	for (uint32_t k = 0; k < i->Faces.size(); ++k) {
		//		Face* pF = &(i->Faces[k]);
		//		m_Tangents[pF->Vertices[0]] += i->FaceTangents[k];
		//		m_Tangents[pF->Vertices[1]] += i->FaceTangents[k];
		//		m_Tangents[pF->Vertices[2]] += i->FaceTangents[k];
		//	}//for[all faces]
		//}//for[submeshes]
		//// normalize tangents
		//for (auto& i : m_Tangents) i.normalize();



	}


	void TriangleMeshController::plane(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f dimensions, Eigen::Vector2i segments, bool twoSided) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pTriangleMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;

		Vector3f scale = Vector3f(dimensions.x(), 0.0f, dimensions.y());
		Vector3f offset = Vector3f(-0.5f, 0.0f, 0.5f).cwiseProduct(scale);

		// generate vertices
		Vector3f v = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();
		for (uint32_t i = 0; i <= segments.y(); i++) {
			for (uint32_t j = 0; j <= segments.x(); j++) {
				vertices.push_back(offset + v.cwiseProduct(scale));
				v.x() += 1.0 / segments.x();

				uvw.x() = float(j) / segments.x();
				uvw.y() = float(i) / segments.y();
				uvws.push_back(uvw);
			}
			v.x() = 0.0f;
			v.z() -= 1.0f / segments.y();
		}

		// generate triangles
		for (uint32_t i = 0; i < segments.y(); ++i) {
			for (uint32_t j = 0; j < segments.x(); ++j) {
				auto v0 = j + i * (segments.x() + 1);
				auto v1 = v0 + segments.x() + 1;
				auto v2 = v0 + segments.x() + 2;
				auto v3 = v0 + 1;

			
				faces.push_back(Eigen::Vector3i(v0, v2, v1));
				faces.push_back(Eigen::Vector3i(v0, v3, v2));
				if (twoSided) {
					faces.push_back(Eigen::Vector3i(v0, v1, v2));
					faces.push_back(Eigen::Vector3i(v0, v2, v3));
				}
			}
		}

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);

		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);

		pMaterials->addMaterial(pMeshMaterial);



	}//plane

	void TriangleMeshController::circle(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f dimensions, uint32_t slices, float tipOffset, bool twoSided) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pTriangleMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		if (slices < 3) slices = 3;

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;
		Vector3f v;
		Vector3f uvw;

		// generate vertices
		for (uint32_t i = 0; i < slices; ++i) {
			float Ratio = float(i) / float(slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(Ratio) * dimensions.x() * 0.5f;
			v.y() = 0.0f;
			v.z() = std::sin(Ratio) * dimensions.y() * 0.5f;
			vertices.push_back(v);

			uvw.x() = (std::cos(Ratio) + 1.0f) / 2.0f;
			uvw.y() = (std::sin(Ratio) + 1.0f) / 2.0f;
			uvws.push_back(uvw);

		}//for[slices]

		vertices.push_back(Vector3f(0.0f, 0.0f + tipOffset, 0.0f));
		uvws.push_back(Vector3f(0.5f, 0.5f, 0.0f));

		// add faces around the cylinder
		for (uint32_t i = 0; i < slices; ++i) {
			Eigen::Vector3i face = Eigen::Vector3i(vertices.size() - 1, (i + 1) % slices, i);
			faces.push_back(face);
			if (twoSided) faces.push_back(Eigen::Vector3i(face.x(), face.z(), face.y()));
		}

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);

	}//circle


	uint32_t cuboidVertexID(uint32_t x, uint32_t y, uint32_t z, Vector3i Segments) {
		return (z * (Segments.x() + 1) * (Segments.y() + 1)) + (y * (Segments.x() + 1)) + x;
	}


	void TriangleMeshController::cuboid(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimensions, Eigen::Vector3i segments) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		// create vertices
		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;

		Vector3f scale = Vector3f(dimensions.x() / segments.x(), dimensions.y() / segments.y(), dimensions.z() / segments.z());
		Vector3f offset = dimensions / 2.0f;
		offset.z() *= -1.0f;

		for (uint32_t z = 0; z <= segments.z(); ++z) {
			for (uint32_t y = 0; y <= segments.y(); ++y) {
				for (uint32_t x = 0; x <= segments.x(); x++) {
					const Vector3f v = Vector3f(x * scale.x(), y * scale.y(), -float(z) * scale.z());
					vertices.push_back(v - offset);

					Vector3f uvw;
					float uu, vv;
					if (x >= 0 && z == 0)
					{
						uu = float(x) / segments.x();
						vv = float(y) / segments.y();
					}
					else if (z >= 0 && x == 0)
					{
						uu = float(z) / segments.z();
						vv = float(y) / segments.y();
					}
					else if (z >= 0 && x == segments.x()) {
						uu = 1.0f - float(z) / segments.z();
						vv = float(y) / segments.y();
					}
					else if (x >= 0 && z == segments.z()) {
						uu = 1.0f - float(x) / segments.x();
						vv = float(y) / segments.y();
					}
					else if (x >= 0 && y == 0) {
						uu = float(x) / segments.x();
						vv = float(z) / segments.z();
					}
					else
					{
						uu = float(x) / segments.x();
						vv = 1.0f - float(z) / segments.z();
					}
					uvw = Vector3f(uu, vv, 0.0f);
					uvws.push_back(uvw);
				}//for[z]
			}//for[y]
		}//for[x]


		for (uint32_t x = 0; x < segments.x(); ++x) {
			for (uint32_t y = 0; y < segments.y(); ++y) {
				// create front and back faces
				Eigen::Vector3i f;
				f.x() = cuboidVertexID(x, y, 0, segments);
				f.y() = cuboidVertexID(x + 1, y, 0, segments);
				f.z() = cuboidVertexID(x + 1, y + 1, 0, segments);
				faces.push_back(f);
				
				f.x() = cuboidVertexID(x, y, 0, segments);
				f.y() = cuboidVertexID(x + 1, y + 1, 0, segments);
				f.z() = cuboidVertexID(x, y + 1, 0, segments);
				faces.push_back(f);

				f.x() = cuboidVertexID(x, y, segments.z(), segments);
				f.y() = cuboidVertexID(x + 1, y + 1, segments.z(), segments);
				f.z() = cuboidVertexID(x + 1, y, segments.z(), segments);
				faces.push_back(f);
				
				f.x() = cuboidVertexID(x, y, segments.z(), segments);
				f.y() = cuboidVertexID(x, y + 1, segments.z(), segments);
				f.z() = cuboidVertexID(x + 1, y + 1, segments.z(), segments);
				faces.push_back(f);
			}//For[y]

			for (uint32_t z = 0; z < segments.z(); ++z) {
				// create bottom and top faces
				Eigen::Vector3i f;
				f.x() = cuboidVertexID(x, 0, z, segments);
				f.y() = cuboidVertexID(x, 0, z + 1, segments);
				f.z() = cuboidVertexID(x + 1, 0, z, segments);
				faces.push_back(f);
	
				f.x() = cuboidVertexID(x, 0, z + 1, segments);
				f.y() = cuboidVertexID(x + 1, 0, z + 1, segments);
				f.z() = cuboidVertexID(x + 1, 0, z, segments);
				faces.push_back(f);

				f.x() = cuboidVertexID(x, segments.y(), z, segments);
				f.y() = cuboidVertexID(x + 1, segments.y(), z, segments);
				f.z() = cuboidVertexID(x, segments.y(), z + 1, segments);
				faces.push_back(f);

				f.x() = cuboidVertexID(x, segments.y(), z + 1, segments);
				f.y() = cuboidVertexID(x + 1, segments.y(), z, segments);
				f.z() = cuboidVertexID(x + 1, segments.y(), z + 1, segments);
				faces.push_back(f);
			}//for[z]
		}//for[x]

		// create left and right
		for (uint32_t z = 0; z < segments.z(); z++) {
			for (uint32_t y = 0; y < segments.y(); y++) {
				Eigen::Vector3i f;
				f.x() = cuboidVertexID(0, y, z, segments);
				f.y() = cuboidVertexID(0, y + 1, z, segments);
				f.z() = cuboidVertexID(0, y, z + 1, segments);
				faces.push_back(f);
	
				f.x() = cuboidVertexID(0, y, z + 1, segments);
				f.y() = cuboidVertexID(0, y + 1, z, segments);
				f.z() = cuboidVertexID(0, y + 1, z + 1, segments);
				faces.push_back(f);

				f.x() = cuboidVertexID(segments.x(), y, z, segments);
				f.y() = cuboidVertexID(segments.x(), y, z + 1, segments);
				f.z() = cuboidVertexID(segments.x(), y + 1, z, segments);
				faces.push_back(f);

				f.x() = cuboidVertexID(segments.x(), y, z + 1, segments);
				f.y() = cuboidVertexID(segments.x(), y + 1, z + 1, segments);
				f.z() = cuboidVertexID(segments.x(), y + 1, z, segments);
				faces.push_back(f);
			}//for[y]
		}//for[z]

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);
	}//cuboid


	void TriangleMeshController::uvSphere(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimensions, uint32_t slices, uint32_t stacks) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		if (stacks < 3) stacks = 3;
		if (slices < 3) slices = 3;

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;
		Eigen::Vector3f v;
		//Eigen::Vector3f uvw = Vector3f::Zero();
		Eigen::Vector2f uv;

		// add top vertex
		vertices.push_back(Vector3f(0.0f, 1.0f, 0.0f).cwiseProduct(0.5f * dimensions));
		uv = CrossForgeMath::equalAreaMapping(Vector3f(0.0f, 1.0f, 0.0f));
		uvws.push_back(Vector3f(uv.x(), uv.y(), 0.0f));

		// generate vertices per stack/slice
		for (uint32_t i = 0; i < stacks; i++) {
			double Phi = EIGEN_PI * double(i + 1) / double(stacks);
			for (uint32_t j = 0; j < slices; j++) {

				double Theta = 2.0 * EIGEN_PI * double(j) / double(slices);
				v.x() = std::sin(Phi) * std::cos(Theta);
				v.y() = std::cos(Phi);
				v.z() = std::sin(Phi) * std::sin(Theta);
				vertices.push_back(v.cwiseProduct(0.5f * dimensions));

				v.z() = -v.z();
				uv = CrossForgeMath::equalAreaMapping(v);
				uvws.push_back(Vector3f(uv.x(), uv.y(), 0.0f));
			}//for[slices]
		}//for[stacks]

		// add bottom vertex
		vertices.push_back(Vector3f(0.0f, -1.0f, 0.0f).cwiseProduct(0.5f * dimensions));
		uv = CrossForgeMath::equalAreaMapping(Vector3f(0.0f, -1.0f, 0.0f));
		uvws.push_back(Vector3f(uv.x(), uv.y(), 0.0f));

		// add top/bottom triangles
		for (uint32_t i = 0; i < slices; ++i) {
			uint32_t i0 = i + 1;
			uint32_t i1 = (i + 1) % slices + 1;
			faces.push_back(Vector3i(0, i1, i0));
			i0 = i + slices * (stacks - 2) + 1;
			i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
			faces.push_back(Vector3i(vertices.size() - 1, i0, i1));
		}

		// add triangles per stack/slice
		for (uint32_t j = 0; j < stacks - 2; ++j) {
			uint32_t j0 = j * slices + 1;
			uint32_t j1 = (j + 1) * slices + 1;
			for (uint32_t i = 0; i < slices; ++i) {
				uint32_t i0 = j0 + i;
				uint32_t i1 = j0 + (i + 1) % slices;
				uint32_t i2 = j1 + (i + 1) % slices;
				uint32_t i3 = j1 + i;
				faces.push_back(Vector3i(i0, i1, i2));
				faces.push_back(Vector3i(i0, i2, i3));
			}
		}//for[stacks]

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);
	}//uvSphere


	void TriangleMeshController::cone(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimensions, uint32_t slices) {
		doubleCone(pTriangleMesh, Vector4f(dimensions.x(), dimensions.y(), dimensions.z(), 0.0f), slices);
	}//Cone

	void TriangleMeshController::doubleCone(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector4f dimensions, uint32_t slices) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;
		
		Vector3f v = Vector3f::Zero();
		Vector2f uv = Vector2f::Zero();

		Vector3f scale = Vector3f(dimensions.x(), 0.0f, dimensions.z());

		// add vertics subdividing a circle
		for (uint32_t i = 0; i < slices; ++i) {
			float Ratio = float(i) / float(slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(Ratio);
			v.z() = std::sin(Ratio);
			vertices.push_back(v.cwiseProduct(0.5f * scale));

			uv = CrossForgeMath::equirectangularMapping(v);
			uvws.push_back(Vector3f(uv.x(), uv.y(), 0.0f));
		}
		// add the tip of the cone
		vertices.push_back(Vector3f(0.0f, dimensions.y(), 0.0f));
		// add bottom vertex at the center of the circle
		vertices.push_back(Vector3f(0.0f, -dimensions.w(), 0.0f));
		uvws.push_back(Vector3f(0.5f, 1.0f, 0.0f));
		uvws.push_back(Vector3f(0.5f, 1.0f, 0.0f));

		// generate triangular faces
		for (uint32_t i = 0; i < slices; ++i) {
			faces.push_back(Vector3i(vertices.size() - 2, (i + 1) % slices, i));
			faces.push_back(Vector3i(vertices.size() - 1, i, (i + 1) % slices));
		}


		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);
	}//doubleCone

	void TriangleMeshController::cylinder(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f topDimensions, Eigen::Vector2f bottomDimensions, float height, uint32_t slices, Eigen::Vector2f tipOffsets) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		if (slices < 3) slices = 3;

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;
		
		Vector3f v = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();

		Vector3f scaleBottom = Vector3f(bottomDimensions.x(), 1.0f, bottomDimensions.y());
		Vector3f scaleTop = Vector3f(topDimensions.x(), 1.0f, topDimensions.y());

		// generate vertices
		for (uint32_t i = 0; i < slices; ++i) {
			float ratio = float(i) / float(slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(ratio) * bottomDimensions.x() * 0.5f;
			v.y() = 0.0f;
			v.z() = std::sin(ratio) * bottomDimensions.y() * 0.5f;
			vertices.push_back(v);

			v.x() = std::cos(ratio) * topDimensions.x() * 0.5f;
			v.y() = height;
			v.z() = std::sin(ratio) * topDimensions.y() * 0.5f;
			vertices.push_back(v);

			uvw.x() = float(i) / float(slices - 1);
			uvw.y() = 0.0f;
			uvws.push_back(uvw);
			uvw.y() = 1.0f;
			uvws.push_back(uvw);

		}//for[slices]

		vertices.push_back(Vector3f(0.0f, height + tipOffsets.x(), 0.0f));
		vertices.push_back(Vector3f(0.0f, 0.0f - tipOffsets.y(), 0.0f));

		uvws.push_back(Vector3f(0.5f, 0.5f, 0.0f));
		uvws.push_back(Vector3f(0.5f, 0.5f, 0.0f));

		// add faces around the cylinder
		for (uint32_t i = 0; i < slices; ++i) {
			auto ii = i * 2;
			auto jj = (ii + 2) % (slices * 2);
			auto kk = (ii + 3) % (slices * 2);
			auto ll = ii + 1;
			faces.push_back(Vector3i(ii, kk, jj));
			faces.push_back(Vector3i(ii, ll, kk));

			// bottom faces
			faces.push_back(Vector3i(ii, jj, vertices.size() - 1));
			// top faces
			faces.push_back(Vector3i(kk, ll, vertices.size() - 2));

		}//for[slices]

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);
	}//cylinder

	void TriangleMeshController::torus(TriangleMeshEntityPtr pTriangleMesh, float radius, float thickness, uint32_t slices, uint32_t stacks) {
		if (nullptr == pTriangleMesh) throw NullpointerExcept("pMesh");
		pTriangleMesh->clear();
		pTriangleMesh->initialize();

		std::vector<Vector3f> vertices;
		std::vector<Vector3f> uvws;
		std::vector<Vector3i> faces;
		Vector3f vert = Vector3f::Zero();
		Vector2f uv = Vector2f::Zero();

		// generate vertices
		for (uint32_t i = 0; i < stacks; ++i) {
			for (uint32_t j = 0; j < slices; ++j) {
				float u = float(j) / slices * EIGEN_PI * 2.0f;
				float v = float(i) / stacks * EIGEN_PI * 2.0f;
				vert.x() = (radius + thickness * std::cos(v)) * std::cos(u);
				vert.y() = thickness * std::sin(v);
				vert.z() = (radius + thickness * std::cos(v)) * std::sin(u);
				vertices.push_back(vert);
				vert.x() = (1.0f + 0.5f * std::cos(v)) * std::cos(u);
				vert.y() = 0.5f * std::sin(v);
				vert.z() = (1.0f + 0.5f * std::cos(v)) * std::sin(u);
				uv = CrossForgeMath::equirectangularMapping(vert);
				uvws.push_back(Eigen::Vector3f(uv.x(), uv.y(), 0.0f));
			}
		}

		// add faces
		for (uint32_t i = 0; i < stacks; ++i) {
			auto iNext = (i + 1) % stacks;
			for (uint32_t j = 0; j < slices; ++j) {
				auto jNext = (j + 1) % slices;
				auto i0 = i * slices + j;
				auto i1 = i * slices + jNext;
				auto i2 = iNext * slices + jNext;
				auto i3 = iNext * slices + j;
				faces.push_back(Eigen::Vector3i(i0, i2, i1));
				faces.push_back(Eigen::Vector3i(i0, i3, i2));
			}
		}

		auto pPositionComp = pTriangleMesh->getPositionsComponent(true);
		auto pTextureCoordinatesComp = pTriangleMesh->getTextureCoordinatesComponent(true);
		auto pMeshDefinitionsComp = pTriangleMesh->getMeshDefinitionsComponent(true);
		auto pMaterials = pTriangleMesh->getMeshMaterialsComponent(true);

		pPositionComp->setPositions(vertices);
		pTextureCoordinatesComp->setTextureCoordinates(uvws);
		MeshDefinitionPtr pMeshDef = std::make_shared<MeshDefinition>();
		pMeshDef->setFaces(faces);
		pMeshDef->setMaterialIndex(0);
		pMeshDefinitionsComp->addMeshDefinition(pMeshDef);
		auto pMeshMaterial = std::make_shared<MeshMaterial>();
		pMeshMaterial->setColor(Eigen::Vector4f::Ones(), MeshMaterial::COLOR_TYPE_AMBIENT);
		pMaterials->addMaterial(pMeshMaterial);
	}//Torus
	
}