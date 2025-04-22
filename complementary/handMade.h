//TESTS ERASE LATER
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

int getRandomFloat(int min, int max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}