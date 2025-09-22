#include "core.h"
#include "vulkanSupport.h"

// Dear ImGui
#include "imgui.h"

// Backends (GLFW + Vulkan)
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

std::vector<Vertex> vertices = {
    {{1000.f, 1000.f}, {1.0f, 0.0f, 0.0f}},
    {{1050.f, 1000.f}, {0.0f, 1.0f, 0.0f}},
    {{1050.f, 1050.f}, {0.0f, 0.0f, 1.0f}},
    {{1000.f, 1050.f}, {1.0f, 1.0f, 1.0f}},
    {{1050.f, 950.f}, {1.0f, 0.0f, 1.0f}}
};

UniformBufferObject ubo{};

std::vector<Quad> quadVertices = {
    {{-1.f, -1.f}, {-1.0f, -1.0f}},
    {{1.f, -1.f}, {1.0f, -1.0f}},
    {{1.f, 1.f}, {1.0f, 1.0f}},
    {{-1.f, 1.f}, {-1.0f, 1.0f}}
};

std::vector<Circle> circleCenters = {
    {{-50.f, -50.f},4.f, {1.0f, 0.0f, 0.0f}}
};

std::vector<uint16_t> quadIndices = {0,1,2,2,3,0};

std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0, 4, 1, 0
};

void ProtoThiApp::run() {
    initWindow();
    initVulkan();
    initImGui();
    mainLoop();
    cleanup();
}

void ProtoThiApp::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void ProtoThiApp::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDescriptorSetLayout();
    createBasicGraphicsPipeline();
    createCircleGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createVertexBuffers();
    createIndexBuffers();
    createQuadBuffer();
    createQuadIndexBuffer();
    createCircleBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
    createCommandBuffers();
    createSyncObjects();
}

void ProtoThiApp::cleanup() {
    vkDeviceWaitIdle(device);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    cleanupSwapChain();

    for(int i = 0; i < 2; i++){
        vkDestroyPipeline(device, graphicsPipelines[i], nullptr);
        vkDestroyPipelineLayout(device, pipelineLayouts[i], nullptr);
    }
    
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroyBuffer(device, quadBuffer, nullptr);
    vkFreeMemory(device, quadBufferMemory, nullptr);
    vkDestroyBuffer(device, quadIndexBuffer, nullptr);
    vkFreeMemory(device, quadIndexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
        vkDestroyBuffer(device, vertexBuffers[i], nullptr);
        vkFreeMemory(device, vertexBufferMemorys[i], nullptr);
        vkDestroyBuffer(device, indexBuffers[i], nullptr);
        vkFreeMemory(device, indexBufferMemorys[i], nullptr);
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBufferMemorys[i], nullptr);
        vkDestroyBuffer(device, circleBuffers[i], nullptr);
        vkFreeMemory(device, circleBufferMemorys[i], nullptr);
    }

    // while(!graphicsPipelines.empty()){
    //     vkDestroyPipeline(device, graphicsPipelines.back(), nullptr);
    //     graphicsPipelines.pop_back();
    // }

    // while(!pipelineLayouts.empty()){
    //     vkDestroyPipelineLayout(device, pipelineLayouts.back(), nullptr);
    //     pipelineLayouts.pop_back();
    // }
    


    while(!stagingBuffers.empty()){
        vkDestroyBuffer(device, *stagingBuffers.back(), nullptr);
        stagingBuffers.pop_back();
    }

    while(!stagingBufferMemorys.empty()){
        vkFreeMemory(device, *stagingBufferMemorys.back(), nullptr);
        stagingBufferMemorys.pop_back();
    }

    

    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void ProtoThiApp::drawFrame() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}