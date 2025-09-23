#include "ui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "globals.h"
#include "imGuiCustom.h"

void UI(ProtoThiApp& hooks, FPSCounter& fps){
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGuiID dockspaceID = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode | (ImGuiDockNodeFlags)ImGuiDockNodeFlags_NoTabBar);
    static bool first = true;
    if(first){
        first = false;
        monocraft = ImGui::GetIO().Fonts->AddFontFromFileTTF("../external/Monocraft.ttf", 18.0f);
        IM_ASSERT(monocraft && "Failed to load font!");
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
    ImGui::Text("Circles: %d", hooks.getCircleAmount());
    ImGui::SliderInt("FPS: ", &speed, 10, 420);
    Slider2DFloat("Spawn Point", &spawnPoint[0], &spawnPoint[1], -1.0f, 1.0f, -1.0f, 1.0f);
    Slider2DFloat("Gravity", &gravity[0], &gravity[1], -5.0f, 5.0f, -5.0f, 5.0f);
    ImGui::SliderFloat("spawnRad: ", &spawnRadius, 0.0f, 100.0f, "%.3f");
    ImGui::Text("Real FPS: %d", (int)(fps.getFPS()));
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImGui::PopFont();
    ImGui::End();


    //ImGui window size
    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceID);
    dockedSizeX = dockedSize.x;
    static int simWidth, simHeight;
    if (node && node->CentralNode)
    {
        simWidth  = (int)node->CentralNode->Size.x;
        simHeight = (int)node->CentralNode->Size.y;
    }
    fps.delay(1 / (float)(speed + 1));
    if(fps.frame()){
        #ifdef DEBUG
        std::cout << "\ncircles: " << hooks.getCircleAmount() << "  ";
        #endif
    }
}