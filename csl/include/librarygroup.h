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

        void addFunction(LibFunction const &func)
        {
            functions.push_back(func);
        }

        static
        std::vector<LibParameter> gatherParameters(
                std::vector<std::shared_ptr<LibraryGroup>> &groups,
                std::string         const &paramName
                );

        void gatherParameters();

        void printStructDefinition(
                std::ostream &out,
                int nIndent
                ) const;

        void printFunctionStack(
                std::ostream &out,
                int           nIndent
                ) const;

        void printParameterDefinition(std::ostream &out) const;

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
        mutable std::vector<LibFunction>  functions;
    };

} // namespace csl
