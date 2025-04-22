#pragma once
#include "core.h"
#include "updateBuffers.cpp"

void HelloTriangleApplication::mainLoop() {
    int frame = 0;
    int x , y;
    glfwGetWindowSize(window, &x, &y);

    vertices.push_back({{0.90f,0.90f},{0.90f,0.90f,0.90f}});
    vertices.push_back({{0.91f,0.90f},{0.90f,0.90f,0.90f}});
    vertices.push_back({{0.90f,0.91f},{0.90f,0.90f,0.90f}});
    vertices.push_back({{0.91f,0.91f},{0.90f,0.90f,0.90f}});
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(6);
    indices.push_back(7);
    indices.push_back(8);
    while (!glfwWindowShouldClose(window)) {
        frame++;
        if(frame > 100){
            vertices.push_back({{getRandomFloat(-100.0f,100.0f)/100.0f, getRandomFloat(-100.0f,100.0f)/100.0f}, {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
            if (vertices.size() >= 3) {
                uint32_t base = static_cast<uint32_t>(vertices.size()) - 1;
                indices.push_back(base);
                indices.push_back(base - 1);
                indices.push_back(base - 2);
            }
        }
        updateIndexBuffer(currentFrame);
        updateVertexBuffer(currentFrame);
        
        glfwPollEvents();
        drawFrame();
        
        //std::cout << x << " y: " << y << std::endl;
        constexpr int delay = 1000 / FPS;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    vkDeviceWaitIdle(device);
}