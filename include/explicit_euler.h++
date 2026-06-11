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
    /// @details Notice how velocity updates after position
    /// @param cur_velocity Current velocity of the orbiting object
    /// @param t Timestep
    /// @param cur_acceleration Current acceleration of the orbiting object
    /// @return Returns the updates velocity value based on the following formula:
    ///         v(n+1) = v(n) + a(n) * t
    Vec2 velocity_update(const Vec2& cur_velocity, float t, const Vec2& cur_acceleration);
}

#endif //UROS26_EXPLICIT_EULER_H
