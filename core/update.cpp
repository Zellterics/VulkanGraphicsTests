#include "core.h"
#include "physicsObject.h"

void ProtoThiApp::update(float deltaTime){
    static std::vector<PhysicsObject> circlePhysics = {{{-50.f, -50.f},{-50.f, -50.f},{0,0}}};
    glm::vec2 randomPos = {getRandomFloat(-300.0f,300.0f), getRandomFloat(-300.0f,-280.0f)};
    circleCenters.push_back({randomPos,getRandomFloat(4.0f,8.0f), {getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f, getRandomFloat(0.0f,100.0f)/100.0f}});
    circlePhysics.push_back({randomPos, randomPos, {0.f,0.f}});
    const int steps = 2;
    const float sub_dt = deltaTime / (float)steps;
    int x, y;
    glfwGetWindowSize(window, &x, &y);
    x /= 2;
    y /= 2;
    int circleAmount = circleCenters.size();


    for (int i = steps; i; i--){

        std::vector<std::vector<std::vector<int>>> circleID((x + 7) / 8, std::vector<std::vector<int>>((y + 7) / 8));

        for(PhysicsObject& circle : circlePhysics){
            circle.accelerate({0.f, .2f});
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
    
        for (int i = 0; i < circleAmount; i++){
            int gridX = ((circleCenters[i].pos.x + x) / 2) / 8;
            int gridY = ((circleCenters[i].pos.y + y) / 2) / 8;
            if (gridX >= 0 && gridX < (x + 7) / 8 &&
                gridY >= 0 && gridY < (y + 7) / 8) {
                circleID[gridX][gridY].push_back(i);
            }
        }

        int gridWidth = circleID.size();
        int gridHeight = circleID[0].size(); // suponiendo rectangular
        
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {
                for (int ID : circleID[x][y]) {
                    for (int j = -1; j <= 1; j++) {
                        for (int k = -1; k <= 1; k++) {
                            int nx = x + j;
                            int ny = y + k;
        
                            if (nx < 0 || ny < 0 || nx >= gridWidth || ny >= gridHeight)
                                continue;
        
                            for (int neighbor : circleID[nx][ny]) {
                                if (ID == neighbor) continue;
        
                                glm::vec2 collision = circlePhysics[ID].currentPos - circlePhysics[neighbor].currentPos;
                                float dist = glm::length(collision);
                                float summedSize = circleCenters[ID].size + circleCenters[neighbor].size;
        
                                if (dist < summedSize && dist > 0.0001f) {
                                    glm::vec2 n = collision / dist;
                                    float delta = summedSize - dist;
                                    circlePhysics[ID].currentPos += 0.5f * delta * n;
                                    circlePhysics[neighbor].currentPos -= 0.5f * delta * n;
                                }
                            }
                        }
                    }
                }
            }
        }

        // for(int i = 0; i < circleAmount; i++){
        //     for(int j = i + 1; j < circleAmount; j++){
        //         glm::vec2 collision = circlePhysics[i].currentPos - circlePhysics[j].currentPos;
        //         float dist = glm::length(collision);
        //         float summedSize = circleCenters[i].size + circleCenters[j].size;
        //         if(dist < summedSize){
        //             glm::vec2 n = collision / dist;
        //             float delta = summedSize - dist;
        //             circlePhysics[i].currentPos += 0.5f * delta * n;
        //             circlePhysics[j].currentPos -= 0.5f * delta * n;
        //         }
        //     }
        // }
        for(int i = 0; i < circleAmount; i++){
            circlePhysics[i].updatePos(sub_dt);

            circleCenters[i].pos = circlePhysics[i].currentPos;
        }
    }
}