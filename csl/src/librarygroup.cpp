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

 #include "librarygroup.h"
#include "librarygenerator.h"

namespace csl {

    std::vector<LibParameter> LibraryGroup::gatherParameters(
            std::vector<std::shared_ptr<LibraryGroup>> &groups,
            std::string         const &paramName
            ) 
    {
        if (groups.empty())
            return {};
        groups[0]->parameters.clear();
        std::vector<LibParameter> tensorParams;
        for (const auto &g : groups) {
            for (const auto &f : g->getFunctions()) {
                auto const &interm = f.getParameters();
                int pos = f.getPosTensorParameter();
                if (pos == -1)
                    pos = interm.size();
                groups[0]->parameters.insert(
                        groups[0]->parameters.end(),
                        interm.begin(),
                        interm.begin() + pos
                        );
                tensorParams.insert(
                        tensorParams.end(),
                        interm.begin() + pos,
                        interm.end()
                        );
            }
        }
        groups[0]->posTensorParam 
            = (tensorParams.empty()) ? -1 : groups[0]->parameters.size();
        groups[0]->parameters.insert(
                groups[0]->parameters.end(),
                tensorParams.begin(),
                tensorParams.end()
                );
        LibFunction::cutParameters(groups[0]->parameters, groups[0]->posTensorParam);
        LibFunction::sortParameters(groups[0]->parameters, groups[0]->posTensorParam);
        for (auto &g : groups) {
            g->parameters = groups[0]->parameters;
            g->posTensorParam = groups[0]->posTensorParam;
            g->paramName = paramName;
        }
        return groups[0]->parameters;
    }

    void LibraryGroup::gatherParameters()
    {
        std::vector<std::shared_ptr<LibraryGroup>>
            v{{std::make_shared<LibraryGroup>(*this)}};
        gatherParameters(v, paramName);
        *this = *v[0];
    }

    void LibraryGroup::setForcedParameters(
            std::vector<LibParameter> const &t_params)
    {
        forcedParameters = t_params;
        int tp = -1;
        LibFunction::cutParameters(forcedParameters, tp);
        tp = -1;
        LibFunction::sortParameters(forcedParameters, tp);
        for (const auto &fp : forcedParameters) {
            for (size_t i = 0; i != parameters.size(); ++i) {
                if (parameters[i].name == fp.name) {
                    parameters.erase(parameters.begin() + i);
                    if (posTensorParam >= static_cast<int>(i))
                        --posTensorParam;
                    break;
                }
            }
        }
    }

    static
    std::pair<std::vector<LibParameter>, std::vector<LibParameter>>
        separateComplexParameters(
                std::vector<LibParameter> const &params1,
                std::vector<LibParameter> const &params2
                )
    {
        std::pair<std::vector<LibParameter>, std::vector<LibParameter>> res;
        res.first.reserve(params1.size()+params2.size());  // real params
        res.second.reserve(params1.size()+params2.size()); // complex params
        for (const auto &par : params1) {
            if (par.type == LibraryGenerator::realUsing)
                res.first.push_back(par);
            else
                res.second.push_back(par);
        }
        for (const auto &par : params2) {
            if (par.type == LibraryGenerator::realUsing)
                res.first.push_back(par);
            else
                res.second.push_back(par);
        }
        return res;
    }

    void LibraryGroup::printResetParameterList(
            std::string               const &nameContainer,
            std::vector<LibParameter> const &params,
            std::ostream &out,
            int           nIndent
            )
    {
        constexpr size_t limitParams = 5;
        const auto newParamLine = '\n' + LibraryGenerator::indent(nIndent + 2);
        out << LibraryGenerator::indent(nIndent) << "for (auto &par : "
            << nameContainer << "{";
        size_t count = 0;
        if (params.size() > limitParams) {
            out << newParamLine;
        }
        for (const auto &par : params) {
            if (++count > limitParams) {
                count = 0;
                out << newParamLine;
            }
            out << '&' << par.name << ", ";
        }
        out << "})\n";
        out << LibraryGenerator::indent(nIndent) << "{\n";
        out << LibraryGenerator::indent(nIndent + 1) << "par->reset();\n";
        out << LibraryGenerator::indent(nIndent) << "}\n";
    }

