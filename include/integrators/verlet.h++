//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#ifndef UROS26_VERLET_H
#define UROS26_VERLET_H

#include "Vec2.h++"
#include "integrators/run_registry.h++"
#include "physics.h++"
#include <string>

namespace verlet {

    /// @brief Updates position of the orbiting object given acceleration
    /// @param position Current position of the object
    /// @param velocity Current velocity of the object
    /// @param acceleration Given acceleration
    /// @param t Timestep
    /// @return Returns updated position
    Vec2 update_position(const Vec2& position, const Vec2& velocity, const Vec2& acceleration, float t);

    /// @brief Updates the velocity of the orbiting object
    /// @param velocity Current velocity of the object
    /// @param prior_acceleration Prior acceleration
    /// @param new_acceleration Newly acquired acceleration
    /// @param t Timestep
    /// @return Returns updated velocity
    Vec2 update_velocity(const Vec2& velocity, const Vec2& prior_acceleration, const Vec2& new_acceleration, float t);

    /// @brief Runs Velocity Verlet
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_verlet(const Vec2& star_pos, const Vec2& planet_pos,
                                       float star_mass, float planet_mass,
                                       float t, int iterations, float G,
                                       const std::string& output_csv = "");
}

#endif //UROS26_VERLET_H
