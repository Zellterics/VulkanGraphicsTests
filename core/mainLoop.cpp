#pragma once
#include "core.h"
#include "updateBuffers.cpp"
#include "update.cpp"

class FPSCounter {
    public:
    bool frame() {
        using namespace std::chrono;

        auto now = steady_clock::now();
        deltaTime = duration_cast<duration<float>>(now - frameStart).count();
        elapsedTime += deltaTime;
        frameStart = now;
        frameCount++;

        if (elapsedTime >= 1.0f) {
            #ifdef DEBUG
            std::cout << "FPS: " << frameCount << std::endl;
            #endif
            frameCount = 0;
            elapsedTime = 0.0f;
            return true;
        }
        return false;
    }
    void delay(float targetFrameTime) {
        using namespace std::chrono;
    
        auto now = steady_clock::now();
        float elapsed = duration_cast<duration<float>>(now - frameStart).count();
        float remaining = targetFrameTime - elapsed;
    
        if (remaining > 0.0f) {
            while (duration_cast<duration<float>>(steady_clock::now() - frameStart).count() < targetFrameTime) {}
        }
    }
    
    float getDeltaTime() const {
        return deltaTime;
    }
    
private:
    float deltaTime = 0.0f;
    std::chrono::steady_clock::time_point frameStart = std::chrono::steady_clock::now();
    float elapsedTime = 0.0f;
    int frameCount = 0;
};

void ProtoThiApp::mainLoop() {
    constexpr float delay = 1 / (float)FPS;
    FPSCounter fps;
    while (!glfwWindowShouldClose(window)) {
        update(0.25);
        renderFrame();
        
        //fps.delay(delay);
        if(fps.frame()){
            #ifdef DEBUG
            std::cout << "\ncircles: " << circleCenters.size() << "  ";
            #endif
        }
    }
    vkDeviceWaitIdle(device);
}

void ProtoThiApp::renderFrame(){
    static long long unsigned int indexSaveSize[MAX_FRAMES_IN_FLIGHT] = {indices.size()};
    static long long unsigned int vertexSaveSize[MAX_FRAMES_IN_FLIGHT] = {vertices.size()};
    static long long unsigned int circleSaveSize[MAX_FRAMES_IN_FLIGHT] = {circleCenters.size()};
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
    glfwPollEvents();
    drawFrame();
}