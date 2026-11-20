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

#include "marty/api/group_theory/group_theory.h"
#include "marty/api/common/error.h"

#include <pybind11/pybind11.h>

#include <memory>
#include <string>
#include <vector>

namespace py    = pybind11;
namespace group = mty::api::group_theory;

namespace {

using GroupPtr = std::shared_ptr<const group::Group>;
// pybind11 holders must match the constness of the class. Nothing reached from
// Python modifies a Group, so the constness is only cast away at this
// boundary.
using GroupHolder = std::shared_ptr<group::Group>;

GroupHolder asHolder(GroupPtr const &group)
{
    return std::const_pointer_cast<group::Group>(group);
}

std::string typeName(py::handle obj)
{
    return py::str(py::type::of(obj).attr("__name__"));
}

// Accepts int and any object with __index__, but not float or str.
int toLabel(py::handle obj)
{
    if (py::isinstance<py::int_>(obj)) {
        return obj.cast<int>();
    }
    if (py::hasattr(obj, "__index__")) {
        return obj.attr("__index__")().cast<int>();
    }
    throw mty::api::TypeError("Dynkin labels must be integers, not "
                              + typeName(obj));
}

// Accepts int and fractions.Fraction, the two types of U(1) charges.
group::Rational toRational(py::handle obj)
{
    if (py::isinstance<py::int_>(obj)) {
        return group::Rational(obj.cast<long long>(), 1);
    }
    py::object fraction = py::module_::import("fractions").attr("Fraction");
    if (py::isinstance(obj, fraction)) {
        return group::Rational(obj.attr("numerator").cast<long long>(),
                               obj.attr("denominator").cast<long long>());
    }
    throw mty::api::TypeError("U(1) charges must be int or Fraction, not "
                              + typeName(obj));
}

py::object toFraction(group::Rational const &r)
{
    return py::module_::import("fractions").attr("Fraction")(r.num, r.den);
}

std::string formatRational(group::Rational const &r)
{
    std::string text = std::to_string(r.num);
    if (r.den != 1) {
        text += "/" + std::to_string(r.den);
    }
    return text;
}

py::tuple labelsTuple(group::Irrep const &irrep)
{
    std::vector<int> labels = irrep.labels();
    py::tuple        result(labels.size());
    for (std::size_t i = 0; i < labels.size(); ++i) {
        result[i] = labels[i];
    }
    return result;
}

// Irrep(*labels) for SU, SO and E6; Irrep(charge) for U(1). A single iterable
// argument is accepted as the list of labels.
group::Irrep groupIrrep(GroupHolder const &self, py::args args)
{
    if (self->isU1()) {
        if (args.size() != 1) {
            throw mty::api::ValueError("U(1) irreps take one charge");
        }
        return self->u1Irrep(toRational(args[0]));
    }
    std::vector<int> labels;
    if (args.size() == 1 && !py::isinstance<py::int_>(args[0])
        && py::hasattr(args[0], "__iter__")) {
        for (py::handle item : args[0]) {
            labels.push_back(toLabel(item));
        }
    }
    else {
        for (py::handle item : args) {
            labels.push_back(toLabel(item));
        }
    }
    return self->irrep(labels);
}

py::object notImplemented()
{
    return py::reinterpret_borrow<py::object>(Py_NotImplemented);
}

// Hashable key consistent with equality: the group name, then the highest
// weight (or the charge for U(1)).
py::tuple irrepKey(group::Irrep const &irrep)
{
    py::object label = irrep.group()->isU1()
                           ? toFraction(irrep.charge())
                           : static_cast<py::object>(labelsTuple(irrep));
    return py::make_tuple(irrep.group()->name(), label);
}

py::dict toDict(group::Decomposition const &d)
{
    py::dict result;
    for (auto const &[irrep, multiplicity] : d.terms) {
        result[py::cast(irrep)] = multiplicity;
    }
    return result;
}

// Index of irrep in the decomposition, or -1.
long findTerm(group::Decomposition const &d, py::handle key)
{
    if (!py::isinstance<group::Irrep>(key)) {
        return -1;
    }
    auto const &irrep = key.cast<group::Irrep const &>();
    for (std::size_t i = 0; i < d.terms.size(); ++i) {
        if (d.terms[i].first == irrep) {
            return static_cast<long>(i);
        }
    }
    return -1;
}

} // namespace

