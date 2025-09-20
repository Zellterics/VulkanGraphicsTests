//TESTS ERASE LATER
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

float getRandomFloat(float min, float max) {
    if(min > max){
        std::swap(min, max);
    }
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

namespace osd{
    std::string getExecutableDir();
}