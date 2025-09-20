//TESTS ERASE LATER
#include <random>

extern std::random_device rd;
extern std::mt19937 rng;

float getRandomFloat(float min, float max);

namespace osd{
    std::string getExecutableDir();
}