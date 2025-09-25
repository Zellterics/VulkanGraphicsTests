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
};

