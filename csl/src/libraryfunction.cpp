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
#include "error.h"
#include "algo.h"
#include <array>
#include <string_view>

namespace csl {

    LibFunction::LibFunction(std::string_view t_name,
                             Expr      const& t_expression,
                             bool             t_complexParameters)
        :name(t_name)
    {
        setExpression(t_expression);
        setComplexParameters(t_complexParameters);
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

    bool LibFunction::isComplexParameters() const
    {
        return complexParameters;
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

    std::vector<std::string> const& LibFunction::getParameters() const
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

    void LibFunction::setComplexParameters(bool t_complexParameters)
    {
        complexParameters = t_complexParameters;
        varType = (complexParameters) ?
            LibraryGenerator::complexUsing : LibraryGenerator::realUsing;
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
            if (p == param)
                return;
        parameters.push_back(param);
    }

    void LibFunction::setParameters(
            std::vector<std::string> const& t_parameters)
    {
        parameters = t_parameters;
        cutParameters();
        sortParameters();
    }

    void LibFunction::removeParameter(std::string_view param)
    {
        for (size_t i = 0; i != parameters.size(); ++i) 
            if (parameters[i] == param) {
                if (tensorParameter >= static_cast<int>(i))
                    --tensorParameter;
                parameters.erase(parameters.begin() + i);
                return;
            }
    }

    void LibFunction::print(std::ostream& out,
                            bool          header,
                            std::string const &initInstruction
                            ) const
    {
        if (header and parameters.size() > nParamThresholdStructure) {
            out << "struct Param_" << name << "{\n\n";
            printParameters(out, ";", true);
            out << "};\n\n";
        }
        printName(out);
        if (parameters.size() <= nParamThresholdStructure)
            printParameters(out);
        else
            out << LibraryGenerator::indent(2) << "Param_" << name << " param\n";
        out << LibraryGenerator::indent(2) << ")";
        if (header)
            out << ";\n";
        else {
            if (tensorial)
                printTensorBody(out, initInstruction);
            else
                printBody(out, initInstruction);
        }
        out << '\n';
    }

    void LibFunction::printName(std::ostream& out) const
    {
        if (tensorial)
            out << LibraryGenerator::tensorUsing << "<" << csl::LibraryGenerator::complexUsing 
                << "> " << name << "(\n";
        else
            out << csl::LibraryGenerator::complexUsing << " " << name << "(\n";
    }

    void LibFunction::printParameters(std::ostream& out,
                                      std::string const& sep,
                                      bool          sepLast) const
    {
        for (size_t i = 0; i != parameters.size(); ++i) {
            out << LibraryGenerator::indent(sepLast ? 1 : 2);
            if (tensorParameter >= 0 and int(i) >= tensorParameter)
                out << LibraryGenerator::tensorUsing << "<";
            else if (!sepLast)
                out << "const ";
            if (complexParameters)
                out << LibraryGenerator::complexUsing;
            else
                out << LibraryGenerator::realUsing;
            if (tensorParameter >= 0 and int(i) >= tensorParameter)
                out << "> " << ((sepLast) ? " " : "const &");
            out << " " << parameters[i];
            if (!(tensorParameter >= 0 and int(i) >= tensorParameter) 
                    and sep == ";")
                out << " = 0";
            if (sepLast or i != parameters.size() - 1)
                out << sep;
            out << "\n";
        }
    }

    void LibFunction::printBody(std::ostream& out,
                                std::string const &initInstruction) const
    {
        out << "\n{\n";
        out << initInstruction;
        if (parameters.size() > nParamThresholdStructure) {
            for (size_t i = 0; i != parameters.size(); ++i) {
                auto const &nameParam = parameters[i];
                out << LibraryGenerator::indent();
                if (tensorParameter >= 0 and int(i) >= tensorParameter)
                    out << LibraryGenerator::tensorUsing << "<";
                else
                    out << "const ";
                out << ((complexParameters) ? 
                        LibraryGenerator::complexUsing 
                        :LibraryGenerator::realUsing);
                if (tensorParameter >= 0 and int(i) >= tensorParameter)
                    out << "> const &";
                else 
                    out << " ";
                out << nameParam << " = param." << nameParam << ';' << '\n';
            }
        }
        Expr toPrint = expression;
        session.printLib(toPrint, perf, out);
        out << "}\n";
    }

    void LibFunction::printTensorBody(
            std::ostream& out,
            std::string const &initInstruction
            ) const
    {
        out << "\n{\n";
        out << initInstruction;
        if (parameters.size() > nParamThresholdStructure) {
            for (const auto& nameParam : parameters) {
                out << LibraryGenerator::indent();
                out << ((complexParameters) ? 
                        LibraryGenerator::complexUsing 
                        :LibraryGenerator::realUsing) << " ";
                out << nameParam << " = param." << nameParam << ';' << '\n';
            }
        }
        Expr toPrint = csl::DeepCopy(expression);
        session.printLib(toPrint, perf, out, true);
        std::vector<std::string> nameTensors;
        for (const auto& tensor : tensors)
            nameTensors.push_back(LibraryGenerator::regularName(tensor->getName()));
        csl::IndexStructure freeStructure = csl::Abbrev::getFreeStructure(
                toPrint->getIndexStructure()
                );

        out << LibraryGenerator::indent() << LibraryGenerator::tensorUsing << "<" 
            << LibraryGenerator::complexUsing << "> tensor({";
        for (size_t i = 0; i != freeStructure.size(); ++i) {
            out << freeStructure[i].getSpace()->getDim();
            if (i < freeStructure.size() - 1)
                out << ", ";
        }
        out << "}, 0.);\n";
        // out << LibraryGenerator::indent() << LibraryGenerator::tensorUsing << "<" 
        //     << LibraryGenerator::complexUsing << ">::iterator iter = tensor.begin();\n";
        int indent = 1;
        std::vector<std::string> nameIndices;
        auto getNameIndex = [&](csl::Index const &index)
        {
            std::string nameIndex = LibraryGenerator::regularName(index.getName()) 
                                    + "_" 
                                    + toString(index.getID());
            return nameIndex;
        };
        for (const csl::Index& index : freeStructure) {
            auto nameIndex = getNameIndex(index);
            out << LibraryGenerator::indent(indent);
            out << "for (size_t " << nameIndex << " = 0; " 
                << nameIndex << " < " << index.getSpace()->getDim() << "; "
                << "++" << nameIndex << ") {\n";
            ++indent;
        }
        csl::ForEachLeaf(toPrint, [&](Expr& el)
        {
            if (el == CSL_I)
                el = LibraryGenerator::imaginary;
            if (not IsIndicialTensor(el))
                return;
            auto pos = std::find_if(
                        tensors.begin(), 
                        tensors.end(), [&](csl::Tensor const& t)
                        {
                            return t.get() == el->getParent_info();
                        });
            if (pos != tensors.end()) 
            {
                std::string name = getTensorName(el->getParent());
                name += "[{";
                csl::IndexStructure structure = el->getIndexStructure();
                bool first = true;
                for (size_t i = 0; i != structure.size(); ++i) {
                    if (structure[i].getType() != cslIndex::Dummy) {
                        if (!first)
                            name += ", ";
                        if (structure[i].getFree()) 
                            name += getNameIndex(structure[i]);
                        else  
                            name += toString(std::abs(structure[i].getValue()));
                    }
                    first = false;
                }
                name += "}]";
                el = csl::constant_s(name);
            }
        });

        std::string name = "tensor";
        name += "[{";
        bool first = true;
        for (size_t i = 0; i != freeStructure.size(); ++i) {
            if (freeStructure[i].getType() != cslIndex::Dummy) {
                if (!first)
                    name += ", ";
                if (freeStructure[i].getFree()) 
                    name += getNameIndex(freeStructure[i]);
                else  
                    name += toString(std::abs(freeStructure[i].getValue()));
            }
            first = false;
        }
        name += "}] = ";
        printExpression(out, toPrint, indent, name);
        for (size_t i = 0; i != freeStructure.size(); ++i) {
            out << LibraryGenerator::indent(--indent) << "}\n";
        }
        out << "\n" << LibraryGenerator::indent() << "return tensor;\n";
        out << "}\n";
    }

    void LibFunction::printExpression(std::ostream& out,
                                      Expr const&   expression,
                                      int           indent,
                                      std::string const& beginning) const
    {
        std::array<char, 4> delimiters = {'+', '*', '-', '/'};
        std::ostringstream sout;
        sout.precision(LibraryGenerator::nDigits);
        expression->print(1, sout, true);
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

    void LibFunction::cutParameters()
    {
        for (size_t i = 0; i != parameters.size(); ++i) {
            for (size_t j = i+1; j < parameters.size(); ++j) {
                if (parameters[i] == parameters[j]) {
                    parameters.erase(parameters.begin() + j);
                    if (tensorParameter >= static_cast<int>(j))
                        --tensorParameter;
                    --j;
                }
            }
        }
    }

    void LibFunction::sortParameters()
    {
        if (tensorParameter != -1) {
            std::sort(parameters.begin(), parameters.begin() + tensorParameter);
            std::sort(parameters.begin() + tensorParameter, parameters.end());
        }
        else
            std::sort(parameters.begin(), parameters.end());
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
        parameters = std::vector<std::string>(unEvaluated.size());
        for (size_t i = 0; i != parameters.size(); ++i)
            parameters[i] = LibraryGenerator::regularName(
                    std::string(unEvaluated[i]->getName()));
        for (size_t i = 0; i != tensors.size(); ++i)
            if (not isEvaluated(tensors[i])) {
                if (tensorParameter == -1)
                    tensorParameter = parameters.size();
                addParameter(getTensorName(tensors[i]));
                // tensors.erase(tensors.begin() + i);
                // --i;
            }
        cutParameters();
        sortParameters();
    }
}
