#include <ThING/extras/imGuiCustom.h>
#include "imgui_internal.h"

bool Slider2DFloat(const char* label,
    float* valueX, float* valueY,
    float minX, float maxX,
    float minY, float maxY,
    ImVec2 size)
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