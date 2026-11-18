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

#include "csl/api/error.h"
#include "marty/api/common/config.h"
#include "marty/api/common/error.h"
#include "marty/api/common/session.h"

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

namespace py = pybind11;

// Defined in symbolic.cpp.
void bindSymbolic(py::module_ &m);

namespace {

// Python classes of the error hierarchy. They are owned by the module
// attributes, so non-owning handles are enough here.
py::handle g_martyError;
py::handle g_valueError;
py::handle g_typeError;
py::handle g_lookupError;
py::handle g_notImplementedError;
py::handle g_stateError;
py::handle g_buildError;
py::handle g_zeroDivisionError;

// Creates `pymarty.errors.<name>` deriving from `base` and, when given, from
// the builtin exception `builtin`, so that `except ValueError` also catches
// it.
py::object defineError(py::module_ &m,
                       char const  *name,
                       py::handle   base,
                       char const  *doc,
                       PyObject    *builtin = nullptr)
{
    py::tuple bases(builtin ? 2 : 1);
    bases[0] = base;
    if (builtin) {
        bases[1] = py::reinterpret_borrow<py::object>(builtin);
    }
    std::string const qualified = std::string("pymarty.errors.") + name;
    py::dict          attributes;
    attributes["__doc__"] = doc;
    PyObject *cls
        = PyErr_NewException(qualified.c_str(), bases.ptr(), attributes.ptr());
    if (!cls) {
        throw py::error_already_set();
    }
    py::object obj = py::reinterpret_steal<py::object>(cls);
    m.attr(name)   = obj;
    return obj;
}

// Maps the errors of the core and of the API layer onto the Python hierarchy.
// Core forms (mty::error::Exception, the bare mty::error::Type and CSLError
// enums, GammaAPIError) are first converted with rethrowAsApiError().
// Anything else keeps the default translation of pybind11 (the translator
// rethrows it, which makes pybind11 try the next translator).
void translateException(std::exception_ptr p)
{
    try {
        try {
            std::rethrow_exception(p);
        }
        catch (mty::error::Exception const &) {
            mty::api::rethrowAsApiError();
        }
        catch (mty::error::Type const &) {
            mty::api::rethrowAsApiError();
        }
        catch (CSLError const &) {
            mty::api::rethrowAsApiError();
        }
        catch (GammaAPIError const &) {
            mty::api::rethrowAsApiError();
        }
    }
    catch (mty::api::BuildError const &e) {
        PyErr_SetString(g_buildError.ptr(), e.what());
    }
    catch (mty::api::StateError const &e) {
        PyErr_SetString(g_stateError.ptr(), e.what());
    }
    catch (mty::api::NotImplementedError const &e) {
        PyErr_SetString(g_notImplementedError.ptr(), e.what());
    }
    catch (mty::api::LookupError const &e) {
        PyErr_SetString(g_lookupError.ptr(), e.what());
    }
    catch (mty::api::TypeError const &e) {
        PyErr_SetString(g_typeError.ptr(), e.what());
    }
    catch (mty::api::ValueError const &e) {
        PyErr_SetString(g_valueError.ptr(), e.what());
    }
    catch (csl::api::ZeroDivisionError const &e) {
        PyErr_SetString(g_zeroDivisionError.ptr(), e.what());
    }
    catch (mty::api::Error const &e) {
        PyErr_SetString(g_martyError.ptr(), e.what());
    }
}

// Flags of the global state, for the `clean_state` fixture of the tests.
py::dict stateSnapshot()
{
    py::dict state;
#define PYMARTY_SNAPSHOT(ns, name) state[#ns "::" #name] = ns::name;
    MTY_MARTY_OPTION_FLAGS(PYMARTY_SNAPSHOT)
    MTY_CSL_OPTION_FLAGS(PYMARTY_SNAPSHOT)
#undef PYMARTY_SNAPSHOT
    state["Model::current"]
        = reinterpret_cast<std::uintptr_t>(mty::Model::current);
    return state;
}

void restoreState(py::dict const &state)
{
#define PYMARTY_RESTORE(ns, name) \
    ns::name = state[#ns "::" #name].cast<bool>();
    MTY_MARTY_OPTION_FLAGS(PYMARTY_RESTORE)
    MTY_CSL_OPTION_FLAGS(PYMARTY_RESTORE)
#undef PYMARTY_RESTORE
    mty::Model::current = reinterpret_cast<mty::Model *>(
        state["Model::current"].cast<std::uintptr_t>());
}

// Throws the error `kind` the way the core would. Core forms are thrown inside
// a Session so that nothing is printed, like an API call would do.
void raiseError(std::string const &kind, std::string const &message)
{
    if (kind == "value") {
        throw mty::api::ValueError(message);
    }
    if (kind == "type") {
        throw mty::api::TypeError(message);
    }
    if (kind == "lookup") {
        throw mty::api::LookupError(message);
    }
    if (kind == "not_implemented") {
        throw mty::api::NotImplementedError(message);
    }
    if (kind == "state") {
        throw mty::api::StateError(message);
    }
    if (kind == "build") {
        throw mty::api::BuildError(message);
    }
    mty::api::Session quiet;
    if (kind == "core_value") {
        CallHEPError(mty::error::ValueError, message);
    }
    if (kind == "core_key") {
        CallHEPError(mty::error::KeyError, message);
    }
    if (kind == "core_type") {
        CallHEPError(mty::error::TypeError, message);
    }
    if (kind == "core_not_implemented") {
        CallHEPError(mty::error::NotImplementedError, message);
    }
    if (kind == "core_model_building") {
        CallHEPError(mty::error::ModelBuildingError, message);
    }
    if (kind == "core_other") {
        CallHEPError(mty::error::RuntimeError, message);
    }
    if (kind == "csl_type") {
        CALL_SMERROR_SPEC(CSLError::TypeError, message);
    }
    if (kind == "csl_value") {
        CALL_SMERROR_SPEC(CSLError::ValueError, message);
    }
    if (kind == "gamma") {
        throw GammaAPIError(message);
    }
    throw mty::api::ValueError("unknown error kind: " + kind);
}

} // namespace

PYBIND11_MODULE(_pymarty, m)
{
    m.doc()               = "Compiled core of the pymarty package.";
    m.attr("__version__") = PYMARTY_VERSION;

    py::object martyError          = defineError(m,
                                        "MartyError",
                                        PyExc_Exception,
                                        "Base class of the errors raised by "
                                                 "pymarty.");
    g_martyError                   = martyError;
    py::object valueError          = defineError(m,
                                        "MartyValueError",
                                        martyError,
                                        "Invalid value passed to pymarty. "
                                                 "Also a ValueError.",
                                        PyExc_ValueError);
    g_valueError                   = valueError;
    py::object typeError           = defineError(m,
                                       "MartyTypeError",
                                       martyError,
                                       "Argument or index of the wrong type. "
                                                 "Also a TypeError.",
                                       PyExc_TypeError);
    g_typeError                    = typeError;
    py::object lookupError         = defineError(m,
                                         "MartyLookupError",
                                         martyError,
                                         "Unknown name or key. Also a "
                                                 "LookupError.",
                                         PyExc_LookupError);
    g_lookupError                  = lookupError;
    py::object notImplementedError = defineError(m,
                                                 "MartyNotImplementedError",
                                                 martyError,
                                                 "Feature not implemented. "
                                                 "Also a NotImplementedError.",
                                                 PyExc_NotImplementedError);
    g_notImplementedError          = notImplementedError;
    py::object stateError          = defineError(m,
                                        "MartyStateError",
                                        martyError,
                                        "Library in an unexpected state. Also "
                                                 "a RuntimeError.",
                                        PyExc_RuntimeError);
    g_stateError                   = stateError;
    py::object buildError          = defineError(m,
                                        "MartyBuildError",
                                        martyError,
                                        "Build input, such as a compiler, a "
                                                 "path or a program, cannot be found "
                                                 "or run. Also a RuntimeError.",
                                        PyExc_RuntimeError);
    g_buildError                   = buildError;
    py::object zeroDivisionError   = defineError(m,
                                               "MartyZeroDivisionError",
                                               martyError,
                                               "Division by zero that the "
                                                 "library refuses, such as 0 ** "
                                                 "-1. Also a "
                                                 "ZeroDivisionError.",
                                               PyExc_ZeroDivisionError);
    g_zeroDivisionError            = zeroDivisionError;

    py::register_exception_translator(translateException);

    bindSymbolic(m);

    // Test hooks for the error and output layers. Not part of the public API.
    py::module_ debug
        = m.def_submodule("_debug", "Test hooks, not part of the public API.");
    debug.def("raise_error", &raiseError, py::arg("kind"), py::arg("message"));
    debug.def("state_snapshot", &stateSnapshot);
    debug.def("restore_state", &restoreState, py::arg("state"));
    debug.def("leak_state", [] {
        // Changes a global flag without a Session, like a buggy API function.
        mty::option::amputateExternalLegs = !mty::option::amputateExternalLegs;
    });
    debug.def(
        "print_cout",
        [](std::string const &text) {
            py::scoped_ostream_redirect redirect;
            std::cout << text << '\n' << std::flush;
        },
        py::arg("text"));

    py::class_<mty::api::Config>(
        m,
        "Config",
        R"doc(Paths and programs used to build and run generated code.

An empty argument is discovered at run time. The order is: the argument, then
the environment (MARTY_INCLUDE_PATH, MARTY_LIBRARY_PATH, MARTY_CXX then CXX,
MARTY_GRAFED), then the package data, then the system locations. Programs
are looked up in PATH. Use pymarty.config.resolve() to build one from the
current environment.)doc")
        .def(
            py::init([](std::string const &includePath,
                        std::string const &libraryPath,
                        std::string const &cxx,
                        std::string const &make,
                        std::string const &grafed,
                        std::string const &packageData) {
                mty::api::ConfigArguments args;
                args.includePath = includePath;
                args.libraryPath = libraryPath;
                args.cxx         = cxx;
                args.make        = make;
                args.grafed      = grafed;
                args.packageData = packageData;
                return mty::api::Config::resolve(args);
            }),
            py::arg("include_path") = "",
            py::arg("library_path") = "",
            py::arg("cxx")          = "",
            py::arg("make")         = "",
            py::arg("grafed")       = "",
            py::arg("package_data") = "",
            R"doc(Config(include_path='', library_path='', cxx='', make='', grafed='', package_data='')

Each argument is an explicit path or program name. An empty string means
discovery at run time, as described in the class.)doc")
        .def_property_readonly("include_path",
                               &mty::api::Config::includePath,
                               "Directory of the MARTY headers.")
        .def_property_readonly("library_path",
                               &mty::api::Config::libraryPath,
                               "Directory of the MARTY libraries.")
        .def_property_readonly("cxx",
                               &mty::api::Config::cxx,
                               "C++ compiler used to build generated code.")
        .def_property_readonly("make",
                               &mty::api::Config::make,
                               "Make program used to build generated code.")
        .def_property_readonly("grafed",
                               &mty::api::Config::grafed,
                               "GRAFED program to display diagrams.");
}
