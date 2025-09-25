#pragma once
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <vector>
#include <ThING/consts.h>
#include <ThING/types/dynamicBuffer.h>

class BufferManager{
public:
    BufferManager() = default;
    BufferManager(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);
    ~BufferManager();
    void uploadBuffer(VkDeviceSize bufferSize, VkBuffer *buffer, void* bufferData);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void udpateBuffer(DynamicBuffer<MAX_FRAMES_IN_FLIGHT>& dynamicBuffer, 
        Buffer& passedBuffer, 
        VkFence& inFlightFences, 
        const void* data, 
        VkDeviceSize newBufferSize, 
        uint32_t frameIndex,
        VkBufferUsageFlags usage);
private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    std::vector<Buffer> buffers;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    VkPhysicalDevice physicalDevice;
};