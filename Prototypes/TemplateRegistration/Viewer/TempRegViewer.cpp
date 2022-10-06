#include "TempRegViewer.h"

#include "../../../CForge/Graphics/GraphicsUtility.h"
#include "../../../CForge/AssetIO/SAssetIO.h"
#include "../../../Examples/SceneUtilities.hpp"

#include <iomanip>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

namespace TempReg {

	TempRegViewer::TempRegViewer(void) : 
		// CForge
		m_WindowTitle(""), m_GBufferWidth(0), m_GBufferHeight(0), m_LastFPSPrint(0), m_FPSCount(0), m_FPS(0.0f), m_RenderScreenshot(false),	m_CloseRenderWin(false), 
		// dataset display data
		m_TemplateDisplayData(nullptr), m_TargetDisplayData(nullptr), m_FittingResDisplayData(nullptr), m_MarkerActors{ nullptr }, m_ShowFeatCorrMarkers(false), m_ShowAutoCorrMarkers(false),
		m_DefaultCamPos(Vector3f::Zero()),
		// input processing
		m_CurrentMBStates{ false }, m_OldMBStates{ false }, m_IgnoreMMB(false), m_IgnoreRMB(false), m_RotateAllViewports(false), m_CorrEditDeleteOnKeyPress(false), m_CorrEditOverwriteOnKeyPress(false),
		m_CorrEditTabKeyModeSwitch(false), m_CurrentCursorPosOGL(Vector2f::Zero()),	m_LastCursorPosOGL(Vector2f::Zero()), m_VPUnderMouse(ViewportIdentifier::NONE), m_VPMouseFocus(ViewportIdentifier::NONE),
		// gui
		m_CheckboxBgGreenCol(0), m_CheckboxBgYellowCol(0), m_CheckboxBgRedCol(0), m_CheckMarkGreenCol(0), m_CheckMarkYellowCol(0), m_SideMenuWinSize(ImVec2(0.0f, 0.0f)), 
		m_ViewerCtrlWinSize(ImVec2(0.0f, 0.0f)), m_ViewportContentArea(Vector4f::Zero()),
		// template fitting
		m_CoarseFitScaleTemplate(false), m_VolComputeComboSelected(TemplateFitter::VolumeComputation::DISABLED)
	{}//Constructor

