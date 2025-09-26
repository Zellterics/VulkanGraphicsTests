#pragma once

#include "ThING/types/buffer.h"
#include "glm/fwd.hpp"
#include <functional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cstdint>

#include <ThING/consts.h>
#include <ThING/core/detail.h>
#include <ThING/graphics/bufferManager.h>
#include <ThING/graphics/pipelineManager.h>
#include <ThING/window/windowManager.h>
#include <ThING/graphics/swapChainManager.h>
#include <ThING/extras/handMade.h>
#include <ThING/extras/fpsCounter.h>
#include <ThING/types.h>



//---------------------------------------------------------------------------------------
//
class ProtoThiApp {
public:
    //SETUP
    ProtoThiApp();
    void run();
    bool setUpdateCallback(std::function<void(ProtoThiApp&, FPSCounter&)>);
    bool setUICallback(std::function<void(ProtoThiApp&, FPSCounter&)>);

    //API
    int getCircleAmount();
    void getWindowSize(int* x, int* y);
    void addCircle(glm::vec2 pos, float radius, glm::vec3 color);
    std::vector<Circle>* getCircleDrawVector();
    void setZoomAndOffset(float zoom, glm::vec2 offset);
    void setBackgroundColor(glm::vec4 color);
    glm::mat4 build2DTransform(glm::vec2 pos, float rotation, glm::vec2 scale);
    void setRotation(glm::vec2 pos, float rotation, glm::vec2 scale);
    std::string makeUniqueId(std::string baseId);
    void addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>& ver, std::vector<uint16_t>& ind);
    void addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>&& ver, std::vector<uint16_t>&& ind);

    //detail
    friend void detail::setResizedFlag(ProtoThiApp& app, bool flag);
private:
    WindowManager windowManager;
    BufferManager bufferManager;
    PipelineManager pipelineManager;
    SwapChainManager swapChainManager;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkCommandPool commandPool;

    uint32_t currentFrame = 0;
    //Buffers
    Buffer vertexBuffers[MAX_FRAMES_IN_FLIGHT];
    Buffer indexBuffers[MAX_FRAMES_IN_FLIGHT];
    Buffer uniformBuffers[MAX_FRAMES_IN_FLIGHT];
    Buffer quadBuffer;
    Buffer circleBuffers[MAX_FRAMES_IN_FLIGHT];
    Buffer quadIndexBuffer;
    
    std::vector<DynamicBuffer<MAX_FRAMES_IN_FLIGHT>> stagingBuffers;

    VkDescriptorPool imguiDescriptorPool;
    //-------------------------------------
    std::vector<VkCommandBuffer> commandBuffers;

    std::function<void(ProtoThiApp&, FPSCounter&)> updateCallback;
    std::function<void(ProtoThiApp&, FPSCounter&)> UICallback;

    std::vector<Vertex> vertices;
    UniformBufferObject ubo;
    std::vector<Quad> quadVertices;
    std::vector<Circle> circleCenters;
    std::vector<uint16_t> quadIndices;
    std::vector<uint16_t> indices;

    // Hooks Variables
    float zoom;
    glm::vec2 offset;
    bool updateUBOFlag;
    bool framebufferResized;
    VkClearValue clearColor;
    std::vector<Polygon> polygons;

    void initVulkan();
    void initImGui();
    void mainLoop();
    
    void renderFrame();
    void updateCustomBuffers(uint32_t frameIndex);
    void updateUniformBuffers(uint32_t frameIndex);
    void cleanup();
    
    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createCustomBuffers();
    void createCommandPool();
    void uploadBuffer(VkDeviceSize bufferSize, VkBuffer *buffer, void* bufferData);
    void createUniformBuffers();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        #ifdef DEBUG
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        #endif
        return VK_FALSE;
    }
};