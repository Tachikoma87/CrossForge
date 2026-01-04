/*****************************************************************************\
*                                                                           *
* File(s): SceneEntity.h and SceneEntity.cpp                        *
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
#ifndef __CROSSFORGE_SCENEENTITY_H__
#define __CROSSFORGE_SCENEENTITY_H__

#include <crossforge/ecs/EntityBase.h>

namespace crossforge {
	class SceneEntity : public EntityBase {
	public:
		static inline std::string identification = "SceneEntity";

		SceneEntity();
		~SceneEntity();

		void initialize();
		void clear();

	protected:
		SceneEntity(const std::string childIdentification);
	};

	using SceneEntityPtr = std::shared_ptr<SceneEntity>;
	using SceneEntityCPtr = std::shared_ptr<const SceneEntity>;
}

#endif 