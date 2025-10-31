/*****************************************************************************\
*                                                                           *
* File(s): LightEntity.h and LightEntity.cpp                        *
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
#ifndef __CROSSFORGE_LIGHTENTITY_H__
#define __CROSSFORGE_LIGHTENTITY_H__

#include "SceneObjectEntity.h"

namespace crossforge {
	class LightEntity : public SceneObjectEntity {
	public:
		static inline std::string identification = "LightEntity";

		enum LightEntityComponents {
			COMPONENTS_ALL = 0xFF,
		};

		LightEntity(uint8_t componentsBitmask);
		~LightEntity();

		void initialize(uint8_t componentsBitmask);
		void clear();

	protected:
		LightEntity(const std::string childIdentification);

	};
	typedef std::shared_ptr<LightEntity> LightEntityPtr;
}

#endif 