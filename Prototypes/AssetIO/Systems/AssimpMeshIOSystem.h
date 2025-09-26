/*****************************************************************************\
*                                                                           *
* File(s): AssimpMeshIOSystem.h and AssimpMeshIOSystem.cpp                  *
*                                                                           *
* Content:                            *
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
#ifndef __CFORGE_SASSIMPMESHIOSYSTEM_H__
#define __CFORGE_SASSIMPMESHIOSYSTEM_H__

#include "TriangleMeshIOSystemBase.h"
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>

namespace CForge {
	class AssimpMeshIOSystem : public TriangleMeshIOSystemBase {
	public:
		inline static std::string identification = "AssimpMeshIOSystem";

		// Constructor/Destructor
		AssimpMeshIOSystem();
		~AssimpMeshIOSystem();

		void initialize() override;
		void clear() override;
		void update() override;
		bool isEntityValid(EntityBasePtr pEntity) const override;

		// overriding methods from parent class
		bool canAcceptFile(const std::string filePath, const Operation operation) const override;
		bool loadMesh(const std::string filePath, TriangleMeshEntityPtr meshEntity) override;
		bool storeMesh(const std::string filePath, TriangleMeshEntityPtr meshEntity) override;

	protected:

		void aiSceneToTriangleMeshEntity(const aiScene* pScene, TriangleMeshEntityPtr pEntity, const std::string Directory);
		void triangleMeshEntityToAiScene(TriangleMeshEntityPtr pEntity, aiScene* pScene);

		/**
		* \brief Utility method to convert an aiVector3D to an Eigen Vector3f.
		*
		* \param[in] aiVector3D Input vector.
		* \return Vector as Eigen class.
		*/
		inline Eigen::Vector3f toEigenVec(const aiVector3D Vec)const;

		/**
		* \brief Utility method to convert AssImp matrix to Eigen matrix.
		*
		* \param[in] Mat Input matrix.
		* \return Matrix as Eigen class.
		*/
		inline Eigen::Matrix4f toEigenMat(const aiMatrix4x4 Mat)const;

		/**
		* \brief Utility method to convert AssImp quaternion to Eigen quaternion.
		*
		* \param[in] Q Input quaternion.
		* \return Quaternion as Eigen class.
		*/
		inline Eigen::Quaternionf toEigenQuat(const aiQuaternion Q)const;

		/**
		* \brief Utility method to convert Eigen vector to AssImp vector.
		*
		* \param[in] Vec Input vector.
		* \return Vector as AssImp class.
		*/
		inline aiVector3D toAiVector(const Eigen::Vector3f Vec)const;

		/**
		* \brief Utility method to convert Eigen matrix to AssImp matrix.
		*
		* \param[in] Mat Input matrix.
		* \return Matrix as AssImp class.
		*/
		inline aiMatrix4x4 toAiMatrix(const Eigen::Matrix4f Mat)const;

		/**
		* \brief Utility method to convert Eigen quaternion to AssImp quaternion.
		*
		* \param[in] Q Input quaternion.
		* \return Quaternion as AssImp class.
		*/
		inline aiQuaternion toAiQuat(const Eigen::Quaternionf Q)const;

		inline aiColor4D toAiColor(const Eigen::Vector4f color)const;


		Assimp::Importer m_Importer;
		Assimp::Exporter m_Exporter;

	};

	typedef std::shared_ptr<AssimpMeshIOSystem> AssimpMeshIOSystemPtr;

}

#endif