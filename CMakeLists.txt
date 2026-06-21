cmake_minimum_required(VERSION 4.2)
project(UROS26)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
execute_process(
        COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
        OUTPUT_VARIABLE pybind11_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
find_package(pybind11 CONFIG REQUIRED)

add_library(UROS26_core
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
        src/integrators/run_registry.cpp
        include/integrators/run_registry.h++
        src/procedural_generation.cpp
        include/procedural_generation.h++
        include/stellar_object.h++
        include/integrators/runtime_timer.h++
)

set_target_properties(UROS26_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(UROS26_core PUBLIC include)
target_include_directories(UROS26_core PUBLIC src)

add_executable(uros26_cli src/main.cpp)
set_target_properties(uros26_cli PROPERTIES OUTPUT_NAME UROS26)
target_link_libraries(uros26_cli PRIVATE UROS26_core)

pybind11_add_module(uros26 src/bindings.cpp)
target_link_libraries(uros26 PRIVATE UROS26_core)
