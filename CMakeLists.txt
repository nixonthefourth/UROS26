cmake_minimum_required(VERSION 4.2)
project(UROS26)

set(CMAKE_CXX_STANDARD 20)

add_executable(UROS26
        src/main.cpp
        include/Vec2.h++
        src/physics.cpp
        include/physics.h++
        src/integrators/explicit_euler.cpp
        include/integrators/explicit_euler.h++
        src/integrators/symplectic_euler.cpp
        include/integrators/symplectic_euler.h++
        src/integrators/verlet.cpp
        include/integrators/verlet.h++
        src/integrators/rk4.cpp
        include/integrators/rk4.h++
        src/integrators/leapfrog.cpp
        include/integrators/leapfrog.h++
        src/procedural_generation.cpp
        include/procedural_generation.h++
        include/stellar_object.h++
)


target_include_directories(UROS26 PUBLIC include)
target_include_directories(UROS26 PUBLIC src)
