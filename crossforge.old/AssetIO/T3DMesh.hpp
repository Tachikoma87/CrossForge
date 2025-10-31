/*****************************************************************************\
*                                                                           *
* File(s): T3DMesh.hpp                                                      *
*                                                                           *
* Content: Template class that stores a triangle mesh.                      *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_T3DMESH_H__
#define __CFORGE_T3DMESH_H__

#include "../Core/CForgeObject.h"
#include "../Math/Box.hpp"

namespace CForge {

	/**
	* \brief Template class that stores a triangle mesh.
	* 
	* \todo Change internal data handling to container.
	* \todo Change pass by pointer to pass by reference, where applicable.
	* \todo Implement copy operator for class and all nested structures.
	* \todo Write concept tutorial page how a 3D mesh is represented.
	* \todo Change internal data management to smart pointer.
	* \todo Overload assign and init operator.
	* \todo Rename bone to join.
	* \todo Add morph target animation data structure.
	*/
	template<typename T>
	class T3DMesh: public CForgeObject {
	public:
		/**
		* \brief Face data structure. Only supports triangles.
		* 
		* It is assumed that the internal data such as normals, vertexes, UVw-coordinates are all of the same size, i.e. one entry for every vertex.
		*/
		struct Face {
			int32_t Vertices[3];	///< The 3 indexes that form the face.

			/**
			* \brief Constructor
			*/
			Face(void) {
				clear();
			}

			/**
			* \brief Destructor.
			*/
			~Face() {
				clear();
			}

			/**
			* \brief Clear method.
			*/
			void clear() {
				Vertices[0] = -1;
				Vertices[1] = -1;
				Vertices[2] = -1;
			}
		};//Face

		/**
		* \brief A submesh is a set collection of faces. It is usually characterized by all faces sharing the same material.
		*/
		struct Submesh {
			std::vector<Face> Faces;					///< Set of faces belonging to this submesh.
			std::vector<Eigen::Vector3f> FaceNormals;	///< Stores face normals (if required)
			std::vector<Eigen::Vector3f> FaceTangents;	///< Stores face tangents (if required)
			int32_t Material;

			/**
			* \brief Constructor
			*/
			Submesh(void) {
				clear();
			}//Submesh

			void init(Submesh* pRef) {
				clear();
				Faces = pRef->Faces;
				Material = pRef->Material;
				FaceNormals = pRef->FaceNormals;
				FaceTangents = pRef->FaceTangents;
			}//initialize

			void clear() {
				Faces.clear();
				FaceNormals.clear();
				FaceTangents.clear();
				Material = -1;
			}
		};//Submesh

		/**
		* \brief The material defines the appearance of a mesh. It consists of PBS parameters, a set of textures, and a set of shaders used to render the mesh later on. 
		*/
		struct Material {
			int32_t ID;				///< Internal ID.
			Eigen::Vector4f Color;	///< RGBA color vector.
			float Metallic;			///< Metallic value.
			float Roughness;		///< Roughness value.
			std::string TexAlbedo;	///< Albedo texture.
			std::string TexNormal;	///< Normal texture.
			std::string TexDepth;	///< Depth texture.

			std::string TexMetallicRoughness;	///< Metallic-roughness texture.
			std::string TexEmissive;			///< Emissive texture.
			std::string TexOcclusion;			///< Occlusion texture.

			std::vector<std::string> VertexShaderGeometryPass;	///< Vertex shaders for the geometry pass.
			std::vector<std::string> FragmentShaderGeometryPass;///< Fragment shaders for the geometry pass.

			std::vector<std::string> VertexShaderShadowPass;	///< Vertex shaders for the shadow pass.
			std::vector<std::string> FragmentShaderShadowPass;	///< Fragment shaders for the shadow pass.

			std::vector<std::string> VertexShaderForwardPass;	///< Vertex shaders for the forward pass.
			std::vector<std::string> FragmentShaderForwardPass;	///< Fragment shaders for the forward pass.

			/**
			* \brief Constructor.
			*/
			Material(void) {
				clear();
			}//Constructor

			/**
			* \brief Destructor.
			*/
			~Material(void) {
				clear();
			}//Destructor

			/**
			* \brief Initialize method.
			* 
			* \param[in] Mat Reference material.
			*/
			void init(const Material &Mat) {
				clear();
				
				ID = Mat.ID;
				Color = Mat.Color;
				Metallic = Mat.Metallic;
				Roughness = Mat.Roughness;
				TexAlbedo = Mat.TexAlbedo;
				TexNormal = Mat.TexNormal;
				TexDepth = Mat.TexDepth;
				TexMetallicRoughness = Mat.TexMetallicRoughness;
				TexEmissive = Mat.TexEmissive;
				TexOcclusion = Mat.TexOcclusion;

					
				VertexShaderGeometryPass = Mat.VertexShaderGeometryPass;
				FragmentShaderGeometryPass = Mat.FragmentShaderGeometryPass;
				VertexShaderShadowPass = Mat.VertexShaderShadowPass;
				FragmentShaderShadowPass = Mat.FragmentShaderShadowPass;
				VertexShaderForwardPass = Mat.VertexShaderForwardPass;
				FragmentShaderForwardPass = Mat.FragmentShaderForwardPass;
				
			}//initialize

			void clear(void) {
				ID = -1;
				Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				Metallic = 0.0f;
				Roughness = 0.0f;
				TexAlbedo = "";
				TexNormal = "";
				TexDepth = "";
				TexMetallicRoughness = "";
				TexEmissive = "";
				TexOcclusion = "";
				VertexShaderGeometryPass.clear();
				FragmentShaderGeometryPass.clear();
				VertexShaderShadowPass.clear();
				FragmentShaderShadowPass.clear();
				VertexShaderForwardPass.clear();
				FragmentShaderForwardPass.clear();
			}//clear

		};//Material


		/**
		* \brief Data structure that holds a bone (aka joint).
		*/
		struct Bone {
			int32_t ID;				///< Internal ID.
			std::string Name;		///< Name of the joint.
			Eigen::Vector3f			Position;			///< Position. 
			Eigen::Matrix4f			InvBindPoseMatrix;	///< Inverse bind pose matrix.
			std::vector<int32_t>	VertexInfluences;	///< Vertexes of the mesh that are influenced by this joint.
			std::vector<float>		VertexWeights;		///< Influence strength.
			Bone* pParent;								///< Parent bone.
			std::vector<Bone*>		Children;			///< Children of this joint.

			/**
			* \brief Constructor
			*/
			Bone(void) {
				clear();
			}

			/**
			* \brief Destructor
			*/
			~Bone(void) {
				clear();
			}

			/**
			* \brief Initialization method.
			* 
			* \param[in] pRef Reference bone.
			* \param[in] pAllBones All bone definitions. Use this if pointers have changed.
			*/
			void init(const Bone* pRef, std::vector<Bone*> *pAllBones) {
				clear();
				ID = pRef->ID;
				Name = pRef->Name;
				Position = pRef->Position;
				InvBindPoseMatrix = pRef->InvBindPoseMatrix;
				VertexInfluences = pRef->VertexInfluences;
				VertexWeights = pRef->VertexWeights;
				pParent = (pRef->pParent == nullptr) ? nullptr : pAllBones->at(pRef->pParent->ID);
				for (auto i : pRef->Children) {
					Children.push_back(pAllBones->at(i->ID));
				}
			}//initialize

			/**
			* \brief Clear method.
			*/
			void clear(void) {
				ID = -1;
				Name = "";
				Position = Eigen::Vector3f::Zero();
				InvBindPoseMatrix = Eigen::Matrix4f::Identity();
				VertexInfluences.clear();
				VertexWeights.clear();
				pParent = nullptr;
				Children.clear();
			}//clear
		};

		/**
		* \brief Keyframes of a single bone for skeletal animations.
		* 
		* \note Vector size of Positions, Rotations, Scalings, and Timestamps have to be equal.
		*/
		struct BoneKeyframes {
			int32_t ID;				///< Internal ID.
			int32_t BoneID;			///< ID of the bone the keyframes belong to.
			std::string BoneName;	///< Name of the bone the keyframes belong to.
			std::vector<Eigen::Vector3f> Positions;		///< Bone positions.
			std::vector<Eigen::Quaternionf> Rotations;	///< Bone rotations.
			std::vector<Eigen::Vector3f> Scalings;		///< Bone scalings.
			std::vector<float> Timestamps;				///< TImestamps.

			/**
			* \brief Constructor.
			*/
			BoneKeyframes(void) {
				clear();
			}

			/**
			* \brief Destructor.
			*/
			~BoneKeyframes(void) {
				clear();
			}

			/**
			* \brief Initialization method.
			* 
			* \param[in] pRef Reference keyframe.
			*/
			void init(const BoneKeyframes* pRef) {
				clear();
				ID = pRef->ID;
				BoneID = pRef->BoneID;
				BoneName = pRef->BoneName;
				Positions = pRef->Positions;
				Rotations = pRef->Rotations;
				Scalings = pRef->Scalings;
				Timestamps = pRef->Timestamps;
			}//initialize

			/**
			*  \brief Clear method.
			*/
			void clear(void) {
				ID = -1;
				BoneID = -1;
				BoneName = "";
				Positions.clear();
				Rotations.clear();
				Scalings.clear();
				Timestamps.clear();
			}
		};


		/**
		* \brief Data structure that holds a skeletal animation.
		*/
		struct SkeletalAnimation {
			std::string Name;		///< Name of the animation.
			float Duration;			///< Total duration in seconds.
			float SamplesPerSecond; ///< Number of keyframes per second.
			std::vector<BoneKeyframes*> Keyframes;	///< The keyframes. One for every bone.

			/**
			* \brief Constructor.
			*/
			SkeletalAnimation(void) {
				clear();
			}

			/**
			* \brief Destructor.
			*/
			~SkeletalAnimation(void) {
				clear();
			}

			/**
			* \brief Initialization method.
			* 
			* \param[in] pRef Reference skeletal animation.
			*/
			void init(const SkeletalAnimation* pRef) {
				if (pRef == this) return;
				clear();
				Name = pRef->Name;
				Duration = pRef->Duration;
				SamplesPerSecond = pRef->SamplesPerSecond;
				for (auto i : pRef->Keyframes) {
					BoneKeyframes* pKey = new BoneKeyframes();
					pKey->init(i);
					Keyframes.push_back(pKey);
				}
			}

			/**
			* \brief Clear method.
			*/
			void clear(void) {
				Name = "";
				Duration = 0.0f;
				SamplesPerSecond = 0.0f;
				for (auto& i : Keyframes) 
				{
					if(nullptr != i) delete i;
					i = nullptr;
				}
				Keyframes.clear();
			}//clear
		};

		/**
		* \brief Data structure that holds a morph target.
		* 
		* \note Vector sizes of VertexIDs, VertexOffsets, and NormalOffsets have to be equal.
		*/
		struct MorphTarget {
			int32_t ID;						///< Internal ID.
			std::string Name;				///< Name of the morph target.
			std::vector<int32_t> VertexIDs;	///< Affected vertexes
			std::vector<Eigen::Vector3f> VertexOffsets; ///< Position offset vectors.
			std::vector<Eigen::Vector3f> NormalOffsets; ///< Normal Offset vectors.

			/**
			* \brief Constructor
			*/
			MorphTarget() {
				clear();
			}

			/**
			* \brief Destructor
			*/
			~MorphTarget() {
				clear();
			}

			/**
			* \brief Clear method.
			*/
			void clear() {
				ID = -1;
				Name = "";
				VertexIDs.clear();
				VertexOffsets.clear();
				NormalOffsets.clear();
			}
			
		};


		/**
		* \brief Computes the axis aligned bounding box of a given mesh.
		* 
		* \param[in] Mesh The mesh the axis aligned bounding box will be computed for.
		* \return Axis aligned bounding box of the mesh.
		* \throws CrossForgeException if mesh contains no vertexes.
		*/
		static Box computeAxisAlignedBoundingBox(const T3DMesh<T> &Mesh) {
			if (Mesh.vertexCount() == 0) throw CForgeExcept("Mesh contains no vertex data. Can not compute axis aligned bounding box");
			Eigen::Vector3f Min = Mesh.m_Positions[0];
			Eigen::Vector3f Max = Mesh.m_Positions[0];
			for (auto i : Mesh.m_Positions) {
				if (i.x() < Min.x()) Min.x() = i.x();
				if (i.y() < Min.y()) Min.y() = i.y();
				if (i.z() < Min.z()) Min.z() = i.z();
				if (i.x() > Max.x()) Max.x() = i.x();
				if (i.y() > Max.y()) Max.y() = i.y();
				if (i.z() > Max.z()) Max.z() = i.z();
			}//for[all position values]

			Box Rval;
			Rval.init(Min, Max);
			return Rval;
		}//computeAxisAlignedBoundingBox

		/**
		* \brief Constructor.
		*/
		T3DMesh(void): CForgeObject("T3DMesh") {
			m_pRootBone = nullptr;
		}//Constructor

		/**
		* \brief Destructor.
		*/
		~T3DMesh(void) {
			clear();
		}//Destructor

		/**
		* \brief Copy initialization constructor.
		* 
		* \param[in] Ref 
		*/
		T3DMesh(const T3DMesh<T>& Other): CForgeObject("T3DMesh") {
			if (this == &Other) return;
			m_pRootBone = nullptr;
			this->init(&Other);
		}

		/**
		* \brief Copy assignment operator.
		* 
		* \param[in] Other The other object.
		*/
		T3DMesh<T>& operator=(const T3DMesh<T>& Other) {
			if (this == &Other) return (*this);
			this->clear();
			this->init(&Other);
			return (*this);
		}

		/**
		* \brief Initialization method. 
		* 
		* \param[in] pRef The other mesh.
		*/
		void init(const T3DMesh* pRef = nullptr) {
			if (nullptr != pRef && pRef != this) {
				clear();
				m_Positions = pRef->m_Positions;
				m_Normals = pRef->m_Normals;
				m_Tangents = pRef->m_Tangents;
				m_UVWs = pRef->m_UVWs;
				m_Colors = pRef->m_Colors;

				// copy submeshes
				for (auto i : pRef->m_Submeshes) {
					Submesh* pS = new Submesh;
					pS->init(i);
					m_Submeshes.push_back(pS);
				}//for[submeshes]
				for (auto i : pRef->m_Materials) {
					Material* pMat = new Material;
					pMat->init((*i));
					m_Materials.push_back(pMat);
				}//for[materials]
				m_AABB = pRef->m_AABB;

				// create new bones
				for (auto i : pRef->m_Bones) {
					Bone* pNewBone = new Bone();
					m_Bones.push_back(pNewBone);
				}
				// initalize bones
				for (auto i : pRef->m_Bones) {
					m_Bones[i->ID]->init(i, &m_Bones);
				}

				// copy skeletal animations
				for (auto i : pRef->m_SkeletalAnimations) {
					SkeletalAnimation* pAnim = new SkeletalAnimation();
					pAnim->init(i);
					m_SkeletalAnimations.push_back(pAnim);
				}

				//// morph target related
				//std::vector<MorphTarget*> m_MorphTargets;

			}
		}//initialize

		/**
		* \brief Clear method.
		*/
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
			m_pRootBone = nullptr;

			for (auto& i : m_Bones) delete i;
			m_Bones.clear();
			for (auto& i : m_SkeletalAnimations) delete i;
			m_SkeletalAnimations.clear();
			
			for (auto& i : m_MorphTargets) delete i;
			m_MorphTargets.clear();

		}//clear

		/**
		* \brief Clear only the skeleton data.
		*/
		void clearSkeleton(void) {
			for (auto i : m_Bones) {
				if (nullptr != i) delete i;
			}
			m_Bones.clear();
		}//clearSkeleton

		/**
		* \brief Clears only the skeletal animations.
		*/
		void clearSkeletalAnimations(void) {
			for (auto i : m_SkeletalAnimations) {
				if (nullptr != i) delete i;
			}
			m_SkeletalAnimations.clear();
		}//clearSkeletalAnimations

		////// Setter

		/**
		* \brief Setter for the vertexes.
		* 
		* \param[in] pCoords Vertex coordinates.
		*/
		void vertices(std::vector<Eigen::Matrix<T, 3, 1>> *pCoords) {
			if (nullptr != pCoords) m_Positions = (*pCoords);
		}//positions

		/**
		* \brief setter for the normals.
		* 
		* \param[in] pNormals New normals.
		*/
		void normals(std::vector<Eigen::Matrix<T, 3, 1>>* pNormals) {
			if (nullptr != pNormals) m_Normals = (*pNormals);
		}//normals

		/**
		* \brief Setter for the tangents.
		* 
		* \param[in] pTangents The new tangents.
		*/
		void tangents(std::vector<Eigen::Matrix<T, 3, 1>>* pTangents) {
			if (nullptr != pTangents) m_Tangents = (*pTangents);
		}//tangents

		/**
		* \brief Setter for the texture coordinates.
		* 
		* \param[in] pUVWs The new texture coordinates.
		*/
		void textureCoordinates(std::vector<Eigen::Matrix<T, 3, 1>>* pUVWs) {
			if (nullptr != pUVWs) m_UVWs = (*pUVWs);
		}//textureCoordinates

		/**
		* \brief Setter for the vertex colors.
		* 
		* \param[in] pColors The new vertex colors.
		*/
		void colors(std::vector<Eigen::Matrix<T, 3, 1>>* pColors) {
			if (nullptr != pColors) m_Colors = (*pColors);
		}//colors

		/**
		* \brief Setter for the bones.
		* 
		* \param[in] pBones Vector of bones.
		* \param[in] Copy Whether the data should by copied or possession goes to the T3DMesh structure.
		*/
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
					m_Bones[i]->InvBindPoseMatrix = pBones->at(i)->InvBindPoseMatrix;
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

		/**
		* \brief Adds a skeletal animation.
		* 
		* \param[in] pAnim Skeletal animation to add.
		* \param[in] Copy Whether data should be copied or possession goes to the T3DMesh structure.
		*/
		void addSkeletalAnimation(SkeletalAnimation* pAnim, bool Copy = true) {

			if (!Copy) {
				m_SkeletalAnimations.push_back(pAnim);
			}
			else {
				// copy
				SkeletalAnimation* pNewAnim = new SkeletalAnimation();
				pNewAnim->Name = pAnim->Name;
				pNewAnim->Duration = pAnim->Duration;
				pNewAnim->SamplesPerSecond = pAnim->SamplesPerSecond;
				for (auto i : pAnim->Keyframes) {
					BoneKeyframes* pBK = new BoneKeyframes();
					(*pBK) = (*i);
					pNewAnim->Keyframes.push_back(pBK);
				}
				m_SkeletalAnimations.push_back(pNewAnim);
			}

		}//addSkeletalAnimation

		/**
		* \brief Add a submesh.
		* 
		* \param[in] pSubmesh Submesh to add.
		* \param[in] Copy Whether data should be copied or possession is transfered to the T3DMesh structure.
		*/
		void addSubmesh(Submesh* pSubmesh, bool Copy) {
			Submesh* pSM = pSubmesh;
			if (Copy) {
				pSM = new Submesh();
				pSM->init(pSubmesh);
			}
			m_Submeshes.push_back(pSM);			
		}//addSubmesh

		/**
		* \brief Add a material.
		* 
		* \param[in] pMat Material to add.
		* \param[in] Copy Whether data should be copied or the T3DMesh structure takes possession.
		*/
		void addMaterial(Material* pMat, bool Copy) {
			Material* pM = pMat;
			if (Copy) {
				pM = new Material();
				pM->init( (*pMat));
			}
			//pM->ID = m_Materials.size();
			m_Materials.push_back(pM);
		}//addMaterial

		/**
		* \brief Add a morph target.
		* 
		* \param[in] pMT Morph target to add.
		* \param[in] Copy Whether data should be copied or T3DMesh structure takes possession.
		*/
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

		/**
		* \brief Returns number of vertices.
		* 
		* \return Number of vertices.
		*/
		uint32_t vertexCount(void)const {
			return m_Positions.size();
		}//vertexCount

		/**
		* \brief Returns number of normals.
		* 
		* \return Number of normals.
		*/
		uint32_t normalCount(void)const {
			return m_Normals.size();
		}//normalCount

		/**
		* \brief Returns number of tangents.
		* 
		* \return Number of tangents.
		*/
		uint32_t tangentCount(void)const {
			return m_Tangents.size();
		}//tangentCount

		/**
		* \brief Returns number of texture coordinates.
		* 
		* \return Number of texture coordinates.
		*/
		uint32_t textureCoordinatesCount(void)const {
			return m_UVWs.size();
		}

		/**
		* \brief Returns number of vertex colors.
		* 
		* \return Number of vertex colors.
		*/
		uint32_t colorCount(void)const {
			return m_Colors.size();
		}//colorCount

		/**
		* \brief Returns number of submeshes.
		* 
		* \return Number of submeshes.
		*/
		uint32_t submeshCount(void)const {
			return m_Submeshes.size();
		}//submeshCount

		/**
		* \brief Returns number of materials.
		* 
		* \return Number of materials.
		*/
		uint32_t materialCount(void)const {
			return m_Materials.size();
		}//materialCount

		/**
		 * \brief Returns number of materials.
		 * 
		 * \return Number of materials.
		 */
		uint32_t boneCount(void)const {
			return m_Bones.size();
		}//boneCount

		/**
		* \brief Returns number of skeletal animations.
		* 
		* \return Number of skeletal animations.
		*/
		uint32_t skeletalAnimationCount(void)const {
			return m_SkeletalAnimations.size();
		}//skeletalAnimationCount

		/**
		* \brief Returns number of morph targets.
		* 
		* \return Number of morph targets.
		*/
		uint32_t morphTargetCount(void)const {
			return m_MorphTargets.size();
		}//morphTargetCount

		////////// Getter

		/**
		* \brief Vertex access operator.
		* 
		* \param[in] Index The index of the vertex.
		* \return Read/write access of the requested vertex.
		*/
		Eigen::Matrix<T, 3, 1>& vertex(int32_t Index) {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		/**
		* \brief Vertex access operator, const version.
		* 
		* \param[in] Index The index of the vertex.
		* \return Vertex position of the requested vertex.
		*/
		const Eigen::Matrix<T, 3, 1> vertex(int32_t Index) const {
			if (Index < 0 || Index >= vertexCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Positions[Index];
		}//vertex

		/**
		* \brief Normal access operator.
		* 
		* \param[in] Index The index of the normal.
		* \return Read/write access of the requested normal.
		*/
		Eigen::Matrix<T, 3, 1>& normal(int32_t Index) {
			if (Index < 0 || Index >= normalCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Normals[Index];
		}//normal

		/**
		* \brief Normal access operator, const version.
		* 
		* \param[in] Index The index of the normal.
		* \return Vector of the requested normal.
		*/
		const Eigen::Matrix<T, 3, 1> normal(int32_t Index) const{
			if (Index < 0 || Index >= normalCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Normals[Index];
		}//normal

		/**
		* \brief Tangent access operator.
		* 
		* \param[in] Index The index of the tangent.
		* \return Read/write access of the requested tangent.
		*/
		Eigen::Matrix<T, 3, 1>& tangent(int32_t Index) {
			if (Index < 0 || Index >= tangentCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Tangents[Index];
		}//tangent

		/**
		* \brief Tangent access operator, const version.
		* 
		* \param[in] Index The index of the tangent.
		* \return Vector of the requested tangent.
		*/
		const Eigen::Matrix<T, 3, 1> tangent(int32_t Index) const {
			if (Index < 0 || Index >= tangentCount()) {
				throw IndexOutOfBoundsExcept("Index");
			}
			return m_Tangents[Index];
		}//tangent

		/**
		* \brief Texture coordinate access operator.
		* 
		* \param[in] Index The index of the texture coordinate.
		* \return Read/write access of the requested texture coordinate.
		*/
		Eigen::Matrix<T, 3, 1>& textureCoordinate(int32_t Index) {
			if (Index < 0 || Index >= textureCoordinatesCount()) throw IndexOutOfBoundsExcept("Index");
			return m_UVWs[Index];
		}//textureCoordinate

		/**
		* \brief Texture coordinate access operator, const version.
		* 
		* \param[in] Index The index of the texture coordinate.
		* \return Vector data of the requested texture coordinate.
		*/
		const Eigen::Matrix<T, 3, 1> textureCoordinate(int32_t Index)const {
			if (Index < 0 || Index >= textureCoordinatesCount()) throw IndexOutOfBoundsExcept("Index");
			return m_UVWs[Index];
		}//textureCoordinate

		/**
		* \brief Vertex color access operator.
		* 
		* \param[in] Index The index of the vertex color.
		* \return Read/write access of the vertex color.
		*/
		Eigen::Matrix<T, 3, 1>& color(int32_t Index) {
			if (Index < 0 || Index >= colorCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Colors[Index];
		}//color

		/**
		* \brief Vertex color access operator.
		* 
		* \param[in] Index The index of the vertex color.
		* \return Vector data of the requested vertex color.
		*/
		const Eigen::Matrix<T, 3, 1> color(int32_t Index)const {
			if (Index < 0 || Index >= colorCount()) throw IndexOutOfBoundsExcept("Index");
			return m_Colors[Index];
		}//color

		/**
		* \brief Submesh access operator.
		* 
		* \param[in] Index The index of the submesh.
		* \return Read/write access of the requested submesh.
		*/
		Submesh* getSubmesh(int32_t Index){
			if (Index < 0 || Index >= m_Submeshes.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Submeshes[Index];
		}//getSubmesh

		/**
		* \brief Submesh access operator, const version.
		* 
		* \param[in] Index The index of the submesh.
		* \return Read access to the submesh.
		*/
		const Submesh* getSubmesh(int32_t Index)const {
			if (Index < 0 || Index >= m_Submeshes.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Submeshes[Index];
		}//getSubmesh

		/**
		* \brief Material access operator.
		* 
		* \param[in] Index The index of the material.
		* \return Read/write access to the material.
		*/
		Material* getMaterial(int32_t Index) {
			if (Index < 0 || Index >= m_Materials.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Materials[Index];
		}//getMaterial

		/**
		* \brief Material access operator, const version.
		* 
		* \param[in] Index The index of the material.
		* \return Read access of the requested material.
		*/
		const Material* getMaterial(int32_t Index)const {
			if (Index < 0 || Index >= m_Materials.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Materials[Index];
		}//getMaterial

		/**
		* \brief Bone access operator.
		* 
		* \param[in] Index The index of the bone.
		* \return Read/write access of the requested bone.
		*/
		Bone* getBone(int32_t Index) {
			if (Index < 0 || Index >= m_Bones.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Bones[Index];
		}//getBone

		/**
		* \brief Bone access operator, const version.
		* 
		* \param[in] Index The index of the bone.
		* \return Read access of the requested bone.
		*/
		const Bone* getBone(int32_t Index)const {
			if (Index < 0 || Index >= m_Bones.size()) throw IndexOutOfBoundsExcept("Index");
			return m_Bones[Index];
		}//getBone
		
		/**
		* \brief Skeletal animation access operator.
		* 
		* \param[in] Index The index of the skeletal animation.
		* \return Read/write access of the requested skeletal animation.
		*/
		SkeletalAnimation* getSkeletalAnimation(int32_t Index) {
			if (Index < 0 || Index >= m_SkeletalAnimations.size()) throw IndexOutOfBoundsExcept("Index");
			return m_SkeletalAnimations[Index];
		}//getSkeletalAnimation

		/**
		* \brief Skeletal animation access operator, const version.
		* 
		* \param[in] Index The index of the skeletal animation.
		* \return Read access of the requested skeletal animation.
		*/
		const SkeletalAnimation* getSkeletalAnimation(int32_t Index)const {
			if (Index < 0 || Index >= m_SkeletalAnimations.size()) throw IndexOutOfBoundsExcept("Index");
			return m_SkeletalAnimations[Index];
		}//getSkeletalAnimation

		/**
		* \brief Morph target access operator.
		* 
		* \param[in] Index The index of the morph target.
		* \return Read/write access of the requested morph target.
		*/
		MorphTarget* getMorphTarget(int32_t Index) {
			if (Index < 0 || Index >= m_MorphTargets.size()) throw IndexOutOfBoundsExcept("Index");
			return m_MorphTargets[Index];
		}//getMorphTarget

		/**
		* \brief Morph target access operator.
		* 
		* \param[in] Index The index of the morph target.
		* \return Read access of the requested morph target.
		*/
		const MorphTarget* getMorphTarget(int32_t Index)const {
			if (Index < 0 || Index >= m_MorphTargets.size()) throw IndexOutOfBoundsExcept("Index");
			return m_MorphTargets[Index];
		}//getMorphTarget

		/**
		* \brief Skeleton's root bone access operator.
		* 
		* \return Read/write access of the skeleton's root bone.
		*/
		Bone* rootBone(void) {
			return m_pRootBone;
		}//rootBone

		/**
		* \brief Skeleton's root bone access operator.
		* 
		* \return Read access of the skeleton's root bone.
		*/
		const Bone* rootBone(void)const {
			return m_pRootBone;
		}//rootBone

		/**
		* \brief Computes per face normals of all submeshes.
		* 
		* \todo Move computation of normals to the submsesh data structure.
		*/
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

		/**
		* \brief Compute per face tangents of all submeshes.
		* 
		* \todo Move computation of tangents to the submesh structure.
		*/
		void computePerFaceTangents(void) {

			if (m_UVWs.size() == 0) throw CForgeExcept("No UVW coordinates. Can not compute tangents.");

			for (auto i : m_Submeshes) {
				i->FaceTangents.clear();
				for (auto F : i->Faces) {
					const Eigen::Vector3f Edge1 = m_Positions[F.Vertices[1]] - m_Positions[F.Vertices[0]];
					const Eigen::Vector3f Edge2 = m_Positions[F.Vertices[2]] - m_Positions[F.Vertices[0]];
					const Eigen::Vector3f DeltaUV1 = m_UVWs[F.Vertices[1]] - m_UVWs[F.Vertices[0]];
					const Eigen::Vector3f DeltaUV2 = m_UVWs[F.Vertices[2]] - m_UVWs[F.Vertices[0]];

					float f = DeltaUV1.x() * DeltaUV2.y() - DeltaUV2.x() + DeltaUV1.y();
					f = (std::abs(f) > 0.0f) ? 1.0f / f : 1.0f;

					Eigen::Vector3f Tangent;
					Tangent.x() = f * (DeltaUV2.y() * Edge1.x() - DeltaUV1.y() * Edge2.x());
					Tangent.y() = f * (DeltaUV2.y() * Edge1.y() - DeltaUV1.y() * Edge2.y());
					Tangent.z() = f * (DeltaUV2.y() * Edge1.z() - DeltaUV1.y() * Edge2.z());
					i->FaceTangents.push_back(Tangent);
				}//for[all faces]
			}//for[all submeshes]

		}//computeTangents

		/**
		* \brief Compute the per vertex normals.
		* 
		* \param[in] ComputePerFaceNormals Whether per face normals should be recomputed or not.
		* 
		* \todo Check this method. Change parameter to RecomputePerFaceNormals and check if per face normals were already computed.
		*/
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

		/**
		* \brief Computes per vertex tangents.
		* 
		* \param[in] ComputePerFaceTangents Whether to re-compute the per face tangents.
		* 
		* \todo Check this method. Change parameter to RecomputePerFaceTangetns and check if per face tangents were already computed.
		*/
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

		/**
		* \brief Axis aligned bounding box access operator.
		* 
		* \return Read/write access to the axis aligned bounding box.
		*/
		Box& aabb() {
			return m_AABB;
		}

		/**
		* \brief Axis aligned bounding box access operator.
		* 
		* \return Read access to the axis aligned bounding box.
		*/
		const Box aabb(void)const {
			return m_AABB;
		}//aabb

		/**
		* \brief Computes the axis aligned bounding box.
		*/
		void computeAxisAlignedBoundingBox(void) {
			if (m_Positions.size() == 0) throw CForgeExcept("Mesh contains no vertex data. Can not compute axis aligned bounding box");
			m_AABB = computeAxisAlignedBoundingBox((*this));
		}//computeAxisAlignedBoundingBox

		/**
		* \brief Applies a 4x4 matrix to the geometry data of the mesh.
		* 
		* \param[in] Mat Transformation matrix.
		*/
		void applyTransformation(const Eigen::Matrix4f Mat) {
			// transform vertices
			for (auto &i : m_Positions) {
				const Eigen::Vector4f v = Mat * Eigen::Vector4f(i.x(), i.y(), i.z(), (T)1.0);
				i = Eigen::Matrix<T, 3, 1>(v.x(), v.y(), v.z());
			}//for[positions]

			for (auto& i : m_Normals) {
				const Eigen::Vector4f v = Mat * Eigen::Vector4f(i.x(), i.y(), i.z(), (T)0.0);
				i = Eigen::Matrix<T, 3, 1>(v.x(), v.y(), v.z());
			}//for[normals]

			for (auto& i : m_Tangents) {
				const Eigen::Vector4f v = Mat * Eigen::Vector4f(i.x(), i.y(), i.z(), (T)0.0);
				i = Eigen::Matrix<T, 3, 1>(v.x(), v.y(), v.z());
			}//for[tangents]

		}//ApplyTransformation

		/**
		* \brief Changes the uvw-tiling of the mesh by the specified factor.
		* 
		* \param[in] Factor Vector containing the change factors.
		*/
		void changeUVTiling(const Eigen::Vector3f Factor) {
			for (auto& i : m_UVWs) i = i.cwiseProduct(Factor);
			if (tangentCount() > 0) computePerVertexTangents(true);
		}//changeUVTiling
			
	protected:
		std::vector<Eigen::Matrix<T, 3, 1>> m_Positions;	///< Vertex positions.
		std::vector<Eigen::Matrix<T, 3, 1>> m_Normals;		///< Per vertex normals.
		std::vector<Eigen::Matrix<T, 3, 1>> m_Tangents;		///< Per vertex tangents.
		std::vector<Eigen::Matrix<T, 3, 1>> m_UVWs;			///< Texture coordinates.
		std::vector<Eigen::Matrix<T, 3, 1>> m_Colors;		///< Vertex colors.
		std::vector<Submesh*> m_Submeshes;					///< Submeshes.
		std::vector<Material*> m_Materials;					///< Materials.
		
		// skeletal animation related
		Bone* m_pRootBone;				///< Root bone.
		std::vector<Bone*> m_Bones;		///< List of bones.
		std::vector<SkeletalAnimation*> m_SkeletalAnimations;	///< List of skeletal animations.

		// morph target related
		std::vector<MorphTarget*> m_MorphTargets;	///< List of morph target animations.

		Box m_AABB;		///< Axis aligned bounding box.
	};//T3DMesh

}//name space


#endif