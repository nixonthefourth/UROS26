//
// Created by Mykyta Khomiakov on 24/07/2026.
//

#include "integrators/leapfrog_4th.h++"
#include <chrono>
#include <cmath>

namespace leapfrog_4th {

    // Yoshida 4th-order composition coefficients
    // w1 + w0 + w1 = 1, chosen to cancel the leading (3rd order) error term
    static const double YOSHIDA_W1 = 1.0 / (2.0 - std::cbrt(2.0));
    static const double YOSHIDA_W0 = -std::cbrt(2.0) / (2.0 - std::cbrt(2.0));

    /// @brief Performs a single classic leapfrog sub-step of size `sub_t`,
    /// used as the building block composed three times in Yoshida's scheme
    /// @param star_pos Position of the star
    /// @param displacement Current position of the planet (updated in place)
    /// @param velocity Current velocity of the planet (updated in place)
    /// @param distance Current star-planet distance (updated in place)
    /// @param star_mass Mass of the star
    /// @param sub_t Sub-step size (w1*t or w0*t)
    /// @param G Gravitational constant
    void leapfrog_substep(const Vec2& star_pos, Vec2& displacement, Vec2& velocity,
                          double& distance, const double star_mass, const double sub_t,
                          const double G) {
        Vec2 init_acceleration = physics::find_g_acceleration(star_mass, distance, star_pos, displacement, G);
        Vec2 half_velocity = leapfrog::half_velocity_update(velocity, init_acceleration, sub_t);
        displacement = leapfrog::position_update(displacement, half_velocity, sub_t);
        distance = physics::find_distance(star_pos, displacement);
        Vec2 new_acceleration = physics::find_g_acceleration(star_mass, distance, star_pos, displacement, G);
        velocity = leapfrog::velocity_update(half_velocity, new_acceleration, sub_t);
    }


    /// @brief Runs Yoshida 4th-order leapfrog by composing three 2nd-order
    /// leapfrog sub-steps per outer timestep with coefficients w1, w0, w1
    /// @param star_pos Current position of the star
    /// @param planet_pos Current position of the planet
    /// @param star_mass Mass of the Star
    /// @param planet_mass Mass of the planet
    /// @param t Outer timestep
    /// @param iterations Number of outer iterations algorithm should run
    /// @param G Gravitational constant
    integrators::RunSummary run_yoshida4(const Vec2& star_pos, const Vec2& planet_pos,
                                         const double star_mass, const double planet_mass,
                                         const double t, const int iterations, const double G,
                                         const std::string& output_csv) {
        double distance = physics::find_distance(star_pos, planet_pos);
        double orbital_speed = physics::find_velocity(star_mass, distance, G);
        Vec2 displacement = planet_pos;
        Vec2 velocity = physics::find_vel_direction(star_pos, planet_pos, distance, orbital_speed);
        double initial_energy = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                    distance, velocity);
        double initial_angular_momentum = physics::find_angular_momentum(planet_mass, velocity, planet_pos - star_pos);
        integrators::RunRegistry registry(output_csv);
        registry.record(0, 0.0, displacement, velocity, distance, initial_energy, initial_angular_momentum, 0.0, 0.0);

        const auto start = std::chrono::steady_clock::now();
        for (int i = 1; i <= iterations; i++) {
            // Three composed leapfrog sub-steps: w1*t, w0*t, w1*t
            leapfrog_substep(star_pos, displacement, velocity, distance, star_mass, YOSHIDA_W1 * t, G);
            leapfrog_substep(star_pos, displacement, velocity, distance, star_mass, YOSHIDA_W0 * t, G);
            leapfrog_substep(star_pos, displacement, velocity, distance, star_mass, YOSHIDA_W1 * t, G);

            double energy_conservation = physics::find_energy_conservation(star_mass, planet_mass, G,
                                                                          distance, velocity);
            double energy_error = physics::energy_error(energy_conservation, initial_energy);
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
