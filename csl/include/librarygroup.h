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

 #pragma once

#include <string>
#include "libraryfunction.h"

namespace csl {

    class LibraryGroup {

    public:

        LibraryGroup(
                std::string t_name, 
                bool t_complexReturn
                )
            :name(t_name),
            paramName(
                    "param" 
                    + ((name.empty()) ? std::string("") : "_" + name) 
                    + "_t"),
            complexReturn(t_complexReturn)
        {}

        LibraryGroup(LibraryGroup const &) = default;
        LibraryGroup &operator=(LibraryGroup const &) = default;

        bool empty() const { return functions.empty(); }

        bool hasComplexReturn() const { return complexReturn; }

        std::string const &getName() const { return name; }

        std::string getParamName() const {
            return paramName;
        }

        void setParamName(std::string const &t_paramName) {
            paramName = t_paramName;
        }

        std::vector<LibParameter> const &getParameters() const {
            return parameters;
        }
        std::vector<LibParameter>       &getParameters()       {
            return parameters;
        }

        std::vector<LibFunction> const &getFunctions() const {
            return functions;
        }
        std::vector<LibFunction>       &getFunctions()       {
            return functions;
        }

        LibFunction &addFunction(LibFunction &&func)
        {
            functions.emplace_back(std::move(func));
            return functions.back();
        }

        std::vector<LibParameter> const &getForcedParameters() const {
            return forcedParameters;
        }

        void setForcedParameters(std::vector<LibParameter> const &t_params);

        static
        std::vector<LibParameter> gatherParameters(
                std::vector<std::shared_ptr<LibraryGroup>> &groups,
                std::string         const &paramName
                );

        void gatherParameters();

        static
        void printResetParameterList(
                std::string               const &nameContainer,
                std::vector<LibParameter> const &params,
                std::ostream &out,
                int nIndent
                );

        void printResetDefinition(
                std::ostream &out,
                int nIndent
                ) const;

        static
        void printPrintParameterList(
                std::string               const &nameContainer,
                std::vector<LibParameter> const &params,
                std::ostream &out,
                int nIndent
                );

        void printPrintDefinition(
                std::ostream &out,
                int nIndent
                ) const;

        void printNameMapElement(
                std::ostream       &out,
                int                 nIndent,
                LibParameter const &param,
                std::string  const &type
                ) const;

        void printNameMap(
                std::ostream &out,
                int           nIndent
                ) const;

        void printStructDefinition(
                std::ostream &out,
                int nIndent
                ) const;

        void printFunctionStack(
                std::ostream &out,
                int           nIndent
                ) const;

        void printParameterDefinition(
                std::ostream &out,
                bool          unusedParam
                ) const;

        void printParameterInitialization(
                std::ostream &out,
                int           nIndent
                ) const;

        void printForwardDefinitions(
                std::ostream &out,
                int           nIndent
                ) const;

        void print(
                std::ostream &out,
                bool          header
                ) const;

    private:

        std::string name;
        std::string paramName;
        bool        complexReturn;
        mutable int                       posTensorParam;
        mutable std::vector<LibParameter> parameters;
        mutable std::vector<LibParameter> forcedParameters;
        mutable std::vector<LibFunction>  functions;
    };

} // namespace csl
