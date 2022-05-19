#ifndef __TEMPREG_GUIMANAGER_H__
#define __TEMPREG_GUIMANAGER_H__

#include <cstdint>
#include <vector>
#include <map>

#include <imgui.h>

#include "ViewportManager.h"

namespace TempReg {

	class GUIManager {
	public:
		GUIManager();
		~GUIManager();

		void init(void* pHandle, uint32_t RenderWinWidth, uint32_t RenderWinHeight);
		void buildNextImGuiFrame(class TempRegAppState* pGlobalAppState, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* DatasetGeometries); //TODO
		void renderImGuiFrame(const bool ClearBuffer);
		void setRenderWinSize(uint32_t RenderWinWidth, uint32_t RenderWinHeight);

	private:
		struct VPArrangementData {
			std::string ArrangementName;
			ViewportArrangementType ArrangementType;
		};

		struct SideMenuState {
			bool Unfolded;
			bool DisplayStateChanged;
		};

		struct ViewportConfigState {
			bool OpenPredefConfigMenu;
			bool LoadConfiguration;
			int VPCountSelected;
			int ActiveVPCount;
			int PrevActiveVPCount;
			int VPArrTypeSelected;
			int ActiveVPArrType;
			std::map<int, std::vector<VPArrangementData>> VPCountToArrangementData;
		};

		struct ViewportOverlayState {
			size_t ViewportIdx;
			ImVec4 ContentArea; // Dear ImGui: origin (ContentArea(0), ContentArea(1)) upper left corner
			bool ViewportSceneEmpty;
			bool ShowTemplate;
			bool ShowDTemplate;
			bool ShowTarget;
		};

		void buildMainMenuBar(ViewportManager* pVPMgr); //TODO
		void buildSideMenu(ViewportManager* pVPMgr); //TODO
		void buildViewportSection(ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries); //TODO
		void buildViewportConfigPredefined(ViewportManager* pVPMgr); //TODO
		void loadViewportOverlayStates(const std::vector<Vector4f>& Tiles);
		void resizeActiveViewportOverlays(const std::vector<Vector4f>& Tiles);
		void buildViewportOverlays(ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries);
		void buildNoViewportsInfo(ImVec2 PosCenter);
		void buildViewportEmptyInfo(size_t ViewportIdx, ImVec2 PosCenter);
		void buildViewportOverlayButtonPanel(size_t VPOverlayIdx, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries); //TODO
		
		// Host window data

		uint32_t m_RenderWinSize[2];
		
		//Main menu bar section
		
		float m_MainMenuBarHeight;
		
		// Side menu section

		ImVec2 m_SideMenuSize;
		SideMenuState m_SideMenuState;

		// Viewport section

		Vector4f m_ViewportSectionArea; // bounding box around entire section reserved for viewports displaying datasets; in OpenGL coordinates (origin in lower left corner)
		ViewportConfigState m_VPConfigState;
		std::vector<ViewportOverlayState> m_VPOStates; // states per viewport overlay
	};
}

#endif
