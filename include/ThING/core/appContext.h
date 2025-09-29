#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct AppContext {
    GLFWwindow* window{};
    VkDevice device{};
    VkPhysicalDevice physicalDevice{};
    VkQueue graphicsQueue{};
    VkQueue presentQueue{};
    VkExtent2D swapChainExtent{};
    VkRenderPass renderPass{};

    float zoom = 1.0f;
    glm::vec2 offset = {0, 0};
    glm::vec4 clearColor = {0, 0, 0, 1};
};
