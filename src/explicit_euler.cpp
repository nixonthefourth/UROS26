//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#include "explicit_euler.h++"

namespace explicit_euler {

    /// @brief Updates position in the explicit Euler scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @return Returns the updated position value through the
    ///         r(n+1) = r(n) + v(n) * t formula
    Vec2 position_update(const Vec2& cur_position, const Vec2& cur_velocity, const float t) {
        return cur_position + cur_velocity * t;
    }

    /// @brief Updates current velocity based on the computed acceleration
    /// @details Notice how velocity updates after position
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @param cur_acceleration Current acceleration of the orbiting object
    /// @return Returns the updates velocity value based on the following formula:
    ///         v(n+1) = v(n) + a(n) * t
    Vec2 velocity_update(const Vec2& cur_velocity, const float t, const Vec2& cur_acceleration) {
        return cur_velocity + cur_acceleration * t;
    }

    /// @brief Runs Explicit Euler
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    void run_explicit_euler(const Vec2& star_pos, const Vec2& planet_pos,
                            const float star_mass, const float planet_mass,
                            const float t, const int iterations, const float G) {

        // Find initial conditions of the problem
        float distance = physics::find_distance(star_pos, planet_pos);
        float orbital_speed = physics::find_velocity(star_mass, distance, G);
        Vec2 velocity = physics::find_vel_direction(star_pos, planet_pos, distance ,orbital_speed);
        float initial_energy = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                    distance, velocity);
        float initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos);

        // Runtime
        for (int i = 0; i <= iterations; i++) {
            // Compute new displacement
            Vec2 new_pos = position_update(planet_pos, velocity, t);

            // Compute new distance for conservation equations
            float conservation_distance = physics::find_distance(star_pos, new_pos);

            // Calculate conservations and errors

            // Energy
            float energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                          conservation_distance, velocity);
            float energy_error = physics::energy_error(energy_conservation, initial_energy);

            // Angular Momentum
            float angular_momentum_conservation = physics::find_angular_momentum(planet_mass, velocity, new_pos);
            float angular_error = physics::angular_error(angular_momentum_conservation, initial_angular_momentum);

            // Compute new acceleration
            Vec2 acceleration = physics::find_g_acceleration(star_mass, conservation_distance, star_pos,
                                                            planet_pos, G);

            // Compute new velocity
            velocity = velocity_update(velocity, t, acceleration);
        }
    }
}
