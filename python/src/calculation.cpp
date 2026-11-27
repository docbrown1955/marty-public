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

/// Bindings of the tree-level calculation: Feynman rules, amplitudes, squared
/// amplitudes and global options. The Python layer (pymarty.calculation)
/// builds the legs and filters and checks the model state.

#include "marty/api/calculation/calculation.h"
#include "csl/api/expr.h"
#include "marty/api/common/error.h"
#include "marty/api/modeling/modeling.h"
#include "marty/api/visualization/visualization.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <vector>

namespace py = pybind11;

namespace {

using csl::api::ExprHandle;
using mty::api::modeling::Amplitude;
using mty::api::modeling::Chirality;
using mty::api::modeling::FeynmanRules;
using mty::api::modeling::Leg;
using mty::api::modeling::Model;
using mty::api::modeling::Order;
using mty::api::modeling::Particle;

/// A particle of the model, given as a name or as a modeling.Particle.
Particle resolveParticle(Model &model, py::handle value)
{
    if (py::isinstance<Particle>(value))
        return value.cast<Particle>();
    return model.particle(value.cast<std::string>());
}

Chirality parseChirality(std::string const &name)
{
    if (name == "dirac")
        return Chirality::Dirac;
    if (name == "left")
        return Chirality::Left;
    if (name == "right")
        return Chirality::Right;
    throw mty::api::ValueError("Unknown chirality '" + name + "'.");
}

/// Legs are (particle, incoming, antiparticle, chirality) tuples.
mty::api::modeling::AmplitudeOptions
amplitudeOptions(Model                         &model,
                 std::string const             &topology,
                 std::vector<py::object> const &force,
                 std::vector<py::object> const &forbid,
                 std::vector<py::object> const &disable,
                 std::vector<int> const        &fermionOrder)
{
    mty::api::modeling::AmplitudeOptions options;
    options.topology = topology;
    for (auto const &item : force)
        options.forceParticles.push_back(resolveParticle(model, item));
    for (auto const &item : forbid)
        options.forbidParticles.push_back(resolveParticle(model, item));
    for (auto const &item : disable)
        options.disableParticles.push_back(resolveParticle(model, item));
    options.fermionOrder = fermionOrder;
    return options;
}

Amplitude computeAmplitude(Model                          &model,
                           Order                           order,
                           std::vector<py::tuple> const   &legs,
                           std::vector<std::size_t> const &momentumIndices,
                           std::string const              &topology,
                           std::vector<py::object> const  &force,
                           std::vector<py::object> const  &forbid,
                           std::vector<py::object> const  &disable,
                           std::vector<int> const         &fermionOrder)
{
    std::vector<Leg> externals;
    for (auto const &leg : legs) {
        externals.push_back(Leg{resolveParticle(model, leg[0]),
                                leg[1].cast<bool>(),
                                leg[2].cast<bool>(),
                                parseChirality(leg[3].cast<std::string>())});
    }
    auto options = amplitudeOptions(
        model, topology, force, forbid, disable, fermionOrder);
    return model.computeAmplitude(order, externals, momentumIndices, options);
}

} // namespace

