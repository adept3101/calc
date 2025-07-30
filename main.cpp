#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <string>
// #include <expat_external.h>
#include <exprtk.hpp>

void setup() {
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW!" << std::endl;
        exit(1);
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Calc", nullptr, nullptr);
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

    return;
}

static std::string text = "";
static char input_text[128] = "";

void render(GLFWwindow *window){
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    symbol_table_t symbol_table;
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;

    ImGuiStyle& style = ImGui::GetStyle();

    style.FrameRounding = 10.0f;
    style.WindowRounding = 10.0f;
    style.WindowBorderSize = 0.0f;

    ImVec4* colors = style.Colors;
     while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Градиентный фон
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, display_w, display_h, 0.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            // Верх - голубой
            glColor4f(0.4f, 0.6f, 0.9f, 1.0f);
            glVertex2f(0.0f, 0.0f);
            glVertex2f(display_w, 0.0f);

            // Низ - белый
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glVertex2f(display_w, display_h);
            glVertex2f(0.0f, display_h);
        glEnd();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const float windowWidth = 400.0f; // размеры окна внутри
        const float windowHeight = 250.0f;
        const ImVec2 windowPos = ImVec2((1280 - windowWidth) * 0.5f, (720 - windowHeight) * 0.5f);
        static bool show_result = false;
        bool cyrilic = false;
        double result;

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

        ImGui::Begin("Calc", nullptr,
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoTitleBar);

        ImVec2 window_size = ImGui::GetWindowSize();
        float center_x = window_size.x * 0.5f;       
        ImGui::Dummy(ImVec2(0.0f, 20.0f)); 

        const float inputWidth = 300.0f;
        ImGui::SetCursorPosX(center_x - inputWidth * 0.5f);
        //ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputTextWithHint("##Input", "Input", input_text, IM_ARRAYSIZE(input_text));
        for(size_t i = 0; input_text[i] != '\0'; ++i){
            unsigned char c = static_cast<unsigned char>(input_text[i]);
            if(c >= 0xD0 && c <= 0xD1){ // диапозон кириллицы
                cyrilic = true;
                break;
            }
        }

        if(cyrilic) {
            ImGui::BeginTooltip();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
            ImGui::Text("Cyrillic not sup");
            ImGui::PopStyleColor();
            ImGui::EndTooltip();
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        const ImVec2 buttonSize = ImVec2(120, 35);
        float buttonPosX = (ImGui::GetWindowSize().x - buttonSize.x) * 0.5f;

        //ImGui::SetCursorPosX(buttonPosX);
        ImGui::SetCursorPos(ImVec2(buttonPosX, 90));
        //ImGui::Dummy(ImVec2(0.0f, 1.0f));
        if(ImGui::Button("Calculate", buttonSize)){
            if (parser.compile(input_text, expression)) {
                result = expression.value();
            }
            show_result = true;
        }

        // ImGui::Dummy(ImVec2(0.0f, 5.0f));

        float resultPosX = (ImGui::GetWindowSize().x - ImGui::CalcTextSize(text.c_str()).x) * 0.5f;
        ImGui::SetCursorPosX(resultPosX);
        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        if (show_result) {
            //ImGui::Dummy(ImVec2(0.0f, 15.0f));
            float resultPosX = (ImGui::GetWindowSize().x - ImGui::CalcTextSize(text.c_str()).x) * 0.5f;
            ImGui::SetCursorPosX(resultPosX);
            ImGui::Text("Result: %.2f", result);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    
    return;
}

int main() {
  //std::cout << "Hello\n";
  setup();
  GLFWwindow *window = glfwGetCurrentContext();
  render(window);
  return 0;
}
