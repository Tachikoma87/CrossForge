/*****************************************************************************\
*                                                                           *
* File(s): RenderingSystem.h and RenderingSystem.cpp                        *
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
#ifndef __CROSSFORGE_RENDERINGSYSTEM_H__
#define __CROSSFORGE_RENDERINGSYSTEM_H__

#include <crossforge/ecs/SystemBase.h>
#include <crossforge/graphics/entities/SceneEntity.h>

namespace crossforge {
	class RenderingSystem : public SystemBase {
	public:
		inline static std::string identification = "RenderingSystem";

		RenderingSystem();
		~RenderingSystem();

		void initialize() override;
		void clear() override;
		void update() override;
		bool isEntityValid(EntityBasePtr pEntity)const override;

		void renderScene(SceneEntityPtr pScene);

	protected:

	};

	typedef std::shared_ptr<RenderingSystem> RenderingSystemPtr;
}

#endif 