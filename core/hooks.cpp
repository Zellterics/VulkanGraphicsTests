#include "core.h"
#include "glm/fwd.hpp"
#include <vector>

int ProtoThiApp::getCircleAmount(){
    return circleCenters.size();
}

void ProtoThiApp::getWindowSize(int* x, int* y){
    glfwGetWindowSize(window, x, y);
    return;
}

void ProtoThiApp::addCircle(glm::vec2 pos, float size, glm::vec3 color){
    circleCenters.push_back({pos, size, color});
}

std::vector<Circle>* ProtoThiApp::getCircleDrawVector(){
    return &circleCenters;
}