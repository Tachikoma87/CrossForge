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
#ifndef __CROSSFORGE_RENDERINGCONTROLLER_H__
#define __CROSSFORGE_RENDERINGCONTROLLER_H__

#include <crossforge/eccs/ControllerBase.h>
#include <crossforge/graphics/entities/CanvasEntity.h>
#include "../../scene/entities/ActorInstanceEntity.h"
#include "../../scene/entities/CameraEntity.h"
#include <crossforge/graphics/entities/ActorPrefabEntity.h>
#include <crossforge/graphics/providers/SShaderProvider.h>
#include <crossforge/graphics/entities/LightsEntity.h>

namespace crossforge {
	class RenderingController : public ControllerBase {
	public:
		inline static std::string identification = "RenderingController";

		static void activateCanvas(CanvasEntityPtr pCanvas, bool clearBackground = false);
		static void drawActor(ShaderProvider::RenderPass renderPass, ActorInstanceEntityPtr pActorInstance, ActorPrefabEntityPtr pActorPrefab, CameraEntityPtr pCamera, LightsEntityPtr pLights);

		~RenderingController();
	protected:
		RenderingController(const std::string childIdentification);
		static ShaderPropertiesComponentPtr m_pShaderProperties;
	};

	using RenderingControllerPtr = std::shared_ptr<RenderingController>;
	using RenderingControllerCPtr = std::shared_ptr<const RenderingController>;

}

#endif 