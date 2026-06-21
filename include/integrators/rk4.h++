//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#ifndef UROS26_RK4_H
#define UROS26_RK4_H

#include "Vec2.h++"
#include "integrators/run_registry.h++"
#include "physics.h++"
#include <string>

namespace rk4 {

    /// @brief Finds acceleration at a given position
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param G Gravitational constant
    /// @return Returns acceleration of the orbiting object at the given position
    Vec2 acceleration_update(const Vec2& star_pos, const Vec2& planet_pos, float star_mass, float G);

    /// @brief Updates position in the RK4 scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param star_pos Current position of the star
    /// @param star_mass Mass of the Star
    /// @param t Timestep
    /// @param G Gravitational constant
    /// @return Returns the updated position value using the fourth-order Runge-Kutta method
    Vec2 position_update(const Vec2& cur_position, const Vec2& cur_velocity,
                         const Vec2& star_pos, float star_mass, float t, float G);

    /// @brief Updates velocity in the RK4 scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param star_pos Current position of the star
    /// @param star_mass Mass of the Star
    /// @param t Timestep
    /// @param G Gravitational constant
    /// @return Returns the updated velocity value using the fourth-order Runge-Kutta method
    Vec2 velocity_update(const Vec2& cur_position, const Vec2& cur_velocity,
                         const Vec2& star_pos, float star_mass, float t, float G);

    /// @brief Runs RK4
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_rk4(const Vec2& star_pos, const Vec2& planet_pos,
                                    float star_mass, float planet_mass,
                                    float t, int iterations, float G,
                                    const std::string& output_csv = "");
}

#endif //UROS26_RK4_H
