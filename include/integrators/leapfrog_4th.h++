//
// Created by Mykyta Khomiakov on 24/07/2026.
//

#ifndef UROS26_LEAPFROG_4TH_H
#define UROS26_LEAPFROG_4TH_H

#include "Vec2.h++"
#include "physics.h++"
#include "integrators/run_registry.h++"
#include <string>
#include "leapfrog.h++"

namespace leapfrog_4th {
    /// @brief Performs a single classic leapfrog sub-step of size `sub_t`,
    /// used as the building block composed three times in Yoshida's scheme
    /// @param star_pos Position of the star
    /// @param displacement Current position of the planet (updated in place)
    /// @param velocity Current velocity of the planet (updated in place)
    /// @param distance Current star-planet distance (updated in place)
    /// @param star_mass Mass of the star
    /// @param sub_t Sub-step size (w1*t or w0*t)
    /// @param G Gravitational constant
    void leapfrog_substep(const Vec2& star_pos, Vec2& displacement, Vec2& velocity,
                          double& distance, double star_mass, double sub_t,
                          double G);

    /// @brief Runs Yoshida 4th-order leapfrog by composing three 2nd-order
    /// leapfrog sub-steps per outer timestep with coefficients w1, w0, w1
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Outer timestep
    /// @param iterations Number of outer iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_yoshida4(const Vec2& star_pos, const Vec2& planet_pos,
                                         double star_mass, double planet_mass,
                                         double t, int iterations, double G,
                                         const std::string& output_csv = "");
}


#endif //UROS26_LEAPFROG_4TH_H
