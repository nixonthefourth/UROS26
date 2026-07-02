# Runtime command: python3 scripts/main.py --output-root /Volumes/Maxtor --problem-count 150 --workers 3
# Use your volume name

from __future__ import annotations

import argparse
import csv
import math
import os
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from statistics import median
from typing import Callable, Iterable


REPO_ROOT = Path(__file__).resolve().parents[1]
BUILD_DIR = REPO_ROOT / "build"

if BUILD_DIR.exists():
    sys.path.insert(0, str(BUILD_DIR))

from uros26 import Vec2, generate, integrators  # noqa: E402


G_AU_SOLAR_YEAR = 4.0 * math.pi * math.pi
DEFAULT_PROBLEM_COUNT = 150
DEFAULT_WORKERS = 3
RESOLUTIONS = (
    ("dt_1e-1", 1e-1, 100_000),
    ("dt_1e-2", 1e-2, 1_000_000),
    ("dt_1e-3", 1e-3, 10_000_000),
)

# Resolutions whose iteration counts exceed this threshold will be downsampled
# to SUMMARY_DOWNSAMPLE_TARGET steps when producing summary table plots so
# that all resolutions contribute an equal number of time-points to comparisons.
SUMMARY_DOWNSAMPLE_TARGET = 100_000


@dataclass(frozen=True)
class IntegratorSpec:
    folder: str
    label: str
    runner_name: str


INTEGRATORS = (
    IntegratorSpec("explicit_euler", "Explicit Euler", "run_explicit_euler"),
    IntegratorSpec("symplectic_euler", "Symplectic Euler", "run_symplectic_euler"),
    IntegratorSpec("leapfrog", "Leapfrog", "run_leapfrog"),
    IntegratorSpec("verlet", "Velocity Verlet", "run_verlet"),
    IntegratorSpec("rk4", "RK4", "run_rk4"),
)


SUMMARY_FIELDS = (
    "simulation_id",
    "problem_id",
    "integrator",
    "resolution",
    "timestep",
    "iterations",
    "star_mass",
    "planet_mass",
    "star_x",
    "star_y",
    "planet_x",
    "planet_y",
    "final_energy_relative_error",
    "final_angular_momentum_relative_error",
    "mean_energy",
    "median_energy",
    "min_energy",
    "max_energy",
    "mean_angular_momentum",
    "median_angular_momentum",
    "min_angular_momentum",
    "max_angular_momentum",
    "mean_energy_relative_error",
    "median_energy_relative_error",
    "min_energy_relative_error",
    "max_energy_relative_error",
    "mean_angular_momentum_relative_error",
    "median_angular_momentum_relative_error",
    "min_angular_momentum_relative_error",
    "max_angular_momentum_relative_error",
    "compute_time_seconds",
    "trajectory_csv",
    "orbit_plot",
    "energy_plot",
    "angular_momentum_plot",
    "energy_relative_error_plot",
    "angular_momentum_relative_error_plot",
)


def integrator_runner(spec: IntegratorSpec) -> Callable:
    module = getattr(integrators, spec.folder)
    return getattr(module, spec.runner_name)


