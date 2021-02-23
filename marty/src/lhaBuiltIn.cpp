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

#include "lhaBuiltIn.h"
#include "lhaData.h"
#include "lha.h"

#include "SM.h"
#include "MSSM.h"

#include <fstream>

namespace mty::lha {

void readFile(std::string const &fileName)
{
    std::ifstream file(fileName, std::ios::in);
    HEPAssert(file,
            mty::error::NameError,
            "File \"" + fileName + "\" not found.")

    readFile(file);
    file.close();
}

void readFile(std::ifstream &file)
{
    LHAFileData data = Reader::readFile(file);
    std::optional<FloatType> model_value = data.getValue("MODSEL", 0);
    int model = (model_value) ? std::round(*model_value) : 0;
    LHAInputData &sm   = getSMInput();
    std::vector<LHAInputData> &mssm = getMSSMInput(model);
    applyLHAOnInputs(data, sm);
    for (auto &mssm_in : mssm)
        applyLHAOnInputs(data, mssm_in);
}

bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        size_t             id,
        csl::Expr              &expr
        )
{
    HEPAssert(csl::IsLiteral(expr),
            mty::error::TypeError,
            "Expression " + toString(expr) + " should be a literal for lha "
            "reader.")
    std::optional<FloatType> value = data.getValue(nameBlock, id);
    if (value)
        expr->setValue(*value);
    return bool(value);
}

bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        size_t             i,
        size_t             j,
        csl::Expr              &expr
        )
{
    HEPAssert(csl::IsLiteral(expr),
            mty::error::TypeError,
            "Expression " + toString(expr) + " should be a literal for lha "
            "reading.")
    std::optional<FloatType> value = data.getValue(nameBlock, i, j);
    if (value)
        expr->setValue(*value);
    return bool(value);
}

bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        csl::Expr              &expr
        )
{
    HEPAssert(csl::IsMatrix(expr),
            mty::error::TypeError,
            "Expression " + toString(expr) + " should be a matrix for lha "
            "block-type reading.")
    auto block = data.getValues(nameBlock);
    if (block.empty())
        return false;
    
    for (size_t i = 0; i != csl::Size(expr); ++i) 
        for (size_t j = 0; j != csl::Size(expr[i]); ++j) 
            if (!applyLHA(data, nameBlock, i+1, j+1, expr[i][i]))
                expr[i][j]->setValue(0);

    return true;
}

void applyLHAOnInputs(
        LHAFileData const &data,
        LHAInputData      &inputs
        )
{
    for (auto &input : inputs)
        if (input.isMatrix)
            applyLHA(data, inputs.nameBlock, input.expr);
        else
            applyLHA(data, inputs.nameBlock, input.id, input.expr);
}

LHAInputData &getSMInput()
{
    using namespace mty::sm_input;
    static bool firstCall = true;
    static LHAInputData smInputs;
    if (firstCall) {
        smInputs.nameBlock = "SMINPUTS";
        smInputs.push_back({1, alpha_em, false});
        smInputs.push_back({2, G_F,      false});
        smInputs.push_back({3, alpha_s,  false});
        smInputs.push_back({4, M_Z,      false});
        smInputs.push_back({5, m_b,      false});
        smInputs.push_back({6, m_t,      false});
        smInputs.push_back({7, m_tau,    false});
        firstCall = false;
    }

    return smInputs;
}

LHAInputData getMINPARInput(int model)
{
    using namespace mty::mssm_input;
    LHAInputData minpar;
    minpar.nameBlock = "MINPAR";
    switch(model) {
    case 1: // mSUGRA
        minpar.push_back({1, m_0,    false}); 
        minpar.push_back({2, m_1,    false}); 
        minpar.push_back({3, tanb,   false}); 
        minpar.push_back({4, sgn_mu, false}); 
        minpar.push_back({5, A_0,    false}); 
        break;
    case 2: // mGMSB
        minpar.push_back({1, Lambda, false}); 
        minpar.push_back({2, Mmess,  false}); 
        minpar.push_back({3, tanb,   false}); 
        minpar.push_back({4, sgn_mu, false}); 
        minpar.push_back({5, N_5,    false}); 
        minpar.push_back({6, cgrav,  false}); 
        break;
    case 3: // mAMSB
        minpar.push_back({1, m_0,    false}); 
        minpar.push_back({2, m_3,    false}); 
        minpar.push_back({3, tanb,   false}); 
        minpar.push_back({4, sgn_mu, false}); 
        break;
    default: // Other models
        minpar.push_back({3, tanb,   false}); 
    }

    return minpar;
}

LHAInputData getEXTPARInput()
{
    using namespace mty::mssm_input;

    LHAInputData extpar;
    extpar.nameBlock = "EXTPAR";
    extpar.push_back({1, M1, false});
    extpar.push_back({2, M2, false});
    extpar.push_back({3, M3, false});

    extpar.push_back({11, At, false});
    extpar.push_back({12, Ab, false});
    extpar.push_back({13, Atau, false});

    extpar.push_back({21, MH1, false});
    extpar.push_back({22, MH2, false});
    extpar.push_back({23, mu, false});
    extpar.push_back({24, MA, false});
    extpar.push_back({25, tanb, false});
    extpar.push_back({26, MA0, false});
    extpar.push_back({27, MHp, false});

    extpar.push_back({31, MeR, false});
    extpar.push_back({32, MmuR, false});
    extpar.push_back({33, MtauR, false});
    extpar.push_back({34, MeR, false});
    extpar.push_back({35, MmuR, false});
    extpar.push_back({36, MtauR, false});

    extpar.push_back({41, Mq1L, false});
    extpar.push_back({42, Mq2L, false});
    extpar.push_back({43, Mq3L, false});
    extpar.push_back({44, MquR, false});
    extpar.push_back({45, MqcR, false});
    extpar.push_back({46, MqtR, false});
    extpar.push_back({47, MqdR, false});
    extpar.push_back({48, MqsR, false});
    extpar.push_back({49, MqbR, false});

    extpar.push_back({51, N1, false});
    extpar.push_back({52, N2, false});
    extpar.push_back({53, N3, false});

    return extpar;
}

LHAInputData getMatrixInput(
        csl::Expr              &matrix, 
        std::string const &name
        )
{
    LHAInputData input;
    input.nameBlock = name;
    input.push_back({0, matrix, true});
    return input;
}

std::vector<LHAInputData> &getMSSMInput(int model) 
{
    using namespace mty::mssm_input;
    static bool firstCall = true;
    static std::vector<LHAInputData> mssmInputs;
    if (firstCall) {
        firstCall = false;
        mssmInputs.push_back(getMINPARInput(model));
        mssmInputs.push_back(getEXTPARInput());
        mssmInputs.push_back(getMatrixInput(MSQ2, "MSQ2IN"));
        mssmInputs.push_back(getMatrixInput(MSL2, "MSL2IN"));
        mssmInputs.push_back(getMatrixInput(MSu2, "MSU2IN"));
        mssmInputs.push_back(getMatrixInput(MSd2, "MSD2IN"));
        mssmInputs.push_back(getMatrixInput(MSe2, "MSE2IN"));
        mssmInputs.push_back(getMatrixInput(Tu, "TUIN"));
        mssmInputs.push_back(getMatrixInput(Td, "TDIN"));
        mssmInputs.push_back(getMatrixInput(Te, "TEIN"));
    }

    return mssmInputs;
}

} // End of namespace lha
