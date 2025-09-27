#include <ThING/core.h>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <ThING/types/vertex.h>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

int ProtoThiApp::getCircleAmount(){
    return circleCenters.size();
}

void ProtoThiApp::getWindowSize(int* x, int* y){
    glfwGetWindowSize(windowManager.getWindow(), x, y);
    return;
}

void ProtoThiApp::addCircle(glm::vec2 pos, float size, glm::vec3 color){
    circleCenters.push_back({pos, size, color});
}

std::vector<Circle>* ProtoThiApp::getCircleDrawVector(){
    return &circleCenters;
}

void ProtoThiApp::setZoomAndOffset(float zoom, glm::vec2 offset){
    this->zoom = zoom;
    this->offset = offset;
    bufferManager.setUpdateUBOFlag(true);
}

void ProtoThiApp::setBackgroundColor(glm::vec4 color){
    clearColor.color.float32[0] = color.x;
    clearColor.color.float32[1] = color.y;
    clearColor.color.float32[2] = color.z;
    clearColor.color.float32[3] = color.w;
}

glm::mat4 ProtoThiApp::build2DTransform(glm::vec2 pos, float rotation, glm::vec2 scale) {
    glm::mat4 t(1.0f);
    t = glm::translate(t, glm::vec3(pos, 0.0f));
    t = glm::rotate(t, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    t = glm::scale(t, glm::vec3(scale, 1.0f));
    return t;
}


void ProtoThiApp::setRotation(glm::vec2 pos, float rotation, glm::vec2 scale){
    polygons[0].pushConstant.tranform = build2DTransform(
            pos,
            glm::radians(rotation),
            scale
        );
}

std::string ProtoThiApp::makeUniqueId(std::string baseId) {
    std::string uniqueId = baseId;
    int counter = 1;

    bool exists = true;
    while (exists) {
        exists = false;
        for (const Polygon& pol : polygons) {
            if (pol.id == uniqueId) {
                exists = true;
                uniqueId = baseId + std::to_string(counter++);
                break;
            }
        }
    }
    return uniqueId;
}


void ProtoThiApp::addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>& ver, std::vector<uint16_t>& ind){
    polygons.push_back({makeUniqueId(id), 
        static_cast<uint32_t>(vertices.size()), 
        static_cast<uint32_t>(ver.size()),
        static_cast<uint32_t>(indices.size()),
        static_cast<uint32_t>(ind.size()),
        true,
        build2DTransform(pos, rotation, scale)
    });
    vertices.reserve(vertices.size() + ver.size());
    indices.reserve(indices.size() + indices.size());
    vertices.insert(vertices.end(), ver.begin(), ver.end());
    indices.insert(indices.end(), ind.begin(), ind.end());
}

void ProtoThiApp::addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>&& ver, std::vector<uint16_t>&& ind){
    polygons.push_back({makeUniqueId(id), 
        static_cast<uint32_t>(vertices.size()), 
        static_cast<uint32_t>(ver.size()),
        static_cast<uint32_t>(indices.size()),
        static_cast<uint32_t>(ind.size()),
        true,
        build2DTransform(pos, rotation, scale)
    });
    vertices.reserve(vertices.size() + ver.size());
    indices.reserve(indices.size() + ind.size());
    vertices.insert(vertices.end(), std::make_move_iterator(ver.begin()), std::make_move_iterator(ver.end()));
    indices.insert(indices.end(), std::make_move_iterator(ind.begin()), std::make_move_iterator(ind.end()));
    std::cout << vertices.size() << std::endl;
    std::cout << indices.size() << std::endl;
}