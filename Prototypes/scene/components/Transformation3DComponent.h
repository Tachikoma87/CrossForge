/*****************************************************************************\
*                                                                           *
* File(s): Transformation3DComponent.h and Transformation3DComponent.cpp                *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_TRANSFORMATION3DCOMPONENT_H__
#define __CROSSFORGE_TRANSFORMATION3DCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class Transformation3DComponent : public ComponentBase {
	public:
		static inline std::string identification = "Transformation3DComponent";

		Eigen::Vector3f& localPosition();
		Eigen::Vector3f& globalPosition();
		Eigen::Quaternionf& localRotation();
		Eigen::Quaternionf& globalRotation();
		Eigen::Vector3f& localScale();
		Eigen::Vector3f& globalScale();

		void initialize();
		void clear();

		Transformation3DComponent();
		~Transformation3DComponent();
	protected:
		Transformation3DComponent(const std::string childIdentification);

		Eigen::Vector3f m_localPosition;
		Eigen::Vector3f m_globalPosition;
		Eigen::Quaternionf m_localRotation;
		Eigen::Quaternionf m_globalRotation;
		Eigen::Vector3f m_localScale;
		Eigen::Vector3f m_globalScale;

	};
	typedef std::shared_ptr<Transformation3DComponent> Transformation3DComponentPtr;
}

#endif 