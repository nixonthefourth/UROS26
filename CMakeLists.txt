cmake_minimum_required(VERSION 4.2)
project(UROS26)

set(CMAKE_CXX_STANDARD 20)

add_executable(UROS26
        src/main.cpp
        include/Vec2.h++
        src/physics.cpp
        include/physics.h++
        src/explicit_euler.cpp
        include/explicit_euler.h++
        src/symplectic_euler.cpp
        include/symplectic_euler.h++
)


target_include_directories(UROS26 PUBLIC include)
target_include_directories(UROS26 PUBLIC src)