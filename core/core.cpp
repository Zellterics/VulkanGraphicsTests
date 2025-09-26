#include <ThING/core.h>
#include <ThING/extras/vulkanSupport.h>

#include "ThING/graphics/bufferManager.h"
#include "ThING/graphics/pipelineManager.h"
#include "imgui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

ProtoThiApp::ProtoThiApp() : windowManager(WIDTH, HEIGHT, "vulkan"){
    zoom = 1;
    offset = {0, 0};
    updateUBOFlag = true;
    framebufferResized = false;
    clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // NEGRO ESTANDAR
    
    vertices = {
        {{ 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
        {{ 0.0f,  1.0f}, {1.0f, 0.0f, 1.0f}}
    };

    polygons.push_back({"test", 0, 5, 0, 9, true, 
        {build2DTransform(
            {100.0f, 100.0f},
            glm::radians(0.0f),
            {50.0f, 50.0f}
        )}});
    
    indices = {
        0, 1, 2, 2, 3, 0, 4, 1, 0
    };

    ubo = {};

    quadVertices = {
        {{-1.f, -1.f}, {-1.0f, -1.0f}},
        {{1.f, -1.f}, {1.0f, -1.0f}},
        {{1.f, 1.f}, {1.0f, 1.0f}},
        {{-1.f, 1.f}, {-1.0f, 1.0f}}
    };

    circleCenters = {
        {{-50.f, -50.f},4.f, {1.0f, 0.0f, 0.0f}}
    };

    quadIndices = {0,1,2,2,3,0};

    

    updateCallback = nullptr;
    UICallback = nullptr;
}

void ProtoThiApp::run() {
    initVulkan();
    initImGui();
    mainLoop();
    cleanup();
}

bool ProtoThiApp::setUpdateCallback(std::function<void(ProtoThiApp&, FPSCounter&)> updateCb){
    updateCallback = updateCb;
    return static_cast<bool>(updateCallback);
}

bool ProtoThiApp::setUICallback(std::function<void(ProtoThiApp&, FPSCounter&)> UICb){
    UICallback = UICb;
    return static_cast<bool>(updateCallback);
}

void ProtoThiApp::initVulkan() {
    createInstance();
    setupDebugMessenger();
    swapChainManager = SwapChainManager{instance, windowManager.getWindow()};
    pickPhysicalDevice();
    createLogicalDevice();
    swapChainManager.setDevice(device);
    swapChainManager.createSwapChain(physicalDevice, windowManager.getWindow());
    swapChainManager.createImageViews();
    pipelineManager.init(device, swapChainManager.getImageFormat());
    swapChainManager.createFramebuffers(pipelineManager.getrenderPass());
    pipelineManager.createPipelines();
    createCommandPool();
    bufferManager = BufferManager{device, physicalDevice, commandPool, graphicsQueue};
    createCustomBuffers();
    createUniformBuffers();
    pipelineManager.createDescriptors(uniformBuffers);
    createCommandBuffers();
    swapChainManager.createSyncObjects();
}

void ProtoThiApp::cleanup() {
    vkDeviceWaitIdle(device);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    swapChainManager.cleanupSwapChain();

    pipelineManager.~PipelineManager();
    vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);

    vkFreeCommandBuffers(device, commandPool,
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data());
    commandBuffers.clear();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, swapChainManager.getRenderFinishedSemaphores()[i], nullptr);
        vkDestroySemaphore(device, swapChainManager.getImageAvailableSemaphores()[i], nullptr);
        vkDestroyFence(device, swapChainManager.getInFlightFences()[i], nullptr);
    }
    quadBuffer.~Buffer();  
    quadIndexBuffer.~Buffer();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vertexBuffers[i].~Buffer();
        indexBuffers[i].~Buffer();  
        uniformBuffers[i].~Buffer(); 
        circleBuffers[i].~Buffer();  
    }

    for (auto& dyn : stagingBuffers) {
        if (dyn.isMapped) {
            vkUnmapMemory(device, dyn.stagingBuffer.memory);
            dyn.isMapped = false;
            dyn.mappedData = nullptr;
        }
        if (dyn.stagingBuffer.buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(device, dyn.stagingBuffer.buffer, nullptr);
        if (dyn.stagingBuffer.memory != VK_NULL_HANDLE)
            vkFreeMemory(device, dyn.stagingBuffer.memory, nullptr);
        dyn.stagingBuffer.buffer = VK_NULL_HANDLE;
        dyn.stagingBuffer.memory = VK_NULL_HANDLE;
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        if (uniformBuffers[i].buffer != VK_NULL_HANDLE)
            vkDestroyBuffer(device, uniformBuffers[i].buffer, nullptr);
        if (uniformBuffers[i].memory != VK_NULL_HANDLE)
            vkFreeMemory(device, uniformBuffers[i].memory, nullptr);
    }
    
    // while(!graphicsPipelines.empty()){
    //     vkDestroyPipeline(device, graphicsPipelines.back(), nullptr);
    //     graphicsPipelines.pop_back();
    // }

    // while(!pipelineLayouts.empty()){
    //     vkDestroyPipelineLayout(device, pipelineLayouts.back(), nullptr);
    //     pipelineLayouts.pop_back();
    // }

    vkDestroyCommandPool(device, commandPool, nullptr);

    vkDestroyDevice(device, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    
    vkDestroySurfaceKHR(instance, swapChainManager.getSurface(), nullptr);
    vkDestroyInstance(instance, nullptr);

    

    glfwTerminate();
}

void ProtoThiApp::drawFrame() {
    vkWaitForFences(device, 1, &swapChainManager.getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChainManager.getSwapChain(), UINT64_MAX, swapChainManager.getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChainManager.recreateSwapChain(physicalDevice, windowManager.getWindow(), pipelineManager.getrenderPass());
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(device, 1, &swapChainManager.getInFlightFences()[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {swapChainManager.getImageAvailableSemaphores()[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {swapChainManager.getRenderFinishedSemaphores()[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, swapChainManager.getInFlightFences()[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChainManager.getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        swapChainManager.recreateSwapChain(physicalDevice, windowManager.getWindow(), pipelineManager.getrenderPass());
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}