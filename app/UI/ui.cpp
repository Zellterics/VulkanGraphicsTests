#include "ui.h"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "globals.h"
#include <ThING/extras/imGuiCustom.h>

void UI(ThING::API& api, FPSCounter& fps){
    static glm::vec2 offset = {0,0};
    static float zoom = 1;
    static float rotation = 0;
    static glm::vec2 scale = {50,50};
    static glm::vec2 position = {0,0};
    static float bgColor[3] = {0,0,0};
    static glm::vec4 bgColorVec = {0,0,0,1};
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

    ImGui::Text("Circles: %d", api.getCircleAmount());
    ImGui::SliderInt("FPS: ", &speed, 10, 420);
    Slider2DFloat("Spawn Point", &spawnPoint[0], &spawnPoint[1], -1.0f, 1.0f, -1.0f, 1.0f);
    Slider2DFloat("Gravity", &gravity[0], &gravity[1], -5.0f, 5.0f, -5.0f, 5.0f);
    ImGui::SliderFloat("spawnRad: ", &spawnRadius, 0.0f, 100.0f, "%.3f");
    ImGui::Text("Real FPS: %d", (int)(fps.getFPS() + 1));

    if (ImGui::Button("Random Triangle")) {
        int sides = getRandomNumber(3, 15);
        float x = getRandomNumber(-200.f, 600.f);
        float y = getRandomNumber(-400.f, 400.f);
        glm::vec2 scale = {abs(x), abs(y)};
        std::string polyId = "poly";
        api.addRegularPol(polyId, sides, {x,y}, {40,40}, {getRandomNumber(0.f, 1.f),getRandomNumber(0.f, 1.f),getRandomNumber(0.f, 1.f)});
    }

    if (ImGui::TreeNode("EXTRAS")) {
        ImGui::SliderFloat("zoom", &zoom, 0, 5);
        ImGui::SliderFloat("rotation", &rotation, 0, 360);
        Slider2DFloat("scale", &scale.x, &scale.y, -500.0f, 500.0f, -500.0f, 500.0f);
        Slider2DFloat("pos", &position.x, &position.y, -500.0f, 500.0f, -500.0f, 500.0f);
        Slider2DFloat("offset", &offset.x, &offset.y, -500.0f, 500.0f, -500.0f, 500.0f);
        ImGui::ColorPicker3("BGColor", bgColor);
        ImGui::TreePop();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopItemWidth();
    ImGui::PopFont();
    ImGui::End();

    bgColorVec = {bgColor[0], bgColor[1], bgColor[2], 1};
    if(offset.x < 40 && offset.x > -40){ offset.x = 0; }
    if(offset.y < 40 && offset.y > -40){ offset.y = 0; }
    api.setRotation(position, rotation, scale);
    api.setBackgroundColor(bgColorVec);
    api.setZoomAndOffset(zoom, offset);

    ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceID);
    dockedSizeX = dockedSize.x;
    static int simWidth, simHeight;
    if (node && node->CentralNode) {
        simWidth  = (int)node->CentralNode->Size.x;
        simHeight = (int)node->CentralNode->Size.y;
    }

    fps.delay(1 / (float)(speed));
    if(fps.frame()){
        #ifdef DEBUG
        std::cout << "\ncircles: " << api.getCircleAmount() << "  ";
        #endif
    }
}