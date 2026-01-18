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

#include <crossforge/eccs/ComponentBase.h>

namespace crossforge {
	class Transformation3DComponent : public ComponentBase {
	public:
		static inline std::string identification = "Transformation3DComponent";

		Transformation3DComponent();
		~Transformation3DComponent();

		void initialize(const std::shared_ptr<const Transformation3DComponent> pRef = nullptr);
		void clear() override;

		/* Accessor */
		Eigen::Vector3f& localPosition();
		Eigen::Vector3f& globalPosition();
		Eigen::Quaternionf& localRotation();
		Eigen::Quaternionf& globalRotation();
		Eigen::Vector3f& localScale();
		Eigen::Vector3f& globalScale();

		/* Getter */
		const Eigen::Vector3f getLocalPosition()const;
		const Eigen::Vector3f getGlobalPosition()const;
		const Eigen::Quaternionf getLocalRotation()const;
		const Eigen::Quaternionf getGlobalRotation()const;
		const Eigen::Vector3f getLocalScale()const;
		const Eigen::Vector3f getGlobalScale()const;
		
		/** Setter */
		void setLocalPosition(const Eigen::Vector3f position);
		void setGlobalPosition(const Eigen::Vector3f position);
		void setLocalRotation(const Eigen::Quaternionf rotation);
		void setGlobalRotation(const Eigen::Quaternionf rotation);
		void setLocalScale(const Eigen::Vector3f scale);
		void setGlobalScale(const Eigen::Vector3f scale);
	protected:
		Transformation3DComponent(const std::string childIdentification);

		Eigen::Vector3f m_localPosition;
		Eigen::Vector3f m_globalPosition;
		Eigen::Quaternionf m_localRotation;
		Eigen::Quaternionf m_globalRotation;
		Eigen::Vector3f m_localScale;
		Eigen::Vector3f m_globalScale;

	};
	using Transformation3DComponentPtr = std::shared_ptr<Transformation3DComponent>;
	using Transformation3DComponentCPtr = std::shared_ptr<const Transformation3DComponent>;
}

#endif 