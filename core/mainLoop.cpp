#include "core.h"
#include "fpsCounter.h"
// Dear ImGui
#include "imgui.h"
#include "imgui_internal.h"

// Backends (GLFW + Vulkan)
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

void ProtoThiApp::mainLoop() {
    constexpr float delay = 1 / (float)FPS;
    FPSCounter fps;
    int speed = FPS;
    float gravity = 1.f;
    float spawnPoint[2] = {0,0};
    float spawnRadius = 10.f;
    ImFont* monocraft = ImGui::GetIO().Fonts->AddFontFromFileTTF("Monocraft.ttf", 18.0f);
    IM_ASSERT(monocraft && "Failed to load font!");
    while (!glfwWindowShouldClose(window)) {
        
        //FRAME INIT
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI
        ImGui::SetNextWindowBgAlpha(0.f);
        ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
        static bool first = true;
        if(first){
            first = false;
            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_CentralNode);
            ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->Size);

            ImGuiID dock_id_left = 0, dock_id_right = 0;
            ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.25f, &dock_id_left, &dock_id_right);
            ImGui::DockBuilderDockWindow("Debug Window", dock_id_left);

            ImGui::DockBuilderFinish(dockspaceID);
        }
        ImGui::Begin("Debug Window", nullptr, ImGuiWindowFlags_NoMove);
        ImVec2 dockedSize = ImGui::GetWindowSize();
        ImGui::PushFont(monocraft);
        ImGui::Text("Vertices: %zu", circleCenters.size());
        ImGui::SliderInt("FPS: ", &speed, 10, 420);
        ImGui::SliderFloat("gravity: ", &gravity, -5.0f, 5.0f, "%.3f");
        ImGui::SliderFloat2("spawnPoint", spawnPoint, -1.0f, 1.0f, "%.3f");
        ImGui::SliderFloat("spawnRadius: ", &spawnRadius, 0.0f, 100.0f, "%.3f");
        ImGui::Text("Real FPS: %d", fps.getFPS());
        ImGui::PopFont();
        ImGui::End();
        

        //ImGui window size
        ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceID);

        static int simWidth, simHeight;
        if (node && node->CentralNode)
        {
            simWidth  = (int)node->CentralNode->Size.x;
            simHeight = (int)node->CentralNode->Size.y;
        }
        //RENDER
        update(0.25, gravity, dockedSize.x, spawnPoint, spawnRadius);
        ImGui::Render();
        renderFrame();
        fps.delay(1 / (float)speed);
        if(fps.frame()){
            #ifdef DEBUG
            std::cout << "\ncircles: " << circleCenters.size() << "  ";
            #endif
        }
    }
    vkDeviceWaitIdle(device);
}

void ProtoThiApp::renderFrame(){
    static long long unsigned int indexSaveSize[MAX_FRAMES_IN_FLIGHT] = {indices.size()};
    static long long unsigned int vertexSaveSize[MAX_FRAMES_IN_FLIGHT] = {vertices.size()};
    static long long unsigned int circleSaveSize[MAX_FRAMES_IN_FLIGHT] = {circleCenters.size()};
    updateUniformBuffers(currentFrame);
    if(indexSaveSize[currentFrame] != indices.size()){
        updateIndexBuffers(currentFrame);
        indexSaveSize[currentFrame] = indices.size();
    }
    if(vertexSaveSize[currentFrame] != vertices.size()){
        updateVertexBuffers(currentFrame);
        vertexSaveSize[currentFrame] = vertices.size();
    }
    if(circleSaveSize[currentFrame] != circleCenters.size()){
        updateCircleBuffers(currentFrame);
        circleSaveSize[currentFrame] = circleCenters.size();
    }
    glfwPollEvents();
    drawFrame();
}