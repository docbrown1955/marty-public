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

#include "marty/api/modeling/modeling.h"
#include "csl/api/expr.h"
#include "marty/api/common/error.h"
#include "marty/api/group_theory/group_theory.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace py       = pybind11;
namespace group    = mty::api::group_theory;
namespace modeling = mty::api::modeling;

using csl::api::ExprHandle;

namespace {

// Matches the holder registered by group_theory.cpp.
using GroupHolder = std::shared_ptr<group::Group>;

std::vector<std::vector<csl::Expr>>
rawRows(std::vector<std::vector<ExprHandle>> const &rows)
{
    std::vector<std::vector<csl::Expr>> raw;
    raw.reserve(rows.size());
    for (auto const &row : rows) {
        std::vector<csl::Expr> line;
        line.reserve(row.size());
        for (auto const &entry : row)
            line.push_back(entry.raw());
        raw.push_back(std::move(line));
    }
    return raw;
}

// Python indices are relative to the end when negative.
std::size_t feynmanRuleIndex(modeling::FeynmanRules const &rules,
                             py::ssize_t                   index)
{
    py::ssize_t const size = static_cast<py::ssize_t>(rules.size());
    if (index < 0)
        index += size;
    if (index < 0 or index >= size)
        throw mty::api::LookupError("Feynman rule index "
                                    + std::to_string(index)
                                    + " is out of range");
    return static_cast<std::size_t>(index);
}

std::string typeName(py::handle obj)
{
    return py::str(py::type::of(obj).attr("__name__"));
}

std::vector<int> toLabels(py::handle obj)
{
    std::vector<int> labels;
    for (auto item : obj) {
        if (!py::isinstance<py::int_>(item))
            throw mty::api::TypeError("Dynkin labels must be integers, not "
                                      + typeName(item));
        labels.push_back(item.cast<int>());
    }
    return labels;
}

// Sets the representation of a particle. Lists are Dynkin labels, an int is a
// charge for U(1) or the single label of a doublet-like group, and a Fraction
// is a rational U(1) charge.
void setRep(modeling::Particle &self,
            std::string const  &group,
            py::object          value)
{
    if (py::isinstance<py::list>(value) or py::isinstance<py::tuple>(value)) {
        self.setLabels(group, toLabels(value));
    }
    else if (py::isinstance<py::int_>(value)) {
        int const n = value.cast<int>();
        if (self.isU1(group))
            self.setCharge(group, n, 1);
        else
            self.setLabels(group, {n});
    }
    else if (py::hasattr(value, "numerator")
             and py::hasattr(value, "denominator")) {
        self.setCharge(group,
                       value.attr("numerator").cast<int>(),
                       value.attr("denominator").cast<int>());
    }
    else {
        throw mty::api::TypeError("representation must be a list of labels, "
                                  "an int or a Fraction, "
                                  "not "
                                  + typeName(value));
    }
}

// renaming is "default" or a (partial) dict from particle name to new names.
void breakGaugeSymmetry(modeling::Model   &self,
                        std::string const &group,
                        py::object         renaming)
{
    if (py::isinstance<py::str>(renaming)) {
        if (renaming.cast<std::string>() != "default")
            throw mty::api::ValueError("renaming must be \"default\" or a "
                                       "dict, got \""
                                       + renaming.cast<std::string>() + "\"");
        self.breakGaugeSymmetry(group);
        return;
    }
    if (!py::isinstance<py::dict>(renaming))
        throw mty::api::TypeError("renaming must be \"default\" or a dict, "
                                  "not "
                                  + typeName(renaming));
    std::map<std::string, std::vector<std::string>> map;
    try {
        map = renaming.cast<std::map<std::string, std::vector<std::string>>>();
    }
    catch (py::cast_error const &) {
        throw mty::api::TypeError("renaming must map particle names to lists "
                                  "of new names");
    }
    self.breakGaugeSymmetry(group, map);
}

} // namespace

