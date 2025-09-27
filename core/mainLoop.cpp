#include <ThING/core.h>

//PASS THIS THING TO A RENDERER CLASS LATER
void ProtoThiApp::renderFrame(){
    // FOR OPTIMIZATION PROPOSES YOU CAN USE A DIRTY FLAG TO UPDATE ONLY WHEN CHANGES WHERE SUBMITED, NOT REALLY NECESSARY WITH THE ACTUAL APPROACH
    bufferManager.updateUniformBuffers(swapChainManager.getExtent(), zoom, offset, currentFrame);
    bufferManager.updateCustomBuffers(vertices, indices, circleCenters, swapChainManager.getInFlightFences(), currentFrame);
    glfwPollEvents();
    drawFrame();
}