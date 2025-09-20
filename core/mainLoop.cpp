#include "core.h"
#include "fpsCounter.h"
// Dear ImGui
#include "imgui.h"
#include "imgui_internal.h"

// Backends (GLFW + Vulkan)
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

bool Slider2DFloat(const char* label,
                   float* valueX, float* valueY,
                   float minX, float maxX,
                   float minY, float maxY,
                   ImVec2 size = ImVec2(150,150))
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(label, size);
    bool hovered = ImGui::IsItemHovered();
    bool active = ImGui::IsItemActive();

    float tX = (*valueX - minX) / (maxX - minX);
    float tY = (*valueY - minY) / (maxY - minY);

    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 mouse = io.MousePos;
        tX = (mouse.x - p.x) / size.x;
        tY = (mouse.y - p.y) / size.y;
        tX = ImClamp(tX, 0.0f, 1.0f);
        tY = ImClamp(tY, 0.0f, 1.0f);
        *valueX = minX + tX * (maxX - minX);
        *valueY = minY + tY * (maxY - minY);
    }

    draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), ImGui::GetColorU32(ImGuiCol_FrameBg), 4.0f);

    draw_list->AddRect(p, ImVec2(p.x + size.x, p.y + size.y), ImGui::GetColorU32(ImGuiCol_Border), 4.0f);

    ImVec2 handle(p.x + tX * size.x, p.y + tY * size.y);
    draw_list->AddCircleFilled(handle, 6.0f, ImGui::GetColorU32(hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));

    ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + size.y + ImGui::GetStyle().ItemInnerSpacing.y));
    ImGui::Text("%s", label);
    ImGui::Text("(%.2f, %.2f)", *valueX, *valueY);

    return active;
}


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
        ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode | (ImGuiDockNodeFlags)ImGuiDockNodeFlags_NoTabBar);
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
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;
        style.Colors[ImGuiCol_DockingEmptyBg] = {1, 1, 1, 0};
        style.Colors[ImGuiCol_WindowBg] = {.01, .01, .01, 1};
        style.Colors[ImGuiCol_FrameBg] = {0, 0, 0, 1};
        style.Colors[ImGuiCol_Border] = {1, 1, 1, 0};
        style.WindowMinSize = ImVec2(200, 200);
        ImGui::Begin("Debug Window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImVec2 dockedSize = ImGui::GetWindowSize();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 12.f);
        ImGui::PushItemWidth(100.f);
        ImGui::PushFont(monocraft);
        ImGui::Text("Circles: %zu", circleCenters.size());
        ImGui::SliderInt("FPS: ", &speed, 10, 420);
        ImGui::SliderFloat("gravity: ", &gravity, -5.0f, 5.0f, "%.3f");
        Slider2DFloat("Spawn Point", &spawnPoint[0], &spawnPoint[1], -1.0f, 1.0f, -1.0f, 1.0f);
        ImGui::SliderFloat("spawnRad: ", &spawnRadius, 0.0f, 100.0f, "%.3f");
        ImGui::Text("Real FPS: %d", fps.getFPS());
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();
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
        fps.delay(1 / (float)(speed + 1));
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