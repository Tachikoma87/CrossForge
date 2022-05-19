#ifndef __TEMPREG_VIEWPORTMANAGER_H__
#define __TEMPREG_VIEWPORTMANAGER_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/VirtualCamera.h"
#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>

#include <map>
#include <queue>
#include <functional>

#include "Dataset.h"

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

	enum class ViewportProjectionMode : int8_t {
		PERSPECTIVE = 0,
		ORTHO = 1
	};

	enum class ViewportControlMode : int8_t {
		GLOBAL = 0, // global view control
		LOCAL = 1 // individual view controls for each Viewport
	};

	enum class DisplayDataArrangementMode : int8_t {
		LAYERED,
		SIDE_BY_SIDE
	};

	enum class MarkerMode : int8_t {
		DISABLED = -1,
		DEFAULT_HOVER = 0,
		DEFAULT_SELECTION,
		CORRESPONDENCE_PAIRS_IDLE, 
		CORRESPONDENCE_PAIRS_SELECTION
	};

	class ViewportManager {
	public:
		struct GlobalViewportState {
			bool GlobalMarkers; // Toggle whether vertex markers should be displayed across all active DatasetViews or only within the Viewport containing the mouse cursor
			bool DisplayCorrespondences; // Toggle whether pairs of corresponding vertices should be highlighted (with vertex markers)
			
			MarkerMode MarkerDisplayMode;

			Vector3f DefaultTemplateColor;
			Vector3f DefaultTargetColor;

			GlobalViewportState() :
				GlobalMarkers(false), DisplayCorrespondences(false), MarkerDisplayMode(MarkerMode::DISABLED) {}
		};

		ViewportManager(size_t MaxViewportCount);
		~ViewportManager();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// ViewportManager interactions
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<Vector4f> calculateViewportTiling(ViewportArrangementType ArrangementType, Vector4f ContentArea);
		void loadViewports(const std::vector<Vector4f>& Tiles);
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
		void renderViewport(size_t VPIndex, CForge::RenderDevice* pRDev);
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
		void updateDatasetModel(DatasetType DT/*, new vertices here...*/); //TODO
		void addDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom); //TODO
		void removeDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom);
		void clearDatasetDisplayData(size_t VPIndex);
		void updateDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom); //TODO
		void showDatasetDisplayData(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, bool Show);
		void setSolidShading(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom); //TODO: test
		void setHausdorffShading(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, std::vector<Vector3f>& VertexColors); //TODO: test
		void renderMode(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, DatasetRenderMode RM); //TODO: test
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

		void initVertexMarkerModels(std::string Filepath);
		void placeSimpleMarker(size_t VPIndex, std::map<DatasetType, DatasetGeometryData*>::iterator itDatasetGeom, MarkerMode MM, uint32_t VertexID);
		void hideSimpleMarker(size_t VPIndex, MarkerMode MM, bool Hide);
		void removeSimpleMarker(size_t VPIndex, MarkerMode MM);
		void removeCorrespondenceMarkerGroup(int64_t CorrMarkerGroup);
		int64_t addCorrespondenceMarkerGroupTemplate(std::map<DatasetType, DatasetGeometryData*>::iterator itTemplateGeom, uint32_t VertexID, int64_t CorrMarkerGroup = (int64_t)-1);
		int64_t addCorrespondenceMarkerGroupTarget(std::map<DatasetType, DatasetGeometryData*>::iterator itTargetGeom, uint32_t VertexID, int64_t CorrMarkerGroup = (int64_t)-1);
		void removeCorrespondenceMarkerSGData(size_t VPIndex, int64_t CorrMarkerGroup); //TODO
		void loadCorrespondenceMarkers(size_t VPIndex); //TODO
		void displayCorrespondenceMarkers(size_t VPIndex, bool Show); //TODO

	private:
		struct MarkerSGData {
			CForge::SGNTransformation TransSGN;
			CForge::SGNGeometry GeomSGN;

			MarkerSGData() {
				TransSGN.clear();
				GeomSGN.clear();
			}
		};

		struct SimpleMarker {
			DatasetType DT;
			uint32_t VertexID;
			MarkerSGData SGData;

			SimpleMarker() : DT(DatasetType::NONE), VertexID(0) {
				SGData.TransSGN.clear();
				SGData.GeomSGN.clear();
			}
		};

		struct CorrespondenceMarkerSGData {
			MarkerSGData TemplateSGData;
			MarkerSGData DTemplateSGData;
			MarkerSGData TargetSGData;

			CorrespondenceMarkerSGData() {
				TemplateSGData.TransSGN.clear();
				TemplateSGData.GeomSGN.clear();
				DTemplateSGData.TransSGN.clear();
				DTemplateSGData.GeomSGN.clear();
				TargetSGData.TransSGN.clear();
				TargetSGData.GeomSGN.clear();
			}
		};

		struct CorrespondenceMarkerGroup {
			MarkerMode MM;
			uint32_t TemplateVertexID, TargetVertexID;
			Vector3f TargetMarkerPos;
			bool ReadyForUse;

			CorrespondenceMarkerGroup() : 
				MM(MarkerMode::CORRESPONDENCE_PAIRS_SELECTION), TemplateVertexID(0), TargetVertexID(0), 
				TargetMarkerPos(Vector3f::Zero()), ReadyForUse(false) {

			}
		};

		struct Viewport {
			CForge::RenderDevice::Viewport VP;

			// viewport scene graph
			CForge::SceneGraph SG;
			CForge::SGNTransformation RootSGN;
			DisplayDataArrangementMode DisplayDataArrangement;
			std::map<DatasetType, DatasetDisplayData*> ActiveDisplayData;
		
			// vertex markers
			//SimpleMarker DefaultHoverMarker;
			//SimpleMarker DefaultSelectionMarker;
			std::map<MarkerMode, SimpleMarker> DefaultMarkers;
			std::vector<CorrespondenceMarkerSGData> CorrMarkerSGData;
			std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> FreeCorrMarkers;
			std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> NewCorrMarkerGroups;
			
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

		int32_t addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY);
		void removeViewport(size_t VPIndex);

		Quaternionf arcballRotation(size_t VPIndex, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL);
		Vector3f mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize);

		GlobalViewportState m_GlobalState;

		std::map<MarkerMode, CForge::StaticActor*> m_MarkerActors; // Mesh data for each marker
		std::map<DatasetType, CForge::T3DMesh<float>> m_DatasetModels; // Mesh / point cloud data of datasets for all viewports

		std::vector<Viewport*> m_Viewports;
		size_t m_MaxViewportCount;
		ViewportArrangementType m_ActiveViewportArrangement;

		std::vector<CorrespondenceMarkerGroup> m_CorrespondenceMarkerGroups;
		std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> m_FreeCorrMarkerGroups;
	};
}

#endif