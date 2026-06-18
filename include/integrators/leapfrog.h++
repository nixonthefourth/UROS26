//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#ifndef UROS26_LEAPFROG_H
#define UROS26_LEAPFROG_H

#include "Vec2.h++"
#include "physics.h++"

namespace leapfrog {

    /// @brief Updates velocity by half a timestep in the leapfrog scheme
    /// @param velocity Current velocity of the orbiting object
    /// @param acceleration Current acceleration of the orbiting object
    /// @param t Timestep
    /// @return Returns the half-step velocity value
    Vec2 half_velocity_update(const Vec2& velocity, const Vec2& acceleration, float t);

    /// @brief Updates position in the leapfrog scheme
    /// @param position Current position of the orbiting object
    /// @param half_velocity Current half-step velocity of the orbiting object
    /// @param t Timestep
    /// @return Returns updated displacement
    Vec2 position_update(const Vec2& position, const Vec2& half_velocity, float t);

    /// @brief Updates velocity by completing the second half of the timestep
    /// @param half_velocity Current half-step velocity of the orbiting object
    /// @param acceleration Newly acquired acceleration
    /// @param t Timestep
    /// @return Returns the full-step velocity value
    Vec2 velocity_update(const Vec2& half_velocity, const Vec2& acceleration, float t);

    /// @brief Runs Leapfrog
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    void run_leapfrog(const Vec2& star_pos, const Vec2& planet_pos,
                      float star_mass, float planet_mass,
                      float t, int iterations, float G);
}

#endif //UROS26_LEAPFROG_H
