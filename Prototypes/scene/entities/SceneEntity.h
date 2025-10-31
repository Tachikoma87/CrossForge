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

		enum SceneEntityComponents: uint8_t {
			
			COMPONENTS_ALL = 0xFF,
		};

		SceneEntity(uint8_t componentsMask = 0);
		~SceneEntity();

		void initialize(uint8_t componentsMask);
		void clear();


	protected:
		SceneEntity(const std::string childIdentification);
	};

	typedef std::shared_ptr<SceneEntity> SceneEntityPtr;
}

#endif 