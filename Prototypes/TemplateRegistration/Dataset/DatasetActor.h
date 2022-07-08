#ifndef __TEMPREG_DATASETACTOR_H__
#define __TEMPREG_DATASETACTOR_H__

#include "../../../CForge/AssetIO/T3DMesh.hpp"
#include "../../../CForge/Graphics/Actors/StaticActor.h"

namespace TempReg {

	enum class DatasetRenderMode {
		FILL,
		LINE,
		POINT
	};

	class DatasetActor : public CForge::IRenderableActor {
	public:

		DatasetActor(void);
		~DatasetActor();

		void init(const CForge::T3DMesh<float>* pMesh, DatasetRenderMode RM);

		void release(void);
		void render(CForge::RenderDevice* pRDev);

		void renderMode(DatasetRenderMode RM);
		DatasetRenderMode renderMode(void)const;

	protected:
		CForge::StaticActor m_Actor;
		DatasetRenderMode m_RenderMode;
	};//DatasetActor
}

#endif