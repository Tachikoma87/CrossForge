/*****************************************************************************\
*                                                                           *
* File(s): SCrossForgeSimpleSceneApp.h and SCrossForgeSimpleSceneApp.cpp                       *
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
#ifndef __CROSSFORGE_SCROSSFORGESIMPLESCENEAPP_H__
#define __CROSSFORGE_SCROSSFORGESIMPLESCENEAPP_H__

#include <crossforge/application/ApplicationBase.h>
#include <crossforge/graphics/entities/WindowEntity.h>
#include <crossforge/graphics/entities/CanvasEntity.h>
#include <crossforge/input/entities/InputDeviceEntity.h>
#include <crossforge/assetio/SAssetIOProvider.h>
#include <crossforge/assetio/entities/VideoEntity.h>
#include <crossforge/graphics/entities/ActorPrefabEntity.h>
#include <crossforge/graphics/entities/LightsEntity.h>


#include "../scene/entities/CameraEntity.h"
#include "../scene/entities/ActorInstanceEntity.h"
#include "../scene/systems/MovementSystem.h"
#include "../scene/entities/SceneEntity.h"
#include "../scene/systems/CameraSystem.h"

namespace crossforge {
	class SCrossForgeSimpleSceneApp : public ApplicationBase {
	public:
		static std::shared_ptr<SCrossForgeSimpleSceneApp> instance();
		static void destroy();

		void initialize() override;
		void update() override;

		~SCrossForgeSimpleSceneApp();
	protected:
		static std::shared_ptr<SCrossForgeSimpleSceneApp> m_pInstance;
		SCrossForgeSimpleSceneApp();

		void testGraphicsUtility();


		WindowEntityPtr m_pMainWin;
		CanvasEntityPtr m_pMainCanvas;
		InputDeviceEntityPtr m_pInputDevice;

		uint64_t m_timestampStart;
		uint64_t m_timestampLastFpsPrint;
		uint64_t m_frameCount;

		ActorPrefabEntityPtr m_pDuckActorPrefab;
		ActorPrefabEntityPtr m_pHelmetActorPrefab;
		ActorPrefabEntityPtr m_pGroundPlanePrefab;

		ActorInstanceEntityPtr m_pDuckActorInstance;
		ActorInstanceEntityPtr m_pDuckActorInstance2;
		ActorInstanceEntityPtr m_pHelmetActorInstance;

		ActorInstanceEntityPtr m_pGroundPlaneInstance;

		std::vector<ActorInstanceEntityPtr> m_pActorInstances;

		CameraEntityPtr m_pCameraEntity;
		LightsEntityPtr m_pSceneLights;

		AssetIOProviderPtr m_pAssetIO;

		SceneObjectEntityPtr m_pRootNode;

		MovementSystemPtr m_pMovementSystem;

		SceneEntityPtr m_pSceneEntity;

		#ifdef CROSSFORGE_OPTION_FFMPEG
		VideoEntityPtr m_pVideoRecorder;
		#endif

		CameraSystemPtr m_pCameraSystem;
	};

	typedef SCrossForgeSimpleSceneApp CrossForgeSimpleSceneApp;
	typedef std::shared_ptr<SCrossForgeSimpleSceneApp> CrossForgeSimpleSceneAppPtr;
}

#endif 