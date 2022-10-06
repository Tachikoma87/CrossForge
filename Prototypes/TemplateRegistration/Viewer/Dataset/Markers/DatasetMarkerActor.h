#ifndef __TEMPREG_DATASETMARKERACTOR_H__
#define __TEMPREG_DATASETMARKERACTOR_H__

#include "../../../CForge/AssetIO/T3DMesh.hpp"
#include "../../../CForge/Graphics/Actors/StaticActor.h"

namespace TempReg {

	class DatasetMarkerActor : public CForge::IRenderableActor {
	public:
		DatasetMarkerActor(void);
		~DatasetMarkerActor();

		void init(const CForge::T3DMesh<float>* pMesh, bool UseOffset);
		void offset(bool UseOffset);

		void release(void);
		void render(CForge::RenderDevice* pRDev);

	protected:
		bool m_UsePolygonOffset;
		CForge::StaticActor m_Actor;
	};//DatasetMarkerActor

}

#endif