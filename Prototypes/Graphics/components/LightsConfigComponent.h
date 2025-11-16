/*****************************************************************************\
*                                                                           *
* File(s): LightsConfigComponent.h and LightsConfigComponent.cpp    *
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
#ifndef __CROSSFORGE_LIGHTSCONFIGCOMPONENT_H__
#define __CROSSFORGE_LIGHTSCONFIGCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class LightsConfigComponent : public ComponentBase {
	public:
		static inline std::string identification = "SceneLightsConfigComponent";

		LightsConfigComponent();
		~LightsConfigComponent();

		void initialize();
		void clear() override;

		uint32_t& activeDirectionalLights();
		uint32_t& activePointLights();
		uint32_t& activeSpotLights();

		uint32_t& directionalLightsUBOSize();
		uint32_t& pointLightsUBOSize();
		uint32_t& spotLightsUBSSize();

	protected:
		LightsConfigComponent(const std::string childIdentification);

		uint32_t m_activeDirectionalLights;
		uint32_t m_activePointLights;
		uint32_t m_activeSpotLights;
		uint32_t m_directionalLightsSize;
		uint32_t m_pointLightsSize;
		uint32_t m_spotLightsSize;
	};

	typedef std::shared_ptr<LightsConfigComponent> SceneLightsConfigComponentPtr;
}

#endif