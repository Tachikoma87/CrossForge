/*****************************************************************************\
*                                                                           *
* File(s): AssimpMeshIO.h and AssimpMeshIO.cpp                               *
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
#ifndef __CFORGE_ASSIMPMESHIO_H__
#define __CFORGE_ASSIMPMESHIO_H__

#include "I3DMeshIO.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace CForge {
	/***
	* \brief 3D mesh import/export plugin that employs AssImp library.
	*
	* \todo Implement exporting of models
	* \todo Do full documentation
	*/
	class AssimpMeshIO : public I3DMeshIO {
	public:
		AssimpMeshIO(void);
		~AssimpMeshIO(void);

		void init(void);
		void clear(void);

		void release(void);
		bool accepted(const std::string Filepath, Operation Op);

		void load(const std::string Filepath, T3DMesh<float> *pMesh);
		void store(const std::string Filepath, const T3DMesh<float>* pMesh);
	
	private:
		void aiSceneTo3DMesh(const aiScene* pScene, T3DMesh<float>* pMesh, std::string Directory);

		inline Eigen::Vector3f toEigenVec(const aiVector3D Vec)const;
		inline Eigen::Matrix4f toEigenMat(const aiMatrix4x4 Mat)const;
		inline Eigen::Quaternionf toEigenQuat(const aiQuaternion Q)const;

		T3DMesh<float>::Bone* getBoneFromName(std::string Name, std::vector<T3DMesh<float>::Bone*>* pBones);
		void retrieveBoneHierarchy(aiNode* pNode, std::vector<T3DMesh<float>::Bone*>* pBones);

		Assimp::Importer m_Importer;

	};//AssimpMeshIO
}//name space


#endif