void bindModeling(py::module_ &m)
{
    py::module_ mod = m.def_submodule("modeling",
                                      "Declarative model construction: gauge "
                                      "groups, particles, breaking.");

    py::class_<modeling::GaugeGroup>(mod,
                                     "GaugeGroup",
                                     "Gauge group of a model: a group from "
                                     "pymarty.group_theory, a name, "
                                     "the name of its gauge bosons and "
                                     "optionally its coupling name.")
        .def(py::init([](GroupHolder const         &group,
                         std::string                name,
                         std::optional<std::string> bosonName,
                         std::optional<std::string> coupling) {
                 modeling::GaugeGroup item{group,
                                           name,
                                           bosonName.value_or(""),
                                           coupling.value_or("")};
                 return item;
             }),
             py::arg("group"),
             py::arg("name"),
             py::arg("boson_name") = py::none(),
             py::arg("coupling")   = py::none())
        .def_readonly(
            "name", &modeling::GaugeGroup::name, "Name of the group.")
        .def_readonly("boson_name",
                      &modeling::GaugeGroup::bosonName,
                      "Name of the gauge bosons.")
        .def_readonly("coupling",
                      &modeling::GaugeGroup::coupling,
                      "Name of the gauge coupling.")
        .def_property_readonly(
            "group",
            [](modeling::GaugeGroup const &self) {
                return std::const_pointer_cast<group::Group>(self.group);
            },
            "Group from pymarty.group_theory.");

    py::class_<modeling::FlavorGroup>(
        mod, "FlavorGroup", "Flavor group of a model: a name and a dimension.")
        .def(py::init([](std::string name, int dim) {
                 return modeling::FlavorGroup{name, dim};
             }),
             py::arg("name"),
             py::arg("dim"))
        .def_readonly(
            "name", &modeling::FlavorGroup::name, "Name of the group.")
        .def_readonly(
            "dim", &modeling::FlavorGroup::dim, "Dimension of the group.");

    py::class_<modeling::Particle>(
        mod, "Particle", "Particle of a model, with its representations.")
        .def_property_readonly(
            "name", &modeling::Particle::name, "Name of the particle.")
        .def("set_rep",
             &setRep,
             py::arg("group"),
             py::arg("value"),
             "Sets the representation under a gauge group: Dynkin labels, an "
             "int, or a Fraction for U(1).")
        .def("__repr__", [](modeling::Particle const &self) {
            return "Particle(" + self.name() + ")";
        });

    py::class_<modeling::FeynmanRules>(
        mod,
        "FeynmanRules",
        R"doc(Lazy view on the Feynman rules of a model.

The rules are computed on first use. Indexing returns the printed form of a
rule, negative indices count from the end.)doc")
        .def("__len__", &modeling::FeynmanRules::size)
        .def(
            "__getitem__",
            [](modeling::FeynmanRules const &self, py::ssize_t index) {
                return self.str(feynmanRuleIndex(self, index));
            },
            py::arg("index"),
            "Printed form of the rule at index.")
        .def("__iter__",
             [](modeling::FeynmanRules const &self) {
                 std::vector<std::string> printed;
                 for (std::size_t i = 0; i < self.size(); ++i)
                     printed.push_back(self.str(i));
                 return py::iter(py::cast(printed));
             })
        .def(
            "latex",
            [](modeling::FeynmanRules const &self, py::ssize_t index) {
                return self.latex(feynmanRuleIndex(self, index));
            },
            py::arg("index"),
            "LaTeX block of the rule at index.")
        .def("latex_document",
             &modeling::FeynmanRules::latexDocument,
             "LaTeX document with all the rules.")
        .def("__repr__", [](modeling::FeynmanRules const &self) {
            return "FeynmanRules(" + std::to_string(self.size()) + ")";
        });

    py::class_<modeling::Model>(mod,
                                "Model",
                                "Model of a QFT: gauge and flavor groups, "
                                "particles, Lagrangian terms "
                                "and Feynman rules. Built from its groups in "
                                "one call.")
        .def(py::init([](std::vector<modeling::GaugeGroup> const  &gauge,
                         std::vector<modeling::FlavorGroup> const &flavor) {
                 return modeling::Model(gauge, flavor);
             }),
             py::arg("gauge"),
             py::arg("flavor") = std::vector<modeling::FlavorGroup>{},
             "Model with the given gauge and flavor groups, initialized and "
             "with its gauge bosons named.")
        .def("weyl_fermion",
             &modeling::Model::weylFermion,
             py::arg("name"),
             py::arg("chirality") = "L",
             "Unattached Weyl fermion, chirality \"L\" or \"R\".")
        .def("dirac_fermion",
             &modeling::Model::diracFermion,
             py::arg("name"),
             "Unattached Dirac fermion.")
        .def("majorana_fermion",
             &modeling::Model::majoranaFermion,
             py::arg("name"),
             "Unattached Majorana fermion.")
        .def("scalar_boson",
             &modeling::Model::scalarBoson,
             py::arg("name"),
             "Unattached real scalar boson.")
        .def("add_particle",
             &modeling::Model::addParticle,
             py::arg("particle"),
             "Adds a particle to the model.")
        .def("particle",
             &modeling::Model::particle,
             py::arg("name"),
             "Particle of the model with this name (LookupError if absent).")
        .def("rename_particle",
             &modeling::Model::renameParticle,
             py::arg("name"),
             py::arg("new_name"),
             "Renames a particle of the model.")
        .def("__contains__",
             &modeling::Model::contains,
             py::arg("name"),
             "Whether the model holds a particle with this name.")
        .def_property_readonly("particles",
                               &modeling::Model::particles,
                               "Particles of the model.")
        .def_property_readonly("feynman_rules",
                               &modeling::Model::feynmanRules,
                               "Lazy view on the Feynman rules of the model.")
        .def("refresh",
             &modeling::Model::refresh,
             "Recomputes the derived quantities of the model.")
        .def(
            "add_term",
            [](modeling::Model  &self,
               ExprHandle const &term,
               bool addHermitic) { self.addTerm(term.raw(), addHermitic); },
            py::arg("term"),
            py::arg("add_hermitic") = false,
            "Adds a term to the Lagrangian; add_hermitic also adds its "
            "conjugate.")
        .def(
            "replace",
            [](modeling::Model  &self,
               ExprHandle const &oldExpr,
               ExprHandle const &newExpr) {
                self.replace(oldExpr.raw(), newExpr.raw());
            },
            py::arg("old"),
            py::arg("new"),
            "Replaces an expression everywhere in the model.")
        .def(
            "rotate_fields",
            [](modeling::Model                            &self,
               std::vector<modeling::Particle> const      &fields,
               std::vector<modeling::Particle> const      &newFields,
               std::vector<std::vector<ExprHandle>> const &rotation,
               bool                                        diagonalize,
               int                                         nMassLess) {
                self.rotateFields(fields,
                                  newFields,
                                  rawRows(rotation),
                                  diagonalize,
                                  nMassLess);
            },
            py::arg("fields"),
            py::arg("new_fields"),
            py::arg("rotation"),
            py::arg("diagonalize_masses") = false,
            py::arg("n_mass_less_fields") = 0,
            "Rotates fields into new_fields with a unitary matrix given as "
            "rows of expressions.")
        .def("birotate_fields",
             &modeling::Model::birotateFields,
             py::arg("fields1"),
             py::arg("fields2"),
             py::arg("n_mass_less_fields") = 0,
             "Rotates the left and right fields of a pair of particles.")
        .def(
            "add_bosonic_mass",
            [](modeling::Model          &self,
               modeling::Particle const &field,
               ExprHandle const         &mass) {
                self.addBosonicMass(field, mass.raw());
            },
            py::arg("field"),
            py::arg("mass"),
            "Adds a bosonic mass term for a field.")
        .def(
            "add_fermionic_mass",
            [](modeling::Model          &self,
               modeling::Particle const &left,
               modeling::Particle const &right,
               ExprHandle const         &mass) {
                self.addFermionicMass(left, right, mass.raw());
            },
            py::arg("left"),
            py::arg("right"),
            py::arg("mass"),
            "Adds a fermionic mass term between a left and a right fermion.")
        .def(
            "add_fermionic_mass",
            [](modeling::Model          &self,
               modeling::Particle const &dirac,
               ExprHandle const         &mass) {
                self.addFermionicMass(dirac, mass.raw());
            },
            py::arg("dirac_fermion"),
            py::arg("mass"),
            "Adds a Dirac fermion mass term.")
        .def(
            "coupling",
            [](modeling::Model const &self, std::string const &name) {
                return ExprHandle(self.coupling(name));
            },
            py::arg("name"),
            "Scalar coupling of the model (LookupError if absent).")
        .def(
            "new_coupling",
            [](modeling::Model &self, std::string const &name) {
                return ExprHandle(self.newCoupling(name));
            },
            py::arg("name"),
            "Creates a scalar coupling of the model and returns it.")
        .def("break_gauge_symmetry",
             &breakGaugeSymmetry,
             py::arg("group"),
             py::arg("renaming") = py::str("default"),
             "Breaks a gauge group. renaming is \"default\" or a dict from "
             "particle names to lists of new names; it may be partial.")
        .def("__str__",
             &modeling::Model::describe,
             "Full description of the model.")
        .def("__repr__", [](modeling::Model const &self) {
            return "Model(" + std::to_string(self.particles().size())
                   + " particles)";
        });

    mod.def("standard_model",
            &modeling::Model::standardModel,
            "The built-in Standard Model.");
    mod.def("qed", &modeling::Model::qed, "The built-in QED model.");
}
