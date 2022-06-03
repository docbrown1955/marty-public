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

#include "librarygenerator.h"
#include "libraryfunction.h"
#include "librarydependency.h"
#include "evaluation.h"
#include "interface.h"
#include "space.h"
#include "utils.h"
#include "abreviation.h"
#include "librarygroup.h"
#include "error.h"
#include "algo.h"
#include <array>
#include <string_view>

namespace csl {

    LibFunction::LibFunction(std::string_view t_name,
                             Expr      const& t_expression,
                             std::shared_ptr<LibraryGroup> const &t_group,
                             bool forceCppFunc)
        :name(t_name),
        cppFunc(forceCppFunc),
        group(t_group)
    {
        setExpression(t_expression);
    }

    bool LibFunction::isTrivial() const 
    {
        return csl::IsNumerical(expression);
    }

    std::string const& LibFunction::getName() const
    {
        return name;
    }

    std::string LibFunction::getTensorName(csl::Parent const &tensor) const
    {
        if (tensor->getType() == cslParent::Metric 
                or tensor->getType() == cslParent::Delta)
            return std::string(tensor->getName()) 
                + "_" 
                + LibraryGenerator::regularName(tensor->getSpace()[0]->getName());
        return LibraryGenerator::regularName(tensor->getName());
    }

    Expr& LibFunction::getExpression()
    {
        return expression;
    }

    Expr const& LibFunction::getExpression() const
    {
        return expression;
    }

    bool LibFunction::isTensorial() const
    {
        return tensorial;
    }

    std::vector<csl::Tensor> const& LibFunction::getTensors() const
    {
        return tensors;
    }

    std::vector<csl::Tensor>& LibFunction::getTensors()
    {
        return tensors;
    }

    std::vector<std::string> const& LibFunction::getIntermediateSteps() const
    {
        return intermediateSteps;
    }

    std::vector<std::string>& LibFunction::getIntermediateSteps()
    {
        return intermediateSteps;
    }

    std::vector<LibParameter> const& LibFunction::getParameters() const
    {
        return parameters;
    }

    void LibFunction::setName(std::string_view t_name)
    {
        name = t_name;
    }

    void LibFunction::setExpression(Expr const& t_expression)
    {
        expression = t_expression;
        parse();
    }

    void LibFunction::addTensor(Expr const& tensor)
    {
        CSL_ASSERT_SPEC(IsIndicialTensor(tensor),
                CSLError::TypeError,
                "Expression " + toString(tensor) + " should be a tensor.");
        csl::Tensor parent = std::dynamic_pointer_cast<TensorParent>(
                tensor->getParent());
        if (tensors.end() == std::find_if(tensors.begin(), tensors.end(), 
                    [&](csl::Tensor const& other)
                    {
                        return other.get() == parent.get();
                    }))
            tensors.push_back(parent);
    }

    void LibFunction::setTensorial(bool t_tensorial)
    {
        tensorial = t_tensorial;
    }

    void LibFunction::setTensors(std::vector<csl::Tensor> const& t_tensors)
    {
        tensors = t_tensors;
    }

    Expr LibFunction::addIntermediate(Expr const& intermediate)
    {
        const size_t count = 1 + intermediateSteps.size();
        std::string nameInterm = "interm_" + toString(count);
        std::ostringstream sout;
        printExpression(
                sout, 
                intermediate, 
                1, 
                LibraryGenerator::complexUsing + " " + nameInterm + " = ");
        intermediateSteps.push_back(sout.str());

        return csl::constant_s(nameInterm);
    }

    void LibFunction::setIntermediateSteps(
            std::vector<std::string> const& t_intermediateSteps)
    {
        intermediateSteps = t_intermediateSteps;
    }

    void LibFunction::addParameter(std::string const &param)
    {
        for (const auto &p : parameters)
            if (p.name == param)
                return;
        parameters.push_back({param, "real_t", "creal_t"});
    }

    void LibFunction::setParameters(
            std::vector<LibParameter> const& t_parameters)
    {
        parameters = t_parameters;
    }

    void LibFunction::removeParameter(std::string_view param)
    {
        for (size_t i = 0; i != parameters.size(); ++i) 
            if (parameters[i].name == param) {
                if (tensorParameter >= static_cast<int>(i))
                    --tensorParameter;
                parameters.erase(parameters.begin() + i);
                return;
            }
    }

    void LibFunction::addInitInstruction(std::string const &t_inst)
    {
        initInstructions.push_back(t_inst);
    }

