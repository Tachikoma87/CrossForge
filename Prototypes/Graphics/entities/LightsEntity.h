/*****************************************************************************\
*                                                                           *
* File(s): LightsEntity.h and LightsEntity.cpp                        *
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
#ifndef __CROSSFORGE_LIGHTSENTITY_H__
#define __CROSSFORGE_LIGHTSENTITY_H__

#include <crossforge/eccs/EntityBase.h>
#include "../components/uniformbuffer/UBODirectionalLightsComponent.h"
#include "../components/uniformbuffer/UBOPointLightsComponent.h"
#include "../components/uniformbuffer/UBOSpotLightsComponent.h"
#include "../components/LightsConfigComponent.h"

namespace crossforge{
	class LightsEntity : public EntityBase {
	public:
		static inline std::string identification = "LightsEntity";

		LightsEntity();
		~LightsEntity();

		virtual void initialize();
		virtual void clear();

		LightsConfigComponentPtr getLightsConfigComponent(bool createIfNotExists = false);
		UBODirectionalLightsComponentPtr getUBODirectionalLightsComponent(bool createIfNotExists = false);
		UBOPointLightsComponentPtr getUBOPointLightsComponent(bool createIfNotExists = false);
		UBOSpotLightsComponentPtr getUBOSpotLightsComponent(bool createIfNotExists = false);
	
	protected:
		LightsEntity(const std::string childIdentification);
	};

	using LightsEntityPtr = std::shared_ptr<LightsEntity>;
	using LightsEntityCPtr = std::shared_ptr<const LightsEntity>;
}

#endif 