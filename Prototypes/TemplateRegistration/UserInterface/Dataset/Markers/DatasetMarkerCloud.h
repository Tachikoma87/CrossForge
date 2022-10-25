#ifndef __TEMPREG_DATASETMARKERCLOUD_H__
#define __TEMPREG_DATASETMARKERCLOUD_H__

#include "DatasetMarkerInstance.h"

#include <vector>
#include <set>
#include <map>

namespace TempReg {

	class DatasetMarkerCloud {
	public:
		DatasetMarkerCloud();
		~DatasetMarkerCloud();

		void init(CForge::ISceneGraphNode* Parent, Vector3f Scale);
		void clear(void);

		void addToSceneGraph(CForge::ISceneGraphNode* pParent);
		void removeFromSceneGraph(void);

		void addMarkerInstance(size_t ID, const Vector3f MarkerPos, DatasetMarkerActor* pActor);
		void removeMarkerInstance(size_t ID);
		void clearMarkerInstances(void);

		void show(bool Show);

		void markerPosition(size_t ID, Vector3f Position);
		void markerActor(size_t ID, DatasetMarkerActor* pActor);
		const DatasetMarkerActor* markerActor(size_t ID) const;

	private:
		// scene graph root for this cloud of markers; used to connect each cloud to the scene graph of a specific viewport during rendering
		CForge::SGNTransformation m_CloudRoot;
		std::vector<DatasetMarkerInstance> m_MarkerInstances;
		std::set<size_t> m_FreeMarkerInstances;
		std::map<size_t, size_t> m_MarkerIDLookup;
		Vector3f m_MarkerScale;
	};
}

#endif