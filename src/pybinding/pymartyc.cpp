#include <marty/api/gamma.h>
#include <marty/core/mrtInterface.h>
#include <marty/sgl/sgl.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

namespace py = pybind11;

PYBIND11_MODULE(pymartyc, m)
{
    m.doc() = "some basic example";
    m.def("minkowski_index",
          &mty::MinkowskiIndex,
          "Returns a minkowski index",
          py::arg("name") = "");
    m.attr("__version__") = "dev";

    py::class_<csl::Index>(m, "Index")
        .def_property("name", &csl::Index::getName, &csl::Index::setName)
        .def("__str__", [](csl::Index const &i) {
            std::ostringstream sout;
            sout << i;
            return sout.str();
        });

    py::class_<mty::gamma_api::Expr>(m, "GExpr");
    py::enum_<mty::gamma_api::FierzBasis>(m, "FierzBasis")
        .value("Standard", mty::gamma_api::FierzBasis::Standard)
        .value("Chiral", mty::gamma_api::FierzBasis::Chiral)
        .export_values();
    m.def("g",
          &mty::gamma_api::g,
          "Minkowski metric - 2 indices",
          py::arg("mu"),
          py::arg("nu"));
    m.def("eps",
          &mty::gamma_api::eps,
          "Full-antisymmetric epsilon tensor - 4 indices",
          py::arg("mu"),
          py::arg("nu"),
          py::arg("rho"),
          py::arg("sigma"));
    m.def("gamma",
          static_cast<mty::gamma_api::Expr (*)(int)>(&mty::gamma_api::gamma),
          "Gamma matrix - 1 index",
          py::arg("mu"));
    m.def("gamma",
          static_cast<mty::gamma_api::Expr (*)(int, int)>(
              &mty::gamma_api::gamma),
          "Antisymmetric gamma matrix - 2 indices",
          py::arg("mu"),
          py::arg("nu"));
    m.def("gamma5",
          static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::gamma5),
          "Gamma5 matrix - no index");
    m.def("P_L",
          static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::P_L),
          "P_L projector - no index");
    m.def("P_R",
          static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::P_R),
          "P_R projector - no index");
    m.def("C",
          static_cast<mty::gamma_api::Expr (*)()>(&mty::gamma_api::C),
          "C conjugation matrix - no index");

    m.def("set_symbolic_dimension",
          static_cast<void (*)(bool)>(&mty::gamma_api::keepSymbolic4D),
          "Set the dimension as symbolic or not (D = 4)",
          py::arg("symbolic"));
    m.def("chain",
          static_cast<mty::gamma_api::Expr (*)(
              std::vector<mty::gamma_api::Expr> const &, int, int)>(
              &mty::gamma_api::chain),
          "Build a fermion current from a list of gamma matrices and the two "
          "border (Dirac) indices.",
          py::arg("gamma_chain"),
          py::arg("left_current_index"),
          py::arg("right_current_index"));
    m.def("trace",
          static_cast<mty::gamma_api::Expr (*)(
              std::vector<mty::gamma_api::Expr> const &)>(
              &mty::gamma_api::trace),
          "Calculate the trace of a chain of gamma matrices.",
          py::arg("gamma_chain"));
    m.def("simplify",
          static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &)>(
              &mty::gamma_api::simplified),
          "Simplify an expression applying gamma-matrix identities",
          py::arg("expr"));
    m.def("order",
          static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &)>(
              &mty::gamma_api::ordered),
          "Order chains of gamma-matrices",
          py::arg("expr"));
    m.def("single_fierz",
          static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &,
                                               mty::gamma_api::Expr const &,
                                               mty::gamma_api::FierzBasis)>(
              &mty::gamma_api::applySingleFierz),
          "Apply once the Fierz identity",
          py::arg("left_chain"),
          py::arg("right_chain"),
          py::arg("basis") = mty::gamma_api::FierzBasis::Chiral);
    m.def("double_fierz",
          static_cast<mty::gamma_api::Expr (*)(mty::gamma_api::Expr const &,
                                               mty::gamma_api::Expr const &,
                                               mty::gamma_api::FierzBasis)>(
              &mty::gamma_api::applyDoubleFierz),
          "Apply twice the Fierz identity",
          py::arg("left_chain"),
          py::arg("right_chain"),
          py::arg("basis") = mty::gamma_api::FierzBasis::Chiral);
    m.def("generate_string",
          static_cast<std::string (*)(mty::gamma_api::Expr const &)>(
              &mty::gamma_api::generateString),
          "Print an expression in a regular string",
          py::arg("expr"));
    m.def("generate_latex",
          static_cast<std::string (*)(mty::gamma_api::Expr const &)>(
              &mty::gamma_api::generateLatex),
          "Print an expression as a LaTeX expression",
          py::arg("expr"));
}