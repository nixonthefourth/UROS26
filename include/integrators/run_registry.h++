#ifndef UROS26_RUN_REGISTRY_H
#define UROS26_RUN_REGISTRY_H

#include "Vec2.h++"

#include <fstream>
#include <limits>
#include <string>
#include <vector>

namespace integrators {
    struct RunSummary {
        double final_energy_relative_error = 0.0;
        double final_angular_momentum_relative_error = 0.0;
        double mean_energy = 0.0;
        double median_energy = 0.0;
        double min_energy = 0.0;
        double max_energy = 0.0;
        double mean_angular_momentum = 0.0;
        double median_angular_momentum = 0.0;
        double min_angular_momentum = 0.0;
        double max_angular_momentum = 0.0;
        double mean_energy_relative_error = 0.0;
        double median_energy_relative_error = 0.0;
        double min_energy_relative_error = 0.0;
        double max_energy_relative_error = 0.0;
        double mean_angular_momentum_relative_error = 0.0;
        double median_angular_momentum_relative_error = 0.0;
        double min_angular_momentum_relative_error = 0.0;
        double max_angular_momentum_relative_error = 0.0;
        double compute_time_seconds = 0.0;
        int samples = 0;
    };

    class RunRegistry {
    public:
        explicit RunRegistry(const std::string& csv_path);

        void record(int step, double time, const Vec2& position, const Vec2& velocity,
                    double distance, double energy, double angular_momentum,
                    double energy_relative_error, double angular_momentum_relative_error);

        RunSummary finalize(double compute_time_seconds);

    private:
        std::ofstream csv;
        std::vector<double> energies;
        std::vector<double> angular_momenta;
        std::vector<double> energy_errors;
        std::vector<double> angular_momentum_errors;

        double sum_energy = 0.0;
        double sum_angular_momentum = 0.0;
        double sum_energy_error = 0.0;
        double sum_angular_momentum_error = 0.0;
        double min_energy = std::numeric_limits<double>::infinity();
        double max_energy = -std::numeric_limits<double>::infinity();
        double min_angular_momentum = std::numeric_limits<double>::infinity();
        double max_angular_momentum = -std::numeric_limits<double>::infinity();
        double min_energy_error = std::numeric_limits<double>::infinity();
        double max_energy_error = 0.0;
        double min_angular_momentum_error = std::numeric_limits<double>::infinity();
        double max_angular_momentum_error = 0.0;

        static double median(std::vector<double> values);
    };
}

#endif //UROS26_RUN_REGISTRY_H