void bindGroupTheory(py::module_ &m)
{
    py::module_ g = m.def_submodule("group_theory",
                                    "Group theory: simple groups, irreps and "
                                    "tensor products.");

    py::class_<group::Group, GroupHolder> groupClass(g, "Group");
    groupClass
        .def_static(
            "su",
            [](int n) { return asHolder(group::Group::su(n)); },
            py::arg("n"))
        .def_static(
            "so",
            [](int n) { return asHolder(group::Group::so(n)); },
            py::arg("n"))
        .def_static("e6", [] { return asHolder(group::Group::e6()); })
        .def_static("u1", [] { return asHolder(group::Group::u1()); })
        .def_property_readonly("name", &group::Group::name)
        .def_property_readonly("rank", &group::Group::rank)
        .def_property_readonly("dim", &group::Group::dim)
        .def_property_readonly("is_u1", &group::Group::isU1)
        .def("irrep",
             [](GroupHolder const &self, py::args args) {
                 return groupIrrep(self, args);
             })
        .def("fundamental", &group::Group::fundamental)
        .def("adjoint", &group::Group::adjoint)
        .def("trivial", &group::Group::trivial)
        .def("tensor_product",
             &group::Group::tensorProduct,
             py::arg("a"),
             py::arg("b"))
        .def("__repr__", &group::Group::name);

    py::class_<group::Irrep> irrepClass(g, "Irrep");
    irrepClass
        .def_property_readonly(
            "group",
            [](group::Irrep const &self) { return asHolder(self.group()); })
        .def_property_readonly("dim", &group::Irrep::dim)
        .def_property_readonly(
            "highest_weight",
            [](group::Irrep const &self) { return labelsTuple(self); })
        .def_property_readonly("charge",
                               [](group::Irrep const &self) {
                                   if (!self.group()->isU1()) {
                                       throw mty::api::ValueError("charge is "
                                                                  "only "
                                                                  "defined "
                                                                  "for U(1) "
                                                                  "irreps");
                                   }
                                   return toFraction(self.charge());
                               })
        .def_property_readonly("casimir",
                               [](group::Irrep const &self) {
                                   return toFraction(self.casimir());
                               })
        .def_property_readonly("dynkin_index",
                               [](group::Irrep const &self) {
                                   return toFraction(self.dynkinIndex());
                               })
        .def_property_readonly("is_self_conjugate",
                               &group::Irrep::isSelfConjugate)
        .def("conjugate", &group::Irrep::conjugate)
        .def("__invert__", &group::Irrep::conjugate)
        .def("__eq__",
             [](group::Irrep const &self, py::handle other) -> py::object {
                 if (!py::isinstance<group::Irrep>(other)) {
                     return notImplemented();
                 }
                 return py::bool_(self == other.cast<group::Irrep const &>());
             })
        .def("__lt__",
             [](group::Irrep const &self, py::handle other) -> py::object {
                 if (!py::isinstance<group::Irrep>(other)) {
                     return notImplemented();
                 }
                 return py::bool_(self < other.cast<group::Irrep const &>());
             })
        .def("__hash__",
             [](group::Irrep const &self) { return py::hash(irrepKey(self)); })
        .def("__mul__",
             [](group::Irrep const &self, py::handle other) -> py::object {
                 if (!py::isinstance<group::Irrep>(other)) {
                     return notImplemented();
                 }
                 return py::cast(self.group()->tensorProduct(
                     self, other.cast<group::Irrep const &>()));
             })
        .def("__repr__", [](group::Irrep const &self) {
            std::string text = self.group()->name() + "[";
            if (self.group()->isU1()) {
                text += formatRational(self.charge());
            }
            else {
                std::vector<int> labels = self.labels();
                for (std::size_t i = 0; i < labels.size(); ++i) {
                    text += (i ? "," : "") + std::to_string(labels[i]);
                }
            }
            return text + "]";
        });

    py::class_<group::Decomposition> decomposition(g, "Decomposition");
    decomposition.def_property_readonly("dim", &group::Decomposition::dim)
        .def(
            "__len__",
            [](group::Decomposition const &self) { return self.terms.size(); })
        .def("__getitem__",
             [](group::Decomposition const &self, py::handle key) {
                 long index = findTerm(self, key);
                 if (index < 0) {
                     throw py::key_error(py::repr(key));
                 }
                 return self.terms[static_cast<std::size_t>(index)].second;
             })
        .def("__contains__",
             [](group::Decomposition const &self, py::handle key) {
                 return findTerm(self, key) >= 0;
             })
        .def("__iter__",
             [](group::Decomposition const &self) {
                 py::list keys;
                 for (auto const &term : self.terms) {
                     keys.append(py::cast(term.first));
                 }
                 return py::iter(keys);
             })
        .def("keys",
             [](group::Decomposition const &self) {
                 return py::list(toDict(self).attr("keys")());
             })
        .def("values",
             [](group::Decomposition const &self) {
                 return py::list(toDict(self).attr("values")());
             })
        .def("items",
             [](group::Decomposition const &self) {
                 return py::list(toDict(self).attr("items")());
             })
        .def("__eq__",
             [](group::Decomposition const &self, py::handle other) {
                 return toDict(self).attr("__eq__")(other);
             })
        .def("__repr__", [](group::Decomposition const &self) {
            return "Decomposition(" + std::string(py::repr(toDict(self)))
                   + ")";
        });
    // Compared to a dict, so unhashable like one.
    decomposition.attr("__hash__") = py::none();
}
