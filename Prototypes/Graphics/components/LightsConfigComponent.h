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

		void initialize(const std::shared_ptr<const LightsConfigComponent> pRef = nullptr);
		void clear() override;

		/** Accessor **/
		uint32_t& activeDirectionalLights();
		uint32_t& activePointLights();
		uint32_t& activeSpotLights();
		uint32_t& directionalLightsSize();
		uint32_t& pointLightsSize();
		uint32_t& spotLightsSize();

		/** Getter **/
		const uint32_t getActiveDirectionalLights()const;
		const uint32_t getActivePointLights()const;
		const uint32_t getActiveSpotLights()const;
		const uint32_t getDirectionalLightsSize()const;
		const uint32_t getPointLightsSize()const;
		const uint32_t getSpotLightsSize()const;

		/** Setter **/
		void setActiveDirectionalLights(const uint32_t activeLights);
		void setActivePointLights(const uint32_t activeLights);
		void setActiveSpotLights(const uint32_t activeLights);
		void setDirectionalLightsSize(const uint32_t size);
		void setPointLightsSize(const uint32_t size);
		void setSpotLightsSize(const uint32_t size);


	protected:
		LightsConfigComponent(const std::string childIdentification);

		uint32_t m_activeDirectionalLights;
		uint32_t m_activePointLights;
		uint32_t m_activeSpotLights;
		uint32_t m_directionalLightsSize;
		uint32_t m_pointLightsSize;
		uint32_t m_spotLightsSize;
	};

	using LightsConfigComponentPtr = std::shared_ptr<LightsConfigComponent>;
	using LightsConfigComponentCPtr = std::shared_ptr<const LightsConfigComponent>;
}

#endif