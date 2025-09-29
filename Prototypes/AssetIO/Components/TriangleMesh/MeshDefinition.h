/*****************************************************************************\
*                                                                           *
* File(s): MeshDefinition.h and MeshDefiniton.cpp            *
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
#ifndef __CFORGE_MESHDEFINITION_H__
#define __CFORGE_MESHDEFINITION_H__

#include <crossforge/Core/SLogger.h>

namespace CForge {
	class MeshDefinition {
	public:
		MeshDefinition();
		~MeshDefinition();

		void initialize();
		void clear();

		Eigen::Vector3i operator[](uint32_t index)const;
		Eigen::Vector3i& operator[](uint32_t index);

		void setFaces(std::vector<Eigen::Vector3i> faces);
		void setFace(Eigen::Vector3i face, uint32_t index);
		std::vector<Eigen::Vector3i>& getFaces();
		std::vector<Eigen::Vector3i> getFaces()const;
		Eigen::Vector3i getFace(uint32_t index)const;
		Eigen::Vector3i& getFace(uint32_t index);

		uint32_t getFaceCount()const;

		void setMaterialIndex(int32_t index);
		int32_t getMaterialindex(void)const;

	protected:
		std::vector<Eigen::Vector3i> m_faces;
		int32_t m_materialIndex;
	};

	typedef std::shared_ptr<MeshDefinition> MeshDefinitionPtr;
}

#endif 