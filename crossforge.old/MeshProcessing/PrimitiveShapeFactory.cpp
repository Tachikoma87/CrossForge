#include "PrimitiveShapeFactory.h"
#include "../Math/CForgeMath.h"

using namespace Eigen;

namespace CForge {

	void PrimitiveShapeFactory::plane(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, Eigen::Vector2i Segments) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		T3DMesh<float>::Face F;

		Vector3f Scale = Vector3f(Dimensions.x(), 0.0f, Dimensions.y());
		Vector3f Offset = Vector3f(-0.5f, 0.0f, 0.5f).cwiseProduct(Scale);

		// generate vertices
		Vector3f v = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();
		for (uint32_t i = 0; i <= Segments.y(); i++) {
			for (uint32_t j = 0; j <= Segments.x(); j++) {
				Vertices.push_back(Offset + v.cwiseProduct(Scale));
				v.x() += 1.0 / Segments.x();

				uvw.x() = float(j) / Segments.x();
				uvw.y() = float(i) / Segments.y();
				UVWs.push_back(uvw);
			}
			v.x() = 0.0f;
			v.z() -= 1.0f / Segments.y();
		}

		// generate triangles
		for (uint32_t i = 0; i < Segments.y(); ++i) {
			for (uint32_t j = 0; j < Segments.x(); ++j) {
				auto v0 = j + i * (Segments.x() + 1);
				auto v1 = v0 + Segments.x() + 1;
				auto v2 = v0 + Segments.x() + 2;
				auto v3 = v0 + 1;
				F.Vertices[0] = v0;
				F.Vertices[2] = v1;
				F.Vertices[1] = v2;
				Sub.Faces.push_back(F);
				F.Vertices[0] = v0;
				F.Vertices[2] = v2;
				F.Vertices[1] = v3;
				Sub.Faces.push_back(F);

			}
		}


		Mat.Color = Vector4f::Ones();
		Sub.Material = 0;
		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//plane

	void PrimitiveShapeFactory::circle(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, uint32_t Slices, float TipOffset) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		if (Slices < 3) Slices = 3;

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		T3DMesh<float>::Face F;
		Vector3f v;
		Vector3f uvw;