	TempRegViewer::~TempRegViewer() {
		// clear all viewports
		for (size_t i = 0; i < ViewportIdentifier::VIEWPORT_COUNT; ++i) {
			m_Viewports[i].SGRoot.clear();
			m_Viewports[i].SceneViewingTrans.clear();
			m_Viewports[i].TemplateViewingTrans.clear();
			m_Viewports[i].TargetViewingTrans.clear();
			m_Viewports[i].FittingResViewingTrans.clear();
			m_Viewports[i].SG.clear();
			m_Viewports[i].Cam.clear();
		}

		// clear DatasetDisplayData
		m_TemplateDisplayData->clear();
		delete m_TemplateDisplayData;
		m_TemplateDisplayData = nullptr;

		m_TargetDisplayData->clear();
		delete m_TargetDisplayData;
		m_TargetDisplayData = nullptr;

		m_FittingResDisplayData->clear();
		delete m_FittingResDisplayData;
		m_FittingResDisplayData = nullptr;
		

		for (auto& MA : m_MarkerActors) {
			MA->release();
			MA = nullptr;
		}
	}//Destructor

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Public methods
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TempRegViewer::init() {
		m_pSMan = CForge::SShaderManager::instance();

		m_WindowTitle = "CForge - Template Registration Example";
		m_FPS = 60.0f;

		bool const LowRes = false;
		bool const HighRes = true;

		uint32_t WinWidth = 1280;
		uint32_t WinHeight = 720;

		m_GBufferWidth = 1024;
		m_GBufferHeight = 768;

		//m_GBufferWidth = 1600;
		//m_GBufferHeight = 900;

		if (LowRes) {
			WinWidth = 720;
			WinHeight = 576;
		}
		if (HighRes) {
			WinWidth = 1600;
			WinHeight = 900;
		}

		// create an OpenGL capable windows
		m_RenderWin.init(Vector2i(100, 100), Vector2i(WinWidth, WinHeight), m_WindowTitle);

		gladLoadGL();

		// configure and initialize rendering pipeline
		m_Config.DirectionalLightsCount = 1;
		m_Config.PointLightsCount = 1;
		m_Config.SpotLightsCount = 0;
		m_Config.ExecuteLightingPass = true;
		m_Config.GBufferHeight = m_GBufferHeight;
		m_Config.GBufferWidth = m_GBufferWidth;
		m_Config.pAttachedWindow = &m_RenderWin;
		m_Config.PhysicallyBasedShading = true;
		m_Config.UseGBuffer = true;
		m_RDev.init(&m_Config);

		// configure and initialize shader configuration device
		m_LC.DirLightCount = 1;
		m_LC.PointLightCount = 1;
		m_LC.SpotLightCount = 0;
		m_LC.PCFSize = 1;
		m_LC.ShadowBias = 0.0004f;
		m_LC.ShadowMapCount = 1;
		m_pSMan->configShader(m_LC);

		// initialize sun (key lights) and back ground light (fill light)
		Vector3f SunPos = Vector3f(-5.0f, 15.0f, 35.0f);
		Vector3f BGLightPos = Vector3f(0.0f, 5.0f, -30.0f);
		m_Sun.init(SunPos, -SunPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 5.0f);
		// sun will cast shadows
		m_Sun.initShadowCasting(1024, 1024, CForge::GraphicsUtility::orthographicProjection(10.0f, 10.0f, 0.1f, 1000.0f));
		m_BGLight.init(BGLightPos, -BGLightPos.normalized(), Vector3f(1.0f, 1.0f, 1.0f), 1.5f, Vector3f(0.0f, 0.0f, 0.0f));

		// set lights
		m_RDev.addLight(&m_Sun);
		m_RDev.addLight(&m_BGLight);

		// we need one viewport for the GBuffer
		m_GBufferVP.Position = Vector2i(0, 0);
		m_GBufferVP.Size = Vector2i(m_GBufferWidth, m_GBufferHeight);

		// stuff for performance monitoring
		m_LastFPSPrint = CForge::CoreUtility::timestamp();
		m_FPSCount = 0;

		std::string GLError = "";
		CForge::GraphicsUtility::checkGLError(&GLError);
		if (!GLError.empty()) printf("GLError occurred: %s\n", GLError.c_str());

		m_RenderScreenshot = false;
		m_CloseRenderWin = false;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Initialize input variables
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		m_CurrentMBStates[0] = m_CurrentMBStates[1] = m_CurrentMBStates[2] = GLFW_RELEASE;
		m_OldMBStates[0] = m_OldMBStates[1] = m_OldMBStates[2] = GLFW_RELEASE;
		m_IgnoreMMB = false;
		m_IgnoreRMB = false;
		m_CurrentCursorPosOGL = Vector2f::Zero();
		m_LastCursorPosOGL = Vector2f::Zero();
		m_VPUnderMouse = ViewportIdentifier::NONE;
		m_VPMouseFocus = ViewportIdentifier::NONE;
		m_RotateAllViewports = false;
		m_CorrEditDeleteOnKeyPress = false;
		m_CorrEditOverwriteOnKeyPress = false;
		m_CorrEditTabKeyModeSwitch = false;
		m_CorrEditActiveMode = CorrespondenceEditMode::DEACTIVATED;
		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Initialize GUI
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//ImGui::StyleColorsLight();
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)m_RenderWin.handle(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		m_CheckboxBgGreenCol = IM_COL32(41, 122, 59, 138);
		m_CheckboxBgYellowCol = IM_COL32(120, 122, 41, 138);
		m_CheckboxBgRedCol = IM_COL32(122, 41, 41, 138);
		m_CheckMarkGreenCol = IM_COL32(0, 255, 0, 255);
		m_CheckMarkYellowCol = IM_COL32(255, 255, 0, 255);
		
		m_SideMenuWinSize.x = 350.0f;
		m_SideMenuWinSize.y = (float)m_RenderWin.height();

		m_ViewerCtrlWinSize.x = (float)m_RenderWin.width() - m_SideMenuWinSize.x + 1;
		m_ViewerCtrlWinSize.y = 23.0f; // == ImGui::GetFrameHeightWithSpacing()

		//TODO: m_ViewportContentArea has to be updated when: (1) m_RenderWin is resized; (2) visibility of GUI is changed via "Hide GUI" button
		m_ViewportContentArea(0) = 0.0f;
		m_ViewportContentArea(1) = 0.0f;
		m_ViewportContentArea(2) = float(m_RenderWin.width()) - m_SideMenuWinSize.x;
		m_ViewportContentArea(3) = float(m_RenderWin.height()) - m_ViewerCtrlWinSize.y;
		
		m_CoarseFitScaleTemplate = false;
		m_VolComputeComboSelected = TemplateFitter::VolumeComputation::DISABLED;

		//ImGui::GetStyle().FrameBorderSize = 1.0f;
		//ImGui::GetStyle().WindowRounding = 4.0f;
		//ImGui::GetStyle().FrameRounding = 3.0f;
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f; // window background alpha
		ImGui::GetStyle().Colors[ImGuiCol_PopupBg].w = 1.0f; // popup background alpha

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		// Initialize rendering components for dataset viewing
		//
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		m_TemplateDisplayData = nullptr;
		m_TargetDisplayData = nullptr;
		m_FittingResDisplayData = nullptr;
		m_MarkerActors = { nullptr };
		m_ShowFeatCorrMarkers = true;
		m_ShowAutoCorrMarkers = true;
		m_DefaultCamPos = Vector3f(0.0f, 0.0f, 60.0f);

		initMarkerActors("Assets/ExampleScenes/TempReg/UnitSphere.obj");
		initViewports();

		// On startup: enable viewports SEPARATE_SCENE_TEMPLATE and SEPARATE_SCENE_TARGET
		m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
		m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TARGET);
		resizeActiveViewports();
	}//init

	void TempRegViewer::initTemplateFromFile(TemplateFitter::GeometryType GeometryType, std::string Filepath) {
		m_TFitter.clearTemplate();
		m_TFitter.loadTemplate(GeometryType, Filepath);

		if (m_TemplateDisplayData != nullptr) {
			m_TemplateDisplayData->clear();
			delete m_TemplateDisplayData;
		}

		if (m_FittingResDisplayData != nullptr) {
			m_FittingResDisplayData->clear();
			delete m_FittingResDisplayData;
		}

		// default color: light grey #d1d1d1
		Vector3f SolidColor = Vector3f(0.819f, 0.819f, 0.819f);

		m_TemplateDisplayData = new DatasetDisplayData();
		m_FittingResDisplayData = new DatasetDisplayData();

		if (GeometryType == TemplateFitter::GeometryType::MESH) {
			m_TemplateDisplayData->initAsMesh(Filepath, SolidColor, Vector3f(0.08f, 0.08f, 0.08f), m_ShowFeatCorrMarkers, m_ShowAutoCorrMarkers);
			m_FittingResDisplayData->initAsMesh(Filepath, SolidColor, Vector3f(0.08f, 0.08f, 0.08f), m_ShowFeatCorrMarkers, m_ShowAutoCorrMarkers);
		}

		const Vector3f& TemplateCentroid = m_TFitter.getTemplate().surfaceCentroid();
		m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TEMPLATE].TemplateViewingTrans.translation(-TemplateCentroid);
		m_Viewports[ViewportIdentifier::SEPARATE_SCENE_FITTINGRES].FittingResViewingTrans.translation(-TemplateCentroid);
	}//initTemplateFromFile

	void TempRegViewer::initTargetFromFile(TemplateFitter::GeometryType GeometryType, std::string Filepath) {
		m_TFitter.clearTarget();
		m_TFitter.loadTarget(GeometryType, Filepath);

		if (m_TargetDisplayData != nullptr) {
			m_TargetDisplayData->clear();
			delete m_TargetDisplayData;
		}

		// default color: light blue #a2c9f6
		Vector3f SolidColor = Vector3f(0.635f, 0.788f, 0.964f);

		m_TargetDisplayData = new DatasetDisplayData();

		if (GeometryType == TemplateFitter::GeometryType::MESH) m_TargetDisplayData->initAsMesh(Filepath, SolidColor, Vector3f(0.08f, 0.08f, 0.08f), m_ShowFeatCorrMarkers, m_ShowAutoCorrMarkers);
		if (GeometryType == TemplateFitter::GeometryType::POINTCLOUD) m_TargetDisplayData->initAsPointCloud(Filepath, SolidColor, Vector3f(0.08f, 0.08f, 0.08f), m_ShowFeatCorrMarkers, m_ShowAutoCorrMarkers);

		const Vector3f& TargetCentroid = (GeometryType == TemplateFitter::GeometryType::MESH) ? m_TFitter.getTarget().surfaceCentroid() : m_TFitter.getTarget().vertexCentroid();
		m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TARGET].TargetViewingTrans.translation(-TargetCentroid);
	}//initTargetFromFile

	void TempRegViewer::processInput(void) {
		m_RenderWin.update(); // calls glfwPollEvents()
		//TODO: check m_RenderWin for windows resize here? -> update m_ViewportContentArea

		// get current mouse states and mouse related states
		m_CurrentMBStates = {
			m_RenderWin.mouse()->buttonState(CForge::Mouse::BTN_LEFT), 
			m_RenderWin.mouse()->buttonState(CForge::Mouse::BTN_RIGHT),
			m_RenderWin.mouse()->buttonState(CForge::Mouse::BTN_MIDDLE) };

		if ((m_CurrentMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_PRESS && m_OldMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_RELEASE) &&
			(m_CurrentMBStates[CForge::Mouse::BTN_RIGHT] == GLFW_PRESS && m_OldMBStates[CForge::Mouse::BTN_RIGHT] == GLFW_RELEASE)) {
			m_IgnoreMMB = true;
			m_IgnoreRMB = true;
		}
		else {
			m_IgnoreMMB = (m_CurrentMBStates[CForge::Mouse::BTN_RIGHT] == GLFW_PRESS && !m_IgnoreRMB) ? true : false;
			m_IgnoreRMB = (m_CurrentMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_PRESS && !m_IgnoreMMB) ? true : false;
		}

		m_CurrentCursorPosOGL = Vector2f(m_RenderWin.mouse()->position().x(), (float)m_RenderWin.height() - m_RenderWin.mouse()->position().y());
		m_VPUnderMouse = mouseInViewport();

		processGeneralKeyboardInput();
		processViewportContentAreaInput();
		processGUIInput();

		// store this frame's mouse states for next frame
		m_OldMBStates[CForge::Mouse::BTN_LEFT] = m_CurrentMBStates[CForge::Mouse::BTN_LEFT];
		m_OldMBStates[CForge::Mouse::BTN_RIGHT] = m_CurrentMBStates[CForge::Mouse::BTN_RIGHT];
		m_OldMBStates[CForge::Mouse::BTN_MIDDLE] = m_CurrentMBStates[CForge::Mouse::BTN_MIDDLE];
		m_LastCursorPosOGL = m_CurrentCursorPosOGL;

		m_RotateAllViewports = false;
	}//processInput

	void TempRegViewer::render(void) {
		bool ClearBuffer = true;

		// render active viewports
		for (const auto& VP : m_ActiveViewports) {
			// update camera for viewport, update scene graph
			if (m_RDev.activeCamera() != &m_Viewports[VP].Cam) m_RDev.activeCamera(&m_Viewports[VP].Cam);
			updateSceneGraph(VP);

			// render scene as usual
			m_RDev.viewport(m_GBufferVP);
			m_RDev.activePass(CForge::RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_Viewports[VP].SG.render(&m_RDev);
			m_RDev.activePass(CForge::RenderDevice::RENDERPASS_GEOMETRY);
			m_Viewports[VP].SG.render(&m_RDev);

			// set viewport and perform lighting pass
			// this will produce the correct tile in the final output window (backbuffer to be specific)
			m_RDev.viewport(m_Viewports[VP].VP);
			m_RDev.activePass(CForge::RenderDevice::RENDERPASS_LIGHTING, nullptr, ClearBuffer);

			ClearBuffer = false;
		}

		// render GUI
		ImGui::Render();
		glViewport(0, 0, m_RenderWin.width(), m_RenderWin.height());
		//glViewport(0, 0, m_RenderWinSize[0], m_RenderWinSize[1]);
		if (ClearBuffer) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (m_RenderScreenshot) {
			m_RenderScreenshot = false;
			CForge::RenderDevice::Viewport V;
			V.Position = Vector2i(0, 0);
			V.Size = Vector2i(m_RenderWin.width(), m_RenderWin.height());
			m_RDev.viewport(V);
			m_RDev.activePass(CForge::RenderDevice::RENDERPASS_FORWARD);
			CForge::SceneUtilities::takeScreenshot("Screenshot.jpg");
		}
	}//render

	bool TempRegViewer::shutdown(void) {		
		return m_RenderWin.shutdown();
	}//shutdown

	void TempRegViewer::finishFrame(void) {
		m_RenderWin.swapBuffers();

		m_FPSCount++;
		if (CForge::CoreUtility::timestamp() - m_LastFPSPrint > 1000U) {
			char Buf[64];
			sprintf(Buf, "FPS: %d\n", m_FPSCount);
			m_FPS = float(m_FPSCount);
			m_FPSCount = 0;
			m_LastFPSPrint = CForge::CoreUtility::timestamp();

			m_RenderWin.title(m_WindowTitle + "[" + std::string(Buf) + "]");
		}

		if (m_CloseRenderWin) m_RenderWin.closeWindow();
	}//finishFrame

	void TempRegViewer::releaseShaderManager(void) {
		m_pSMan->release();
	}//releaseShaderManager

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// Private methods
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TempRegViewer::initViewports(void) {
		auto& VPSeparateTemplate = m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TEMPLATE];
		VPSeparateTemplate.VP.Position = Vector2i(0, 0);
		VPSeparateTemplate.VP.Size = Vector2i(m_ViewportContentArea(2), m_ViewportContentArea(3));
		VPSeparateTemplate.SGRoot.init(nullptr);
		VPSeparateTemplate.SG.init(&VPSeparateTemplate.SGRoot);
		VPSeparateTemplate.SceneViewingTrans.init(&VPSeparateTemplate.SGRoot);
		VPSeparateTemplate.TemplateViewingTrans.init(&VPSeparateTemplate.SceneViewingTrans);
		VPSeparateTemplate.TargetViewingTrans.init(&VPSeparateTemplate.SceneViewingTrans);
		VPSeparateTemplate.FittingResViewingTrans.init(&VPSeparateTemplate.SceneViewingTrans);
		VPSeparateTemplate.TemplateVisible = true;
		VPSeparateTemplate.TargetVisible = false;
		VPSeparateTemplate.FittingResVisible = false;
		VPSeparateTemplate.Cam.init(m_DefaultCamPos, Vector3f::UnitY());
		VPSeparateTemplate.Cam.projectionMatrix(VPSeparateTemplate.VP.Size.x(), VPSeparateTemplate.VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		auto& VPSeparateDTemplate = m_Viewports[ViewportIdentifier::SEPARATE_SCENE_FITTINGRES];
		VPSeparateDTemplate.VP.Position = Vector2i(0, 0);
		VPSeparateDTemplate.VP.Size = Vector2i(m_ViewportContentArea(2), m_ViewportContentArea(3));
		VPSeparateDTemplate.SGRoot.init(nullptr);
		VPSeparateDTemplate.SG.init(&VPSeparateDTemplate.SGRoot);
		VPSeparateDTemplate.SceneViewingTrans.init(&VPSeparateDTemplate.SGRoot);
		VPSeparateDTemplate.TemplateViewingTrans.init(&VPSeparateDTemplate.SceneViewingTrans);
		VPSeparateDTemplate.TargetViewingTrans.init(&VPSeparateDTemplate.SceneViewingTrans);
		VPSeparateDTemplate.FittingResViewingTrans.init(&VPSeparateDTemplate.SceneViewingTrans);
		VPSeparateDTemplate.TemplateVisible = false;
		VPSeparateDTemplate.TargetVisible = false;
		VPSeparateDTemplate.FittingResVisible = true;
		VPSeparateDTemplate.Cam.init(m_DefaultCamPos, Vector3f::UnitY());
		VPSeparateDTemplate.Cam.projectionMatrix(VPSeparateDTemplate.VP.Size.x(), VPSeparateDTemplate.VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		auto& VPSeparateTarget = m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TARGET];
		VPSeparateTarget.VP.Position = Vector2i(0, 0);
		VPSeparateTarget.VP.Size = Vector2i(m_ViewportContentArea(2), m_ViewportContentArea(3));
		VPSeparateTarget.SGRoot.init(nullptr);
		VPSeparateTarget.SG.init(&VPSeparateTarget.SGRoot);
		VPSeparateTarget.SceneViewingTrans.init(&VPSeparateTarget.SGRoot);
		VPSeparateTarget.TemplateViewingTrans.init(&VPSeparateTarget.SceneViewingTrans);
		VPSeparateTarget.TargetViewingTrans.init(&VPSeparateTarget.SceneViewingTrans);
		VPSeparateTarget.FittingResViewingTrans.init(&VPSeparateTarget.SceneViewingTrans);
		VPSeparateTarget.TemplateVisible = false;
		VPSeparateTarget.TargetVisible = true;
		VPSeparateTarget.FittingResVisible = false;
		VPSeparateTarget.Cam.init(m_DefaultCamPos, Vector3f::UnitY());
		VPSeparateTarget.Cam.projectionMatrix(VPSeparateTarget.VP.Size.x(), VPSeparateTarget.VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		auto& VPShared = m_Viewports[ViewportIdentifier::SHARED_SCENE];
		VPShared.VP.Position = Vector2i(0, 0);
		VPShared.VP.Size = Vector2i(m_ViewportContentArea(2), m_ViewportContentArea(3));
		VPShared.SGRoot.init(nullptr);
		VPShared.SG.init(&VPShared.SGRoot);
		VPShared.SceneViewingTrans.init(&VPShared.SGRoot);
		VPShared.TemplateViewingTrans.init(&VPShared.SceneViewingTrans);
		VPShared.TargetViewingTrans.init(&VPShared.SceneViewingTrans);
		VPShared.FittingResViewingTrans.init(&VPShared.SceneViewingTrans);
		VPShared.TemplateVisible = true;
		VPShared.TargetVisible = true;
		VPShared.FittingResVisible = false;
		VPShared.Cam.init(m_DefaultCamPos, Vector3f::UnitY());
		VPShared.Cam.projectionMatrix(VPShared.VP.Size.x(), VPShared.VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);

		m_ActiveViewports.clear();
	}//initViewports

	void TempRegViewer::initMarkerActors(std::string Filepath) {
		CForge::T3DMesh<float> RawMesh;
		CForge::SAssetIO::load(Filepath, &RawMesh);
		CForge::SceneUtilities::setMeshShader(&RawMesh, 1.0f, 0.0f);
		RawMesh.computePerVertexNormals();

		// Initialize marker model for MarkerColor::GEOMETRY_SELECTION
		std::vector<Vector3f> Colors = std::vector<Vector3f>(RawMesh.vertexCount(), Vector3f(0.0f, 0.0f, 0.0f)); // yellow
		RawMesh.colors(&Colors);
		m_MarkerActors[MarkerColor::GEOMETRY_SELECTION] = new DatasetMarkerActor();
		m_MarkerActors[MarkerColor::GEOMETRY_SELECTION]->init(&RawMesh, true);

		// Initialize marker model for MarkerColor::FEATURECORR_IDLE
		Colors.assign(RawMesh.vertexCount(), Vector3f(0.0f, 0.5f, 0.0f)); // dark green #32853a
		RawMesh.colors(&Colors);
		m_MarkerActors[MarkerColor::FEATURECORR_IDLE] = new DatasetMarkerActor();
		m_MarkerActors[MarkerColor::FEATURECORR_IDLE]->init(&RawMesh, false);

		// Initialize marker model for MarkerColor::FEATURECORR_SELECTED
		Colors.assign(RawMesh.vertexCount(), Vector3f(0.0f, 1.0f, 0.0f)); // bright green #00ff16
		RawMesh.colors(&Colors);
		m_MarkerActors[MarkerColor::FEATURECORR_SELECTION] = new DatasetMarkerActor();
		m_MarkerActors[MarkerColor::FEATURECORR_SELECTION]->init(&RawMesh, true);

		// Initialize marker model for MarkerColor::AUTOCORR_IDLE
		Colors.assign(RawMesh.vertexCount(), Vector3f(0.5f, 0.0f, 0.0f)); // dark red #8b3234
		RawMesh.colors(&Colors);
		m_MarkerActors[MarkerColor::AUTOCORR_IDLE] = new DatasetMarkerActor();
		m_MarkerActors[MarkerColor::AUTOCORR_IDLE]->init(&RawMesh, false);

		// Initialize marker model for MarkerColor::AUTOCORR_SELECTED
		Colors.assign(RawMesh.vertexCount(), Vector3f(1.0f, 0.0f, 0.0f)); // bright red #ff0005
		RawMesh.colors(&Colors);
		m_MarkerActors[MarkerColor::AUTOCORR_SELECTION] = new DatasetMarkerActor();
		m_MarkerActors[MarkerColor::AUTOCORR_SELECTION]->init(&RawMesh, true);
	}//initMarkerActors

	void TempRegViewer::processGeneralKeyboardInput(void) {
		if (!ImGui::GetIO().WantCaptureKeyboard) {
			m_RenderScreenshot = m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_F10, true);
			m_CloseRenderWin = m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_ESCAPE);
			m_RotateAllViewports = (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_LEFT_ALT));

			// Correspondence editing key actions
			if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_DELETE) && m_CorrEditActiveMode == CorrespondenceEditMode::VIEW_AND_DELETE)
				if (!m_SelectedCorrsByTemplateID.empty()) m_CorrEditDeleteOnKeyPress = true;

			if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_ENTER) && m_CorrEditActiveMode == CorrespondenceEditMode::CREATE) {
				if (m_CorrEditCreateSelection.TemplatePointSelected && m_CorrEditCreateSelection.TargetPointSelected) {
					if (m_TFitter.hasCorrespondence(m_CorrEditCreateSelection.TemplatePointID)) m_CorrEditOverwriteOnKeyPress = true;
					else inputCreateCorrespondence();
				}
			}
		}// end [if (!ImGui::GetIO().WantCaptureKeyboard)]
	}//processGeneralKeyboardInput

	void TempRegViewer::processViewportContentAreaInput(void) {
		// Process keyboard inputs...
		if (!ImGui::GetIO().WantCaptureKeyboard) {
			// Camera update //TODO: disable WASD movement later, change to orbit camera (lookAt) - blender like?
			if (m_VPUnderMouse != ViewportIdentifier::NONE) {
				float MovementSpeed = 0.4f;
				if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_W)) m_Viewports[m_VPUnderMouse].Cam.forward(MovementSpeed); // move forward
				if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_S)) m_Viewports[m_VPUnderMouse].Cam.forward(-MovementSpeed); // move backward
				if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_A)) m_Viewports[m_VPUnderMouse].Cam.right(-MovementSpeed); // strafe left
				if (m_RenderWin.keyboard()->keyPressed(CForge::Keyboard::KEY_D)) m_Viewports[m_VPUnderMouse].Cam.right(MovementSpeed); // strafe right
			}
		}// end [if (!ImGui::GetIO().WantCaptureKeyboard)]

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Process mouse inputs...				
		if (!ImGui::GetIO().WantCaptureMouse) {
			// -> process middle mouse button
			if (!m_IgnoreMMB) {
				if (m_CurrentMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_PRESS) {
					if (m_OldMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_RELEASE) m_VPMouseFocus = m_VPUnderMouse; // focus viewport under mouse on middle mouse button click
						
					if (m_VPMouseFocus != ViewportIdentifier::NONE) {
						Vector2f RotationStart = (m_OldMBStates[CForge::Mouse::BTN_MIDDLE] == GLFW_RELEASE) ? m_CurrentCursorPosOGL : m_LastCursorPosOGL;
						Vector2f RotationEnd = m_CurrentCursorPosOGL;

						trackballRotateScene(m_VPMouseFocus, RotationStart, RotationEnd, m_RotateAllViewports);
					}
				}
				else {
					m_VPMouseFocus = ViewportIdentifier::NONE;
				}
			} // end processing of middle mouse button

			// -> process left mouse button
			if (m_VPUnderMouse != ViewportIdentifier::NONE) {
				if (m_CurrentMBStates[CForge::Mouse::BTN_LEFT] == GLFW_RELEASE && m_OldMBStates[CForge::Mouse::BTN_LEFT] == GLFW_PRESS) { // execute processing on button release to make sure processing is not repeated next frame
					if (m_CorrEditActiveMode == CorrespondenceEditMode::DEACTIVATED || m_CorrEditActiveMode == CorrespondenceEditMode::VIEW_AND_DELETE) {
						inputRaycastVisibleDatasets(m_VPUnderMouse, m_CurrentCursorPosOGL);
						inputClickSelectCorrespondence(m_VPUnderMouse);
					}
					if (m_CorrEditActiveMode == CorrespondenceEditMode::CREATE) {
						inputRaycastVisibleDatasets(m_VPUnderMouse, m_CurrentCursorPosOGL);
						inputClickSelectGeometryPoint(m_VPUnderMouse);
					}
				}
			}// end processing of left mouse button

			// -> process right mouse button
			if (!m_IgnoreRMB) {
				if (m_CurrentMBStates[CForge::Mouse::BTN_RIGHT] == GLFW_PRESS) {
					if (m_OldMBStates[CForge::Mouse::BTN_RIGHT] == GLFW_RELEASE) m_VPMouseFocus = m_VPUnderMouse; // focus viewport under mouse on right mouse button click
					
					if (m_VPMouseFocus != ViewportIdentifier::NONE) {
						if (m_RenderWin.mouse()->buttonState(CForge::Mouse::BTN_RIGHT)) { //TODO disable WASD movement later, change to orbit camera (lookAt) - blender like?
							float RotationSpeed = 1.0f;
							const Eigen::Vector2f MouseDelta = m_RenderWin.mouse()->movement();
							m_Viewports[m_VPMouseFocus].Cam.rotY(CForge::GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.x()));
							m_Viewports[m_VPMouseFocus].Cam.pitch(CForge::GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.y()));
							m_RenderWin.mouse()->movement(Vector2f::Zero());
						}
					}
				}
				else {
					m_VPMouseFocus = ViewportIdentifier::NONE;
				}				
			}// end processing of right mouse button
		}// end [if (!ImGui::GetIO().WantCaptureMouse)]
	}//processViewportContentAreaInput
	
	void TempRegViewer::processGUIInput(void) {
		// Dear ImGui's input processing starts with these 3 functions:
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ...and extends over all widget-functions (e.g. ImGui::Begin(...) - ImGui::End(...), ImGui::Button(...), etc.)
		//TODO: rename these methods to better reflect that!

		//TODO:
		/*
		if (<hide gui>) {

		}
		else {

		}
		*/

		
		guiViewerControlWindow();
		guiSideMenuWindow();
		//TODO: buildViewportIdentifiers(...);
		guiCorrEditViewDeletePopupOnKeyPress();
		guiCorrEditViewOverwritePopupOnKeyPress();
	}//processGUIInput

	void TempRegViewer::resizeActiveViewports(void) {
		auto Tiling = calculateViewportTiling();

		size_t TileIdx = 0;
		for (auto& VP : m_ActiveViewports) {
			Vector4i Tile = Tiling[TileIdx].array().ceil().cast<int>();
			m_Viewports[VP].VP.Position(0) = Tile(0);
			m_Viewports[VP].VP.Position(1) = Tile(1);
			m_Viewports[VP].VP.Size(0) = Tile(2);
			m_Viewports[VP].VP.Size(1) = Tile(3);
			m_Viewports[VP].Cam.projectionMatrix(m_Viewports[VP].VP.Size.x(), m_Viewports[VP].VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
			++TileIdx;
		}
	}//resizeActiveViewports

	std::vector<Vector4f> TempRegViewer::calculateViewportTiling(void) {
		std::vector<Vector4f> Tiles;
		Vector4f Tile0 = Vector4f::Zero();
		Vector4f Tile1 = Vector4f::Zero();
		Vector4f Tile2 = Vector4f::Zero();

		switch (m_ActiveViewports.size()) {
		case 1: {
			Tile0(0) = m_ViewportContentArea(0); // Pos.x
			Tile0(1) = m_ViewportContentArea(1); // Pos.y
			Tile0(2) = m_ViewportContentArea(2); // Size.x
			Tile0(3) = m_ViewportContentArea(3); // Size.y

			Tiles.push_back(Tile0);

			break;
		}
		case 2: {
			Tile0(0) = m_ViewportContentArea(0); // Pos0.x
			Tile0(1) = m_ViewportContentArea(1); // Pos0.y
			Tile0(2) = m_ViewportContentArea(2) / 2.0f; // Size0.x
			Tile0(3) = m_ViewportContentArea(3); // Size0.y

			Tile1(0) = m_ViewportContentArea(0) + (m_ViewportContentArea(2) / 2.0f); // Pos1.x
			Tile1(1) = m_ViewportContentArea(1); // Pos1.y
			Tile1(2) = m_ViewportContentArea(2) / 2.0f; // Size1.x
			Tile1(3) = m_ViewportContentArea(3); // Size1.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);

			break;
		}
		case 3: {
			Tile0(0) = m_ViewportContentArea(0); // Pos.x
			Tile0(1) = m_ViewportContentArea(1); // Pos.y
			Tile0(2) = m_ViewportContentArea(2) / 3.0f; // Size.x
			Tile0(3) = m_ViewportContentArea(3); // Size.y

			Tile1(0) = m_ViewportContentArea(0) + (m_ViewportContentArea(2) / 3.0f); // Pos.x
			Tile1(1) = m_ViewportContentArea(1); // Pos.y
			Tile1(2) = m_ViewportContentArea(2) / 3.0f; // Size.x
			Tile1(3) = m_ViewportContentArea(3); // Size.y

			Tile2(0) = m_ViewportContentArea(0) + ((m_ViewportContentArea(2) / 3.0f) * 2.0f); // Pos.x
			Tile2(1) = m_ViewportContentArea(1); // Pos.y
			Tile2(2) = m_ViewportContentArea(2) / 3.0f; // Size.x
			Tile2(3) = m_ViewportContentArea(3); // Size.y

			Tiles.push_back(Tile0);
			Tiles.push_back(Tile1);
			Tiles.push_back(Tile2);

			break;
		}
		}

		return Tiles;
	}//calculateViewportTiling
	
	TempRegViewer::ViewportIdentifier TempRegViewer::mouseInViewport(void) {
		ViewportIdentifier ViewportID = ViewportIdentifier::NONE;	
		Vector2f CurrentCursorPosOGL = Vector2f(m_RenderWin.mouse()->position().x(), (float)m_RenderWin.height() - m_RenderWin.mouse()->position().y());

		for (const auto& VP : m_ActiveViewports) {
			if (CurrentCursorPosOGL.x() >= (float)m_Viewports[VP].VP.Position.x() && CurrentCursorPosOGL.x() <= (float)m_Viewports[VP].VP.Position.x() + (float)m_Viewports[VP].VP.Size.x()) {
				if (CurrentCursorPosOGL.y() >= (float)m_Viewports[VP].VP.Position.y() && CurrentCursorPosOGL.y() <= (float)m_Viewports[VP].VP.Position.y() + (float)m_Viewports[VP].VP.Size.y()) {
					ViewportID = VP;
					break;
				}
			}
		}

		return ViewportID;
	}//mouseInViewport

	void TempRegViewer::updateSceneGraph(ViewportIdentifier ViewportID) {
		m_TemplateDisplayData->removeFromSceneGraph();
		if (m_Viewports[ViewportID].TemplateVisible) m_TemplateDisplayData->addToSceneGraph(&m_Viewports[ViewportID].TemplateViewingTrans);
		
		m_TargetDisplayData->removeFromSceneGraph();
		if (m_Viewports[ViewportID].TargetVisible) m_TargetDisplayData->addToSceneGraph(&m_Viewports[ViewportID].TargetViewingTrans);

		m_FittingResDisplayData->removeFromSceneGraph();
		if (m_Viewports[ViewportID].FittingResVisible) m_FittingResDisplayData->addToSceneGraph(&m_Viewports[ViewportID].FittingResViewingTrans);

		// update the scene graph for this viewport
		m_Viewports[ViewportID].SG.update(60.0f / m_FPS);
	}//updateSceneGraph

	void TempRegViewer::resetViewportCam(ViewportIdentifier ViewportID) {
		m_Viewports[ViewportID].Cam.resetToOrigin();
		m_Viewports[ViewportID].Cam.position(m_DefaultCamPos);
		m_Viewports[ViewportID].Cam.projectionMatrix(m_Viewports[ViewportID].VP.Size.x(), m_Viewports[ViewportID].VP.Size.y(), CForge::GraphicsUtility::degToRad(45.0f), 0.1f, 1000.0f);
	}//viewportCamReset

	void TempRegViewer::trackballRotateScene(ViewportIdentifier ViewportID, Vector2f Start, Vector2f End, const bool ApplyToAllViewports) {
		Quaternionf Rotation = trackballRotation(ViewportID, Start, End);

		m_Viewports[ViewportID].SceneViewingTrans.rotation(Rotation * m_Viewports[ViewportID].SceneViewingTrans.rotation());

		if (ApplyToAllViewports) {
			for (const auto& ActiveVP : m_ActiveViewports) {
				if (ActiveVP == ViewportID) continue;
				m_Viewports[ActiveVP].SceneViewingTrans.rotation(Rotation * m_Viewports[ActiveVP].SceneViewingTrans.rotation());
			}
		}
	}//trackballRotateScene

	void TempRegViewer::calculateFittingErrorColors(DatasetIdentifier DatasetID, std::vector<float>& FittingErrorVals) { //TODO
		//TODO
	}//calculateFittingErrorColors

	Matrix4f TempRegViewer::modelMatrixDataset(ViewportIdentifier ViewportID, DatasetIdentifier DatasetID) {
		if (DatasetID == DatasetIdentifier::NONE) throw CForgeExcept("DatasetID");

		Vector3f Translation = Vector3f::Zero();
		Quaternionf Rotation = Quaternionf::Identity();
		Vector3f Scaling = Vector3f::Ones();

		CForge::SGNTransformation* DatasetViewingTrans = nullptr;
		if (DatasetID == DatasetIdentifier::TEMPLATE) DatasetViewingTrans = &m_Viewports[ViewportID].TemplateViewingTrans;
		if (DatasetID == DatasetIdentifier::TARGET) DatasetViewingTrans = &m_Viewports[ViewportID].TargetViewingTrans;
		if (DatasetID == DatasetIdentifier::FITTING_RESULT) DatasetViewingTrans = &m_Viewports[ViewportID].FittingResViewingTrans;

		DatasetViewingTrans->buildTansformation(&Translation, &Rotation, &Scaling);

		Matrix4f TransMat = Matrix4f::Identity();
		TransMat(0, 3) = Translation(0);
		TransMat(1, 3) = Translation(1);
		TransMat(2, 3) = Translation(2);

		Matrix4f RotMat = Matrix4f::Identity();
		Matrix3f RotQuatToMat = Rotation.toRotationMatrix();
		RotMat.block<3, 3>(0, 0) = RotQuatToMat;

		Matrix4f ScaleMat = Matrix4f::Identity();
		ScaleMat(0, 0) = Scaling(0);
		ScaleMat(1, 1) = Scaling(1);
		ScaleMat(2, 2) = Scaling(2);

		Matrix4f Model = TransMat * RotMat * ScaleMat;
		return Model;
	}//modelMatrixDataset

	void TempRegViewer::clearAllCorrMarkers(void) {
		m_TemplateDisplayData->clearFeatureCorrMarkers();
		m_TemplateDisplayData->clearAutomaticCorrMarkers();

		m_TargetDisplayData->clearFeatureCorrMarkers();
		m_TargetDisplayData->clearAutomaticCorrMarkers();

		m_FittingResDisplayData->clearFeatureCorrMarkers();
		m_FittingResDisplayData->clearAutomaticCorrMarkers();
	}//clearAllCorrMarkers

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Input Processing (in 3D scenes and GUI)
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TempRegViewer::inputRaycastVisibleDatasets(ViewportIdentifier ViewportID, Vector2f& CursorPosOGL) {
		m_RayIntersectRes.Dataset = DatasetIdentifier::NONE;
		m_RayIntersectRes.Face = -1;
		m_RayIntersectRes.MeshIntersection = Vector3f::Zero();
		m_RayIntersectRes.BarycentricCoords = Vector3f::Zero();
		m_RayIntersectRes.PclPoint = -1;

		if (ViewportID == ViewportIdentifier::NONE) return; // clicked outside of all visible viewports, ray would miss all datasets anyways

		Vector4f Viewport = Vector4f(m_Viewports[ViewportID].VP.Position.x(), m_Viewports[ViewportID].VP.Position.y(), m_Viewports[ViewportID].VP.Size.x(), m_Viewports[ViewportID].VP.Size.y());
		Matrix4f View = m_Viewports[ViewportID].Cam.cameraMatrix();
		Matrix4f Projection = m_Viewports[ViewportID].Cam.projectionMatrix();

		if (m_Viewports[ViewportID].TemplateVisible) {
			const auto& Geometry = m_TFitter.getTemplate();
			Matrix4f Model = modelMatrixDataset(ViewportID, DatasetIdentifier::TEMPLATE);
			Geometry.raycastMeshPoint(CursorPosOGL, Viewport, Model, View, Projection, m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, m_RayIntersectRes.BarycentricCoords);
			if (m_RayIntersectRes.Face >= 0) m_RayIntersectRes.Dataset = DatasetIdentifier::TEMPLATE;
		}

		if (m_RayIntersectRes.Dataset != DatasetIdentifier::NONE) return;

		if (m_Viewports[ViewportID].FittingResVisible) {
			const auto& Geometry = m_TFitter.getFittingResult();
			Matrix4f Model = modelMatrixDataset(ViewportID, DatasetIdentifier::FITTING_RESULT);
			Geometry.raycastMeshPoint(CursorPosOGL, Viewport, Model, View, Projection, m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, m_RayIntersectRes.BarycentricCoords);
			if (m_RayIntersectRes.Face >= 0) m_RayIntersectRes.Dataset = DatasetIdentifier::FITTING_RESULT;
		}

		if (m_RayIntersectRes.Dataset != DatasetIdentifier::NONE) return;

		if (m_Viewports[ViewportID].TargetVisible) {
			const auto& Geometry = m_TFitter.getTarget();
			Matrix4f Model = modelMatrixDataset(ViewportID, DatasetIdentifier::TARGET);

			if (m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::MESH) { // pick point on mesh
				Geometry.raycastMeshPoint(CursorPosOGL, Viewport, Model, View, Projection, m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, m_RayIntersectRes.BarycentricCoords);
				if (m_RayIntersectRes.Face >= 0) m_RayIntersectRes.Dataset = DatasetIdentifier::TARGET;
			}
			else { // pick point in point cloud
				Geometry.raycastPclPoint(CursorPosOGL, Viewport, Model, View, Projection, m_RayIntersectRes.PclPoint);
				if (m_RayIntersectRes.PclPoint >= 0) m_RayIntersectRes.Dataset = DatasetIdentifier::TARGET;
			}
		}
	}//inputRaycastVisibleDatasets

	void TempRegViewer::inputClickSelectGeometryPoint(ViewportIdentifier ViewportUnderMouse) {
		if (ViewportUnderMouse == ViewportIdentifier::SEPARATE_SCENE_TEMPLATE || ViewportUnderMouse == ViewportIdentifier::SEPARATE_SCENE_FITTINGRES) {
			if (m_RayIntersectRes.Dataset == DatasetIdentifier::NONE) {
				m_TemplateDisplayData->hideSingleMarker();
				m_FittingResDisplayData->hideSingleMarker();
				m_CorrEditCreateSelection.TemplatePointSelected = false;
			}
			else {
				int32_t ClosestVertex = -1;

				if (m_RayIntersectRes.Dataset == DatasetIdentifier::TEMPLATE)
					ClosestVertex = m_TFitter.getTemplate().closestVertex(m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, 0.05f);

				if (m_RayIntersectRes.Dataset == DatasetIdentifier::FITTING_RESULT)
					ClosestVertex = m_TFitter.getFittingResult().closestVertex(m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, 0.05f);

				if (ClosestVertex < 0) {
					m_TemplateDisplayData->hideSingleMarker();
					m_FittingResDisplayData->hideSingleMarker();
					m_CorrEditCreateSelection.TemplatePointSelected = false;
				}
				else {
					const auto& TempVert = m_TFitter.getTemplate().vertexPosition(ClosestVertex);
					const auto& FitVert = m_TFitter.getFittingResult().vertexPosition(ClosestVertex);

					m_TemplateDisplayData->placeSingleMarker(m_MarkerActors[MarkerColor::GEOMETRY_SELECTION], TempVert);
					m_FittingResDisplayData->placeSingleMarker(m_MarkerActors[MarkerColor::GEOMETRY_SELECTION], FitVert);

					m_CorrEditCreateSelection.TemplatePointSelected = true;
					m_CorrEditCreateSelection.TemplatePointID = ClosestVertex;
					m_CorrEditCreateSelection.DistanceEuclidean = (m_CorrEditCreateSelection.TargetPointSelected) ? (FitVert - m_CorrEditCreateSelection.TargetPointPos).norm() : FLT_MAX;
				}
			}
		}

		if (ViewportUnderMouse == ViewportIdentifier::SEPARATE_SCENE_TARGET) {
			if (m_RayIntersectRes.Dataset == DatasetIdentifier::NONE) {
				m_TargetDisplayData->hideSingleMarker();
				m_CorrEditCreateSelection.TargetPointSelected = false;
			}
			else {
				if (m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::MESH) {
					m_TargetDisplayData->placeSingleMarker(m_MarkerActors[MarkerColor::GEOMETRY_SELECTION], m_RayIntersectRes.MeshIntersection);
					
					m_CorrEditCreateSelection.TargetPointSelected = true;
					m_CorrEditCreateSelection.TargetPointID = -1;
					m_CorrEditCreateSelection.TargetFace = m_RayIntersectRes.Face;
					m_CorrEditCreateSelection.TargetPointPos = m_RayIntersectRes.MeshIntersection;
					m_CorrEditCreateSelection.TargetPointNormal = Vector3f::Zero(); //TODO!
					m_CorrEditCreateSelection.DistanceEuclidean = (m_CorrEditCreateSelection.TemplatePointSelected) ? 
						(m_TFitter.getFittingResult().vertexPosition(m_CorrEditCreateSelection.TemplatePointID) - m_CorrEditCreateSelection.TargetPointPos).norm() : FLT_MAX;
				}
				else { // DatasetGeometries[m_RayIntersectRes.Dataset].geometryType() == TemplateFitter::GeometryType::POINTCLOUD
					if (m_RayIntersectRes.PclPoint < 0) {
						m_TargetDisplayData->hideSingleMarker();
						m_CorrEditCreateSelection.TargetPointSelected = false;
					}
					else {
						const auto& Target = m_TFitter.getTarget();
						const auto& TgtVert = Target.vertexPosition(m_RayIntersectRes.PclPoint);
						const auto& TgtVertN = Target.vertexNormal(m_RayIntersectRes.PclPoint);

						m_TargetDisplayData->placeSingleMarker(m_MarkerActors[MarkerColor::GEOMETRY_SELECTION], TgtVert);

						m_CorrEditCreateSelection.TargetPointSelected = true;
						m_CorrEditCreateSelection.TargetPointID = m_RayIntersectRes.PclPoint;
						m_CorrEditCreateSelection.TargetFace = -1;
						m_CorrEditCreateSelection.TargetPointPos = TgtVert;
						m_CorrEditCreateSelection.TargetPointNormal = TgtVertN;
						m_CorrEditCreateSelection.DistanceEuclidean = (m_CorrEditCreateSelection.TemplatePointSelected) ?
							(m_TFitter.getFittingResult().vertexPosition(m_CorrEditCreateSelection.TemplatePointID) - m_CorrEditCreateSelection.TargetPointPos).norm() : FLT_MAX;
					}
				}
			}
		}
	}//inputClickSelectGeometryPoint

	void TempRegViewer::inputClickSelectCorrespondence(ViewportIdentifier ViewportUnderMouse) {
		if (ViewportUnderMouse == ViewportIdentifier::SEPARATE_SCENE_TARGET || ViewportUnderMouse == ViewportIdentifier::NONE) return; // do nothing
		
		// new click, new selection -> undo last selection
		for (auto TempVertID : m_SelectedCorrsByTemplateID) {
			const auto& CorrData = m_TFitter.correspondenceData(TempVertID);
			if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
				auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];
				m_TemplateDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
				m_FittingResDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
				m_TargetDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
			}
			else {
				auto pMarkerActor = m_MarkerActors[MarkerColor::AUTOCORR_IDLE];
				m_TemplateDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
				m_FittingResDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
				m_TargetDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
			}
		}

		m_SelectedCorrsByTemplateID.clear();

		// get new selection
		int32_t ClosestVertex = -1;
		if (m_RayIntersectRes.Dataset == DatasetIdentifier::TEMPLATE) 
			ClosestVertex = m_TFitter.getTemplate().closestVertex(m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, 0.05f);

		if (m_RayIntersectRes.Dataset == DatasetIdentifier::FITTING_RESULT)
			ClosestVertex = m_TFitter.getFittingResult().closestVertex(m_RayIntersectRes.Face, m_RayIntersectRes.MeshIntersection, 0.05f);
		
		if (ClosestVertex >= 0) { // template vertex in range
			if (m_TFitter.hasCorrespondence(ClosestVertex)) { // does it have a correspondence?	
				m_SelectedCorrsByTemplateID.push_back(ClosestVertex); // correspondence found, select it
				const auto& CorrData = m_TFitter.correspondenceData(ClosestVertex);

				// and mark it
				if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
					auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_SELECTION];
					m_TemplateDisplayData->setFeatureCorrMarkerColor(ClosestVertex, pMarkerActor);
					m_FittingResDisplayData->setFeatureCorrMarkerColor(ClosestVertex, pMarkerActor);
					m_TargetDisplayData->setFeatureCorrMarkerColor(ClosestVertex, pMarkerActor);
				}
				else {
					auto pMarkerActor = m_MarkerActors[MarkerColor::AUTOCORR_SELECTION];
					m_TemplateDisplayData->setAutomaticCorrMarkerColor(ClosestVertex, pMarkerActor);
					m_FittingResDisplayData->setAutomaticCorrMarkerColor(ClosestVertex, pMarkerActor);
					m_TargetDisplayData->setAutomaticCorrMarkerColor(ClosestVertex, pMarkerActor);
				}
			}
		}
	}//inputClickSelectCorrespondence

	void TempRegViewer::inputCreateCorrespondence(void) {
		if (m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::MESH) {
			m_TFitter.createCorrespondenceToMesh(
				TemplateFitter::CorrespondenceType::FEATURE, 
				m_CorrEditCreateSelection.TemplatePointID,	
				m_CorrEditCreateSelection.TargetFace, 
				m_CorrEditCreateSelection.TargetPointPos, 
				m_CorrEditCreateSelection.TargetPointNormal);
		}
		else {
			m_TFitter.createCorrespondenceToPCL(TemplateFitter::CorrespondenceType::FEATURE, m_CorrEditCreateSelection.TemplatePointID, m_CorrEditCreateSelection.TargetPointID);
		}

		const auto& TempVert = m_TFitter.getTemplate().vertexPosition(m_CorrEditCreateSelection.TemplatePointID);
		const auto& FitVert = m_TFitter.getFittingResult().vertexPosition(m_CorrEditCreateSelection.TemplatePointID);
		auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];

		m_TemplateDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, TempVert, pMarkerActor);
		m_FittingResDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, FitVert, pMarkerActor);
		m_TargetDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, m_CorrEditCreateSelection.TargetPointPos, pMarkerActor);

		m_TemplateDisplayData->hideSingleMarker();
		m_FittingResDisplayData->hideSingleMarker();
		m_TargetDisplayData->hideSingleMarker();

		m_CorrEditCreateSelection.TemplatePointSelected = false;
		m_CorrEditCreateSelection.TargetPointSelected = false;
	}//inputCreateCorrespondence

	void TempRegViewer::inputOverwriteCorrespondence(void) {
		const auto& OldCorrData = m_TFitter.correspondenceData(m_CorrEditCreateSelection.TemplatePointID);

		// Remove old correspondence data, markers
		if (OldCorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
			m_TemplateDisplayData->removeFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
			m_FittingResDisplayData->removeFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
			m_TargetDisplayData->removeFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
		}
		else {
			m_TemplateDisplayData->removeAutomaticCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
			m_FittingResDisplayData->removeAutomaticCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
			m_TargetDisplayData->removeAutomaticCorrMarker(m_CorrEditCreateSelection.TemplatePointID);
		}

		m_TFitter.deleteCorrespondence(m_CorrEditCreateSelection.TemplatePointID);

		// Add new correspondence data, markers
		if (m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::MESH) {
			m_TFitter.createCorrespondenceToMesh(
				TemplateFitter::CorrespondenceType::FEATURE, 
				m_CorrEditCreateSelection.TemplatePointID, 
				m_CorrEditCreateSelection.TargetFace, 
				m_CorrEditCreateSelection.TargetPointPos, 
				m_CorrEditCreateSelection.TargetPointNormal);
		}
		else {
			m_TFitter.createCorrespondenceToPCL(TemplateFitter::CorrespondenceType::FEATURE, m_CorrEditCreateSelection.TemplatePointID, m_CorrEditCreateSelection.TargetPointID);
		}

		const auto& TempVert = m_TFitter.getTemplate().vertexPosition(m_CorrEditCreateSelection.TemplatePointID);
		const auto& FitVert = m_TFitter.getFittingResult().vertexPosition(m_CorrEditCreateSelection.TemplatePointID);
		auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];

		m_TemplateDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, TempVert, pMarkerActor);
		m_FittingResDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, FitVert, pMarkerActor);
		m_TargetDisplayData->addFeatureCorrMarker(m_CorrEditCreateSelection.TemplatePointID, m_CorrEditCreateSelection.TargetPointPos, pMarkerActor);

		m_TemplateDisplayData->hideSingleMarker();
		m_FittingResDisplayData->hideSingleMarker();
		m_TargetDisplayData->hideSingleMarker();

		m_CorrEditCreateSelection.TemplatePointSelected = false;
		m_CorrEditCreateSelection.TargetPointSelected = false;
	}//inputOverwriteCorrespondence

	void TempRegViewer::inputClearCorrespondences(TemplateFitter::CorrespondenceType CT) {
		if (CT == TemplateFitter::CorrespondenceType::NONE) {
			clearAllCorrMarkers();
			m_TFitter.clearCorrespondenceStorage();
			m_SelectedCorrsByTemplateID.clear();
		}
		else {
			if (CT == TemplateFitter::CorrespondenceType::FEATURE) {
				m_TemplateDisplayData->clearFeatureCorrMarkers();
				m_FittingResDisplayData->clearFeatureCorrMarkers();
				m_TargetDisplayData->clearFeatureCorrMarkers();
			}
			else {
				m_TemplateDisplayData->clearAutomaticCorrMarkers();
				m_FittingResDisplayData->clearAutomaticCorrMarkers();
				m_TargetDisplayData->clearAutomaticCorrMarkers();
			}

			for (auto TempVertID : m_SelectedCorrsByTemplateID) {
				const auto& CorrData = m_TFitter.correspondenceData(TempVertID);
				if (CorrData.Type == CT) {
					m_SelectedCorrsByTemplateID.erase(
						std::remove_if(m_SelectedCorrsByTemplateID.begin(), m_SelectedCorrsByTemplateID.end(), [TempVertID](uint32_t i) { return i == TempVertID; }), m_SelectedCorrsByTemplateID.end());
				}
			}

			m_TFitter.clearCorrespondenceStorage(CT);
		}
	}//inputClearCorrespondences

	void TempRegViewer::inputDeleteSelectedCorrespondences(void) {
		for (auto TempVertID : m_SelectedCorrsByTemplateID) {
			const auto& CorrData = m_TFitter.correspondenceData(TempVertID);

			if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
				m_TemplateDisplayData->removeFeatureCorrMarker(TempVertID);
				m_FittingResDisplayData->removeFeatureCorrMarker(TempVertID);
				m_TargetDisplayData->removeFeatureCorrMarker(TempVertID);
			}
			else {
				m_TemplateDisplayData->removeAutomaticCorrMarker(TempVertID);
				m_FittingResDisplayData->removeAutomaticCorrMarker(TempVertID);
				m_TargetDisplayData->removeAutomaticCorrMarker(TempVertID);
			}

			m_TFitter.deleteCorrespondence(TempVertID);
		}

		m_SelectedCorrsByTemplateID.clear();
	}//inputDeleteSelectedCorrespondences
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// GUI
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void TempRegViewer::guiViewerControlWindow(void) {

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));

		m_ViewerCtrlWinSize.x = float(m_RenderWin.width()) - m_SideMenuWinSize.x + 1;
		
		ImGuiWindowFlags ViewerControlWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;
				
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(m_ViewerCtrlWinSize);
		ImGui::Begin("##viewer_control_menu", nullptr, ViewerControlWindowFlags);
		ImGui::PopStyleVar(2);

		if (m_CorrEditActiveMode == CorrespondenceEditMode::DEACTIVATED) guiViewerButtonShowTemplateFitting();
		else guiViewerButtonShowCorrEdit();
		ImGui::SameLine();
		guiViewerButtonMeshVis();
		ImGui::SameLine();
		guiViewerButtonColorization();
		ImGui::SameLine();
		guiViewerButtonReset();
		ImGui::SameLine();

		ImGui::BeginDisabled();
		if (ImGui::Button("Hide GUI")) {
			//TODO
		}
		ImGui::EndDisabled();
		guiTooltip("/*TODO*/");
		
		ImGui::End();
	}//guiViewerControlWindow

	void TempRegViewer::guiSideMenuWindow(void) {
		m_SideMenuWinSize.y = float(m_RenderWin.height());

		ImGui::SetNextWindowPos(ImVec2(float(m_RenderWin.width()) - m_SideMenuWinSize.x, 0.0f));
		ImGui::SetNextWindowSize(m_SideMenuWinSize);

		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
		ImGui::Begin("##side_menu", nullptr, WindowFlags);

		if (m_CorrEditActiveMode == CorrespondenceEditMode::DEACTIVATED) guiSideMenuMainView();
		else guiSideMenuCorrEditView();

		ImGui::End();
	}//guiSideMenuWindow

	void TempRegViewer::guiSideMenuMainView(void) {
		if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_DefaultOpen)) {
			guiFileStateCheckboxes();
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			guiFileLoadButtons();
		}

		guiSpacedSeparator();

		if (ImGui::CollapsingHeader("Correspondence Data", ImGuiTreeNodeFlags_DefaultOpen)) {
			guiCorrespondenceDataSummary();
			ImGui::Dummy(ImVec2(0.0f, 2.0f));		
			guiSelectedCorrespondencePropertiesList();
			ImGui::Dummy(ImVec2(0.0f, 2.0f));
			guiEditCorrespondencesButton();
		}

		guiSpacedSeparator();

		if (ImGui::CollapsingHeader("Template Fitting", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::TreeNode("Overview")) {
				ImGui::Spacing();
								
				ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
				ImGui::BeginDisabled();

				std::string FeatCorrsReadyLabel;
				const size_t FeatCorrCount = m_TFitter.featureCorrespondences().size();

				if (FeatCorrCount < 3) {
					ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgRedCol);
					FeatCorrsReadyLabel = "Missing " + std::to_string(3 - FeatCorrCount) + " feature correspondences";
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgGreenCol);
					ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkGreenCol);
					FeatCorrsReadyLabel = std::to_string(FeatCorrCount) + " feature correspondences ready";
				}

				FeatCorrsReadyLabel.append("##featcorrs_avail");

				bool FeatureCorrsReady = (FeatCorrCount < 3) ? false : true;
				ImGui::Checkbox(FeatCorrsReadyLabel.c_str(), &FeatureCorrsReady);
				guiTooltip("The fitting process requires at least 3 user-defined correspondences between template and target to calculate a transformation.\n\nUse the correspondence editing menu (accessible via the \"Edit Correspondences >\" button) to create a set of correspondences.", ImGuiHoveredFlags_AllowWhenDisabled);

				ImGui::PopStyleColor((FeatCorrCount < 3) ? 1 : 2);
				ImGui::EndDisabled();
				ImGui::PopStyleVar();

				ImGui::Dummy(ImVec2(0.0f, 2.0f));

				//TODO: 
				// -> current registration error
				// -> current iteration / max iterations of fine fitting algorithm

				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::TreeNode("Coarse Fitting")) {
				ImGui::Spacing();

				const size_t FeatCorrCount = m_TFitter.featureCorrespondences().size();

				if (FeatCorrCount < 3) ImGui::BeginDisabled();
				if (ImGui::Checkbox("Scale Template", &m_CoarseFitScaleTemplate))
					m_VolComputeComboSelected = (m_CoarseFitScaleTemplate) ? TemplateFitter::VolumeComputation::FEATURE_AABBS : TemplateFitter::VolumeComputation::DISABLED;
				if (FeatCorrCount < 3) ImGui::EndDisabled();
				guiTooltip("/*TODO*/", ImGuiHoveredFlags_AllowWhenDisabled);

				ImGui::Dummy(ImVec2(0.0f, 2.0f));

				if (FeatCorrCount < 3) ImGui::BeginDisabled();
				if (ImGui::Button("Compute Alignment")) {
					// compute alignment and transform geometry
					m_TFitter.computeCoarseFitting(m_VolComputeComboSelected);

					// update display data of fitting result
					const auto& FittingResGeometry = m_TFitter.getFittingResult();
					m_FittingResDisplayData->updateModelData(FittingResGeometry.vertexPositions());

					const auto& FeatCorrs = m_TFitter.featureCorrespondences();
					for (auto& C : FeatCorrs) m_FittingResDisplayData->setFeatureCorrMarkerPosition(C, FittingResGeometry.vertexPosition(C));

					const auto& AutoCorrs = m_TFitter.automaticCorrespondences();
					for (auto& C : AutoCorrs) m_FittingResDisplayData->setAutomaticCorrMarkerPosition(C, FittingResGeometry.vertexPosition(C));
				}
				if (FeatCorrCount < 3) ImGui::EndDisabled();
				guiTooltip("Compute a coarse alignment between template and target.", ImGuiHoveredFlags_AllowWhenDisabled);

				ImGui::TreePop();
			}

			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::TreeNode("Fine Fitting")) {
				ImGui::Spacing();


				ImGui::TreePop();
			}
		}
	}//guiSideMenuMainView

	void TempRegViewer::guiFileStateCheckboxes(void) {
		ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
		ImGui::BeginDisabled();

		if (ImGui::BeginTable("##files_state_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
			bool TemplateActive = m_TFitter.templateActive();
			bool TargetActive = m_TFitter.targetActive();

			ImGui::TableNextColumn();

			if (TemplateActive) {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgGreenCol);
				ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkGreenCol);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgRedCol);
			}

			ImGui::Checkbox("Template ready", &TemplateActive);

			ImGui::PopStyleColor((TemplateActive) ? 2 : 1);

			ImGui::TableNextColumn();

			if (TargetActive) {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgGreenCol);
				ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkGreenCol);
			}
			else {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgRedCol);
			}

			ImGui::Checkbox("Target ready", &TargetActive);

			ImGui::PopStyleColor((TargetActive) ? 2 : 1);

			ImGui::EndTable();
		}

		ImGui::EndDisabled();
		ImGui::PopStyleVar();
	}//guiFileStateCheckboxes

	void TempRegViewer::guiFileLoadButtons(void) {
		if (ImGui::BeginTable("##load_buttons_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
			ImGui::TableNextColumn();

			if (ImGui::Button("Load Template", ImVec2(-FLT_MIN, 0.0f))) {
				//TODO
			}
			guiTooltip("/*TODO*/");

			ImGui::TableNextColumn();

			if (ImGui::Button("Load Target", ImVec2(-FLT_MIN, 0.0f))) {
				//TODO
			}
			guiTooltip("/*TODO*/");

			ImGui::EndTable();
		}

		if (ImGui::Button("Save Fitting Result", ImVec2(-FLT_MIN, 0.0f))) {
			//TODO
		}
		guiTooltip("/*TODO*/");
	}//guiFileLoadButtons

	void TempRegViewer::guiEditCorrespondencesButton(void) {
		if (ImGui::Button("Edit Correspondences >", ImVec2(-FLT_MIN, 0.0f))) {
			m_CorrEditActiveMode = CorrespondenceEditMode::VIEW_AND_DELETE;

			m_TemplateDisplayData->hideSingleMarker();
			m_FittingResDisplayData->hideSingleMarker();
			m_TargetDisplayData->hideSingleMarker();
			m_CorrEditCreateSelection.TemplatePointSelected = false;
			m_CorrEditCreateSelection.TargetPointSelected = false;

			for (auto TempVertID : m_SelectedCorrsByTemplateID) {
				const auto& CorrData = m_TFitter.correspondenceData(TempVertID);

				if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
					auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];
					m_TemplateDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
				}
				else {
					auto pMarkerActor = m_MarkerActors[MarkerColor::AUTOCORR_IDLE];
					m_TemplateDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
				}
			}
			m_SelectedCorrsByTemplateID.clear();

			m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
			m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
			m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TARGET);
			m_ActiveViewports.erase(ViewportIdentifier::SHARED_SCENE);
			resizeActiveViewports();
		}
		guiTooltip("/*TODO: explain what this button does; explain what \"feature correspondences\" are*/");
	}//guiEditCorrespondencesButton
	
	void TempRegViewer::guiSideMenuCorrEditView(void) {
		ImGui::PushID("corredit_view_");

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Correspondence Editing");

		float ExitButtonWidth = 100.0f; //ImGui::CalcTextSize("Exit >").x + (2.0f * ImGui::GetStyle().FramePadding.x); //<- actual width of button
		ImGui::SameLine(ImGui::GetContentRegionMax().x - ExitButtonWidth);

		guiCorrEditExitButton(ExitButtonWidth);

		//TODO: tabulator key based switching of correspondence tool modes:
		//TODO: query tab-based mode switching from ViewportInputManager
		//TODO: execute switch, retain or delete selections of formerly active mode?

		if (ImGui::BeginTable("##corredit_mode_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
			ImGui::TableNextColumn(); guiCorrEditViewDelRadioButton();
			ImGui::TableNextColumn(); guiCorrEditCreateRadioButton();
			ImGui::EndTable();
		}

		guiSpacedSeparator();

		ImGuiTabItemFlags ViewDelTabFlags = ImGuiTabItemFlags_NoReorder;
		ImGuiTabItemFlags CreateTabFlags = ImGuiTabItemFlags_NoReorder;

		guiCorrespondenceDataSummary();
		
		ImGui::Dummy(ImVec2(0.0f, 2.0f));

		if (m_CorrEditActiveMode == CorrespondenceEditMode::VIEW_AND_DELETE) guiViewDelPanel();
		if (m_CorrEditActiveMode == CorrespondenceEditMode::CREATE) guiCreatePanel();

		ImGui::PopID();
	}//guiSideMenuCorrEditView

	void TempRegViewer::guiCorrEditExitButton(float ExitButtonWidth) {
		if (ImGui::Button("Exit >", ImVec2(ExitButtonWidth, 0.0f))) {
			m_TemplateDisplayData->hideSingleMarker();
			m_FittingResDisplayData->hideSingleMarker();
			m_TargetDisplayData->hideSingleMarker();
			m_CorrEditCreateSelection.TemplatePointSelected = false;
			m_CorrEditCreateSelection.TargetPointSelected = false;

			for (auto TempVertID : m_SelectedCorrsByTemplateID) {
				const auto& CorrData = m_TFitter.correspondenceData(TempVertID);

				if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
					auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];
					m_TemplateDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
				}
				else {
					auto pMarkerActor = m_MarkerActors[MarkerColor::AUTOCORR_IDLE];
					m_TemplateDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
				}
			}

			m_SelectedCorrsByTemplateID.clear();
			m_CorrEditActiveMode = CorrespondenceEditMode::DEACTIVATED;
		}
	}//guiCorrEditExitButton

	void TempRegViewer::guiCorrEditViewDelRadioButton(void) {
		if (ImGui::RadioButton("View / Delete##mode_selector_viewdel", (m_CorrEditActiveMode == CorrespondenceEditMode::VIEW_AND_DELETE))) {
			m_CorrEditActiveMode = CorrespondenceEditMode::VIEW_AND_DELETE;
			m_TemplateDisplayData->hideSingleMarker();
			m_FittingResDisplayData->hideSingleMarker();
			m_TargetDisplayData->hideSingleMarker();
			m_CorrEditCreateSelection.TemplatePointSelected = false;
			m_CorrEditCreateSelection.TargetPointSelected = false;
		}
	}//guiCorrEditViewDelRadioButton

	void TempRegViewer::guiCorrEditCreateRadioButton(void) {
		if (ImGui::RadioButton("Create##mode_selector_create", (m_CorrEditActiveMode == CorrespondenceEditMode::CREATE))) {
			m_CorrEditActiveMode = CorrespondenceEditMode::CREATE;

			for (auto TempVertID : m_SelectedCorrsByTemplateID) {
				const auto& CorrData = m_TFitter.correspondenceData(TempVertID);

				if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) {
					auto pMarkerActor = m_MarkerActors[MarkerColor::FEATURECORR_IDLE];
					m_TemplateDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setFeatureCorrMarkerColor(TempVertID, pMarkerActor);
				}
				else {
					auto pMarkerActor = m_MarkerActors[MarkerColor::AUTOCORR_IDLE];
					m_TemplateDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_FittingResDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
					m_TargetDisplayData->setAutomaticCorrMarkerColor(TempVertID, pMarkerActor);
				}
			}

			m_SelectedCorrsByTemplateID.clear();
		}
	}//guiCorrEditCreateRadioButton

	void TempRegViewer::guiViewDelPanel(void) {
		ImGui::PushID("viewdel_panel_");

		guiSelectedCorrespondencePropertiesList();

		guiSpacedSeparator();

		// Buttons for delete operations

		if (ImGui::BeginTable("##delete_buttons_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
			const size_t FeatCorrCount = m_TFitter.featureCorrespondences().size();
			const size_t AutoCorrCount = m_TFitter.automaticCorrespondences().size();
			const size_t TotalCorrCount = FeatCorrCount + AutoCorrCount;

			//	"Selected" button
			ImGui::TableNextColumn();
			guiDeleteSelectedButton();

			// "All" button
			ImGui::TableNextColumn();
			guiDeleteAllButton(TotalCorrCount);

			// "Feature" button
			ImGui::TableNextColumn();
			guiDeleteFeatureButton(FeatCorrCount);

			// "Automatic" button
			ImGui::TableNextColumn();
			guiDeleteAutomaticButton(AutoCorrCount);

			ImGui::EndTable();
		}

		ImGui::PopID();
	}//guiViewDelPanel

	void TempRegViewer::guiDeleteSelectedButton(void) {
		if (m_SelectedCorrsByTemplateID.empty()) {
			ImGui::BeginDisabled();
			ImGui::Button("Delete Selected##delete_select_corr", ImVec2(-FLT_MIN, 0.0f));
			ImGui::EndDisabled();
		}
		else {
			if (ImGui::Button("Delete Selected##delete_select_corr", ImVec2(-FLT_MIN, 0.0f)))
				ImGui::OpenPopup("Delete Selected Correspondence##del_select_corr_popup_bybutton");

			ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

			//
			// Popup window
			//
			if (ImGui::BeginPopupModal("Delete Selected Correspondence##del_select_corr_popup_bybutton", nullptr, ImGuiWindowFlags_NoResize)) {
				float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

				ImGui::TextWrapped("Are you sure you wish to delete the selected correspondence?");

				ImGui::Dummy(ImVec2(0.0f, 20.0f));

				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

				if (ImGui::Button("Confirm##yes_del_select_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) {
					inputDeleteSelectedCorrespondences();
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##no_del_select_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}// end popup window
		}
	}//guiDeleteSelectedButton
	
	void TempRegViewer::guiDeleteAllButton(size_t TotalCorrCount) {
		if (TotalCorrCount == 0) {
			ImGui::BeginDisabled();
			ImGui::Button("Delete All##delete_all_corr", ImVec2(-FLT_MIN, 0.0f));
			ImGui::EndDisabled();
		}
		else {
			if (ImGui::Button("Delete All##delete_all_corr", ImVec2(-FLT_MIN, 0.0f)))
				ImGui::OpenPopup("Delete All Correspondences##del_all_corr_popup_bybutton");

			ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

			//
			// Popup window
			//
			if (ImGui::BeginPopupModal("Delete All Correspondences##del_all_corr_popup_bybutton", nullptr, ImGuiWindowFlags_NoResize)) {
				float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

				ImGui::TextWrapped("Are you sure you wish to delete all currently active correspondences?");

				ImGui::Dummy(ImVec2(0.0f, 20.0f));

				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

				if (ImGui::Button("Confirm##yes_del_all_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) {
					inputClearCorrespondences();
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##no_del_all_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}// end popup window
		}
	}//guiDeleteAllButton
	
	void TempRegViewer::guiDeleteFeatureButton(size_t FeatCorrCount) {
		if (FeatCorrCount == 0) {
			ImGui::BeginDisabled();
			if (ImGui::Button("Delete Feature##delete_feat_corr", ImVec2(-FLT_MIN, 0.0f)));
			ImGui::EndDisabled();
		}
		else {
			if (ImGui::Button("Delete Feature##delete_feat_corr", ImVec2(-FLT_MIN, 0.0f)))
				ImGui::OpenPopup("Delete Feature Correspondences##del_feat_corr_popup_bybutton");

			ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

			//
			// Popup window
			//
			if (ImGui::BeginPopupModal("Delete Feature Correspondences##del_feat_corr_popup_bybutton", nullptr, ImGuiWindowFlags_NoResize)) {
				float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

				ImGui::TextWrapped("Are you sure you wish to delete all currently active feature correspondences?");

				ImGui::Dummy(ImVec2(0.0f, 20.0f));

				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

				if (ImGui::Button("Confirm##yes_del_feat_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) {
					inputClearCorrespondences(TemplateFitter::CorrespondenceType::FEATURE);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##no_del_feat_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}// end popup window
		}
	}//guiDeleteFeatureButton
	
	void TempRegViewer::guiDeleteAutomaticButton(size_t AutoCorrCount) {
		if (AutoCorrCount == 0) {
			ImGui::BeginDisabled();
			if (ImGui::Button("Delete Automatic##delete_auto_corr", ImVec2(-FLT_MIN, 0.0f)));
			ImGui::EndDisabled();
		}
		else {
			if (ImGui::Button("Delete Automatic##delete_auto_corr", ImVec2(-FLT_MIN, 0.0f)))
				ImGui::OpenPopup("Delete Automatic Correspondences##del_auto_corr_popup_bybutton");

			ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

			//
			// Popup window
			//
			if (ImGui::BeginPopupModal("Delete Automatic Correspondences##del_auto_corr_popup_bybutton", nullptr, ImGuiWindowFlags_NoResize)) {
				float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

				ImGui::TextWrapped("Are you sure you wish to delete all currently active automatic correspondences?");

				ImGui::Dummy(ImVec2(0.0f, 20.0f));

				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

				if (ImGui::Button("Confirm##yes_del_auto_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) {
					inputClearCorrespondences(TemplateFitter::CorrespondenceType::AUTOMATIC);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##no_del_auto_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}// end popup window
		}
	}//guiDeleteAutomaticButton

	void TempRegViewer::guiCreatePanel(void) {
		ImGui::PushID("create_panel_");

		guiSelectedGeometryPointsPropertiesList();

		guiSpacedSeparator();

		if (ImGui::BeginTable("##create_buttons_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
			// "Create" / "Overwrite" button
			ImGui::TableNextColumn();
			guiCreateAndOverwriteButton();
			
			// "Clear" button
			ImGui::TableNextColumn();
			guiClearAllGeometryPointSelectionButton();

			// "Clear Template" button
			ImGui::TableNextColumn();
			guiClearTemplateGeometryPointSelectionButton();

			// "Clear Target" button
			ImGui::TableNextColumn();
			guiClearTargetGeometryPointSelectionButton();			

			ImGui::EndTable();
		}
		
		ImGui::PopID();
	}//guiCreatePanel

	void TempRegViewer::guiSelectedGeometryPointsPropertiesList(void) {
		// State of current selection
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		if (ImGui::TreeNode("Current Selection")) {
			ImGui::Spacing();

			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
			ImGui::BeginDisabled();

			if (ImGui::BeginTable("##create_selection_state_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {

				ImGui::TableNextColumn();

				if (m_CorrEditCreateSelection.TemplatePointSelected) {
					if (!m_TFitter.hasCorrespondence(m_CorrEditCreateSelection.TemplatePointID)) {
						ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgGreenCol);
						ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkGreenCol);

						ImGui::Checkbox("Template selection", &m_CorrEditCreateSelection.TemplatePointSelected);
						guiTooltip("/*TODO: tooltip for TemplateVertSelected == true, hasCorrespondence == false*/");
					}
					else {
						ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgYellowCol);
						ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkYellowCol);

						ImGui::Checkbox("Template selection", &m_CorrEditCreateSelection.TemplatePointSelected);
						guiTooltip("/*TODO: tooltip for TemplateVertSelected == true, hasCorrespondence == true*/");
					}

					ImGui::PopStyleColor(2);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgRedCol);

					ImGui::Checkbox("Template selection", &m_CorrEditCreateSelection.TemplatePointSelected);
					guiTooltip("/*TODO: tooltip for TemplateVertSelected == false*/");

					ImGui::PopStyleColor();
				}

				ImGui::TableNextColumn();

				if (m_CorrEditCreateSelection.TargetPointSelected) {
					ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgGreenCol);
					ImGui::PushStyleColor(ImGuiCol_CheckMark, m_CheckMarkGreenCol);

					ImGui::Checkbox("Target selection", &m_CorrEditCreateSelection.TargetPointSelected);
					guiTooltip("/*TODO: tooltip for TargetPointSelected == true*/");

					ImGui::PopStyleColor(2);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_FrameBg, m_CheckboxBgRedCol);

					ImGui::Checkbox("Target selection", &m_CorrEditCreateSelection.TargetPointSelected);
					guiTooltip("/*TODO: tooltip for TargetPointSelected == false*/");

					ImGui::PopStyleColor();
				}

				ImGui::EndTable();
			}

			if (m_CorrEditCreateSelection.TemplatePointSelected) {
				if (m_TFitter.hasCorrespondence(m_CorrEditCreateSelection.TemplatePointID)) {
					const auto& CorrData = m_TFitter.correspondenceData(m_CorrEditCreateSelection.TemplatePointID);
					if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) ImGui::TextWrapped("Warning: Collision with existing feature correspondence at selected template vertex!");
					else ImGui::TextWrapped("Warning: Collision with existing automatic correspondence at selected template vertex!");
				}
			}

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::Text("Euclidean Distance:");

			if (!m_CorrEditCreateSelection.TemplatePointSelected || !m_CorrEditCreateSelection.TargetPointSelected) {
				char EuclidDistBuf[4] = "---";
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##eucliddist_textbox", EuclidDistBuf, 4, ImGuiInputTextFlags_ReadOnly);
				guiTooltip("Correspondence incomplete!", ImGuiHoveredFlags_AllowWhenDisabled);
			}
			else {
				std::string EuclidDistString = guiFloatToTextLabel(m_CorrEditCreateSelection.DistanceEuclidean);
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##eucliddist_textbox", &EuclidDistString, ImGuiInputTextFlags_ReadOnly);
				guiTooltip("Euclidean distance between selected points", ImGuiHoveredFlags_AllowWhenDisabled);
			}

			//
			//TODO: other properties of a potential correspondence between the selected points here...
			//

			ImGui::EndDisabled();
			ImGui::PopStyleVar();

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			ImGui::TextWrapped("(i) Left click on datasets to select a template vertex / target point.");
			ImGui::PopStyleColor();

			ImGui::TreePop();
		}
	}//guiSelectedGeometryPointsPropertiesList

	void TempRegViewer::guiCreateAndOverwriteButton(void) {
		bool UseOverwriteButton = false;
		if (m_CorrEditCreateSelection.TemplatePointSelected)
			if (m_TFitter.hasCorrespondence(m_CorrEditCreateSelection.TemplatePointID))
				UseOverwriteButton = true;

		if (UseOverwriteButton) {
			bool DisableButton = (!m_CorrEditCreateSelection.TargetPointSelected) ? true : false;

			if (DisableButton) ImGui::BeginDisabled();

			if (ImGui::Button("Overwrite##overwrite_corr", ImVec2(-FLT_MIN, 0.0f))) ImGui::OpenPopup("Overwrite Correspondence##overwrite_corr_popup_bybutton");

			if (DisableButton) {
				ImGui::EndDisabled();
				guiTooltip("No target point selected!", ImGuiHoveredFlags_AllowWhenDisabled);
			}

			ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

			//
			// Popup window
			//
			if (ImGui::BeginPopupModal("Overwrite Correspondence##overwrite_corr_popup_bybutton", nullptr, ImGuiWindowFlags_NoResize)) {
				float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

				ImGui::TextWrapped("Are you sure you wish to overwrite the current correspondence of the selected template vertex?");

				ImGui::Dummy(ImVec2(0.0f, 20.0f));

				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

				if (ImGui::Button("Confirm##yes_overwrite_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) {
					inputOverwriteCorrespondence();
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##no_overwrite_corr_bybutton", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}// end popup window

		}
		else {
			bool DisableButton = (!m_CorrEditCreateSelection.TemplatePointSelected || !m_CorrEditCreateSelection.TargetPointSelected) ? true : false;

			if (DisableButton) ImGui::BeginDisabled();

			if (ImGui::Button("Create##create_corr", ImVec2(-FLT_MIN, 0.0f))) inputCreateCorrespondence();

			if (DisableButton) {
				ImGui::EndDisabled();

				std::string TooltipText;

				if (!m_CorrEditCreateSelection.TemplatePointSelected) TooltipText.append("No template vertex selected!\n");
				if (!m_CorrEditCreateSelection.TargetPointSelected) TooltipText.append("No target point selected!");

				guiTooltip(TooltipText.c_str(), ImGuiHoveredFlags_AllowWhenDisabled);
			}
		}
	}//guiCreateAndOverwriteButton

	void TempRegViewer::guiClearAllGeometryPointSelectionButton(void) {
		if (m_CorrEditCreateSelection.TemplatePointSelected || m_CorrEditCreateSelection.TargetPointSelected) {
			if (ImGui::Button("Clear##clear_both_select", ImVec2(-FLT_MIN, 0.0f))) {
				m_TemplateDisplayData->hideSingleMarker();
				m_FittingResDisplayData->hideSingleMarker();
				m_TargetDisplayData->hideSingleMarker();
				m_CorrEditCreateSelection.TemplatePointSelected = false;
				m_CorrEditCreateSelection.TargetPointSelected = false;
			}
		}
		else {
			ImGui::BeginDisabled();
			ImGui::Button("Clear##clear_both_select_disabled", ImVec2(-FLT_MIN, 0.0f));
			ImGui::EndDisabled();
		}
		guiTooltip("Clear selections on both datasets", ImGuiHoveredFlags_AllowWhenDisabled);
	}//guiClearAllGeometryPointSelectionButton

	void TempRegViewer::guiClearTemplateGeometryPointSelectionButton(void) {
		if (m_CorrEditCreateSelection.TemplatePointSelected) {
			if (ImGui::Button("Clear Template##clear_template_select", ImVec2(-FLT_MIN, 0.0f))) {
				m_TemplateDisplayData->hideSingleMarker();
				m_FittingResDisplayData->hideSingleMarker();
				m_CorrEditCreateSelection.TemplatePointSelected = false;
			}
		}
		else {
			ImGui::BeginDisabled();
			ImGui::Button("Clear Template##clear_template_select_disabled", ImVec2(-FLT_MIN, 0.0f));
			ImGui::EndDisabled();
		}
		guiTooltip("/*Clear selection on template dataset*/", ImGuiHoveredFlags_AllowWhenDisabled);
	}//guiClearTemplateGeometryPointSelectionButton

	void TempRegViewer::guiClearTargetGeometryPointSelectionButton(void) {
		if (m_CorrEditCreateSelection.TargetPointSelected) {
			if (ImGui::Button("Clear Target##clear_target_select", ImVec2(-FLT_MIN, 0.0f))) {
				m_TargetDisplayData->hideSingleMarker();
				m_CorrEditCreateSelection.TargetPointSelected = false;
			}
		}
		else {
			ImGui::BeginDisabled();
			ImGui::Button("Clear Target##clear_target_select_disabled", ImVec2(-FLT_MIN, 0.0f));
			ImGui::EndDisabled();
		}
		guiTooltip("/*Clear selection on target dataset*/", ImGuiHoveredFlags_AllowWhenDisabled);
	}//guiClearTargetGeometryPointSelectionButton

	void TempRegViewer::guiViewerButtonShowTemplateFitting(void) {

		if (ImGui::Button("Show")) ImGui::OpenPopup("viewer_show_popup");

		if (ImGui::BeginPopup("viewer_show_popup", ImGuiWindowFlags_AlwaysAutoResize)) {

			bool SharedSceneActive = (m_ActiveViewports.count(ViewportIdentifier::SHARED_SCENE) > 0) ? true : false;
			bool ShowTemplate = false;
			bool ShowFittingRes = false;
			bool ShowTarget = false;

			if (!SharedSceneActive) {
				ShowTemplate = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE) > 0) ? true : false;
				ShowFittingRes = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES) > 0) ? true : false;
				ShowTarget = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TARGET) > 0) ? true : false;
			}
			else {
				ShowTemplate = m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible;
				ShowFittingRes = m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible;
				ShowTarget = m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible;
			}

			if (ImGui::BeginTable("##viewer_show_popup_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {

				ImGui::TableNextColumn();

				if (ImGui::Checkbox("Template", &ShowTemplate)) {
					if (!SharedSceneActive) { // separate scenes active
						if (ShowTemplate) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
						else m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
						resizeActiveViewports();
					}

					if (SharedSceneActive) // shared scene active
						m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible = ShowTemplate;
				}

				if (ImGui::Checkbox("Target", &ShowTarget)) {
					if (!SharedSceneActive) { // separate scenes active
						if (ShowTarget) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TARGET);
						else m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_TARGET);
						resizeActiveViewports();
					}

					if (SharedSceneActive) // shared scene active
						m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible = ShowTarget;
				}

				if (ImGui::Checkbox("Fitting Result", &ShowFittingRes)) {
					if (!SharedSceneActive) { // separate scenes active
						if (ShowFittingRes) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
						else m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
						resizeActiveViewports();
					}

					if (SharedSceneActive) // shared scene active
						m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible = ShowFittingRes;
				}

				ImGui::TableNextColumn();

				if (ImGui::RadioButton("Separate Scenes", !SharedSceneActive)) {
					m_ActiveViewports.erase(ViewportIdentifier::SHARED_SCENE);

					if (ShowTemplate) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
					if (ShowFittingRes) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
					if (ShowTarget) m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TARGET);
					
					if (m_ActiveViewports.size() > 0) resizeActiveViewports();
				}

				if (ImGui::RadioButton("Shared Scene", SharedSceneActive)) {
					if (ShowTemplate) m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
					if (ShowFittingRes) m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
					if (ShowTarget) m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_TARGET);

					m_ActiveViewports.insert(ViewportIdentifier::SHARED_SCENE);

					m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible = ShowTemplate;
					m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible = ShowFittingRes;
					m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible = ShowTarget;

					resizeActiveViewports();
				}

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}
	}//guiViewerButtonShowTemplateFitting

	void TempRegViewer::guiViewerButtonShowCorrEdit(void) {

		if (ImGui::Button("Show")) ImGui::OpenPopup("viewer_show_popup");

		if (ImGui::BeginPopup("viewer_show_popup")) {

			bool ShowTemplate = m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
			bool ShowFittingRes = m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);

			if (ImGui::RadioButton("Template", ShowTemplate)) {
				m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
				m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
				resizeActiveViewports();
			}

			if (ImGui::RadioButton("Fitting Result", ShowFittingRes)) {
				m_ActiveViewports.erase(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
				m_ActiveViewports.insert(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
				resizeActiveViewports();
			}

			ImGui::EndPopup();
		}
	}//guiViewerButtonShowCorrEdit

	void TempRegViewer::guiViewerButtonMeshVis(void) {

		if (ImGui::Button("Mesh Visualization")) ImGui::OpenPopup("viewer_mesh_vis");

		if (ImGui::BeginPopup("viewer_mesh_vis")) {

			if (ImGui::BeginTable("##viewer_mesh_vis_popup_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchProp)) {
				bool SharedSceneActive = (m_ActiveViewports.count(ViewportIdentifier::SHARED_SCENE) > 0) ? true : false;
				bool ShowTemplate = false;
				bool ShowFittingRes = false;
				bool ShowTarget = false;

				if (!SharedSceneActive) {
					ShowTemplate = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE) > 0) ? true : false;
					ShowFittingRes = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES) > 0) ? true : false;
					ShowTarget = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TARGET) > 0) ? true : false;
				}
				else {
					ShowTemplate = m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible;
					ShowFittingRes = m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible;
					ShowTarget = m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible;
				}

				if (!ShowTemplate) ImGui::BeginDisabled();
				
				bool ShowTemplateSurface = m_TemplateDisplayData->showDatasetAsSurface();
				bool ShowTemplateWireframe = m_TemplateDisplayData->showDatasetAsWireframe();

				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Template:");

				ImGui::TableNextColumn();
				if (ImGui::Checkbox("Surface##template", &ShowTemplateSurface))
					m_TemplateDisplayData->showDatasetAsSurface(ShowTemplateSurface);

				ImGui::SameLine();

				if (ImGui::Checkbox("Wireframe##template", &ShowTemplateWireframe))
					m_TemplateDisplayData->showDatasetAsWireframe(ShowTemplateWireframe);

				if (!ShowTemplate) ImGui::EndDisabled();

				if (!ShowTarget || m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::POINTCLOUD) ImGui::BeginDisabled();

				bool ShowTargetSurface = m_TargetDisplayData->showDatasetAsSurface();
				bool ShowTargetWireframe = m_TargetDisplayData->showDatasetAsWireframe();
				
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Target:");

				ImGui::TableNextColumn();

				if (ImGui::Checkbox("Surface##target", &ShowTargetSurface))
					m_TargetDisplayData->showDatasetAsSurface(ShowTargetSurface);

				ImGui::SameLine();

				if (ImGui::Checkbox("Wireframe##target", &ShowTargetWireframe))
					m_TargetDisplayData->showDatasetAsWireframe(ShowTargetWireframe);

				if (!ShowTarget || m_TFitter.targetGeometryType() == TemplateFitter::GeometryType::POINTCLOUD) ImGui::EndDisabled();

				if (!ShowFittingRes) ImGui::BeginDisabled();

				bool ShowDTemplateSurface = m_FittingResDisplayData->showDatasetAsSurface();
				bool ShowDTemplateWireframe = m_FittingResDisplayData->showDatasetAsWireframe();

				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Fitting Result:");

				ImGui::TableNextColumn();

				if (ImGui::Checkbox("Surface##dtemplate", &ShowDTemplateSurface))
					m_FittingResDisplayData->showDatasetAsSurface(ShowDTemplateSurface);
					
				ImGui::SameLine();

				if (ImGui::Checkbox("Wireframe##dtemplate", &ShowDTemplateWireframe))
					m_FittingResDisplayData->showDatasetAsWireframe(ShowDTemplateWireframe);

				if (!ShowFittingRes) ImGui::EndDisabled();

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}
	}//guiViewerButtonMeshVis

	void TempRegViewer::guiViewerButtonColorization(void) {

		if (ImGui::Button("Dataset Colors")) ImGui::OpenPopup("viewer_dataset_colors");

		if (ImGui::BeginPopup("viewer_dataset_colors")) { //TODO: add color picker widget and pass chosen color to ViewportRenderManager; add way to reset color of dataset back to default?

			if (ImGui::BeginTable("##viewer_dataset_colors_popup_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchProp)) {
				bool SharedSceneActive = (m_ActiveViewports.count(ViewportIdentifier::SHARED_SCENE) > 0) ? true : false;
				bool ShowTemplate = false;
				bool ShowFittingRes = false;
				bool ShowTarget = false;

				if (!SharedSceneActive) {
					ShowTemplate = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE) > 0) ? true : false;
					ShowFittingRes = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES) > 0) ? true : false;
					ShowTarget = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TARGET) > 0) ? true : false;
				}
				else {
					ShowTemplate = m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible;
					ShowFittingRes = m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible;
					ShowTarget = m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible;
				}

				if (!ShowTemplate) ImGui::BeginDisabled();

				const DatasetDisplayData::DatasetColor ActiveTemplateCol = m_TemplateDisplayData->activeDatasetColor();
				
				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Template:");

				ImGui::TableNextColumn();

				if (ImGui::RadioButton("Solid##template", (ActiveTemplateCol == DatasetDisplayData::DatasetColor::SOLID_COLOR)))
					m_TemplateDisplayData->activateSolidColor();
					
				ImGui::SameLine();

				if (ImGui::RadioButton("Fitting Error##template", (ActiveTemplateCol == DatasetDisplayData::DatasetColor::FITTING_ERROR_COLOR)))
					m_TemplateDisplayData->activateFittingErrorColors();
					
				if (!ShowTemplate) ImGui::EndDisabled();
				if (!ShowTarget) ImGui::BeginDisabled();

				const DatasetDisplayData::DatasetColor ActiveTargetCol = m_TargetDisplayData->activeDatasetColor();

				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Target:");

				ImGui::TableNextColumn();

				if (ImGui::RadioButton("Solid##dtemplate", (ActiveTargetCol == DatasetDisplayData::DatasetColor::SOLID_COLOR)))
					m_TargetDisplayData->activateSolidColor();
					
				ImGui::SameLine();

				if (ImGui::RadioButton("Fitting Error##dtemplate", (ActiveTargetCol == DatasetDisplayData::DatasetColor::FITTING_ERROR_COLOR)))
					m_TargetDisplayData->activateFittingErrorColors();
					
				if (!ShowTarget) ImGui::EndDisabled();
				if (!ShowFittingRes) ImGui::BeginDisabled();

				const DatasetDisplayData::DatasetColor ActiveDTemplateCol = m_FittingResDisplayData->activeDatasetColor();

				ImGui::TableNextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Fitting Result:");

				ImGui::TableNextColumn();

				if (ImGui::RadioButton("Solid##dtemplate", (ActiveDTemplateCol == DatasetDisplayData::DatasetColor::SOLID_COLOR)))
					m_FittingResDisplayData->activateSolidColor();

				ImGui::SameLine();

				if (ImGui::RadioButton("Fitting Error##dtemplate", (ActiveDTemplateCol == DatasetDisplayData::DatasetColor::FITTING_ERROR_COLOR)))
					m_FittingResDisplayData->activateFittingErrorColors();

				if (!ShowFittingRes) ImGui::EndDisabled();

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}
	}//guiViewerButtonColorization

	void TempRegViewer::guiViewerButtonReset(void) {

		if (ImGui::Button("Reset")) ImGui::OpenPopup("viewer_reset");

		if (ImGui::BeginPopup("viewer_reset", ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginTable("##viewer_reset_popup_layout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame)) {
				bool SharedSceneActive = (m_ActiveViewports.count(ViewportIdentifier::SHARED_SCENE) > 0) ? true : false;
				bool ShowTemplate = false;
				bool ShowFittingRes = false;
				bool ShowTarget = false;

				if (!SharedSceneActive) {
					ShowTemplate = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE) > 0) ? true : false;
					ShowFittingRes = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES) > 0) ? true : false;
					ShowTarget = (m_ActiveViewports.count(ViewportIdentifier::SEPARATE_SCENE_TARGET) > 0) ? true : false;
				}
				else {
					ShowTemplate = m_Viewports[ViewportIdentifier::SHARED_SCENE].TemplateVisible;
					ShowFittingRes = m_Viewports[ViewportIdentifier::SHARED_SCENE].FittingResVisible;
					ShowTarget = m_Viewports[ViewportIdentifier::SHARED_SCENE].TargetVisible;
				}

				ImVec2 ResetButtonSize(ImGui::CalcTextSize(" Fitting Result ").x + (8 * ImGui::GetStyle().FramePadding.x), 0.0f);

				if (SharedSceneActive) {
					ImGui::TableNextColumn();
					ImGui::Text("Camera:");

					ImGui::TableNextColumn();
					ImGui::Text("Dataset Rotations:");

					ImGui::TableNextColumn();
					if (ImGui::Button("Reset##reset_shared_scene_cam", ResetButtonSize)) resetViewportCam(ViewportIdentifier::SHARED_SCENE);

					ImGui::TableNextColumn();
					if (!ShowTemplate && !ShowTarget && !ShowFittingRes) ImGui::BeginDisabled();
					if (ImGui::Button("Reset##reset_shared_scene_rot", ResetButtonSize)) m_Viewports[ViewportIdentifier::SHARED_SCENE].SceneViewingTrans.rotation(Quaternionf::Identity());
					if (!ShowTemplate && !ShowTarget && !ShowFittingRes) ImGui::EndDisabled();
				}
				else {
					ImGui::TableNextColumn();
					ImGui::Text("Cameras:");

					ImGui::TableNextColumn();
					ImGui::Text("Dataset Rotations:");

					ImGui::TableNextColumn();
					if (m_ActiveViewports.size() == 0) ImGui::BeginDisabled();
					if (ImGui::Button("All##reset_all_separate_cams", ResetButtonSize)) for (auto VP : m_ActiveViewports) resetViewportCam(VP);
					if (m_ActiveViewports.size() == 0) ImGui::EndDisabled();

					if (!ShowTemplate) ImGui::BeginDisabled();
					if (ImGui::Button("Template##reset_template_cam", ResetButtonSize)) resetViewportCam(ViewportIdentifier::SEPARATE_SCENE_TEMPLATE);
					if (!ShowTemplate) ImGui::EndDisabled();

					if (!ShowTarget) ImGui::BeginDisabled();
					if (ImGui::Button("Target##reset_target_cam", ResetButtonSize)) resetViewportCam(ViewportIdentifier::SEPARATE_SCENE_TARGET);
					if (!ShowTarget) ImGui::EndDisabled();

					if (!ShowFittingRes) ImGui::BeginDisabled();
					if (ImGui::Button("Fitting Result##reset_dtemplate_cam", ResetButtonSize)) resetViewportCam(ViewportIdentifier::SEPARATE_SCENE_FITTINGRES);
					if (!ShowFittingRes) ImGui::EndDisabled();

					ImGui::TableNextColumn();
					if (!ShowTemplate && !ShowTarget && !ShowFittingRes) ImGui::BeginDisabled();
					if (ImGui::Button("All##reset_all_separate_rot", ResetButtonSize)) {
						if (ShowTemplate) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TEMPLATE].SceneViewingTrans.rotation(Quaternionf::Identity());
						if (ShowFittingRes) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_FITTINGRES].SceneViewingTrans.rotation(Quaternionf::Identity());
						if (ShowTarget) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TARGET].SceneViewingTrans.rotation(Quaternionf::Identity());
					}
					if (!ShowTemplate && !ShowTarget && !ShowFittingRes) ImGui::EndDisabled();

					if (!ShowTemplate) ImGui::BeginDisabled();
					if (ImGui::Button("Template##reset_template_rot", ResetButtonSize)) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TEMPLATE].SceneViewingTrans.rotation(Quaternionf::Identity());
					if (!ShowTemplate) ImGui::EndDisabled();

					if (!ShowTarget) ImGui::BeginDisabled();
					if (ImGui::Button("Target##reset_target_rot", ResetButtonSize)) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_TARGET].SceneViewingTrans.rotation(Quaternionf::Identity());
					if (!ShowTarget) ImGui::EndDisabled();

					if (!ShowFittingRes) ImGui::BeginDisabled();
					if (ImGui::Button("Fitting Result##reset_dtemplate_rot", ResetButtonSize)) m_Viewports[ViewportIdentifier::SEPARATE_SCENE_FITTINGRES].SceneViewingTrans.rotation(Quaternionf::Identity());
					if (!ShowFittingRes) ImGui::EndDisabled();
				}

				ImGui::EndTable();
			}

			ImGui::EndPopup();
		}
	}//guiViewerButtonReset

	void TempRegViewer::guiCorrEditViewDeletePopupOnKeyPress(void) {
		if (m_CorrEditDeleteOnKeyPress) {
			m_CorrEditDeleteOnKeyPress = false;
			ImGui::OpenPopup("Delete Selected Correspondence##del_select_corr_popup_keypress");
		}

		ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

		//
		// Popup window
		//
		if (ImGui::BeginPopupModal("Delete Selected Correspondence##del_select_corr_popup_keypress", nullptr, ImGuiWindowFlags_NoResize)) {
			float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

			ImGui::TextWrapped("Are you sure you wish to delete the selected correspondence?");

			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

			if (ImGui::Button("Confirm##yes_del_select_corr_bykeypress", ImVec2(PopupButtonsWidth, 0))) {
				inputDeleteSelectedCorrespondences();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel##no_del_select_corr_bykeypress", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}// end popup window
	}//guiCorrEditViewDeletePopupOnKeyPress

	void TempRegViewer::guiCorrEditViewOverwritePopupOnKeyPress(void) {
		if (m_CorrEditOverwriteOnKeyPress) {
			m_CorrEditOverwriteOnKeyPress = false;
			ImGui::OpenPopup("Overwrite Correspondence##overwrite_corr_popup_keypress");
		}

		ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(Center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(400.0f, 110.0f), ImGuiCond_Always);

		//
		// Popup window
		//
		if (ImGui::BeginPopupModal("Overwrite Correspondence##overwrite_corr_popup_keypress", nullptr, ImGuiWindowFlags_NoResize)) {
			float PopupButtonsWidth = ImGui::CalcTextSize("Confirm").x + (2.0f * ImGui::GetStyle().FramePadding.x);

			ImGui::TextWrapped("Are you sure you wish to overwrite the current correspondence of the selected template vertex?");

			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - (PopupButtonsWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x));

			if (ImGui::Button("Confirm##yes_overwrite_corr_bykeypress", ImVec2(PopupButtonsWidth, 0))) {
				inputOverwriteCorrespondence();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel##no_overwrite_corr_bykeypress", ImVec2(PopupButtonsWidth, 0))) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}// end popup window
	}//guiCorrEditViewOverwritePopupOnKeyPress

	void TempRegViewer::guiCorrespondenceDataSummary(void) {
		ImGui::PushID("summary_panel_");

		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		if (ImGui::TreeNode("Overview")) {
			ImGui::Spacing();

			const size_t TemplateVertCount = m_TFitter.getTemplate().vertexCount();
			const size_t FeatCorrCount = m_TFitter.featureCorrespondences().size();
			const size_t AutoCorrCount = m_TFitter.automaticCorrespondences().size();
			const size_t TotalCorrCount = FeatCorrCount + AutoCorrCount;

			char TempVertCountBuf[64];
			char TotalCorrCountBuf[64];
			char FeatCorrCountBuf[64];
			char AutoCorrCountBuf[64];

			sprintf(TempVertCountBuf, "%llu", TemplateVertCount);
			// -> second number is total template vertex count 
			// -> meaning "N correspondences are possible (one for each template vertex -> second number), M correspondences are currently present (first number)" -> turn into tooltip
			sprintf(TotalCorrCountBuf, "%llu / %llu", TotalCorrCount, TemplateVertCount);
			sprintf(FeatCorrCountBuf, "%llu", FeatCorrCount);
			sprintf(AutoCorrCountBuf, "%llu", AutoCorrCount);

			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
			ImGui::BeginDisabled();

			ImGui::Text("Template Vertex Count:");
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputText("##template_vert_count_textbox", TempVertCountBuf, 64, ImGuiInputTextFlags_ReadOnly);
			guiTooltip("/*TODO: tooltip*/", ImGuiHoveredFlags_AllowWhenDisabled);
			//ImGui::Text(TempVertCountBuf);

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::Text("Active Correspondences:");
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputText("##active_corr_textbox", TotalCorrCountBuf, 64, ImGuiInputTextFlags_ReadOnly);
			guiTooltip("/*TODO: tooltip*/", ImGuiHoveredFlags_AllowWhenDisabled);
			//ImGui::Text(TotalCorrCountBuf);

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::Text("Feature Correspondences:");
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputText("##feat_corr_textbox", FeatCorrCountBuf, 64, ImGuiInputTextFlags_ReadOnly);
			guiTooltip("/*TODO: tooltip*/", ImGuiHoveredFlags_AllowWhenDisabled);
			//ImGui::Text(FeatCorrCountBuf);

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::Text("Automatic Correspondences:");
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::InputText("##auto_corr_textbox", AutoCorrCountBuf, 64, ImGuiInputTextFlags_ReadOnly);
			guiTooltip("/*TODO: tooltip*/", ImGuiHoveredFlags_AllowWhenDisabled);
			//ImGui::Text(AutoCorrCountBuf);

			ImGui::EndDisabled();
			ImGui::PopStyleVar();

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			guiShowCorrespondenceCheckboxes();

			ImGui::TreePop();
		}

		ImGui::PopID();
	}//guiCorrespondenceDataSummary

	void TempRegViewer::guiShowCorrespondenceCheckboxes(void) {
		if (ImGui::Checkbox("Show Feature Correspondences", &m_ShowFeatCorrMarkers)) {
			m_TemplateDisplayData->showFeatureCorrMarkers(m_ShowFeatCorrMarkers);
			m_FittingResDisplayData->showFeatureCorrMarkers(m_ShowFeatCorrMarkers);
			m_TargetDisplayData->showFeatureCorrMarkers(m_ShowFeatCorrMarkers);
		}

		if (ImGui::Checkbox("Show Automatic Correspondences", &m_ShowAutoCorrMarkers)) {
			m_TemplateDisplayData->showAutomaticCorrMarkers(m_ShowAutoCorrMarkers);
			m_FittingResDisplayData->showAutomaticCorrMarkers(m_ShowAutoCorrMarkers);
			m_TargetDisplayData->showAutomaticCorrMarkers(m_ShowAutoCorrMarkers);
		}
	}//guiShowCorrespondenceCheckboxes

	void TempRegViewer::guiSelectedCorrespondencePropertiesList(void) {
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		if (ImGui::TreeNode("Current Selection")) {
			ImGui::Spacing();

			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.0f);
			ImGui::BeginDisabled();

			if (!m_SelectedCorrsByTemplateID.empty()) {
				const auto& CorrData = m_TFitter.correspondenceData(m_SelectedCorrsByTemplateID[0]); // for now the selection of correspondences is limited to one correspondence at a time

				char CorrTypeBuf[32];
				if (CorrData.Type == TemplateFitter::CorrespondenceType::FEATURE) strcpy(CorrTypeBuf, "Feature Correspondence");
				else strcpy(CorrTypeBuf, "Automatic Correspondence");

				std::string EuclidDistString = guiFloatToTextLabel(CorrData.DistanceEuclidean);

				//
				// Type of selected correspondence
				//
				ImGui::Text("Correspondence Type:");

				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##active_corr_textbox", CorrTypeBuf, 64, ImGuiInputTextFlags_ReadOnly);

				//
				// Euclidean distance between points of selected correspondence
				//
				ImGui::Text("Euclidean Distance:");

				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##eucliddist_textbox", &EuclidDistString, ImGuiInputTextFlags_ReadOnly);
				guiTooltip("/*TODO: tooltip*/", ImGuiHoveredFlags_AllowWhenDisabled);

				//
				//TODO: other properties of a potential correspondence between the selected points here...
				//
			}
			else {
				char CorrTypeBuf[4] = "---";
				char EuclidDistBuf[4] = "---";

				//
				// Type of selected correspondence
				//
				ImGui::Text("Correspondence Type:");

				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##active_corr_textbox", CorrTypeBuf, 4, ImGuiInputTextFlags_ReadOnly);
				guiTooltip("No correspondence selected", ImGuiHoveredFlags_AllowWhenDisabled);

				//
				// Euclidean distance between points of selected correspondence
				//
				ImGui::Text("Euclidean Distance:");

				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::InputText("##active_corr_textbox", EuclidDistBuf, 4, ImGuiInputTextFlags_ReadOnly);
				guiTooltip("No correspondence selected", ImGuiHoveredFlags_AllowWhenDisabled);

				//
				//TODO: other properties of a potential correspondence between the selected points here...
				//
			}

			ImGui::EndDisabled();
			ImGui::PopStyleVar();

			ImGui::Dummy(ImVec2(0.0f, 2.0f));

			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			ImGui::TextWrapped("(i) Left click on template vertices with active markers to select a correspondence.");
			ImGui::PopStyleColor();

			ImGui::TreePop();
		}
	}//guiSelectedCorrespondencePropertiesList

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Helper functions
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Quaternionf TempRegViewer::trackballRotation(ViewportIdentifier ViewportID, Vector2f CursorPosStartOGL, Vector2f CursorPosEndOGL) {
		Vector3f Start = mapToSphereHyperbolic(CursorPosStartOGL, m_Viewports[ViewportID].VP.Position, m_Viewports[ViewportID].VP.Size);
		Vector3f End = mapToSphereHyperbolic(CursorPosEndOGL, m_Viewports[ViewportID].VP.Position, m_Viewports[ViewportID].VP.Size);

		// rotate with camera
		Matrix3f RotMat;
		RotMat.block<3, 3>(0, 0) = m_Viewports[ViewportID].Cam.cameraMatrix().block<3, 3>(0, 0);
		Quaternionf CamRotation = Quaternionf(RotMat.transpose());

		Start = CamRotation * Start;
		End = CamRotation * End;

		Vector3f DragDir = End - Start;

		Vector3f RotationAxis = Start.cross(End);
		RotationAxis.normalize();

		Quaternionf ModelRotation = Quaternionf::Identity();
		ModelRotation = AngleAxisf(CForge::GraphicsUtility::degToRad(DragDir.norm() * 65.0f), RotationAxis);
		return ModelRotation;
	}//trackballRotation

	Vector3f TempRegViewer::mapToSphereHyperbolic(Vector2f CursorPosOGL, Vector2i VPOffset, Vector2i VPSize) {
		float Radius = 1.0f;
		float MappedX = 2.0f * (CursorPosOGL.x() - (float)VPOffset.x()) / (float)VPSize.x() - 1.0f;
		float MappedY = 2.0f * (CursorPosOGL.y() - (float)VPOffset.y()) / (float)VPSize.y() - 1.0f;
		float MappedZ;
		float LengthSquared = (MappedX * MappedX) + (MappedY * MappedY);

		if (LengthSquared <= Radius * Radius / 2.0f) MappedZ = std::sqrtf((Radius * Radius) - LengthSquared);
		else MappedZ = ((Radius * Radius) / 2.0f) / std::sqrtf(LengthSquared);

		return Vector3f(MappedX, MappedY, MappedZ);
	}//mapToSphereHyperbolic

	std::string TempRegViewer::guiFloatToTextLabel(float Val) {
		std::stringstream Stream;
		std::string Res;

		Stream << std::fixed << std::setprecision(4) << Val;
		Res = Stream.str();

		size_t LastNonZero = Res.find_last_not_of('0');
		if (LastNonZero != std::string::npos && LastNonZero + 1 < Res.size())
			Res.erase((Res.at(LastNonZero) == '.') ? LastNonZero + 2 : LastNonZero + 1, std::string::npos);

		return Res;
	}//guiFloatToTextLabel
	
	void TempRegViewer::guiSpacedSeparator(void) {
		ImGui::Dummy(ImVec2(0.0f, 2.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 2.0f));
	}//guiSpacedSeparator

	void TempRegViewer::guiTooltip(const char* Text, ImGuiHoveredFlags HoveredFlags) {
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
			ImGui::TextUnformatted(Text);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}//guiTooltip
}