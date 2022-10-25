#ifndef __TEMPREG_DATASETMARKERINSTANCE_H__
#define __TEMPREG_DATASETMARKERINSTANCE_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

#include "../../CForge/Graphics/SceneGraph/ISceneGraphNode.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>

#include "DatasetMarkerActor.h"

using namespace Eigen;

namespace TempReg {

	class DatasetMarkerInstance {
	public:
		DatasetMarkerInstance();
		~DatasetMarkerInstance();

		void init(CForge::ISceneGraphNode* pParent, DatasetMarkerActor* pMarkerActor = nullptr, Vector3f Position = Vector3f::Zero(), Vector3f Scaling = Vector3f::Ones());
		void clear(void);

		void addToSceneGraph(CForge::ISceneGraphNode* pParent);
		void removeFromSceneGraph(void);

		void show(bool Show);

		void translation(Vector3f Translation);
		void actor(DatasetMarkerActor* pActor);
		const DatasetMarkerActor* DatasetMarkerInstance::actor(void) const;

	private:
		CForge::SGNTransformation m_TransSGN;
		CForge::SGNGeometry m_GeomSGN;
	};
}

#endif