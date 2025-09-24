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
    // FOR OPTIMIZATION PROPOSES YOU CAN USE A DIRTY FLAG TO UPDATE ONLY WHEN CHANGES WHERE SUBMITED, NOT REALLY NECESSARY WITH THE ACTUAL APPROACH
    updateUniformBuffers(currentFrame);
    updateIndexBuffers(currentFrame);
    updateVertexBuffers(currentFrame);
    updateCircleBuffers(currentFrame);
    glfwPollEvents();
    drawFrame();
}