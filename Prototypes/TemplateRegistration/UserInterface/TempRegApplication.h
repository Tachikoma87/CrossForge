#ifndef __TEMPREG_TEMPREGAPPLICATION_H__
#define __TEMPREG_TEMPREGAPPLICATION_H__

#include "../../CForge/Core/ITListener.hpp"

#include "../../CForge/AssetIO/T3DMesh.hpp"

#include "../../CForge/Graphics/GLWindow.h"
#include "../../CForge/Graphics/Shader/SShaderManager.h"
#include "../../CForge/Graphics/RenderDevice.h"
#include "../../CForge/Graphics/Lights/DirectionalLight.h"
#include "../../CForge/Graphics/Lights/PointLight.h"
#include "../../CForge/Graphics/VirtualCamera.h"
#include "../../CForge/Graphics/SceneGraph/SceneGraph.h"
#include "../../CForge/Graphics/SceneGraph/SGNGeometry.h"
#include "../../CForge/Graphics/SceneGraph/SGNTransformation.h"

#include <Eigen/Eigen>
#include <imgui.h>

#include <set>
#include <array>
#include <vector>

#include "../Registration/TemplateFitter.h"
#include "Dataset/DatasetDisplayData.h"
#include "Dataset/Markers/DatasetMarkerInstance.h"
#include "Dataset/Markers/DatasetMarkerCloud.h"

using namespace Eigen;

namespace TempReg {

	class TempRegApplication : public CForge::ITListener<CForge::GLWindowMsg> { //TODO: override listen(...) function for window resize events

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Enums
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private:
		enum class DatasetIdentifier {
			NONE,
			TEMPLATE,
			TARGET,
			FITTING_RESULT
		};

		enum ViewportIdentifier : int32_t {
			NONE = -1,
			SEPARATE_SCENE_TEMPLATE = 0,
			SEPARATE_SCENE_FITTINGRES = 1,
			SEPARATE_SCENE_TARGET = 2,
			SHARED_SCENE = 3,
			VIEWPORT_COUNT = 4
		};

		enum class ProjectionMode {
			PERSPECTIVE,
			ORTHO
		};

		enum MarkerColor : int32_t {
			GEOMETRY_SELECTION = 0,
			FEATURECORR_IDLE = 1,
			FEATURECORR_SELECTION = 2,
			AUTOCORR_IDLE = 3,
			AUTOCORR_SELECTION = 4,
			COLOR_COUNT = 5
		};
		
		enum class CorrespondenceEditMode {
			DEACTIVATED,
			VIEW_AND_DELETE,
			CREATE
		};

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Member variables
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	private:
		struct Viewport {
			CForge::RenderDevice::Viewport VP;

			// viewport scene graph data
			CForge::SceneGraph SG;
			CForge::SGNTransformation SGRoot;
			bool TemplateVisible;
			bool TargetVisible;
			bool FittingResVisible;
			CForge::SGNTransformation SceneViewingTrans;
			CForge::SGNTransformation TemplateViewingTrans;
			CForge::SGNTransformation TargetViewingTrans;
			CForge::SGNTransformation FittingResViewingTrans;
			
			// Viewing controls
			CForge::VirtualCamera Cam;
			ProjectionMode ProjMode;
			Vector3f OrbitEye, OrbitTarget;
			float OrbitZoom;

			Viewport(void) {
				TemplateVisible = TargetVisible = FittingResVisible = false;
				ProjMode = ProjectionMode::PERSPECTIVE;
				OrbitEye = OrbitTarget = Vector3f::Zero(); 
				OrbitZoom = 0.0f;
			}
		};

		struct CreateSelection {
			bool TemplatePointSelected;
			bool TargetPointSelected;
			int32_t TemplatePointID;
			int32_t TargetPointID;
			int32_t TargetFace;
			Vector3f TemplatePointPos;
			Vector3f TemplatePointNormal;
			Vector3f TargetPointPos;
			Vector3f TargetPointNormal;
			float DistanceEuclidean;
			// additional correspondence properties here...

			CreateSelection(void) {
				clear();
			}//Constructor

			void clear(void) {
				// template data
				TemplatePointSelected = false;
				TemplatePointID = -1;
				TemplatePointPos = Vector3f::Zero();
				TemplatePointNormal = Vector3f::Zero();

				// target data
				TargetPointSelected = false;
				TargetPointID = -1;
				TargetFace = -1;
				TargetPointPos = Vector3f::Zero();
				TargetPointNormal = Vector3f::Zero();

				// distances
				DistanceEuclidean = FLT_MAX;
			}//clear

			void clearTemplateData(void) {
				TemplatePointSelected = false;
				TemplatePointID = -1;
				TemplatePointPos = Vector3f::Zero();
				TemplatePointNormal = Vector3f::Zero();
				DistanceEuclidean = FLT_MAX;
			}//clearTemplateData