		// generate vertices
		for (uint32_t i = 0; i < Slices; ++i) {
			float Ratio = float(i) / float(Slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(Ratio) * Dimensions.x() * 0.5f;
			v.y() = 0.0f;
			v.z() = std::sin(Ratio) * Dimensions.y() * 0.5f;
			Vertices.push_back(v);

			uvw.x() = (std::cos(Ratio) + 1.0f) / 2.0f;
			uvw.y() = (std::sin(Ratio) + 1.0f) / 2.0f;
			UVWs.push_back(uvw);

		}//for[slices]

		Vertices.push_back(Vector3f(0.0f, 0.0f + TipOffset, 0.0f));
		UVWs.push_back(Vector3f(0.5f, 0.5f, 0.0f));

		// add faces around the cylinder
		for (uint32_t i = 0; i < Slices; ++i) {
			F.Vertices[0] = Vertices.size() - 1;
			F.Vertices[1] = (i + 1) % Slices;
			F.Vertices[2] = i;
			Sub.Faces.push_back(F);
		}


		Mat.Color = Vector4f::Ones();
		Sub.Material = 0;
		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//circle


	uint32_t cuboidVertexID(uint32_t x, uint32_t y, uint32_t z, Vector3i Segments) {
		return (z * (Segments.x() + 1) * (Segments.y() + 1)) + (y * (Segments.x() + 1)) + x;
	}


	void PrimitiveShapeFactory::cuboid(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, Eigen::Vector3i Segments) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();
		// create vertices
		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		Mat.Color = Vector4f::Ones();

		Vector3f Scale = Vector3f(Dimensions.x() / Segments.x(), Dimensions.y() / Segments.y(), Dimensions.z() / Segments.z());
		Vector3f Offset = Dimensions / 2.0f;
		Offset.z() *= -1.0f;

		for (uint32_t z = 0; z <= Segments.z(); ++z) {
			for (uint32_t y = 0; y <= Segments.y(); ++y) {
				for (uint32_t x = 0; x <= Segments.x(); x++) {
					const Vector3f v = Vector3f(x * Scale.x(), y * Scale.y(), -float(z) * Scale.z());
					Vertices.push_back(v - Offset);

					Vector3f uvw;
					float uu, vv;
					if (x >= 0 && z == 0)
					{
						uu = float(x) / Segments.x();
						vv = float(y) / Segments.y();
					}
					else if (z >= 0 && x == 0)
					{
						uu = float(z) / Segments.z();
						vv = float(y) / Segments.y();
					}
					else if (z >= 0 && x == Segments.x()) {
						uu = 1.0f - float(z) / Segments.z();
						vv = float(y) / Segments.y();
					}
					else if (x >= 0 && z == Segments.z()) {
						uu = 1.0f - float(x) / Segments.x();
						vv = float(y) / Segments.y();
					}
					else if (x >= 0 && y == 0) {
						uu = float(x) / Segments.x();
						vv = float(z) / Segments.z();
					}
					else
					{
						uu = float(x) / Segments.x();
						vv = 1.0f - float(z) / Segments.z();
					}
					uvw = Vector3f(uu, vv, 0.0f);
					UVWs.push_back(uvw);
				}//for[z]
			}//for[y]
		}//for[x]


		for (uint32_t x = 0; x < Segments.x(); ++x) {
			for (uint32_t y = 0; y < Segments.y(); ++y) {
				// create front and back faces
				T3DMesh<float>::Face F;
				F.Vertices[0] = cuboidVertexID(x, y, 0, Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, y, 0, Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, y + 1, 0, Segments);
				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(x, y, 0, Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, y + 1, 0, Segments);
				F.Vertices[2] = cuboidVertexID(x, y + 1, 0, Segments);
				Sub.Faces.push_back(F);

				F.Vertices[0] = cuboidVertexID(x, y, Segments.z(), Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, y + 1, Segments.z(), Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, y, Segments.z(), Segments);
				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(x, y, Segments.z(), Segments);
				F.Vertices[1] = cuboidVertexID(x, y + 1, Segments.z(), Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, y + 1, Segments.z(), Segments);
				Sub.Faces.push_back(F);
			}//For[y]

			for (uint32_t z = 0; z < Segments.z(); ++z) {
				// create bottom and top faces
				T3DMesh<float>::Face F;
				F.Vertices[0] = cuboidVertexID(x, 0, z, Segments);
				F.Vertices[1] = cuboidVertexID(x, 0, z + 1, Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, 0, z, Segments);
				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(x, 0, z + 1, Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, 0, z + 1, Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, 0, z, Segments);

				Sub.Faces.push_back(F);

				F.Vertices[0] = cuboidVertexID(x, Segments.y(), z, Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, Segments.y(), z, Segments);
				F.Vertices[2] = cuboidVertexID(x, Segments.y(), z + 1, Segments);
				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(x, Segments.y(), z + 1, Segments);
				F.Vertices[1] = cuboidVertexID(x + 1, Segments.y(), z, Segments);
				F.Vertices[2] = cuboidVertexID(x + 1, Segments.y(), z + 1, Segments);
				Sub.Faces.push_back(F);
			}//for[z]
		}//for[x]

		// create left and right
		for (uint32_t z = 0; z < Segments.z(); z++) {
			for (uint32_t y = 0; y < Segments.y(); y++) {
				T3DMesh<float>::Face F;
				F.Vertices[0] = cuboidVertexID(0, y, z, Segments);
				F.Vertices[1] = cuboidVertexID(0, y + 1, z, Segments);
				F.Vertices[2] = cuboidVertexID(0, y, z + 1, Segments);
				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(0, y, z + 1, Segments);
				F.Vertices[1] = cuboidVertexID(0, y + 1, z, Segments);
				F.Vertices[2] = cuboidVertexID(0, y + 1, z + 1, Segments);
				Sub.Faces.push_back(F);

				F.Vertices[0] = cuboidVertexID(Segments.x(), y, z, Segments);
				F.Vertices[1] = cuboidVertexID(Segments.x(), y, z + 1, Segments);
				F.Vertices[2] = cuboidVertexID(Segments.x(), y + 1, z, Segments);

				Sub.Faces.push_back(F);
				F.Vertices[0] = cuboidVertexID(Segments.x(), y, z + 1, Segments);
				F.Vertices[1] = cuboidVertexID(Segments.x(), y + 1, z + 1, Segments);
				F.Vertices[2] = cuboidVertexID(Segments.x(), y + 1, z, Segments);
				Sub.Faces.push_back(F);
			}//for[y]
		}//for[z]

		Sub.Material = 0;

		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//cuboid


	void PrimitiveShapeFactory::uvSphere(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices, uint32_t Stacks) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		if (Stacks < 3) Stacks = 3;
		if (Slices < 3) Slices = 3;

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Face F;
		T3DMesh<float>::Material Mat;
		Eigen::Vector3f v;
		Eigen::Vector3f uvw = Vector3f::Zero();

		// add top vertex
		Vertices.push_back(Vector3f(0.0f, 1.0f, 0.0f).cwiseProduct(0.5f*Dimensions));
		uvw = CForgeMath::equalAreaMapping(Vector3f(0.0f, 1.0f, 0.0f));
		UVWs.push_back(uvw);

		// generate vertices per stack/slice
		for (uint32_t i = 0; i < Stacks; i++) {
			double Phi = EIGEN_PI * double(i + 1) / double(Stacks);
			for (uint32_t j = 0; j < Slices; j++) {

				double Theta = 2.0 * EIGEN_PI * double(j) / double(Slices);
				v.x() = std::sin(Phi) * std::cos(Theta);
				v.y() = std::cos(Phi);
				v.z() = std::sin(Phi) * std::sin(Theta);
				Vertices.push_back(v.cwiseProduct(0.5f*Dimensions));

				v.z() = -v.z();
				uvw = CForgeMath::equalAreaMapping(v);
				UVWs.push_back(uvw);
			}//for[slices]
		}//for[stacks]

		// add bottom vertex
		Vertices.push_back(Vector3f(0.0f, -1.0f, 0.0f).cwiseProduct(0.5f*Dimensions));
		uvw = CForgeMath::equalAreaMapping(Vector3f(0.0f, -1.0f, 0.0f));
		UVWs.push_back(uvw);

		// add top/bottom triangles
		for (uint32_t i = 0; i < Slices; ++i) {
			uint32_t i0 = i + 1;
			uint32_t i1 = (i + 1) % Slices + 1;
			F.Vertices[0] = 0;
			F.Vertices[1] = i1;
			F.Vertices[2] = i0;
			Sub.Faces.push_back(F);
			i0 = i + Slices * (Stacks - 2) + 1;
			i1 = (i + 1) % Slices + Slices * (Stacks - 2) + 1;
			F.Vertices[0] = Vertices.size() - 1;
			F.Vertices[1] = i0;
			F.Vertices[2] = i1;
			Sub.Faces.push_back(F);
		}

		// add triangles per stack/slice
		for (uint32_t j = 0; j < Stacks - 2; ++j) {
			uint32_t j0 = j * Slices + 1;
			uint32_t j1 = (j + 1) * Slices + 1;
			for (uint32_t i = 0; i < Slices; ++i) {
				uint32_t i0 = j0 + i;
				uint32_t i1 = j0 + (i + 1) % Slices;
				uint32_t i2 = j1 + (i + 1) % Slices;
				uint32_t i3 = j1 + i;
				F.Vertices[0] = i0;
				F.Vertices[1] = i1;
				F.Vertices[2] = i2;
				Sub.Faces.push_back(F);
				F.Vertices[0] = i0;
				F.Vertices[1] = i2;
				F.Vertices[2] = i3;
				Sub.Faces.push_back(F);
			}
		}//for[stacks]

		Sub.Material = 0;
		Mat.Color = Vector4f::Ones();
		Mat.Metallic = 0.04f;
		Mat.Roughness = 0.1f;

		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//uvSphere

	void PrimitiveShapeFactory::cone(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices) {
		doubleCone(pMesh, Vector4f(Dimensions.x(), Dimensions.y(), Dimensions.z(), 0.0f), Slices);
	}//Cone

	void PrimitiveShapeFactory::doubleCone(T3DMesh<float>* pMesh, Eigen::Vector4f Dimensions, uint32_t Slices) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		T3DMesh<float>::Face F;
		Vector3f v = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();

		Vector3f Scale = Vector3f(Dimensions.x(), 0.0f, Dimensions.z());

		// add vertics subdividing a circle
		for (uint32_t i = 0; i < Slices; ++i) {
			float Ratio = float(i) / float(Slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(Ratio);
			v.z() = std::sin(Ratio);
			Vertices.push_back(v.cwiseProduct(0.5f*Scale));

			uvw = CForgeMath::equirectangularMapping(v);
			UVWs.push_back(uvw);
		}
		// add the tip of the cone
		Vertices.push_back(Vector3f(0.0f, Dimensions.y(), 0.0f));
		// add bottom vertex at the center of the circle
		Vertices.push_back(Vector3f(0.0f, -Dimensions.w(), 0.0f));
		UVWs.push_back(Vector3f(0.5f, 1.0f, 0.0f));
		UVWs.push_back(Vector3f(0.5f, 1.0f, 0.0f));

		// generate triangular faces
		for (uint32_t i = 0; i < Slices; ++i) {
			F.Vertices[0] = Vertices.size() - 2;
			F.Vertices[1] = (i + 1) % Slices;
			F.Vertices[2] = i;
			Sub.Faces.push_back(F);
			F.Vertices[0] = Vertices.size() - 1;
			F.Vertices[1] = i;
			F.Vertices[2] = (i + 1) % Slices;
			Sub.Faces.push_back(F);
		}


		Mat.Color = Vector4f::Ones();
		Sub.Material = 0;
		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//doubleCone

	void PrimitiveShapeFactory::cylinder(T3DMesh<float>* pMesh, Eigen::Vector2f TopDimensions, Eigen::Vector2f BottomDimensions, float Height, uint32_t Slices, Eigen::Vector2f TipOffsets) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		if (Slices < 3) Slices = 3;

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		T3DMesh<float>::Face F;
		Vector3f v = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();

		Vector3f ScaleBottom = Vector3f(BottomDimensions.x(), 1.0f, BottomDimensions.y());
		Vector3f ScaleTop = Vector3f(TopDimensions.x(), 1.0f, TopDimensions.y());

		// generate vertices
		for (uint32_t i = 0; i < Slices; ++i) {
			float Ratio = float(i) / float(Slices) * (EIGEN_PI * 2.0f);
			v.x() = std::cos(Ratio) * BottomDimensions.x() * 0.5f;
			v.y() = 0.0f;
			v.z() = std::sin(Ratio) * BottomDimensions.y() * 0.5f;
			Vertices.push_back(v);

			v.x() = std::cos(Ratio) * TopDimensions.x() * 0.5f;
			v.y() = Height;
			v.z() = std::sin(Ratio) * TopDimensions.y() * 0.5f;
			Vertices.push_back(v);

			uvw.x() = float(i) / float(Slices - 1);
			uvw.y() = 0.0f;
			UVWs.push_back(uvw);
			uvw.y() = 1.0f;
			UVWs.push_back(uvw);

		}//for[slices]

		Vertices.push_back(Vector3f(0.0f, Height + TipOffsets.x(), 0.0f));
		Vertices.push_back(Vector3f(0.0f, 0.0f - TipOffsets.y(), 0.0f));

		UVWs.push_back(Vector3f(0.5f, 0.5f, 0.0f));
		UVWs.push_back(Vector3f(0.5f, 0.5f, 0.0f));

		// add faces around the cylinder
		for (uint32_t i = 0; i < Slices; ++i) {
			auto ii = i * 2;
			auto jj = (ii + 2) % (Slices * 2);
			auto kk = (ii + 3) % (Slices * 2);
			auto ll = ii + 1;
			F.Vertices[0] = ii;
			F.Vertices[1] = kk;
			F.Vertices[2] = jj;
			Sub.Faces.push_back(F);
			F.Vertices[0] = ii;
			F.Vertices[1] = ll;
			F.Vertices[2] = kk;
			Sub.Faces.push_back(F);

			// bottom faces
			F.Vertices[0] = ii;
			F.Vertices[1] = jj;
			F.Vertices[2] = Vertices.size() - 1;
			Sub.Faces.push_back(F);
			// top faces
			F.Vertices[0] = kk;
			F.Vertices[1] = ll;
			F.Vertices[2] = Vertices.size() - 2;
			Sub.Faces.push_back(F);

		}//for[slices]

		Mat.Color = Vector4f::Ones();
		Sub.Material = 0;
		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//cylinder

	void PrimitiveShapeFactory::torus(T3DMesh<float>* pMesh, float Radius, float Thickness, uint32_t Slices, uint32_t Stacks) {
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");
		pMesh->clear();

		std::vector<Vector3f> Vertices;
		std::vector<Vector3f> UVWs;
		T3DMesh<float>::Submesh Sub;
		T3DMesh<float>::Material Mat;
		T3DMesh<float>::Face F;
		Vector3f V = Vector3f::Zero();
		Vector3f uvw = Vector3f::Zero();

		// generate vertices
		for (uint32_t i = 0; i < Stacks; ++i) {
			for (uint32_t j = 0; j < Slices; ++j) {
				float u = float(j) / Slices * EIGEN_PI * 2.0f;
				float v = float(i) / Stacks * EIGEN_PI * 2.0f;
				V.x() = (Radius + Thickness * std::cos(v)) * std::cos(u);
				V.y() = Thickness * std::sin(v);
				V.z() = (Radius + Thickness * std::cos(v)) * std::sin(u);
				Vertices.push_back(V);
				V.x() = (1.0f + 0.5f * std::cos(v)) * std::cos(u);
				V.y() = 0.5f * std::sin(v);
				V.z() = (1.0f + 0.5f * std::cos(v)) * std::sin(u);
				uvw = CForgeMath::equirectangularMapping(V);
				UVWs.push_back(uvw);
			}
		}

		// add faces
		for (uint32_t i = 0; i < Stacks; ++i) {
			auto iNext = (i + 1) % Stacks;
			for (uint32_t j = 0; j < Slices; ++j) {
				auto jNext = (j + 1) % Slices;
				auto i0 = i * Slices + j;
				auto i1 = i * Slices + jNext;
				auto i2 = iNext * Slices + jNext;
				auto i3 = iNext * Slices + j;
				F.Vertices[0] = i0;
				F.Vertices[1] = i2;
				F.Vertices[2] = i1;
				Sub.Faces.push_back(F);
				F.Vertices[0] = i0;
				F.Vertices[1] = i3;
				F.Vertices[2] = i2;
				Sub.Faces.push_back(F);
			}
		}

		Mat.Color = Vector4f::Ones();
		Sub.Material = 0;
		pMesh->vertices(&Vertices);
		pMesh->textureCoordinates(&UVWs);
		pMesh->addSubmesh(&Sub, true);
		pMesh->addMaterial(&Mat, true);
	}//Torus



	PrimitiveShapeFactory::PrimitiveShapeFactory(void) : CForgeObject("PrimitiveFactory") {

	}//Constructor

	PrimitiveShapeFactory::~PrimitiveShapeFactory(void) {

	}//Destructor


}//name space