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
#include "marty/api/common/error.h"
#include "marty/api/group_theory/group_theory.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace py       = pybind11;
namespace group    = mty::api::group_theory;
namespace modeling = mty::api::modeling;

namespace {

// Matches the holder registered by group_theory.cpp.
using GroupHolder = std::shared_ptr<group::Group>;

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

    py::class_<modeling::GaugeGroup>(mod, "GaugeGroup")
        .def(py::init([](GroupHolder const &group,
                         std::string        name,
                         std::string        bosonName) {
                 return modeling::GaugeGroup{group, name, bosonName};
             }),
             py::arg("group"),
             py::arg("name"),
             py::arg("boson_name"))
        .def_readonly("name", &modeling::GaugeGroup::name)
        .def_readonly("boson_name", &modeling::GaugeGroup::bosonName)
        .def_property_readonly("group", [](modeling::GaugeGroup const &self) {
            return std::const_pointer_cast<group::Group>(self.group);
        });

    py::class_<modeling::Particle>(mod, "Particle")
        .def_property_readonly("name", &modeling::Particle::name)
        .def("set_rep", &setRep, py::arg("group"), py::arg("value"))
        .def("__repr__", [](modeling::Particle const &self) {
            return "Particle(" + self.name() + ")";
        });

    py::class_<modeling::Model>(mod, "Model")
        .def(py::init([](std::vector<modeling::GaugeGroup> const &gauge) {
                 return modeling::Model(gauge);
             }),
             py::arg("gauge"))
        .def("weyl_fermion",
             &modeling::Model::weylFermion,
             py::arg("name"),
             py::arg("chirality") = "L")
        .def(
            "add_particle", &modeling::Model::addParticle, py::arg("particle"))
        .def("particle", &modeling::Model::particle, py::arg("name"))
        .def("__contains__", &modeling::Model::contains, py::arg("name"))
        .def_property_readonly("particles", &modeling::Model::particles)
        .def_property_readonly("feynman_rules",
                               [](modeling::Model const &self) {
                                   std::vector<std::string> printed;
                                   auto rules = self.feynmanRules();
                                   for (std::size_t i = 0; i < rules.size();
                                        ++i)
                                       printed.push_back(rules.str(i));
                                   return printed;
                               })
        .def("break_gauge_symmetry",
             &breakGaugeSymmetry,
             py::arg("group"),
             py::arg("renaming") = py::str("default"))
        .def("__repr__", [](modeling::Model const &self) {
            return "Model(" + self.describe() + ")";
        });

    mod.def("standard_model",
            &modeling::Model::standardModel,
            "The built-in Standard Model.");
    mod.def("qed", &modeling::Model::qed, "The built-in QED model.");
}
