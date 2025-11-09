/*****************************************************************************\
*                                                                           *
* File(s): SceneObjectEntityController.h and SceneObjectEntityController.cpp*
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
#ifndef __CROSSFORGE_SCENEOBJECTENTITYCONTROLLER_H__
#define __CROSSFORGE_SCENEOBJECTENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/SceneObjectEntity.h"
#include "../components/Transformation3DComponent.h"

namespace crossforge {
	class SceneObjectEntityController : public ControllerBase {
	public:
		static inline std::string identification = "SceneObjectEntityController";

		static void buildGlobalTransformation(SceneObjectEntityPtr pSceneObject, Transformation3DComponentPtr pParentTransform = nullptr, uint32_t depthLevel = 0);
		static void gatherRenderableObjects(SceneObjectEntityPtr pSceneObject, std::vector<SceneObjectEntityPtr>& visibleObjects);

		static void updateTransformationUbo(SceneObjectEntityPtr pSceneObject);

		static void rotate(SceneObjectEntityPtr pSceneObjectEntity, const Eigen::Quaternionf rotation);
		static void moveForward(SceneObjectEntityPtr pSceneObjectEntity, const float delta);
		static void moveRight(SceneObjectEntityPtr pSceneObjectEntity, const float delta);
		static void moveUp(SceneObjectEntityPtr pSceneObjectEntity, const float delta);
		static void yaw(SceneObjectEntityPtr pSceneObjectEntity, const float theta);
		static void roll(SceneObjectEntityPtr pSceneObjectEntity, const float theta);
		static void pitch(SceneObjectEntityPtr pSceneObjectEntity, const float theta);

	protected:
		SceneObjectEntityController(const std::string childIdentification);
		~SceneObjectEntityController();
	};
	typedef std::shared_ptr<SceneObjectEntityController> SceneObjectEntityControllerPtr;
}

#endif 
