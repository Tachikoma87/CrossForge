#ifndef __TEMPREG_VIEWPORTMANAGER_H__
#define __TEMPREG_VIEWPORTMANAGER_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/VirtualCamera.h"
#include "../../CForge/Graphics/Actors/StaticActor.h"
#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>

#include <map>

#include "MeshDataset.h"
#include "Arcball.h"

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

	enum class DatasetViewProjectionMode : int8_t {
		PERSPECTIVE = 0,
		ORTHO = 1
	};

	enum class DatasetViewControlMode : int8_t {
		GLOBAL = 0, // global view control
		LOCAL = 1 // individual view controls for each Viewport
	};

	enum class MarkerMode : int8_t {
		DISABLED = -1,
		//VERTEX_DEFAULT, TODO? (vertex marker for unselected vertices, to be used as default for all vertices?)
		HOVER = 0,
		ACTIVE_SELECTION,
		CORRESPONDENCE_PAIRS_IDLE
	};

	class ViewportManager {
	public:
		struct MarkerActor {
			Vector3f MarkerColor;
			CForge::StaticActor* pActor;
		};

		struct GlobalViewportState {
			bool GlobalMarkers; // Toggle whether vertex markers should be displayed across all active DatasetViews or only within the Viewport containing the mouse cursor
			bool DisplayCorrespondences; // Toggle whether pairs of corresponding vertices should be highlighted (with vertex markers)
			MarkerMode MarkerDisplayMode;
			std::map<MarkerMode, MarkerActor*> m_MarkerActors; // One actor per MarkerMode

			GlobalViewportState() :
				GlobalMarkers(false), DisplayCorrespondences(false), MarkerDisplayMode(MarkerMode::DISABLED) {}
		};

		ViewportManager(size_t MaxViewportCount);
		~ViewportManager();

		// ViewportManager interactions

		std::vector<Vector4f> calculateViewportTiling(ViewportArrangementType ArrangementType, Vector4f ContentArea);
		void loadViewports(const std::vector<Vector4f>& Tiles);
		void resizeActiveViewports(const std::vector<Vector4f>& Tiles);
		void updateViewport(size_t VPIndex, CForge::VirtualCamera* pCam, float FPSScale);
		void renderViewport(size_t VPIndex, CForge::RenderDevice* pRDev);
		Vector2i viewportPosition(size_t VPIndex) const;
		Vector2i viewportSize(size_t VPIndex) const;
		Vector2f viewportCenter(size_t VPIndex) const;
		size_t activeViewportCount(void) const;
		size_t maxViewportCount(void) const;
		CForge::RenderDevice::Viewport getRenderDeviceViewport(size_t VPIndex) const;
		// returns ID of Viewport containing the mouse cursor, else returns -1
		size_t mouseInViewport(CForge::Mouse* pMouse) const;

		// Viewport viewing interactions

		void setCamProjectionMode(size_t VPIndex, DatasetViewProjectionMode Mode);
		Quaternionf arcballRotate(size_t VPIndex, bool FirstClick, uint32_t CursorX, uint32_t CursorY, const CForge::VirtualCamera* pCam);
		void applyArcballRotation(size_t VPIndex, Quaternionf Rotation);
		void setCamPos(size_t VPIndex, Vector3f Eye);
		void setCamTarget(size_t VPIndex, Vector3f Target);
		void setCamZoom(size_t VPIndex, float Zoom);
		void moveCamPos(size_t VPIndex, Vector3f Movement);
		void moveCamTarget(size_t VPIndex, Vector3f Movement);
		void zoomCam(size_t VPIndex, float Change);

		DatasetViewProjectionMode getCamProjectionMode(size_t VPIndex) const;
		Vector3f getCamPos(size_t VPIndex) const;
		Vector3f getCamTarget(size_t VPIndex) const;
		float getCamZoom(size_t VPIndex) const;

		// MeshDataset interactions

		void addDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, Vector3f Position, Quaternionf Rotation, Vector3f Scale);
		void removeDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset);
		void removeAllDatasets(size_t VPIndex);
		void updateDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset); //TODO
		void showDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, bool Rval); //TODO
		bool datasetActive(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset) const;
		void TESTCameraRotateDataset(size_t VPIndex, std::map<DatasetType, MeshDataset>::iterator itDataset, Quaternionf CamRot);

		// Vertex marker interactions

		void initVertexMarkers(std::string Filepath);
		void markerMode(MarkerMode Mode);
		void displayCorrespondences(bool Rval);

	private:
		struct DatasetDisplayData {
			CForge::SGNTransformation DatasetTransSGN;
			CForge::SGNGeometry DatasetGeomSGN;
			CForge::StaticActor* pDatasetActor;
			CForge::SGNTransformation VertexMarkerTransSGN; // global vertex marker transformation
			std::vector<CForge::SGNGeometry> MarkerGeomSGNs; // individual vertex marker instances
		};

		struct Viewport {
			CForge::RenderDevice::Viewport VP;

			// Viewport scene graph
			CForge::SceneGraph SG;
			CForge::SGNTransformation RootSGN;
			std::map<DatasetType, DatasetDisplayData*> ActiveDisplayData;
		
			// Correspondence lines: original template ("T") -> target ("T")
			//CForge::SGNTransformation TTCorrTransSGN;
			//CForge::SGNGeometry TTCorrGeomSGN;

			// Correspondence lines: deformed template ("DT") -> target ("T")
			//CForge::SGNTransformation DTTCorrTransSGN;
			//CForge::SGNGeometry DTTCorrGeomSGN;

			// Viewing controls
			DatasetViewProjectionMode ProjMode;
			Arcball ArcballControls;
			Vector3f OrbitEye, OrbitTarget;
			float OrbitZoom;
		};

		struct ViewTile {
			int32_t LinkedViewport;
			Vector2f Position; // in floats, to accommodate for GUI
			Vector2f Size;
		};

		struct ViewTileArrangement {
			std::vector<ViewTile> Tiles;
		};

		int32_t addViewport(int VPPositionX, int VPPositionY, int VPSizeX, int VPSizeY);
		void removeViewport(size_t VPIndex);

		CForge::T3DMesh<float> m_MarkerModelData; // Mesh data for all markers				
		GlobalViewportState m_GlobalState;

		std::vector<Viewport*> m_Viewports;
		size_t m_MaxViewportCount;
		ViewportArrangementType m_ActiveViewportArrangement;
	};
}

#endif
