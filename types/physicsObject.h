#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <array>

struct PhysicsObject{
    glm::vec2 currentPos;
    glm::vec2 oldPos;
    glm::vec2 acceleration;

    void updatePos(float dt){
        const glm::vec2 velocity = (currentPos - oldPos);
        oldPos = currentPos;
        currentPos = currentPos + velocity + acceleration * dt * dt;
        acceleration = {};
    }

    void accelerate(glm::vec2 acc){
        acceleration += acc;
    }

};