    void LibFunction::printExternC(std::ostream& out,
                            bool          header,
                            std::string const &initInstruction
                            ) const
    {
        if (header) {
            group->printForwardDefinitions(out, 0, true);
            out << '\n';
        }
        printCName(out);
        out << LibraryGenerator::indent(2);
<<<<<<< HEAD:src/csl/libraryfunction.cpp
        group->printParameterDefinition(out, isTrivial(), true);
=======
        group->printParameterDefinition(out, csl::IsNumerical(expression), true);
>>>>>>> Generation of C functions:csl/src/libraryfunction.cpp
        out << '\n' << LibraryGenerator::indent(2) << ")";
        if (header)
            out << ";\n";
        else {
            out << "\n{\n";
            for (const auto &inst : initInstructions) {
                out << LibraryGenerator::indent(1) << inst << '\n';
            }
            for (const auto &param : parameters) {
                out << LibraryGenerator::indent(1)
                    << "const " << param.ctype << ' ' << param.name << " = param->" 
                    << param.name << ";\n";
            }
            // group->printParameterInitialization(out, 1);
            printBody(out, initInstruction, true);
            out << "}";
        }
        out << '\n';
    }

    void LibFunction::print(std::ostream& out,
                            bool          header,
                            std::string const &initInstruction
                            ) const
    {
        if (header) {
            group->printForwardDefinitions(out, 0, false);
            out << '\n';
        }
        printName(out);
        out << LibraryGenerator::indent(2);
        group->printParameterDefinition(out, isTrivial());
        out << '\n' << LibraryGenerator::indent(2) << ")";
        if (header)
            out << ";\n";
        else {
            out << "\n{\n";
            for (const auto &inst : initInstructions) {
                out << LibraryGenerator::indent(1) << inst << '\n';
            }
            for (const auto &param : parameters) {
                out << LibraryGenerator::indent(1)
                    << "const " << param.type << ' ' << param.name << " = param."
                    << param.name << ";\n";
            }
            // group->printParameterInitialization(out, 1);
            printBody(out, initInstruction, false);
            out << "}";
        }
        out << '\n';
    }

    void LibFunction::printCppWrapper(
            std::ostream& out
            ) const
    {
        printName(out);
        out << LibraryGenerator::indent(2);
<<<<<<< HEAD:src/csl/libraryfunction.cpp
        group->printParameterDefinition(out, isTrivial());
        out << '\n' << LibraryGenerator::indent(2) << ")";
        out << "\n{\n";
        if (isTrivial()) {
            csl::Expr toPrint = expression;
            session.printLib(toPrint, perf, out);
        }
        else {
            out << LibraryGenerator::indent(1) << "cparam_t cparam;\n";
            for (const auto &param : parameters) {
                if (
                        param.type == LibraryGenerator::complexUsing
                        && !LibraryGenerator::isQuadruplePrecision()
                ) {
                    out << LibraryGenerator::indent(1)
                        << "cparam." << param.name << " = param." << param.name << ".get().real() + _mty_I*"
                        << "param." << param.name << ".get().imag();\n";
                }
                else {
                    out << LibraryGenerator::indent(1)
                        << "cparam." << param.name << " = param." << param.name << ";\n"; 
                }
            }
            if (group->hasComplexReturn()) {
                out << LibraryGenerator::indent(1) << "auto res = c_" << name 
                    << "(&cparam);\n";
                out << LibraryGenerator::indent(1) << "return {"
                    << "res.real, res.imag};\n";
            }
            else {
                out << LibraryGenerator::indent(1) << "return c_"
                    << name << "(&cparam);\n";
            }
        }
=======
        group->printParameterDefinition(out, csl::IsNumerical(expression));
        out << '\n' << LibraryGenerator::indent(2) << ")";
        out << "\n{\n";
        out << LibraryGenerator::indent(1) << "cparam_t cparam;\n";
        for (const auto &param : parameters) {
            if (
                    param.type == LibraryGenerator::complexUsing
                    && !LibraryGenerator::isQuadruplePrecision()
            ) {
                out << LibraryGenerator::indent(1)
                    << "cparam." << param.name << " = param." << param.name << ".real() + _Complex_I*"
                    << "param." << param.name << ".imag();\n";
            }
            else {
                out << LibraryGenerator::indent(1)
                    << "cparam." << param.name << " = param." << param.name << ";\n"; 
            }
        }
        if (group->hasComplexReturn()) {
            out << LibraryGenerator::indent(1) << "auto res = c_" << name 
                << "(&cparam);\n";
            out << LibraryGenerator::indent(1) << "return {"
                << "res.real, res.imag};\n";
        }
        else {
            out << LibraryGenerator::indent(1) << "return c_"
                << name << "(&cparam);\n";
        }
>>>>>>> Full C/C++ library generation:csl/src/libraryfunction.cpp
        out << "}";
        out << '\n';
    }

    void LibFunction::printName(std::ostream& out) const
    {
        out << (group->hasComplexReturn() ?
                LibraryGenerator::complexUsing 
                :LibraryGenerator::realUsing) << " " << name << "(\n";
    }

