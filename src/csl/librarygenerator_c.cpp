// C backend of the library generator. The C library is the numerical core:
// everything emitted here is plain C and is consumed by the C++ and Python
// layers, which only wrap it.

#include "librarygenerator.h"

#include <fstream>

namespace csl {

void LibraryGenerator::printCSpectrum() const
{
    if (!uniqueParamStruct or (diagData.empty() and massExpressions.empty()))
        return;

    std::string guard = "CSL_LIB_" + name + "_CSPECTRUM_H_INCLUDED";
    std::ofstream header(path + "/" + clibDir + "/cspectrum.h");
    header << "#ifndef " << guard << "\n";
    header << "#define " << guard << "\n\n";
    header << "#include \"cparams.h\"\n\n";
    header << "#ifdef __cplusplus\n";
    header << "extern \"C\" {\n";
    header << "#endif\n\n";
    header << "extern double c_spectrum_precision;\n\n";
    if (!diagData.empty())
        header << "void c_updateDiagonalization(cparam_t *params);\n";
    if (!massExpressions.empty())
        header << "void c_updateMassExpressions(cparam_t *params);\n";
    header << "void c_updateSpectrum(cparam_t *params);\n\n";
    header << "#ifdef __cplusplus\n";
    header << "} // extern \"C\" block\n";
    header << "#endif\n\n";
    header << "#endif\n";
    header.close();

    std::ofstream source(path + "/" + clibDir + "/cspectrum.c");
    source << "#include <stdio.h>\n";
    source << "#include <stdlib.h>\n";
    source << "#include \"ccommon.h\"\n";
    source << "#include \"cdiagonalization.h\"\n";
    source << "#include \"cspectrum.h\"\n";
    for (const auto &mass : massExpressions)
        source << "#include \"c_" << mass << ".h\"\n";
    source << '\n';
    source << "double c_spectrum_precision = 1e-5;\n\n";

    if (!diagData.empty()) {
        auto dependencies = getDiagonalizationDependencies();
        const std::string realPart = "creal";
        const std::string imagPart = "cimag";
        source << "void c_updateDiagonalization(cparam_t *params)\n{\n";
        for (const auto &dep : dependencies)
            source << indent(1) << "const ccomplex_t " << regularName(dep)
                   << " = params->" << regularName(dep) << ";\n";
        for (const auto &diag : diagData) {
            const size_t N        = diag.masses.size();
            const size_t s1       = N * N;
            const bool   bidiag   = diag.mixings.size() != s1;
            const size_t entries  = 2 * N * N;

            source << indent(1) << "{\n";
            source << indent(2) << "const size_t N = " << N << ";\n";
            source << indent(2) << "double massMatrix[" << entries << "];\n";
            for (size_t i = 0; i != s1; ++i) {
                source << indent(2) << "{\n";
                source << indent(3) << "const ccomplex_t entry = "
                       << diag.cExpressions[i] << ";\n";
                source << indent(3) << "massMatrix[" << 2 * i
                       << "] = " << realPart << "(entry);\n";
                source << indent(3) << "massMatrix[" << 2 * i + 1
                       << "] = " << imagPart << "(entry);\n";
                source << indent(2) << "}\n";
            }
            source << indent(2) << "double transfer[" << entries << "];\n";
            if (bidiag)
                source << indent(2) << "double transfer2[" << entries
                       << "];\n";
            source << indent(2) << "double mass[" << N << "];\n";
            if (bidiag)
                source << indent(2)
                       << "c_bidiagonalize(N, massMatrix, transfer, "
                          "transfer2, mass, c_spectrum_precision);\n";
            else
                source << indent(2)
                       << "c_diagonalize_hermitian(N, massMatrix, transfer, "
                          "mass, true);\n";

            for (size_t i = 0; i != diag.mixings.size(); ++i) {
                const bool first = i < s1;
                const size_t k   = first ? i : i - s1;
                source << indent(2) << "params->" << diag.mixings[i]
                       << " = " << (first ? "transfer" : "transfer2") << "["
                       << 2 * k << "] + " << (first ? "transfer" : "transfer2")
                       << "[" << 2 * k + 1 << "] * I;\n";
            }
            for (size_t i = 0; i != N; ++i) {
                const std::string &mass = diag.masses[i];
                if (diag.squaredMass) {
                    source << indent(2) << "if (0 > mass[" << i << "]) {\n";
                    source << indent(3) << "fprintf(stderr, \"Warning: "
                           << "negative squared mass for " << mass
                           << ".\\n\");\n";
                    source << indent(2) << "}\n";
                    source << indent(2) << "mass[" << i
                           << "] = sqrt(mass[" << i << "]);\n";
                }
                source << indent(2) << "params->" << mass << " = mass[" << i
                       << "];\n";
            }
            source << indent(1) << "}\n";
        }
        source << "}\n\n";
    }

    if (!massExpressions.empty()) {
        source << "void c_updateMassExpressions(cparam_t *params)\n{\n";
        for (const auto &mass : massExpressions)
            source << indent(1) << "params->" << mass << " = c_" << mass
                   << "(params).real;\n";
        source << "}\n\n";
    }

    source << "void c_updateSpectrum(cparam_t *params)\n{\n";
    if (!diagData.empty())
        source << indent(1) << "c_updateDiagonalization(params);\n";
    if (!massExpressions.empty())
        source << indent(1) << "c_updateMassExpressions(params);\n";
    source << "}\n";
    source.close();
}

} // namespace csl
