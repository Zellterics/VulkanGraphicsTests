#pragma once
#include <concepts>
#include <random>

extern std::random_device rd;
extern std::mt19937 rng;
template<std::floating_point T>
T getRandomNumber(T min, T max) {
    if(min > max){
        std::swap(min, max);
    }
    std::uniform_real_distribution<T> dist(min, max);
    return dist(rng);
}

template<std::integral T>
T getRandomNumber(T min, T max) {
    if(min > max){
        std::swap(min, max);
    }
    std::uniform_int_distribution<T> dist(min, max);
    return dist(rng);
}


namespace osd{
    std::string getExecutableDir();
}