#pragma once
#include "imgui.h"

bool Slider2DFloat(const char* label,
    float* valueX, float* valueY,
    float minX, float maxX,
    float minY, float maxY,
    ImVec2 size = ImVec2(150,150));