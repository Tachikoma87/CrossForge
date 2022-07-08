#ifndef __TEMPREG_VIEWPORTRENDERMANAGER_H__
#define __TEMPREG_VIEWPORTRENDERMANAGER_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/VirtualCamera.h"
#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>

#include <map>
#include <set>

#include "Dataset/DatasetCommon.h"
#include "Dataset/DatasetGeometryData.h"
#include "Dataset/DatasetDisplayData.h"
#include "Dataset/Markers/DatasetMarkerCommon.h"
#include "Dataset/Markers/DatasetMarkerInstance.h"
#include "Dataset/Markers/DatasetMarkerCloud.h"

using namespace Eigen;

namespace TempReg {
	
	enum class ViewportArrangementType {
		ONE_FULLSCREEN, // default viewport arrangement
		TWO_COLUMNS,
		THREE_BIGTOP,
		THREE_BIGBOTTOM,
		THREE_BIGLEFT,
		THREE_BIGRIGHT,
		THREE_EVEN,
		THREE_COLUMNS,
		FOUR_EVEN
	};

	enum class ViewportProjectionMode {
		PERSPECTIVE,
		ORTHO
	};

	enum class ViewportControlMode {
		GLOBAL, // global view control
		LOCAL // individual view controls for each Viewport
	};

	enum class DisplayDataArrangementMode {
		LAYERED,
		SIDE_BY_SIDE
	};
	
	class ViewportRenderManager {
	public:
		struct GlobalViewportState {
			bool GlobalMarkers; // Toggle whether vertex markers should be displayed across all active DatasetViews or only within the Viewport containing the mouse cursor
			bool DisplayCorrespondences; // Toggle whether pairs of corresponding vertices should be highlighted (with vertex markers)

			Vector3f DefaultTemplateColor;
			Vector3f DefaultTargetColor;

			GlobalViewportState() :
				GlobalMarkers(false), DisplayCorrespondences(false) {}
		};

		ViewportRenderManager(size_t MaxViewportCount);
		~ViewportRenderManager();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// ViewportManager interactions
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<Vector4f> calculateViewportTiling(ViewportArrangementType ArrangementType, Vector4f ContentArea);
		void loadViewports(const std::vector<Vector4f>& Tiles, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries);
		void resizeActiveViewports(const std::vector<Vector4f>& Tiles);	
		size_t activeViewportCount(void) const;
		// returns ID of Viewport containing the mouse cursor, else returns -1
		int32_t mouseInViewport(Vector2f CursorPosOGL) const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Per viewport interactions
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void updateViewport(size_t VPIndex, float FPSScale);
		void renderViewport(size_t VPIndex, CForge::RenderDevice& RDev);
		Vector2i viewportPosition(size_t VPIndex) const;
		Vector2i viewportSize(size_t VPIndex) const;
		Vector2f viewportCenter(size_t VPIndex) const;
		CForge::VirtualCamera* viewportCam(size_t VPIndex);
		CForge::RenderDevice::Viewport viewportGetRenderDeviceViewport(size_t VPIndex) const;
		void viewportSetCamProjectionMode(size_t VPIndex, ViewportProjectionMode Mode);
		void viewportArcballRotate(size_t VPIndex, Vector2f Start, Vector2f End);
		void acrballRotateAllViewports(size_t VPIndex, Vector2f Start, Vector2f End);
		void viewportSetCamPos(size_t VPIndex, Vector3f Eye);
		void viewportSetCamTarget(size_t VPIndex, Vector3f Target);
		void viewportSetCamZoom(size_t VPIndex, float Zoom);
		void viewportMoveCamPos(size_t VPIndex, Vector3f Movement);
		void viewportMoveCamTarget(size_t VPIndex, Vector3f Movement);
		void viewportZoomCam(size_t VPIndex, float Change);

		ViewportProjectionMode viewportCamProjectionMode(size_t VPIndex) const;
		Matrix4f viewportViewMatrix(size_t VPIndex) const;
		Matrix4f viewportProjectionMatrix(size_t VPIndex) const;
		Vector3f viewportCamPos(size_t VPIndex) const;
		Vector3f viewportCamTarget(size_t VPIndex) const;
		float viewportCamZoom(size_t VPIndex) const;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Dataset interactions: modifying DatasetDisplayData
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void initDatasetModelFromFile(DatasetType DT, std::string Filepath);
		void updateDatasetModel(DatasetType DT/*, new vertex positions here...*/); //TODO
		
