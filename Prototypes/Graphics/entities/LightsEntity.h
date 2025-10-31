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

#include <crossforge/ecs/EntityBase.h>
#include "../components/uniformbuffer/UBODirectionalLightsComponent.h"
#include "../components/uniformbuffer/UBOPointLightsComponent.h"
#include "../components/uniformbuffer/UBOSpotLightsComponent.h"
#include "../components/LightsConfigComponent.h"

namespace crossforge{
	class LightsEntity : public EntityBase {
	public:
		static inline std::string identification = "LightsEntity";

		enum LightsEntityComponents: uint8_t {
			COMPONENT_LIGHTS_CONFIG = 0x01,
			COMPONENT_DIRECTIONAL_LIGHTS_UBO = 0x02,
			COMPONENT_POINT_LIGHTS_UBO = 0x04,
			COMPONENT_SPOT_LIGHTS_UBO = 0x08,
			COMPONENTS_ALL = 0xFF,
		};

		LightsEntity(uint8_t componentsMap);
		~LightsEntity();

		virtual void initialize(uint8_t componentsMap);
		virtual void clear();

		SceneLightsConfigComponentPtr getLightsConfigComponent();
		UBODirectionalLightsComponentPtr getUBODirectionalLightsComponent();
		UBOPointLightsComponentPtr getUBOPointLightsComponent();
		UBOSpotLightsComponentPtr getUBOSpotLightsComponent();
	
	protected:
		LightsEntity(const std::string childIdentification);
	};

	typedef std::shared_ptr<LightsEntity> LightsEntityPtr;
}

#endif 