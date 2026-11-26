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

/// Bindings of the library generator used by pymarty.simulation. The C++ side
/// (marty/api/simulation) validates the names and runs the compilation; the
/// Python layer only converts arguments and loads the result.

#include "marty/api/simulation/simulation.h"
#include "csl/api/expr.h"
#include "marty/api/modeling/modeling.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

using mty::api::simulation::Generator;

void bindSimulation(py::module_ &m)
{
    py::module_ mod = m.def_submodule("simulation",
                                      "Generation and compilation of "
                                      "standalone libraries.");

    py::class_<Generator>(mod, "Generator", R"doc(Generator of one C library.

Functions and the spectrum of a model are added, then the sources are written
and compiled in ``library_directory``. The library is ``lib<name>.so`` in
``<library_directory>/build/lib``.)doc")
        .def(py::init<std::string const &, std::string const &>(),
             py::arg("name"),
             py::arg("directory"))
        .def_property_readonly("library_directory",
                               &Generator::libraryDirectory)
        .def("add_include_path",
             &Generator::addIncludePath,
             py::arg("path"),
             "Adds a directory to the include path of the generated sources.")
        .def("add_library_path",
             &Generator::addLibraryPath,
             py::arg("path"),
             "Adds a directory to the library path used at link time.")
        .def("add_function",
             &Generator::addFunction,
             py::arg("name"),
             py::arg("expr"),
             "Adds the function `name` with the value `expr`.")
        .def("add_spectrum",
             &Generator::addSpectrum,
             py::arg("model"),
             "Adds the spectrum of a model: the mass functions and the "
             "default "
             "parameters of its particles. Call compute_model_widths first to "
             "include the widths. Can be called once.")
        .def("compute_model_widths",
             &Generator::computeModelWidths,
             py::arg("model"),
             py::arg("order"),
             "Computes the widths of the particles of the model at the given "
             "order, before add_spectrum.")
        .def("set_quadruple_precision",
             &Generator::setQuadruplePrecision,
             py::arg("enabled"),
             "Uses quadruple precision inside the C calculations only; the C "
             "interface and Python keep double precision. NotImplementedError "
             "when MARTY was built without quadruple precision.")
        .def("print_sources",
             &Generator::print,
             "Writes the sources and the Makefile, without compiling them.")
        .def("build",
             &Generator::build,
             py::arg("n_jobs"),
             py::arg("make"),
             "Writes the sources and compiles the shared library with "
             "`make`.");
}
