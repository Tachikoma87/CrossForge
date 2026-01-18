/*****************************************************************************\
*                                                                           *
* File(s): SphericalTransformationComponent.h and SphericalTransformationComponent.cpp               *
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
#ifndef __CROSSFORGE_SPHERICALTRANSFORMATIONCOMPONENT_H__
#define __CROSSFORGE_SPHERICALTRANSFORMATIONCOMPONENT_H__

#include <crossforge/eccs/ComponentBase.h>

namespace crossforge {
	class SphericalTransformationComponent : public ComponentBase {
	public:
		static inline std::string identification = "SphericalTransformationComponent";

		SphericalTransformationComponent();
		~SphericalTransformationComponent();

		void initialize(const std::shared_ptr<const SphericalTransformationComponent> pRef = nullptr);
		void clear() override;

		const Eigen::Vector3f getPosition()const;

		/* accessor */
		Eigen::Vector3f& origin();
		float& rho();
		float& theta();
		float& phi();

		/* Getters */
		const Eigen::Vector3f getOrigin()const;
		const float getRho()const;
		const float getTheta()const;
		const float getPhi()const;

		/* Setters */
		void setOrigin(const Eigen::Vector3f origin);
		void setRho(const float rho);
		void setTheta(const float theta);
		void setPhi(const float phi);
		

	protected:
		SphericalTransformationComponent(const std::string childIdentification);

		Eigen::Vector3f m_origin;
		float m_rho;	// radius or distance from origin
		float m_theta;	// azimuthal angle
		float m_phi;	// polar angle
	};

	using SphericalTransformationComponentPtr = std::shared_ptr<SphericalTransformationComponent>;
	using SphericalTransformationComponentCPtr = std::shared_ptr<const SphericalTransformationComponent>;
}


#endif