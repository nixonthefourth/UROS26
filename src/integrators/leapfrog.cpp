//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#include "integrators/leapfrog.h++"

#include <chrono>

namespace leapfrog {

    /// @brief Updates velocity by half a timestep in the leapfrog scheme
    /// @param velocity Current velocity of the orbiting object
    /// @param acceleration Current acceleration of the orbiting object
    /// @param t Timestep
    /// @return Returns the half-step velocity value
    Vec2 half_velocity_update(const Vec2& velocity, const Vec2& acceleration, const float t) {
        return velocity + acceleration * (0.5f * t);
    }

    /// @brief Updates position in the leapfrog scheme
    /// @param position Current position of the orbiting object
    /// @param half_velocity Current half-step velocity of the orbiting object
    /// @param t Timestep
    /// @return Returns updated displacement
    Vec2 position_update(const Vec2& position, const Vec2& half_velocity, const float t) {
        return position + half_velocity * t;
    }

    /// @brief Updates velocity by completing the second half of the timestep
    /// @param half_velocity Current half-step velocity of the orbiting object
    /// @param acceleration Newly acquired acceleration
    /// @param t Timestep
    /// @return Returns the full-step velocity value
    Vec2 velocity_update(const Vec2& half_velocity, const Vec2& acceleration, const float t) {
        return half_velocity + acceleration * (0.5f * t);
    }

    /// @brief Runs Leapfrog
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_leapfrog(const Vec2& star_pos, const Vec2& planet_pos,
                                         const float star_mass, const float planet_mass,
                                         const float t, const int iterations, const float G,
                                         const std::string& output_csv) {
        // Find initial conditions of the problem
        float distance = physics::find_distance(star_pos, planet_pos);
        float orbital_speed = physics::find_velocity(star_mass, distance, G);
        Vec2 displacement = planet_pos;
        Vec2 velocity = physics::find_vel_direction(star_pos, planet_pos, distance ,orbital_speed);
        float initial_energy = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                    distance, velocity);
        float initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos);
        integrators::RunRegistry registry(output_csv);
        registry.record(0, 0.0, displacement, velocity, distance, initial_energy, initial_angular_momentum, 0.0, 0.0);

        const auto start = std::chrono::steady_clock::now();
        // Runtime
        for (int i = 1; i <= iterations; i++) {
            // Compute acceleration
            Vec2 init_acceleration = physics::find_g_acceleration(star_mass, distance, star_pos, displacement, G);

            // Compute half-step velocity
            Vec2 half_velocity = half_velocity_update(velocity, init_acceleration, t);

            // Compute new displacement
            displacement = position_update(displacement, half_velocity, t);

            // Compute new distance
            distance = physics::find_distance(star_pos, displacement);

            // Compute new acceleration
            Vec2 new_acceleration = physics::find_g_acceleration(star_mass, distance, star_pos, displacement, G);

            // Compute new velocity
            velocity = velocity_update(half_velocity, new_acceleration, t);

            // Calculate energy conservation and angular momentum
            // Energy
            float energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                          distance, velocity);
            float energy_error = physics::energy_error(energy_conservation, initial_energy);

            // Angular Momentum
            float angular_momentum_conservation = physics::find_angular_momentum(planet_mass, velocity, displacement);
            float angular_error = physics::angular_error(angular_momentum_conservation, initial_angular_momentum);
            registry.record(i, i * t, displacement, velocity, distance, energy_conservation,
                            angular_momentum_conservation, energy_error, angular_error);
        }

        const auto stop = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = stop - start;
        return registry.finalize(elapsed.count());
    }
}