    void LibraryGroup::printResetDefinition(
            std::ostream &out,
            int           nIndent
            ) const
    {
        auto const &[realParams, complexParams] = separateComplexParameters(
                parameters, forcedParameters
                );
        out << LibraryGenerator::indent(nIndent) << "void reset()\n";
        out << LibraryGenerator::indent(nIndent) << "{\n";
        out << LibraryGenerator::indent(nIndent+1) 
            << "using real_params = std::array<csl::InitSanitizer<"
            << LibraryGenerator::realUsing << ">*, " 
            << realParams.size() << ">;\n";
        out << LibraryGenerator::indent(nIndent+1) 
            << "using complex_params = std::array<csl::InitSanitizer<"
            << LibraryGenerator::complexUsing << ">*, " 
            << complexParams.size() << ">;\n";
        out << '\n';
        printResetParameterList("real_params", realParams, out, nIndent+1);
        out << '\n';
        printResetParameterList("complex_params", complexParams, out, nIndent+1);
        out << LibraryGenerator::indent(nIndent) << "}\n";
    }

    void LibraryGroup::printPrintParameterList(
            std::string               const &nameContainer,
            std::vector<LibParameter> const &params,
            std::ostream &out,
            int           nIndent
            )
    {
        constexpr size_t limitParams = 5;
        const auto newParamLine = '\n' + LibraryGenerator::indent(nIndent + 2);
        out << LibraryGenerator::indent(nIndent) << "for (auto const &par : "
            << nameContainer << "{";
        size_t count = 0;
        if (params.size() > limitParams) {
            out << newParamLine;
        }
        for (const auto &par : params) {
            if (++count > limitParams) {
                count = 0;
                out << newParamLine;
            }
            out << '&' << par.name << ", ";
        }
        out << "})\n";
        out << LibraryGenerator::indent(nIndent) << "{\n";
        out << LibraryGenerator::indent(nIndent + 1) << "out << \"  -> \";\n";
        out << LibraryGenerator::indent(nIndent + 1) << "par->print(out);\n";
        out << LibraryGenerator::indent(nIndent) << "}\n";
    }

    void LibraryGroup::printPrintDefinition(
            std::ostream &out,
            int           nIndent
            ) const
    {
        auto const &[realParams, complexParams] = separateComplexParameters(
                parameters, forcedParameters
                );
        out << LibraryGenerator::indent(nIndent) 
            << "void print(std::ostream &out = std::cout)\n";
        out << LibraryGenerator::indent(nIndent) << "{\n";
        out << LibraryGenerator::indent(nIndent+1) 
            << "using real_params = std::array<csl::InitSanitizer<"
            << LibraryGenerator::realUsing << "> const*, " 
            << realParams.size() << ">;\n";
        out << LibraryGenerator::indent(nIndent+1) 
            << "using complex_params = std::array<csl::InitSanitizer<"
            << LibraryGenerator::complexUsing << "> const*, " 
            << complexParams.size() << ">;\n";
        out << '\n';
        out << LibraryGenerator::indent(2) << "out << \"param_t struct:\\n\";\n";
        out << LibraryGenerator::indent(2) << "out << \"Real parameters\\n\";\n";
        printPrintParameterList("real_params", realParams, out, nIndent+1);
        out << '\n';
        out << LibraryGenerator::indent(2) << "out << \"Complex parameters\\n\";\n";
        printPrintParameterList("complex_params", complexParams, out, nIndent+1);
        out << LibraryGenerator::indent(2) << "out << \"\\n\";\n";
        out << LibraryGenerator::indent(nIndent) << "}\n";
    }

    void LibraryGroup::printStructDefinition(
            std::ostream &out,
            int           nIndent
            ) const
    {
        out << LibraryGenerator::indent(nIndent);
        out << "struct " << getParamName();
        out << " {\n\n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "///////////////////////////////////////\n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "// Elementary parameters to be defined \n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "///////////////////////////////////////\n\n";
        for (const auto &par : parameters) {
            out << LibraryGenerator::indent(nIndent + 1);
            out << "csl::InitSanitizer<" << par.type << "> " 
                << par.name << " { \"" << par.name 
                << "\" };\n";
        }
        out << "\n\n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "///////////////////////////////////////\n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "// Parameters functions of others  \n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "// through diagonalization or mass \n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "// expressions, see updateSpectrum()  \n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "// in global.h or set them by hand  \n";
        out << LibraryGenerator::indent(nIndent + 1)
            << "///////////////////////////////////////\n\n";
        for (const auto &par : forcedParameters) {
            out << LibraryGenerator::indent(nIndent + 1);
            out << "csl::InitSanitizer<" << par.type << "> " 
                << par.name << " { \"" << par.name 
                << "\" };\n";
        }
        out << '\n';
        printResetDefinition(out, nIndent + 1);
        out << '\n';
        printPrintDefinition(out, nIndent + 1);
        out << '\n';
        printNameMap(out, nIndent + 1);
        out << LibraryGenerator::indent(nIndent) << "};\n";
    }