		void updateDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData>::iterator itDatasetGeom); //TODO
		void showDatasetDisplayData(size_t VPIndex, DatasetType DT, bool Show);
		void enableWireframeActor(size_t VPIndex, DatasetType DT, bool Show);
		void enablePrimitivesActor(size_t VPIndex, DatasetType DT, bool Show);
		void setSolidColorShading(size_t VPIndex, DatasetType DT, bool PrimitivesActor, Vector3f* Color = nullptr); //TODO: test
		//TODO: requires methods from registration module, DO NOT USE YET
		//void setHausdorffDistColorShading(size_t VPIndex, DatasetType DT, std::vector<float>& HausdorffVals); //TODO: change to support pPrimitivesActor and pWireframeActor; calculateHausdorffVetexColors(...) + testing
		void setDatasetDisplayDataArrangement(size_t VPIndex, DisplayDataArrangementMode Arrangement);
		void arrangeDatasetDisplayDataLayered(size_t VPIndex);
		void arrangeDatasetDisplayDataSideBySide(size_t VPIndex);
		DisplayDataArrangementMode activeDatasetDisplayDataArrangement(size_t VPIndex) const;
		std::vector<DatasetType> activeDatasetTypes(size_t VPIndex) const;
		Matrix4f datasetModelMatrix(size_t VPIndex, DatasetType DT) const;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Vertex marker & correspondence information interactions
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void initMarkerData(std::string Filepath);
		void placeDefaultHoverMarker(DatasetType DT, const Vector3f VertexPos);
		void placeDefaultSelectMarker(DatasetType DT, const Vector3f VertexPos, MarkerColor MC);
		bool showDefaultHoverMarkerOnDataset(size_t VPIndex, DatasetType DT) const;
		void showDefaultHoverMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);
		bool showDefaultSelectMarkerOnDataset(size_t VPIndex, DatasetType DT) const;
		void showDefaultSelectMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);
		void addFeatMarkerTemplate(size_t FeatPointID, const Vector3f MarkerPos);
		void addCorrMarkerTemplate(size_t CorrPointID, const Vector3f MarkerPos);
		void addFeatMarkerTarget(size_t FeatPointID, const Vector3f MarkerPos);
		void addCorrMarkerTarget(size_t CorrPointID, const Vector3f MarkerPos);
		void removeFeatMarkerTemplate(size_t FeatPointID);
		void removeCorrMarkerTemplate(size_t CorrPointID);
		void removeFeatMarkerTarget(size_t FeatPointID);
		void removeCorrMarkerTarget(size_t CorrPointID);
		void clearFeatMarkers(void);
		void clearCorrMarkers(void);
		void updateFeatMarkerPosition(DatasetType DT, size_t FeatPointID, const Vector3f MarkerPos);
		void updateCorrMarkerPosition(DatasetType DT, size_t CorrPointID, const Vector3f MarkerPos);
		bool showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT) const;
		void showFeatMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show);
		bool showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT) const;
		void showCorrMarkersOnDataset(size_t VPIndex, DatasetType DT, bool Show);
		void markerModeFeatMarkerPair(size_t TemplatePointID, size_t TargetPointID, MarkerMode MM);
		void markerModeCorrMarkerPair(size_t TemplatePointID, size_t TargetPointID, MarkerMode MM);
		void markerModeFeatMarkerTemplate(size_t TemplatePointID, MarkerMode MM);
		void markerModeCorrMarkerTemplate(size_t TemplatePointID, MarkerMode MM);
		void markerModeFeatMarkerTarget(size_t TargetPointID, MarkerMode MM);
		void markerModeCorrMarkerTarget(size_t TargetPointID, MarkerMode MM);
		MarkerMode markerModeCorrMarkerTemplate(size_t TemplatePointID) const;
		MarkerMode markerModeCorrMarkerTarget(size_t TemplatePointID) const;
		MarkerMode markerModeFeatMarkerTemplate(size_t TemplatePointID) const;
		MarkerMode markerModeFeatMarkerTarget(size_t TemplatePointID) const;
		void DEBUG_placeMeshMarker(DatasetType DT, const Vector3f Pos);
		void DEBUG_showMeshMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);

	private:
		struct Viewport {
			CForge::RenderDevice::Viewport VP;

			// viewport scene graph
			CForge::SceneGraph SG;
			CForge::SGNTransformation RootSGN;
			DisplayDataArrangementMode DisplayDataArrangement;
			std::map<DatasetType, DatasetDisplayData*> DatasetDisplayData;
		
			// vertex markers
			std::map<DatasetType, bool> ShowDefaultHoverMarker;
			std::map<DatasetType, bool> ShowDefaultSelectMarker;
			std::map<DatasetType, bool> DEBUG_ShowDebugMeshMarker;
			std::map<DatasetType, bool> ShowFeatMarkers;
			std::map<DatasetType, bool> ShowCorrMarkers;

			// Correspondence lines: original template ("T") -> target ("T")
			//CForge::SGNTransformation TTCorrTransSGN;
			//CForge::SGNGeometry TTCorrGeomSGN;

			// Correspondence lines: deformed template ("DT") -> target ("T")
			//CForge::SGNTransformation DTTCorrTransSGN;
			//CForge::SGNGeometry DTTCorrGeomSGN;

			// Viewing controls
			CForge::VirtualCamera Cam;
			ViewportProjectionMode ProjMode;
			Vector3f OrbitEye, OrbitTarget;
			float OrbitZoom;
		};

		int32_t addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries);

		void addDatasetDisplayData(Viewport* pVP, DatasetType DT, DatasetGeometryType GT);
		void clearDatasetDisplayData(size_t VPIndex);
		//TODO: requires methods from registration module, DO NOT USE YET
		//std::vector<Vector3f> calculateHausdorffVertexColors(std::vector<float>& HausdorffVals); //TODO

		Quaternionf arcballRotation(size_t VPIndex, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL);
		Vector3f mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize);

		GlobalViewportState m_GlobalState;

		std::map<MarkerColor, CForge::StaticActor*> m_MarkerActors;
		std::map<DatasetType, CForge::T3DMesh<float>> m_DatasetModels; // Mesh / point cloud data of datasets for all viewports

		std::vector<Viewport*> m_Viewports;
		size_t m_MaxViewportCount;
		ViewportArrangementType m_ActiveViewportArrangement;

		// vertex markers:
		std::map<DatasetType, DatasetMarkerInstance> m_DefaultHoverMarkers;
		std::map<DatasetType, DatasetMarkerInstance> m_DefaultSelectMarkers;
		std::map<DatasetType, DatasetMarkerInstance> m_DEBUG_MeshMarkers;

		// Clouds of markers (here feature point markers, correspondence point markers) per dataset
		std::map<DatasetType, DatasetMarkerCloud> m_FeatMarkerClouds;
		std::map<DatasetType, DatasetMarkerCloud> m_CorrMarkerClouds;
	};
}

#endif