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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 1.3

 * \brief
 */
#ifndef CSL_LIBRARY_DEPENDENCY
#define CSL_LIBRARY_DEPENDENCY

#include "abstract.h"
#include <iostream>
#include <string>
#include <vector>

namespace csl {

class LibDependency {

  public:
    static LibDependency const &cmath()
    {
        static LibDependency cmath;
        static bool          created = false;
        if (!created) {
            cmath.addInclude("cmath", true);
            created = true;
        }
        return cmath;
    }

    static LibDependency const &complex()
    {
        static LibDependency cmath;
        static bool          created = false;
        if (!created) {
            cmath.addInclude("complex", true);
            created = true;
        }
        return cmath;
    }

    static LibDependency const &csl()
    {
        static LibDependency csl;
        static bool          created = false;
        if (!created) {
            csl.addInclude("csl", true);
            csl.addLib("-lcsl");
            created = true;
        }
        return csl;
    }

    static LibDependency const &marty()
    {
        static LibDependency marty;
        static bool          created = false;
        if (!created) {
            marty.addInclude("marty", true);
            marty.addLib("-lmarty");
            created = true;
        }
        return marty;
    }

    struct IncludeType {

        std::string name;
        bool        global;

        IncludeType(std::string const &t_name) : name(t_name), global(false)
        {
        }

        IncludeType(std::string const &t_name, bool t_global)
            : name(t_name), global(t_global)
        {
        }

        bool operator==(IncludeType const &other) const
        {
            return name == other.name;
        }
    };

    static std::vector<IncludeType> defaultInclude()
    {
        return {};
    }

    static std::vector<std::string> defaultLib()
    {
        return {};
    }

  public:
    std::vector<IncludeType> const &getInclude() const
    {
        return include;
    }
    std::vector<std::string> const &getLibrary() const
    {
        return library;
    }
    std::vector<Expr> const &getParams() const
    {
        return params;
    }

    std::vector<IncludeType> &getInclude()
    {
        return include;
    }
    std::vector<std::string> &getLibrary()
    {
        return library;
    }
    std::vector<Expr> &getParams()
    {
        return params;
    }

    void addInclude(std::string const &t_include, bool global = false)
    {
        IncludeType inc{t_include, global};
        addInclude(inc);
    }

    void addInclude(IncludeType const &t_include)
    {
        for (const auto &inc : include)
            if (inc == t_include)
                return;
        include.push_back(t_include);
    }

    void addLib(std::string const &t_lib)
    {
        for (const auto &lib : library)
            if (lib == t_lib)
                return;
        library.insert(library.begin(), t_lib);
    }

    void addParam(Expr const &expr)
    {
        for (const auto &p : params)
            if (expr == p)
                return;
        params.push_back(expr);
    }

    void removeInclude(std::string const &t_include)
    {
        for (auto iter = include.begin(); iter != include.end(); ++iter)
            if (*iter == t_include) {
                include.erase(iter);
                return;
            }
    }

    void removeLib(std::string const &t_lib)
    {
        for (auto iter = library.begin(); iter != library.end(); ++iter)
            if (*iter == t_lib) {
                library.erase(iter);
                return;
            }
    }

    void removeParam(Expr const &expr)
    {
        for (auto p = params.begin(); p != params.end(); ++p)
            if (expr == *p) {
                params.erase(p);
                return;
            }
    }

    void printInclude(std::ostream &out) const
    {
        for (const IncludeType &inc : include) {
            char separator[2];
            separator[0] = (inc.global) ? '<' : '\"';
            separator[1] = (inc.global) ? '>' : '\"';
            out << "#include " << separator[0] << inc.name << separator[1];
            out << '\n';
        }
    }

    void printLib(std::ostream &out, bool clang = false) const
    {
        for (const std::string &lib : library) {
            if (!clang or lib != "-lgfortran")
                out << lib << " ";
        }
    }

    bool containsInclude(std::string const &includeType) const
    {
        for (const auto &i : include)
            if (includeType == i.name)
                return true;
        return false;
    }

    bool containsLib(std::string const &libType) const
    {
        for (const auto &l : library)
            if (libType == l)
                return true;
        return false;
    }

    LibDependency &operator+=(LibDependency const &other)
    {
        for (const auto &inc : other.include)
            addInclude(inc);
        for (const auto &lib : other.library)
            addLib(lib);
        for (const auto &p : other.params)
            addParam(p);

        return *this;
    }

    LibDependency operator+(LibDependency const &other)
    {
        LibDependency newLib(*this);
        return newLib += other;
    }

  protected:
    std::vector<IncludeType> include = defaultInclude();

    std::vector<std::string> library = defaultLib();

    std::vector<Expr> params;
};
} // namespace csl

#endif // ifndef CSL_LIBRARY_DEPENDENCY