    void LibraryGroup::printNameMapElement(
            std::ostream       &out,
            int                 nIndent,
            LibParameter const &param,
            std::string  const &type
            ) const
    {
        if (param.type == type) {
            out << LibraryGenerator::indent(1+nIndent) 
                << "{\"" << param.name << "\", &" << param.name << "},\n";
        }
    }

    void LibraryGroup::printNameMap(
            std::ostream &out,
            int           nIndent
            ) const
    {
        out << LibraryGenerator::indent(nIndent)
            << "std::map<std::string, csl::InitSanitizer<"
            << LibraryGenerator::realUsing << ">*> realParams {\n";
        for (const auto &param : parameters) {
            printNameMapElement(
                    out, nIndent, param, LibraryGenerator::realUsing);
        }
        for (const auto &param : forcedParameters) {
            printNameMapElement(
                    out, nIndent, param, LibraryGenerator::realUsing);
        }
        out << "};\n\n";
        out << LibraryGenerator::indent(nIndent)
            << "std::map<std::string, csl::InitSanitizer<" 
            << LibraryGenerator::complexUsing << ">*> complexParams {\n";
        for (const auto &param : parameters) {
            printNameMapElement(
                    out, nIndent, param, LibraryGenerator::complexUsing);
        }
        for (const auto &param : forcedParameters) {
            printNameMapElement(
                    out, nIndent, param, LibraryGenerator::complexUsing);
        }
        out << LibraryGenerator::indent(nIndent) << "};\n\n";
    }

    void LibraryGroup::printFunctionStack(
            std::ostream &out,
            int           nIndent
            ) const
    {
        out << LibraryGenerator::indent(nIndent)
            << "inline std::array<Callable<" << (complexReturn ? 
                    LibraryGenerator::complexUsing 
                    :LibraryGenerator::realUsing) << ", " << getParamName();
        out << ">, " << functions.size() << "> f_" << name << " = {\n";
        for (const auto &f : functions) {
            out << LibraryGenerator::indent(nIndent + 1);
            out << "Callable{\"" << f.getName() << "\", " << f.getName() << "},\n";
        }
        out << LibraryGenerator::indent(nIndent) << "};\n\n";

        out << LibraryGenerator::indent(nIndent)
            << "inline std::map<std::string, Callable<" << (complexReturn ? 
                    LibraryGenerator::complexUsing 
                    :LibraryGenerator::realUsing) << ", " << getParamName();
        out << ">> fmap_" << name << " {\n";
        for (size_t i = 0; i != functions.size(); ++i) {
            out << LibraryGenerator::indent(nIndent + 1);
            out << "{\"" << functions[i].getName() 
                << "\", f_" << name << "[" << i << "]},\n";
        }
        out << LibraryGenerator::indent(nIndent) << "};\n\n";
    }

    void LibraryGroup::printParameterDefinition(
            std::ostream &out,
            bool unusedParam
            ) const
    {
        out << getParamName() << " const &";
        if (!unusedParam)
            out << "param";
    }

    void LibraryGroup::printParameterInitialization(
            std::ostream &out,
            int           nIndent
            ) const
    {
        for (size_t i = 0; i != parameters.size(); ++i) {
            auto const &nameParam = parameters[i].name;
            out << LibraryGenerator::indent(nIndent);
            out << "auto const &";
            out << nameParam << " = param." << nameParam << ';' << '\n';
        }
    }

    void LibraryGroup::printForwardDefinitions(
            std::ostream &out,
            int           nIndent
            ) const
    {
        out << LibraryGenerator::indent(nIndent) << "struct " << getParamName()
            << ";\n";
    }

    void LibraryGroup::print(
            std::ostream &out,
            bool          header
            ) const
    {
        if (header) {
            printStructDefinition(out, 0);
        }
    }

} // namespace csl
