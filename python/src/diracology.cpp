// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "marty/api/diracology.h"
#include "csl/api/expr.h"
#include "marty/api/common/error.h"
#include "marty/core/diracology.h"

#include <pybind11/pybind11.h>

#include <memory>
#include <string>
#include <vector>

namespace py    = pybind11;
namespace dirac = mty::api::diracology;

namespace {

using Expr = dirac::Expr;

// Converts a Python number or symbolic expression into a CSL coefficient.
// Returns false when the type is not a supported coefficient.
bool toCoefficient(py::handle obj, dirac::Coefficient &out)
{
    if (py::isinstance<py::int_>(obj)) {
        out = dirac::fromInt(obj.cast<long>());
        return true;
    }
    if (py::isinstance<py::float_>(obj)) {
        out = dirac::fromDouble(obj.cast<double>());
        return true;
    }
    py::object fraction = py::module_::import("fractions").attr("Fraction");
    if (py::isinstance(obj, fraction)) {
        out = dirac::fromRational(obj.attr("numerator").cast<long>(),
                                  obj.attr("denominator").cast<long>());
        return true;
    }
    if (py::isinstance<csl::api::ExprHandle>(obj)) {
        out = obj.cast<csl::api::ExprHandle const &>().raw();
        return true;
    }
    return false;
}

std::vector<Expr> toChain(py::iterable items)
{
    std::vector<Expr> chain;
    for (py::handle item : items) {
        if (!py::isinstance<Expr>(item)) {
            throw mty::api::TypeError("a gamma chain must contain only "
                                      "DiracExpr objects");
        }
        chain.push_back(item.cast<Expr>());
    }
    return chain;
}

// Applies op to two operands, where at least one is a DiracExpr and the
// other is a DiracExpr or a number. Returns NotImplemented otherwise, so that
// Python can try the reflected operation of the other operand.
template <typename Op>
py::object binary(Expr const &self, py::handle other, bool reflected, Op op)
{
    if (py::isinstance<Expr>(other)) {
        Expr rhs = other.cast<Expr>();
        return py::cast(reflected ? op(rhs, self) : op(self, rhs));
    }
    dirac::Coefficient coefficient;
    if (!toCoefficient(other, coefficient)) {
        return py::reinterpret_borrow<py::object>(Py_NotImplemented);
    }
    return py::cast(reflected ? op(coefficient, self) : op(self, coefficient));
}

// Canonical equality: a DiracExpr is compared to another DiracExpr or to a
// number, the number being compared in CSL.
py::object equal(Expr const &self, py::handle other)
{
    if (py::isinstance<Expr>(other)) {
        return py::cast(dirac::equals(self, other.cast<Expr>()));
    }
    dirac::Coefficient coefficient;
    if (!toCoefficient(other, coefficient)) {
        return py::reinterpret_borrow<py::object>(Py_NotImplemented);
    }
    return py::cast(dirac::equalsCoefficient(self, coefficient));
}

// Activates a global convention of the facade for a `with` block. Guard is one
// of the Scoped* classes of marty/api/diracology.h, built from a bool.
template <typename Guard>
class ScopedBlock {
  public:
    explicit ScopedBlock(bool value) : _value(value)
    {
    }

    void enter()
    {
        if (_guard) {
            throw mty::api::StateError("context manager entered twice");
        }
        _guard = std::make_unique<Guard>(_value);
    }

    void exit()
    {
        _guard.reset();
    }

  private:
    bool                   _value;
    std::unique_ptr<Guard> _guard;
};

using SymbolicDimension    = ScopedBlock<dirac::ScopedSymbolicDimension>;
using EpsilonIndexOrdering = ScopedBlock<dirac::ScopedEpsilonIndexOrdering>;

// Binds a ScopedBlock as a context manager named `pyName`.
template <typename Block>
void bindScopedBlock(py::module_ &m, char const *pyName)
{
    py::class_<Block>(m, pyName)
        .def(py::init<bool>(), py::arg("value"))
        .def(
            "__enter__",
            [](Block &self) {
                self.enter();
                return &self;
            },
            py::return_value_policy::reference_internal)
        .def("__exit__", [](Block &self, py::args) {
            self.exit();
            return false;
        });
}

} // namespace

