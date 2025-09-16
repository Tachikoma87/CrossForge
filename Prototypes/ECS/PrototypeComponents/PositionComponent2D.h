/*****************************************************************************\
*                                                                           *
* File(s): PositionComponent.h and PositionComponent.cpp                    *
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
#ifndef __CFORGE_POSITIONCOMPONENT_H__
#define __CFORGE_POSITIONCOMPONENT_H__

#include <crossforge/Math/CForgeMath.h>
#include "../ComponentBase.h"


namespace CForge {
	class PositionComponent2D : public ComponentBase {
	public:
		inline static const std::string identification = "PositionComponent2D";

		PositionComponent2D(const Eigen::Vector2f pos = Eigen::Vector2f::Zero());
		~PositionComponent2D();

		Eigen::Vector2f GetPosition()const;
		Eigen::Vector2f& GetPosition();
		void SetPosition(const Eigen::Vector2f pos);

	protected:
		Eigen::Vector2f m_position;
	};

	typedef std::shared_ptr<PositionComponent2D> PositionComponent2DPtr;
}


#endif