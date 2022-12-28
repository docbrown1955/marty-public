#include <csl/options.h>
#include <marty/api/gamma.h>
#include <marty/core/diracology.h>
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
        .def(py::init<int>())
        .def(py::init<float>())
        .def("__str__",
             [](mty::gamma_api::Expr const &expr) {
                 return mty::gamma_api::generateString(expr);
             })
        .def("__copy__",
             [](mty::gamma_api::Expr const &expr) { return expr->copy(); })
        .def("__deepcopy__",
             [](mty::gamma_api::Expr const &expr, py::object) {
                 return sgl::DeepCopy(expr);
             })
        .def("to_latex",
             [](mty::gamma_api::Expr const &expr) {
                 return mty::gamma_api::generateLatex(expr);
             })
        .def("simplify",
             [](mty::gamma_api::Expr const &expr) {
                 return mty::gamma_api::simplified(expr);
             })
        .def("order",
             [](mty::gamma_api::Expr const &expr) {
                 return mty::gamma_api::ordered(expr);
             })
        .def("project",
             [](mty::gamma_api::Expr const &expr,
                mty::gamma_api::FierzBasis  basis) {
                 return mty::gamma_api::project(expr, basis);
             })
        .def("__len__",
             [](mty::gamma_api::Expr const &expr) { return expr->size(); })
        .def("__getitem__",
             [](mty::gamma_api::Expr const &expr, int index) {
                 if (static_cast<unsigned int>(index) >= expr->size()) {
                     throw std::out_of_range("Index " + std::to_string(index)
                                             + " is out of range of expr of "
                                               "size "
                                             + std::to_string(expr->size()));
                 }
                 return expr->argument(index);
             })
        .def("__eq__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        == sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
        .def("__ne__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        != sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
        .def("__lt__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        < sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
        .def("__gt__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        > sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
        .def("__le__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        <= sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
        .def("__ge__",
             [](mty::gamma_api::Expr const &left,
                mty::gamma_api::Expr const &right) {
                 return sgl::sgl_to_csl(left, mty::defaultSGLTensorSet())
                        >= sgl::sgl_to_csl(right, mty::defaultSGLTensorSet());
             })
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
    gapi.attr("D") = mty::gamma_api::DMinko;
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
}