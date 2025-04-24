#pragma once
#include "core.h"
#include "updateBuffers.cpp"

void ProtoThiApp::mainLoop() {
    int frame = 0;
    float x , y;
    //glfwGetWindowSize(window, &x, &y);
    
    while (!glfwWindowShouldClose(window)) {
        y = swapChainExtent.height;
        x = swapChainExtent.width;
        frame++;
        if(frame > 100){
            vertices.push_back({{getRandomFloat(-300.0f,300.0f), getRandomFloat(-300.0f,300.0f)}, {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
            if (vertices.size() >= 3) {
                uint32_t base = static_cast<uint32_t>(vertices.size()) - 1;
                indices.push_back(base);
                indices.push_back(base - 1);
                indices.push_back(base - 2);
            }
            circleCenters.push_back({{getRandomFloat(-300.0f,300.0f), getRandomFloat(-300.0f,300.0f)},{(float)getRandomFloat(5.0f,50.0f)}, {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
        }
        updateUniformBuffers(currentFrame);
        updateIndexBuffers(currentFrame);
        updateVertexBuffers(currentFrame);
        updateCircleBuffers(currentFrame);
        
        glfwPollEvents();
        drawFrame();
        
        std::cout << x << " y: " << y << std::endl;
        constexpr int delay = 1000 / FPS;
        //std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    vkDeviceWaitIdle(device);
}