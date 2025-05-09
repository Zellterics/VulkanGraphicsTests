#pragma once
#include "core.h"
#include <glm/gtc/matrix_transform.hpp>

void ProtoThiApp::createUniformBuffers(){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        void* data;
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],uniformBufferMemorys[i]);
    }
}

void ProtoThiApp::updateUniformBuffers(uint32_t frameIndex){
    const VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    static void* mappedData[MAX_FRAMES_IN_FLIGHT] = {nullptr};

    static UniformBufferObject ubo{};
    static float width = 0;
    static float height = 0;


    if(width != (float) swapChainExtent.width || height != (float) swapChainExtent.height){
        width = (float) swapChainExtent.width;
        height = (float) swapChainExtent.height;
        float halfWidth = width / 2.0f;
        float halfHeight = height / 2.0f;
        ubo.projection = glm::ortho(
            -halfWidth, halfWidth,
            -halfHeight, halfHeight,
            -1.0f, 1.0f
        );
    }
    if(!mappedData[frameIndex]){
        vkMapMemory(device, uniformBufferMemorys[frameIndex], 0, bufferSize, 0, &mappedData[frameIndex]);
    }
    
    memcpy(mappedData[frameIndex], &ubo, (size_t) bufferSize);
}
