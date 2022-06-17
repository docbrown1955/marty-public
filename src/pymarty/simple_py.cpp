#include "../marty/core/mrtInterface.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

namespace py = pybind11;

PYBIND11_MODULE(pymarty, m)
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
}
