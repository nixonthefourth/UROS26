//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#include "integrators/verlet.h++"

#include <chrono>

namespace verlet {

    /// @brief Updates position of the orbiting object given acceleration
    /// @param position Current position of the object
    /// @param velocity Current velocity of the object
    /// @param acceleration Given acceleration
    /// @param t Timestep
    /// @return Returns updated position
    Vec2 update_position(const Vec2& position, const Vec2& velocity, const Vec2& acceleration, const double t) {
        Vec2 velocity_component = velocity * t;
        Vec2 acceleration_component = 0.5 * acceleration * (t * t);

        return position + velocity_component + acceleration_component;
    }

    /// @brief Updates the velocity of the orbiting object
    /// @param velocity Current velocity of the object
    /// @param prior_acceleration Prior acceleration
    /// @param new_acceleration Newly acquired acceleration
    /// @param t Timestep
    /// @return Returns updated velocity
    Vec2 update_velocity(const Vec2& velocity, const Vec2& prior_acceleration, const Vec2& new_acceleration,
                         const double t) {
        return velocity + 0.5 * (prior_acceleration + new_acceleration) * t;
    }

    /// @brief Runs Velocity Verlet
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Timestep
    /// @param iterations Number of iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_verlet(const Vec2& star_pos, const Vec2& planet_pos,
                                       const double star_mass, const double planet_mass,
                                       const double t, const int iterations, const double G,
                                       const std::string& output_csv) {
        // Find initial conditions of the problem
        double distance = physics::find_distance(star_pos, planet_pos);
        double orbital_speed = physics::find_velocity(star_mass, distance, G);
        Vec2 displacement = planet_pos;
        Vec2 velocity = physics::find_vel_direction(star_pos, planet_pos, distance ,orbital_speed);
        double initial_energy = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                    distance, velocity);
        double initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos);        integrators::RunRegistry registry(output_csv);
        registry.record(0, 0.0, displacement, velocity, distance, initial_energy, initial_angular_momentum, 0.0, 0.0);

        const auto start = std::chrono::steady_clock::now();
        
        // Runtime
        for (int i = 1; i <= iterations; i++) {

            // Compute acceleration using current distance (correct)
            Vec2 init_acceleration = physics::find_g_acceleration(star_mass, distance, star_pos, displacement, G);

            // Compute new displacement
            displacement = update_position(displacement, velocity, init_acceleration, t);

            // Compute new distance — use a separate variable
            double new_distance = physics::find_distance(star_pos, displacement);

            // Compute new acceleration with the new distance
            Vec2 new_acceleration = physics::find_g_acceleration(star_mass, new_distance, star_pos, displacement, G);

            // Compute new velocity
            velocity = update_velocity(velocity, init_acceleration, new_acceleration, t);

            double energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                        new_distance, velocity);
            double energy_error = physics::energy_error(energy_conservation, initial_energy);

            double angular_momentum_conservation = physics::find_angular_momentum(planet_mass, velocity, displacement);
            double angular_error = physics::angular_error(angular_momentum_conservation, initial_angular_momentum);

            registry.record(i, i * t, displacement, velocity, new_distance, energy_conservation,
                            angular_momentum_conservation, energy_error, angular_error);

            distance = new_distance; // update for next iteration
        }

        const auto stop = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = stop - start;
        return registry.finalize(elapsed.count());
    }
}