    void LibFunction::printCName(std::ostream& out) const
    {
        out << (group->hasComplexReturn() ?
<<<<<<< HEAD:src/csl/libraryfunction.cpp
<<<<<<< HEAD:src/csl/libraryfunction.cpp
<<<<<<< HEAD:src/csl/libraryfunction.cpp
                std::string("ccomplex_return_t")
                :LibraryGenerator::crealUsing) << " c_" << name << "(\n";
=======
                LibraryGenerator::complexUsing 
                :LibraryGenerator::realUsing) << " c_" << name << "(\n";
>>>>>>> Generation of C functions:csl/src/libraryfunction.cpp
=======
                LibraryGenerator::ccomplexUsing 
=======
                std::string("ccomplex_return_t")
>>>>>>> Full C/C++ library generation:csl/src/libraryfunction.cpp
                :LibraryGenerator::crealUsing) << " c_" << name << "(\n";
>>>>>>> Resolved conflicts in looptools files:csl/src/libraryfunction.cpp
    }

    void LibFunction::printBody(std::ostream& out,
                                std::string const &initInstruction,
                                bool isCSource) const
    {
        out << initInstruction;
        Expr toPrint = expression;
        if (isCSource)
            session.printCLib(toPrint, perf, out);
        else
            session.printLib(toPrint, perf, out);
    }

    void LibFunction::printExpression(std::ostream& out,
                                      Expr const&   expression,
                                      int           indent,
                                      std::string const& beginning) const
    {
        std::array<char, 4> delimiters = {'+', '*', '-', '/'};
        std::ostringstream sout;
        sout.precision(LibraryGenerator::nDigits);
        expression->print(1, sout, LibraryMode::CppLib);
        std::vector<std::string> lines;
        std::string str = sout.str();

        size_t base = 0;
        size_t max = 80 - LibraryGenerator::nSpaceIndent * indent;
        size_t end = 0;
        for (size_t i = max - beginning.size(); i < str.size(); i += max) {
            for (size_t j = i; j --> base ;)
                if (std::find(delimiters.begin(), delimiters.end(), str[j])
                        != delimiters.end()) {
                    lines.push_back(std::string(str.begin() + base,
                                                str.begin() + j));
                    base = j;
                    i    = j;
                    break;
                }
            end = i;
        }
        if (end != str.size() - 1)
            lines.push_back(std::string(str.begin()+end, str.end()));
        for (size_t i = 0; i != lines.size(); ++i) {
            out << LibraryGenerator::indent(indent) << ((i == 0) ? beginning : "  ") 
                << lines[i] << ((i == lines.size()-1) ? ";" : "") << "\n";
        }
    }

    void LibFunction::cutParameters(
            std::vector<LibParameter> &parameters,
            int                       &tensorParameter
            )
    {
        for (size_t i = 0; i != parameters.size(); ++i) {
            for (size_t j = i+1; j < parameters.size(); ++j) {
                if (parameters[i].name == parameters[j].name) {
                    parameters.erase(parameters.begin() + j);
                    if (tensorParameter >= static_cast<int>(j))
                        --tensorParameter;
                    --j;
                }
            }
        }
    }

    void LibFunction::sortParameters(
            std::vector<LibParameter> &parameters,
            int                        tensorParameter
            )
    {
        auto comp = [&](LibParameter const &A, LibParameter const &B) {
            return A.type < B.type 
                || (A.type == B.type && A.name < B.name);
        };
        if (static_cast<size_t>(tensorParameter) < parameters.size()) {
            std::sort(parameters.begin(), parameters.begin() + tensorParameter, comp);
            std::sort(parameters.begin() + tensorParameter, parameters.end(), comp);
        }
        else
            std::sort(parameters.begin(), parameters.end(), comp);
    }

    void LibFunction::parse()
    {
        auto isEvaluated = [&](csl::Tensor const &t)
        {
            return csl::AllOfLeafs(t->getTensor(), [&](Expr const &el)
            {
                return el->getPrimaryType() == csl::PrimaryType::Numerical;
            });
        };
        tensorial = not expression->getFreeIndexStructure().empty();
        // lib_log << "Parsing expression ...\n";
        session = LibEvalSession::parseExpression(expression, true);
        session.simplify();
        perf = session.getPerf(expression);
        session.gatherUnEvalAndTensors(expression);
        std::vector<Expr> unEvaluated = session.getUnEval();
        tensors = session.getTensors();
        parameters = std::vector<LibParameter>(unEvaluated.size());
        for (size_t i = 0; i != parameters.size(); ++i)
            parameters[i] = {
                LibraryGenerator::regularName(
                    std::string(unEvaluated[i]->getName())
                    ),
                (unEvaluated[i]->isReal()) ? 
                    LibraryGenerator::realUsing 
                    :LibraryGenerator::complexUsing,
                (unEvaluated[i]->isReal()) ? 
                    LibraryGenerator::crealUsing 
                    :LibraryGenerator::ccomplexUsing,
            };
        for (size_t i = 0; i != tensors.size(); ++i)
            if (not isEvaluated(tensors[i])) {
                if (tensorParameter == -1)
                    tensorParameter = parameters.size();
                addParameter(getTensorName(tensors[i]));
                // tensors.erase(tensors.begin() + i);
                // --i;
            }
        cutParameters(parameters, tensorParameter);
        sortParameters(parameters, tensorParameter);
    }
}
