#include "core.h"
#include "physicsObject.h"
#include <thread>
#include "imgui.h"

void ProtoThiApp::update(float deltaTime, float gravity, int dockedSize, float spawnPoint[2], float spawnRadius){
    int x, y;
    static bool changedResolution = false;
    glfwGetWindowSize(window, &x, &y);
    x /= 2;
    y /= 2;
    static std::vector<PhysicsObject> circlePhysics = {{{-50.f, -50.f},{-50.f, -50.f},{0,0}}};
    glm::vec2 randomPos = {
        getRandomFloat((x - dockedSize / 2) * spawnPoint[0] + dockedSize / 2, ((x - dockedSize / 2 - spawnRadius) * spawnPoint[0]) + spawnRadius + dockedSize / 2), 
        getRandomFloat(y * spawnPoint[1], ((y - spawnRadius) * spawnPoint[1]) + spawnRadius)};
    const int BIGGER_RADIUS = 8;
    const int BIGGER_RADIUS_MINUS = BIGGER_RADIUS - 1;
    const int SMALLER_RADIUS = 4;
    circleCenters.push_back({randomPos,getRandomFloat(SMALLER_RADIUS,BIGGER_RADIUS), {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
    circlePhysics.push_back({randomPos, randomPos, {0.f,0.f}});
    const int steps = 2;
    const float sub_dt = deltaTime / (float)steps;
    int circleAmount = circleCenters.size();
    int gridWidth = (x + BIGGER_RADIUS_MINUS) / BIGGER_RADIUS;
    int gridHeight = (y + BIGGER_RADIUS_MINUS) / BIGGER_RADIUS;
    static std::vector<std::vector<int>> circleID(gridWidth * gridHeight);
    // DEAR GOD I NEED TO CLEAN THIS CODE UP, BUT I NEED TO FINISH THIS SOON!!!!
    static int saveX = x, saveY = y;//YEP I NEED MORE DESCRIPTIVE NAMES
    static bool firstTime = true;
    if(saveX != x || saveY != y){
        changedResolution = true;
    }
    if(changedResolution){
        circleID.clear();
        circleID.resize(gridWidth * gridHeight);
        changedResolution = false;
        firstTime = true;
    }
    if (firstTime) {
        for (std::vector<int> &reserveVector : circleID) {
            reserveVector.reserve(8);
        }
        firstTime = false;
    }

    static unsigned const long long int NUM_THREADS = std::max(1u, std::thread::hardware_concurrency());
    for (int i = steps; i; i--){
        
        
        for (std::vector<int> &clearID : circleID){
            clearID.clear();
        }
        for (int i = 0; i < circleAmount; i++){
            int gridX = ((circleCenters[i].pos.x + x) / 2) / BIGGER_RADIUS;
            int gridY = ((circleCenters[i].pos.y + y) / 2) / BIGGER_RADIUS;
            int index = (gridY * gridWidth) + gridX;
            if (gridX >= 0 && gridX < gridWidth &&
                gridY >= 0 && gridY < gridHeight) {
                circleID[index].push_back(i);
            }
        }

        for(PhysicsObject& circle : circlePhysics){
            circle.accelerate({0.f, gravity});
        }
        auto calculateCollissions = [&] (int start, int finish){
        for (int x = start; x < finish; x++) { // 20 veces extremadamente rapido
            for (int y = 0; y < gridHeight; y++) { // 20 veces extremadamente rapido
                for (int ID : circleID[(y * gridWidth) + x]) {     // cantidad de objetos
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
        
                            for (int neighbor : circleID[(ny * gridWidth) + nx]) {
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
            const glm::vec2 minBound{-x + dockedSize, -y};
            const glm::vec2 maxBound{ x, y};

            if (circlePhysics[i].currentPos.x < minBound.x + circleCenters[i].size){ 
                circlePhysics[i].currentPos.x = minBound.x + circleCenters[i].size;
            }
            if (circlePhysics[i].currentPos.x > maxBound.x - circleCenters[i].size) {
                circlePhysics[i].currentPos.x = maxBound.x - circleCenters[i].size;
            }
            if (circlePhysics[i].currentPos.y < minBound.y + circleCenters[i].size) {
                circlePhysics[i].currentPos.y = minBound.y + circleCenters[i].size;
            }
            if (circlePhysics[i].currentPos.y > maxBound.y - circleCenters[i].size) {
                circlePhysics[i].currentPos.y = maxBound.y - circleCenters[i].size;
            }
        }

    }
}