			void clearTargetData(void) {
				TargetPointSelected = false;
				TargetPointID = -1;
				TargetFace = -1;
				TargetPointPos = Vector3f::Zero();
				TargetPointNormal = Vector3f::Zero();
				DistanceEuclidean = FLT_MAX;
			}//clearTargetData

			void setTemplateData(int32_t PointID, Vector3f Pos, Vector3f Normal) {
				TemplatePointSelected = true;
				TemplatePointID = PointID;
				TemplatePointPos = Pos;
				TemplatePointNormal = Normal;
				DistanceEuclidean = (TargetPointSelected) ? (TemplatePointPos - TargetPointPos).norm() : FLT_MAX;
			}//setTemplateData

			void setTargetData(int32_t PointID, int32_t Face, Vector3f Pos, Vector3f Normal) {
				TargetPointSelected = true;
				TargetPointID = PointID;
				TargetFace = Face;
				TargetPointPos = Pos;
				TargetPointNormal = Normal;
				DistanceEuclidean = (TargetPointSelected) ? (TemplatePointPos - TargetPointPos).norm() : FLT_MAX;
			}//setTargetData

			const bool ready(void) const {
				return TemplatePointSelected && TargetPointSelected;
			}//ready

			const bool hasSelection(void) const {
				return TemplatePointSelected || TargetPointSelected;
			}//hasSelection
		};

		struct RayGeometryIntersection {
			DatasetIdentifier Dataset;
			int32_t Face;
			int32_t PclPoint;
			Vector3f MeshIntersection;
			Vector3f BarycentricCoords;

			RayGeometryIntersection(void) {
				clear();
			}//Constructor

			void clear(void) {
				Dataset = DatasetIdentifier::NONE;
				Face = -1;
				PclPoint = -1;
				MeshIntersection = Vector3f::Zero();
				BarycentricCoords = Vector3f::Zero();
			}//clear
		};

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Rendering variables
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		CForge::GLWindow m_RenderWin;
		CForge::SShaderManager* m_pShaderMan;
		CForge::RenderDevice m_RenderDev;
		CForge::DirectionalLight m_Sun;
		CForge::RenderDevice::Viewport m_GBufferVP;
		std::string m_WindowTitle;
		uint64_t m_LastFPSPrint;
		int32_t m_FPSCount;
		float m_FPS;
		bool m_RenderScreenshot;
		uint32_t m_ScreenshotCount;
		bool m_CloseRenderWin;
		
		std::array<Viewport, ViewportIdentifier::VIEWPORT_COUNT> m_Viewports;
		std::set<ViewportIdentifier> m_ActiveViewports;
		DatasetDisplayData* m_TemplateDisplayData;
		DatasetDisplayData* m_TargetDisplayData;
		DatasetDisplayData* m_FittingResDisplayData;
		std::array<DatasetMarkerActor*, MarkerColor::COLOR_COUNT> m_MarkerActors;
		bool m_ShowFeatCorrMarkers;
		bool m_ShowAutoCorrMarkers;
		Vector3f m_DefaultCamPos;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Input variables
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::array<bool, 3> m_CurrentMBStates; // LMB == [0], RMB == [1], MMB == [2]
		std::array<bool, 3> m_OldMBStates; // LMB == [0], RMB == [1], MMB == [2]
		bool m_IgnoreMMB;
		bool m_IgnoreRMB;
		bool m_RotateAllViewports;
		bool m_CorrEditDeleteOnKeyPress;
		bool m_CorrEditOverwriteOnKeyPress;
		bool m_CorrEditTabKeyModeSwitch;

		Vector2f m_CurrentCursorPosOGL;
		Vector2f m_LastCursorPosOGL;
		ViewportIdentifier m_VPUnderMouse;
		ViewportIdentifier m_VPMouseFocus;
		RayGeometryIntersection m_RayIntersectRes;
		std::vector<size_t> m_SelectedCorrsByTemplateID; // a set of template vertex IDs, each used to reference an accompanying correspondence
		CreateSelection m_CorrEditCreateSelection;
		CorrespondenceEditMode m_CorrEditActiveMode;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GUI variables
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		ImU32 m_CheckboxBgGreenCol;
		ImU32 m_CheckboxBgYellowCol;
		ImU32 m_CheckboxBgRedCol;
		ImU32 m_CheckMarkGreenCol;
		ImU32 m_CheckMarkYellowCol;
		
		float m_SideMenuWinWidth;
		Vector4f m_ViewportContentArea; // bounding box around entire section reserved for viewports displaying datasets; in OpenGL coordinates (origin in lower left corner)

