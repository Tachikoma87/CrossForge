#ifndef __TEMPREG_TEMPREGAPPSTATE_H__
#define __TEMPREG_TEMPREGAPPSTATE_H__

#include "../../CForge/Input/Mouse.h"

#include <Eigen/Eigen>

#include <set>

#include "Dataset/DatasetCommon.h"
#include "Dataset/CorrespondenceStorage.h"

using namespace Eigen;

namespace TempReg {
	
	enum class ViewportInteractionMode {
		VIEWING,
		FEATURE_EDITING_VERTVERT,
		FEATURE_EDITING_VERTSURF,
		CORRESPONDENCE_EDITING_VERTVERT,
		CORRESPONDENCE_EDITING_VERTSURF
	};

	enum class PickingResultType {
		NONE,
		MESH_VERTEX,
		MESH_SURFACE_POINT,
		PCL_POINT,
		FEATURE_POINT_VERTEX,
		FEATURE_POINT_SURFACE,
		FEATURE_PAIR,
		CORRESPONDENCE_POINT_VERTEX,
		CORRESPONDENCE_POINT_SURFACE,
		CORRESPONDENCE_PAIR
	};
	
	class TempRegAppState {
	public:
		struct RayIntersectionResult {
			DatasetType Dataset;
			DatasetGeometryType GeometryType;
			int Face;
			int Vertex;
			Vector3f IntersectionPos;
			Vector3f BarycentricCoords;
			
			RayIntersectionResult() :
				Dataset(DatasetType::NONE), GeometryType(DatasetGeometryType::MESH), Face(-1), Vertex(-1), IntersectionPos(Vector3f::Zero()), BarycentricCoords(Vector3f::Zero()) {}
		};

		struct PickingResult {
			size_t ViewportID;
			PickingResultType ResultType;
			DatasetType Dataset;
			//size_t Vertex;
			size_t TemplatePointID;
			size_t TargetPointID;
			size_t Face;
			Vector3f SurfacePoint;
			//std::vector<size_t> Feats;
			//std::vector<size_t> Corrs;

			PickingResult() : 
				ViewportID(0), ResultType(PickingResultType::NONE), Dataset(DatasetType::NONE), TemplatePointID(0), TargetPointID(0), Face(0), SurfacePoint(Vector3f::Zero()) {}
		};

		TempRegAppState(void);
		~TempRegAppState();

		void queueScreenshot(bool rVal);		
		void lockCursor(void* pHandle);
		void unlockCursor(void* pHandle);
		void prevMouseButtonState(CForge::Mouse::Button Btn, bool Down);
		void viewportInteractionMode(ViewportInteractionMode Mode);
		void mouseButtonViewportFocus(int32_t VPIndex);
		void currMMBCursorPos(Vector2f Pos);
		void prevMMBCursorPos(Vector2f Pos);
		void rayIntersectionResult(const RayIntersectionResult Res);
		//void rayIntersectionResult(DatasetType DT, DatasetGeometryType GT, int IntersectedFace, int IntersectedPoint, Vector3f IntersectionPos, Vector3f BarycentricCoords);
		void clearRayIntersectionResult(void);
		void prevSelectRes(PickingResult PickRes);
		void prevHoverRes(PickingResult PickRes);

		bool screenshotQueued(void) const;
		bool mouseCapturedByGui(void) const;
		bool keyboardCapturedByGui(void) const;
		bool cursorLocked(void) const;
		bool prevMouseButtonState(CForge::Mouse::Button Btn) const;
		ViewportInteractionMode viewportInteractionMode(void) const;
		int32_t mouseButtonViewportFocus(void) const;
		Vector2f currMMBCursorPos(void) const;
		Vector2f prevMMBCursorPos(void) const;
		const RayIntersectionResult& rayIntersectionResult(void) const;
		const PickingResult& prevSelectRes(void) const;
		const PickingResult& prevHoverRes(void) const;
				
	private:
		bool m_GuiCaptureMouse, m_GuiCaptureKeyboard;
		bool m_TakeScreenshot;
		bool m_glfwCursorLock;
		bool m_MouseBtnOldState[3]; // LMB == [0], RMB == [1], MMB == [2]
		Vector2f m_CursorPosMMB, m_OldCursorPosMMB;
		ViewportInteractionMode m_VPInteractionMode;
		int32_t m_MouseBtnViewportFocus;
		RayIntersectionResult m_RayIntersectRes;
		PickingResult m_SelectRes, m_OldSelectRes;
		PickingResult m_HoverRes, m_OldHoverRes;
		bool m_DisplayCorrespondences;
		bool m_DisplayFeatures;
	};
}

#endif