#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "Vec2.h++"
#include "integrators/explicit_euler.h++"
#include "integrators/leapfrog.h++"
#include "integrators/run_registry.h++"
#include "integrators/rk4.h++"
#include "integrators/symplectic_euler.h++"
#include "integrators/verlet.h++"
#include "physics.h++"
#include "procedural_generation.h++"

namespace py = pybind11;

PYBIND11_MODULE(uros26, m) {
    m.doc() = "Python bindings for the UROS26 orbital simulation project";

    py::class_<Vec2>(m, "Vec2")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("x"), py::arg("y"))
        .def_readwrite("x", &Vec2::x)
        .def_readwrite("y", &Vec2::y)
        .def("perpendicular", &Vec2::perpendicular)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * double())
        .def(double() * py::self)
        .def(py::self / double())
        .def("__repr__", [](const Vec2& v) {
            return "Vec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });

    py::class_<integrators::RunSummary>(m, "RunSummary")
        .def_readonly("final_energy_relative_error", &integrators::RunSummary::final_energy_relative_error)
        .def_readonly("final_angular_momentum_relative_error",
                      &integrators::RunSummary::final_angular_momentum_relative_error)
        .def_readonly("mean_energy", &integrators::RunSummary::mean_energy)
        .def_readonly("median_energy", &integrators::RunSummary::median_energy)
        .def_readonly("min_energy", &integrators::RunSummary::min_energy)
        .def_readonly("max_energy", &integrators::RunSummary::max_energy)
        .def_readonly("mean_angular_momentum", &integrators::RunSummary::mean_angular_momentum)
        .def_readonly("median_angular_momentum", &integrators::RunSummary::median_angular_momentum)
        .def_readonly("min_angular_momentum", &integrators::RunSummary::min_angular_momentum)
        .def_readonly("max_angular_momentum", &integrators::RunSummary::max_angular_momentum)
        .def_readonly("mean_energy_relative_error", &integrators::RunSummary::mean_energy_relative_error)
        .def_readonly("median_energy_relative_error", &integrators::RunSummary::median_energy_relative_error)
        .def_readonly("min_energy_relative_error", &integrators::RunSummary::min_energy_relative_error)
        .def_readonly("max_energy_relative_error", &integrators::RunSummary::max_energy_relative_error)
        .def_readonly("mean_angular_momentum_relative_error",
                      &integrators::RunSummary::mean_angular_momentum_relative_error)
        .def_readonly("median_angular_momentum_relative_error",
                      &integrators::RunSummary::median_angular_momentum_relative_error)
        .def_readonly("min_angular_momentum_relative_error",
                      &integrators::RunSummary::min_angular_momentum_relative_error)
        .def_readonly("max_angular_momentum_relative_error",
                      &integrators::RunSummary::max_angular_momentum_relative_error)
        .def_readonly("compute_time_seconds", &integrators::RunSummary::compute_time_seconds)
        .def_readonly("samples", &integrators::RunSummary::samples);

    py::module_ physics_module = m.def_submodule("physics", "Physics helper functions");
    physics_module.def("find_distance", &physics::find_distance, py::arg("a"), py::arg("b"));
    physics_module.def("find_g_acceleration", &physics::find_g_acceleration,
                       py::arg("m_a"), py::arg("distance"), py::arg("a"), py::arg("b"), py::arg("G"));
    physics_module.def("find_velocity", &physics::find_velocity,
                       py::arg("m"), py::arg("distance"), py::arg("G"));
    physics_module.def("find_vel_direction", &physics::find_vel_direction,
                       py::arg("a"), py::arg("b"), py::arg("distance"), py::arg("vel"));
    physics_module.def("find_energy_conservation", &physics::find_energy_conservation,
                       py::arg("M"), py::arg("m"), py::arg("G"), py::arg("distance"), py::arg("b_velocity"));
    physics_module.def("energy_error", &physics::energy_error, py::arg("e_n"), py::arg("e_0"));
    physics_module.def("find_angular_momentum", &physics::find_angular_momentum,
                       py::arg("m"), py::arg("b_velocity"), py::arg("b"));
    physics_module.def("angular_error", &physics::angular_error, py::arg("l_n"), py::arg("l_0"));

    py::module_ generate_module = m.def_submodule("generate", "Procedural generation functions");
    py::class_<generate::ProblemSetup>(generate_module, "ProblemSetup")
        .def_readonly("star_mass", &generate::ProblemSetup::star_mass)
        .def_readonly("planet_mass", &generate::ProblemSetup::planet_mass)
        .def_readonly("star_pos", &generate::ProblemSetup::star_pos)
        .def_readonly("planet_pos", &generate::ProblemSetup::planet_pos);

    generate_module.def("solar_mass", &generate::solar_mass);
    generate_module.def("planetary_mass", &generate::planetary_mass, py::arg("stellar_mass"));
    generate_module.def("separation_distance", &generate::separation_distance);
    generate_module.def("problem_setup", &generate::problem_setup, py::arg("seed"));

    py::module_ integrators_module = m.def_submodule("integrators", "Orbital integration runners");

    py::module_ explicit_euler_module = integrators_module.def_submodule("explicit_euler");
    explicit_euler_module.def("run_explicit_euler", &explicit_euler::run_explicit_euler,
                              py::arg("star_pos"), py::arg("planet_pos"),
                              py::arg("star_mass"), py::arg("planet_mass"),
                              py::arg("t"), py::arg("iterations"), py::arg("G"),
                              py::arg("output_csv") = "");

    py::module_ symplectic_euler_module = integrators_module.def_submodule("symplectic_euler");
    symplectic_euler_module.def("run_symplectic_euler", &symplectic_euler::run_symplectic_euler,
                                py::arg("star_pos"), py::arg("planet_pos"),
                                py::arg("star_mass"), py::arg("planet_mass"),
                                py::arg("t"), py::arg("iterations"), py::arg("G"),
                                py::arg("output_csv") = "");

    py::module_ verlet_module = integrators_module.def_submodule("verlet");
    verlet_module.def("run_verlet", &verlet::run_verlet,
                      py::arg("star_pos"), py::arg("planet_pos"),
                      py::arg("star_mass"), py::arg("planet_mass"),
                      py::arg("t"), py::arg("iterations"), py::arg("G"),
                      py::arg("output_csv") = "");

    py::module_ rk4_module = integrators_module.def_submodule("rk4");
    rk4_module.def("run_rk4", &rk4::run_rk4,
                   py::arg("star_pos"), py::arg("planet_pos"),
                   py::arg("star_mass"), py::arg("planet_mass"),
                   py::arg("t"), py::arg("iterations"), py::arg("G"),
                   py::arg("output_csv") = "");

    py::module_ leapfrog_module = integrators_module.def_submodule("leapfrog");
    leapfrog_module.def("run_leapfrog", &leapfrog::run_leapfrog,
                        py::arg("star_pos"), py::arg("planet_pos"),
                        py::arg("star_mass"), py::arg("planet_mass"),
                        py::arg("t"), py::arg("iterations"), py::arg("G"),
                        py::arg("output_csv") = "");
}