def plot_orbit(csv_path: Path, plot_path: Path, title: str, samples: int, max_points: int) -> None:
    configure_matplotlib()

    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    stride = max(1, samples // max_points)
    x_values: list[float] = []
    y_values: list[float] = []

    with csv_path.open(newline="") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            step = int(row["step"])
            if step % stride == 0 or step == samples - 1:
                x_values.append(float(row["position_x"]))
                y_values.append(float(row["position_y"]))

    fig, ax = plt.subplots(figsize=(7, 7), dpi=160)
    ax.plot(x_values, y_values, linewidth=0.8)
    ax.scatter([0.0], [0.0], s=32, marker="*", label="Central body")
    ax.set_aspect("equal", adjustable="box")
    ax.set_xlabel("x [AU]")
    ax.set_ylabel("y [AU]")
    ax.set_title(title)
    ax.grid(True, linewidth=0.4, alpha=0.45)
    ax.legend(loc="best")
    fig.tight_layout()
    fig.savefig(plot_path)
    plt.close(fig)


def plot_time_series(
    csv_path: Path,
    plot_path: Path,
    title: str,
    field: str,
    ylabel: str,
    samples: int,
    max_points: int,
    log_scale: bool = False,
) -> None:
    configure_matplotlib()

    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    stride = max(1, samples // max_points)
    time_values: list[float] = []
    field_values: list[float] = []

    with csv_path.open(newline="") as csv_file:
        reader = csv.DictReader(csv_file)
        for row in reader:
            step = int(row["step"])
            if step % stride == 0 or step == samples - 1:
                time_values.append(float(row["time"]))
                value = float(row[field])
                field_values.append(log_safe(value) if log_scale else value)

    fig, ax = plt.subplots(figsize=(9, 5), dpi=160)
    ax.plot(time_values, field_values, linewidth=0.8)
    ax.set_xlabel("time [yr]")
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    if log_scale:
        ax.set_yscale("log")
    ax.grid(True, linewidth=0.4, alpha=0.45)
    # Zoom the horizontal axis tightly to the data so inter-integrator
    # variation is cleaner to see without excess whitespace.
    if time_values:
        t_min, t_max = min(time_values), max(time_values)
        t_range = t_max - t_min
        h_pad = t_range * 0.01 if t_range > 0 else 1.0
        ax.set_xlim(t_min - h_pad, t_max + h_pad)
    fig.tight_layout()
    fig.savefig(plot_path)
    plt.close(fig)


def summary_to_row(summary, metadata: dict[str, object]) -> dict[str, object]:
    return {
        **metadata,
        "final_energy_relative_error": summary.final_energy_relative_error,
        "final_angular_momentum_relative_error": summary.final_angular_momentum_relative_error,
        "mean_energy": summary.mean_energy,
        "median_energy": summary.median_energy,
        "min_energy": summary.min_energy,
        "max_energy": summary.max_energy,
        "mean_angular_momentum": summary.mean_angular_momentum,
        "median_angular_momentum": summary.median_angular_momentum,
        "min_angular_momentum": summary.min_angular_momentum,
        "max_angular_momentum": summary.max_angular_momentum,
        "mean_energy_relative_error": summary.mean_energy_relative_error,
        "median_energy_relative_error": summary.median_energy_relative_error,
        "min_energy_relative_error": summary.min_energy_relative_error,
        "max_energy_relative_error": summary.max_energy_relative_error,
        "mean_angular_momentum_relative_error": summary.mean_angular_momentum_relative_error,
        "median_angular_momentum_relative_error": summary.median_angular_momentum_relative_error,
        "min_angular_momentum_relative_error": summary.min_angular_momentum_relative_error,
        "max_angular_momentum_relative_error": summary.max_angular_momentum_relative_error,
        "compute_time_seconds": summary.compute_time_seconds,
    }


def run_problem(
    problem_id: int,
    output_root: str,
    seed_base: int,
    max_plot_points: int,
    gravitational_constant: float,
) -> list[dict[str, object]]:
    problem = generate.problem_setup(seed_base + problem_id)
    rows: list[dict[str, object]] = []

    for spec in INTEGRATORS:
        runner = integrator_runner(spec)

        for resolution_name, timestep, iterations in RESOLUTIONS:
            simulation_id = f"sim_{problem_id:04d}_{spec.folder}_{resolution_name}"
            print("Simulating:", simulation_id)
            
            run_dir = Path(output_root) / spec.folder / f"sim_{problem_id:04d}" / resolution_name
            run_dir.mkdir(parents=True, exist_ok=True)

            trajectory_csv = run_dir / "trajectory.csv"
            orbit_plot = run_dir / "orbit.png"
            energy_plot = run_dir / "energy_conservation.png"
            angular_momentum_plot = run_dir / "angular_momentum.png"
            energy_relative_error_plot = run_dir / "energy_relative_error.png"
            angular_momentum_relative_error_plot = run_dir / "angular_momentum_relative_error.png"

            summary = runner(
                problem.star_pos,
                problem.planet_pos,
                problem.star_mass,
                problem.planet_mass,
                timestep,
                iterations,
                gravitational_constant,
                str(trajectory_csv),
            )

            plot_orbit(
                trajectory_csv,
                orbit_plot,
                f"{spec.label} | sim {problem_id:04d} | dt={timestep:g}",
                summary.samples,
                max_plot_points,
            )
            plot_time_series(
                trajectory_csv,
                energy_plot,
                f"{spec.label} Energy Conservation | sim {problem_id:04d} | dt={timestep:g}",
                "energy",
                "energy",
                summary.samples,
                max_plot_points,
            )
            plot_time_series(
                trajectory_csv,
                angular_momentum_plot,
                f"{spec.label} Angular Momentum | sim {problem_id:04d} | dt={timestep:g}",
                "angular_momentum",
                "angular momentum",
                summary.samples,
                max_plot_points,
            )
            plot_time_series(
                trajectory_csv,
                energy_relative_error_plot,
                f"{spec.label} Relative Energy Error | sim {problem_id:04d} | dt={timestep:g}",
                "energy_relative_error",
                "relative energy error",
                summary.samples,
                max_plot_points,
                log_scale=True,
            )
            plot_time_series(
                trajectory_csv,
                angular_momentum_relative_error_plot,
                f"{spec.label} Relative Angular Momentum Error | sim {problem_id:04d} | dt={timestep:g}",
                "angular_momentum_relative_error",
                "relative angular momentum error",
                summary.samples,
                max_plot_points,
                log_scale=True,
            )

            rows.append(
                summary_to_row(
                    summary,
                    {
                        "simulation_id": simulation_id,
                        "problem_id": problem_id,
                        "integrator": spec.label,
                        "resolution": resolution_name,
                        "timestep": timestep,
                        "iterations": iterations,
                        "star_mass": problem.star_mass,
                        "planet_mass": problem.planet_mass,
                        "star_x": problem.star_pos.x,
                        "star_y": problem.star_pos.y,
                        "planet_x": problem.planet_pos.x,
                        "planet_y": problem.planet_pos.y,
                        "trajectory_csv": str(trajectory_csv),
                        "orbit_plot": str(orbit_plot),
                        "energy_plot": str(energy_plot),
                        "angular_momentum_plot": str(angular_momentum_plot),
                        "energy_relative_error_plot": str(energy_relative_error_plot),
                        "angular_momentum_relative_error_plot": str(angular_momentum_relative_error_plot),
                    },
                )
            )

    return rows


def write_summary(summary_path: Path, rows: list[dict[str, object]]) -> None:
    with summary_path.open("w", newline="") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=SUMMARY_FIELDS)
        writer.writeheader()
        writer.writerows(rows)


def read_summary(summary_path: Path) -> list[dict[str, str]]:
    with summary_path.open(newline="") as csv_file:
        return list(csv.DictReader(csv_file))


def mean(values: Iterable[float]) -> float:
    values = list(values)
    if not values:
        return 0.0

    return sum(values) / len(values)


def log_safe(value: float) -> float:
    return value if value > 0.0 else 1e-18


def configure_matplotlib() -> None:
    temp_root = Path(os.environ.get("TMPDIR", "/tmp"))
    matplotlib_cache = temp_root / "uros26_matplotlib"
    font_cache = temp_root / "uros26_fontconfig"
    matplotlib_cache.mkdir(parents=True, exist_ok=True)
    font_cache.mkdir(parents=True, exist_ok=True)
    os.environ.setdefault("MPLCONFIGDIR", str(matplotlib_cache))
    os.environ.setdefault("XDG_CACHE_HOME", str(font_cache))


def plot_summary_table(summary_path: Path, output_dir: Path) -> None:
    configure_matplotlib()

    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    rows = read_summary(summary_path)
    output_dir.mkdir(parents=True, exist_ok=True)

    integrator_order = [spec.label for spec in INTEGRATORS]
    resolution_order = [resolution[0] for resolution in RESOLUTIONS]

    grouped: dict[tuple[str, str], list[dict[str, str]]] = {}
    for row in rows:
        grouped.setdefault((row["integrator"], row["resolution"]), []).append(row)

    def grouped_means(field: str) -> dict[tuple[str, str], float]:
        return {
            key: mean(float(row[field]) for row in group)
            for key, group in grouped.items()
        }

    def grouped_maxima(field: str) -> dict[tuple[str, str], float]:
        return {
            key: max(float(row[field]) for row in group)
            for key, group in grouped.items()
        }

    def grouped_medians(field: str) -> dict[tuple[str, str], float]:
        return {
            key: median(float(row[field]) for row in group)
            for key, group in grouped.items()
        }

    def bar_plot(filename: str, title: str, ylabel: str, values: dict[tuple[str, str], float],
                 log_scale: bool = False) -> None:
        n_res = len(resolution_order)
        x_positions = list(range(len(integrator_order)))
        # Distribute bars evenly; leave a small gap between integrator groups.
        total_bar_width = 0.75
        width = total_bar_width / n_res
        offsets = [
            -total_bar_width / 2 + width * (i + 0.5)
            for i in range(n_res)
        ]

        fig, ax = plt.subplots(figsize=(12, 5), dpi=160)
        for resolution, offset in zip(resolution_order, offsets):
            heights = [values.get((integrator, resolution), 0.0) for integrator in integrator_order]
            ax.bar(
                [x + offset for x in x_positions],
                heights,
                width,
                label=resolution.replace("_", "="),
            )

        ax.set_title(title)
        ax.set_ylabel(ylabel)
        ax.set_xticks(x_positions)
        ax.set_xticklabels(integrator_order, rotation=20, ha="right")
        ax.grid(axis="y", linewidth=0.4, alpha=0.45)
        ax.legend(title="Resolution")
        if log_scale:
            ax.set_yscale("log")
        fig.tight_layout()
        fig.savefig(output_dir / filename)
        plt.close(fig)

    def timestep_plot(filename: str, title: str, ylabel: str, field: str) -> None:
        fig, ax = plt.subplots(figsize=(8, 5), dpi=160)
        for integrator in integrator_order:
            points = []
            for resolution in resolution_order:
                key = (integrator, resolution)
                if key in grouped:
                    timestep = float(grouped[key][0]["timestep"])
                    error = mean(float(row[field]) for row in grouped[key])
                    points.append((timestep, log_safe(error)))

            if points:
                points.sort()
                ax.plot(
                    [point[0] for point in points],
                    [point[1] for point in points],
                    marker="o",
                    linewidth=1.2,
                    label=integrator,
                )

        ax.set_title(title)
        ax.set_xlabel("timestep [yr]")
        ax.set_ylabel(ylabel)
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.grid(True, linewidth=0.4, alpha=0.45)
        ax.legend(loc="best")
        fig.tight_layout()
        fig.savefig(output_dir / filename)
        plt.close(fig)

    def boxplot(filename: str, title: str, ylabel: str, field: str) -> None:
        fig, ax = plt.subplots(figsize=(11, 5), dpi=160)
        labels: list[str] = []
        values: list[list[float]] = []

        for integrator in integrator_order:
            for resolution in resolution_order:
                group = grouped.get((integrator, resolution), [])
                if not group:
                    continue
                labels.append(f"{integrator}\n{resolution.replace('_', '=')}")
                values.append([log_safe(float(row[field])) for row in group])

        ax.boxplot(values, labels=labels, showfliers=False)
        ax.set_title(title)
        ax.set_ylabel(ylabel)
        ax.set_yscale("log")
        ax.grid(axis="y", linewidth=0.4, alpha=0.45)
        ax.tick_params(axis="x", labelrotation=35)
        fig.tight_layout()
        fig.savefig(output_dir / filename)
        plt.close(fig)

    def plot_trajectory_stat(
        filename: str,
        title: str,
        ylabel: str,
        field: str,
        reducer: Callable[[Iterable[float]], float],
        max_points: int = 1_200,
        log_scale: bool = False,
    ) -> None:
        fig, ax = plt.subplots(figsize=(10, 5), dpi=160)
        all_times: list[float] = []

        for resolution in resolution_order:
            for integrator in integrator_order:
                group = grouped.get((integrator, resolution), [])
                if not group:
                    continue

                raw_iterations = int(group[0]["iterations"])
                # Downsample longer runs so every resolution contributes at most
                # SUMMARY_DOWNSAMPLE_TARGET time-points, making the comparison fair.
                effective_samples = min(raw_iterations + 1, SUMMARY_DOWNSAMPLE_TARGET + 1)
                stride = max(1, (raw_iterations + 1) // effective_samples)
                # Then further stride to stay within the plotting max_points budget.
                plot_stride = max(1, effective_samples // max_points)
                combined_stride = stride * plot_stride

                times_by_index: dict[int, list[float]] = {}
                values_by_index: dict[int, list[float]] = {}

                for row in group:
                    csv_path = Path(row["trajectory_csv"])
                    if not csv_path.exists():
                        continue

                    with csv_path.open(newline="") as trajectory_file:
                        reader = csv.DictReader(trajectory_file)
                        for trajectory_row in reader:
                            step = int(trajectory_row["step"])
                            if step % combined_stride != 0 and step != raw_iterations:
                                continue

                            index = step // combined_stride
                            times_by_index.setdefault(index, []).append(float(trajectory_row["time"]))
                            value = float(trajectory_row[field])
                            values_by_index.setdefault(index, []).append(log_safe(value) if log_scale else value)

                if not values_by_index:
                    continue

                indices = sorted(values_by_index)
                t_series = [mean(times_by_index[index]) for index in indices]
                all_times.extend(t_series)
                ax.plot(
                    t_series,
                    [reducer(values_by_index[index]) for index in indices],
                    linewidth=1.0,
                    label=f"{integrator}, {resolution.replace('_', '=')}",
                )

        ax.set_title(title)
        ax.set_xlabel("time [yr]")
        ax.set_ylabel(ylabel)
        if log_scale:
            ax.set_yscale("log")
        ax.grid(True, linewidth=0.4, alpha=0.45)
        ax.legend(loc="best", fontsize=8)
        # Zoom horizontal axis tightly so inter-integrator differences are clearer.
        if all_times:
            t_min, t_max = min(all_times), max(all_times)
            t_range = t_max - t_min
            h_pad = t_range * 0.01 if t_range > 0 else 1.0
            ax.set_xlim(t_min - h_pad, t_max + h_pad)
        fig.tight_layout()
        fig.savefig(output_dir / filename)
        plt.close(fig)

    runtime_mean = grouped_means("compute_time_seconds")
    energy_error_mean = grouped_means("mean_energy_relative_error")
    energy_error_median = grouped_medians("median_energy_relative_error")
    energy_error_max = grouped_maxima("max_energy_relative_error")
    angular_error_mean = grouped_means("mean_angular_momentum_relative_error")
    angular_error_median = grouped_medians("median_angular_momentum_relative_error")
    angular_error_max = grouped_maxima("max_angular_momentum_relative_error")
    energy_mean = grouped_means("mean_energy")
    angular_momentum_mean = grouped_means("mean_angular_momentum")

    bar_plot("average_runtime_seconds.png", "Average Runtime", "seconds", runtime_mean)
    bar_plot("mean_relative_energy_error.png", "Mean Relative Energy Error", "relative error",
             energy_error_mean, log_scale=True)
    bar_plot("max_relative_energy_error.png", "Maximum Relative Energy Error", "relative error",
             energy_error_max, log_scale=True)
    bar_plot("mean_relative_angular_momentum_error.png", "Mean Relative Angular Momentum Error", "relative error",
             angular_error_mean, log_scale=True)
    bar_plot("max_relative_angular_momentum_error.png", "Maximum Relative Angular Momentum Error", "relative error",
             angular_error_max, log_scale=True)
    bar_plot("mean_energy.png", "Mean Energy Conservation", "energy", energy_mean)
    bar_plot("mean_angular_momentum.png", "Mean Angular Momentum Conservation", "angular momentum",
             angular_momentum_mean)
    bar_plot("median_relative_energy_error.png", "Median Relative Energy Error", "relative error",
             energy_error_median, log_scale=True)
    bar_plot("median_relative_angular_momentum_error.png", "Median Relative Angular Momentum Error",
             "relative error", angular_error_median, log_scale=True)
    timestep_plot("energy_error_vs_timestep.png", "Relative Energy Error vs Timestep",
                  "mean relative energy error", "mean_energy_relative_error")
    timestep_plot("angular_momentum_error_vs_timestep.png", "Angular Momentum Error vs Timestep",
                  "mean relative angular momentum error", "mean_angular_momentum_relative_error")
    boxplot("energy_error_boxplot.png", "Distribution of Relative Energy Errors",
            "mean relative energy error", "mean_energy_relative_error")
    plot_trajectory_stat("energy_conservation_over_time.png", "Mean Energy Conservation Over Time",
                         "energy", "energy", mean)
    plot_trajectory_stat("angular_momentum_over_time.png", "Mean Angular Momentum Over Time",
                         "angular momentum", "angular_momentum", mean)
    plot_trajectory_stat("relative_energy_error_over_time.png", "Mean Relative Energy Error Over Time",
                         "relative energy error", "energy_relative_error", mean, log_scale=True)
    plot_trajectory_stat("angular_momentum_error_over_time.png", "Mean Angular Momentum Error Over Time",
                         "relative angular momentum error", "angular_momentum_relative_error", mean, log_scale=True)
    plot_trajectory_stat("median_energy_conservation_over_time.png",
                         "Median Energy Conservation Over Time", "energy", "energy", median)
    plot_trajectory_stat("median_angular_momentum_over_time.png",
                         "Median Angular Momentum Over Time", "angular momentum", "angular_momentum", median)
    plot_trajectory_stat("median_relative_energy_error_over_time.png",
                         "Median Relative Energy Error Over Time", "relative energy error",
                         "energy_relative_error", median, log_scale=True)
    plot_trajectory_stat("median_angular_momentum_error_over_time.png",
                         "Median Angular Momentum Error Over Time", "relative angular momentum error",
                         "angular_momentum_relative_error", median, log_scale=True)

    fig, ax = plt.subplots(figsize=(8, 5), dpi=160)
    for resolution in resolution_order:
        x_values = []
        y_values = []
        labels = []
        for integrator in integrator_order:
            key = (integrator, resolution)
            if key in runtime_mean and key in energy_error_max:
                x_values.append(runtime_mean[key])
                y_values.append(energy_error_max[key])
                labels.append(integrator)

        ax.scatter(x_values, y_values, label=resolution.replace("_", "="), s=48)
        for x_value, y_value, label in zip(x_values, y_values, labels):
            ax.annotate(label, (x_value, y_value), fontsize=8, xytext=(4, 4), textcoords="offset points")

    ax.set_title("Accuracy vs Runtime")
    ax.set_xlabel("average runtime [seconds]")
    ax.set_ylabel("maximum relative energy error")
    ax.set_yscale("log")
    ax.grid(True, linewidth=0.4, alpha=0.45)
    ax.legend(title="Resolution")
    fig.tight_layout()
    fig.savefig(output_dir / "accuracy_vs_runtime.png")
    plt.close(fig)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run the UROS26 parallel orbital simulation campaign."
    )
    parser.add_argument(
        "--output-root",
        type=Path,
        default=os.environ.get("UROS26_OUTPUT_ROOT"),
        help="External-drive directory where the timestamped run folder will be created.",
    )
    parser.add_argument("--problem-count", type=int, default=DEFAULT_PROBLEM_COUNT)
    parser.add_argument("--workers", type=int, default=DEFAULT_WORKERS)
    parser.add_argument("--seed-base", type=int, default=26_000)
    parser.add_argument("--max-plot-points", type=int, default=5_000)
    parser.add_argument("--G", type=float, default=G_AU_SOLAR_YEAR)
    parser.add_argument(
        "--run-name",
        default=f"uros26_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
        help="Name of the run folder created under --output-root.",
    )
    parser.add_argument(
        "--plot-summary-only",
        type=Path,
        help="Generate summary plots from an existing summary.csv without rerunning simulations.",
    )

    args = parser.parse_args()
    if args.plot_summary_only is None and args.output_root is None:
        parser.error("--output-root is required unless --plot-summary-only is used")

    return args


def main() -> None:
    args = parse_args()

    if args.plot_summary_only is not None:
        summary_path = Path(args.plot_summary_only).expanduser().resolve()
        plot_dir = summary_path.parent / "summary_plots"
        plot_summary_table(summary_path, plot_dir)
        print(f"Summary plots written to {plot_dir}")
        return

    output_root = Path(args.output_root).expanduser().resolve()
    run_root = output_root / args.run_name
    run_root.mkdir(parents=True, exist_ok=False)

    expected_rows = args.problem_count * len(INTEGRATORS) * len(RESOLUTIONS)
    print(f"Writing simulation campaign to {run_root}")
    print(f"Expected summary rows: {expected_rows}")
    print(f"Worker processes: {args.workers}")

    rows: list[dict[str, object]] = []
    with ProcessPoolExecutor(max_workers=args.workers) as executor:
        futures = [
            executor.submit(
                run_problem,
                problem_id,
                str(run_root),
                args.seed_base,
                args.max_plot_points,
                args.G,
            )
            for problem_id in range(args.problem_count)
        ]

        for completed, future in enumerate(as_completed(futures), start=1):
            rows.extend(future.result())
            print(f"Completed problem {completed}/{args.problem_count}")

    rows.sort(key=lambda row: (int(row["problem_id"]), str(row["integrator"]), str(row["resolution"])))
    summary_path = run_root / "summary.csv"
    write_summary(summary_path, rows)
    print(f"Summary written to {summary_path}")
    plot_dir = run_root / "summary_plots"
    plot_summary_table(summary_path, plot_dir)
    print(f"Summary plots written to {plot_dir}")


if __name__ == "__main__":
    main()