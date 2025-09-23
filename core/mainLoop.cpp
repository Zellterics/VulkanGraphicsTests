#include "core.h"
// Dear ImGui
#include "imgui.h"

// Backends (GLFW + Vulkan)
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"



void ProtoThiApp::mainLoop() {
    constexpr float delay = 1 / (float)FPS;
    FPSCounter fps;
    while (!glfwWindowShouldClose(window)) {
        
        //FRAME INIT
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Callbacks
        UICallback(*this, fps);
        updateCallback(*this, fps);
        
        //RENDER
        ImGui::Render();
        renderFrame();
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