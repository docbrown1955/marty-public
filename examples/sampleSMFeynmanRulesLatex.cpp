// Writes the Feynman rules of the Standard Model as a standalone LaTeX
// document, to cross-check the generated diagrams and vertex values.
//
// Usage: sampleSMFeynmanRulesLatex.x [output.tex]
// Compile the output with lualatex (or latex with tikz-feynman installed).

#include "marty.h"
#include "marty/models/sm.h"
#include <fstream>
#include <iostream>

using namespace mty;

int main(int argc, char **argv)
{
    std::string fileName = argc > 1 ? argv[1] : "sm_feynman_rules.tex";

    SM_Model sm;
    auto const &rules = sm.getFeynmanRules();

    std::ofstream out(fileName);
    out << "\\documentclass{article}\n"
        << "\\usepackage{tikz-feynman}\n"
        << "\\usepackage{amsmath}\n"
        << "\\usepackage[margin=2cm]{geometry}\n"
        << "\\begin{document}\n"
        << "\\section*{Standard Model Feynman rules}\n"
        << LatexFeynmanRules(rules)
        << "\\end{document}\n";

    std::cout << rules.size() << " Feynman rules written to " << fileName
              << std::endl;
    return 0;
}
