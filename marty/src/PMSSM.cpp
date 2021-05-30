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

#include "PMSSM.h"
#include "mrtInterface.h"
#include "CKM.h"
#include "SM.h"

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class PMSSM_Model                              //
/*************************************************/
///////////////////////////////////////////////////

PMSSM_Model::PMSSM_Model(
        std::string const &t_flhafile,
        std::string const &t_saveFile
        )
    :mty::MSSM_Model(t_flhafile, t_saveFile, false)
{
    std::cout << "Initializing gauge and particle content ..." << std::endl;
    initContent();
    std::cout << "Initializing interactions ..." << std::endl;
    initInteractions();
    std::cout << "Gathering MSSM inputs ..." << std::endl;
    gatherMSSMInputs();
    approximateYukawa();
    approximateInputMatrices();
    std::cout << "Getting to low energy Lagrangian ..." << std::endl;
    getToLowEnergyLagrangian();
    std::cout << "Checking Hermiticity ..." << std::endl;
    checkHermiticity();
}

PMSSM_Model::~PMSSM_Model()
{

}

void PMSSM_Model::approximateYukawa()
{
    csl::Expr Yu_t = Yu->getTensor();
    csl::Expr Yd_t = Yd->getTensor();
    csl::Expr Ye_t = Ye->getTensor();

    for (size_t i = 0; i != 3; ++i)
        for (size_t j = 0; j != 3; ++j)
            if (i != 2 or j != 2) {
                Yu_t[i][j] = CSL_0;
                Yd_t[i][j] = CSL_0;
                Ye_t[i][j] = CSL_0;
            }

    Yu->setTensor(Yu_t);
    Yd->setTensor(Yd_t);
    Ye->setTensor(Ye_t);
}

void PMSSM_Model::approximateInputMatrices()
{
    M1 = mssm_input::M1;
    M2 = mssm_input::M2;
    M3 = mssm_input::M3;
    mu_h = mssm_input::mu;
    beta_h = mssm_input::beta;
    csl::Expr MqL = M2_s_Q->getTensor();
    MqL[0][0] = csl::pow_s(mssm_input::Mq1L, 2);
    MqL[2][2] = csl::pow_s(mssm_input::Mq3L, 2);
    csl::Expr MlL = M2_s_L->getTensor();
    MlL[0][0] = csl::pow_s(mssm_input::MeL, 2);
    MlL[2][2] = csl::pow_s(mssm_input::MtauL, 2);
    csl::Expr MuR = M2_s_U->getTensor();
    MuR[0][0] = csl::pow_s(mssm_input::MquR, 2);
    MuR[2][2] = csl::pow_s(mssm_input::MqtR, 2);
    csl::Expr MdR = M2_s_D->getTensor();
    MdR[0][0] = csl::pow_s(mssm_input::MqdR, 2);
    MdR[2][2] = csl::pow_s(mssm_input::MqbR, 2);
    csl::Expr MlR = M2_s_E->getTensor();
    MlR[0][0] = csl::pow_s(mssm_input::MeR, 2);
    MlR[2][2] = csl::pow_s(mssm_input::MtauR, 2);
    csl::Expr A1 = Au->getTensor();
    A1[2][2] = mssm_input::At;
    csl::Expr A2 = Ad->getTensor();
    A2[2][2] = mssm_input::Ab;
    csl::Expr A3 = Ae->getTensor();
    A3[2][2] = mssm_input::Atau;

    for (size_t i = 0; i != 3; ++i)
        for (size_t j = 0 ; j != 3; ++j) 
            if (i != j) {
                MqL[i][j] = CSL_0;
                MlL[i][j] = CSL_0;
                MuR[i][j] = CSL_0;
                MdR[i][j] = CSL_0;
                MlR[i][j] = CSL_0;
                A1[i][j] = CSL_0;
                A2[i][j] = CSL_0;
                A3[i][j] = CSL_0;
            }
            else if (i != 2) {
                if (i == 1) {
                    MqL[i][i] = MqL[i-1][i-1];
                    MlL[i][i] = MlL[i-1][i-1];
                    MuR[i][i] = MuR[i-1][i-1];
                    MdR[i][i] = MdR[i-1][i-1];
                    MlR[i][i] = MlR[i-1][i-1];
                }
                A1[i][i] = CSL_0;
                A2[i][i] = CSL_0;
                A3[i][i] = CSL_0;
            }
    M2_s_Q->setTensor(MqL);
    M2_s_L->setTensor(MlL);
    M2_s_U->setTensor(MuR);
    M2_s_D->setTensor(MdR);
    M2_s_E->setTensor(MlR);

    Au->setTensor(A1);
    Ad->setTensor(A2);
    Ae->setTensor(A2);
}

void PMSSM_Model::approximateQuarkMasses()
{
    // replace(m_u,  CSL_0);
    // replace(m_c,  CSL_0);
    // replace(m_d,  CSL_0);
    // // replace(m_s,  CSL_0);
    // replace(m_e,  CSL_0);
    // replace(m_mu, CSL_0);
}

