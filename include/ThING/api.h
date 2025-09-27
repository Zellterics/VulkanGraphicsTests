#pragma once
#include <ThING/core.h>

namespace ThING{
    class API{
    public:
        API();
        ~API();

        bool setUpdateCallback(std::function<void(ThING::API&, FPSCounter&)>);
        bool setUICallback(std::function<void(ThING::API&, FPSCounter&)>);
        void run();

        int getCircleAmount();
        void getWindowSize(int* x, int* y);
        void addCircle(glm::vec2 pos, float size, glm::vec3 color);
        std::vector<Circle>* getCircleDrawVector();
        void setZoomAndOffset(float zoom, glm::vec2 offset);
        void setBackgroundColor(glm::vec4 color);
        glm::mat4 build2DTransform(glm::vec2 pos, float rotation, glm::vec2 scale);
        void setRotation(glm::vec2 pos, float rotation, glm::vec2 scale);
        std::string makeUniqueId(std::string baseId);
        void addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>& ver, std::vector<uint16_t>& ind);
        void addPolygon(std::string& id, glm::vec2 pos, float rotation, glm::vec2 scale, std::vector<Vertex>&& ver, std::vector<uint16_t>&& ind);
    private:
        void mainLoop();

        std::function<void(ThING::API&, FPSCounter&)> updateCallback;
        std::function<void(ThING::API&, FPSCounter&)> uiCallback;

        


        ProtoThiApp app;
    };
}