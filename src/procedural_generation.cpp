//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#include "procedural_generation.h++"

namespace generate {

    /// @brief Generates solar mass using Mersenne-Twister pseudo-random engine
    /// @return Returns uniform real distribution of the values between 1.0 and 2.4
    float solar_mass() {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> distribution(1.f, 2.4);

        return distribution(gen);
    }

    /// @brief Derives planetary mass of the orbiting object from the prior solar mass
    /// @param stellar_mass Substituted stellar mass
    /// @return Returns derivation of M/333000
    float planetary_mass(float const stellar_mass) {
        return stellar_mass/333000;
    }

    /// @brief Generates separation using Mersenne-Twister pseudo-random engine
    /// @return Returns Vec2 type with values for x and y coordinates between 1.f and 5.f.
    Vec2 separation_distance() {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> x_distribution(1.f, 5.f);
        std::uniform_real_distribution<float> y_distribution(1.f, 5.f);

        return {x_distribution(gen), y_distribution(gen)};
    }
}
