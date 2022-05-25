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


namespace CForge {

	/**
	* \brief Template class that stores a triangle mesh.
	* 
	* \todo Do full documentation
	*/

	template<typename T>
	class T3DMesh: public CForgeObject {
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

		struct Submesh {
			std::vector<Face> Faces;
			int32_t Material;
			std::vector<Eigen::Vector3f> FaceNormals; ///< Stores face normals (if required)
			std::vector<Eigen::Vector3f> FaceTangents; ///< Stores face tangents (if required)
			Eigen::Quaternion<float> RotationOffset; // rotation relative to parent
			Eigen::Matrix<T, 3, 1> TranslationOffset; // translation relative to parent
			std::vector<Submesh*> Children;
			Submesh* pParent;

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
		};//Submesh

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

		struct AABB {
			Eigen::Vector3f Min;
			Eigen::Vector3f Max;

			Eigen::Vector3f diagonal(void) {
				return (Max - Min);
			}
		};//AxisAlignedBoundingBox

		struct Bone {
			int32_t ID;
			std::string Name;
			Eigen::Vector3f			Position;
			Eigen::Matrix4f			OffsetMatrix;
			std::vector<int32_t>	VertexInfluences;
			std::vector<float>		VertexWeights;
			Bone* pParent;
			std::vector<Bone*>		Children;

		};

		struct BoneKeyframes {
			int32_t ID;
			int32_t BoneID;
			std::string BoneName;
			std::vector<Eigen::Vector3f> Positions;
			std::vector<Eigen::Quaternionf> Rotations;
			std::vector<Eigen::Vector3f> Scalings;
			std::vector<float> Timestamps;
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

		T3DMesh(void): CForgeObject("TDMesh") {
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

		}//clear

		void clearSkeleton(void) {
			for (auto i : m_Bones) {
				if (nullptr != i) delete i;
			}
			m_Bones.clear();
		}//clearSkeleton

		void clearSkeletalAnimations(void) {
			for (auto i : m_SkeletalAnimations) {
				if (nullptr != i) delete i;
			}
			m_SkeletalAnimations.clear();
		}//clearSkeletalAnimations

		////// Setter
		void vertices(std::vector<Eigen::Matrix<T, 3, 1>> *pCoords) {
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
			else if(nullptr != pBones) {
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

		const Eigen::Matrix<T, 3, 1> normal(int32_t Index) const{
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

		Submesh* getSubmesh(int32_t Index){
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
			if(ComputePerFaceNormals) computePerFaceNormals();

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

	protected:
		std::vector<Eigen::Matrix<T, 3, 1>> m_Positions; ///< Vertex positions
		std::vector<Eigen::Matrix<T, 3, 1>> m_Normals; ///< per vertex normals
		std::vector<Eigen::Matrix<T, 3, 1>> m_Tangents; ///< per vertex tangents
		std::vector<Eigen::Matrix<T, 3, 1>> m_UVWs; ///< texture coordinates
		std::vector<Eigen::Matrix<T, 3, 1>> m_Colors; ///< vertex colors
		std::vector<Submesh*> m_Submeshes; ///< Submeshes
		std::vector<Material*> m_Materials; ///< Materials
		Submesh* m_pRoot; ///< Root of the mesh's graph
		AABB m_AABB;

		// skeletal animation related
		Bone* m_pRootBone;
		std::vector<Bone*> m_Bones;
		std::vector<SkeletalAnimation*> m_SkeletalAnimations;

		// morph target related
		std::vector<MorphTarget*> m_MorphTargets;

	};//T3DMesh

}//name space


#endif