		bool m_CoarseFitScaleTemplate;
		TemplateFitter::VolumeComputation m_VolComputeComboSelected;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Template Fitter
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		TemplateFitter m_TFitter;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Methods
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	public:
		TempRegApplication(void);
		~TempRegApplication();

		void run(void);
					
	private:
		void initViewports(void);
		void initMarkerActors(std::string Filepath);
		void initTemplateMeshFromFile(std::string Filepath);
		void initTargetMeshFromFile(std::string Filepath);
		void initTargetPclFromFile(std::string Filepath);

		void listen(CForge::GLWindowMsg Msg);
		void processInput(void);
		void processGeneralKeyboardInput(void);
		void processViewportContentAreaInput(void);
		void processGUIInput(void);

		void render(void);
		
		void resizeActiveViewports(void);
		std::vector<Vector4f> calculateViewportTiling(void);
		ViewportIdentifier mouseInViewport(void); // returns ID of viewport containing the mouse cursor
		void updateSceneGraph(ViewportIdentifier ViewportID);
		void resetViewportCam(ViewportIdentifier ViewportID);
		void trackballRotateScene(ViewportIdentifier ViewportID, Vector2f Start, Vector2f End, const bool ApplyToAllViewports);
		void calculateFittingErrorColors(DatasetIdentifier DatasetID, std::vector<float>& FittingErrorVals); //TODO
		Matrix4f modelMatrixDataset(ViewportIdentifier ViewportID, DatasetIdentifier DatasetID);
		void clearAllCorrMarkers(void);
		//std::vector<Vector3f> calculateFittingErrorVertexColors(std::vector<float>& RegErrorVals); //TODO: requires methods from template fitting module

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Input Processing (in 3D scenes and GUI)
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Perform raycast test against visible datasets of a given viewport. (for picking / highlighting of vertices)
		void inputRaycastVisibleDatasets(ViewportIdentifier ViewportID, Vector2f& CursorPosOGL);
		void inputClickSelectGeometryPoint(ViewportIdentifier ViewportUnderMouse);
		void inputClickSelectCorrespondence(ViewportIdentifier ViewportUnderMouse);
		void inputCreateCorrespondence(void);
		void inputOverwriteCorrespondence(void);
		void inputClearCorrespondences(TemplateFitter::CorrespondenceType CT = TemplateFitter::CorrespondenceType::NONE);
		void inputDeleteSelectedCorrespondences(void);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// GUI
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// main window functions
		void guiViewerControlWindow(void); //TODO
		void guiSideMenuWindow(void); //TODO

		// side menu
		void guiSideMenuMainView(void); //TODO
		void guiFileStateCheckboxes(void);
		void guiFileLoadButtons(void);
		void guiEditCorrespondencesButton(void);

		// side menu -> correspondence editing sub menu
		void guiSideMenuCorrEditView(void);
		void guiCorrEditExitButton(float ExitButtonWidth);
		void guiCorrEditViewDelRadioButton(void);
		void guiCorrEditCreateRadioButton(void);

		// side menu -> correspondence editing sub menu -> view / delete correspondences mode
		void guiViewDelPanel(void);
		void guiDeleteSelectedButton(void);
		void guiDeleteAllButton(size_t TotalCorrCount);
		void guiDeleteFeatureButton(size_t FeatCorrCount);
		void guiDeleteAutomaticButton(size_t AutoCorrCount);

		// side menu -> correspondence editing sub menu -> create correspondences mode
		void guiCreatePanel(void);
		void guiSelectedGeometryPointsPropertiesList(void);
		void guiCreateAndOverwriteButton(void);
		void guiClearAllGeometryPointSelectionButton(void);
		void guiClearTemplateGeometryPointSelectionButton(void);
		void guiClearTargetGeometryPointSelectionButton(void);
				
		// viewer control buttons (for viewer control window above viewports)
		void guiViewerButtonShowTemplateFitting(void);
		void guiViewerButtonShowCorrEdit(void);
		void guiViewerButtonMeshVis(void);
		void guiViewerButtonColorization(void);
		void guiViewerButtonReset(void);

		// keyboard triggered popups
		void guiCorrEditViewDeletePopupOnKeyPress(void);
		void guiCorrEditViewOverwritePopupOnKeyPress(void);

		// other repeated functions
		void guiCorrespondenceDataSummary(void);
		void guiShowCorrespondenceCheckboxes(void);
		void guiSelectedCorrespondencePropertiesList(void);
		

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 
		// Helpers
		//
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Quaternionf trackballRotation(ViewportIdentifier ViewportID, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL);
		Vector3f mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize);
		std::string guiFloatToTextLabel(float Val);
		void guiSpacedSeparator(void);
		void guiTooltip(const char* Text, ImGuiHoveredFlags HoveredFlags = 0);
	};
}

#endif