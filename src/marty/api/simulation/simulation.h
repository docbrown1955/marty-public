/**
 * @file simulation.h
 * @brief Generation and compilation of a standalone MARTY library from
 * expressions, for the Python simulation module.
 *
 * The generated library exposes the C core (c_<name>_explicit functions).
 * Nothing here computes physics beyond the requested model operations: the
 * expressions are given by the caller, or come from a model's spectrum.
 */

#ifndef MTY_API_SIMULATION_SIMULATION_H_INCLUDED
#define MTY_API_SIMULATION_SIMULATION_H_INCLUDED

#include "../../../csl/api/expr.h"
#include "../../../csl/librarygenerator.h"
#include "../modeling/modeling.h"

#include <string>
#include <vector>

namespace mty::api::simulation {

class Generator {
  public:
    /// Library `name` is written in the directory `directory/name`.
    /// ValueError when the name is not a C identifier.
    Generator(std::string const &name, std::string const &directory);

    /// Directory of the library: the generated Makefile and sources.
    [[nodiscard]] std::string libraryDirectory() const;

    void addIncludePath(std::string const &path);
    void addLibraryPath(std::string const &path);

    /// Adds the function `name` = `expr`. ValueError when the name is not a C
    /// identifier or is already used.
    void addFunction(std::string const          &name,
                     csl::api::ExprHandle const &expr);

    /// Adds the spectrum of `model`: diagonalization data, and the mass
    /// functions and default parameters of its particles, as
    /// mty::Library::generateSpectrum does. Widths set by computeModelWidths
    /// are included when called before. StateError when already added.
    void addSpectrum(modeling::Model &model);

    /// Computes the widths of the particles of `model` at the given order.
    void computeModelWidths(modeling::Model &model, modeling::Order order);

    /// Quadruple precision inside the C calculations only. NotImplementedError
    /// when MARTY was built without quadruple precision.
    void setQuadruplePrecision(bool enabled);

    /// Writes all sources and the Makefile.
    void print();

    /// Writes the sources, then compiles the shared library with `make`,
    /// started with an argument vector (no shell). BuildError when make fails;
    /// its log is in the library directory.
    void build(unsigned int nJobs, std::string const &make);

  private:
    /// Adds a function, checking that its name is unused. Spectrum functions
    /// clear the LoopTools cache before they are evaluated.
    void addFunctionNamed(std::string const &name,
                          csl::Expr const   &expr,
                          bool               clearCache);

    csl::LibraryGenerator    m_generator;
    std::string              m_name;
    std::string              m_directory;
    std::vector<std::string> m_functions;
    bool                     m_hasSpectrum = false;
};

} // namespace mty::api::simulation

#endif
