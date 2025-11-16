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

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class SphericalTransformationComponent : public ComponentBase {
	public:
		static inline std::string identification = "SphericalTransformationComponent";

		SphericalTransformationComponent();
		~SphericalTransformationComponent();

		void initialize();
		void clear() override;

		Eigen::Vector3f& origin();
		float& rho();
		float& theta();
		float& phi();

		Eigen::Vector3f getPosition();

	protected:
		SphericalTransformationComponent(const std::string childIdentification);

		Eigen::Vector3f m_origin;
		float m_rho;	// radius or distance from origin
		float m_theta;	// azimuthal angle
		float m_phi;	// polar angle

	};

	typedef std::shared_ptr<SphericalTransformationComponent> SphericalTransformationComponentPtr;
}


#endif