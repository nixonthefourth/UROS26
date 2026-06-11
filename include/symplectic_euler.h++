//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#ifndef UROS26_SYMPLECTIC_EULER_H
#define UROS26_SYMPLECTIC_EULER_H

#include "Vec2.h++"
#include "physics.h++"

namespace symplectic_euler {
    /// @brief Updates velocity of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @param cur_acceleration Current acceleration of the orbiting object
    /// @return Returns the updated velocity first prior to displacement
    Vec2 velocity_update(const Vec2& cur_velocity, float t, const Vec2& cur_acceleration);

    /// @brief Updates the position of the orbiting object
    /// @param cur_position Current position of the object
    /// @param t Timestep
    /// @param cur_velocity Current velocity of the orbiting object
    /// @return Returns updated displacement
    Vec2 position_update(const Vec2& cur_position, float t, const Vec2& cur_velocity);

    /// @brief Runtime of the Symplectic Euler
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    void run_symplectic_euler(const Vec2& star_pos, const Vec2& planet_pos,
                            float star_mass, float planet_mass,
                            float t, int iterations, float G);
}

#endif //UROS26_SYMPLECTIC_EULER_H
