#include "ThING/consts.h"
#include "ThING/types/dynamicBuffer.h"
#include <ThING/graphics/bufferManager.h>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

BufferManager::BufferManager(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) 
: device(device), physicalDevice(physicalDevice), commandPool(commandPool), graphicsQueue(graphicsQueue) {

}

BufferManager::~BufferManager(){

}



void BufferManager::uploadBuffer(VkDeviceSize bufferSize, VkBuffer *buffer, void* bufferData){
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, bufferData, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);
    copyBuffer(stagingBuffer, *buffer, bufferSize);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory){
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}


void BufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

uint32_t BufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties){
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void BufferManager::udpateBuffer(DynamicBuffer<MAX_FRAMES_IN_FLIGHT>& dynamicBuffer, 
    Buffer& passedBuffer, 
    VkFence& inFlightFences, 
    const void* data, 
    VkDeviceSize newBufferSize, 
    uint32_t frameIndex,
    VkBufferUsageFlags usage){
    if (dynamicBuffer.bufferSizes[frameIndex] < newBufferSize){
        dynamicBuffer.bufferSizes[frameIndex] = newBufferSize;
        if(passedBuffer.buffer){
            vkWaitForFences(device, 1, &inFlightFences, VK_TRUE, UINT64_MAX);
            passedBuffer.~Buffer();
            passedBuffer.device = device;
        }    
        createBuffer(dynamicBuffer.bufferSizes[frameIndex], 
            usage, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            passedBuffer.buffer, 
            passedBuffer.memory);
        if(dynamicBuffer.stagingBuffer.buffer){
            vkUnmapMemory(device, dynamicBuffer.stagingBuffer.memory);
            dynamicBuffer.stagingBuffer.~Buffer();
            dynamicBuffer.stagingBuffer.device = device;
            dynamicBuffer.stagingBuffer = {};
            dynamicBuffer.stagingBuffer.device = device;
            dynamicBuffer.isMapped = false;
            dynamicBuffer.isCreated = false;
            dynamicBuffer.mappedData = nullptr;
        }
    }

    if (!dynamicBuffer.isCreated){
        createBuffer(newBufferSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            dynamicBuffer.stagingBuffer.buffer, 
            dynamicBuffer.stagingBuffer.memory);
        dynamicBuffer.isCreated = true;
    }
    
    if(!dynamicBuffer.isMapped){
        vkMapMemory(device, dynamicBuffer.stagingBuffer.memory, 0, newBufferSize, 0, &dynamicBuffer.mappedData);
        dynamicBuffer.isMapped = true;
    }
    memcpy(dynamicBuffer.mappedData, data, (size_t) newBufferSize);
    copyBuffer(dynamicBuffer.stagingBuffer.buffer, passedBuffer.buffer, newBufferSize);
}