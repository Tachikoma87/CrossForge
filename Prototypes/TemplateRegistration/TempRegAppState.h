#ifndef __TEMPREG_TEMPREGAPPSTATE_H__
#define __TEMPREG_TEMPREGAPPSTATE_H__

#include "../../CForge/Input/Mouse.h"

#include <Eigen/Eigen>

#include <set>

#include "Dataset.h"

using namespace Eigen;

namespace TempReg {
	
	enum class ViewportInteractionMode {
		VIEWING,
		FEATURE_EDITING,
		CORRESPONDENCE_EDITING
	};

	struct RayMeshIntersectionResult {
		DatasetType IntersectedDataset;
		Vector3f IntersectionPos;
		Vector3f BarycentricCoords;
		int IntersectedFace;

		RayMeshIntersectionResult() : 
			IntersectedDataset(DatasetType::NONE), IntersectionPos(Vector3f::Zero()), BarycentricCoords(Vector3f::Zero()), IntersectedFace(-1) {}
	};

	struct RayPclIntersectionResult {
		DatasetType IntersectedDataset;
		int IntersectedPoint;

		RayPclIntersectionResult() :
			IntersectedDataset(DatasetType::NONE), IntersectedPoint(-1) {}
	};

	enum class PickingResultType {
		NONE,
		MESH_VERTEX,
		PCL_POINT,
		FEATURE_POINT,
		CORRESPONDENCE_POINT
	};

	struct PickingResult {
		size_t ViewportID;
		PickingResultType ResultType;
		DatasetType PickedDataset;
		uint32_t PickedVertex;
		uint32_t PickedFeaturePoint;
		uint32_t PickedCorrPoint;
		
		PickingResult() :
			ViewportID(0), PickedDataset(DatasetType::NONE), ResultType(PickingResultType::NONE),
			PickedVertex(0), PickedFeaturePoint(0), PickedCorrPoint(0) {}
	};

	class TempRegAppState {
	public:
		TempRegAppState(void);
		~TempRegAppState();

		void queueScreenshot(bool rVal);		
		void lockCursor(void* pHandle);
		void unlockCursor(void* pHandle);
		void oldMouseButtonState(CForge::Mouse::Button Btn, bool Down);
		void mouseButtonViewportFocus(int32_t VPIndex);
		void currentMMBCursorPos(Vector2f Pos);
		void oldMMBCursorPos(Vector2f Pos);
		void currentPickResClick(PickingResult PickRes);
		void oldPickResClick(PickingResult PickRes);
		void currentPickResHover(PickingResult PickRes);
		void oldPickResHover(PickingResult PickRes);
		void manualCorrespondenceTemplate(size_t VertexID, CorrespondenceType CT);
		void manualCorrespondenceTarget(int64_t VertexID);
		void manualCorrespondenceTarget(Vector3f Position);
		void clearManualCorrespondence(void);

		bool screenshotQueued(void) const;
		bool mouseCapturedByGui(void) const;
		bool keyboardCapturedByGui(void) const;
		bool cursorLocked(void) const;
		bool oldMouseButtonState(CForge::Mouse::Button Btn) const;
		int32_t mouseButtonViewportFocus(void) const;
		Vector2f currentMMBCursorPos(void) const;
		Vector2f oldMMBCursorPos(void) const;
		PickingResult currentPickResClick(void) const;
		PickingResult oldPickResClick(void) const;
		PickingResult currentPickResHover(void) const;
		PickingResult oldPickResHover(void) const;
		bool manualCorrespondenceReady(void) const;
		CorrespondencePair manualCorrespondence(void) const;
				
	private:
		bool m_GuiCaptureMouse, m_GuiCaptureKeyboard;
		bool m_TakeScreenshot;
		bool m_glfwCursorLock;
		bool m_MouseBtnOldState[3]; // LMB == [0], RMB == [1], MMB == [2]
		Vector2f m_CursorPosMMB, m_OldCursorPosMMB;
		int32_t m_MouseBtnViewportFocus;
		PickingResult m_PickResClick, m_OldPickResClick;
		PickingResult m_PickResHover, m_OldPickResHover;
		bool m_ManualCorrTemplateReady, m_ManualCorrTargetReady;
		CorrespondencePair m_ManualCorrespondence;
	};
}

#endif