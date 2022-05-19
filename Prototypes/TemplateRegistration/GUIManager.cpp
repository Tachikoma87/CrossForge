#include "GUIManager.h"

#include <glad/glad.h>

#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "TempRegAppState.h"

namespace TempReg {

	GUIManager::GUIManager() :
		m_MainMenuBarHeight(-1.0f), m_RenderWinSize{0, 0}, m_SideMenuSize(ImVec2(0.0f, 0.0f)) {
		
	}

	GUIManager::~GUIManager() {

	}

	void GUIManager::init(void* pHandle, uint32_t RenderWinWidth, uint32_t RenderWinHeight) {
		gladLoadGL();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGui::StyleColorsLight();
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)pHandle, true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		setRenderWinSize(RenderWinWidth, RenderWinHeight);

		m_SideMenuState.Unfolded = true;
		m_SideMenuState.DisplayStateChanged = false;
		m_SideMenuSize.x = (float)m_RenderWinSize[0] / 5.0f;

		m_VPConfigState.OpenPredefConfigMenu = false;
		m_VPConfigState.LoadConfiguration = false;
		m_VPConfigState.ActiveVPCount = -1;
		m_VPConfigState.PrevActiveVPCount = 0;
		m_VPConfigState.ActiveVPArrType = -1;
		m_VPConfigState.VPCountSelected = -1;
		m_VPConfigState.VPArrTypeSelected = 0;

		// temporary hack - hardcoded values
		// -> emplace keys
		m_VPConfigState.VPCountToArrangementData.insert(std::pair<int, std::vector<VPArrangementData>>(1, std::vector<VPArrangementData>()));
		m_VPConfigState.VPCountToArrangementData.insert(std::pair<int, std::vector<VPArrangementData>>(2, std::vector<VPArrangementData>()));
		m_VPConfigState.VPCountToArrangementData.insert(std::pair<int, std::vector<VPArrangementData>>(3, std::vector<VPArrangementData>()));
		m_VPConfigState.VPCountToArrangementData.insert(std::pair<int, std::vector<VPArrangementData>>(4, std::vector<VPArrangementData>()));

		// -> fill values
		auto* val = &(m_VPConfigState.VPCountToArrangementData.at(1));
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "fullscreen";
		val->back().ArrangementType = ViewportArrangementType::ONE_FULLSCREEN;

		val = &(m_VPConfigState.VPCountToArrangementData.at(2));
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "horizontal columns";
		val->back().ArrangementType = ViewportArrangementType::TWO_COLUMNS;

		val = &(m_VPConfigState.VPCountToArrangementData.at(3));
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "large viewport at top";
		val->back().ArrangementType = ViewportArrangementType::THREE_BIGTOP;
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "large viewport at bottom";
		val->back().ArrangementType = ViewportArrangementType::THREE_BIGBOTTOM;
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "large viewport on left";
		val->back().ArrangementType = ViewportArrangementType::THREE_BIGLEFT;
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "large viewport on right";
		val->back().ArrangementType = ViewportArrangementType::THREE_BIGRIGHT;
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "horizontal columns";
		val->back().ArrangementType = ViewportArrangementType::THREE_COLUMNS;
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "evenly tiled";
		val->back().ArrangementType = ViewportArrangementType::THREE_EVEN;

