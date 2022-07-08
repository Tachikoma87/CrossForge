#ifndef __TEMPREG_VIEWPORTINPUTUTMANAGER_H__
#define __TEMPREG_VIEWPORTINPUTUTMANAGER_H__

#include "../../CForge/Graphics/GLWindow.h"

#include "TempRegAppState.h"
#include "GUIManager.h"
#include "ViewportRenderManager.h"
#include "Dataset/DatasetGeometryData.h"
#include "Dataset/CorrespondenceStorage.h"

#include <map>

using namespace Eigen;

namespace TempReg {

	class ViewportInputManager {
	public:
		ViewportInputManager();
		~ViewportInputManager();

		void processInput(
			CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, GUIManager& GUIMgr, ViewportRenderManager& VPRMgr,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

	private:
		struct ManualCorrespondenceData {
			size_t TemplatePointUID;
			size_t TargetPointUID;
			size_t TargetFaceID;
			Vector3f TargetPointPos;
			Vector3f TargetPointBaryCoords;
			bool VertexVertexPair;
			CorrespondenceType Type;
		};

		void processKeyboard(CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, CorrespondenceStorage& CorrStorage);

		void processLMBDown(
			CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage,
			Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard);

		void clearPreviousSelectionHighlighting(const TempRegAppState::PickingResult& PrevSelection, ViewportRenderManager& VPRMgr);

		void lmbCorrPairSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage, DatasetType DT);

		void lmbTemplateVertexCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void lmbTargetSurfPointCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void lmbTargetVertexCorrPointSelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void lmbEmptySelection(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr);

		void processManualCorrespondenceCreation(TempRegAppState::PickingResult& CurrSelectRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, 
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void processMMBDown(CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard);

		void processMMBUp(TempRegAppState& GlobalAppState);

		void processRMBDown(
			CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries,
			CorrespondenceStorage& CorrStorage, Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard);

		void processMouseHover(
			CForge::GLWindow& RenderWin, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage,
			Vector2f CurrentCursorPosOGL, int32_t ViewportUnderMouse, bool CaptureKeyboard);

		void clearPreviousHoverHighlighting(TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr);

		void templateCorrPairHovering(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			DatasetGeometryData& TemplateGeometry, CorrespondenceStorage& CorrStorage);

		void hoverTemplateVertices(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void hoverTargetVertices(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		void hoverTargetPrecisePoints(TempRegAppState::PickingResult& CurrHoverRes, TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, int32_t ViewportUnderMouse,
			std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, CorrespondenceStorage& CorrStorage);

		// raycast against datasets (for picking / highlighting of vertices) if mouse cursor hovers over a viewport
		// only do raycast when mutliple datasets are displayed side by side (NOT layered over each other)
		void raycastDatasets(TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, std::map<DatasetType, DatasetGeometryData>& DatasetGeometries, Vector2f& CursorPosOGL, size_t VPIndex);

		void pickPointOnMesh(DatasetGeometryData& Geometry, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, Vector2f& CursorPosOGL, int& IntersectedFace,
			Vector3f& IntersectionPoint, Vector3f& BaryCoords);

		/*void pickPointInPcl(Vector2f& CursorPosOGL, Vector4f& Viewport, Matrix4f& Model, Matrix4f& View, Matrix4f& Projection, DatasetGeometryData& Geometry, TempRegAppState::RayMeshIntersectionResult& RayPclRes);*/

		int64_t pickVertex(const TempRegAppState::RayIntersectionResult& RayIntersectRes, const DatasetGeometryData& Geometry, float Tolerance);

		/*void pickCorrespondences(const TempRegAppState& GlobalAppState, ViewportRenderManager& VPRMgr, DatasetGeometryData& Geometry, CorrespondenceStorage& CorrStorage, size_t VPIndex);*/

		/*void selectPclDatasetComponent(...);*/

		ManualCorrespondenceData m_ManualCorrData;
		bool m_ManualCorrTemplateReady;
		bool m_ManualCorrTargetReady;
	};
}

#endif