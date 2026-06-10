cmake_minimum_required(VERSION 4.2)
project(UROS26)

set(CMAKE_CXX_STANDARD 20)

add_executable(UROS26
        src/main.cpp
        include/Vec2.h++
        src/physics.cpp
        include/physics.h++
)


target_include_directories(UROS26 PUBLIC include)
target_include_directories(UROS26 PUBLIC src)