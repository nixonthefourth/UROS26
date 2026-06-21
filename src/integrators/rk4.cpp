//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#include "integrators/rk4.h++"
#include "integrators/runtime_timer.h++"

namespace rk4 {

    /// @brief Finds acceleration at a given position
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param G Gravitational constant
    /// @return Returns acceleration of the orbiting object at the given position
    Vec2 acceleration_update(const Vec2& star_pos, const Vec2& planet_pos, const float star_mass, const float G) {
        float distance = physics::find_distance(star_pos, planet_pos);

        return physics::find_g_acceleration(star_mass, distance, star_pos, planet_pos, G);
    }

    /// @brief Updates position in the RK4 scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param star_pos Current position of the star
    /// @param star_mass Mass of the Star
    /// @param t Timestep
    /// @param G Gravitational constant
    /// @return Returns the updated position value using the fourth-order Runge-Kutta method
    Vec2 position_update(const Vec2& cur_position, const Vec2& cur_velocity,
                         const Vec2& star_pos, const float star_mass, const float t, const float G) {
        Vec2 k1_position = cur_velocity;
        Vec2 k1_velocity = acceleration_update(star_pos, cur_position, star_mass, G);

        Vec2 k2_position = cur_velocity + 0.5f * t * k1_velocity;
        Vec2 k2_velocity = acceleration_update(star_pos, cur_position + 0.5f * t * k1_position, star_mass, G);

        Vec2 k3_position = cur_velocity + 0.5f * t * k2_velocity;
        Vec2 k3_velocity = acceleration_update(star_pos, cur_position + 0.5f * t * k2_position, star_mass, G);

        Vec2 k4_position = cur_velocity + t * k3_velocity;

        return cur_position + (t / 6.0f) * (k1_position + 2.0f * k2_position + 2.0f * k3_position + k4_position);
    }

    /// @brief Updates velocity in the RK4 scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param star_pos Current position of the star
    /// @param star_mass Mass of the Star
    /// @param t Timestep
    /// @param G Gravitational constant
    /// @return Returns the updated velocity value using the fourth-order Runge-Kutta method
    Vec2 velocity_update(const Vec2& cur_position, const Vec2& cur_velocity,
                         const Vec2& star_pos, const float star_mass, const float t, const float G) {
        Vec2 k1_position = cur_velocity;
        Vec2 k1_velocity = acceleration_update(star_pos, cur_position, star_mass, G);

        Vec2 k2_position = cur_velocity + 0.5f * t * k1_velocity;
        Vec2 k2_velocity = acceleration_update(star_pos, cur_position + 0.5f * t * k1_position, star_mass, G);

        Vec2 k3_position = cur_velocity + 0.5f * t * k2_velocity;
        Vec2 k3_velocity = acceleration_update(star_pos, cur_position + 0.5f * t * k2_position, star_mass, G);

        Vec2 k4_velocity = acceleration_update(star_pos, cur_position + t * k3_position, star_mass, G);

        return cur_velocity + (t / 6.0f) * (k1_velocity + 2.0f * k2_velocity + 2.0f * k3_velocity + k4_velocity);
    }

    /// @brief Runs RK4
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    void run_rk4(const Vec2& star_pos, const Vec2& planet_pos,
                 const float star_mass, const float planet_mass,
                 const float t, const int iterations, const float G) {
        integrators::RuntimeTimer timer("RK4");

        // Find initial conditions of the problem
        float distance = physics::find_distance(star_pos, planet_pos);
        float orbital_speed = physics::find_velocity(star_mass, distance, G);
        Vec2 displacement = planet_pos;
        Vec2 velocity = physics::find_vel_direction(star_pos, planet_pos, distance ,orbital_speed);
        float initial_energy = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                    distance, velocity);
        float initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos);

        // Runtime
        for (int i = 0; i <= iterations; i++) {
            // Compute new displacement
            Vec2 new_pos = position_update(displacement, velocity, star_pos, star_mass, t, G);

            // Compute new velocity
            velocity = velocity_update(displacement, velocity, star_pos, star_mass, t, G);

            // Update displacement
            displacement = new_pos;

            // Compute new distance for conservation equations
            distance = physics::find_distance(star_pos, displacement);

            // Calculate conservations and errors

            // Energy
            float energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                          distance, velocity);
            float energy_error = physics::energy_error(energy_conservation, initial_energy);

            // Angular Momentum
            float angular_momentum_conservation = physics::find_angular_momentum(planet_mass, velocity, displacement);
            float angular_error = physics::angular_error(angular_momentum_conservation, initial_angular_momentum);
        }
    }
}
