/*****************************************************************************\
*                                                                           *
* File(s): SceneNodeEntityController.h and SceneNodeEntityController.cpp*
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
#ifndef __CROSSFORGE_SCENENODEENTITYCONTROLLER_H__
#define __CROSSFORGE_SCENENODEENTITYCONTROLLER_H__

#include <crossforge/eccs/ControllerBase.h>
#include <crossforge/scene/entities/SceneNodeEntity.h>

namespace crossforge {
	class SceneNodeEntityController : public ControllerBase {
	public:
		static inline std::string identification = "SceneNodeEntityController";

		static void buildGlobalTransformation(SceneNodeEntityPtr pSceneNode, Transformation3DComponentPtr pParentTransform = nullptr, uint32_t depthLevel = 0);
		static void gatherRenderableObjects(SceneNodeEntityPtr pSceneNode, std::vector<SceneNodeEntityPtr>& visibleObjects);

		static void updateTransformationUbo(SceneNodeEntityPtr pSceneNode);

		static void rotate(SceneNodeEntityPtr pSceneNode, const Eigen::Quaternionf rotation);
		static void rotate(SceneNodeEntityPtr pSceneNode, const float theta, const Eigen::Vector3f axis);
		static void moveForward(SceneNodeEntityPtr pSceneNode, const float delta);
		static void moveRight(SceneNodeEntityPtr pSceneNode, const float delta);
		static void moveUp(SceneNodeEntityPtr pSceneNode, const float delta);
		static void yaw(SceneNodeEntityPtr pSceneNode, const float theta);
		static void roll(SceneNodeEntityPtr pSceneNode, const float theta);
		static void pitch(SceneNodeEntityPtr pSceneNode, const float theta);
		static void lookAt(SceneNodeEntityPtr pSceneNode, Eigen::Vector3f origin, Eigen::Vector3f target, Eigen::Vector3f up = Eigen::Vector3f::UnitY());

	protected:
		SceneNodeEntityController(const std::string childIdentification);
		~SceneNodeEntityController();
	};
	using SceneNodeEntityControllerPtr = std::shared_ptr<SceneNodeEntityController>;
	using SceneNodeEntityControllerCPtr = std::shared_ptr<const SceneNodeEntityController>;
}

#endif 
