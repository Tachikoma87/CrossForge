
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void initImGUI(void* pHandle) {

	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)pHandle, false);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}//initImGUI