void bindDiracology(py::module_ &m)
{
    py::module_ d = m.def_submodule("diracology",
                                    "Dirac algebra: gamma matrices, traces "
                                    "and Fierz.");

    py::enum_<dirac::FierzBasis>(d, "FierzBasis")
        .value("STANDARD", dirac::FierzBasis::Standard)
        .value("CHIRAL", dirac::FierzBasis::Chiral);

    py::class_<Expr> expr(d, "DiracExpr");
    expr.def("__str__", [](Expr const &e) { return dirac::generateString(e); })
        .def("__repr__",
             [](Expr const &e) {
                 return "DiracExpr(" + dirac::generateString(e) + ")";
             })
        .def("to_latex", [](Expr const &e) { return dirac::generateLatex(e); })
        .def("_repr_latex_",
             [](Expr const &e) { return "$" + dirac::generateLatex(e) + "$"; })
        .def("simplify", [](Expr const &e) { return dirac::simplified(e); })
        .def("order", [](Expr const &e) { return dirac::ordered(e); })
        .def(
            "project",
            [](Expr const &e, dirac::FierzBasis basis) {
                return dirac::project(e, basis);
            },
            py::arg("basis"))
        .def("__eq__", &equal)
        .def("__neg__", [](Expr const &e) { return -e; })
        .def("__add__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, false, [](auto const &a, auto const &b) {
                         return a + b;
                     });
             })
        .def("__radd__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, true, [](auto const &a, auto const &b) {
                         return a + b;
                     });
             })
        .def("__sub__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, false, [](auto const &a, auto const &b) {
                         return a - b;
                     });
             })
        .def("__rsub__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, true, [](auto const &a, auto const &b) {
                         return a - b;
                     });
             })
        .def("__mul__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, false, [](auto const &a, auto const &b) {
                         return a * b;
                     });
             })
        .def("__rmul__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, true, [](auto const &a, auto const &b) {
                         return a * b;
                     });
             })
        .def("__truediv__",
             [](Expr const &self, py::handle other) {
                 return binary(
                     self, other, false, [](auto const &a, auto const &b) {
                         return a / b;
                     });
             })
        .def("__rtruediv__", [](Expr const &self, py::handle other) {
            return binary(self, other, true, [](auto const &a, auto const &b) {
                return a / b;
            });
        });
    // Defining __eq__ on a value type that has no stable hash: unhashable.
    expr.attr("__hash__") = py::none();

    d.def("g", &dirac::g, py::arg("mu"), py::arg("nu"));
    d.def("eps",
          &dirac::eps,
          py::arg("a"),
          py::arg("b"),
          py::arg("c"),
          py::arg("d"));
    d.def("gamma", py::overload_cast<int>(&dirac::gamma), py::arg("mu"));
    d.def("gamma",
          py::overload_cast<int, int>(&dirac::gamma),
          py::arg("mu"),
          py::arg("nu"));
    d.def("gamma5", &dirac::gamma5);
    d.def("P_L", &dirac::P_L);
    d.def("P_R", &dirac::P_R);
    d.def("C", &dirac::C);
    d.attr("D") = dirac::DMinko();
    d.attr("I") = dirac::imaginaryUnit();

    d.def(
        "current",
        [](py::iterable chain, int iLeft, int iRight) {
            return dirac::current(toChain(chain), iLeft, iRight);
        },
        py::arg("chain"),
        py::arg("i_left"),
        py::arg("i_right"));
    d.def(
        "trace",
        [](py::iterable chain) { return dirac::trace(toChain(chain)); },
        py::arg("chain"));

    d.def(
        "single_fierz",
        [](Expr const &c1, Expr const &c2, dirac::FierzBasis basis) {
            auto result = dirac::singleFierz(c1, c2, basis);
            return py::make_tuple(result.rhs, result.lhs);
        },
        py::arg("chain1"),
        py::arg("chain2"),
        py::arg("basis"));
    d.def(
        "double_fierz",
        [](Expr const &c1, Expr const &c2, dirac::FierzBasis basis) {
            auto result = dirac::doubleFierz(c1, c2, basis);
            return py::make_tuple(result.rhs, result.lhs);
        },
        py::arg("chain1"),
        py::arg("chain2"),
        py::arg("basis"));

    d.def("set_symbolic_dimension",
          &dirac::keepSymbolic4D,
          py::arg("keep_symbolic"));

    bindScopedBlock<SymbolicDimension>(d, "SymbolicDimension");
    bindScopedBlock<EpsilonIndexOrdering>(d, "EpsilonIndexOrdering");
}
