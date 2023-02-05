#include "../include/imgui_cmpl.h"

#include "../../vendor/imgui/imgui.h"
#include "../../vendor/imgui/imgui_impl_glfw.h"
#include "../../vendor/imgui/imgui_impl_opengl3.h"

extern "C" {
#include "../include/window.h"
#include "../include/camera2D.h"
}

static ImGuiIO io;
void wondererImguiInit() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(wondererGetGLFWwindow(), true);
  ImGui_ImplOpenGL3_Init("#version 450");
}

void wondererImguiNewFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void wondererImguiDraw() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void wondererImguiDestroy() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}


