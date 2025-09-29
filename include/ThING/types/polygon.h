#include <cstdint>
#include <glm/glm.hpp>
#include <string>

struct PushConstantData {
    glm::mat4 tranform;
};

struct Polygon{
    std::string id;
    uint32_t vertexOffset;
    uint32_t vertexCount;
    uint32_t indexOffset;
    uint32_t indexCount;
    bool alive;
    PushConstantData pushConstant;
    static constexpr unsigned long PushConstantSize(){
        return sizeof(PushConstantData);
    }
    Polygon(){
        id = NULL_POLYGON_ID;
        alive = false;
    }
    Polygon(std::string id){
        this->id = id;
    }
    Polygon(const std::string& id, const uint32_t& vertexOffset, const uint32_t& vertexCount, const uint32_t& indexOffset, const uint32_t& indexCount, const bool& alive, const PushConstantData& pushConstant){
        this->id = id;
        this->vertexOffset = vertexOffset;
        this->vertexCount = vertexCount;
        this->indexOffset = indexOffset;
        this->indexCount = indexCount;
        this->alive = alive;
        this->pushConstant = pushConstant;
    }
};

