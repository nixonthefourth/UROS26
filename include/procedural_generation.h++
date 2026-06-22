//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#ifndef UROS26_PROCEDURAL_GENERATION_H
#define UROS26_PROCEDURAL_GENERATION_H

#include <random>
#include "Vec2.h++"

namespace generate {
    struct ProblemSetup {
        double star_mass;
        double planet_mass;
        Vec2 star_pos;
        Vec2 planet_pos;
    };

    /// @brief Generates solar mass using Mersenne-Twister pseudo-random engine
    /// @return Returns uniform real distribution of the values between 1.0 and 2.4
    double solar_mass();

    /// @brief Derives planetary mass of the orbiting object from the prior solar mass
    /// @param stellar_mass Substituted stellar mass
    /// @return Returns derivation of M/333000
    double planetary_mass(double stellar_mass);

    /// @brief Generates separation using Mersenne-Twister pseudo-random engine
    /// @return Returns Vec2 type with values for x and y coordinates between 1.f and 5.f.
    Vec2 separation_distance();

    /// @brief Generates a complete reproducible two-body problem from a fixed seed.
    /// @param seed Seed used by the Mersenne-Twister pseudo-random engine.
    /// @return Returns masses and initial positions for the simulation.
    ProblemSetup problem_setup(unsigned int seed);
}

#endif //UROS26_PROCEDURAL_GENERATION_H
