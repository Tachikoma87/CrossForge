/*****************************************************************************\
*                                                                           *
* File(s): Movement3DComponent.h and Movement3DComponent.cpp    *
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
#ifndef __CROSSFORGE_MOVEMENT3DCOMPONENT_H__
#define __CROSSFORGE_MOVEMENT3DCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class Movement3DComponent : public ComponentBase {
	public:
		static inline std::string identification = "Movement3DComponent";
		Movement3DComponent();
		~Movement3DComponent();

		void initialize();
		void clear() override;

		Eigen::Vector3f& positionDelta();
		Eigen::Quaternionf& rotationDelta();
		Eigen::Vector3f& scaleDelta();

	protected:
		Movement3DComponent(const std::string childIdentification);

		Eigen::Vector3f m_positionDelta;
		Eigen::Quaternionf m_rotationDetla;
		Eigen::Vector3f m_scaleDelta;
	};
	typedef std::shared_ptr<Movement3DComponent> Movement3DComponentPtr;
}

#endif 