		val = &(m_VPConfigState.VPCountToArrangementData.at(4));
		val->push_back(VPArrangementData());
		val->back().ArrangementName = "evenly tiled";
		val->back().ArrangementType = ViewportArrangementType::FOUR_EVEN;
	}//init

	void GUIManager::buildNextImGuiFrame(TempRegAppState* pGlobalAppState, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// build gui windows & widgets based on GlobalAppState + ViewportManager...
		m_VPConfigState.OpenPredefConfigMenu = false;

		buildMainMenuBar(pVPMgr);
		
		buildViewportConfigPredefined(pVPMgr);

		buildSideMenu(pVPMgr);
		
		buildViewportSection(pVPMgr, pDatasetGeometries);
	}//buildNextImGuiFrame

	void GUIManager::renderImGuiFrame(const bool ClearBuffer) {
		ImGui::Render();
		glViewport(0, 0, m_RenderWinSize[0], m_RenderWinSize[1]);
		if (ClearBuffer) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}//renderImGuiFrame

	void GUIManager::setRenderWinSize(uint32_t RenderWinWidth, uint32_t RenderWinHeight) {
		m_RenderWinSize[0] = RenderWinWidth;
		m_RenderWinSize[1] = RenderWinHeight;
	}//setRenderWinSize

	void GUIManager::buildMainMenuBar(ViewportManager* pVPMgr) {//TODO
		if (ImGui::BeginMainMenuBar()) {
			
			if (ImGui::BeginMenu("File")) {
				
				//TODO

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Views")) {

				if (ImGui::MenuItem("Predefined viewport arrangement..."))
					m_VPConfigState.OpenPredefConfigMenu = true;

				ImGui::EndMenu();
			}
			m_MainMenuBarHeight = ImGui::GetFrameHeight();
			ImGui::EndMainMenuBar();
		}
	}//buildMainMenuBar

	void GUIManager::buildSideMenu(ViewportManager* pVPMgr) {//TODO
		
		m_SideMenuSize.y = (float)m_RenderWinSize[1] - m_MainMenuBarHeight;

		// build fold/unfold arrow button
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));

		ImGuiWindowFlags ArrowBtnWindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration;

		if (m_SideMenuState.Unfolded) {
			ImVec2 Pos((float)m_RenderWinSize[0] - m_SideMenuSize.x - ImGui::GetFrameHeight(), m_MainMenuBarHeight);
			ImGui::SetNextWindowPos(Pos);

			ImGui::Begin("##arrowbtn_window_unfolded", nullptr, ArrowBtnWindowFlags);

			if (ImGui::ArrowButton("##fold", ImGuiDir_Right)) {
				m_SideMenuState.Unfolded = false;
				m_SideMenuState.DisplayStateChanged = true;
				m_SideMenuSize.x = 0.0f;
			}

			ImGui::End();
		}
		else {
			ImVec2 Pos((float)m_RenderWinSize[0] - ImGui::GetFrameHeight(), m_MainMenuBarHeight);
			ImGui::SetNextWindowPos(Pos);

			ImGui::Begin("##arrowbtn_window_folded", nullptr, ArrowBtnWindowFlags);

			if (ImGui::ArrowButton("##unfold", ImGuiDir_Left)) {
				m_SideMenuState.Unfolded = true;
				m_SideMenuState.DisplayStateChanged = true;
				m_SideMenuSize.x = (float)m_RenderWinSize[0] / 5.0f;
			}

			ImGui::End();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		// build side menu
		if (m_SideMenuState.Unfolded) {

			ImVec2 Pos((float)m_RenderWinSize[0] - m_SideMenuSize.x, m_MainMenuBarHeight);

			ImGui::SetNextWindowPos(Pos);
			ImGui::SetNextWindowSize(m_SideMenuSize);

			ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
			ImGui::Begin("##sidemenu_window", nullptr, WindowFlags);

			//TODO: window contents here....

			ImGui::End();
		}		
	}//buildSideMenu

	void GUIManager::buildViewportSection(ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries) {

		m_ViewportSectionArea(0) = 0.0f;
		m_ViewportSectionArea(1) = 0.0f;
		m_ViewportSectionArea(2) = m_RenderWinSize[0];
		m_ViewportSectionArea(3) = m_RenderWinSize[1] - m_MainMenuBarHeight;

		if (m_SideMenuState.Unfolded) 
			m_ViewportSectionArea(2) -= m_SideMenuSize.x;

		if (m_VPConfigState.ActiveVPCount == -1 && m_VPConfigState.ActiveVPArrType == -1) {
			ImVec2 PosCenter(
				m_ViewportSectionArea(0) + (m_ViewportSectionArea(2) / 2.0f),
				m_ViewportSectionArea(1) + (m_ViewportSectionArea(3) / 2.0f));

			buildNoViewportsInfo(PosCenter);

			return;
		}

		if (m_VPConfigState.LoadConfiguration) {
			int VPCount = m_VPConfigState.ActiveVPCount;
			int PrevVPCount = m_VPConfigState.PrevActiveVPCount;
			int ArrTypeIdx = m_VPConfigState.ActiveVPArrType;
			auto ArrangementType = m_VPConfigState.VPCountToArrangementData.at(VPCount)[ArrTypeIdx].ArrangementType;

			const auto Tiles = pVPMgr->calculateViewportTiling(ArrangementType, m_ViewportSectionArea);
			pVPMgr->loadViewports(Tiles);
			loadViewportOverlayStates(Tiles);
		}

		if (m_VPConfigState.ActiveVPCount != -1 && m_VPConfigState.ActiveVPArrType != -1) {
			
			if (m_SideMenuState.DisplayStateChanged) {		
				// update dimensions of viewport section + individual viewports
				int VPCount = m_VPConfigState.ActiveVPCount;
				int ArrTypeIdx = m_VPConfigState.ActiveVPArrType;
				auto ArrangementType = m_VPConfigState.VPCountToArrangementData.at(VPCount)[ArrTypeIdx].ArrangementType;

				auto Tiles = pVPMgr->calculateViewportTiling(ArrangementType, m_ViewportSectionArea);
				pVPMgr->resizeActiveViewports(Tiles);
				resizeActiveViewportOverlays(Tiles);
			}

			buildViewportOverlays(pVPMgr, pDatasetGeometries);
		}

		m_VPConfigState.LoadConfiguration = false;
		m_SideMenuState.DisplayStateChanged = false;
	}//buildViewportSection

	void GUIManager::buildViewportConfigPredefined(ViewportManager* pVPMgr) {//TODO
		
		if (m_VPConfigState.OpenPredefConfigMenu)
			ImGui::OpenPopup("Choose a predefined viewport arrangement");

		// center popup on screen (only with first appearance of window; once created the popup can be moved)
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Choose a predefined viewport arrangement", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

			if (ImGui::BeginTable("predefconfig", 2, ImGuiTableFlags_BordersInnerV)) {

				float ComboBoxWidth = 50.0f;
				std::string ComboBoxLabel = "Number of viewports";
				float ComboBoxWidgetWidth = ComboBoxWidth + 40.0f + ImGui::GetStyle().ItemInnerSpacing.x + ImGui::CalcTextSize(ComboBoxLabel.c_str()).x;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, ComboBoxWidgetWidth * 1.25f);
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

				//left column
				ImGui::TableNextColumn();

				// ComboBox: "Number of viewports"
				
				ImGui::PushItemWidth(ComboBoxWidth);

				std::string PreviewVal = (m_VPConfigState.VPCountSelected == -1) ? "..." : std::to_string(m_VPConfigState.VPCountSelected);
				if (ImGui::BeginCombo(ComboBoxLabel.c_str(), PreviewVal.c_str())) {

					for (int i = 1; i <= 4; ++i) {
						ImGui::PushID(i);
						std::string Label = std::to_string(i);
						const bool Selected = (m_VPConfigState.VPCountSelected == i);
						if (ImGui::Selectable(Label.c_str(), Selected)) m_VPConfigState.VPCountSelected = i;
						if (Selected) ImGui::SetItemDefaultFocus();
						ImGui::PopID();
					}
					ImGui::EndCombo();
				}

				ImGui::PopItemWidth();

				ImGui::NewLine();

				// Listbox: "Arrangement types"
				ImGui::Text("Arrangement types:");
				ImGui::PushItemWidth(ComboBoxWidgetWidth * 1.2f);
				if (ImGui::BeginListBox("##vp_arrangement_types")) {

					if (m_VPConfigState.VPCountSelected != -1) {

						const auto& Arrangements = m_VPConfigState.VPCountToArrangementData.at(m_VPConfigState.VPCountSelected);

						for (int i = 0; i < Arrangements.size(); ++i) {
							ImGui::PushID(i);
							const bool Selected = (m_VPConfigState.VPArrTypeSelected == i);
							if (ImGui::Selectable(Arrangements[i].ArrangementName.c_str(), Selected)) m_VPConfigState.VPArrTypeSelected = i;
							if (Selected) ImGui::SetItemDefaultFocus();
							ImGui::PopID();
						}
					}					
					ImGui::EndListBox();
				}

				ImGui::PopItemWidth();

				//right column
				ImGui::TableNextColumn();

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ComboBoxWidgetWidth * 0.05f);

				ImGui::Text("Preview:");

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ComboBoxWidgetWidth * 0.05f);

				if (ImGui::BeginChild("##previewpane", ImVec2(400, 225), true)) {
					
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y / 2.0f) - (ImGui::GetTextLineHeight() / 2.0f));
					ImGui::Text("TODO..."); //TODO: show preview of chosen viewport arrangement

					ImGui::EndChild();
				}
				ImGui::EndTable();
			}

			ImGui::NewLine();

			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - 240);
			
			if (ImGui::Button("Apply", ImVec2(120, 0))) {
				
				// apply selected configuration
				m_VPConfigState.LoadConfiguration = true;
				m_VPConfigState.PrevActiveVPCount = (m_VPConfigState.ActiveVPCount == -1) ? 0 : m_VPConfigState.ActiveVPCount;
				m_VPConfigState.ActiveVPCount = m_VPConfigState.VPCountSelected;
				m_VPConfigState.ActiveVPArrType = m_VPConfigState.VPArrTypeSelected;
				m_VPConfigState.VPArrTypeSelected = 0;

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0))) {
				
				// discard any chosen settings when cancelling out of the config menu
				m_VPConfigState.LoadConfiguration = false;
				m_VPConfigState.VPCountSelected = -1;
				m_VPConfigState.VPArrTypeSelected = 0;

				ImGui::CloseCurrentPopup();
			}

			ImGui::SetItemDefaultFocus();

			ImGui::EndPopup();
		}
	}//buildViewportConfigPredefined

	void GUIManager::loadViewportOverlayStates(const std::vector<Vector4f>& Tiles) {

		size_t i = 0;

		// reuse existing viewport overlay states
		while (i < std::min(Tiles.size(), m_VPOStates.size())) {
			
			Vector4f Tile = Tiles[i];
			
			m_VPOStates[i].ViewportIdx = i;
			
			// convert viewport content area boundaries from OpenGL coordinate space (origin in lower left corner of screen)
			// to Dear ImGui coordinate space (origin in upper left corner of screen)
			// x-axis stays the same, y-axis flips over

			float ContentAreaPadding = 10.0f;
			float ImGuiContentAreaOriginX = Tile(0) + ContentAreaPadding;
			float ImGuiContentAreaOriginY = m_RenderWinSize[1] - Tile(1) - Tile(3) + ContentAreaPadding;
			float ImGuiContentAreaSizeX = Tile(2) - (ContentAreaPadding * 2.0f) - ImGui::GetFrameHeight(); // - ImGui::GetFrameHeight() to make room for ArrowButton of SideMenu
			float ImGuiContentAreaSizeY = Tile(3) - (ContentAreaPadding * 2.0f);

			m_VPOStates[i].ContentArea = ImVec4(ImGuiContentAreaOriginX, ImGuiContentAreaOriginY, ImGuiContentAreaSizeX, ImGuiContentAreaSizeY);

			++i;
		}

		// add missing viewport overlay states
		while (i < Tiles.size()) {
			
			Vector4f Tile = Tiles[i];

			m_VPOStates.push_back(ViewportOverlayState());
			m_VPOStates[i].ViewportIdx = i;

			// convert viewport content area boundaries from OpenGL coordinate space (origin in lower left corner of screen)
			// to Dear ImGui coordinate space (origin in upper left corner of screen)
			// x-axis stays the same, y-axis flips over

			float ContentAreaPadding = 10.0f;
			float ImGuiContentAreaOriginX = Tile(0) + ContentAreaPadding;
			float ImGuiContentAreaOriginY = m_RenderWinSize[1] - Tile(1) - Tile(3) + ContentAreaPadding;
			float ImGuiContentAreaSizeX = Tile(2) - (ContentAreaPadding * 2.0f) - ImGui::GetFrameHeight();
			float ImGuiContentAreaSizeY = Tile(3) - (ContentAreaPadding * 2.0f);

			m_VPOStates[i].ContentArea = ImVec4(ImGuiContentAreaOriginX, ImGuiContentAreaOriginY, ImGuiContentAreaSizeX, ImGuiContentAreaSizeY);
			m_VPOStates[i].ViewportSceneEmpty = true;

			++i;
		}

		//remove spare viewport overlay states
		if (i < m_VPOStates.size())
			m_VPOStates.erase(m_VPOStates.begin() + i, m_VPOStates.end());

	}//loadViewportOverlayStates

	void GUIManager::resizeActiveViewportOverlays(const std::vector<Vector4f>& Tiles) {
		if (Tiles.size() != m_VPOStates.size()) throw IndexOutOfBoundsExcept("Tiles.size() != m_VPOStates.size()");
		
		for (size_t i = 0; i < Tiles.size(); ++i) {
			
			Vector4f Tile = Tiles[i];

			// convert viewport content area boundaries from OpenGL coordinate space (origin in lower left corner of screen)
			// to Dear ImGui coordinate space (origin in upper left corner of screen)
			// x-axis stays the same, y-axis flips over

			float ContentAreaPadding = 10.0f;
			float ImGuiContentAreaOriginX = Tile(0) + ContentAreaPadding;
			float ImGuiContentAreaOriginY = m_RenderWinSize[1] - Tile(1) - Tile(3) + ContentAreaPadding;
			float ImGuiContentAreaSizeX = Tile(2) - (ContentAreaPadding * 2.0f) - ImGui::GetFrameHeight(); // - ImGui::GetFrameHeight() to make room for ArrowButton of SideMenu
			float ImGuiContentAreaSizeY = Tile(3) - (ContentAreaPadding * 2.0f);

			m_VPOStates[i].ContentArea = ImVec4(ImGuiContentAreaOriginX, ImGuiContentAreaOriginY, ImGuiContentAreaSizeX, ImGuiContentAreaSizeY);
		}
	}//resizeActiveViewportOverlays

	void GUIManager::buildViewportOverlays(ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries) {
		
		for (size_t i = 0; i < m_VPOStates.size(); ++i) {
			
			buildViewportOverlayButtonPanel(i, pVPMgr, pDatasetGeometries);
			
			if (m_VPOStates[i].ViewportSceneEmpty) {
				ImVec2 PosCenter(pVPMgr->viewportCenter(i)(0), m_RenderWinSize[1] - pVPMgr->viewportCenter(i)(1));
				buildViewportEmptyInfo(i, PosCenter);
			}
		}
	}//buildViewportOverlays

	void GUIManager::buildNoViewportsInfo(ImVec2 PosCenter) {
		
		std::string FirstLine = "( no viewports active )";
		std::string SecondLine = "Use the View menu (main menu bar) to";
		std::string ThirdLine = "configure additional viewports.";
				
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
				
		ImGui::SetNextWindowBgAlpha(0.25f); // transparent background
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.890f, 0.890f, 0.890f, 1.0f));
		ImGui::SetNextWindowContentSize(ImVec2(ImGui::CalcTextSize(FirstLine.c_str()).x + 140.0f, 0.0f));
		ImGui::SetNextWindowPos(PosCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		ImGui::Begin("##noviewports", nullptr, WindowFlags);
		
		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing
		ImGui::NewLine();
		float WinWidth = ImGui::GetWindowSize().x;
		ImGui::SameLine((WinWidth / 2.0f) - (ImGui::CalcTextSize(FirstLine.c_str()).x / 2.0f)); // used to center this line of text by setting an offset value

		ImGui::Text(FirstLine.c_str());

		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing

		ImGui::NewLine();
		ImGui::SameLine((WinWidth / 2.0f) - (ImGui::CalcTextSize(SecondLine.c_str()).x / 2.0f)); // used to center this line of text by setting an offset value
		ImGui::Text(SecondLine.c_str());
		
		ImGui::NewLine();
		ImGui::SameLine((WinWidth / 2.0f) - (ImGui::CalcTextSize(ThirdLine.c_str()).x / 2.0f)); // used to center this line of text by setting an offset value
		ImGui::Text(ThirdLine.c_str());

		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing

		ImGui::End();
		ImGui::PopStyleColor();
	}//buildNoViewportsInfo

	void GUIManager::buildViewportEmptyInfo(size_t ViewportIdx, ImVec2 PosCenter) {
				
		std::string FirstLine = "Viewport " + std::to_string(ViewportIdx);
		std::string SecondLine = "(viewport empty)";

		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		ImGui::SetNextWindowBgAlpha(0.25f); // transparent background
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.890f, 0.890f, 0.890f, 1.0f));
		ImGui::SetNextWindowContentSize(ImVec2(ImGui::CalcTextSize(SecondLine.c_str()).x + 40.0f, 0.0f));
		ImGui::SetNextWindowPos(PosCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		std::string WindowUID = "##emptyviewport_vp" + std::to_string(ViewportIdx);
		ImGui::Begin(WindowUID.c_str(), nullptr, WindowFlags);

		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing
		ImGui::NewLine();
		ImGui::SameLine((ImGui::GetWindowSize().x / 2.0f) - (ImGui::CalcTextSize(FirstLine.c_str()).x / 2.0f)); // used to center this line of text by setting an offset value

		ImGui::Text(FirstLine.c_str());

		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing
		ImGui::NewLine();
		ImGui::SameLine((ImGui::GetWindowSize().x / 2.0f) - (ImGui::CalcTextSize(SecondLine.c_str()).x / 2.0f)); // used to center this line of text by setting an offset value

		ImGui::Text(SecondLine.c_str());

		ImGui::Dummy(ImVec2(0.0f, 15.0f)); // dummy to add vertical spacing

		ImGui::End();
		ImGui::PopStyleColor();
	}//buildViewportEmptyInfo

	void GUIManager::buildViewportOverlayButtonPanel(size_t VPOverlayIdx, ViewportManager* pVPMgr, std::map<DatasetType, DatasetGeometryData*>* pDatasetGeometries) {//TODO

		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | /*ImGuiWindowFlags_NoBackground |*/ //<- zum Test auskommentiert
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

		std::string WindowUID = "##vpoverlay_controlpanel_vp" + std::to_string(VPOverlayIdx);

		ImGui::Begin(WindowUID.c_str(), nullptr, WindowFlags);

		ImGui::SameLine();

		ImVec2 ButtonPos;

		// "Overlay" button
		if (ImGui::Button("Overlay")) {
			//TODO
		}
		
		//TODO: Overlay popup

		// "Shading" button
		ButtonPos = ImGui::GetCursorScreenPos();
		if (ImGui::Button("Shading"))
			ImGui::OpenPopup("shading_popup");
		
		if (ImGui::BeginPopup("showhide_popup")) {
			ImVec2 PopupPos(ButtonPos.x, ButtonPos.y + ImGui::GetFrameHeight());
			ImGui::SetWindowPos(PopupPos);
						
			//TODO: radio buttons for dataset selection



			ImGui::EndPopup();
		}
		//TODO

		// "Show/Hide" button
		ButtonPos = ImGui::GetCursorScreenPos();
		if (ImGui::Button("Show"))
			ImGui::OpenPopup("showhide_popup");

		if (ImGui::BeginPopup("showhide_popup")) {
			ImVec2 PopupPos(ButtonPos.x, ButtonPos.y + ImGui::GetFrameHeight());
			ImGui::SetWindowPos(PopupPos);

			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

			if (ImGui::MenuItem("Template", "", &m_VPOStates[VPOverlayIdx].ShowTemplate)) {
				if (m_VPOStates[VPOverlayIdx].ShowTemplate == true) {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::TEMPLATE);

					if (itD != pDatasetGeometries->end()) {
						pVPMgr->addDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = false;
					}
				}
				else {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::TEMPLATE);
					
					if (itD != pDatasetGeometries->end()) {
						pVPMgr->removeDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
					}

					if (!m_VPOStates[VPOverlayIdx].ShowTemplate && !m_VPOStates[VPOverlayIdx].ShowDTemplate && !m_VPOStates[VPOverlayIdx].ShowTarget)
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = true;
				}
			}

			if (ImGui::MenuItem("Deformed Template", "", &m_VPOStates[VPOverlayIdx].ShowDTemplate)) {
				if (m_VPOStates[VPOverlayIdx].ShowDTemplate == true) {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::DTEMPLATE);

					if (itD != pDatasetGeometries->end()) {
						pVPMgr->addDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = false;
					}
				}
				else {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::DTEMPLATE);
					
					if (itD != pDatasetGeometries->end()) {
						pVPMgr->removeDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
					}

					if (!m_VPOStates[VPOverlayIdx].ShowTemplate && !m_VPOStates[VPOverlayIdx].ShowDTemplate && !m_VPOStates[VPOverlayIdx].ShowTarget)
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = true;
				}
			}

			if (ImGui::MenuItem("Target", "", &m_VPOStates[VPOverlayIdx].ShowTarget)) {
				if (m_VPOStates[VPOverlayIdx].ShowTarget == true) {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::TARGET);

					if (itD != pDatasetGeometries->end()) {
						pVPMgr->addDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = false;
					}
						
				}
				else {
					size_t VPIdx = m_VPOStates[VPOverlayIdx].ViewportIdx;
					auto itD = pDatasetGeometries->find(DatasetType::TARGET);
					
					if (itD != pDatasetGeometries->end()) {
						pVPMgr->removeDatasetDisplayData(VPIdx, itD);
						if (pVPMgr->activeDatasetDisplayDataArrangement(VPIdx) == DisplayDataArrangementMode::SIDE_BY_SIDE) pVPMgr->arrangeDatasetDisplayDataSideBySide(VPIdx);
						else pVPMgr->arrangeDatasetDisplayDataLayered(VPIdx);
					}
					
					if (!m_VPOStates[VPOverlayIdx].ShowTemplate && !m_VPOStates[VPOverlayIdx].ShowDTemplate && !m_VPOStates[VPOverlayIdx].ShowTarget)
						m_VPOStates[VPOverlayIdx].ViewportSceneEmpty = true;
				}
			}
			ImGui::PopItemFlag();
			ImGui::EndPopup();
		}

		// reposition panel using size of finalized window //TODO: precompute!
		ImVec2 Pos(m_VPOStates[VPOverlayIdx].ContentArea.x + m_VPOStates[VPOverlayIdx].ContentArea.z - ImGui::GetWindowWidth(), m_VPOStates[VPOverlayIdx].ContentArea.y);
		ImGui::SetWindowPos(Pos);

		ImGui::End();
	}//buildViewportOverlayButtonPanel
}