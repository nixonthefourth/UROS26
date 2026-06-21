//
// Created for runtime measurements on 21/06/2026.
//

#ifndef UROS26_RUNTIME_TIMER_H
#define UROS26_RUNTIME_TIMER_H

#include <chrono>
#include <iostream>
#include <string_view>

namespace integrators {
    class RuntimeTimer {
    public:
        explicit RuntimeTimer(std::string_view integrator_name)
            : name(integrator_name), start(std::chrono::steady_clock::now()) {
        }

        ~RuntimeTimer() {
            const auto stop = std::chrono::steady_clock::now();
            const std::chrono::duration<double, std::milli> elapsed = stop - start;
            std::cout << name << " runtime: " << elapsed.count() << " ms\n";
        }

    private:
        std::string_view name;
        std::chrono::steady_clock::time_point start;
    };
}

#endif //UROS26_RUNTIME_TIMER_H
