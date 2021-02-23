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

    void LibraryGroup::printStructDefinition(
            std::ostream &out,
            int           nIndent
            ) const
    {
        out << LibraryGenerator::indent(nIndent);
        out << "struct " << getParamName();
        out << " {\n";
        for (size_t i = 0; i != parameters.size(); ++i) {
            out << LibraryGenerator::indent(nIndent + 1);
            out << parameters[i].type << " " << parameters[i].name << ";\n";
        }
        out << LibraryGenerator::indent(nIndent) << "};\n";
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
    }

    void LibraryGroup::printParameterDefinition(std::ostream &out) const
    {
        out << getParamName() << " const &param";
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
