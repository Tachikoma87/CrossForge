/*****************************************************************************\
*                                                                           *
* File(s): ShaderEntity.h and ShaderEntity.cpp                        *
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
#ifndef __CROSSFORGE_SHADERENTITY_H__
#define __CROSSFORGE_SHADERENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/shader/ShaderSourceComponent.h"
#include "../components/shader/RenderingShaderComponent.h"
#include "../components/shader/ShaderPropertiesComponent.h"

namespace crossforge {
	class ShaderEntity : public EntityBase {
	public:
		static inline std::string identification = "ShaderEntity";
		enum ShaderEntityComponents: uint8_t {
			SHADER_SOURCE_COMPONENT = 0x01,
			RENDERING_SHADER_COMPONENT = 0x02,
			SHADER_PROPERTIES_COMPONENT = 0x04,
			COMPONENTS_ALL = 0xFF
		};

		ShaderEntity(uint8_t componentsBitmask = 0);
		~ShaderEntity();

		void initialize(uint8_t componentsBitmask);
		void clear();

		ShaderSourceComponentPtr getShaderSourceComponent();
		RenderingShaderComponentPtr getRenderingShaderComponent();
		ShaderPropertiesComponentPtr getShaderPropertiesComponent();

	protected:
		ShaderEntity(const std::string childIdentification);

	};

	typedef std::shared_ptr<ShaderEntity> ShaderEntityPtr;
}

#endif 