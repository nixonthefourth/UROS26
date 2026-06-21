#include "integrators/run_registry.h++"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace integrators {
    RunRegistry::RunRegistry(const std::string& csv_path) {
        if (!csv_path.empty()) {
            const std::filesystem::path path(csv_path);
            if (path.has_parent_path()) {
                std::filesystem::create_directories(path.parent_path());
            }

            csv.open(path);
            if (!csv) {
                throw std::runtime_error("Could not open trajectory CSV: " + csv_path);
            }

            csv << "step,time,position_x,position_y,velocity_x,velocity_y,distance,"
                   "energy,angular_momentum,energy_relative_error,"
                   "angular_momentum_relative_error\n";
        }
    }

    void RunRegistry::record(const int step, const double time, const Vec2& position, const Vec2& velocity,
                             const double distance, const double energy, const double angular_momentum,
                             const double energy_relative_error,
                             const double angular_momentum_relative_error) {
        if (csv) {
            csv << step << ','
                << time << ','
                << position.x << ','
                << position.y << ','
                << velocity.x << ','
                << velocity.y << ','
                << distance << ','
                << energy << ','
                << angular_momentum << ','
                << energy_relative_error << ','
                << angular_momentum_relative_error << '\n';
        }

        energies.push_back(energy);
        angular_momenta.push_back(angular_momentum);
        energy_errors.push_back(energy_relative_error);
        angular_momentum_errors.push_back(angular_momentum_relative_error);

        sum_energy += energy;
        sum_angular_momentum += angular_momentum;
        sum_energy_error += energy_relative_error;
        sum_angular_momentum_error += angular_momentum_relative_error;
        min_energy = std::min(min_energy, energy);
        max_energy = std::max(max_energy, energy);
        min_angular_momentum = std::min(min_angular_momentum, angular_momentum);
        max_angular_momentum = std::max(max_angular_momentum, angular_momentum);
        min_energy_error = std::min(min_energy_error, energy_relative_error);
        max_energy_error = std::max(max_energy_error, energy_relative_error);
        min_angular_momentum_error = std::min(min_angular_momentum_error, angular_momentum_relative_error);
        max_angular_momentum_error = std::max(max_angular_momentum_error, angular_momentum_relative_error);
    }

    RunSummary RunRegistry::finalize(const double compute_time_seconds) {
        if (csv) {
            csv.close();
        }

        RunSummary summary;
        summary.compute_time_seconds = compute_time_seconds;
        summary.samples = static_cast<int>(energy_errors.size());

        if (energy_errors.empty()) {
            return summary;
        }

        summary.final_energy_relative_error = energy_errors.back();
        summary.final_angular_momentum_relative_error = angular_momentum_errors.back();
        summary.mean_energy = sum_energy / static_cast<double>(energies.size());
        summary.median_energy = median(energies);
        summary.min_energy = min_energy;
        summary.max_energy = max_energy;
        summary.mean_angular_momentum = sum_angular_momentum / static_cast<double>(angular_momenta.size());
        summary.median_angular_momentum = median(angular_momenta);
        summary.min_angular_momentum = min_angular_momentum;
        summary.max_angular_momentum = max_angular_momentum;
        summary.mean_energy_relative_error = sum_energy_error / static_cast<double>(energy_errors.size());
        summary.median_energy_relative_error = median(energy_errors);
        summary.min_energy_relative_error = min_energy_error;
        summary.max_energy_relative_error = max_energy_error;
        summary.mean_angular_momentum_relative_error =
            sum_angular_momentum_error / static_cast<double>(angular_momentum_errors.size());
        summary.median_angular_momentum_relative_error = median(angular_momentum_errors);
        summary.min_angular_momentum_relative_error = min_angular_momentum_error;
        summary.max_angular_momentum_relative_error = max_angular_momentum_error;

        return summary;
    }

    double RunRegistry::median(std::vector<double> values) {
        if (values.empty()) {
            return 0.0;
        }

        std::sort(values.begin(), values.end());
        const std::size_t middle = values.size() / 2;
        if (values.size() % 2 == 0) {
            return (values[middle - 1] + values[middle]) / 2.0;
        }

        return values[middle];
    }
}
