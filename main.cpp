#include <iostream>
#include <GL/glew.h>
#include "glfw/include/GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

void setup() {
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW!" << std::endl;
        exit(1);
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RIP TEHNIK", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creating GLFW window!" << std::endl;
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW!" << std::endl;
        exit(1);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

int main() {
  std::cout << "Hello\n";
  return 0;
}
