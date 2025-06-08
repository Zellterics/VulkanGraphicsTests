#include "core.h"
#include "physicsObject.h"
#include <thread>

void ProtoThiApp::update(float deltaTime){
    static std::vector<PhysicsObject> circlePhysics = {{{-50.f, -50.f},{-50.f, -50.f},{0,0}}};
    glm::vec2 randomPos = {getRandomFloat(100.0f,-100.0f), getRandomFloat(-300.0f,-280.0f)};
    const int BIGGER_RADIUS = 8;
    const int BIGGER_RADIUS_MINUS = BIGGER_RADIUS - 1;
    const int SMALLER_RADIUS = 4;
    circleCenters.push_back({randomPos,getRandomFloat(SMALLER_RADIUS,BIGGER_RADIUS), {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
    circlePhysics.push_back({randomPos, randomPos, {0.f,0.f}});
    const int steps = 2;
    const float sub_dt = deltaTime / (float)steps;
    int x, y;
    glfwGetWindowSize(window, &x, &y);
    x /= 2;
    y /= 2;
    int circleAmount = circleCenters.size();
    int gridWidth = (x + BIGGER_RADIUS_MINUS) / BIGGER_RADIUS;
    int gridHeight = (y + BIGGER_RADIUS_MINUS) / BIGGER_RADIUS;

    static unsigned const int NUM_THREADS = std::max(1u, std::thread::hardware_concurrency());
    for (int i = steps; i; i--){
        
        std::vector<std::vector<std::vector<int>>> circleID(gridWidth, std::vector<std::vector<int>>(gridHeight));
        for (int i = 0; i < circleAmount; i++){
            int gridX = ((circleCenters[i].pos.x + x) / 2) / BIGGER_RADIUS;
            int gridY = ((circleCenters[i].pos.y + y) / 2) / BIGGER_RADIUS;
            if (gridX >= 0 && gridX < gridWidth &&
                gridY >= 0 && gridY < gridHeight) {
                circleID[gridX][gridY].push_back(i);
            }
        }

        for(PhysicsObject& circle : circlePhysics){
            circle.accelerate({0.f, 1.f});
        }
        auto calculateCollissions = [&] (int start, int finish){
        for (int x = start; x < finish; x++) { // 20 veces extremadamente rapido
            for (int y = 0; y < gridHeight; y++) { // 20 veces extremadamente rapido
                for (int ID : circleID[x][y]) {     // cantidad de objetos
                    PhysicsObject &actualCirclePhysics = circlePhysics[ID];
                    Circle &actualCircleCenter = circleCenters[ID];
                    for (int j = 0; j <= 1; j++) {   //2 veces
                        for (int k = -1; k <= 1; k++) { // 3 veces
                            if(j == 0 && k==1)
                                continue;
                            int nx = x + j;
                            int ny = y + k;
        
                            if (nx < 0 || ny < 0 || nx >= finish || ny >= gridHeight)
                                continue;
        
                            for (int neighbor : circleID[nx][ny]) {
                                if (ID == neighbor) continue;
                                if(j == 0 && k == 0 && ID > neighbor) continue;

                                PhysicsObject &neighborCirclePhysics = circlePhysics[neighbor];
                                Circle &neighborCircleCenter = circleCenters[neighbor];

                                glm::vec2 collision = actualCirclePhysics.currentPos - neighborCirclePhysics.currentPos;
                                float summedSize = actualCircleCenter.size + neighborCircleCenter.size;
                                float dist2 = glm::dot(collision, collision);
                                if(dist2 > summedSize * summedSize){
                                    continue;
                                }
                                float dist = glm::sqrt(dist2);
                                
        
                                if (dist < summedSize && dist > 0.001f) {
                                    glm::vec2 n = collision / dist;
                                    float delta = summedSize - dist;
                                    glm::vec2 correction = .4f * delta * n;
                                    actualCirclePhysics.currentPos += correction;
                                    neighborCirclePhysics.currentPos -= correction;
                                }
                            }
                        }
                    }
                }
            }
        }};
        std::vector<std::thread> threads;
        for(int i = 0; ; i++){
            if(i >= NUM_THREADS - 1){
                threads.emplace_back(calculateCollissions, (gridWidth/NUM_THREADS) * i, gridWidth);
                break;
            }
            threads.emplace_back(calculateCollissions, (gridWidth/NUM_THREADS) * i, (gridWidth/NUM_THREADS) * (i + 1) + 1);
        }
        for(std::thread &th : threads){
            if(th.joinable()){
                th.join();
            }
        }


        for(int i = 0; i < circleAmount; i++){
            circlePhysics[i].updatePos(sub_dt);

            circleCenters[i].pos = circlePhysics[i].currentPos;
        }
        for(int i = 0; i < circleAmount; i++){
            const glm::vec2 minBound{-x, -y};
            const glm::vec2 maxBound{x, y};
            
            if (circlePhysics[i].currentPos.x < minBound.x + circleCenters[i].size){ 
                float delta = (minBound.x + circleCenters[i].size) - circlePhysics[i].currentPos.x;
                circlePhysics[i].currentPos.x += delta;
            }
            if (circlePhysics[i].currentPos.x > maxBound.x - circleCenters[i].size) {
                float delta = circlePhysics[i].currentPos.x - (maxBound.x - circleCenters[i].size);
                circlePhysics[i].currentPos.x -= delta;
            };
            if (circlePhysics[i].currentPos.y < minBound.y + circleCenters[i].size) {
                float delta = (minBound.y + circleCenters[i].size) - circlePhysics[i].currentPos.y;
                circlePhysics[i].currentPos.y += delta;
            };
            if (circlePhysics[i].currentPos.y > maxBound.y - circleCenters[i].size) {
                float delta = circlePhysics[i].currentPos.y - (maxBound.y - circleCenters[i].size);
                circlePhysics[i].currentPos.y -= delta;
            };
        }
    }
}