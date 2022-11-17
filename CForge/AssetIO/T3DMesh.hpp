/*****************************************************************************\
*                                                                           *
* File(s): T3DMesh.hpp                                     *
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
#ifndef __CFORGE_T3DMESH_H__
#define __CFORGE_T3DMESH_H__

#include "../Core/CForgeObject.h"
#include "../Core/CoreUtility.hpp"
#include "../Graphics/GraphicsUtility.h"

#include "igl/read_triangle_mesh.h"
#include "Eigen/src/SparseQR/SparseQR.h"
#include "Eigen/src/SVD/BDCSVD.h"
#include "Eigen/src/SVD/JacobiSVD.h"
#include "Eigen/src/QR/ColPivHouseholderQR.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

namespace CForge {

	/**
	* \brief Template class that stores a triangle mesh.
	*
	* \todo Do full documentation
	*/

	template<typename T>
	class T3DMesh : public CForgeObject {
	public:
		/**
		* \brief Face data structure. Only supports triangles.
		*/
		struct Face {
			int32_t Vertices[3];

			Face(void) {
				CoreUtility::memset(Vertices, -1, 3);
			}
		};//Face

		struct AABB {
			Eigen::Vector3f Min;
			Eigen::Vector3f Max;

			Eigen::Vector3f diagonal(void) {
				return (Max - Min);
			}

			Eigen::Vector3f center(void) {
				Eigen::Vector3f half_diagonal = diagonal();
				half_diagonal.x() = half_diagonal.x() / 2.0f;
				half_diagonal.y() = half_diagonal.y() / 2.0f;
				half_diagonal.z() = half_diagonal.z() / 2.0f;
				return (Min + half_diagonal);
			}
		};//AxisAlignedBoundingBox

		struct Submesh {
			std::vector<Face> Faces;
			std::vector<int32_t> FaceID;
			int32_t Material;
			std::vector<Eigen::Vector3f> FaceNormals; ///< Stores face normals (if required)
			std::vector<Eigen::Vector3f> FaceTangents; ///< Stores face tangents (if required)
			Eigen::Quaternion<float> RotationOffset; // rotation relative to parent
			Eigen::Matrix<T, 3, 1> TranslationOffset; // translation relative to parent
			std::vector<Submesh*> Children;
			Submesh* pParent;
			int32_t ID;

			Submesh(void) {

			}//Submesh

			void init(Submesh* pRef) {
				Faces = pRef->Faces;
				RotationOffset = pRef->RotationOffset;
				TranslationOffset = pRef->TranslationOffset;
				Children = pRef->Children;
				pParent = pRef->pParent;
				Material = pRef->Material;
				FaceNormals = pRef->FaceNormals;
				FaceTangents = pRef->FaceTangents;
			}//initialize

			AABB initBB(Eigen::MatrixXf Transf_Positions) {

				AABB boundingBox;

				boundingBox.Min = Eigen::Vector3f(Transf_Positions.row(0));
				boundingBox.Max = Eigen::Vector3f(Transf_Positions.row(0));

				for (auto i : this->Faces) {

					for (int j = 0; j < 3; j++) {

						if (Transf_Positions(i.Vertices[j], 0) < boundingBox.Min.x())
							boundingBox.Min.x() = Transf_Positions(i.Vertices[j], 0);
						if (Transf_Positions(i.Vertices[j], 0) > boundingBox.Max.x())
							boundingBox.Max.x() = Transf_Positions(i.Vertices[j], 0);

						if (Transf_Positions(i.Vertices[j], 1) < boundingBox.Min.y())
							boundingBox.Min.y() = Transf_Positions(i.Vertices[j], 1);
						if (Transf_Positions(i.Vertices[j], 1) > boundingBox.Max.y())
							boundingBox.Max.y() = Transf_Positions(i.Vertices[j], 1);

						if (Transf_Positions(i.Vertices[j], 2) < boundingBox.Min.z())
							boundingBox.Min.z() = Transf_Positions(i.Vertices[j], 2);
						if (Transf_Positions(i.Vertices[j], 2) > boundingBox.Max.z())
							boundingBox.Max.z() = Transf_Positions(i.Vertices[j], 2);
					}
				}
				return boundingBox;
			}

			AABB initBB(std::vector<Eigen::Vector3f> Transf_Positions) {

				AABB boundingBox;

				boundingBox.Min = Transf_Positions[0];
				boundingBox.Max = Transf_Positions[0];

				for (auto i : this->Faces) {

					for (int j = 0; j < 3; j++) {

						if (Transf_Positions[i.Vertices[j]].x() < boundingBox.Min.x())
							boundingBox.Min.x() = Transf_Positions[i.Vertices[j]].x();
						if (Transf_Positions[i.Vertices[j]].x() > boundingBox.Max.x())
							boundingBox.Max.x() = Transf_Positions[i.Vertices[j]].x();

						if (Transf_Positions[i.Vertices[j]].y() < boundingBox.Min.y())
							boundingBox.Min.y() = Transf_Positions[i.Vertices[j]].y();
						if (Transf_Positions[i.Vertices[j]].y() > boundingBox.Max.y())
							boundingBox.Max.y() = Transf_Positions[i.Vertices[j]].y();

						if (Transf_Positions[i.Vertices[j]].z() < boundingBox.Min.z())
							boundingBox.Min.z() = Transf_Positions[i.Vertices[j]].z();
						if (Transf_Positions[i.Vertices[j]].z() > boundingBox.Max.z())
							boundingBox.Max.z() = Transf_Positions[i.Vertices[j]].z();
					}
				}
				return boundingBox;
			}
		};//Submesh


		struct Line {
			Eigen::Vector3f p;
			Eigen::Vector3f direction;

			Line(Eigen::Vector3f Vertex, Eigen::Vector3f pDirection) {
				p = Vertex;
				direction = pDirection;
			}
			Line() {

			}
		};

		struct Baryc_Vertex {
			int Vertex; //Index of Vertex of T3DMesh
			Eigen::Vector4f BC_Vertex; //BC of Vertex
		};

		struct Tetraeder {
			int Vertices[4]; //Index of Verts of TetMesh
			std::vector<Baryc_Vertex*> Embedded_Vertices;
		};

		struct TetMesh {
			Eigen::MatrixXd TV; //Vertice Positions
			Eigen::MatrixXi TF;
			std::vector<Tetraeder*> TT;
			Eigen::SparseMatrix<float, Eigen::RowMajor> Interpolation_C_Sparse;
			Eigen::MatrixXf Interpolation_C;
			std::vector<Eigen::Matrix<T, 3, 1>> Tet_Deformation;
		};

		struct Material {
			int32_t ID;
			Eigen::Vector4f Color;
			float Metallic;
			float Roughness;
			std::string TexAlbedo;
			std::string TexNormal;
			std::string TexDepth;

			std::vector<std::string> VertexShaderGeometryPass;
			std::vector<std::string> FragmentShaderGeometryPass;

			std::vector<std::string> VertexShaderShadowPass;
			std::vector<std::string> FragmentShaderShadowPass;

			std::vector<std::string> VertexShaderForwardPass;
			std::vector<std::string> FragmentShaderForwardPass;

			Material(void) {
				ID = -1;
				Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				TexAlbedo = "";
				TexNormal = "";
				TexDepth = "";
				Metallic = 0.0f;
				Roughness = 0.8f;

				VertexShaderGeometryPass.clear();
				FragmentShaderGeometryPass.clear();
				VertexShaderShadowPass.clear();
				FragmentShaderShadowPass.clear();
				VertexShaderForwardPass.clear();
				FragmentShaderForwardPass.clear();
			}//Constructor

			~Material(void) {
				clear();
			}//Destructor

			void init(const Material* pMat = nullptr) {
				clear();
				if (nullptr != pMat) {
					ID = pMat->ID;
					Color = pMat->Color;
					TexAlbedo = pMat->TexAlbedo;
					TexNormal = pMat->TexNormal;

					VertexShaderGeometryPass = pMat->VertexShaderGeometryPass;
					FragmentShaderGeometryPass = pMat->FragmentShaderGeometryPass;
					VertexShaderShadowPass = pMat->VertexShaderShadowPass;
					FragmentShaderShadowPass = pMat->FragmentShaderShadowPass;
					VertexShaderForwardPass = pMat->VertexShaderForwardPass;
					FragmentShaderForwardPass = pMat->FragmentShaderForwardPass;
				}
			}//initialize

			void clear(void) {
				ID = -1;
				Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				TexAlbedo = "";
				TexNormal = "";
				TexDepth = "";
				VertexShaderGeometryPass.clear();
				FragmentShaderGeometryPass.clear();
				VertexShaderShadowPass.clear();
				FragmentShaderShadowPass.clear();
				VertexShaderForwardPass.clear();
				FragmentShaderForwardPass.clear();
			}//clear

		};//Material

		struct Bone {
			int32_t					ID;
			std::string				Name;
			Eigen::Vector3f			Position;
			Eigen::Matrix4f			OffsetMatrix;
			std::vector<int32_t>	VertexInfluences;
			std::vector<float>		VertexWeights;
			Bone* pParent;
			std::vector<Bone*>		Children;
			float					capsuleRadius = 0;
		};

		struct RotationAxis {
			Eigen::Vector3f Axis[23];
		};

		//Copy of one Frame in Animation
		struct Pose {
			int32_t frame;
			std::vector<int32_t> IDs;
			std::vector<int32_t> BoneIDs;
			std::vector<Eigen::Vector3f> Positions;
			std::vector<Eigen::Quaternionf> Rotations;
			std::vector<Eigen::Vector3f> Scalings;
			std::vector<Eigen::Matrix4f> SkinningMatrix;
		};

		struct BoneKeyframes {
			int32_t ID;
			int32_t BoneID;
			std::string BoneName;
			std::vector<Eigen::Vector3f> Positions;
			std::vector<Eigen::Quaternionf> Rotations;
			std::vector<Eigen::Vector3f> Scalings;
			std::vector<Eigen::Matrix4f> SkinningMatrix;
			std::vector<Eigen::Matrix4f> LocalTransform;
			std::vector<float> collisionDepths;
			std::vector<float> Timestamps;
		};

		struct ChangedKeyframes {
			int32_t Frame;
			std::vector<Eigen::Quaternionf> Rotations;
			std::vector<int32_t> BoneIDs;
		};

		struct SkeletalAnimation {
			std::string Name;
			float Duration; // total duration in seconds
			float Speed; // keyframes per second
			std::vector<BoneKeyframes*> Keyframes;
		};

		struct MorphTarget {
			int32_t ID;
			std::string Name;
			std::vector<int32_t> VertexIDs;	///< Affected vertex
			std::vector<Eigen::Vector3f> VertexOffsets; ///< Offset from original position
			std::vector<Eigen::Vector3f> NormalOffsets; ///< Normal Offsets

		};

		T3DMesh(void) : CForgeObject("TDMesh") {
			m_pRoot = nullptr;
			m_pRootBone = nullptr;
		}//Constructor

		~T3DMesh(void) {
			clear();
		}//Destructor

		void init(const T3DMesh* pRef = nullptr) {
			if (nullptr != pRef && pRef != this) {
				clear();
				m_Positions = pRef->m_Positions;
				m_Normals = pRef->m_Normals;
				m_Tangents = pRef->m_Tangents;
				m_UVWs = pRef->m_UVWs;
				m_Colors = pRef->m_Colors;
				for (auto i : pRef->m_Submeshes) {
					Submesh* pS = new Submesh;
					pS->init(i);
					m_Submeshes.push_back(pS);
				}//for[submeshes]
				for (auto i : pRef->m_Materials) {
					Material* pMat = new Material;
					pMat->init(i);
					m_Materials.push_back(pMat);
				}//for[materials]
				m_AABB = pRef->m_AABB;
			}
		}//initialize

		void clear(void) {
			m_Positions.clear();
			m_Normals.clear();
			m_Tangents.clear();
			m_UVWs.clear();
			m_Colors.clear();
			for (auto& i : m_Submeshes) delete i;
			m_Submeshes.clear();
			for (auto& i : m_Materials) delete i;
			m_Materials.clear();
			m_pRoot = nullptr;
			m_pRootBone = nullptr;

			for (auto& i : m_Bones) delete i;
			m_Bones.clear();
			for (auto& i : m_SkeletalAnimations) delete i;
			m_SkeletalAnimations.clear();

			for (auto& i : m_MorphTargets) delete i;
			m_MorphTargets.clear();

			Tri_Deformations.clear();
			Tri_Normal_Deformations.clear();
			Tet_Deformations.clear();
			for (auto& i : m_ChangedFrames) {
				delete i;
			}
			m_ChangedFrames.clear();
			for (auto& i : frameWasChanged) {
				i.clear();
			}
			frameWasChanged.clear();

			for (auto& i : m_SubmeshVertices) {
				i.clear();
			}
			m_SubmeshVertices.clear();

			for (auto& i : m_TetMesh.TT) {
				delete i;
			}
			m_TetMesh.TT.clear();

			for (auto& i : aRotationAxis) delete i;
			aRotationAxis.clear();

		}//clear

		void clearSkeleton(void) {
			for (auto i : m_Bones) {
				if (nullptr != i) delete i;
			}
			m_Bones.clear();
		}//clearSkeleton

		void clearSkeletalAnimations(void) {
			for (auto& i : m_SkeletalAnimations) delete i;
			m_SkeletalAnimations.clear();
		}

		//-----Tetmesh

		Eigen::Vector4f barycentricCoord(Eigen::Matrix4f tetCoord, Eigen::Vector3f vertex) {
			Eigen::Vector4f Rval;
			Eigen::Vector4f vertex4;
			vertex4.x() = vertex.x();
			vertex4.y() = vertex.y();
			vertex4.z() = vertex.z();
			vertex4.w() = 1;

			Eigen::Matrix4f tempMatrix;

			for (int i = 0; i < 4; i++) {
				tempMatrix = tetCoord;
				tempMatrix(0, i) = vertex.x();
				tempMatrix(1, i) = vertex.y();
				tempMatrix(2, i) = vertex.z();
				tempMatrix(3, i) = 1;

				float determ1 = tempMatrix.determinant();
				float determ2 = tetCoord.determinant();

				Rval[i] = determ1 / determ2;
			}

			return Rval;

		}

		Eigen::Vector3f getTetrahedronCentroid(Eigen::Vector3f vertex[4]) {
			return ((vertex[0] + vertex[1] + vertex[2] + vertex[3]) / 4);
		}

		float GetSquaredDistancePointToTetrahedron(Eigen::Vector3f p, Eigen::Matrix4f tetCoords)
		{
			Eigen::Vector3f tet[4];
			for (int i = 0; i < 4; i++) {
				tet[i].x() = tetCoords(0, i);
				tet[i].y() = tetCoords(1, i);
				tet[i].z() = tetCoords(2, i);
			}

			Eigen::Vector3f centroid = GetTetrahedronCentroid(tet);
			Eigen::Vector3f c_p = centroid - p;
			float Rval = (c_p.x() * c_p.x()) + (c_p.y() * c_p.y()) + (c_p.z() * c_p.z());

			return Rval;
		}

		float GetDistancePointToTetrahedron(Eigen::Vector3f p, Eigen::Matrix4f tetCoords)
		{
			return sqrt(GetSquaredDistancePointToTetrahedron(p, tetCoords));
		}

		void addVertices(int j) {

			Eigen::Vector3f Vertex = m_Positions[j];
			Eigen::Matrix4f tetCoords;

			//Edited following: https://dennis2society.de/painless-tetrahedral-barycentric-mapping
			float minDist = FLT_MAX;
			unsigned int closestTetrahedronIndex;

			for (int i = 0; i < this->m_TetMesh.TT.size(); i++) {

				Tetraeder* Tet = this->m_TetMesh.TT.at(i);

				for (int t = 0; t < 4; t++) {
					tetCoords(0, t) = m_TetMesh.TV(Tet->Vertices[t], 0);
					tetCoords(1, t) = m_TetMesh.TV(Tet->Vertices[t], 1);
					tetCoords(2, t) = m_TetMesh.TV(Tet->Vertices[t], 2);
					tetCoords(3, t) = 1;
				}

				Eigen::Vector4f BC = barycentricCoord(tetCoords, Vertex);

				if (BC.x() >= 0 && (BC.y() >= 0 && (BC.z() >= 0 && BC.w() >= 0))) {
					Baryc_Vertex* new_Embedded = new Baryc_Vertex;
					new_Embedded->Vertex = j;
					new_Embedded->BC_Vertex[0] = BC[0];
					new_Embedded->BC_Vertex[1] = BC[1];
					new_Embedded->BC_Vertex[2] = BC[2];
					new_Embedded->BC_Vertex[3] = BC[3];

					Tet->Embedded_Vertices.push_back(new_Embedded);
					return;

				}
				else {
					float dist = GetDistancePointToTetrahedron(Vertex, tetCoords);
					if (dist < minDist) {
						minDist = dist;
						closestTetrahedronIndex = i;
					}
				}

			}


			//Vertex not contained in any Tetrahedron, embed in Tetrahedron with smallest distance
			Tetraeder* Tet = this->m_TetMesh.TT.at(closestTetrahedronIndex);
			for (int t = 0; t < 4; t++) {
				tetCoords(0, t) = m_TetMesh.TV(Tet->Vertices[t], 0);
				tetCoords(1, t) = m_TetMesh.TV(Tet->Vertices[t], 1);
				tetCoords(2, t) = m_TetMesh.TV(Tet->Vertices[t], 2);
				tetCoords(3, t) = 1;
			}
			Eigen::Vector4f BC = barycentricCoord(tetCoords, Vertex);

			Baryc_Vertex* new_Embedded = new Baryc_Vertex;
			new_Embedded->Vertex = j;
			new_Embedded->BC_Vertex[0] = BC[0];
			new_Embedded->BC_Vertex[1] = BC[1];
			new_Embedded->BC_Vertex[2] = BC[2];
			new_Embedded->BC_Vertex[3] = BC[3];

			Tet->Embedded_Vertices.push_back(new_Embedded);


		}

		void addTet(int Verts[4]) {
			Tetraeder* Tet = new Tetraeder;
			Tet->Vertices[0] = Verts[0];
			Tet->Vertices[1] = Verts[1];
			Tet->Vertices[2] = Verts[2];
			Tet->Vertices[3] = Verts[3];

			this->m_TetMesh.TT.push_back(Tet);
		}

		//From Tet_Deformation to new Tri_Deformation
		Eigen::VectorXf compute_Tri_Deformation(Eigen::VectorXf Tet_Deformation) {
			Eigen::MatrixXf C = this->m_TetMesh.Interpolation_C;
			Eigen::VectorXf Rval = C * Tet_Deformation;
			return Rval;

		}

		//From Initial Tri_Deformation get Tet_Deformation
		Eigen::VectorXf compute_Tet_Deformation(Eigen::VectorXf Tri_Deformation) {

			Eigen::MatrixXf Interpolation_C = this->m_TetMesh.Interpolation_C;
			Eigen::VectorXf Rval(Interpolation_C.cols());

			// Ver 1
			/*
			JacobiSVD<MatrixXf> svd(Interpolation_C, ComputeThinU | ComputeThinV);
			Rval = svd.solve(Tri_Deformation);
			*/

			// Ver 2
			ColPivHouseholderQR<MatrixXf> dec(Interpolation_C);
			Rval = dec.solve(Tri_Deformation);

			return Rval;

		}

		//Doesn't work
		Eigen::VectorXf compute_Tet_Deformation_Sparse(Eigen::VectorXf Tri_Deformation) {

			Eigen::SparseQR<SparseMatrix<float, RowMajor>, Eigen::COLAMDOrdering<int>> solver;
			solver.compute(this->m_TetMesh.Interpolation_C);
			Eigen::VectorXf Rval;

			if (solver.info() != Eigen::ComputationInfo::Success)
			{
				//decomp failed
				printf("Failed\n");
				return Rval;
			}

			Rval = solver.solve(Tri_Deformation);

			return Rval;
		}

		//Get Interpolation Matrix C (Sparse one doesn't work)
		void interpolate_Tet_Tri() {

			Eigen::MatrixXf Interpolation_C(3 * m_Positions.size(), 3 * this->m_TetMesh.TV.rows());

			for (int r = 0; r < Interpolation_C.rows(); r++) {
				for (int c = 0; c < Interpolation_C.cols(); c++) {
					Interpolation_C(r, c) = 0;
				}
			}

			SparseMatrix<float, RowMajor> Interpolation_C_Sparse(3 * m_Positions.size(), 3 * this->m_TetMesh.TV.rows());
			std::vector<Eigen::Triplet<float, int>> coefficients;


			//Over all Tets
			for (auto tet : m_TetMesh.TT) {

				//For all 4 Vertices of a Tet
				for (int i = 0; i < 4; i++) {
					//Current Tet_Vert ID = current col
					int col_ID = tet->Vertices[i];

					//all embedded Tri Vertices in Tet
					for (auto tri : tet->Embedded_Vertices) {

						//Current Tri_Vert ID = current row
						int row_ID = tri->Vertex;
						//BC_Vertex[i] = BC that needs to be applied to current Tet_Vert
						float BC = tri->BC_Vertex[i];
						//			  | Tet.x
						//			  | Tet.y
						//			  | Tet.x
						//			  |  ...
						//_______________
						// BC 0  0 ...| Tri.x
						// 0  BC 0 ...| Tri.y
						// 0  0 BC ...| Tri.z
						// ... ... ...|  ...
						// for current Tet_Vert and embedded Tri_Vert

						if (BC > 0) {
							Interpolation_C(row_ID * 3 + 0, col_ID * 3 + 0) = BC;
							Interpolation_C(row_ID * 3 + 1, col_ID * 3 + 1) = BC;
							Interpolation_C(row_ID * 3 + 2, col_ID * 3 + 2) = BC;

							coefficients.push_back(Eigen::Triplet<float>({ row_ID * 3 + 0, col_ID * 3 + 0, BC }));
							coefficients.push_back(Eigen::Triplet<float>({ row_ID * 3 + 1, col_ID * 3 + 1, BC }));
							coefficients.push_back(Eigen::Triplet<float>({ row_ID * 3 + 2, col_ID * 3 + 2, BC }));
						}

					}
				}



			}

			Interpolation_C_Sparse.setFromTriplets(coefficients.begin(), coefficients.end());
			Interpolation_C_Sparse.makeCompressed();
			coefficients.clear();

			this->m_TetMesh.Interpolation_C_Sparse = Interpolation_C_Sparse;
			this->m_TetMesh.Interpolation_C = Interpolation_C;
		}

		//----End Tetmesh


		//---- Preparation Functions to sort everything in order for collision functions to work -------------
		void interpolateKeyframes(int frames, Bone* pBone, int AnimationID) {

			int ID = pBone->ID;
			int count = 0;
			BoneKeyframes* pBoneKeyframe = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID);
			std::vector<Eigen::Vector3f> testPositions;
			std::vector<Eigen::Vector3f> testScalings;
			std::vector<Eigen::Quaternionf> testRotations;
			std::vector<float> testTimestamps;

			for (int i = 0; i < frames; i++) {
				testTimestamps.push_back(i / 1.0f);
			}

			for (int i = 0; i < frames; i++) {

				if (i == pBoneKeyframe->Timestamps.at(count)) {
					testPositions.push_back(pBoneKeyframe->Positions.at(count));
					testRotations.push_back(pBoneKeyframe->Rotations.at(count));
					testScalings.push_back(pBoneKeyframe->Scalings.at(count));
				}
				//Interpolation needed
				else {
					int start = pBoneKeyframe->Timestamps.at(count - 1);
					int end = pBoneKeyframe->Timestamps.at(count);
					int difference = end - start;
					float step = 1.0f / difference;

					Eigen::Vector3f StartTrans = pBoneKeyframe->Positions.at(count - 1);
					Eigen::Vector3f EndTrans = pBoneKeyframe->Positions.at(count);
					Eigen::Vector3f StartScale = pBoneKeyframe->Scalings.at(count - 1);
					Eigen::Vector3f EndScale = pBoneKeyframe->Scalings.at(count);
					Eigen::Quaternionf StartRot = pBoneKeyframe->Rotations.at(count - 1);
					Eigen::Quaternionf EndRot = pBoneKeyframe->Rotations.at(count);

					for (float j = step; j < 1; (j += step)) {

						Eigen::Vector3f Trans = j * EndTrans + (1 - j) * StartTrans;
						Eigen::Vector3f Scale = j * EndScale + (1 - j) * StartScale;
						Eigen::Quaternionf Rot = StartRot.slerp(j, EndRot);

						testPositions.push_back(Trans);
						testRotations.push_back(Rot);
						testScalings.push_back(Scale);

						i++;
					}

					testPositions.push_back(pBoneKeyframe->Positions.at(count));
					testRotations.push_back(pBoneKeyframe->Rotations.at(count));
					testScalings.push_back(pBoneKeyframe->Scalings.at(count));

				}

				count++;
			}

			this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID)->Positions = testPositions;
			this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID)->Scalings = testScalings;
			this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID)->Rotations = testRotations;
			this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID)->Timestamps = testTimestamps;

			pBoneKeyframe = nullptr;

			//for children
			for (auto i : pBone->Children) {
				interpolateKeyframes(frames, i, AnimationID);
			}

		}

		//Call this to assign all BoneKeyFrames the SkinningMatrices
		void setSkinningMats(int32_t startFrame, int32_t endFrame, int AnimationID, int RootBoneID) {
			//interpolateKeyframes(frames, getBone(0));
			//for each Frame go through Bones and calc Skinning Matrix
			for (int32_t i = startFrame; i <= endFrame; ++i) {
				calcSkinningMats(i, getBone(RootBoneID), Eigen::Matrix4f::Identity(), AnimationID);
			}
		}

		void calcSkinningMats(int Frame, Bone* pBone, Eigen::Matrix4f ParentTransform, int AnimationID) {

			int ID = pBone->ID;
			BoneKeyframes* pBoneKeyframe = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID);

			Eigen::Matrix4f BoneTransform;
			const Matrix4f R = GraphicsUtility::rotationMatrix(pBoneKeyframe->Rotations.at(Frame));
			const Matrix4f T = GraphicsUtility::translationMatrix(pBoneKeyframe->Positions.at(Frame));
			//const Matrix4f S = GraphicsUtility::scaleMatrix(pBoneKeyframe->Scalings.at(Frame));
			BoneTransform = T * R; // *S;

			Matrix4f LocalTransform = ParentTransform * BoneTransform;
			Matrix4f SkinningMatrix = LocalTransform * pBone->OffsetMatrix;

			while (pBoneKeyframe->SkinningMatrix.size() <= Frame) {
				pBoneKeyframe->SkinningMatrix.push_back(Eigen::Matrix4f::Identity());
			}
			while (pBoneKeyframe->LocalTransform.size() <= Frame) {
				pBoneKeyframe->LocalTransform.push_back(Eigen::Matrix4f::Identity());
			}
			pBoneKeyframe->SkinningMatrix[Frame] = SkinningMatrix;
			pBoneKeyframe->LocalTransform[Frame] = LocalTransform;
			pBoneKeyframe = nullptr;

			//for children
			for (auto i : pBone->Children) {
				calcSkinningMats(Frame, i, LocalTransform, AnimationID);
			}

		}

		Line boneDirection(Bone* pBone, Bone* childBone, int32_t AnimationID, int frame) {
			int ID1 = pBone->ID;
			int ID2 = childBone->ID;
			BoneKeyframes* pBoneKeyframe = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID1);
			BoneKeyframes* childBoneKeyframe = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID2);

			Eigen::Matrix4f pBoneTransform = pBoneKeyframe->LocalTransform.at(frame);
			Eigen::Vector3f pBonePosition(pBoneTransform(0, 3) / pBoneTransform(3, 3), pBoneTransform(1, 3) / pBoneTransform(3, 3), pBoneTransform(2, 3) / pBoneTransform(3, 3));
			Eigen::Matrix4f childTransform = childBoneKeyframe->LocalTransform.at(frame);
			Eigen::Vector3f childPosition(childTransform(0, 3), childTransform(1, 3), childTransform(2, 3));

			Line Rval(pBonePosition, childPosition - pBonePosition);
			return Rval;
		}

		float vectorLength(Eigen::Vector3f v) {
			return sqrt(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
		}

		Line getCapsuleDirection(Bone* pBone, int32_t AnimationID, int frame) {
			Line Rval(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 0, 0));

			/*if (pBone->Children.size() > 1) {
				for (auto i : pBone->Children) {
					Line temp = boneDirection(pbone, i, AnimationID);
					if (vectorLength(temp.direction) > vectorLength(Rval.direction)) {
						Rval = temp;
					}
				}
			}
			else {
				Rval = boneDirection(pBone, pBone->Children.at(0), AnimationID);
			}*/

			Rval = boneDirection(pBone, pBone->Children.at(0), AnimationID, frame);

			return Rval;
		}

		void setCapsuleRadius(Bone* pBone, int32_t AnimationID) {

			Line boneDirection;

			if (pBone->Name == "RightWrist" || pBone->Name == "LeftWrist") {

				int ID = pBone->ID;
				BoneKeyframes* pBoneKeyframe = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(ID);
				Eigen::Matrix4f pBoneTransform = pBoneKeyframe->LocalTransform.at(0);
				Eigen::Vector3f pBonePosition(pBoneTransform(0, 3), pBoneTransform(1, 3), pBoneTransform(2, 3));
				boneDirection.p = pBonePosition;

				Eigen::Vector3f directionOfLine(15.810148f, 0, 0);
				if (pBone->Name == "RightWrist") {
					directionOfLine = -1 * directionOfLine;
				}
				boneDirection.direction = directionOfLine;

			}
			else {
				boneDirection = getCapsuleDirection(pBone, AnimationID, 0);
			}

			float radiusTemp = 0;

			float numVerts = 0;
			for (int i = 0; i < pBone->VertexInfluences.size(); i++) {
				if (pBone->VertexWeights.at(i) > 0.3f) {
					Eigen::Vector3f vertex = this->m_Positions.at(pBone->VertexInfluences.at(i));
					float distanceRadius = GetDistanceLinePoint(boneDirection, vertex);
					if (distanceRadius > -1) {
						radiusTemp += distanceRadius;
						numVerts += 1.0;
					}
				}

			}

			//radiusTemp = radiusTemp / pBone->VertexInfluences.size() + max_rad;
			radiusTemp = radiusTemp / numVerts;
			pBone->capsuleRadius = radiusTemp;

		}

		void setTri_Deformation(int startFrame, int endFrame, int AnimationID) {

			//go through all Frames
			for (int frame = startFrame; frame <= endFrame; frame++) {

				Eigen::VectorXf pTri_Def;
				Eigen::VectorXf pNor_Def;

				std::vector<Eigen::Matrix4f> T = getVertexTransMatrix(frame, AnimationID);

				pTri_Def = getDeformationVector(T);
				pNor_Def = getDeformationNormals(T);

				while (this->Tri_Deformations.size() <= frame) {
					Eigen::VectorXf Test(m_Positions.size() * 3);
					this->Tri_Deformations.push_back(Test);
					this->Tri_Normal_Deformations.push_back(Test);
				}
				this->Tri_Deformations.at(frame) = pTri_Def;
				this->Tri_Normal_Deformations.at(frame) = pNor_Def;

				T.clear();
			}
		}

		BoneKeyframes* sortKeyframes(int oldFrameID, int newFrameID, int AnimationID) {
			BoneKeyframes* pFrame = this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(oldFrameID);
			pFrame->ID = newFrameID;
			pFrame->BoneID = newFrameID;
			return pFrame;
		}

		std::vector<Bone*> findBones(std::vector<Bone*> currentBones, Bone* pBone) {

			currentBones.push_back(pBone);

			for (auto i : pBone->Children) {
				currentBones = findBones(currentBones, i);
			}

			return currentBones;
		}

		void sortBones(int Animations) {

			std::vector<Bone*> sortedBones;
			std::vector<std::vector<BoneKeyframes*>> sortedBoneKeyframes;

			for (int i = 0; i < Animations; i++) {
				std::vector<BoneKeyframes*> tempVector;
				sortedBoneKeyframes.push_back(tempVector);
			}

			for (int i = 0; i < m_Bones.size(); i++) {

				if (m_Bones.at(i)->pParent == nullptr) {
					sortedBones = findBones(sortedBones, m_Bones.at(i));
					break;
				}
			}

			for (int i = 0; i < m_Bones.size(); i++) {
				int oldID = sortedBones.at(i)->ID;
				sortedBones.at(i)->ID = i;
				for (int j = 0; j < Animations; j++) {
					BoneKeyframes* temp = sortKeyframes(oldID, i, j);
					sortedBoneKeyframes.at(j).push_back(temp);
				}
			}

			this->m_Bones.clear();
			this->m_Bones = sortedBones;
			for (int j = 0; j < Animations; j++) {
				m_SkeletalAnimations.at(j)->Keyframes.clear();
				m_SkeletalAnimations.at(j)->Keyframes = sortedBoneKeyframes.at(j);
			}

			m_pRootBone = m_Bones.at(0);

		}

		void initChangedFrames(int frameCount) {
			for (int i = 0; i < boneCount(); i++) {
				ChangedKeyframes* pNew = new ChangedKeyframes;
				pNew->Frame = -2;
				m_ChangedFrames.push_back(pNew);
				std::vector<bool> changes;
				for (int i = 0; i < frameCount; i++) {
					changes.push_back(false);
				}
				this->frameWasChanged.push_back(changes);
			}
			ChangedKeyframes* pOriginal = new ChangedKeyframes;
			pOriginal->Frame = -1;
			this->lastOriginal = pOriginal;
		}

		void initRotationAxis(void) {
			RotationAxis* RightShoulder = new RotationAxis;
			RotationAxis* RightElbow = new RotationAxis;
			RotationAxis* RightWrist = new RotationAxis;
			RotationAxis* LeftShoulder = new RotationAxis;
			RotationAxis* LeftElbow = new RotationAxis;
			RotationAxis* LeftWrist = new RotationAxis;
			//aRotationAxis.at(i)
			//i = colliding Submesh/Bone Index
			//if this Bone Submesh collides, what Bones can be rotated with inverseKinematic?


			//Hip - Right Collar
			for (int i = 0; i < 8; i++) {
				this->aRotationAxis.push_back(nullptr);
				RightShoulder->Axis[i] = Eigen::Vector3f(0, 0, 0);
				RightElbow->Axis[i] = Eigen::Vector3f(0, 0, 0);
				RightWrist->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftShoulder->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftElbow->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftWrist->Axis[i] = Eigen::Vector3f(0, 0, 0);
			}

			//Right Shoulder == 8
			RightShoulder->Axis[8] = Eigen::Vector3f(0, 0, 1);
			RightElbow->Axis[8] = Eigen::Vector3f(0, 0, 1);
			RightWrist->Axis[8] = Eigen::Vector3f(0, 0, 1);
			LeftShoulder->Axis[8] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[8] = Eigen::Vector3f(0, 0, 0);
			LeftWrist->Axis[8] = Eigen::Vector3f(0, 0, 0);

			//Right Elbow
			RightShoulder->Axis[9] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[9] = Eigen::Vector3f(1, 0, 0);
			RightWrist->Axis[9] = Eigen::Vector3f(1, 0, 0);
			LeftShoulder->Axis[9] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[9] = Eigen::Vector3f(0, 0, 0);
			LeftWrist->Axis[9] = Eigen::Vector3f(0, 0, 0);

			//Right Wrist
			RightShoulder->Axis[10] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[10] = Eigen::Vector3f(0, 0, 0);
			RightWrist->Axis[10] = Eigen::Vector3f(0, 0, 1);
			LeftShoulder->Axis[10] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[10] = Eigen::Vector3f(0, 0, 0);
			LeftWrist->Axis[10] = Eigen::Vector3f(0, 0, 0);

			//Left Collar
			RightShoulder->Axis[11] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[11] = Eigen::Vector3f(0, 0, 0);
			RightWrist->Axis[11] = Eigen::Vector3f(0, 0, 0);
			LeftShoulder->Axis[11] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[11] = Eigen::Vector3f(0, 0, 0);
			LeftWrist->Axis[11] = Eigen::Vector3f(0, 0, 0);

			//Left Shoulder
			RightShoulder->Axis[12] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[12] = Eigen::Vector3f(0, 0, 0);
			RightWrist->Axis[12] = Eigen::Vector3f(0, 0, 0);
			LeftShoulder->Axis[12] = Eigen::Vector3f(0, 0, 1);
			LeftElbow->Axis[12] = Eigen::Vector3f(0, 0, 1);
			LeftWrist->Axis[12] = Eigen::Vector3f(0, 0, 1);

			//Left Elbow
			RightShoulder->Axis[13] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[13] = Eigen::Vector3f(0, 0, 0);
			RightWrist->Axis[13] = Eigen::Vector3f(0, 0, 0);
			LeftShoulder->Axis[13] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[13] = Eigen::Vector3f(1, 0, 0);
			LeftWrist->Axis[13] = Eigen::Vector3f(1, 0, 0);

			//Left Wrist
			RightShoulder->Axis[14] = Eigen::Vector3f(0, 0, 0);
			RightElbow->Axis[14] = Eigen::Vector3f(0, 0, 0);
			RightWrist->Axis[14] = Eigen::Vector3f(0, 0, 0);
			LeftShoulder->Axis[14] = Eigen::Vector3f(0, 0, 0);
			LeftElbow->Axis[14] = Eigen::Vector3f(0, 0, 0);
			LeftWrist->Axis[14] = Eigen::Vector3f(0, 0, 1);

			for (int i = 15; i < 23; i++) {
				RightShoulder->Axis[i] = Eigen::Vector3f(0, 0, 0);
				RightElbow->Axis[i] = Eigen::Vector3f(0, 0, 0);
				RightWrist->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftShoulder->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftElbow->Axis[i] = Eigen::Vector3f(0, 0, 0);
				LeftWrist->Axis[i] = Eigen::Vector3f(0, 0, 0);
			}

			this->aRotationAxis.push_back(RightShoulder);
			this->aRotationAxis.push_back(RightElbow);
			this->aRotationAxis.push_back(RightWrist);
			this->aRotationAxis.push_back(nullptr);
			this->aRotationAxis.push_back(LeftShoulder);
			this->aRotationAxis.push_back(LeftElbow);
			this->aRotationAxis.push_back(LeftWrist);

			for (int i = 15; i < 23; i++) {
				this->aRotationAxis.push_back(nullptr);
			}

		}

		//-------------------------------

		Eigen::VectorXf getTriDeformation(int32_t frame) {

			if (frame < 0 || frame >= Tri_Deformations.size()) throw IndexOutOfBoundsExcept("Frame");

			return Tri_Deformations.at(frame);
			/*Eigen::VectorXf VecDeformation = Tri_Deformations.at(frame);
			Eigen::MatrixXf MatrixDeformation(m_Positions.size(), 3);

			for (int i = 0; i < m_Positions.size(); i++) {
				MatrixDeformation.row(i) = Eigen::RowVector3f(VecDeformation(i * 3), VecDeformation(i * 3 + 1), VecDeformation(i * 3 + 2));
			}

			return MatrixDeformation;*/
		}

		Eigen::MatrixXf getTriDeformationMatrix(int32_t frame) {
			Eigen::VectorXf MeshVec = getTriDeformation(frame);
			Eigen::MatrixXf MeshVertices(vertexCount(), 3);
			for (int i = 0; i < MeshVertices.rows(); i++) {
				MeshVertices.row(i) = Eigen::RowVector3f(MeshVec(i * 3), MeshVec(i * 3 + 1), MeshVec(i * 3 + 2));
			}

			return MeshVertices;
		}

		bool boundingSphereIntersectionTest(AABB mesh1BB, AABB mesh2BB) {
			bool spheresIntersect = false;
			Eigen::Vector3f center1 = mesh1BB.center();
			Eigen::Vector3f center2 = mesh2BB.center();

			float radius1 = sqrt(mesh1BB.diagonal().dot(mesh1BB.diagonal()));
			float radius2 = sqrt(mesh2BB.diagonal().dot(mesh2BB.diagonal()));

			Eigen::Vector3f distance = center1 - center2;
			spheresIntersect = sqrt(distance.dot(distance)) <= (radius1 + radius2);

			return spheresIntersect;
		}

		Eigen::MatrixXf determineAABBminmax(AABB mesh1BB, AABB mesh2BB) {
			Eigen::MatrixXf LapMinMax;
			LapMinMax.resize(2, 3);

			Eigen::Vector3f lap_min(mesh1BB.Min.x() > mesh2BB.Min.x() ? mesh1BB.Min.x() : mesh2BB.Min.x(),
				mesh1BB.Min.y() > mesh2BB.Min.y() ? mesh1BB.Min.y() : mesh2BB.Min.y(),
				mesh1BB.Min.z() > mesh2BB.Min.z() ? mesh1BB.Min.z() : mesh2BB.Min.z());

			Eigen::Vector3f lap_max(mesh1BB.Max.x() < mesh2BB.Max.x() ? mesh1BB.Max.x() : mesh2BB.Max.x(),
				mesh1BB.Max.y() < mesh2BB.Max.y() ? mesh1BB.Max.y() : mesh2BB.Max.y(),
				mesh1BB.Max.z() < mesh2BB.Max.z() ? mesh1BB.Max.z() : mesh2BB.Max.z());

			LapMinMax.row(0) = Eigen::RowVector3f(lap_min);
			LapMinMax.row(1) = Eigen::RowVector3f(lap_max);

			return LapMinMax;
		}

		bool sameSide(Eigen::Vector3f point1, Eigen::Vector3f point2, Eigen::Vector3f point3, Eigen::Vector3f point4)
		{
			Eigen::Vector3f cp1 = (point4 - point3).cross(point1 - point3);
			Eigen::Vector3f cp2 = (point4 - point3).cross(point2 - point3);

			if (cp1.dot(cp2) >= 0.0f)
				return true;
			return false;
		}

		bool vertInsideTriangle(Eigen::Vector3f tri[3], Eigen::Vector3f vert) {
			bool Rval = sameSide(vert, tri[0], tri[1], tri[2]) && sameSide(vert, tri[1], tri[0], tri[2]) && sameSide(vert, tri[2], tri[0], tri[1]);
			return Rval;
		}

		Eigen::Vector3f closestPointOnLineSegment(Eigen::Vector3f A, Eigen::Vector3f B, Eigen::Vector3f reference_Point) {
			Eigen::Vector3f AB = B - A;
			float t = (reference_Point - A).dot(AB) / AB.dot(AB);
			float saturateT = 0;
			if (t > 0) {
				saturateT = t;
			}
			if (saturateT > 1) {
				saturateT = 1;
			}
			return Eigen::Vector3f(A + (saturateT * AB));
		}

		float sphereTriangleIntersectionTest(Eigen::Vector3f tri[3], Eigen::Vector3f center, float radius) {
			Eigen::Vector3f N = get_normal(tri[0], tri[1], tri[2]); // plane normal
			float dist = (center - tri[0]).dot(N); // signed distance between sphere and plane
			if (dist < -radius || dist > radius)
				return -1; //no intersection


			Eigen::Vector3f point0 = center - dist * N; // projected sphere center on triangle plane
			// Now determine whether point0 is inside all triangle edges: 
			Eigen::Vector3f c0 = (point0 - tri[0]).cross(tri[1] - tri[0]);
			Eigen::Vector3f c1 = (point0 - tri[1]).cross(tri[2] - tri[1]);
			Eigen::Vector3f c2 = (point0 - tri[2]).cross(tri[0] - tri[2]);
			bool inside = c0.dot(N) <= 0 && c1.dot(N) <= 0 && c2.dot(N) <= 0;

			float radiussq = radius * radius; // sphere radius squared

			// Edge 1:
			Eigen::Vector3f point1 = closestPointOnLineSegment(tri[0], tri[1], center);
			Eigen::Vector3f v1 = center - point1;
			float distsq1 = v1.dot(v1);
			bool intersects = distsq1 < radiussq;

			// Edge 2:
			Eigen::Vector3f point2 = closestPointOnLineSegment(tri[1], tri[2], center);
			Eigen::Vector3f v2 = center - point2;
			float distsq2 = v2.dot(v2);
			intersects |= distsq2 < radiussq;

			// Edge 3:
			Eigen::Vector3f point3 = closestPointOnLineSegment(tri[2], tri[0], center);
			Eigen::Vector3f v3 = center - point3;
			float distsq3 = v3.dot(v3);
			intersects |= distsq3 < radiussq;

			if (inside || intersects)
			{
				Eigen::Vector3f best_point = point0;
				Eigen::Vector3f intersection_vec;

				if (inside)
				{
					intersection_vec = center - point0;
				}
				else
				{
					Eigen::Vector3f d = center - point1;
					float best_distsq = d.dot(d);
					best_point = point1;
					intersection_vec = d;

					d = center - point2;
					float distsq = d.dot(d);
					if (distsq < best_distsq)
					{
						distsq = best_distsq;
						best_point = point2;
						intersection_vec = d;
					}

					d = center - point3;
					distsq = d.dot(d);
					if (distsq < best_distsq)
					{
						distsq = best_distsq;
						best_point = point3;
						intersection_vec = d;
					}
				}

				float len = vectorLength(intersection_vec);
				float penetration_depth = radius - len; // radius = sphere radius
				return penetration_depth; // intersection success
			}
			else {
				return -1;
			}
		}

		float capsuleTriangleIntersectionTest(Line Capsule, float capsuleRadius, Eigen::Vector3f tri[3]) {

			Eigen::Vector3f CapsuleNormal = Capsule.direction.normalized();

			Eigen::Vector3f LineEndOffset = capsuleRadius * CapsuleNormal;
			Eigen::Vector3f A = Capsule.p;
			Eigen::Vector3f B = A + Capsule.direction;
			Eigen::Vector3f Base = A - LineEndOffset;
			Eigen::Vector3f Tip = B + LineEndOffset;

			// Then for each triangle, ray-plane intersection:
			//  N is the triangle plane normal (it was computed in sphere – triangle intersection case)
			Eigen::Vector3f N = get_normal(tri[0], tri[1], tri[2]);

			float t = N.dot(tri[0] - Base) / (abs(N.dot(CapsuleNormal)));
			Eigen::Vector3f line_plane_intersection = Base + t * CapsuleNormal;

			//closestPointOnTriangleToLinePlaneIntersection
			Eigen::Vector3f reference_Point;
			// Determine whether line_plane_intersection is inside all triangle edges:
			Eigen::Vector3f c0 = (line_plane_intersection - tri[0]).cross(tri[1] - tri[0]);
			Eigen::Vector3f c1 = (line_plane_intersection - tri[1]).cross(tri[2] - tri[1]);
			Eigen::Vector3f c2 = (line_plane_intersection - tri[2]).cross(tri[0] - tri[2]);
			bool inside = c0.dot(N) <= 0 && c1.dot(N) <= 0 && c2.dot(N) <= 0;

			if (inside) {
				reference_Point = line_plane_intersection;
			}
			else {
				// Edge 1:
				Eigen::Vector3f point1 = closestPointOnLineSegment(tri[0], tri[1], line_plane_intersection);
				Eigen::Vector3f v1 = line_plane_intersection - point1;
				float distsq = v1.dot(v1);
				float best_dist = distsq;
				reference_Point = point1;

				//Edge 2: 
				Eigen::Vector3f point2 = closestPointOnLineSegment(tri[1], tri[2], line_plane_intersection);
				Eigen::Vector3f v2 = line_plane_intersection - point2;
				distsq = v2.dot(v2);
				if (distsq < best_dist) {
					reference_Point = point2;
					best_dist = distsq;
				}

				//Edge 3: 
				Eigen::Vector3f point3 = closestPointOnLineSegment(tri[2], tri[0], line_plane_intersection);
				Eigen::Vector3f v3 = line_plane_intersection - point3;
				distsq = v3.dot(v3);
				if (distsq < best_dist) {
					reference_Point = point3;
					best_dist = distsq;
				}

			}

			Eigen::Vector3f center = closestPointOnLineSegment(A, B, reference_Point);

			return sphereTriangleIntersectionTest(tri, center, capsuleRadius);

		}
		//----------------------------------------------------------------------------------

		//Everything for Triangle Triangle Intersection test ---------------
		//Collision between Two Triangle Meshes
		bool inBoundingBox(Eigen::Vector3f tri[3], Eigen::Vector3f lap_min, Eigen::Vector3f lap_max) {

			unsigned int below_min[3] = { 0,0,0 };
			unsigned int above_max[3] = { 0,0,0 };

			for (unsigned int c = 0; c < 3; c++) {
				if (tri[c].x() < lap_min.x()) below_min[0]++;
				else if (tri[c].x() > lap_max.x()) above_max[0]++;

				if (tri[c].y() < lap_min.y()) below_min[1]++;
				else if (tri[c].y() > lap_max.y()) above_max[1]++;

				if (tri[c].z() < lap_min.z()) below_min[2]++;
				else if (tri[c].z() > lap_max.z()) above_max[2]++;
			}
			if (below_min[0] == 3 || above_max[0] == 3   // Dreieck wird verworfen, wenn *alle* drei
				|| below_min[1] == 3 || above_max[1] == 3   // Eckpunkte *jeweils* in jeder Dimension
				|| below_min[2] == 3 || above_max[2] == 3) {
				//cout << "Failed Test 1" << endl; // ausserhalb der Schnitt-Box liegen!
				return false;
			}

			return true;

		}

		bool SAT(Eigen::Vector2f tri1[3], Eigen::Vector2f tri2[3]) {
			//Fuer jede Achse von Triangle 1
			for (int i = 0, j = 2; i < 3; j = i, i++) {

				Eigen::Vector2f axis((-1) * (tri1[j].y() - tri1[i].y()), (tri1[j].x() - tri1[i].x()));
				axis.normalize();

				// Alle Vertices Triangle 1
				// Von den auf die Achse projizierten Vertices Min und Max bestimmten
				float p1min = axis.dot(tri1[0]);
				float p1max = p1min;

				for (int k = 1; k < 3; k++)
				{
					float temp = axis.dot(tri1[k]);
					if (temp < p1min) p1min = temp;
					if (temp > p1max) p1max = temp;
				}


				//Alle Vertices Triangle 2
				// Von den auf die Achse projizierten Vertices Min und Max bestimmten
				float p2min = axis.dot(tri2[0]);
				float p2max = p2min;

				for (int k = 1; k < 3; k++)
				{
					float temp = axis.dot(tri2[k]);
					if (temp < p2min) p2min = temp;
					if (temp > p2max) p2max = temp;

				}

				//Wenn es keine Ueberlappung der beiden Intervalle gibt dann schneiden sich die Dreiecke nicht
				if ((p1min - p2max > 0) || (p2min - p1max > 0)) {
					return false;
				}

			}

			//Fuer jede Achse von Triangle 2
			for (int i = 0, j = 2; i < 3; j = i, i++) {

				Eigen::Vector2f axis((-1) * (tri2[j].y() - tri2[i].y()), (tri2[j].x() - tri2[i].x()));
				axis.normalize();


				// Alle Vertices Triangle 1
				// Von den auf die Achse projizierten Vertices Min und Max bestimmten
				float p1min = axis.dot(tri1[0]);
				float p1max = p1min;

				for (int k = 1; k < 3; k++)
				{
					float temp = axis.dot(tri1[k]);
					if (temp < p1min) p1min = temp;
					if (temp > p1max) p1max = temp;

				}

				//Alle Vertices Triangle 2
				// Von den auf die Achse projizierten Vertices Min und Max bestimmten
				float p2min = axis.dot(tri2[0]);
				float p2max = p2min;

				for (int k = 1; k < 3; k++)
				{
					float temp = axis.dot(tri2[k]);
					if (temp < p2min) p2min = temp;
					if (temp > p2max) p2max = temp;

				}

				//Wenn es keine Ueberlappung der beiden Intervalle gibt dann schneiden sich die Dreiecke nicht
				if ((p1min - p2max > 0) || (p2min - p1max > 0)) {
					return false;
				}

			}

			return true;
		}

		float plane_d(Eigen::Vector3f normal, Eigen::Vector3f vert) {
			return normal.dot(vert);
		}

		float signed_distance(Eigen::Vector3f normal, Eigen::Vector3f vert, float d) {
			return (normal.dot(vert) + d);
		}

		Eigen::Vector3f get_normal(Eigen::Vector3f vert1, Eigen::Vector3f vert2, Eigen::Vector3f vert3) {
			Eigen::Vector3f edge1(vert2.x() - vert1.x(), vert2.y() - vert1.y(), vert2.z() - vert1.z());
			Eigen::Vector3f edge2(vert3.x() - vert1.x(), vert3.y() - vert1.y(), vert3.z() - vert1.z());
			Eigen::Vector3f normal(edge1.cross(edge2));
			normal.normalize();
			return normal;
		}

		Line plane_intersection(Eigen::Vector3f plane_A_normal, float plane_A_d, Eigen::Vector3f plane_B_normal, float plane_B_d) {
			Line line1;

			Eigen::Vector3f direction = plane_A_normal.cross(plane_B_normal);
			if (sqrt(direction.dot(direction)) == 0) {
				line1.p = Eigen::Vector3f(NAN, NAN, NAN);
				line1.direction = Eigen::Vector3f(NAN, NAN, NAN);
			}
			else {
				line1.direction = direction;
				float s_1, s_2, a_1, b_1;
				s_1 = plane_A_d;
				s_2 = plane_B_d;
				float nln2dot = plane_A_normal.dot(plane_B_normal);
				float n1normsqr = plane_A_normal.dot(plane_A_normal);
				float n2normsqr = plane_B_normal.dot(plane_B_normal);

				a_1 = (s_2 * nln2dot - s_1 * n2normsqr) / ((nln2dot * nln2dot) - (n1normsqr * n2normsqr));
				b_1 = (s_1 * nln2dot - s_2 * n2normsqr) / ((nln2dot * nln2dot) - (n1normsqr * n2normsqr));
				line1.p = Eigen::Vector3f(a_1 * plane_A_normal.x() + b_1 * plane_B_normal.x(),
					a_1 * plane_A_normal.y() + b_1 * plane_B_normal.y(),
					a_1 * plane_A_normal.z() + b_1 * plane_B_normal.z());
			}
			return line1;

		}

		bool triangleIntersection(Eigen::Vector3f tri1[3], Eigen::Vector3f tri2[3]) {
			//Compute Plane Equation of T1
			Eigen::Vector3f tri1_normal(get_normal(tri1[0], tri1[1], tri1[2]));
			float plane1_d = (-1) * plane_d(tri1_normal, tri1[0]);
			//Compute Plane Equation of T2
			Eigen::Vector3f tri2_normal(get_normal(tri2[0], tri2[1], tri2[2]));
			float plane2_d = (-1) * plane_d(tri2_normal, tri2[0]);

			//Compute the signed distances d_tri1 of vertices of T1
			float d_tri1[3];
			d_tri1[0] = signed_distance(tri2_normal, tri1[0], plane2_d);
			d_tri1[1] = signed_distance(tri2_normal, tri1[1], plane2_d);
			d_tri1[2] = signed_distance(tri2_normal, tri1[2], plane2_d);

			//Compute the signed distances d_tri2 of vertices of T2
			float d_tri2[3];
			d_tri2[0] = signed_distance(tri1_normal, tri2[0], plane1_d);
			d_tri2[1] = signed_distance(tri1_normal, tri2[1], plane1_d);
			d_tri2[2] = signed_distance(tri1_normal, tri2[2], plane1_d);

			//Compare signs of d_tri*
			//if they are all the same return false
			if (d_tri1[0] < 0 && d_tri1[1] < 0 && d_tri1[2] < 0) return false;
			if (d_tri1[0] > 0 && d_tri1[1] > 0 && d_tri1[2] > 0) return false;
			if (d_tri2[0] < 0 && d_tri2[1] < 0 && d_tri2[2] < 0) return false;
			if (d_tri2[0] > 0 && d_tri2[1] > 0 && d_tri2[2] > 0) return false;

			if (tri1_normal[0] == tri2_normal[0] &&
				tri1_normal[1] == tri2_normal[1] &&
				tri1_normal[2] == tri2_normal[2]) {
				if (plane1_d == plane2_d) {

					if (fabs(tri1_normal[0]) >= fabs(tri1_normal[1]) && fabs(tri1_normal[0]) >= fabs(tri1_normal[2])) {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][1],tri1[0][2]),
														Eigen::Vector2f(tri1[1][1],tri1[1][2]),
														Eigen::Vector2f(tri1[2][1],tri1[2][2]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][1],tri2[0][2]),
														Eigen::Vector2f(tri2[1][1],tri2[1][2]),
														Eigen::Vector2f(tri2[2][1],tri2[2][2]) };
						return SAT(tri1_2D, tri2_2D);
					}
					else if (fabs(tri1_normal[1]) >= fabs(tri1_normal[0]) && fabs(tri1_normal[1]) >= fabs(tri1_normal[2])) {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][0],tri1[0][2]),
														Eigen::Vector2f(tri1[1][0],tri1[1][2]),
														Eigen::Vector2f(tri1[2][0],tri1[2][2]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][0],tri2[0][2]),
														Eigen::Vector2f(tri2[1][0],tri2[1][2]),
														Eigen::Vector2f(tri2[2][0],tri2[2][2]) };
						return SAT(tri1_2D, tri2_2D);
					}
					else {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][0],tri1[0][1]),
														Eigen::Vector2f(tri1[1][0],tri1[1][1]),
														Eigen::Vector2f(tri1[2][0],tri1[2][1]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][0],tri2[0][1]),
														Eigen::Vector2f(tri2[1][0],tri2[1][1]),
														Eigen::Vector2f(tri2[2][0],tri2[2][1]) };
						return SAT(tri1_2D, tri2_2D);
					}
				}
				else return false;
			}
			else if (tri1_normal[0] == (-1) * tri2_normal[0] && tri1_normal[1] == (-1) * tri2_normal[1] && tri1_normal[2] == (-1) * tri2_normal[2]) {
				if (plane1_d == (-1) * plane2_d) {

					if (fabs(tri1_normal[0]) >= fabs(tri1_normal[1]) && fabs(tri1_normal[0]) >= fabs(tri1_normal[2])) {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][1],tri1[0][2]),
														Eigen::Vector2f(tri1[1][1],tri1[1][2]),
														Eigen::Vector2f(tri1[2][1],tri1[2][2]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][1],tri2[0][2]),
														Eigen::Vector2f(tri2[1][1],tri2[1][2]),
														Eigen::Vector2f(tri2[2][1],tri2[2][2]) };
						return SAT(tri1_2D, tri2_2D);
					}
					else if (fabs(tri1_normal[1]) >= fabs(tri1_normal[0]) && fabs(tri1_normal[1]) >= fabs(tri1_normal[2])) {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][0],tri1[0][2]),
														Eigen::Vector2f(tri1[1][0],tri1[1][2]),
														Eigen::Vector2f(tri1[2][0],tri1[2][2]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][0],tri2[0][2]),
														Eigen::Vector2f(tri2[1][0],tri2[1][2]),
														Eigen::Vector2f(tri2[2][0],tri2[2][2]) };
						return SAT(tri1_2D, tri2_2D);
					}
					else {
						Eigen::Vector2f tri1_2D[3] = { Eigen::Vector2f(tri1[0][0],tri1[0][1]),
														Eigen::Vector2f(tri1[1][0],tri1[1][1]),
														Eigen::Vector2f(tri1[2][0],tri1[2][1]) };

						Eigen::Vector2f tri2_2D[3] = { Eigen::Vector2f(tri2[0][0],tri2[0][1]),
														Eigen::Vector2f(tri2[1][0],tri2[1][1]),
														Eigen::Vector2f(tri2[2][0],tri2[2][1]) };
						return SAT(tri1_2D, tri2_2D);
					}
				}
				else return false;
			}

			Line intersection_line = plane_intersection(tri1_normal, plane1_d, tri2_normal, plane2_d);
			if (intersection_line.p[0] == NAN)
				return false;

			unsigned int order_tri1[3] = { 0, };
			if ((d_tri1[0] > 0 && d_tri1[2] > 0) || (d_tri1[0] < 0 && d_tri1[2] < 0))
				order_tri1[0] = 1;
			else if ((d_tri1[0] > 0 && d_tri1[1] > 0) || (d_tri1[0] < 0 && d_tri1[1] < 0))
				order_tri1[0] = 2;

			order_tri1[1] = (order_tri1[0] + 1) % 3;
			order_tri1[2] = (order_tri1[0] + 2) % 3;


			unsigned int order_tri2[3] = { 0, };
			if ((d_tri2[0] > 0 && d_tri2[2] > 0) || (d_tri2[0] < 0 && d_tri2[2] < 0))
				order_tri2[0] = 1;
			else if ((d_tri2[0] > 0 && d_tri2[1] > 0) || (d_tri2[0] < 0 && d_tri2[1] < 0))
				order_tri2[0] = 2;

			order_tri2[1] = (order_tri2[0] + 1) % 3;
			order_tri2[2] = (order_tri2[0] + 2) % 3;


			d_tri1[0] = fabs(d_tri1[0]);
			d_tri1[1] = fabs(d_tri1[1]);
			d_tri1[2] = fabs(d_tri1[2]);
			d_tri2[0] = fabs(d_tri2[0]);
			d_tri2[1] = fabs(d_tri2[1]);
			d_tri2[2] = fabs(d_tri2[2]);

			// Fuer die Kanten von tri1 ...
			float t01 = d_tri1[order_tri1[1]] / (d_tri1[order_tri1[0]] + d_tri1[order_tri1[1]]);// gilt: t10 = 1-t01
			float t02 = d_tri1[order_tri1[2]] / (d_tri1[order_tri1[0]] + d_tri1[order_tri1[2]]); // gilt: t20 = 1-t02

			Eigen::Vector3f is_tri1[2] = { Eigen::Vector3f(tri1[order_tri1[0]][0] * t01 + tri1[order_tri1[1]][0] * (1.0f - t01),
												tri1[order_tri1[0]][1] * t01 + tri1[order_tri1[1]][1] * (1.0f - t01),
												tri1[order_tri1[0]][2] * t01 + tri1[order_tri1[1]][2] * (1.0f - t01)),
											Eigen::Vector3f(tri1[order_tri1[0]][0] * t02 + tri1[order_tri1[2]][0] * (1.0f - t02),
												tri1[order_tri1[0]][1] * t02 + tri1[order_tri1[2]][1] * (1.0f - t02),
												tri1[order_tri1[0]][2] * t02 + tri1[order_tri1[2]][2] * (1.0f - t02)) };

			// Fuer die Kanten von tri2 ...
			t01 = d_tri2[order_tri2[1]] / (d_tri2[order_tri2[0]] + d_tri2[order_tri2[1]]);// gilt: t10 = 1-t01
			t02 = d_tri2[order_tri2[2]] / (d_tri2[order_tri2[0]] + d_tri2[order_tri2[2]]); // gilt: t20 = 1-t02

			Eigen::Vector3f is_tri2[2] = { Eigen::Vector3f(tri2[order_tri2[0]][0] * t01 + tri2[order_tri2[1]][0] * (1.0f - t01),
												 tri2[order_tri2[0]][1] * t01 + tri2[order_tri2[1]][1] * (1.0f - t01),
												 tri2[order_tri2[0]][2] * t01 + tri2[order_tri2[1]][2] * (1.0f - t01)),
											Eigen::Vector3f(tri2[order_tri2[0]][0] * t02 + tri2[order_tri2[2]][0] * (1.0f - t02),
												 tri2[order_tri2[0]][1] * t02 + tri2[order_tri2[2]][1] * (1.0f - t02),
												 tri2[order_tri2[0]][2] * t02 + tri2[order_tri2[2]][2] * (1.0f - t02)) };
			Eigen::Vector3f is_line_dir = tri1_normal.cross(tri2_normal); //intersection_line.direction;
			Eigen::Vector3f is_line_org = is_tri1[0];

			Eigen::Vector3f temp1(is_tri1[0][0] - is_line_org[0],
				is_tri1[0][1] - is_line_org[1],
				is_tri1[0][2] - is_line_org[2]);

			float dot_tri1[2] = { Eigen::Vector3f(is_tri1[0][0] - is_line_org[0],
											is_tri1[0][1] - is_line_org[1],
											is_tri1[0][2] - is_line_org[2]).dot(is_line_dir),
								 Eigen::Vector3f(is_tri1[1][0] - is_line_org[0],
											is_tri1[1][1] - is_line_org[1],
											is_tri1[1][2] - is_line_org[2]).dot(is_line_dir) };

			float dot_tri2[2] = { Eigen::Vector3f(is_tri2[0][0] - is_line_org[0],
											is_tri2[0][1] - is_line_org[1],
											is_tri2[0][2] - is_line_org[2]).dot(is_line_dir),
								  Eigen::Vector3f(is_tri2[1][0] - is_line_org[0],
											is_tri2[1][1] - is_line_org[1],
											is_tri2[1][2] - is_line_org[2]).dot(is_line_dir) };


			// Sortieren der (Linien)Parameter fuer beide Intervalle (notwendig fuer den Vergleichstest min ? max):
			if (dot_tri1[0] > dot_tri1[1]) {
				float tmp = dot_tri1[0];
				dot_tri1[0] = dot_tri1[1];
				dot_tri1[1] = tmp;
			}
			if (dot_tri2[0] > dot_tri2[1]) {
				float tmp = dot_tri2[0];
				dot_tri2[0] = dot_tri2[1];
				dot_tri2[1] = tmp;
			}


			if (dot_tri1[0] > dot_tri2[1]  // min1 > max2
				|| dot_tri1[1] < dot_tri2[0])  // max1 < min2
				return false;



			return true;
		}

		//End Triangle Triangle Intersection test --------------------------


		//Inverse Kinematics -----------------
		float GetDistanceLinePoint(Line pLine, Eigen::Vector3f point) {
			float Rval;

			Eigen::Vector3f normalizedDirection = pLine.direction.normalized();

			float length = vectorLength(pLine.direction);
			Eigen::Vector3f OriginToPoint = point - pLine.p;
			float t = normalizedDirection.dot(OriginToPoint);

			if (t >= 0 && t <= length) {
				Eigen::Vector3f pointPrime = pLine.p + t * normalizedDirection;
				Eigen::Vector3f pointToPointPrime = point - pointPrime;
				Rval = vectorLength(pointToPointPrime);
			}
			else Rval = -1;
			/*else if (t < 0) {
				Eigen::Vector3f pointToPoint = point - pLine.p;
				Rval = vectorLength(pointToPoint);
			}
			else {
				Eigen::Vector3f tip = pLine.p + pLine.direction;
				Eigen::Vector3f pointToPoint = point - tip;
				Rval = vectorLength(pointToPoint);
			}*/
			return Rval;
		}


		float linePlaneIntersection(Line pLine, Eigen::Vector3f normal, float negative_d) {

			float Rval = 0;

			float planeRayDot = normal.dot(pLine.direction);

			//Ray Direction and Plane parallel
			//Usually check if Ray Origin in Plane or not
			// 
			//Here doesn't matter if there's a distance between Ray 
			//and Plane when they're parallel (Reason: distance won't change if move along Ray)
			if (abs(planeRayDot) < 0.01) {
				/*
				if (tri_normal.dot(pLine.p) == plane_d) {
					return Rval;
				}
				*/
				return 0;
			}
			else {
				//Rval is distance from Ray Origin to Plane
				Rval = (-1) * (normal.dot(pLine.p) + negative_d);
				Rval = Rval / planeRayDot;

				//Ray only defined for +Rval, otherwise reject intersection
				/*if (Rval < 0) {
					Rval = 0;
				}*/

				return Rval;
			}


		}

		float lineTriangleIntersection(Line pLine, Eigen::Vector3f tri[3]) {
			Eigen::Vector3f tri_normal(get_normal(tri[0], tri[1], tri[2]));
			float d = -1 * plane_d(tri_normal, tri[0]);
			return linePlaneIntersection(pLine, tri_normal, d);

		}

		std::vector<int32_t> getAffectedTriangles(std::vector<Eigen::Vector2i> affectedTriangles, int32_t Mesh) {
			std::vector<int32_t> affectedTrianglesMesh1;

			for (int i = 0; i < affectedTriangles.size(); i++) {
				int32_t index;
				if (Mesh == 0) {
					index = affectedTriangles.at(i).x();
				}
				else {
					index = affectedTriangles.at(i).y();
				}


				int j = 0;

				while (j < affectedTrianglesMesh1.size()) {
					//Face already in Vector
					if (affectedTrianglesMesh1.at(j) == index) {
						j = affectedTrianglesMesh1.size();
					}
					j++;
				}
				//if Face not already in Vector : j == affectedTrianglesMesh1.size()
				//if Face already in Vector : j == affectedTrianglesMesh1.size() + 1

				if (j == affectedTrianglesMesh1.size()) {
					affectedTrianglesMesh1.push_back(index);
				}
			}

			return affectedTrianglesMesh1;
		}

		std::vector<int32_t> getAffectedVertices(std::vector<int32_t> affectedTrianglesMesh1) {

			std::vector<int32_t> affectedVertices;

			for (int i = 0; i < affectedTrianglesMesh1.size(); i++) {

				for (int j = 0; j < 3; j++) {

					int32_t index = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesMesh1.at(i)).Vertices[j];

					int k = 0;

					while (k < affectedVertices.size() && index >= affectedVertices.at(k)) {
						//Vertex already in Vector
						if (affectedVertices.at(k) == index) {
							k = affectedVertices.size();
						}
						k++;
					}

					if (k <= affectedVertices.size()) {

						if (k == affectedVertices.size()) {
							affectedVertices.insert(affectedVertices.end(), index);
						}
						else {
							auto itPos = affectedVertices.begin() + k;
							affectedVertices.insert(itPos, index);
						}
					}
				}
			}

			return affectedVertices;
		}

		std::vector<Eigen::Vector3f> getEndeffector(Pose pPose, std::vector<int32_t> affectedVertices) { //std::vector<Eigen::Vector2i> affectedTriangles) {
			std::vector<Eigen::Vector3f> newVertexPositions;

			std::vector<Eigen::Matrix4f> T = getVertexTransMatrixFromPose(pPose);
			Eigen::VectorXf p_Tri_Deformation = getDeformationVector(T);

			//std::vector<int32_t> affectedVertices = getAffectedVertices(affectedTriangles);

			for (int i = 0; i < affectedVertices.size(); i++) {
				Eigen::Vector3f Vertex;
				Vertex.x() = p_Tri_Deformation(affectedVertices.at(i) * 3 + 0);
				Vertex.y() = p_Tri_Deformation(affectedVertices.at(i) * 3 + 1);
				Vertex.z() = p_Tri_Deformation(affectedVertices.at(i) * 3 + 2);
				newVertexPositions.push_back(Vertex);
			}

			affectedVertices.clear();
			T.clear();

			return newVertexPositions;
		}

		Pose copyFrame(int frame, int AnimationID) {
			Pose poseCopy;
			poseCopy.frame = frame;

			for (auto boneKeyframe : this->m_SkeletalAnimations.at(AnimationID)->Keyframes) {

				poseCopy.IDs.push_back(boneKeyframe->ID);
				poseCopy.BoneIDs.push_back(boneKeyframe->BoneID);
				poseCopy.Positions.push_back(boneKeyframe->Positions.at(frame));
				poseCopy.Rotations.push_back(boneKeyframe->Rotations.at(frame));
				poseCopy.Scalings.push_back(boneKeyframe->Scalings.at(frame));
				poseCopy.SkinningMatrix.push_back(boneKeyframe->SkinningMatrix.at(frame));

			}
			return poseCopy;
		}

		Pose updateSkeletonHistory(Pose P) {

			Eigen::Matrix4f LocalTransformList[23];

			for (int i = 0; i < P.BoneIDs.size(); i++) {
				Eigen::Matrix4f ParentTransform;
				if (i > 0) {
					ParentTransform = LocalTransformList[getBone(i)->pParent->ID];
				}
				else {
					ParentTransform = Eigen::Matrix4f::Identity();
				}

				Eigen::Matrix4f BoneTransform;

				const Matrix4f R = GraphicsUtility::rotationMatrix(P.Rotations.at(i));
				const Matrix4f T = GraphicsUtility::translationMatrix(P.Positions.at(i));
				//const Matrix4f S = GraphicsUtility::scaleMatrix(P.Scalings.at(i));
				BoneTransform = T * R; // *S;

				Matrix4f LocalTransform = ParentTransform * BoneTransform;
				LocalTransformList[i] = LocalTransform;
				Matrix4f SkinningMatrix = LocalTransform * getBone(i)->OffsetMatrix;
				P.SkinningMatrix.at(i) = SkinningMatrix;
			}


			return P;
		}

		float computeError(std::vector<Eigen::Vector3f> CurrentPosition, std::vector<Eigen::Vector3f> TargetPosition) {

			float Rval = 0;

			for (int i = 0; i < TargetPosition.size(); i++) {

				Eigen::Vector3f pDistance = TargetPosition.at(i) - CurrentPosition.at(i);

				Rval = Rval + sqrt(pDistance.dot(pDistance));

			}
			Rval = Rval / TargetPosition.size();

			return Rval;

		}

		float findFittingD(std::vector<Eigen::Vector3f> MeshVertices, std::vector<int32_t> affectedVerticesSorted2, Eigen::Vector3f Normal) {
			float Rval = 0;

			Eigen::Vector3f allVerts(0, 0, 0);

			for (int i = 0; i < affectedVerticesSorted2.size(); i++) {
				Eigen::Vector3f Vert = MeshVertices.at(affectedVerticesSorted2.at(i));
				allVerts += Vert;
			}
			float size = affectedVerticesSorted2.size();

			allVerts = allVerts / size;
			Rval = allVerts.dot(Normal);

			return Rval;
		}

		std::vector<int32_t> getGoodVertices(int frame, Submesh* Submesh1, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> TrianglesToCheck, std::vector<Eigen::Vector2i> affectedTriangles, std::vector<Eigen::Vector3f> MeshVertices) {

			std::vector<int32_t> goodVertices;

			for (int i = 0; i < affectedTriangles.size(); i++) {

				Eigen::Vector3f Tri2[3];
				int32_t Vertex1 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0];
				int32_t Vertex2 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1];
				int32_t Vertex3 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2];

				Tri2[0] = MeshVertices.at(Vertex1);
				Tri2[1] = MeshVertices.at(Vertex2);
				Tri2[2] = MeshVertices.at(Vertex3);

				Eigen::Vector3f TriNormal(0, 0, 0);
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 2));
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 2));
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 2));
				TriNormal.normalize();



				for (int j = 0; j < 3; j++) {
					int VertIndex = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[j];
					Eigen::Vector3f Vertex = MeshVertices.at(VertIndex);
					Line pLine(Vertex, TriNormal);
					//Check if this performs correctly
					float temp = lineTriangleIntersection(pLine, Tri2);

					if (temp < 0) {
						if (forbiddenVertices.size() != 0) {
							int32_t index = findNewPosition(forbiddenVertices, VertIndex);

							if (forbiddenVertices.at(index) != VertIndex) {
								if (index == forbiddenVertices.size() - 1 && forbiddenVertices.at(index) < VertIndex) {
									forbiddenVertices.push_back(VertIndex);
								}
								else {
									forbiddenVertices.insert(forbiddenVertices.begin() + index, VertIndex);
								}
							}
						}
						else {
							forbiddenVertices.push_back(VertIndex);
						}

						if (goodVertices.size() > 0) {
							int index = findNewPosition(goodVertices, VertIndex);

							if (goodVertices.at(index) == VertIndex) {
								goodVertices.erase(goodVertices.begin() + index);
							}

						}
					}
					else {
						if (goodVertices.size() != 0) {
							int index = findNewPosition(goodVertices, VertIndex);
							if (goodVertices.at(index) != VertIndex) {
								if (index == goodVertices.size() - 1 && goodVertices.at(index) < VertIndex) {
									goodVertices.push_back(VertIndex);
								}
								else {
									goodVertices.insert(goodVertices.begin() + index, VertIndex);
								}
							}

						}
						else {
							goodVertices.push_back(VertIndex);
						}
					}
				}
			}

			std::vector<int32_t> goodVerticesNew = findAdditionalVertices(Submesh1, goodVertices, forbiddenVertices, TrianglesToCheck);

			goodVertices.clear();

			return goodVerticesNew;
		}

		std::vector<int32_t> getAllTrianglesInside(int frame, Submesh* Submesh1, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> TrianglesToCheck, std::vector<Eigen::Vector2i> affectedTriangles, std::vector<Eigen::Vector3f> MeshVertices) {

			std::vector<int32_t> goodVertices;

			for (int i = 0; i < affectedTriangles.size(); i++) {

				Eigen::Vector3f Tri2[3];
				int32_t Vertex1 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0];
				int32_t Vertex2 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1];
				int32_t Vertex3 = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2];

				Tri2[0] = MeshVertices.at(Vertex1);
				Tri2[1] = MeshVertices.at(Vertex2);
				Tri2[2] = MeshVertices.at(Vertex3);

				Eigen::Vector3f TriNormal(0, 0, 0);
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex1 * 3 + 2));
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex2 * 3 + 2));
				TriNormal += Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 0),
					this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 1),
					this->Tri_Normal_Deformations.at(frame)(Vertex3 * 3 + 2));
				TriNormal.normalize();



				for (int j = 0; j < 3; j++) {
					int VertIndex = m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[j];
					Eigen::Vector3f Vertex = MeshVertices.at(VertIndex);
					Line pLine(Vertex, TriNormal);
					//Check if this performs correctly
					float temp = lineTriangleIntersection(pLine, Tri2);

					if (temp < 0) {
						if (forbiddenVertices.size() != 0) {
							int32_t index = findNewPosition(forbiddenVertices, VertIndex);

							if (forbiddenVertices.at(index) != VertIndex) {
								if (index == forbiddenVertices.size() - 1 && forbiddenVertices.at(index) < VertIndex) {
									forbiddenVertices.push_back(VertIndex);
								}
								else {
									forbiddenVertices.insert(forbiddenVertices.begin() + index, VertIndex);
								}
							}
						}
						else {
							forbiddenVertices.push_back(VertIndex);
						}

						if (goodVertices.size() > 0) {
							int index = findNewPosition(goodVertices, VertIndex);

							if (goodVertices.at(index) == VertIndex) {
								goodVertices.erase(goodVertices.begin() + index);
							}

						}
					}
					else {
						if (goodVertices.size() != 0) {
							int index = findNewPosition(goodVertices, VertIndex);
							if (goodVertices.at(index) != VertIndex) {
								if (index == goodVertices.size() - 1 && goodVertices.at(index) < VertIndex) {
									goodVertices.push_back(VertIndex);
								}
								else {
									goodVertices.insert(goodVertices.begin() + index, VertIndex);
								}
							}

						}
						else {
							goodVertices.push_back(VertIndex);
						}
					}
				}
			}

			std::vector<int32_t> allTrianglesInside = findAdditionalVertices(Submesh1, goodVertices, forbiddenVertices, TrianglesToCheck);

			goodVertices.clear();

			return allTrianglesInside;
		}


		std::vector<Eigen::Vector3f> getTargetPositionVersion1(int frame, std::vector<int32_t> goodVerticesNew, std::vector<int32_t> affectedTrianglesSorted2, std::vector<int32_t> affectedVertices2, std::vector<Eigen::Vector3f> MeshVertices) {

			std::vector<Eigen::Vector3f> TargetPosition;
			Eigen::Vector3f sumNormals(0, 0, 0);
			float max_distance = 0.0f;
			//printf("%d\n", max_distance);
			for (int i = 0; i < affectedTrianglesSorted2.size(); i++) {

				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));

					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();

			float approximateD = findFittingD(MeshVertices, affectedVertices2, sumNormals);

			for (int i = 0; i < goodVerticesNew.size(); i++) {
				Eigen::Vector3f Vertex = MeshVertices.at(goodVerticesNew.at(i));
				Line pLine(Vertex, sumNormals);
				float temp = linePlaneIntersection(pLine, sumNormals, -1 * approximateD);
				if (temp > max_distance) {
					max_distance = temp;
				}
			}

			for (int i = 0; i < goodVerticesNew.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(goodVerticesNew.at(i));
				temp = temp + (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}

			return TargetPosition;
		}

		std::vector<Eigen::Vector3f> getTargetPositionVersion2(int frame, std::vector<Eigen::Vector2i> affectedTriangles, std::vector<int32_t> affectedVertices, std::vector<int32_t> affectedTrianglesSorted2, std::vector<Eigen::Vector3f> MeshVertices) {
			std::vector<Eigen::Vector3f> TargetPosition;
			Eigen::Vector3f sumNormals(0, 0, 0);
			float max_distance = 0;


			//Normal from Vertices in Mesh2
			for (int i = 0; i < affectedTrianglesSorted2.size(); i++) {
				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));
					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();

			//max_distance from Intersection Ray-Triangle / Vert Mesh1 to Triangle Mesh2
			for (int i = 0; i < affectedTriangles.size(); i++) {

				Eigen::Vector3f Tri2[3];
				Tri2[0] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0]);
				Tri2[1] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1]);
				Tri2[2] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2]);
				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Vertex = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[j]);
					Line pLine(Vertex, sumNormals);

					float temp = lineTriangleIntersection(pLine, Tri2);
					if (max_distance < temp) {
						max_distance = temp;
					}
				}
			}

			for (int i = 0; i < affectedVertices.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(affectedVertices.at(i));
				temp += (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}


			return TargetPosition;
		}

		std::vector<Eigen::Vector3f> getTargetPositionVersion3(int frame, std::vector<Eigen::Vector2i> affectedTriangles, std::vector<int32_t> affectedVertices, std::vector<int32_t> affectedTrianglesSorted, std::vector<Eigen::Vector3f> MeshVertices) {
			std::vector<Eigen::Vector3f> TargetPosition;
			Eigen::Vector3f sumNormals(0, 0, 0);
			float max_distance = 0;

			//Normal from Vertices in Mesh1
			for (int i = 0; i < affectedTrianglesSorted.size(); i++) {
				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));
					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();
			sumNormals = -1.0 * sumNormals;

			//max_distance from Intersection Ray-Triangle / Vert Mesh1 to Triangle Mesh2
			for (int i = 0; i < affectedTriangles.size(); i++) {

				Eigen::Vector3f Tri2[3];
				Tri2[0] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0]);
				Tri2[1] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1]);
				Tri2[2] = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2]);
				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Vertex = MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[j]);
					Line pLine(Vertex, sumNormals);

					float temp = lineTriangleIntersection(pLine, Tri2);
					if (max_distance < temp) {
						max_distance = temp;
					}
				}
			}

			for (int i = 0; i < affectedVertices.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(affectedVertices.at(i));
				temp += (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}

			return TargetPosition;
		}

		std::vector<Eigen::Vector3f> getTargetPositionVersion4(int frame, std::vector<int32_t> goodVerticesNew, std::vector<int32_t> affectedTrianglesSorted2, std::vector<int32_t> affectedVertices2, std::vector<Eigen::Vector3f> MeshVertices) {

			std::vector<Eigen::Vector3f> TargetPosition;
			Eigen::Vector3f sumNormals(0, 0, 0);
			Eigen::Vector3f sumNormalsMesh2(0, 0, 0);
			float max_distance = 0;

			for (int i = 0; i < goodVerticesNew.size(); i++) {
				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(goodVerticesNew.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(goodVerticesNew.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(goodVerticesNew.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));

					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();

			sumNormals = -1.0 * sumNormals;

			for (int i = 0; i < affectedTrianglesSorted2.size(); i++) {
				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));

					sumNormalsMesh2 = sumNormalsMesh2 + Normal;
				}
			}

			sumNormalsMesh2.normalize();
			float approximateD = findFittingD(MeshVertices, affectedVertices2, sumNormalsMesh2);

			for (int i = 0; i < goodVerticesNew.size(); i++) {
				Eigen::Vector3f Vertex = MeshVertices.at(goodVerticesNew.at(i));
				Line pLine(Vertex, sumNormals);
				float temp = linePlaneIntersection(pLine, sumNormalsMesh2, -1 * approximateD);
				if (temp > max_distance) {
					max_distance = temp;
				}
			}

			for (int i = 0; i < goodVerticesNew.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(goodVerticesNew.at(i));
				temp = temp + (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}

			return TargetPosition;
		}

		std::vector<Eigen::Vector3f> getTargetPositionVersion5(int frame, std::vector<int32_t> affectedTrianglesSorted, std::vector<int32_t> allAffectedTriangles, std::vector<int32_t> triangleVertices, std::vector<Eigen::Vector3f> MeshVertices, Bone* boneMesh1, Bone* boneMesh2, int AnimationID) {

			std::vector<Eigen::Vector3f> TargetPosition;

			Eigen::Vector3f sumNormals(0, 0, 0);
			float max_distance = 0.0f;
			//printf("%d\n", max_distance);
			for (int i = 0; i < affectedTrianglesSorted.size(); i++) {

				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));

					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();
			sumNormals = -1 * sumNormals;
			float numDistances = 0;


			for (int i = 0; i < affectedTrianglesSorted.size(); i++) {
				Eigen::Vector3f tri[3] = {
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[0]),
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[1]),
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[2]) };

				Line Capsule = getCapsuleDirection(boneMesh2, AnimationID, frame);
				float distance = capsuleTriangleIntersectionTest(Capsule, boneMesh2->capsuleRadius, tri);

				if (distance > 0) {
					max_distance = max_distance + distance;
					numDistances++;
				}

			}

			max_distance = max_distance / (affectedTrianglesSorted.size() * 1.0f);
			if (max_distance > m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.at(frame))
				m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.at(frame) = max_distance;


			for (int i = 0; i < triangleVertices.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(triangleVertices.at(i));
				temp = temp + (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}

			return TargetPosition;
		}

		std::vector<Eigen::Vector3f> getTargetPositionVersion6(int frame, std::vector<int32_t> affectedTrianglesSorted, std::vector<int32_t> affectedTrianglesSorted2, std::vector<int32_t> allAffectedTriangles, std::vector<int32_t> triangleVertices, std::vector<Eigen::Vector3f> MeshVertices, Bone* boneMesh1, Bone* boneMesh2, int AnimationID) {

			std::vector<Eigen::Vector3f> TargetPosition;

			Eigen::Vector3f sumNormals(0, 0, 0);
			float max_distance = 0.0f;
			//printf("%d\n", max_distance);
			for (int i = 0; i < affectedTrianglesSorted2.size(); i++) {

				int32_t IndexNormal[3];
				IndexNormal[0] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[0];
				IndexNormal[1] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[1];
				IndexNormal[2] = m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted2.at(i)).Vertices[2];

				for (int j = 0; j < 3; j++) {
					Eigen::Vector3f Normal = Eigen::Vector3f(this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 0),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 1),
						this->Tri_Normal_Deformations.at(frame)(IndexNormal[j] * 3 + 2));

					sumNormals = sumNormals + Normal;
				}
			}

			sumNormals.normalize();

			float numDistances = 0;


			for (int i = 0; i < affectedTrianglesSorted.size(); i++) {
				Eigen::Vector3f tri[3] = {
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[0]),
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[1]),
				MeshVertices.at(m_BoneSubmeshes.at(0)->Faces.at(affectedTrianglesSorted.at(i)).Vertices[2]) };

				Line Capsule = getCapsuleDirection(boneMesh2, AnimationID, frame);
				float distance = capsuleTriangleIntersectionTest(Capsule, boneMesh2->capsuleRadius, tri);

				if (distance > 0) {
					max_distance = max_distance + distance;
					numDistances++;
				}

			}

			max_distance = max_distance / (affectedTrianglesSorted2.size() * 1.0f);
			if (max_distance > m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.at(frame))
				m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.at(frame) = max_distance;


			for (int i = 0; i < triangleVertices.size(); i++) {
				Eigen::Vector3f temp = MeshVertices.at(triangleVertices.at(i));
				temp = temp + (max_distance * sumNormals);
				TargetPosition.push_back(temp);
			}

			return TargetPosition;
		}

		void overwriteCurrentOriginal(int32_t BoneID, int frame, int AnimationID) {
			lastOriginal->BoneIDs.clear();
			lastOriginal->Rotations.clear();
			lastOriginal->Frame = frame;

			int numParents;

			switch (BoneID) {
			case 8:
			case 12:
				numParents = 0;
				break;
			case 9:
			case 13:
				numParents = 1;
				break;
			case 10:
			case 14:
				numParents = 2;
				break;;
			default:
				numParents = 0;
				break;
			}


			for (int i = numParents; i >= 0; i--) {
				frameWasChanged.at(BoneID - i).at(frame) = true;
				lastOriginal->BoneIDs.push_back(BoneID - i);
				Eigen::Quaternionf Rotation = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(BoneID - i)->Rotations.at(frame);
				lastOriginal->Rotations.push_back(Rotation);
			}
		}

		void overwriteLastChanges(int32_t BoneID, int frame, int AnimationID) {
			ChangedKeyframes* pToChange = m_ChangedFrames.at(BoneID);
			pToChange->Frame = frame;
			pToChange->BoneIDs.clear();
			pToChange->Rotations.clear();

			for (int i = 0; i < lastOriginal->BoneIDs.size(); i++) {
				int32_t Bone = lastOriginal->BoneIDs.at(i);
				Eigen::Quaternionf Change = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(Bone)->Rotations.at(frame) * lastOriginal->Rotations.at(i).inverse();
				pToChange->BoneIDs.push_back(Bone);
				pToChange->Rotations.push_back(Change);
			}

		}

		float SolveWithInverseKinematic(int frame, int AnimationID, int32_t Version, Bone* boneMesh1, Bone* boneMesh2, std::vector<Eigen::Vector2i> affectedTriangles, std::vector<bool> checkMesh1, Submesh* Submesh1, Submesh* Submesh2, std::vector<Eigen::Vector3f> MeshVertices) {
			float max_distance = 0;
			std::vector<int32_t> affectedTrianglesSorted = getAffectedTriangles(affectedTriangles, 0);
			std::vector<int32_t> affectedTrianglesSorted2 = getAffectedTriangles(affectedTriangles, 1);
			std::vector<int32_t> affectedVertices = getAffectedVertices(affectedTrianglesSorted);

			std::vector<Eigen::Vector3f> TargetPosition;

			int32_t BoneID = Submesh1->ID - 1;

			float remainingError = 0;
			Pose pPose = copyFrame(frame, AnimationID);

			bool thisEditedBefore = frameWasChanged.at(BoneID).at(frame);
			/*if (this->lastOriginal->Frame > -1) {
				if (this->lastOriginal->Frame == frame && this->lastOriginal->BoneIDs.at(lastOriginal->BoneIDs.size() - 1) == BoneID) {
					thisEditedBefore = true;
				}
			}*/
			if (thisEditedBefore == false) {
				overwriteCurrentOriginal(BoneID, frame, AnimationID);
				while (m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.size() < frame + 1) {
					m_SkeletalAnimations.at(AnimationID)->Keyframes.at(boneMesh1->ID)->collisionDepths.push_back(0.0f);
				}
			}

			bool beforeEditedBefore = false;
			if (enableInitialGuess == true) {
				if (m_ChangedFrames.at(BoneID)->Frame == frame - 1) {
					beforeEditedBefore = true;
				}
			}


			if (thisEditedBefore == true || beforeEditedBefore == false) {
				Pose newFrame;
				std::vector<Eigen::Vector3f> end;

				if (Version == 1) {

					std::vector<int32_t> forbiddenVertices;
					std::vector<int32_t> TrianglesToCheck;

					for (int i = 0; i < checkMesh1.size(); i++) {
						if (checkMesh1.at(i) == true) {
							TrianglesToCheck.push_back(i);
						}
						else {
							for (int j = 0; j < 3; j++) {
								int32_t VertIndex = Submesh1->Faces.at(i).Vertices[j];
								if (forbiddenVertices.size() != 0) {
									int32_t Index = findNewPosition(forbiddenVertices, VertIndex);
									if (forbiddenVertices.at(Index) != VertIndex) {
										if (Index == forbiddenVertices.size() - 1 && forbiddenVertices.at(Index) < VertIndex) {
											forbiddenVertices.push_back(VertIndex);
										}
										else {
											forbiddenVertices.insert(forbiddenVertices.begin() + Index, VertIndex);
										}
									}
								}
								else {
									forbiddenVertices.push_back(VertIndex);
								}
							}
						}
					}

					std::vector<int32_t> affectedVertices2 = getAffectedVertices(affectedTrianglesSorted2);
					std::vector<int32_t> goodVerticesNew = getGoodVertices(frame, Submesh1, forbiddenVertices, TrianglesToCheck, affectedTriangles, MeshVertices);

					TargetPosition = getTargetPositionVersion1(frame, goodVerticesNew, affectedTrianglesSorted2, affectedVertices2, MeshVertices);
					newFrame = computeInverseKinematic(Submesh1->ID, goodVerticesNew, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, goodVerticesNew);

					forbiddenVertices.clear();
					TrianglesToCheck.clear();
					goodVerticesNew.clear();
					affectedVertices2.clear();
				}
				else if (Version == 2) {

					TargetPosition = getTargetPositionVersion2(frame, affectedTriangles, affectedVertices, affectedTrianglesSorted2, MeshVertices);
					newFrame = computeInverseKinematic(Submesh1->ID, affectedVertices, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, affectedVertices);
				}
				else if (Version == 4) {

					std::vector<int32_t> forbiddenVertices;
					std::vector<int32_t> TrianglesToCheck;

					for (int i = 0; i < checkMesh1.size(); i++) {
						if (checkMesh1.at(i) == true) {
							TrianglesToCheck.push_back(i);
						}
						else {
							for (int j = 0; j < 3; j++) {
								int32_t VertIndex = Submesh1->Faces.at(i).Vertices[j];
								if (forbiddenVertices.size() != 0) {
									int32_t Index = findNewPosition(forbiddenVertices, VertIndex);
									if (forbiddenVertices.at(Index) != VertIndex) {
										if (Index == forbiddenVertices.size() - 1 && forbiddenVertices.at(Index) < VertIndex) {
											forbiddenVertices.push_back(VertIndex);
										}
										else {
											forbiddenVertices.insert(forbiddenVertices.begin() + Index, VertIndex);
										}
									}
								}
								else {
									forbiddenVertices.push_back(VertIndex);
								}
							}
						}
					}


					std::vector<int32_t> affectedVertices2 = getAffectedVertices(affectedTrianglesSorted2);
					std::vector<int32_t> goodVerticesNew = getGoodVertices(frame, Submesh1, forbiddenVertices, TrianglesToCheck, affectedTriangles, MeshVertices);

					TargetPosition = getTargetPositionVersion4(frame, goodVerticesNew, affectedTrianglesSorted2, affectedVertices2, MeshVertices);
					newFrame = computeInverseKinematic(Submesh1->ID, goodVerticesNew, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, goodVerticesNew);

					forbiddenVertices.clear();
					TrianglesToCheck.clear();
					goodVerticesNew.clear();
					affectedVertices2.clear();
				}
				else if (Version == 5) {
					std::vector<int32_t> triangleVertices = getAffectedVertices(affectedTrianglesSorted);
					TargetPosition = getTargetPositionVersion5(frame, affectedTrianglesSorted, affectedTrianglesSorted, triangleVertices, MeshVertices, boneMesh1, boneMesh2, AnimationID);
					newFrame = computeInverseKinematic(Submesh1->ID, triangleVertices, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, triangleVertices);

					triangleVertices.clear();
				}
				else if (Version == 6) {
					std::vector<int32_t> triangleVertices = getAffectedVertices(affectedTrianglesSorted);
					TargetPosition = getTargetPositionVersion6(frame, affectedTrianglesSorted, affectedTrianglesSorted2, affectedTrianglesSorted, triangleVertices, MeshVertices, boneMesh1, boneMesh2, AnimationID);
					newFrame = computeInverseKinematic(Submesh1->ID, triangleVertices, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, triangleVertices);

					triangleVertices.clear();

				}
				else {

					TargetPosition = getTargetPositionVersion3(frame, affectedTriangles, affectedVertices, affectedTrianglesSorted, MeshVertices);
					newFrame = computeInverseKinematic(Submesh1->ID, affectedVertices, pPose, TargetPosition, DeltaTheta, LearningRate, Epsilon);
					end = getEndeffector(newFrame, affectedVertices);
				}

				remainingError = computeError(TargetPosition, end);
				pPose = newFrame;

				end.clear();

			}
			else {
				Pose thisFrame = copyFrame(frame, AnimationID);

				for (int i = 0; i < m_ChangedFrames.at(BoneID)->BoneIDs.size(); i++) {
					int32_t Bone = m_ChangedFrames.at(BoneID)->BoneIDs.at(i);
					Eigen::Quaternionf fullChange = thisFrame.Rotations.at(Bone) * m_ChangedFrames.at(BoneID)->Rotations.at(i);
					fullChange.normalize();
					thisFrame.Rotations.at(Bone) = thisFrame.Rotations.at(Bone).slerp(initialGuess, fullChange);
				}

				thisFrame = updateSkeletonHistory(thisFrame);

				remainingError = Epsilon + 1.0f;

				pPose = thisFrame;
			}

			//calculateChanges(pPose, newFrame, frame);
			for (int i = 0; i < 23; i++) {
				this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(i)->Rotations.at(frame) = pPose.Rotations.at(i);
				this->m_SkeletalAnimations.at(AnimationID)->Keyframes.at(i)->SkinningMatrix.at(frame) = pPose.SkinningMatrix.at(i);
			}
			if (enableInitialGuess == true) {
				overwriteLastChanges(BoneID, frame, AnimationID);
			}

			TargetPosition.clear();
			affectedTrianglesSorted.clear();
			affectedTrianglesSorted2.clear();
			affectedVertices.clear();

			return remainingError;
		}

		Pose computeInverseKinematic(int SubmeshID, std::vector<int32_t> affectedVertices, Pose pPose, std::vector<Eigen::Vector3f> targetPosition, float DeltaTheta, float fLearningRate, float fEpsilon) {

			RotationAxis* pRotationAxis = aRotationAxis.at(SubmeshID - 1);

			float pGradients[23]; // gradients

			for (int i = 0; i < 23; i++) {
				pGradients[i] = 0;
			}

			int startBone;
			int endBone;

			switch (SubmeshID) {
			case 9:
				startBone = 8;
				endBone = 8;
				break;
			case 10:
				startBone = 8;
				endBone = 9;
				break;
			case 11:
				startBone = 8;
				endBone = 10;
				break;
			case 13:
				startBone = 12;
				endBone = 12;
				break;
			case 14:
				startBone = 12;
				endBone = 13;
				break;
			case 15:
				startBone = 12;
				endBone = 14;
				break;
			default:
				return pPose;
				break;
			}


			std::vector<Eigen::Vector3f> EndPosition = getEndeffector(pPose, affectedVertices); // current position of the end-effector
			float fRemainingError = computeError(EndPosition, targetPosition); // error value for current pose
			Pose P; // temporary object necessary for computations

			uint32_t unIterationCount = 0; // failsafe to prohibit endless computations
			// iterate until end effector's position is close to target position
			//fEpsilon is the aspired remainingError
			while (unIterationCount < 20 && fRemainingError > fEpsilon/*... Exercise 13 - Task 1.(e)...*/) {
				// copy Pose for this iteration
				P = pPose;

				// compute all gradients (partial derivative for each joint)
				for (int32_t i = startBone; i <= endBone; ++i) {

					// Exercise 13 - Task 1.(b)
					// create Delta Theta quaternion for this joint
					// hint: glm::toQuat converts a rotation matrix to a quaternion
					Eigen::AngleAxisf Angle(DeltaTheta, pRotationAxis->Axis[i]);
					Eigen::Quaternionf DeltaQ = Quaternionf(Angle);

					// apply change to joint
					P.Rotations.at(i) *= DeltaQ;
					//P.Rotations.at(i).normalize();

					// update transformation hierarchy
					P = updateSkeletonHistory(P);

					// Exercise 13 - Task 1.(c)
					// compute gradient
					std::vector<Eigen::Vector3f> s = getEndeffector(P, affectedVertices);
					float error1 = computeError(s, targetPosition);
					float error2 = computeError(EndPosition, targetPosition);
					pGradients[i] = (error1 - error2) / DeltaTheta;

					// revert change to pose so gradient for next joint is not affected
					P.Rotations.at(i) *= (DeltaQ.inverse());
				}//for[joints]

				// update pose
				for (int32_t i = startBone; i <= endBone; ++i) {
					// Exercise 13 - Task 1.(d)
					// compute change based on gradient and learning rate, than apply to pose's joint
					float fChange = -fLearningRate * pGradients[i];
					Eigen::AngleAxisf Angle2(fChange, pRotationAxis->Axis[i]);
					pPose.Rotations.at(i) *= Quaternionf(Angle2);
				}//for[joints]

				pPose = updateSkeletonHistory(pPose);
				// update EndPosition
				EndPosition = getEndeffector(pPose, affectedVertices);
				fRemainingError = computeError(EndPosition, targetPosition);
				++unIterationCount;
			}//while[not close to target position]

			pRotationAxis = nullptr;
			delete[] pRotationAxis;

			return pPose;
		}

		bool findInVector(std::vector<int32_t> Vector, int32_t wanted) {

			for (int i = 0; i < Vector.size(); i++) {
				if (Vector.at(i) == wanted)
					return true;
			}
			return false;
		}

		bool findInVector2(std::vector<int32_t> Vector, int32_t wanted) {

			int32_t index = findNewPosition(Vector, wanted);

			if (Vector.at(index) == wanted)
				return true;
			else return false;

		}

		int findNewPosition(std::vector<int32_t> Vector, int32_t toBeInserted) {

			bool found = false;
			int low = 0;
			int high = Vector.size() - 1;
			int currentIndex = 0;

			while (high >= low) {
				currentIndex = low + (high - low) / 2;
				int32_t currentValue = Vector.at(currentIndex);
				if (currentValue == toBeInserted)
					return currentIndex;
				else {
					if (currentValue < toBeInserted) {
						low = currentIndex + 1;
					}
					else {
						high = currentIndex - 1;
					}
				}
			}
			return currentIndex;

		}

		std::vector<int32_t> findAdditionalVertices(Submesh* Mesh, std::vector<int32_t> goodVertices, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> trianglesToCheck) {

			int currentTriToCheck = 0;
			int lastSize = trianglesToCheck.size();

			while (trianglesToCheck.size() > 0) {

				//go from beginning to end
				while (currentTriToCheck < trianglesToCheck.size()) {
					//3 verts of a Triangle
					int32_t verts[3] = { Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[0],
										Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[1],
										Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[2] };
					//bad ones are outside of BB or outside of other Mesh
					bool foundBad[3] = { false, false, false };
					if (findInVector(forbiddenVertices, verts[0]) == true) {
						foundBad[0] = true;
					}
					if (findInVector(forbiddenVertices, verts[1]) == true) {
						foundBad[1] = true;
					}
					if (findInVector(forbiddenVertices, verts[2]) == true) {
						foundBad[2] = true;
					}

					//good ones are inside other mesh
					bool foundGood[3] = { false, false, false };

					if (foundBad[0] == false) {
						if (findInVector(goodVertices, verts[0]) == true) {
							foundGood[0] = true;
						}
					}
					if (foundBad[1] == false) {
						if (findInVector(goodVertices, verts[1]) == true) {
							foundGood[1] = true;
						}
					}
					if (foundBad[2] == false) {
						if (findInVector(goodVertices, verts[2]) == true) {
							foundGood[2] = true;
						}
					}


					if (foundBad[0] == false && foundBad[1] == false && foundBad[2] == false) {
						if (foundGood[0] && foundGood[1] || foundGood[1] && foundGood[2] || foundGood[0] && foundGood[2]) {


							if (foundGood[0] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[0]);

									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[0]) {
										goodVertices.push_back(verts[0]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[0]);
									}
								}
								else {
									goodVertices.push_back(verts[0]);
								}
							}

							if (foundGood[1] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[1]);
									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[1]) {
										goodVertices.push_back(verts[1]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[1]);
									}
								}
								else {
									goodVertices.push_back(verts[1]);
								}
							}

							if (foundGood[2] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[2]);
									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[2]) {
										goodVertices.push_back(verts[2]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[2]);
									}
								}
								else {
									goodVertices.push_back(verts[2]);
								}
							}

							trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);


						}
						else {
							currentTriToCheck++;
						}
					}
					else {
						trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);
					}

				}
				if (lastSize > trianglesToCheck.size()) {
					currentTriToCheck = 0;
					lastSize = trianglesToCheck.size();
				}
				else {
					trianglesToCheck.clear();
				}

			}

			return goodVertices;
		}

		std::vector<int32_t> findTrianglesInside(Submesh* Mesh, std::vector<int32_t> goodVertices, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> trianglesToCheck) {

			std::vector<int32_t> trianglesInside;
			int currentTriToCheck = 0;
			int lastSize = trianglesToCheck.size();

			while (trianglesToCheck.size() > 0) {

				//go from beginning to end
				while (currentTriToCheck < trianglesToCheck.size()) {
					//3 verts of a Triangle
					int32_t verts[3] = { Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[0],
										Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[1],
										Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[2] };
					//bad ones are outside of BB or outside of other Mesh
					bool foundBad[3] = { false, false, false };
					if (findInVector(forbiddenVertices, verts[0]) == true) {
						foundBad[0] = true;
					}
					if (findInVector(forbiddenVertices, verts[1]) == true) {
						foundBad[1] = true;
					}
					if (findInVector(forbiddenVertices, verts[2]) == true) {
						foundBad[2] = true;
					}

					//good ones are inside other mesh
					bool foundGood[3] = { false, false, false };

					if (foundBad[0] == false) {
						if (findInVector(goodVertices, verts[0]) == true) {
							foundGood[0] = true;
						}
					}
					if (foundBad[1] == false) {
						if (findInVector(goodVertices, verts[1]) == true) {
							foundGood[1] = true;
						}
					}
					if (foundBad[2] == false) {
						if (findInVector(goodVertices, verts[2]) == true) {
							foundGood[2] = true;
						}
					}


					if (foundBad[0] == false && foundBad[1] == false && foundBad[2] == false) {
						if (foundGood[0] && foundGood[1] || foundGood[1] && foundGood[2] || foundGood[0] && foundGood[2]) {


							if (foundGood[0] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[0]);

									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[0]) {
										goodVertices.push_back(verts[0]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[0]);
									}
								}
								else {
									goodVertices.push_back(verts[0]);
								}
							}

							if (foundGood[1] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[1]);
									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[1]) {
										goodVertices.push_back(verts[1]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[1]);
									}
								}
								else {
									goodVertices.push_back(verts[1]);
								}
							}

							if (foundGood[2] == false) {
								if (goodVertices.size() != 0) {
									int index = findNewPosition(goodVertices, verts[2]);
									if (index == goodVertices.size() - 1 && goodVertices.at(index) < verts[2]) {
										goodVertices.push_back(verts[2]);
									}
									else {
										goodVertices.insert(goodVertices.begin() + index, verts[2]);
									}
								}
								else {
									goodVertices.push_back(verts[2]);
								}
							}

							trianglesInside.push_back(trianglesToCheck.at(currentTriToCheck));
							trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);


						}
						else {
							currentTriToCheck++;
						}
					}
					else {
						trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);
					}

				}
				if (lastSize > trianglesToCheck.size()) {
					currentTriToCheck = 0;
					lastSize = trianglesToCheck.size();
				}
				else {
					trianglesToCheck.clear();
				}

			}

			return trianglesInside;
		}


		//----------- de Boor
		//k current degree
		//p = degree
		//i index of Intervall, that u falls in
		//u position on curve
		//knots = intervalls, eg (0,1,2,3,4) for intervalls [0,1), [1,2), [2,3), [3,4)
		//Positions of Control Points, #controlpoints = n, 0...n-1 
		//pad vector, so that first and last controlpoint * #degree+1
		//n >= 2p


		void Test(void) {
			Line Capsule(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 4, 0));
			float capsuleRadius = 3.0f;
			Eigen::Vector3f tri[3] = { Eigen::Vector3f(0, 2, 1), Eigen::Vector3f(0, 2, 2),Eigen::Vector3f(1, 2, 1) };
			float depth1 = sphereTriangleIntersectionTest(tri, Eigen::Vector3f(0, 2, 0), capsuleRadius);

			float penetrationDepth = capsuleTriangleIntersectionTest(Capsule, capsuleRadius, tri);

		}


		//Functions needed to compute Quaternion B-Spline
		float deBoor(int degree, int i, float u, std::vector<float> knots) {
			if (degree == 0) {
				if (knots.at(i) <= u && u < knots.at(i + 1)) {
					return 1;
				}
				else {
					return 0;
				}
			}
			else {
				float Deboor1 = deBoor(degree - 1, i, u, knots);
				float Deboor2 = deBoor(degree - 1, i + 1, u, knots);
				float alpha1 = -1;
				float alpha2 = -1;

				if (Deboor1 == 0) {
					alpha1 = 0;
				}
				if (Deboor2 == 0) {
					alpha2 = 0;
				}
				if (alpha1 != 0) {
					float temp1 = u - knots.at(i);
					float temp2 = knots.at(i + degree) - knots.at(i);
					if (temp2 == 0 && temp1 == 0)
						alpha1 = 0;
					else {
						alpha1 = temp1 / temp2;
					}
				}
				if (alpha2 != 0) {
					float temp1 = knots.at(i + degree + 1) - u;
					float temp2 = knots.at(i + degree + 1) - knots.at(i + 1);
					if (temp2 == 0 && temp1 == 0)
						alpha2 = 0;
					else {
						alpha2 = temp1 / temp2;
					}
				}
				float Basis = (alpha1 * Deboor1 + alpha2 * Deboor2);

				return Basis;
			}
		}

		float otherDeBoor(int degree, int i, float u, std::vector<float> knots) {
			float Rval = 0;
			int k = degree + 1;

			for (int j = i; j <= (knots.size() - 2 - degree); j++) {
				Rval = Rval + deBoor(degree, j, u, knots);
			}
			return Rval;
		}

		float quatLength(Eigen::Quaternionf q) {
			return sqrt(q.w() * q.w() + q.x() * q.x() + q.y() * q.y() + q.z() * q.z());
		}

		Eigen::Quaternionf quatPow(Eigen::Quaternionf q, float t) {
			//q.normalize();
			Eigen::Quaternionf logq = quatLog(q);
			logq.w() *= t;
			logq.x() *= t;
			logq.y() *= t;
			logq.z() *= t;
			return quatExp(logq);

			/*double phi = acosh((q.w() * 180.0) / M_PI);
			float qpow = pow(quatLength(q), t);
			Eigen::Vector3f v_norm(q.x(), q.y(), q.z());
			v_norm.normalize();
			v_norm = qpow * v_norm * (sin((t * phi * 180.0) / M_PI));
			float new_w = qpow * (cos((phi * t * 180.0) / M_PI));*/
			//return Eigen::Quaternionf(new_w, v_norm.x(), v_norm.y(), v_norm.z());
		}

		Eigen::Quaternionf quatExp(Eigen::Quaternionf q) {
			Eigen::Vector3f v(q.x(), q.y(), q.z());
			float epowa = pow(M_E, q.w());
			float vLength = vectorLength(v);
			float new_w = epowa * cos(vLength);
			v.normalize();
			v = epowa * v * sin(vLength);
			return Eigen::Quaternionf(new_w, v.x(), v.y(), v.z());
		}

		Eigen::Quaternionf quatLog(Eigen::Quaternionf q) {
			//q.normalize();
			Eigen::Vector3f v_norm(q.x(), q.y(), q.z());
			v_norm.normalize();
			Eigen::Quaternionf Rval(1, 0, 0, 0);
			float qMag = quatLength(q);
			Rval.w() = log(qMag);
			float rq = q.w() / qMag;
			rq = acos(rq);
			v_norm = v_norm * rq;
			Rval.x() = v_norm.x();
			Rval.y() = v_norm.y();
			Rval.z() = v_norm.z();

			return Rval;
		}

		//Compute Quaternion B-Spline
		Eigen::Quaternionf quatCurve(int degree, float u, std::vector<float> knots, std::vector<Eigen::Quaternionf> qi) {
			int l = 0;
			Eigen::Quaternionf q(1, 0, 0, 0);

			for (int i = -1 * degree; knots.at(i + degree) <= u; i++) {
				l = i;
			}

			if (l < 0) {
				l = 0;
				q = quatPow(qi.at(0), otherDeBoor(degree, 0, u, knots));
			}
			else {
				q = qi.at(l);
			}
			for (int i = l + 1; i < qi.size() && knots.at(i) < u; i++) {
				Eigen::Quaternionf logq = quatLog(qi.at(i - 1).inverse() * qi.at(i));
				float otherDeboorIK = otherDeBoor(degree, i, u, knots);
				logq.w() *= otherDeboorIK;
				logq.x() *= otherDeboorIK;
				logq.y() *= otherDeboorIK;
				logq.z() *= otherDeboorIK;

				Eigen::Quaternionf expq = quatExp(logq);
				q = q * expq;

			}

			return q;
		}

		//constructs the knot vector
		void curveFitting(int startFrame, int endFrame, std::vector<Eigen::Quaternionf> qi, int BoneID, int AnimationID, int degree) {

			std::vector<int> Frames;
			for (int i = startFrame; i <= endFrame; i++) {
				Frames.push_back(i);
			}

			std::vector<float> knots;
			int n = (qi.size() - 1);
			int k = degree + 1;
			float segments = n - k + 2;

			//turn down degree if no segments possible
			//#segments = n-(degree+1)+2
			while (segments < 1.0) {
				degree = degree - 1;
				segments = (qi.size() - 1) - (degree + 1.0) + 2.0;
			}

			segments = 1 / segments;

			//Construct Knot Vector
			//First and Last Frame in Curve should not be edited, therefore degree+1-x insertion in knot vector
			for (int i = 0; i <= degree; i++) {
				knots.push_back(0.0);
			}
			for (float i = segments; i < 1; (i += segments)) {
				knots.push_back(i);
			}
			for (int i = 0; i <= degree; i++) {
				knots.push_back(1.0);
			}

			//compute step size for frames
			float frameSegments = 1.0 / (qi.size() - 1);

			for (int j = 1; j < (qi.size() - 1); j++) {

				if (Frames.at(j) > startFrame && Frames.at(j) < endFrame) {
					float u = frameSegments * j;
					Eigen::Quaternionf newQ(1, 0, 0, 0);

					/*newQ = quatPow(qi.at(0), otherDeBoor(degree, 0, u, knots));

					for (int i = 1; i < qi.size(); i++) {
						Eigen::Quaternionf wi = quatLog(qi.at(i - 1).inverse() * qi.at(i));
						float deb = otherDeBoor(degree, i, u, knots);
						wi.w() *= deb;
						wi.x() *= deb;
						wi.y() *= deb;
						wi.z() *= deb;
						newQ *= quatExp(wi);
					}

					newQ.normalize();*/

					//compute rotation at current frame
					newQ = quatCurve(degree, u, knots, qi);
					newQ.normalize();
					m_SkeletalAnimations.at(AnimationID)->Keyframes.at(BoneID)->Rotations.at(Frames.at(j)) = newQ;

				}

			}
			Frames.clear();
			knots.clear();
		}

		//ignore, just testing
		void afterMatch2(int AnimationID, int degree) {
			int BoneCount = this->frameWasChanged.size();
			int FrameCount = this->frameWasChanged.at(0).size();
			std::vector<Eigen::Quaternionf> qi;
			int startFrame;
			int endFrame;

			for (int j = 0; j < BoneCount; j++) {
				if (j == 8 || j == 9 || j == 10 || j == 12 || j == 13 || j == 14) {
					//try to make a full bspline of all frames
					startFrame = INFINITY;
					endFrame = INFINITY;
					for (int i = 0; i < FrameCount; i++) {

						if (startFrame > FrameCount) {
							if (this->frameWasChanged.at(j).at(i) == false) {
								if ((i + 1) < this->frameWasChanged.at(0).size()) {
									if (this->frameWasChanged.at(j).at(i + 1) == true) {
										startFrame = i;
										Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
										qi.push_back(q);
									}
								}
							}
						}
						else {
							if (this->frameWasChanged.at(j).at(i) == false) {

								int search = 3;
								bool found = false;
								int index = 1;

								while (found == false && index < 2) {

									if ((i + index) < this->frameWasChanged.at(0).size()) {
										if (this->frameWasChanged.at(j).at(i + index) == true) {
											found = true;
										}
										else {
											index++;
										}
									}
									else {
										index = 2;
									}
								}

								if (found == true) {
									for (int r = 0; r <= index; r++) {
										Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
										qi.push_back(q);
										i++;
									}
								}
								else {
									Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
									qi.push_back(q);
									endFrame = i;

									curveFitting(startFrame, endFrame, qi, j, AnimationID, degree);

									qi.clear();
									startFrame = INFINITY;
									endFrame = INFINITY;
									i = i + index - 1;
								}
							}
							else {
								Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
								qi.push_back(q);
							}

						}


					}
					if (qi.size() > 0) {
						endFrame = FrameCount - 1;
						curveFitting(startFrame, endFrame, qi, j, AnimationID, degree);
					}

					qi.clear();
				}

			}

			qi.clear();

		}

		//constructs the control points
		//A curve starts with an unedited Frame and ends with last frame in Animation or next unedited Frame
		void afterMatch(int AnimationID, int degree) {
			int BoneCount = this->frameWasChanged.size();
			int FrameCount = this->frameWasChanged.at(0).size();
			std::vector<Eigen::Quaternionf> qi;
			int startFrame;
			int endFrame;

			for (int j = 0; j < BoneCount; j++) {

				startFrame = 0;
				endFrame = 0;
				for (int i = 0; i < FrameCount; i++) {

					//if current Frame is false (not edited)
					if (this->frameWasChanged.at(j).at(i) == false) {
						//and next is true, then it's the beginning of the curve
						//if both not edited then ignore current frame
						if ((i + 1) < this->frameWasChanged.at(0).size()) {
							if (this->frameWasChanged.at(j).at(i + 1) == true) {
								startFrame = i;
								Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
								qi.push_back(q);
							}
						}
					}
					//if the current frame is true
					else {
						if (i + 1 < this->frameWasChanged.at(0).size()) {
							//and next is false, then it's the end of the curve
							if (this->frameWasChanged.at(j).at(i + 1) == false) {


								Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
								qi.push_back(q);

								Eigen::Quaternionf q2 = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i + 1);
								qi.push_back(q2);
								endFrame = i + 1;

								//actually compute B-Spline
								curveFitting(startFrame, endFrame, qi, j, AnimationID, degree);

								qi.clear();

							}
							//if both are true then in the middle of the curve
							else {
								Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
								qi.push_back(q);
							}
						}
						//current is true, but reached end of Animation, end of curve
						else {
							Eigen::Quaternionf q = m_SkeletalAnimations.at(AnimationID)->Keyframes.at(j)->Rotations.at(i);
							qi.push_back(q);
							endFrame = i;

							curveFitting(startFrame, endFrame, qi, j, AnimationID, degree);

							qi.clear();

						}
					}
				}
				qi.clear();

			}

			qi.clear();

		}


		//-------- Collisionfunctions, for now between two Submeshes, where Submesh1 is supposed to be manipulated in case of collision
		bool collisionTest(int Submesh1Index, int Submesh2Index, int frame, int AnimationID, int Version) {
			Eigen::VectorXf MeshDeformation = this->Tri_Deformations.at(frame);

			//Submesh, where Bones shall be updated in case of collision
			Submesh* Submesh1 = this->m_BoneSubmeshes.at(Submesh1Index);
			Bone* boneMesh1 = this->getBone(Submesh1Index - 1);
			//Submesh, that might collide with Submesh1
			Submesh* Submesh2 = this->m_BoneSubmeshes.at(Submesh2Index);
			Bone* boneMesh2 = this->getBone(Submesh2Index - 1);
			//Submeshes need updated Vertex Positions for AABB
			std::vector<Eigen::Vector3f> MeshVertices;

			for (int i = 0; i < this->vertexCount(); i++) {
				Eigen::Vector3f updatedVertex(MeshDeformation(i * 3 + 0), MeshDeformation(i * 3 + 1), MeshDeformation(i * 3 + 2));
				MeshVertices.push_back(updatedVertex);
			}

			AABB mesh1BB = Submesh1->initBB(MeshVertices);
			AABB mesh2BB = Submesh2->initBB(MeshVertices);


			//Sphere Test -------------------------
			bool spheresIntersect = false, boxesIntersect = false, trianglesIntersect = false;
			Eigen::Vector3f center1 = mesh1BB.center();
			Eigen::Vector3f center2 = mesh2BB.center();

			float radius1 = sqrt(mesh1BB.diagonal().dot(mesh1BB.diagonal()));
			float radius2 = sqrt(mesh2BB.diagonal().dot(mesh2BB.diagonal()));

			Eigen::Vector3f distance = center1 - center2;
			spheresIntersect = sqrt(distance.dot(distance)) <= (radius1 + radius2);

			if (!spheresIntersect) return false;


			//Box Test------------------------------

			Eigen::Vector3f lap_min(mesh1BB.Min.x() > mesh2BB.Min.x() ? mesh1BB.Min.x() : mesh2BB.Min.x(),
				mesh1BB.Min.y() > mesh2BB.Min.y() ? mesh1BB.Min.y() : mesh2BB.Min.y(),
				mesh1BB.Min.z() > mesh2BB.Min.z() ? mesh1BB.Min.z() : mesh2BB.Min.z());

			Eigen::Vector3f lap_max(mesh1BB.Max.x() < mesh2BB.Max.x() ? mesh1BB.Max.x() : mesh2BB.Max.x(),
				mesh1BB.Max.y() < mesh2BB.Max.y() ? mesh1BB.Max.y() : mesh2BB.Max.y(),
				mesh1BB.Max.z() < mesh2BB.Max.z() ? mesh1BB.Max.z() : mesh2BB.Max.z());

			boxesIntersect = true;

			if (lap_min.x() > lap_max.x()) boxesIntersect = false;
			if (lap_min.y() > lap_max.y()) boxesIntersect = false;
			if (lap_min.z() > lap_max.z()) boxesIntersect = false;


			if (!boxesIntersect) return false;

			//Check for Faces of Mesh2, whether they're inside overlapping BB or not
			std::vector <bool> checkMesh1;
			std::vector <bool> checkMesh2;

			for (auto i : Submesh2->Faces) {
				Eigen::Vector3f tri[3];
				tri[0] = MeshVertices[i.Vertices[0]];
				tri[1] = MeshVertices[i.Vertices[1]];
				tri[2] = MeshVertices[i.Vertices[2]];

				checkMesh2.push_back(inBoundingBox(tri, lap_min, lap_max));
			}

			for (auto i : Submesh1->Faces) {
				Eigen::Vector3f tri[3];
				tri[0] = MeshVertices[i.Vertices[0]];
				tri[1] = MeshVertices[i.Vertices[1]];
				tri[2] = MeshVertices[i.Vertices[2]];

				checkMesh1.push_back(inBoundingBox(tri, lap_min, lap_max));
			}

			//Triangle Test
			std::vector<Eigen::Vector2i> affectedTriangles;

			for (int i = 0; i < Submesh1->Faces.size(); i++) {
				if (checkMesh1.at(i) == false)
					continue;

				Eigen::Vector3f tri1[3];
				tri1[0] = MeshVertices[Submesh1->Faces.at(i).Vertices[0]];
				tri1[1] = MeshVertices[Submesh1->Faces.at(i).Vertices[1]];
				tri1[2] = MeshVertices[Submesh1->Faces.at(i).Vertices[2]];

				for (int j = 0; j < Submesh2->Faces.size(); j++) {

					if (checkMesh2.at(j) == false) {
						continue;
					}

					Eigen::Vector3f tri2[3];
					tri2[0] = MeshVertices[Submesh2->Faces.at(j).Vertices[0]];
					tri2[1] = MeshVertices[Submesh2->Faces.at(j).Vertices[1]];
					tri2[2] = MeshVertices[Submesh2->Faces.at(j).Vertices[2]];

					if (triangleIntersection(tri1, tri2) == false)
						continue;

					// Sonst: wir haben (endlich) einen Schnitt gefunden!
					trianglesIntersect = true;
					affectedTriangles.push_back(Eigen::Vector2i(Submesh1->FaceID.at(i), Submesh2->FaceID.at(j)));
				}
			}

			float newError = 0;
			//do stuff with colliding triangles
			if (affectedTriangles.size() > 0) {
				newError = SolveWithInverseKinematic(frame, AnimationID, Version, boneMesh1, boneMesh2, affectedTriangles, checkMesh1, Submesh1, Submesh2, MeshVertices);
			}

			if (newError > Epsilon) {
				trianglesIntersect = true;
			}
			else trianglesIntersect = false;

			affectedTriangles.clear();
			MeshVertices.clear();
			checkMesh1.clear();
			checkMesh2.clear();
			Submesh1 = nullptr;
			Submesh2 = nullptr;
			delete[] Submesh1;
			delete[] Submesh2;

			return trianglesIntersect;
		}

		void resolveCollisions(int32_t BoneIDs1[4], int32_t BoneIDs2[4], int32_t startFrame, int32_t endFrame, int32_t numFrames, int32_t numAnimations, int32_t AnimationToEdit, int32_t Version, int32_t numIterations) {

			//Sort Bones starting from Root / Hip
			this->sortBones(numAnimations);
			//compile all existing Submeshes into m_BoneSubmeshes(0) and add Submeshes for each Bone
			this->initBoneSubmeshes(0.0f);

			//calculate Mesh Deformation
			this->interpolateKeyframes(numFrames, this->getBone(0), AnimationToEdit);
			this->setSkinningMats(0, numFrames - 1, AnimationToEdit, 0);

			setCapsuleRadius(getBone(BoneIDs2[0]), AnimationToEdit);
			setCapsuleRadius(getBone(BoneIDs2[2]), AnimationToEdit);

			this->setTri_Deformation(0, numFrames - 1, AnimationToEdit);

			//aRotationAxis.at(i), i = BoneID, that is colliding
			// 
			//aRotationAxis.at(i)->RotationAxis.Axis[j], Axis of each Bone j, depending on which Bone/Submesh collides
			//if Bone j can't/shouldn't be influenced when Bone i is colliding, then respective Axis=(0,0,0)
			this->initRotationAxis();

			//if a BoneKeyframe gets changed, then the Original Rotation gets stored along with the timestamp
			//changes to the Original get stored in the skeletalAnimation itself
			//when trying to resolve a collision, if the Bone in the frame before the current one was edited, then
			//start with an initial guess for the targetPose, consisting of the Difference in Rotation (Original and Edited) in the frame before * initialGuess
			this->initChangedFrames(numFrames);

			//Actual collision resolvement
			for (int i = startFrame; i <= endFrame; i++) {


				bool collision = true;
				int iteration = 0;
				//Right Elbow + Right Hip
				/*printf("%d, Right\n", i);
				while (collision == true && iteration < numIterations) {
					//BoneIDs + 1 = BoneSubmeshID
					collision = this->collisionTest(BoneIDs1[0] + 1, BoneIDs2[0] + 1, i, AnimationToEdit, Version);
					this->setSkinningMats(i, i, AnimationToEdit, 0);
					this->setTri_Deformation(i, i, AnimationToEdit);
					iteration++;
				}

				collision = true;
				iteration = 0;*/
				printf("%d, Right\n", i);
				//Right Wrist + Right Hip
				while (collision == true && iteration < numIterations) {
					//BoneIDs + 1 = BoneSubmeshID
					collision = this->collisionTest(BoneIDs1[1] + 1, BoneIDs2[1] + 1, i, AnimationToEdit, Version);
					this->setSkinningMats(i, i, AnimationToEdit, 0);
					this->setTri_Deformation(i, i, AnimationToEdit);
					iteration++;
				}

				iteration = 0;
				collision = true;

				//Left Elbow + Left Hip
				/*printf("%d, Left\n", i);
				while (collision == true && iteration < numIterations) {
					collision = this->collisionTest(BoneIDs1[2] + 1, BoneIDs2[2] + 1, i, AnimationToEdit, Version);
					this->setSkinningMats(i, i, AnimationToEdit, 0);
					this->setTri_Deformation(i, i, AnimationToEdit);
					iteration++;
				}

				collision = true;
				iteration = 0;*/

				printf("%d, Left\n", i);
				//Left Wrist + Left Hip
				while (collision == true && iteration < numIterations) {
					collision = this->collisionTest(BoneIDs1[3] + 1, BoneIDs2[3] + 1, i, AnimationToEdit, Version);
					this->setSkinningMats(i, i, AnimationToEdit, 0);
					this->setTri_Deformation(i, i, AnimationToEdit);
					iteration++;
				}

			}

			//CurveFitting with B-Splines
			afterMatch(AnimationToEdit, 3);

		}

		//Need Submeshes for Bones, if model contains more than one mesh need a Submesh that combines them
		void initBoneSubmeshes(float minWeight) {

			Submesh* combinedMesh = new Submesh;
			combinedMesh->ID = 0;
			for (int i = 0; i < m_Submeshes.size(); i++) {

				for (int j = 0; j < m_Submeshes.at(i)->Faces.size(); j++) {
					combinedMesh->Faces.push_back(m_Submeshes.at(i)->Faces.at(j));
					combinedMesh->FaceID.push_back(combinedMesh->FaceID.size());
				}

			}

			this->m_BoneSubmeshes.push_back(combinedMesh);

			initSubmeshes(m_pRootBone, minWeight);

		}

		void initSubmeshes(Bone* pBone, float minWeight) {

			Submesh* pMesh = new Submesh;
			pMesh->ID = pBone->ID + 1;

			for (int f = 0; f < m_BoneSubmeshes.at(0)->Faces.size(); f++) {

				Face face = m_BoneSubmeshes.at(0)->Faces.at(f);
				int i = 0;
				bool found = false;

				while (i < pBone->VertexInfluences.size() && found == false) {
					int VertexID = pBone->VertexInfluences.at(i);
					float weight = pBone->VertexWeights.at(i);
					if (weight > minWeight) {
						if (face.Vertices[0] == VertexID || face.Vertices[1] == VertexID || face.Vertices[2] == VertexID) {
							found = true;
						}
						else {
							i++;
						}
					}
					else {
						i++;
					}
				}

				if (found == true) {
					pMesh->Faces.push_back(face);
					pMesh->FaceID.push_back(f);
				}

			}

			if (pBone->pParent == nullptr) {
				pMesh->pParent = nullptr;
			}
			else {
				pMesh->pParent = m_BoneSubmeshes.at(pBone->pParent->ID + 1);
				m_BoneSubmeshes.at(pBone->pParent->ID + 1)->Children.push_back(pMesh);
			}

			m_BoneSubmeshes.push_back(pMesh);

			std::vector<int32_t> verticesInSubmesh;

			for (int i = 0; i < pMesh->Faces.size(); i++) {
				for (int j = 0; j < 3; j++) {
					int32_t VertID = pMesh->Faces.at(i).Vertices[j];
					bool found = false;
					int32_t k = 0;
					while (found == false && k < verticesInSubmesh.size()) {
						if (verticesInSubmesh.at(k) == VertID) {
							found = true;
						}
						else {
							k++;
						}
					}
					if (found == false) {
						verticesInSubmesh.push_back(VertID);
					}
				}
			}

			m_SubmeshVertices.push_back(verticesInSubmesh);


			for (auto child : pBone->Children) {
				initSubmeshes(child, minWeight);
			}

		}


		//Set Tetmesh
		//TV, TF, TT from igl readMesh
		void initTetmesh(Eigen::MatrixXd TV, Eigen::MatrixXi TF, Eigen::MatrixXi TT) {
			this->m_TetMesh.TV = TV;
			this->m_TetMesh.TF = TF;

			for (int i = 0; i < TT.rows(); i++) {
				int Verts[4] = { -1, -1, -1, -1 };
				for (int j = 0; j < TT.cols(); j++) {
					Verts[j] = TT(i, j);
				}
				this->addTet(Verts);
			}

			//Embed Vertices in TetMesh
			for (int i = 0; i < this->m_Positions.size(); i++) {
				addVertices(i);
			}

			interpolate_Tet_Tri();
		}

		//might not need this
		void init_VF(std::string str) {
			igl::read_triangle_mesh(str, V, F);
		}


		//------------------
		uint32_t getMatchingVertex(uint32_t RedundantVertexID, std::vector<std::pair<uint32_t, uint32_t>>* pRedundantVertices) {
			uint32_t Rval = 0;

			for (auto i : (*pRedundantVertices)) {
				if (i.second == RedundantVertexID) {
					Rval = i.first;
					break;
				}
			}
			return Rval;
		}//getMatchingVertex

		void mergeRedundantVertices() {
			float Epsilon = 0.00025f;

			std::vector<std::pair<uint32_t, uint32_t>> RedundantVertices;
			std::vector<bool> IsRedundant;
			std::vector <uint32_t> VertexMapping;
			for (uint32_t i = 0; i < this->vertexCount(); ++i) {
				IsRedundant.push_back(false);
				VertexMapping.push_back(i);
			}

			for (uint32_t i = 0; i < this->vertexCount(); ++i) {
				if (IsRedundant[i]) continue;
				auto v1 = this->vertex(i);
				printf("Checking vertex %d/%d\r", i, this->vertexCount());

				for (uint32_t k = i + 1; k < this->vertexCount(); ++k) {
					auto v2 = this->vertex(k);

					if ((v2 - v1).dot(v2 - v1) < Epsilon) {
						RedundantVertices.push_back(std::pair<uint32_t, uint32_t>(i, k));
						IsRedundant[k] = true;
						break;
					}

				}//for[all remaining vertices]
			}//for[all vertices]

			printf("Found %d double vertices\n", uint32_t(RedundantVertices.size()));

			// rebuild vertices, normals, tangents
			std::vector<Eigen::Vector3f> Vertices;
			std::vector<Eigen::Vector3f> Normals;
			std::vector<Eigen::Vector3f> Tangents;


			for (uint32_t i = 0; i < this->vertexCount(); ++i) {
				VertexMapping[i] = Vertices.size();
				if (!IsRedundant[i]) {
					Vertices.push_back(this->vertex(i));
				}
			}
			for (uint32_t i = 0; i < this->normalCount(); ++i) {
				if (!IsRedundant[i]) Normals.push_back(this->normal(i));
			}
			for (uint32_t i = 0; i < this->tangentCount(); ++i) {
				if (!IsRedundant[i]) Tangents.push_back(this->tangent(i));
			}

			// replace indices in faces
			for (uint32_t i = 0; i < this->submeshCount(); ++i) {
				auto* pM = this->getSubmesh(i);
				for (auto& f : pM->Faces) {
					if (IsRedundant[f.Vertices[0]]) f.Vertices[0] = getMatchingVertex(f.Vertices[0], &RedundantVertices);
					if (IsRedundant[f.Vertices[1]]) f.Vertices[1] = getMatchingVertex(f.Vertices[1], &RedundantVertices);
					if (IsRedundant[f.Vertices[2]]) f.Vertices[2] = getMatchingVertex(f.Vertices[2], &RedundantVertices);

					// and remapp
					f.Vertices[0] = VertexMapping[f.Vertices[0]];
					f.Vertices[1] = VertexMapping[f.Vertices[1]];
					f.Vertices[2] = VertexMapping[f.Vertices[2]];

				}//for[faces of submesh]
			}//for[submeshes]

			// replace vertex weights
			for (uint32_t i = 0; i < this->boneCount(); ++i) {
				auto* pBone = this->getBone(i);

				// collect data
				std::vector<int32_t> Influences;
				std::vector<float> Weights;

				for (uint32_t k = 0; k < pBone->VertexInfluences.size(); ++k) {
					uint32_t ID = pBone->VertexInfluences[k];
					if (IsRedundant[ID]) pBone->VertexInfluences[k] = getMatchingVertex(ID, &RedundantVertices);

					pBone->VertexInfluences[k] = VertexMapping[pBone->VertexInfluences[k]];
				}

			}//for[all bones]

			// replace mesh data
			if (Vertices.size() > 0) this->vertices(&Vertices);
			if (Normals.size() > 0) this->normals(&Normals);
			if (Tangents.size() > 0) this->tangents(&Tangents);

		}//mergeDoubleVertices

		int32_t boneSubmeshCount(void) {
			return m_BoneSubmeshes.size();
		}

		Eigen::Vector3f getUpdatedVertexPosition(int32_t frame, int32_t vertexID) {
			if (vertexID < 0 || frame > Tri_Deformations.size() - 1) throw IndexOutOfBoundsExcept("Index");
			return Eigen::Vector3f(this->Tri_Deformations.at(frame)(vertexID * 3), this->Tri_Deformations.at(frame)(vertexID * 3+1), this->Tri_Deformations.at(frame)(vertexID * 3+2));
		}

		std::vector<int32_t> getVerticesInSubmesh(int32_t index) {
			if (m_SubmeshVertices.size() > index) {
				return m_SubmeshVertices.at(index);
			}
			return m_SubmeshVertices.at(0);
		}


		////// Setter
		void vertices(std::vector<Eigen::Matrix<T, 3, 1>>* pCoords) {
			if (nullptr != pCoords) m_Positions = (*pCoords);
		}//positions

		void normals(std::vector<Eigen::Matrix<T, 3, 1>>* pNormals) {
			if (nullptr != pNormals) m_Normals = (*pNormals);
		}//normals

		void tangents(std::vector<Eigen::Matrix<T, 3, 1>>* pTangents) {
			if (nullptr != pTangents) m_Tangents = (*pTangents);
		}//tangents

		void textureCoordinates(std::vector<Eigen::Matrix<T, 3, 1>>* pUVWs) {
			if (nullptr != pUVWs) m_UVWs = (*pUVWs);
		}//textureCoordinates

		void colors(std::vector<Eigen::Matrix<T, 3, 1>>* pColors) {
			if (nullptr != pColors) m_Colors = (*pColors);
		}//colors

		void bones(std::vector<Bone*>* pBones, bool Copy = true) {
			for (auto i : m_Bones) delete i;
			m_Bones.clear();

			if (nullptr != pBones && Copy) {
				// create bones
				for (size_t i = 0; i < pBones->size(); ++i) m_Bones.push_back(new Bone());

				for (size_t i = 0; i < pBones->size(); ++i) {
					// copy data
					m_Bones[i]->ID = i;
					m_Bones[i]->Position = pBones->at(i)->Position;
					m_Bones[i]->OffsetMatrix = pBones->at(i)->OffsetMatrix;
					m_Bones[i]->VertexInfluences = pBones->at(i)->VertexInfluences;
					m_Bones[i]->VertexWeights = pBones->at(i)->VertexWeights;
					// create links
					m_Bones[i]->pParent = m_Bones[pBones->at(i)->pParent->ID];
					for (size_t k = 0; k < pBones->at(i)->Children.size(); ++k) {
						m_Bones[i]->Children.push_back(m_Bones[pBones->at(i)->Children[k]->ID]);
					}//for[children]
				}//for[bones]

			}
			else if (nullptr != pBones) {
				m_Bones = (*pBones);
			}

			// find root bone
			for (auto i : m_Bones) {
				if (i->pParent == nullptr) m_pRootBone = i;
			}//for[all bones]

		}//bones

		void addSkeletalAnimation(SkeletalAnimation* pAnim, bool Copy = true) {

			if (!Copy) {
				m_SkeletalAnimations.push_back(pAnim);
			}
			else {
				// copy
				SkeletalAnimation* pNewAnim = new SkeletalAnimation();
				pNewAnim->Name = pAnim->Name;
				pNewAnim->Duration = pAnim->Duration;
				for (auto i : pAnim->Keyframes) {
					BoneKeyframes* pBK = new BoneKeyframes();
					(*pBK) = (*i);
					pNewAnim->Keyframes.push_back(pBK);
				}
				m_SkeletalAnimations.push_back(pNewAnim);
			}

		}//addSkeletalAnimation

		void addSubmesh(Submesh* pSubmesh, bool Copy) {
			Submesh* pSM = pSubmesh;
			if (Copy) {
				pSM = new Submesh();
				pSM->init(pSubmesh);
			}
			m_Submeshes.push_back(pSM);
		}//addSubmesh

		void addMaterial(Material* pMat, bool Copy) {
			Material* pM = pMat;
			if (Copy) {
				pM = new Material();
				pM->init(pMat);
			}
			m_Materials.push_back(pM);
		}//addMaterial

		void addMorphTarget(MorphTarget* pMT, bool Copy = true) {
			MorphTarget* pNewMT = pMT;
			if (Copy) {
				pNewMT = new MorphTarget();
				pNewMT->Name = pMT->Name;
				pNewMT->VertexIDs = pMT->VertexIDs;
				pNewMT->VertexOffsets = pMT->VertexOffsets;
				pNewMT->NormalOffsets = pMT->NormalOffsets;
			}
			pNewMT->ID = m_MorphTargets.size();
			m_MorphTargets.push_back(pNewMT);
		}//addMorphTarget

		///
		uint32_t vertexCount(void)const {
			return m_Positions.size();
		}//vertexCount

		uint32_t normalCount(void)const {
			return m_Normals.size();
		}//normalCount

		uint32_t tangentCount(void)const {
			return m_Tangents.size();
		}//tangentCount

		uint32_t textureCoordinatesCount(void)const {
			return m_UVWs.size();
		}

		uint32_t colorCount(void)const {
			return m_Colors.size();
		}//colorCount

		uint32_t submeshCount(void)const {
			return m_Submeshes.size();
		}//submeshCount

		uint32_t materialCount(void)const {
			return m_Materials.size();
		}//materialCount

		uint32_t boneCount(void)const {
			return m_Bones.size();
		}//boneCount

		uint32_t skeletalAnimationCount(void)const {
			return m_SkeletalAnimations.size();
		}//skeletalAnimationCount

		uint32_t morphTargetCount(void)const {
			return m_MorphTargets.size();
		}//morphTargetCount

		////////// Getter
		Eigen::Matrix<T, 3, 1>& vertex(int32_t Index) {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		const Eigen::Matrix<T, 3, 1> vertex(int32_t Index) const {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		Eigen::Matrix<T, 3, 1>& normal(int32_t Index) {
			if (Index < 0 || Index >= normalCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Normals[Index];
		}//normal

		const Eigen::Matrix<T, 3, 1> normal(int32_t Index) const {
			if (Index < 0 || Index >= normalCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Normals[Index];
		}//normal

		Eigen::Matrix<T, 3, 1>& tangent(int32_t Index) {
			if (Index < 0 || Index >= tangentCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Tangents[Index];
		}//tangent

		const Eigen::Matrix<T, 3, 1> tangent(int32_t Index) const {
			if (Index < 0 || Index >= tangentCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Tangents[Index];
		}//tangent

		Eigen::Matrix<T, 3, 1>& textureCoordinate(int32_t Index) {
			if (Index < 0 || Index >= textureCoordinatesCount()) throw IndexOutOfBoundsExcept("Index");
			return m_UVWs[Index];
		}//textureCoordinate

		const Eigen::Matrix<T, 3, 1> textureCoordinate(int32_t Index)const {
			if (Index < 0 || Index >= textureCoordinatesCount()) throw IndexOutOfBoundsExcept("Index");
			return m_UVWs[Index];
		}//textureCoordinate

		Eigen::Matrix<T, 3, 1>& color(int32_t Index) {
			if (Index < 0 || Index >= colorCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Colors[Index];
		}//color

		const Eigen::Matrix<T, 3, 1> color(int32_t Index)const {
			if (Index < 0 || Index >= colorCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Colors[Index];
		}//color

		Submesh* getBoneSubmesh(int32_t Index) {
			if (Index < 0 || Index >= m_BoneSubmeshes.size()) throw IndexOutOfBoundsExcept("Index");
			return m_BoneSubmeshes[Index];
		}//getSubmesh

		Submesh* getSubmesh(int32_t Index) {
			if (Index < 0 || Index >= m_Submeshes.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Submeshes[Index];
		}//getSubmesh

		const Submesh* getSubmesh(int32_t Index)const {
			if (Index < 0 || Index >= m_Submeshes.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Submeshes[Index];
		}//getSubmesh

		Material* getMaterial(int32_t Index) {
			if (Index < 0 || Index >= m_Materials.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Materials[Index];
		}//getMaterial

		const Material* getMaterial(int32_t Index)const {
			if (Index < 0 || Index >= m_Materials.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Materials[Index];
		}//getMaterial

		Bone* getBone(int32_t Index) {
			if (Index < 0 || Index >= m_Bones.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Bones[Index];
		}//getBone

		const Bone* getBone(int32_t Index)const {
			if (Index < 0 || Index >= m_Bones.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Bones[Index];
		}//getBone

		SkeletalAnimation* getSkeletalAnimation(int32_t Index) {
			if (Index < 0 || Index >= m_SkeletalAnimations.size()) throw IndexOutOfBoundsExcept("Index");
			return m_SkeletalAnimations[Index];
		}//getSkeletalAnimation

		const SkeletalAnimation* getSkeletalAnimation(int32_t Index)const {
			if (Index < 0 || Index >= m_SkeletalAnimations.size()) throw IndexOutOfBoundsExcept("Index");
			return m_SkeletalAnimations[Index];
		}//getSkeletalAnimation

		MorphTarget* getMorphTarget(int32_t Index) {
			if (Index < 0 || Index >= m_MorphTargets.size()) throw IndexOutOfBoundsExcept("Index");
			return m_MorphTargets[Index];
		}//getMorphTarget

		const MorphTarget* getMorphTarget(int32_t Index)const {
			if (Index < 0 || Index >= m_MorphTargets.size()) throw IndexOutOfBoundsExcept("Index");
			return m_MorphTargets[Index];
		}//getMorphTarget

		Bone* rootBone(void) {
			return m_pRootBone;
		}//rootBone

		const Bone* rootBone(void)const {
			return m_pRootBone;
		}//rootBone

		void computePerFaceNormals(void) {
			for (auto i : m_Submeshes) {
				i->FaceNormals.clear();
				for (auto k : i->Faces) {
					const Eigen::Vector3f a = m_Positions[k.Vertices[0]] - m_Positions[k.Vertices[2]];
					const Eigen::Vector3f b = m_Positions[k.Vertices[1]] - m_Positions[k.Vertices[2]];
					const Eigen::Vector3f n = a.cross(b);
					i->FaceNormals.push_back(n.normalized());
				}//for[faces]
			}//for[submeshes]
		}//computePerFaceNormals

		void computePerFaceTangents(void) {

			if (m_UVWs.size() == 0) throw CForgeExcept("No UVW coordinates. Can not compute tangents.");

			for (auto i : m_Submeshes) {
				i->FaceTangents.clear();
				for (auto F : i->Faces) {
					const Eigen::Vector3f Edge1 = m_Positions[F.Vertices[1]] - m_Positions[F.Vertices[0]];
					const Eigen::Vector3f Edge2 = m_Positions[F.Vertices[2]] - m_Positions[F.Vertices[0]];
					const Eigen::Vector3f DeltaUV1 = m_UVWs[F.Vertices[1]] - m_UVWs[F.Vertices[0]];
					const Eigen::Vector3f DeltaUV2 = m_UVWs[F.Vertices[2]] - m_UVWs[F.Vertices[0]];

					float f = 1.0f / (DeltaUV1.x() * DeltaUV2.y() - DeltaUV2.x() + DeltaUV1.y());

					Eigen::Vector3f Tangent;
					Tangent.x() = f * (DeltaUV2.y() * Edge1.x() - DeltaUV1.y() * Edge2.x());
					Tangent.y() = f * (DeltaUV2.y() * Edge1.y() - DeltaUV1.y() * Edge2.y());
					Tangent.z() = f * (DeltaUV2.y() * Edge1.z() - DeltaUV1.y() * Edge2.z());
					i->FaceTangents.push_back(Tangent);
				}//for[all faces]
			}//for[all submeshes]

		}//computeTangents

		void computePerVertexNormals(bool ComputePerFaceNormals = true) {
			if (ComputePerFaceNormals) computePerFaceNormals();

			m_Normals.clear();
			// create normals
			for (uint32_t i = 0; i < m_Positions.size(); ++i) m_Normals.push_back(Eigen::Vector3f::Zero());

			// sum normals
			for (auto i : m_Submeshes) {
				for (uint32_t k = 0; k < i->Faces.size(); ++k) {
					Face* pF = &(i->Faces[k]);
					m_Normals[pF->Vertices[0]] = m_Normals[pF->Vertices[0]] + i->FaceNormals[k];
					m_Normals[pF->Vertices[1]] = m_Normals[pF->Vertices[1]] + i->FaceNormals[k];
					m_Normals[pF->Vertices[2]] = m_Normals[pF->Vertices[2]] + i->FaceNormals[k];
				}//for[faces]
			}//for[sub meshes

			// normalize normals
			for (auto& i : m_Normals) i.normalize();

		}//computeperVertexNormals

		void computePerVertexTangents(bool ComputePerFaceTangents = true) {
			if (ComputePerFaceTangents) computePerFaceTangents();

			m_Tangents.clear();

			// create tangents
			for (uint32_t i = 0; i < m_Positions.size(); ++i) m_Tangents.push_back(Eigen::Vector3f::Zero());

			// sum tangents
			for (auto i : m_Submeshes) {
				for (uint32_t k = 0; k < i->Faces.size(); ++k) {
					Face* pF = &(i->Faces[k]);
					m_Tangents[pF->Vertices[0]] += i->FaceTangents[k];
					m_Tangents[pF->Vertices[1]] += i->FaceTangents[k];
					m_Tangents[pF->Vertices[2]] += i->FaceTangents[k];
				}//for[all faces]
			}//for[submeshes]

			// normalize tangents
			for (auto& i : m_Tangents) i.normalize();

		}//computePerVertexTangents

		AABB aabb(void)const {
			return m_AABB;
		}//aabb

		void computeAxisAlignedBoundingBox(void) {
			if (m_Positions.size() == 0) throw CForgeExcept("Mesh contains no vertex data. Can not compute axis aligned bounding box");
			m_AABB.Min = m_Positions[0];
			m_AABB.Max = m_Positions[0];
			for (auto i : m_Positions) {
				if (i.x() < m_AABB.Min.x()) m_AABB.Min.x() = i.x();
				if (i.y() < m_AABB.Min.y()) m_AABB.Min.y() = i.y();
				if (i.z() < m_AABB.Min.z()) m_AABB.Min.z() = i.z();
				if (i.x() > m_AABB.Max.x()) m_AABB.Max.x() = i.x();
				if (i.y() > m_AABB.Max.y()) m_AABB.Max.y() = i.y();
				if (i.z() > m_AABB.Max.z()) m_AABB.Max.z() = i.z();
			}//for[all position values]
		}//computeAxisAlignedBoundingBox

		std::vector<Eigen::VectorXf> Tri_Deformations;
		std::vector<Eigen::VectorXf> Tri_Normal_Deformations;

	protected:
		std::vector<Eigen::Matrix<T, 3, 1>> m_Positions; ///< Vertex positions
		std::vector<Eigen::Matrix<T, 3, 1>> m_Normals; ///< per vertex normals
		std::vector<Eigen::Matrix<T, 3, 1>> m_Tangents; ///< per vertex tangents
		std::vector<Eigen::Matrix<T, 3, 1>> m_UVWs; ///< texture coordinates
		std::vector<Eigen::Matrix<T, 3, 1>> m_Colors; ///< vertex colors
		std::vector<Submesh*> m_Submeshes; ///< Submeshes
		std::vector<Submesh*> m_BoneSubmeshes; ///< Submeshes
		std::vector<Material*> m_Materials; ///< Materials
		Submesh* m_pRoot; ///< Root of the mesh's graph
		AABB m_AABB;

		//------------
		TetMesh m_TetMesh;
		std::vector<Eigen::VectorXf> Tet_Deformations;
		Eigen::MatrixXd V;
		Eigen::MatrixXi F;
		//------------

		// skeletal animation related
		Bone* m_pRootBone;
		std::vector<Bone*> m_Bones;
		std::vector<SkeletalAnimation*> m_SkeletalAnimations;
		std::vector<RotationAxis*> aRotationAxis;
		std::vector<ChangedKeyframes*> m_ChangedFrames;
		ChangedKeyframes* lastOriginal = nullptr;
		std::vector<std::vector<bool>> frameWasChanged;
		std::vector<std::vector<int32_t>> m_SubmeshVertices;

		float DeltaTheta = 0.000025f;
		float Epsilon = 0.20f;
		float LearningRate = 0.000035f;
		float initialGuess = 0.9f;
		bool enableInitialGuess = true;

		// morph target related
		std::vector<MorphTarget*> m_MorphTargets;

		//----- Functions

		std::vector<Eigen::Matrix4f> getVertexTransMatrixFromPose(Pose pPose) {
			std::vector<Eigen::Matrix4f> T;
			for (int i = 0; i < m_Positions.size(); i++) {
				T.push_back(Eigen::Matrix4f::Zero());
			}

			for (int j = 0; j < pPose.BoneIDs.size(); j++) {

				Bone* pBone = this->m_Bones.at(pPose.BoneIDs.at(j));
				//add SkinningMatrix*Weight to each VertexMatrix
				for (int i = 0; i < pBone->VertexInfluences.size(); i++) {
					T.at(pBone->VertexInfluences.at(i)) += pBone->VertexWeights.at(i) * pPose.SkinningMatrix.at(j);
				}
			}

			for (int i = 0; i < m_Positions.size(); i++) {

				for (int r = 0; r < 4; r++) {
					for (int c = 0; c < 4; c++) {
						T.at(i)(r, c) = T.at(i)(r, c) / T.at(i)(3, 3);
					}
				}

			}

			return T;

		}

		std::vector<Eigen::Matrix4f> getVertexTransMatrix(int frame, int AnimationID) {
			std::vector<Eigen::Matrix4f> T;
			for (int i = 0; i < m_Positions.size(); i++) {
				T.push_back(Eigen::Matrix4f::Zero());
			}

			for (auto boneKeyframe : this->m_SkeletalAnimations.at(AnimationID)->Keyframes) {

				Bone* pBone = this->m_Bones.at(boneKeyframe->BoneID);
				//add SkinningMatrix*Weight to each VertexMatrix
				for (int i = 0; i < pBone->VertexInfluences.size(); i++) {
					//Eigen::Matrix4f InverseSkinning = boneKeyframe->SkinningMatrix.at(0).inverse();
					T.at(pBone->VertexInfluences.at(i)) += pBone->VertexWeights.at(i) * boneKeyframe->SkinningMatrix.at(frame);
				}
			}

			for (int i = 0; i < m_Positions.size(); i++) {

				for (int r = 0; r < 4; r++) {
					for (int c = 0; c < 4; c++) {
						T.at(i)(r, c) = T.at(i)(r, c) / T.at(i)(3, 3);
					}
				}

			}

			return T;

		}

		Eigen::VectorXf getDeformationVector(std::vector<Eigen::Matrix4f> T) {
			//go through all Vertices
			Eigen::VectorXf pTri_Def(m_Positions.size() * 3);

			for (int i = 0; i < m_Positions.size(); i++) {

				Eigen::Vector3f Vertex = this->m_Positions.at(i);
				Eigen::Vector4f Position = Eigen::Vector4f(Vertex.x(), Vertex.y(), Vertex.z(), 1);

				Matrix4f TransformationMatrix = T.at(i);
				Eigen::Vector4f defPosition = TransformationMatrix * Position;

				pTri_Def(i * 3 + 0) = defPosition.x() / defPosition.w();
				pTri_Def(i * 3 + 1) = defPosition.y() / defPosition.w();
				pTri_Def(i * 3 + 2) = defPosition.z() / defPosition.w();
			}

			return pTri_Def;
		}

		Eigen::VectorXf getDeformationNormals(std::vector<Eigen::Matrix4f> T) {
			Eigen::VectorXf pNor_Def(m_Normals.size() * 3);

			//go through all Vertices
			for (int i = 0; i < m_Positions.size(); i++) {

				Eigen::Vector3f Normal3 = this->m_Normals.at(i);
				Eigen::Vector4f Normal4 = Eigen::Vector4f(Normal3.x(), Normal3.y(), Normal3.z(), 1);

				Matrix4f TransformationMatrix = T.at(i);
				//TransformationMatrix.transposeInPlace();
				//TransformationMatrix = TransformationMatrix.inverse();
				Eigen::Vector4f defNormal = TransformationMatrix * Normal4;
				Normal3.x() = defNormal.x();
				Normal3.y() = defNormal.y();
				Normal3.z() = defNormal.z();

				Normal3.normalize();

				pNor_Def(i * 3 + 0) = Normal3.x();
				pNor_Def(i * 3 + 1) = Normal3.y();
				pNor_Def(i * 3 + 2) = Normal3.z();
			}

			return pNor_Def;
		}



	};//T3DMesh

}//name space


#endif