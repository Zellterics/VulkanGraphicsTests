#include "core.h"

void ProtoThiApp::saveStagingBuffer(VkBuffer *buffer){
    stagingBuffers.push_back((buffer));
}
void ProtoThiApp::saveStagingBufferMemorys(VkDeviceMemory *bufferMemory){
    stagingBufferMemorys.push_back((bufferMemory));
}

void ProtoThiApp::updateVertexBuffers(uint32_t frameIndex){
    static VkDeviceSize bufferSizes[MAX_FRAMES_IN_FLIGHT] = { 0 };
    VkDeviceSize newBufferSize = sizeof(vertices[0]) * vertices.size();
    static VkBuffer stagingBuffer;
    static VkDeviceMemory stagingBufferMemory;
    static bool isCreated = false;
    static void* mappedData;
    static bool isMapped = false;
    static bool savedBuffer = false;
    if(!savedBuffer){
        saveStagingBuffer(&stagingBuffer);
        saveStagingBufferMemorys(&stagingBufferMemory);
        savedBuffer = true;
    }
    if (bufferSizes[frameIndex] < newBufferSize){
        bufferSizes[frameIndex] = newBufferSize;
        if(vertexBuffers[frameIndex]){
            vkWaitForFences(device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
            vkDestroyBuffer(device, vertexBuffers[frameIndex], nullptr);
            vkFreeMemory(device, vertexBufferMemorys[frameIndex], nullptr);
        }    
        createBuffer(bufferSizes[frameIndex], VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffers[frameIndex], vertexBufferMemorys[frameIndex]);
        if(stagingBuffer){
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
            isMapped = false;
            isCreated = false;
            mappedData = nullptr;
        }
    }

    if (!isCreated){
        createBuffer(newBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        isCreated = true;
    }
    
    if(!isMapped){
        vkMapMemory(device, stagingBufferMemory, 0, newBufferSize, 0, &mappedData);
        isMapped = true;
    }
    memcpy(mappedData, vertices.data(), (size_t) newBufferSize);
    copyBuffer(stagingBuffer, vertexBuffers[frameIndex], newBufferSize);
}

void ProtoThiApp::updateCircleBuffers(uint32_t frameIndex){
    static VkDeviceSize bufferSizes[MAX_FRAMES_IN_FLIGHT] = { 0 };
    VkDeviceSize newBufferSize = sizeof(circleCenters[0]) * circleCenters.size();
    static VkBuffer stagingBuffer;
    static VkDeviceMemory stagingBufferMemory;
    static bool isCreated = false;
    static void* mappedData;
    static bool isMapped = false;
    static bool savedBuffer = false;
    if(!savedBuffer){
        saveStagingBuffer(&stagingBuffer);
        saveStagingBufferMemorys(&stagingBufferMemory);
        savedBuffer = true;
    }
    if (bufferSizes[frameIndex] < newBufferSize){
        bufferSizes[frameIndex] = newBufferSize;
        if(circleBuffers[frameIndex]){
            vkWaitForFences(device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
            vkDestroyBuffer(device, circleBuffers[frameIndex], nullptr);
            vkFreeMemory(device, circleBufferMemorys[frameIndex], nullptr);
        }    
        createBuffer(bufferSizes[frameIndex], VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, circleBuffers[frameIndex], circleBufferMemorys[frameIndex]);
        if(stagingBuffer){
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
            isMapped = false;
            isCreated = false;
            mappedData = nullptr;
        }
    }

    if (!isCreated){
        createBuffer(newBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        isCreated = true;
    }
    
    if(!isMapped){
        vkMapMemory(device, stagingBufferMemory, 0, newBufferSize, 0, &mappedData);
        isMapped = true;
    }
    memcpy(mappedData, circleCenters.data(), (size_t) newBufferSize);
    copyBuffer(stagingBuffer, circleBuffers[frameIndex], newBufferSize);
}


void ProtoThiApp::updateIndexBuffers(uint32_t frameIndex){
    static VkDeviceSize bufferSizes[MAX_FRAMES_IN_FLIGHT] = {0};
    VkDeviceSize newBufferSize = sizeof(indices[0]) * indices.size();
    static VkBuffer stagingBuffer;
    static VkDeviceMemory stagingBufferMemory;
    static void* mappedData;
    static bool isMapped = false;
    static bool isCreated = false;
    static bool savedBuffer = false;
    if(!savedBuffer){
        saveStagingBuffer(&stagingBuffer);
        saveStagingBufferMemorys(&stagingBufferMemory);
        savedBuffer = true;
    }

    if(bufferSizes[frameIndex] < newBufferSize){
        bufferSizes[frameIndex] = newBufferSize;
        if(indexBuffers[frameIndex]){
            vkWaitForFences(device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
            vkDestroyBuffer(device, indexBuffers[frameIndex], nullptr);
            vkFreeMemory(device, indexBufferMemorys[frameIndex], nullptr);
        }
        createBuffer(bufferSizes[frameIndex], VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffers[frameIndex], indexBufferMemorys[frameIndex]);
        if(stagingBuffer){
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
            isMapped = false;
            isCreated = false;
            mappedData = nullptr;
        }
    }
    if (!isCreated){
        createBuffer(newBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        isCreated = true;
    }
    if (!isMapped){
        vkMapMemory(device, stagingBufferMemory, 0, newBufferSize, 0, &mappedData);
        isMapped = true;
    }
    
    memcpy(mappedData, indices.data(), (size_t) newBufferSize);
    copyBuffer(stagingBuffer, indexBuffers[frameIndex], newBufferSize);
}