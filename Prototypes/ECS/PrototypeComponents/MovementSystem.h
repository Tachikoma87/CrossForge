/*****************************************************************************\
*                                                                           *
* File(s): MovementSystem.h and MovementSystem.cpp                    *
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
#ifndef __CFORGE_MOVEMENTSYSTEM_H__
#define __CFORGE_MOVEMENTSYSTEM_H__

#include "../SystemBase.h"

namespace CForge {
	class MovementSystem : public SystemBase {
	public:
		inline static const std::string identification = "MovementSystem";

		MovementSystem();
		~MovementSystem();

		void Initialize() override;
		void Clear() override;
		void Update() override;

		bool IsEntityValid(EntityBasePtr entity)const override;
		bool IsEntityRegistered(EntityBasePtr entity)const override;

		bool AddEntity(EntityBasePtr pEntity);


	protected:
		std::vector<EntityBasePtr> m_entities;
	};

	typedef std::shared_ptr<MovementSystem> MovementSystemPtr;
}

#endif 