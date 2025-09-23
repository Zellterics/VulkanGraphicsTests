#pragma once

#include <functional>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>

#include "handMade.h"
#include "vertex.h"
#include "uniformBufferObject.h"

#include "quad.h"
#include "circle.h"

#include "fpsCounter.h"
constexpr int FPS = 60;
const uint32_t WIDTH = 1200;
const uint32_t HEIGHT = 800;
const int MAX_FRAMES_IN_FLIGHT = 3;

//---------------------------------------------------------------------------------------
//
class ProtoThiApp {
public:
    //SETUP
    ProtoThiApp();
    void run();
    bool setUpdateCallback(std::function<void(ProtoThiApp&, FPSCounter&)>);
    bool setUICallback(std::function<void(ProtoThiApp&, FPSCounter&)>);

    //HOOKS
    int getCircleAmount();
    void getWindowSize(int* x, int* y);
    void addCircle(glm::vec2 pos, float radius, glm::vec3 color);
    std::vector<Circle>* getCircleDrawVector();
    
private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayouts[2];
    VkPipeline graphicsPipelines[2];

    VkCommandPool commandPool;
    //*------------------------------------
    VkBuffer vertexBuffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory vertexBufferMemorys[MAX_FRAMES_IN_FLIGHT];
    
    VkBuffer indexBuffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory indexBufferMemorys[MAX_FRAMES_IN_FLIGHT];

    VkBuffer uniformBuffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory uniformBufferMemorys[MAX_FRAMES_IN_FLIGHT];

    VkBuffer quadBuffer;
    VkDeviceMemory quadBufferMemory;

    VkBuffer circleBuffers[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory circleBufferMemorys[MAX_FRAMES_IN_FLIGHT];
    
    VkBuffer quadIndexBuffer;
    VkDeviceMemory quadIndexBufferMemory;

    std::vector<VkBuffer*> stagingBuffers;
    std::vector<VkDeviceMemory*> stagingBufferMemorys;

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorPool descriptorPool;
    //-------------------------------------
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    uint32_t currentFrame = 0;

    std::function<void(ProtoThiApp&, FPSCounter&)> updateCallback;
    std::function<void(ProtoThiApp&, FPSCounter&)> UICallback;

    std::vector<Vertex> vertices;
    UniformBufferObject ubo;
    std::vector<Quad> quadVertices;
    std::vector<Circle> circleCenters;
    std::vector<uint16_t> quadIndices;
    std::vector<uint16_t> indices;

    bool framebufferResized = false;

    void initWindow();
    void initVulkan();
    void initImGui();
    void mainLoop();
    void cleanupSwapChain();

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<ProtoThiApp*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void update(float deltaTime, float gravity[2], int dockedSize, float spawnPoint[2], float spawnRadius);
    void renderFrame();
    void updateVertexBuffers(uint32_t frameIndex);
    void updateIndexBuffers(uint32_t frameIndex);
    void updateUniformBuffers(uint32_t frameIndex);
    void updateCircleBuffers(uint32_t frameIndex);
    void cleanup();
    void recreateSwapChain();
    void createInstance();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createBasicGraphicsPipeline();
    void createCircleGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createVertexBuffers();
    void createIndexBuffers();
    void createQuadBuffer();
    void createQuadIndexBuffer();
    void createCircleBuffer();
    void uploadBuffer(VkDeviceSize bufferSize, VkBuffer *buffer, void* bufferData);
    void saveStagingBuffer(VkBuffer *buffer);
    void saveStagingBufferMemorys(VkDeviceMemory *bufferMemory);
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
    void createUniformBuffers();
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void createSyncObjects();
    void drawFrame();
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    //imgui

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    static std::vector<char> readFile(const std::string& filename) {
        std::string base = osd::getExecutableDir();
        std::ifstream file(std::filesystem::path(base) / filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            #ifndef DEBUG
            throw std::runtime_error("failed to open file!");
            #endif
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        #ifdef DEBUG
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        #endif
        return VK_FALSE;
    }
};