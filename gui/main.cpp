#include <stdio.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"

#include "wav-reader.h"

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
    WavReader reader(argv[1]);

    vector<double> double_data = reader.getData();
    vector<float> data(double_data.begin(), double_data.end());

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) { return 1; }

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "Speaker Recognition GUI", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window, true);

    bool show_test_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();

        // 1. Show a simple window Tip: if we don't call
        // ImGui::Begin()/ImGui::End() the widgets appears in a window
        // automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End
        // pair
        if (show_another_window) {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in
        // ImGui::ShowTestWindow()
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(700, 200), ImGuiSetCond_FirstUseEver);

        ImGui::Begin("Sound");
        ImGui::PlotLines("", data.data(), data.size(),
                         0, "", -1, 1, ImVec2(700, 150));
        ImGui::Text("Data");
        ImGui::End();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}
