#include <csl/options.h>
#include <marty/api/gamma.h>
#include <marty/core/mrtInterface.h>
#include <marty/sgl/sgl.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

namespace py = pybind11;

PYBIND11_MODULE(_pymarty, m)
{
    py::module gapi
        = m.def_submodule("gamma_api", "An API for gamma-matrix calculations");
    py::class_<mty::gamma_api::Expr>(gapi, "Expr")
        .def(-py::self)
        .def(py::self + py::self)
        .def(py::self * py::self)
        .def(py::self - py::self)
        .def(py::self / py::self)
        .def(py::self + int())
        .def(py::self * int())
        .def(py::self - int())
        .def(py::self / int())
        .def(int() + py::self)
        .def(int() * py::self)
        .def(int() - py::self)
        .def(int() / py::self)
        .def(py::self + float())
        .def(py::self * float())
        .def(py::self - float())
        .def(py::self / float())
        .def(float() + py::self)
        .def(float() * py::self)
        .def(float() - py::self)
        .def(float() / py::self);
    py::enum_<mty::gamma_api::FierzBasis>(gapi, "FierzBasis")
        .value("Standard", mty::gamma_api::FierzBasis::Standard)
        .value("Chiral", mty::gamma_api::FierzBasis::Chiral)
        .export_values();
    gapi.def("g",
             &mty::gamma_api::g,
             "Minkowski metric - 2 indices",
             py::arg("mu"),
             py::arg("nu"));
    gapi.def("eps",
             &mty::gamma_api::eps,
             "Full-antisymmetric epsilon tensor - 4 indices",
             py::arg("mu"),
             py::arg("nu"),
             py::arg("rho"),
             py::arg("sigma"));
    gapi.def(
        "gamma",
        static_cast<mty::gamma_api::Expr (*)(int)>(&mty::gamma_api::gamma),
        "Gamma matrix - 1 index",
        py::arg("mu"));
    gapi.def("gamma",
             static_cast<mty::gamma_api::Expr (*)(int, int)>(
                 &mty::gamma_api::gamma),
             "Antisymmetric gamma matrix - 2 indices",
             py::arg("mu"),
             py::arg("nu"));
    gapi.def("gamma5",
             static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::gamma5),
             "Gamma5 matrix - no index");
    gapi.def("P_L",
             static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::P_L),
             "P_L projector - no index");
    gapi.def("P_R",
             static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::P_R),
             "P_R projector - no index");
    gapi.def("C",
             static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::C),
             "C conjugation matrix - no index");

    gapi.def("current",
             static_cast<mty::gamma_api::Expr (*)(
                 std::vector<mty::gamma_api::Expr> const &, int, int)>(
                 &mty::gamma_api::current),
             "Build a fermion current from a list of gamma matrices and "
             "the two "
             "border (Dirac) indices.",
             py::arg("gamma_chain"),
             py::arg("left_current_index"),
             py::arg("right_current_index"));
    gapi.def("set_symbolic_dimension",
             static_cast<void (*)(bool)>(&mty::gamma_api::keepSymbolic4D),
             "Set the dimension as symbolic or not (D = 4)",
             py::arg("symbolic"));
    gapi.def("trace",
             static_cast<mty::gamma_api::Expr (*)(
                 std::vector<mty::gamma_api::Expr> const &)>(
                 &mty::gamma_api::trace),
             "Calculate the trace of a chain of gamma matrices.",
             py::arg("gamma_chain"));
    gapi.def(
        "simplify",
        static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &)>(
            &mty::gamma_api::simplified),
        "Simplify an expression applying gamma-matrix identities",
        py::arg("expr"));
    gapi.def(
        "order",
        static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &)>(
            &mty::gamma_api::ordered),
        "Order chains of gamma-matrices",
        py::arg("expr"));
    gapi.def("project",
             static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &,
                                                  mty::gamma_api::FierzBasis)>(
                 &mty::gamma_api::project),
             "Project fermion currents on a basis (chiral or not)",
             py::arg("expr"),
             py::arg("basis"));
    gapi.def("single_fierz",
             static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &,
                                                  mty::gamma_api::Expr const &,
                                                  mty::gamma_api::FierzBasis)>(
                 &mty::gamma_api::applySingleFierz),
             "Apply once the Fierz identity",
             py::arg("left_chain"),
             py::arg("right_chain"),
             py::arg("basis") = mty::gamma_api::FierzBasis::Chiral);
    gapi.def("double_fierz",
             static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &,
                                                  mty::gamma_api::Expr const &,
                                                  mty::gamma_api::FierzBasis)>(
                 &mty::gamma_api::applyDoubleFierz),
             "Apply twice the Fierz identity",
             py::arg("left_chain"),
             py::arg("right_chain"),
             py::arg("basis") = mty::gamma_api::FierzBasis::Chiral);
    gapi.def("generate_string",
             static_cast<std::string (*)(mty::gamma_api::Expr const &)>(
                 &mty::gamma_api::generateString),
             "Print an expression in a regular string",
             py::arg("expr"));
    gapi.def("generate_latex",
             static_cast<std::string (*)(mty::gamma_api::Expr const &)>(
                 &mty::gamma_api::generateLatex),
             "Print an expression as a LaTeX expression",
             py::arg("expr"));
}