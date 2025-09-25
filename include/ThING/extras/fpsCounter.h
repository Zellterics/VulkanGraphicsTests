#include <chrono>

class FPSCounter {
public:
    bool frame();
    void delay(float targetFrameTime);
    int getFPS() const;
    float getDeltaTime() const;
    
private:
    float deltaTime = 0.0f;
    std::chrono::steady_clock::time_point frameStart = std::chrono::steady_clock::now();
    float elapsedTime = 0.0f;
    int frameCount = 0;
};