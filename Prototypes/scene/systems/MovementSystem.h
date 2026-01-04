/*****************************************************************************\
*                                                                           *
* File(s): MovementSystem.h and MovementSystem.cpp                        *
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
#ifndef __CROSSFORGE_MOVEMENTSYSTEM_H__
#define __CROSSFORGE_MOVEMENTSYSTEM_H__

#include <crossforge/ecs/SystemBase.h>

namespace crossforge {
	class MovementSystem : public SystemBase {
	public:
		static inline std::string identification = "MovementSystem";

		MovementSystem();
		~MovementSystem();

		void initialize();
		void clear();
		void update();
		bool isEntityValid(EntityBasePtr pEntity)const;

	protected:
		MovementSystem(const std::string childIdentification);

	};

	using MovementSystemPtr = std::shared_ptr<MovementSystem>;
	using MovementSystemCPtr = std::shared_ptr<const MovementSystem>;
}


#endif 