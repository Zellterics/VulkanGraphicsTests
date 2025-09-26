#include <ThING/core.h>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

void ProtoThiApp::createUniformBuffers(){
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        void* data;
        uniformBuffers[i].device = device;
        bufferManager.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i].buffer,uniformBuffers[i].memory);
    }
}

void ProtoThiApp::updateUniformBuffers(uint32_t frameIndex){
    const VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    static void* mappedData[MAX_FRAMES_IN_FLIGHT] = {nullptr};

    UniformBufferObject ubo{};
    static float width = 0;
    static float height = 0;


    if(width != (float) swapChainManager.getExtent().width || height != (float) swapChainManager.getExtent().height || updateUBOFlag){
        width = (float) swapChainManager.getExtent().width;
        height = (float) swapChainManager.getExtent().height;
        if(zoom == 0){
            zoom = .001;
        }
        float halfWidth = (width / 2.0f) / zoom;
        float halfHeight = (height / 2.0f) / zoom;
        ubo.projection = glm::ortho(
            -halfWidth + offset.x, halfWidth + offset.x,
            -halfHeight + offset.y, halfHeight + offset.y,
            -1.0f, 1.0f
        );
    }
    if(!mappedData[frameIndex]){
        vkMapMemory(device, uniformBuffers[frameIndex].memory, 0, bufferSize, 0, &mappedData[frameIndex]);
        updateUBOFlag = false;
    }
    
    memcpy(mappedData[frameIndex], &ubo, (size_t) bufferSize);
}
