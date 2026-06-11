//
// Created by Mykyta Khomiakov on 10/06/2026.
//

#include "physics.h++"

namespace physics {

    /// @brief Finds distance between two stellar bodies.
    /// @param a The heavier body.
    /// @param b The lighter orbiting body.
    /// @details Follows a simple Pythagorean of squaring two distances and subtracting them.
    ///          Two subtracted squares are added and their square root is taken.
    /// @return Returns the equation as a float directly.
    float find_distance(const Vec2& a, const Vec2& b) {
        return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    }

    /// @brief Finds the gravitational acceleration between two bodies.
    /// @param m_a Mass of the heavier body in the system.
    /// @param distance Distance between those objects found in find_distance(a, b).
    /// @param a The vector position of the heavier body.
    /// @param b The vector position of the lighter orbiting body.
    /// @param G Constant of gravitational attraction
    /// @details Simplifies Newton's `F=ma` and `F=G*(Mm/r^3)*r_vector.
    ///          Finds vector distance between the two objects for r_vector.
    /// @return Returns the acceleration
    Vec2 find_g_acceleration(float m_a, float distance, const Vec2& a, const Vec2& b, const float G) {
        // Narrowing conversion that auto-promotes int to float to calculate the gravitational component
        float grav_component = (-G * m_a) / pow(distance, 3);
        Vec2 acceleration = grav_component * (b - a);

        return acceleration;
    }

    /// @brief Finds the circular velocity
    /// @param m Mass of the star
    /// @param distance Distance between two object
    /// @param G Constant of gravitational attraction
    /// @return Returns the circular orbital velocity of the object
    float find_velocity(float m, float distance, const float G) {
        return sqrt((G*m) / distance);
    }

    /// @brief Finds the direction of the current velocity.
    /// @param a The current coordinates of the heavier object.
    /// @param b The current coordinates of the orbiting object.
    /// @param distance Distance between the two objects.
    /// @param vel Current velocity of the orbiting object.
    /// @return Returns vector velocity
    Vec2 find_vel_direction(const Vec2& a, const Vec2& b, float distance, float vel) {
        Vec2 radius_vector = b - a;
        Vec2 unit_perpendicular = radius_vector.perpendicular() / distance;
        Vec2 vector_vel = vel * unit_perpendicular;

        return vector_vel;
    }

    /// @brief Finds energy conservation at a current timestep
    /// @param M Mass of the bigger object
    /// @param m Mass of a lighter object
    /// @param G Constant of gravitational attraction
    /// @param distance Distance between the two objects
    /// @param b_velocity Orbiting Object's Velocity
    /// @return Returns the energy conservation of the integrator based on the Hamiltonian framework.
    float find_energy_conservation(float M, float m, const float G, float distance, const Vec2& b_velocity) {
        return 0.5 * m * (pow(b_velocity.x, 2) + pow(b_velocity.y, 2)) - (G * M * m)/distance;
    }

    /// @brief Calculates the relative energy error in the system
    /// @param e_n Energy of the current timestep
    /// @param e_0 Energy at the start
    /// @return Returns the relative error of the energy system
    float energy_error(float e_n, float e_0) {
        return std::abs(e_n - e_0) / std::abs(e_0);
    }

    /// @brief Finds angular momentum at the current timestep
    /// @param m Mass of the orbiting object
    /// @param b_velocity Velocity of the orbiting object
    /// @param b Position of the orbiting object
    /// @return Returns current angular momentum
    float find_angular_momentum(float m, const Vec2& b_velocity, const Vec2&b) {
        return m * ((b.x * b_velocity.y) - (b.y * b_velocity.x));
    }

    /// @brief Calculates the relative error of angular momentum in the system
    /// @param l_n Angular momentum of the current timestep
    /// @param l_0 Angular momentum at the start
    /// @return Returns the relative error of the angular momentum
    float angular_error(float l_n, float l_0) {
        return std::abs(l_n - l_0) / std::abs(l_0);
    }
}
