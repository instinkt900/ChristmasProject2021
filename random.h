#pragma once

#include <random>

class Random {
public:
    Random(uint32_t seed)
    :m_generator(seed) {

    }

    template<typename T>
    typename std::enable_if_t<std::is_integral_v<T>, T> Range(T min, T max) {
        std::uniform_int_distribution<> distr(min, max);
        return static_cast<T>(distr(m_generator));
    }

    template<typename T>
    typename std::enable_if_t<std::is_floating_point_v<T>, T> Range(T min, T max) {
        std::uniform_real_distribution<> distr(min, max);
        return static_cast<T>(distr(m_generator));
    }
private:
    std::mt19937 m_generator;
};
