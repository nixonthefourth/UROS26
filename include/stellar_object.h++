//
// Created by Mykyta Khomiakov on 18/06/2026.
//

#ifndef UROS26_STELLAR_OBJECT_H
#define UROS26_STELLAR_OBJECT_H

#include <utility>

#include "Vec2.h++"

// Class of the stellar objects
class stellar_object {
private: // Separation of concerns. Private parameters.
    const float mass; // Object's relative mass
    std::string name; // Name
    Vec2 position; // Relative position based on a prior Vec2 definition

public: // Public accessors
    // Constructor
    stellar_object(float m, std::string  n, const Vec2& pos, const Vec2& v)
    : mass(m), name(std::move(n)), position(pos) {
    }

    // Getters
    [[nodiscard]] float get_mass() const { return mass; } // Get mass
    [[nodiscard]] const std::string& get_name() const { return name; } // Get name by reference
    Vec2& get_position() { return position; } // Get mutable position by reference
    [[nodiscard]] const Vec2& get_position() const { return position; } // Get const position by reference

    // Setters
    void set_position(const Vec2& pos) { position = pos; } // Set Position
};

#endif //UROS26_STELLAR_OBJECT_H
