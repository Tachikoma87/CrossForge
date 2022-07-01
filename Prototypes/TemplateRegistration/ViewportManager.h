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
#include <set>
//#include <functional>

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

	enum class MarkerColor {
		DEFAULT_VERTEX_HOVERING,
		DEFAULT_VERTEX_SELECTION,
		FEATURE_IDLE,
		FEATURE_HOVERED,
		FEATURE_SELECTED,
		CORRESPONDENCE_IDLE,
		CORRESPONDENCE_HOVERED,
		CORRESPONDENCE_SELECTED,
		DEBUG
	};

	enum class MarkerMode {
		NONE,
		IDLE,
		HOVERED,
		SELECTED,
		DEBUG_MESH_MARKER
	};

	class ViewportManager {
	public:
		struct GlobalViewportState {
			bool GlobalMarkers; // Toggle whether vertex markers should be displayed across all active DatasetViews or only within the Viewport containing the mouse cursor
			bool DisplayCorrespondences; // Toggle whether pairs of corresponding vertices should be highlighted (with vertex markers)

			Vector3f DefaultTemplateColor;
			Vector3f DefaultTargetColor;

			GlobalViewportState() :
				GlobalMarkers(false), DisplayCorrespondences(false) {}
		};

		ViewportManager(size_t MaxViewportCount);
		~ViewportManager();

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
		void enableWireframeActor(size_t VPIndex, DatasetType DT, bool Active);
		void enablePrimitivesActor(size_t VPIndex, DatasetType DT, bool Active);
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
		void placeDefaultVertexHoverMarker(size_t VPIndex, DatasetType DT, const Vector3f VertexPos);
		void placeDefaultVertSelectMarker(size_t VPIndex, DatasetType DT, const Vector3f VertexPos);
		bool showDefaultVertexHoverMarkerOnDataset(size_t VPIndex, DatasetType DT) const;
		void showDefaultVertexHoverMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);
		bool showDefaultVertSelectMarkerOnDataset(size_t VPIndex, DatasetType DT) const;
		void showDefaultVertSelectMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);
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
		void markerModeFeatMarkerPair(DatasetType DT, size_t TemplatePointID, size_t TargetPointID, MarkerMode MM);
		void markerModeCorrMarkerPair(DatasetType DT, size_t TemplatePointID, size_t TargetPointID, MarkerMode MM);
		void DEBUG_placeMeshMarker(size_t VPIndex, DatasetType DT, const Vector3f Pos);
		void DEBUG_showMeshMarkerOnDataset(size_t VPIndex, DatasetType DT, bool Show);

	private:
		struct MarkerSGData {
			CForge::SGNTransformation TransSGN; //TODO: try again with only GeomSGN to save space! (if it works, remove MarkerSGData struct entirely!)
			CForge::SGNGeometry GeomSGN;

			MarkerSGData() {
				TransSGN.clear();
				GeomSGN.clear();
			}
		};

		struct Viewport {
			CForge::RenderDevice::Viewport VP;

			// viewport scene graph
			CForge::SceneGraph SG;
			CForge::SGNTransformation RootSGN;
			DisplayDataArrangementMode DisplayDataArrangement;
			std::map<DatasetType, DatasetDisplayData*> DatasetDisplayData;
		
			// vertex markers
			std::map<DatasetType, bool> ShowDefaultVertexHoverMarker;
			std::map<DatasetType, bool> ShowDefaultVertexSelectMarker;
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
		std::map<DatasetType, MarkerSGData> m_DefaultVertHoverMarkers;
		std::map<DatasetType, MarkerSGData> m_DefaultVertSelectMarkers;
		std::map<DatasetType, MarkerSGData> m_DEBUG_MeshMarkers;

		// roots for each cloud of "paired markers" (= feature point pairs, correspondence point pairs); per dataset
		// used to connect each cloud of markers to a specific viewport during rendering
		std::map<DatasetType, CForge::SGNTransformation> m_FeatMarkerRoots;
		std::map<DatasetType, CForge::SGNTransformation> m_CorrMarkerRoots;
		
		// lists existing markers per dataset (markers for simple vertex selection, feature points, correspondence points)
		std::map<DatasetType, std::vector<MarkerSGData>> m_FeatMarkers;
		std::map<DatasetType, std::vector<MarkerSGData>> m_CorrMarkers;
		
		// Lookup tables providing assignments: external ID -> internal marker ID for template (deformed template) and target
		// NOTE: Only used for manual creation / removal of correspondence pairs; completely new correspondence configurations 
		// (such as those created by the registration algorithm itself) will ignore these lookup tables and instead overwrite existing markers
		std::map<size_t, size_t> m_TemplateFeatIDLookup, m_TargetFeatIDLookup;
		std::map<size_t, size_t> m_TemplateCorrIDLookup, m_TargetCorrIDLookup;
		
		// lists free slots per vector in m_FeatMarkers and m_CorrMarkers; smallest index first; same free slots for a correspondence / feature point pair across all vectors
		std::set<size_t> m_FreeFeatMarkersTemplate, m_FreeFeatMarkersTarget;
		std::set<size_t> m_FreeCorrMarkersTemplate, m_FreeCorrMarkersTarget;
	};
}

#endif