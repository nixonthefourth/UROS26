//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#ifndef UROS26_EXPLICIT_EULER_H
#define UROS26_EXPLICIT_EULER_H

#include "Vec2.h++"
#include "physics.h++"

namespace explicit_euler {
    /// @brief Updates position in the explicit Euler scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @return Returns the updated position value through the
    ///         r(n+1) = r(n) + v(n) * t formula
    Vec2 position_update(const Vec2& cur_position, const Vec2& cur_velocity, float t);

    /// @brief Updates current velocity based on the computed acceleration
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @param cur_acceleration Current acceleration of the orbiting object
    /// @return Returns the updated velocity value based on the following formula:
    ///         v(n+1) = v(n) + a(n) * t
    Vec2 velocity_update(const Vec2& cur_velocity, float t, const Vec2& cur_acceleration);

    /// @brief Runs Explicit Euler
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    void run_explicit_euler(const Vec2& star_pos, const Vec2& planet_pos,
                            float star_mass, float planet_mass,
                            float t, int iterations, float G);
}

#endif //UROS26_EXPLICIT_EULER_H
