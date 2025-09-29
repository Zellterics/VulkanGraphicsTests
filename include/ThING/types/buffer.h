#pragma once

#include <vulkan/vulkan.h>

struct Buffer {
    VkBuffer buffer{VK_NULL_HANDLE};
    VkDeviceMemory memory{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};

    Buffer() = default;
    Buffer(VkDevice device, VkBuffer b, VkDeviceMemory m)
        : buffer(b), memory(m), device(device) {}

    ~Buffer() {
        if (buffer != VK_NULL_HANDLE) vkDestroyBuffer(device, buffer, nullptr);
        if (memory != VK_NULL_HANDLE) vkFreeMemory(device, memory, nullptr);
        buffer = VK_NULL_HANDLE;
        memory = VK_NULL_HANDLE;
    }
};