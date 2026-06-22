//
// Created by Mykyta Khomiakov on 11/06/2026.
//

#include "integrators/explicit_euler.h++"

#include <chrono>

namespace explicit_euler {

    /// @brief Updates position in the explicit Euler scheme
    /// @param cur_position Current position of the orbiting object
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @return Returns the updated position value through the
    ///         r(n+1) = r(n) + v(n) * t formula
    Vec2 position_update(const Vec2& cur_position, const Vec2& cur_velocity, const double t) {
        return cur_position + cur_velocity * t;
    }

    /// @brief Updates current velocity based on the computed acceleration
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @param cur_acceleration Current acceleration of the orbiting object
    /// @return Returns the updated velocity value based on the following formula:
    ///         v(n+1) = v(n) + a(n) * t
    Vec2 velocity_update(const Vec2& cur_velocity, const double t, const Vec2& cur_acceleration) {
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
    integrators::RunSummary run_explicit_euler(const Vec2& star_pos, const Vec2& planet_pos,
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
        double initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos - star_pos);
        integrators::RunRegistry registry(output_csv);
        registry.record(0, 0.0, displacement, velocity, distance, initial_energy, initial_angular_momentum, 0.0, 0.0);

        const auto start = std::chrono::steady_clock::now();
        // Runtime
        for (int i = 1; i <= iterations; i++) {
            // Compute acceleration
            Vec2 acceleration = physics::find_g_acceleration(star_mass, distance, star_pos,
                                                            displacement, G);

            // Compute new displacement
            displacement = position_update(displacement, velocity, t);

            // Compute new velocity
            velocity = velocity_update(velocity, t, acceleration);

            // Compute new distance for conservation equations
            distance = physics::find_distance(star_pos, displacement);

            // Calculate conservations and errors

            // Energy
            double energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                          distance, velocity);
            double energy_error = physics::energy_error(energy_conservation, initial_energy);

            // Angular Momentum
            double angular_momentum_conservation = physics::find_angular_momentum(planet_mass, velocity,
                                                                                 displacement - star_pos);
            double angular_error = physics::angular_error(angular_momentum_conservation, initial_angular_momentum);
            registry.record(i, i * t, displacement, velocity, distance, energy_conservation,
                            angular_momentum_conservation, energy_error, angular_error);
        }

        const auto stop = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = stop - start;
        return registry.finalize(elapsed.count());
    }
}
