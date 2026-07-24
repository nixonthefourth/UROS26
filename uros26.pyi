from __future__ import annotations


class Vec2:
    x: float
    y: float

    def __init__(self, x: float = 0.0, y: float = 0.0) -> None: ...
    def perpendicular(self) -> Vec2: ...


class RunSummary:
    final_energy_relative_error: float
    final_angular_momentum_relative_error: float
    mean_energy: float
    median_energy: float
    min_energy: float
    max_energy: float
    mean_angular_momentum: float
    median_angular_momentum: float
    min_angular_momentum: float
    max_angular_momentum: float
    mean_energy_relative_error: float
    median_energy_relative_error: float
    min_energy_relative_error: float
    max_energy_relative_error: float
    mean_angular_momentum_relative_error: float
    median_angular_momentum_relative_error: float
    min_angular_momentum_relative_error: float
    max_angular_momentum_relative_error: float
    compute_time_seconds: float
    samples: int


class _Physics:
    def find_distance(self, a: Vec2, b: Vec2) -> float: ...
    def find_g_acceleration(
        self, m_a: float, distance: float, a: Vec2, b: Vec2, G: float
    ) -> Vec2: ...
    def find_velocity(self, m: float, distance: float, G: float) -> float: ...
    def find_vel_direction(self, a: Vec2, b: Vec2, distance: float, vel: float) -> Vec2: ...
    def find_energy_conservation(
        self, M: float, m: float, G: float, distance: float, b_velocity: Vec2
    ) -> float: ...
    def energy_error(self, e_n: float, e_0: float) -> float: ...
    def find_angular_momentum(self, m: float, b_velocity: Vec2, b: Vec2) -> float: ...
    def angular_error(self, l_n: float, l_0: float) -> float: ...


class _Generate:
    class ProblemSetup:
        star_mass: float
        planet_mass: float
        star_pos: Vec2
        planet_pos: Vec2

    def solar_mass(self) -> float: ...
    def planetary_mass(self, stellar_mass: float) -> float: ...
    def separation_distance(self) -> Vec2: ...
    def problem_setup(self, seed: int) -> ProblemSetup: ...


class _ExplicitEuler:
    def run_explicit_euler(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _SymplecticEuler:
    def run_symplectic_euler(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _Verlet:
    def run_verlet(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _Rk4:
    def run_rk4(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _Leapfrog:
    def run_leapfrog(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _Leapfrog4:
    def run_yoshida4(
        self,
        star_pos: Vec2,
        planet_pos: Vec2,
        star_mass: float,
        planet_mass: float,
        t: float,
        iterations: int,
        G: float,
        output_csv: str = "",
    ) -> RunSummary: ...


class _Integrators:
    explicit_euler: _ExplicitEuler
    symplectic_euler: _SymplecticEuler
    verlet: _Verlet
    rk4: _Rk4
    leapfrog: _Leapfrog
    leapfrog_4th: _Leapfrog4
    leapfrog4: _Leapfrog4


physics: _Physics
generate: _Generate
integrators: _Integrators
