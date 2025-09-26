/*****************************************************************************\
*                                                                           *
* File(s): NormalDataComponent.h and NormalDataComponent.cpp                *
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
#ifndef __CFORGE_NORMALDATACOMPONENT_H__
#define __CFORGE_NORMALDATACOMPONENT_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/ComponentBase.h"

namespace CForge {
	class NormalDataComponent: public ComponentBase {
	public:
		inline static std::string identification = "NormalDataComponent";

		NormalDataComponent();
		~NormalDataComponent();

		void initialize(std::vector<Eigen::Vector3f> normals = std::vector<Eigen::Vector3f>());
		void clear();

		const Eigen::Vector3f operator[](const uint32_t index)const;
		Eigen::Vector3f& operator[](const uint32_t index);

		void setNormals(std::vector<Eigen::Vector3f> normals);
		void setNormal(Eigen::Vector3f normal, uint32_t index);
		std::vector<Eigen::Vector3f> getNormals()const;
		std::vector<Eigen::Vector3f>& getNormals();
		Eigen::Vector3f getNormal(uint32_t index)const;
		Eigen::Vector3f& getNormal(uint32_t index);

		uint32_t getNormalCount()const;

	protected:
		std::vector<Eigen::Vector3f> m_normals;
	};

	typedef std::shared_ptr<NormalDataComponent> NormalDataComponentPtr;
}

#endif