void PMSSM_Model::approximateCKM()
{
    csl::Expr V = V_CKM->getTensor();
    csl::vector_expr terms = clearDependencies(
            L.interaction,
            [&](Lagrangian::TermType const &term) {
                csl::Expr expr = term->getTerm();
                return expr->dependsOn(sm_input::V_ud_mod.get())
                    or expr->dependsOn(sm_input::V_us_mod.get())
                    or expr->dependsOn(sm_input::V_ub_mod.get())
                    or expr->dependsOn(sm_input::V_cd_mod.get())
                    or expr->dependsOn(sm_input::V_cs_mod.get())
                    or expr->dependsOn(sm_input::V_cb_mod.get())
                    or expr->dependsOn(sm_input::V_td_mod.get())
                    or expr->dependsOn(sm_input::V_ts_mod.get())
                    or expr->dependsOn(sm_input::V_tb_mod.get());
            });

    auto isSmallCKM = [&](csl::Expr const &el)
    {
        return el == sm_input::V_us_mod
            or el == sm_input::V_ub_mod
            or el == sm_input::V_cd_mod
            or el == sm_input::V_cb_mod
            or el == sm_input::V_td_mod
            or el == sm_input::V_ts_mod;
    };
    auto countSmallCKM = [&](csl::Expr const &product)
    {
        size_t count = 0;
        for (const auto &arg : product)
            if (isSmallCKM(arg))
                ++count;
            else if (csl::IsPow(arg)
                    and isSmallCKM(arg[0])
                    and csl::IsInteger(arg[1]))
                count += arg[1]->evaluateScalar();
        return count;
    };

    for (auto& term : terms) {
        csl::Transform(term, [&](csl::Expr &node)
        {
            if (csl::IsProd(node) and countSmallCKM(node) > 1) {
                node = CSL_0;
                return true;
            }
            return false;
        });
        L.push_back(term);
    }
}

void PMSSM_Model::approximateSFermionMixings()
{
    auto stop = [&](mty::QuantumField const &field)
    {
        return field.getParent_info() == getParticle("st_L").get()
            or field.getParent_info() == getParticle("st_R").get();
    };
    auto sbottom = [&](mty::QuantumField const &field)
    {
        return field.getParent_info() == getParticle("sb_L").get()
            or field.getParent_info() == getParticle("sb_R").get();
    };
    auto stau = [&](mty::QuantumField const &field)
    {
        return field.getParent_info() == getParticle("stau_L").get()
            or field.getParent_info() == getParticle("stau_R").get();
    };

    for (size_t i = 0; i != L.mass.size(); ++i) {
        std::vector<mty::QuantumField> const &content 
            = L.mass[i]->getContent();
        if (content[0].getParent_info() == content[1].getParent_info())
            continue;
        mty::QuantumField const &A = content[0];
        mty::QuantumField const &B = content[1];
        if (A.getSpinDimension() != 1
                or (stop(A) and stop(B))
                or (sbottom(A) and sbottom(B))
                or (stau(A) and stau(B)))
            continue;
        L.mass.erase(L.mass.begin() + i);
        --i;
    }
}
void PMSSM_Model::renameSFermions()
{
    std::vector<std::string> names = 
        {"st", "sb", "stau"};
    std::string susyName;
    std::string newName;
    for (const auto &name : names) {
        susyName = name + "_L";
        newName  = name + "_1; \\tilde{" + name + "_1}";
        renameParticle(susyName, newName);
        susyName = name + "_R";
        newName  = name + "_2; \\tilde{" + std::string(name.begin()+1, name.end()) + "_2}";
        renameParticle(susyName, newName);
    }
}

void PMSSM_Model::getToLowEnergyLagrangian()
{
    std::cout << "Breaking SU(2)_L symmetry ...\n";
    breakSU2LGaugeSymmetry();
    replaceWBoson();
    std::cout << "Expanding around Higgses VEV's ...\n";
    expandAroundVEVs();
    L.mergeTerms();
    std::cout << "Diagonalizing SM mass matrices ...\n";
    diagonalize2By2Matrices();
    diagonalizeYukawas();
    L.mergeTerms();
    adjustCouplingConstants();
    std::cout << "Breaking flavor symmetry ...\n";
    breakSMFlavorSymmetry();
    approximateQuarkMasses();
    approximateCKM();
    std::cout << "Diagonalizing MSSM mass matrices ...\n";
    L.mergeTerms();
    approximateSFermionMixings();
    std::cout << "Diagonalizing Neutralinos ..." << std::endl;
    diagonalizeNeutralinos();
    std::cout << "Diagonalizing Charginos ..." << std::endl;
    diagonalizeCharginos();
    std::cout << "Promoting Majorana fermions ..." << std::endl;
    promoteMajoranas();
    std::cout << "Diagonalizing SFermions ..." << std::endl;
    diagonalizeSFermions();
    renameSFermions();
    generateDiracFermions();
    gatherMasses();

    promoteToGoldstone("Gp", "W");
    promoteToGoldstone("G0", "Z");

    refresh();
}

} // End of namespace mty
