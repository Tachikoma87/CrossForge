#pragma once

#include "InstanceActor.h"
#include "CForge/Graphics/SceneGraph/ISceneGraphNode.h"
#include "../../../CForge/Graphics/Actors/IRenderableActor.h"

namespace CForge {

	class CFORGE_API InstanceSGN : public ISceneGraphNode {
	public:
		InstanceSGN(void);
		~InstanceSGN(void);

		void init(ISceneGraphNode* pParent, InstanceActor* pRenderable, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale);
		void buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale);

		void clear(void);

		void position(Eigen::Vector3f Position);
		void rotation(Eigen::Quaternionf Rotation);
		void scale(Eigen::Vector3f Scale);
		void actor(InstanceActor* pActor);

		Eigen::Vector3f position(void)const;
		Eigen::Quaternionf rotation(void)const;
		Eigen::Vector3f scale(void)const;
		InstanceActor* actor(void)const;

		void update(float FPSScale);
	
		void render(RenderDevice* pRDev, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale);
	private:
		Eigen::Vector3f m_Position;
		Eigen::Quaternionf m_Rotation;
		Eigen::Vector3f m_Scale;
		InstanceActor* m_pRenderable;
	};
}
