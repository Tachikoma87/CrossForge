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
		
		ShaderEntity();
		~ShaderEntity();

		void initialize();
		void clear();

		ShaderSourceComponentPtr getShaderSourceComponent(bool createIfNotExists = false);
		RenderingShaderComponentPtr getRenderingShaderComponent(bool createIfNotExists = false);
		ShaderPropertiesComponentPtr getShaderPropertiesComponent(bool createIfNotExists = false);

	protected:
		ShaderEntity(const std::string childIdentification);

	};

	using ShaderEntityPtr = std::shared_ptr<ShaderEntity>;
	using ShaderEntityCPtr = std::shared_ptr<const ShaderEntity>;
}

#endif 