# UROS-26

**Investigating Comparative Performance of Numerical Integrators on a Hamiltonian Two-Body System Under Newtonian Mechanics**

UROS-26 is a C++20 orbital simulation engine that models the classical two-body gravitational problem under six numerical integration schemes — Explicit Euler, Symplectic Euler, Velocity Verlet, Leapfrog, 4th-order Leapfrog via Yoshida composition, and 4th-order Runge-Kutta (RK4). The core engine is exposed to Python via [pybind11](https://github.com/pybind/pybind11), and a parallel simulation campaign + analysis pipeline is provided in Python for generating trajectories, conservation diagnostics, and comparison plots.

---

## Table of Contents

- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Build Instructions](#build-instructions)
  - [macOS](#macos)
  - [Linux](#linux)
  - [Windows](#windows)
- [Running the Project](#running-the-project)
  - [C++ CLI executable](#c-cli-executable)
  - [Python simulation campaign](#python-simulation-campaign)
  - [Using the Python bindings directly](#using-the-python-bindings-directly)
- [Output](#output)
- [Troubleshooting](#troubleshooting)

---

## Project Structure

```
UROS26/
├── CMakeLists.txt
├── requirements.txt
├── include/                  # Public headers (Vec2, physics, integrators, ...)
├── src/                      # Implementation + pybind11 bindings
│   ├── bindings.cpp          # Python module definition (module name: uros26)
│   ├── main.cpp              # C++ CLI entry point
│   ├── physics.cpp
│   ├── procedural_generation.cpp
│   └── integrators/
├── scripts/
│   └── main.py                # Parallel simulation campaign + plotting
├── uros26.pyi                # Type stubs for the compiled extension
└── docs/                      # LaTeX report (paper, abstract, methods, ...)
```

The build produces two targets:

| Target | Type | Description |
|---|---|---|
| `UROS26` | Executable | Native C++ CLI (`src/main.cpp`) |
| `uros26` | Python extension module | pybind11 module importable from Python |

---

## Dependencies

### Required for all platforms

- **CMake ≥ 4.2**
- **A C++20-capable compiler**
- **Python 3.12** (interpreter + development headers)
- **pybind11** (Python package, provides CMake config via `python -m pybind11 --cmakedir`)
- **matplotlib** (only required to run `scripts/main.py` for plotting)

These last two are listed in `requirements.txt`:

```
pybind11
matplotlib
```

> **Note:** `find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)` requires the Python *development* package (headers + `Development.Module` artifacts), not just the interpreter. On Linux this is commonly a separate OS package (e.g. `python3-dev`).

---

## Build Instructions

In all cases, start by cloning the repository and creating a Python virtual environment (recommended so CMake picks up a consistent, known Python and so `pybind11 --cmakedir` resolves correctly):

```bash
git clone <repository-url> UROS26
cd UROS26
python3 -m venv .venv
```

Activate it:

- macOS/Linux: `source .venv/bin/activate`
- Windows (PowerShell): `.venv\Scripts\Activate.ps1`
- Windows (cmd.exe): `.venv\Scripts\activate.bat`

Then install the Python dependencies:

```bash
pip install -r requirements.txt
```

### macOS

1. **Install Xcode Command Line Tools** (provides Clang with C++20 support):
   ```bash
   xcode-select --install
   ```
2. **Install CMake and Python 3.12** (via [Homebrew](https://brew.sh)):
   ```bash
   brew install cmake python@3.12
   ```
3. **Create the venv with the Homebrew Python** and install dependencies as above, e.g.:
   ```bash
   python3.12 -m venv .venv
   source .venv/bin/activate
   pip install -r requirements.txt
   ```
4. **Configure and build:**
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j$(sysctl -n hw.ncpu)
   ```

### Linux

1. **Install build tools and a C++20 compiler** (GCC ≥ 10 or Clang ≥ 12). Example for Debian/Ubuntu:
   ```bash
   sudo apt update
   sudo apt install build-essential cmake python3 python3-dev python3-venv
   ```
   Example for Fedora:
   ```bash
   sudo dnf install gcc-c++ cmake python3 python3-devel
   ```
2. **Create the venv and install dependencies:**
   ```bash
   python3 -m venv .venv
   source .venv/bin/activate
   pip install -r requirements.txt
   ```
3. **Configure and build:**
   ```bash
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j$(nproc)
   ```

### Windows

1. **Install Visual Studio 2022 (or later)** with the **"Desktop development with C++"** workload — this provides the MSVC toolchain with C++20 support. Alternatively, install the standalone [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/).
2. **Install CMake** ≥ 4.2 from [cmake.org](https://cmake.org/download/) (or via `winget install Kitware.CMake`), ensuring it's added to `PATH`.
3. **Install Python 3.12** from [python.org](https://www.python.org/downloads/) (check "Add python.exe to PATH" during install), or via `winget install Python.Python.3.12`.
4. **Create the venv and install dependencies** (PowerShell):
   ```powershell
   python -m venv .venv
   .venv\Scripts\Activate.ps1
   pip install -r requirements.txt
   ```
5. **Configure and build** using the Visual Studio generator:
   ```powershell
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   cmake --build build --config Release
   ```
   Alternatively, if you have the Ninja generator and the MSVC environment loaded (e.g. via the "Developer PowerShell for VS 2022" prompt):
   ```powershell
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

> On Windows, build artifacts (the `uros26*.pyd` extension and `UROS26.exe`) are typically placed under `build\Release\` (multi-config generators like Visual Studio) rather than directly in `build\`.

---

## Running the Project

### C++ CLI executable

The native executable is currently a minimal entry point (`src/main.cpp`) and does not yet implement a command-line interface; it returns immediately. It is provided primarily as a build sanity check.

```bash
./build/UROS26          # macOS/Linux
.\build\Release\UROS26.exe   # Windows (Visual Studio generator)
```

### Python simulation campaign

The main workflow is the parallel simulation driver at `scripts/main.py`, which procedurally generates two-body problems, runs all six integrators at three timestep resolutions, writes per-run trajectory CSVs, and produces comparison plots with `matplotlib`.

From the repository root, with your virtual environment active and the project built:

```bash
python3 scripts/main.py --output-root /path/to/output/directory --problem-count 2000 --workers 4
```

Key arguments:

| Argument | Default | Description |
|---|---|---|
| `--output-root` | *(required, or use `--plot-summary-only`)* | Directory where the timestamped run folder is created |
| `--problem-count` | `20` | Number of procedurally generated two-body problems |
| `--workers` | `4` | Size of the process pool |
| `--seed-base` | `26000` | Base seed for reproducible procedural generation |
| `--max-plot-points` | `5000` | Maximum points per trajectory plot (for downsampling) |
| `--G` | `4π²` | Gravitational constant (AU³ yr⁻² M☉⁻¹, normalised units) |
| `--run-name` | `uros26_<timestamp>` | Name of the run folder under `--output-root` |
| `--plot-summary-only` | — | Path to an existing `summary.csv`; regenerates summary plots only, skipping simulation |

The script automatically locates the compiled extension by inserting `<repo_root>/build` onto `sys.path`, so it must be run **after** building, and ideally from the repository root (or with the repo root resolvable relative to the script's location).

Example — regenerate only the summary plots from a previous run:

```bash
python3 scripts/main.py --plot-summary-only /path/to/output/uros26_20260620_153000/summary.csv
```

### Using the Python bindings directly

Once built, the `uros26` extension module can be imported directly (ensure `build/` — or `build/Release/` on Windows — is on your `PYTHONPATH`, or run from a script that inserts it as `scripts/main.py` does):

```python
import sys
sys.path.insert(0, "build")  # or "build/Release" on Windows

from uros26 import Vec2, physics, generate, integrators

problem = generate.problem_setup(seed=42)

summary = integrators.leapfrog.run_leapfrog(
    problem.star_pos, problem.planet_pos,
    problem.star_mass, problem.planet_mass,
    t=1e-3, iterations=1_000_000, G=4 * 3.14159265 ** 2,
    output_csv="trajectory.csv",
)

print(summary.final_energy_relative_error)
```

The fourth-order Leapfrog/Yoshida runner is exposed as `integrators.leapfrog_4th.run_yoshida4(...)` and accepts the same arguments as the other integrators.

See `uros26.pyi` for the full typed interface.

---

## Output

Each simulation campaign run produces, under `--output-root/<run-name>/`:

- `<integrator>/sim_XXXX/<resolution>/trajectory.csv` — full per-step trajectory (position, velocity, distance, energy, angular momentum, relative errors)
- `<integrator>/sim_XXXX/<resolution>/orbit.png`, `energy_conservation.png`, `angular_momentum.png`, `energy_relative_error.png`, `angular_momentum_relative_error.png` — per-run plots
- `summary.csv` — master table of scalar summary statistics across all runs
- `summary_plots/` — cross-integrator comparison plots (runtime, error vs. timestep, accuracy vs. runtime, etc.)

---

## Troubleshooting

- **`Could not find pybind11`**: Ensure pybind11 is installed in the *same* Python environment CMake is using (`pip show pybind11`), and that `python -m pybind11 --cmakedir` runs without error in that environment.
- **CMake picks the wrong Python**: Pass `-DPython_EXECUTABLE=$(which python3)` (or the full path to your venv's interpreter) explicitly to the `cmake -S . -B build` command.
- **`ImportError` when running `scripts/main.py`**: Confirm the project has been built and that the compiled module (`uros26*.so` / `uros26*.pyd`) exists under `build/` (or `build/Release/` on Windows with multi-config generators).
- **Windows: missing C++20 features**: Make sure you're using a recent MSVC toolset (Visual Studio 2022 17.x or later) and that the generator/architecture (`-A x64`) matches your installed Python's bitness.
