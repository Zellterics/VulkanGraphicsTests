#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

struct PhysicsObject{
    glm::vec2 currentPos;
    glm::vec2 oldPos;
    glm::vec2 acceleration;

    void updatePos(float dt){
        const glm::vec2 velocity = (currentPos - oldPos);
        oldPos = currentPos;
        const float RESISTANCE = .3f;
        currentPos = currentPos + velocity + (acceleration - velocity * RESISTANCE) * dt * dt;
        acceleration = {0.0f, 0.0f};
    }

    void accelerate(glm::vec2 acc){
        acceleration += acc;
    }

};