#pragma once
#include "core.h"
#include "updateBuffers.cpp"

class FPSCounter {
    public:
        bool frame() {
            using namespace std::chrono;
            frameCount++;
    
            auto now = high_resolution_clock::now();
            auto elapsed = duration_cast<duration<float>>(now - lastTime);
    
            if (elapsed.count() >= 1.0f) {
                std::cout << "FPS: " << frameCount;
                frameCount = 0;
                lastTime = now;
                return true;
            }
            return false;
        }
    
    private:
        std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;
};

void ProtoThiApp::mainLoop() {
    FPSCounter fps;
    int frame = 0;
    float x , y;
    long long unsigned int indexSaveSize[MAX_FRAMES_IN_FLIGHT] = {indices.size()};
    long long unsigned int vertexSaveSize[MAX_FRAMES_IN_FLIGHT] = {vertices.size()};
    long long unsigned int circleSaveSize[MAX_FRAMES_IN_FLIGHT] = {circleCenters.size()};
    //glfwGetWindowSize(window, &x, &y);

    
    while (!glfwWindowShouldClose(window)) {

        frame++;
        if(frame > 100&&vertices.size() < 50000){
            vertices.push_back({{getRandomFloat(-300.0f,300.0f), getRandomFloat(-300.0f,300.0f)}, {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
            if (vertices.size() >= 3) {uint32_t base = static_cast<uint32_t>(vertices.size()) - 1;indices.push_back(base);indices.push_back(base - 1);indices.push_back(base - 2);}
            // circleCenters.push_back({{getRandomFloat(-300.0f,300.0f), getRandomFloat(-300.0f,300.0f)},{(float)getRandomFloat(5.0f,50.0f)}, {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
        }
        updateUniformBuffers(currentFrame);
        if(indexSaveSize[currentFrame] != indices.size()){
            updateIndexBuffers(currentFrame);
            indexSaveSize[currentFrame] = indices.size();
        }
        if(vertexSaveSize[currentFrame] != vertices.size()){
            updateVertexBuffers(currentFrame);
            vertexSaveSize[currentFrame] = vertices.size();
        }
        if(circleSaveSize[currentFrame] != circleCenters.size()){
            updateCircleBuffers(currentFrame);
            circleSaveSize[currentFrame] = circleCenters.size();
        }
        
        if(fps.frame()){
            std::cout << "  amount: " << vertices.size() << "\n";
        }
        glfwPollEvents();
        drawFrame();
        constexpr int delay = 1000 / FPS;
        //std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    vkDeviceWaitIdle(device);
}