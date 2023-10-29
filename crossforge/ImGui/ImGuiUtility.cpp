#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "ImGuiUtility.h"

namespace CForge {

	void ImGuiUtility::initImGui(GLWindow* pWindow) {
		if (nullptr == pWindow) throw NullpointerExcept("pWindow");
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsDark();

		io.Fonts->AddFontDefault();

		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(pWindow->handle()), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

	}//initImGui

	void ImGuiUtility::newFrame(void) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}//newFrame

	void ImGuiUtility::render(void) {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}//render

	void ImGuiUtility::shutdownImGui(void) {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}//shutdownImGui


	ImGuiUtility::ImGuiUtility(void) {

	}//Constructor

	ImGuiUtility::~ImGuiUtility(void) {

	}//Destructor

}