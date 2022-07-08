#ifndef __TEMPREG_DATASETDISPLAYDATA_H__
#define __TEMPREG_DATASETDISPLAYDATA_H__

#include "../../../CForge/AssetIO/T3DMesh.hpp"
#include "../../../CForge/Graphics/SceneGraph/ISceneGraphNode.h"
#include "../../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "DatasetCommon.h"
#include "DatasetActor.h"

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {

	enum class DatasetShadingMode {
		SOLID_COLOR,
		HAUSDORFF_DISTANCE
	};

	class DatasetDisplayData {
	public:
		DatasetDisplayData(void);
		~DatasetDisplayData();

		void init(
			DatasetType DT, DatasetGeometryType GT, CForge::T3DMesh<float>* pModel, Vector3f Color,
			CForge::ISceneGraphNode* pParent, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale);

		void addToSceneGraph(CForge::ISceneGraphNode* pParent);
		void removeFromSceneGraph(void);
		void clear(void);

		DatasetShadingMode shadingMode(void) const;
		void shadingMode(DatasetShadingMode Mode);

		void show(bool Show);
		void showPrimitives(bool Show);
		void showWireframe(bool Show);
		bool shown(void);

		void primitivesColor(CForge::T3DMesh<float>* pModel, Vector3f Color);
		void wireframeColor(CForge::T3DMesh<float>* pModel, Vector3f Color);

		Quaternionf rotation(void) const;
		void rotation(Quaternionf Rotation);

		Vector3f translation(void) const;
		void translation(Vector3f Translation);

		CForge::SGNTransformation* transformationSGN(void);

		Matrix4f modelMatrix(void);

	private:
		void initPrimitivesActor(DatasetGeometryType GT, CForge::T3DMesh<float>* pModel, Vector3f Color);
		void initWireframeActor(CForge::T3DMesh<float>* pModel, Vector3f Color);
		void initSceneGraphData(DatasetGeometryType GT, CForge::ISceneGraphNode* pParent, Eigen::Vector3f Translation, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale);

		DatasetShadingMode m_ShadingMode;

		DatasetActor* m_pPrimitivesActor;
		DatasetActor* m_pWireframeActor;
		
		CForge::SGNTransformation m_TransSGN;
		CForge::SGNGeometry m_PrimitivesGeomSGN;
		CForge::SGNGeometry m_WireframeGeomSGN;
	};

	//struct DatasetDisplayData { //TODO turn into class
	//	DatasetShadingMode ShadingMode;
	//	DatasetActor* pPrimitivesActor;
	//	DatasetActor* pWireframeActor;
	//	CForge::SGNTransformation TransSGN;
	//	CForge::SGNGeometry PrimitivesGeomSGN;
	//	CForge::SGNGeometry WireframeGeomSGN;
	//};//DatasetDisplayData
}

#endif