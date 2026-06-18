//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#ifndef UROS26_PROCEDURAL_GENERATION_H
#define UROS26_PROCEDURAL_GENERATION_H

#include <random>
#include "Vec2.h++"

namespace generate {
    /// @brief Generates solar mass using Mersenne-Twister pseudo-random engine
    /// @return Returns uniform real distribution of the values between 1.0 and 2.4
    float solar_mass();

    /// @brief Derives planetary mass of the orbiting object from the prior solar mass
    /// @param stellar_mass Substituted stellar mass
    /// @return Returns derivation of M/333000
    float planetary_mass(float stellar_mass);

    /// @brief Generates separation using Mersenne-Twister pseudo-random engine
    /// @return Returns Vec2 type with values for x and y coordinates between 1.f and 5.f.
    Vec2 separation_distance();
}

#endif //UROS26_PROCEDURAL_GENERATION_H
