#ifndef __TEMPREG_DATASETDISPLAYDATA_H__
#define __TEMPREG_DATASETDISPLAYDATA_H__

#include "../../../CForge/AssetIO/T3DMesh.hpp"
#include "../../../CForge/Graphics/SceneGraph/ISceneGraphNode.h"
#include "../../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include "Markers/DatasetMarkerInstance.h"
#include "Markers/DatasetMarkerCloud.h"
#include "DatasetActor.h"

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {

	class DatasetDisplayData {
	public:
		enum class DatasetColor {
			SOLID_COLOR,
			FITTING_ERROR_COLOR
		};

		DatasetDisplayData(void);
		~DatasetDisplayData();

		void initAsMesh(std::string Filepath, Vector3f SolidColor, Vector3f MarkerScale = Vector3f(0.5f, 0.5f, 0.5f), bool ShowFeatCorrMarkers = true, bool ShowAutoCorrMarkers = true);
		void initAsPointCloud(std::string Filepath, Vector3f SolidColor, Vector3f MarkerScale = Vector3f(0.5f, 0.5f, 0.5f), bool ShowFeatCorrMarkers = true, bool ShowAutoCorrMarkers = true);
		
		void updateModelData(const Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& VertexPositions);
		void addToSceneGraph(CForge::ISceneGraphNode* pParent);
		void removeFromSceneGraph(void);
		void clear(void);

		void showDatasetAsSurface(bool Show);
		const bool showDatasetAsSurface(void) const;
		void showDatasetAsWireframe(bool Show);
		const bool showDatasetAsWireframe(void) const;

		void solidColor(Vector3f Color);
		void activateSolidColor(void);
		void fittingErrorColors(std::vector<Vector3f>& VertexColors);		
		void activateFittingErrorColors(void);
		const DatasetColor activeDatasetColor(void) const;

		void placeSingleMarker(DatasetMarkerActor* pMarkerActor, Vector3f Pos);
		void hideSingleMarker(void);
		void addFeatureCorrMarker(size_t MarkerID, Vector3f Pos, DatasetMarkerActor* pMarkerActor);
		void removeFeatureCorrMarker(size_t MarkerID);
		void setFeatureCorrMarkerColor(size_t MarkerID, DatasetMarkerActor* pMarkerActor);
		void setFeatureCorrMarkerPosition(size_t MarkerID, Vector3f Pos);
		void showFeatureCorrMarkers(bool Show);
		void addAutomaticCorrMarker(size_t MarkerID, Vector3f Pos, DatasetMarkerActor* pMarkerActor);
		void removeAutomaticCorrMarker(size_t MarkerID);
		void setAutomaticCorrMarkerColor(size_t MarkerID, DatasetMarkerActor* pMarkerActor);
		void setAutomaticCorrMarkerPosition(size_t MarkerID, Vector3f Pos);
		void showAutomaticCorrMarkers(bool Show);
		void clearFeatureCorrMarkers(void);
		void clearAutomaticCorrMarkers(void);

	private:
		CForge::T3DMesh<float> m_RawModelData;
		bool m_IsMesh;

		DatasetActor::DatasetRenderMode m_DatasetRenderMode;
		bool m_ShowAsSurface;
		bool m_ShowAsWireframe;

		DatasetActor* m_pSurfaceActor;
		DatasetActor* m_pSecondaryWireframeActor;
		
		CForge::SGNGeometry m_SurfaceGeomSGN;
		CForge::SGNGeometry m_SecondaryWireframeGeomSGN;

		Vector3f m_DefaultSolidColor;
		Vector3f m_SolidColor;
		std::vector<Vector3f> m_FittingErrorColors;

		DatasetColor m_ActiveColor;

		DatasetMarkerInstance m_SingleMarker;
		DatasetMarkerCloud m_FeatCorrMarkers;
		DatasetMarkerCloud m_AutoCorrMarkers;
	};
}

#endif