void bindCalculation(py::module_ &m)
{
    py::module_ mod = m.def_submodule("calculation",
                                      "Tree-level calculation on a finalized "
                                      "model.");

    py::enum_<Order>(mod, "Order", "Order of a perturbative calculation.")
        .value("TREE", Order::TreeLevel)
        .value("ONE_LOOP", Order::OneLoop);

    py::class_<FeynmanRules>(
        mod, "FeynmanRules", "Read-only view of the Feynman rules of a model.")
        .def("__len__", &FeynmanRules::size)
        .def("str",
             &FeynmanRules::str,
             py::arg("index"),
             "Printed form of the rule at index.")
        .def("latex",
             &FeynmanRules::latex,
             py::arg("index"),
             "LaTeX block of the rule at index.")
        .def("latex_document",
             &FeynmanRules::latexDocument,
             "LaTeX document with one block per rule.")
        .def("vertex_particles",
             &FeynmanRules::vertexParticles,
             py::arg("index"),
             "Names of the fields of the vertex at index, in product order.")
        .def(
            "vertex_expression",
            [](FeynmanRules const &self, std::size_t index) {
                return ExprHandle(self.vertexExpression(index));
            },
            py::arg("index"),
            "Expression of the vertex at index.");

    py::class_<Amplitude>(mod, "Amplitude", "Amplitude of external legs.")
        .def("diagram_count",
             &Amplitude::diagramCount,
             "Number of Feynman diagrams.")
        .def("display",
             &mty::api::calculation::displayAmplitude,
             "Printed form of every diagram expression.")
        .def("diagram_particles",
             &mty::api::calculation::diagramParticles,
             py::arg("index"),
             "Names of the particles of the diagram at index.")
        .def("diagram_mediators",
             &mty::api::calculation::diagramMediators,
             py::arg("index"),
             "Names of the mediator particles of the diagram at index.")
        .def("select",
             &Amplitude::select,
             py::arg("indices"),
             "Copy of the amplitude keeping only the diagrams at indices.")
        .def(
            "diagram_expression",
            [](Amplitude const &self, std::size_t index) {
                return ExprHandle(
                    mty::api::calculation::diagramExpression(self, index));
            },
            py::arg("index"),
            "Expression of the diagram at index.");

    mod.def(
        "compute_feynman_rules",
        [](Model const &model) { return model.feynmanRules(); },
        py::arg("model"),
        "Feynman rules of the model.");

    mod.def("compute_amplitude",
            &computeAmplitude,
            py::arg("model"),
            py::arg("order"),
            py::arg("legs"),
            py::arg("kinematics")    = std::vector<std::size_t>{},
            py::arg("topology")      = "",
            py::arg("force")         = std::vector<py::object>{},
            py::arg("forbid")        = std::vector<py::object>{},
            py::arg("disable")       = std::vector<py::object>{},
            py::arg("fermion_order") = std::vector<int>{},
            "Amplitude of the legs at the given order, with the options.");

    mod.def(
        "compute_width",
        [](Model                         &model,
           Order                          order,
           py::object                     particle,
           std::string const             &topology,
           std::vector<py::object> const &force,
           std::vector<py::object> const &forbid,
           std::vector<py::object> const &disable,
           std::vector<int> const        &fermionOrder) {
            auto options = amplitudeOptions(
                model, topology, force, forbid, disable, fermionOrder);
            return ExprHandle(model.computeWidth(
                order, resolveParticle(model, particle), options));
        },
        py::arg("model"),
        py::arg("order"),
        py::arg("particle"),
        py::arg("topology")      = "",
        py::arg("force")         = std::vector<py::object>{},
        py::arg("forbid")        = std::vector<py::object>{},
        py::arg("disable")       = std::vector<py::object>{},
        py::arg("fermion_order") = std::vector<int>{},
        "Total decay width of the particle, summed over its two-body decays.");
    mod.def(
        "compute_squared_amplitude",
        [](Model &model, Amplitude const &amplitude) {
            return ExprHandle(model.computeSquaredAmplitude(amplitude));
        },
        py::arg("model"),
        py::arg("amplitude"),
        "Squared amplitude, summed over spins and polarizations.");
    mod.def(
        "compute_squared_amplitude",
        [](Model &model, Amplitude const &left, Amplitude const &right) {
            return ExprHandle(model.computeSquaredAmplitude(left, right));
        },
        py::arg("model"),
        py::arg("left"),
        py::arg("right"),
        "Interference term of two amplitudes, summed over spins and "
        "polarizations.");

    py::class_<mty::api::calculation::WilsonSet>(
        mod, "WilsonSet", "Wilson coefficients of an amplitude.")
        .def(
            "coefficient",
            [](mty::api::calculation::WilsonSet const &self,
               std::vector<ExprHandle> const          &operators) {
                std::vector<csl::Expr> raw;
                for (auto const &op : operators)
                    raw.push_back(op.raw());
                return ExprHandle(self.coefficient(raw));
            },
            py::arg("operators"),
            "Coefficient of the sum of the given operator expressions.");

    mod.def(
        "wilson_coefficients",
        [](Model const      &model,
           Amplitude const  &amplitude,
           ExprHandle const &operatorCoefficient,
           bool              dedicatedMatching) {
            return mty::api::calculation::wilsonCoefficients(
                model,
                amplitude,
                operatorCoefficient.raw(),
                dedicatedMatching);
        },
        py::arg("model"),
        py::arg("amplitude"),
        py::arg("operator_coefficient"),
        py::arg("dedicated_matching") = false,
        "Wilson coefficients of an amplitude, with the given overall factor "
        "of every operator.");
    mod.def(
        "chromo_magnetic_operator",
        [](Model const                            &model,
           mty::api::calculation::WilsonSet const &wilsons,
           std::string const                      &coupling) {
            std::vector<ExprHandle> out;
            for (auto const &op :
                 mty::api::calculation::chromoMagneticOperator(
                     model, wilsons, coupling))
                out.emplace_back(op);
            return out;
        },
        py::arg("model"),
        py::arg("wilsons"),
        py::arg("coupling"),
        "Operators of the chromomagnetic dipole with the given Dirac coupling "
        "(S, P, L, R, V, A, VL, VR, T, TA, TL or TR).");
    mod.def(
        "chromo_magnetic_coefficient",
        [](Model const                            &model,
           mty::api::calculation::WilsonSet const &wilsons,
           std::string const                      &coupling) {
            return ExprHandle(mty::api::calculation::chromoMagneticCoefficient(
                model, wilsons, coupling));
        },
        py::arg("model"),
        py::arg("wilsons"),
        py::arg("coupling"),
        "Coefficient of the chromomagnetic dipole with the given Dirac coupling, "
        "matched against the Wilson coefficients in one step.");
    mod.def(
        "dimension6_coefficient",
        [](Model const                            &model,
           mty::api::calculation::WilsonSet const &wilsons,
           std::string const                      &left,
           std::string const                      &right,
           std::string const                      &colorGroup,
           std::string const                      &colorCoupling) {
            return ExprHandle(mty::api::calculation::dimension6Coefficient(
                model, wilsons, left, right, colorGroup, colorCoupling));
        },
        py::arg("model"),
        py::arg("wilsons"),
        py::arg("left"),
        py::arg("right"),
        py::arg("color_group")    = "",
        py::arg("color_coupling") = "Id",
        "Coefficient of the dimension-6 operator with the given left and right "
        "Dirac couplings, matched against the Wilson coefficients in one step. "
        "An empty color_group means no color structure.");

    mod.def("option_names",
            &mty::api::calculation::optionNames,
            "Names of the boolean global options, in snake_case.");
    mod.def("get_option",
            &mty::api::calculation::getOption,
            py::arg("name"),
            "Value of a global option.");
    mod.def("set_option",
            &mty::api::calculation::setOption,
            py::arg("name"),
            py::arg("value"),
            "Sets a global option.");

    py::module_ vis = mod.def_submodule("visualization",
                                        "Export and viewing of the diagrams "
                                        "of an amplitude.");
    vis.def("diagrams_json",
            &mty::api::visualization::diagramsJson,
            py::arg("amplitude"),
            "Diagram file of the amplitude, in the format read by GRAFED.");
    vis.def("save_diagrams",
            &mty::api::visualization::saveDiagrams,
            py::arg("amplitude"),
            py::arg("path"),
            "Writes the diagram file of the amplitude, readable by the owner "
            "only.");
    vis.def("diagrams_latex",
            &mty::api::visualization::diagramsLatex,
            py::arg("amplitude"),
            "Standalone LaTeX document drawing every diagram.");
    vis.def("rules_latex",
            &mty::api::visualization::rulesLatex,
            py::arg("rules"),
            "Standalone LaTeX document with one block per Feynman rule.");
    vis.def(
        "render_png",
        [](std::string const &document) {
            std::string png = mty::api::visualization::renderPng(document);
            return py::bytes(png);
        },
        py::arg("latex_document"),
        "PNG bytes of a standalone LaTeX document.");
    vis.def("find_grafed",
            &mty::api::visualization::findGrafed,
            py::arg("explicit_path") = "",
            "Path of the GRAFED executable, or an empty string.");
    vis.def("launch_grafed",
            &mty::api::visualization::launchGrafed,
            py::arg("executable"),
            py::arg("json_path"),
            "Starts GRAFED on a diagram file without waiting; returns the "
            "pid.");
    vis.def("wait_process",
            &mty::api::visualization::waitProcess,
            py::arg("pid"),
            "Waits for a process; returns its exit status.");
}
