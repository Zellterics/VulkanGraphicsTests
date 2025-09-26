#include "ThING/types/circle.h"
#include "ThING/types/vertex.h"
#include <ThING/core.h>
#include <cstdint>

void ProtoThiApp::updateCustomBuffers( uint32_t frameIndex){
    if(stagingBuffers.size() < 3){
        stagingBuffers.emplace_back();
        stagingBuffers.emplace_back();
        stagingBuffers.emplace_back();
        stagingBuffers[0].stagingBuffer.device = device;
        stagingBuffers[1].stagingBuffer.device = device;
        stagingBuffers[2].stagingBuffer.device = device;
    }
    bufferManager.udpateBuffer(stagingBuffers[0], vertexBuffers[frameIndex], swapChainManager.getInFlightFences()[frameIndex], vertices.data(), vertices.size() * sizeof(Vertex), frameIndex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT); //vertex
    bufferManager.udpateBuffer(stagingBuffers[1], indexBuffers[frameIndex], swapChainManager.getInFlightFences()[frameIndex], indices.data(), indices.size() * sizeof(uint16_t), frameIndex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);// index
    bufferManager.udpateBuffer(stagingBuffers[2], circleBuffers[frameIndex], swapChainManager.getInFlightFences()[frameIndex], circleCenters.data(), circleCenters.size() * sizeof(Circle), frameIndex, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);// circle
}