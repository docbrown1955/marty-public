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

#include "MSSM.h"
#include "diracology.h"
#include "fermionicField.h"
#include "vectorField.h"
#include "scalarField.h"
#include "mrtInterface.h"
#include "SM.h"
#include "CKM.h"
#include "ghostField.h"

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Class MSSM_Model                              //
/*************************************************/
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Constructor, Destructor
///////////////////////////////////////////////////

MSSM_Model::MSSM_Model(
        std::string const &t_saveFile,
        bool               init)
    :mty::Model(),
    mty::MSSM_Data(),
    saveFile(t_saveFile)
{
    if (init) {
        std::ofstream save;
        if (!saveFile.empty())
            save.open(saveFile);
        std::cout << "Initializing gauge and particle content ..." 
            << std::endl;
        initContent();
        std::cout << "Initializing interactions ..." << std::endl;
        initInteractions();
        if (save) {
            save << "****************************" << std::endl;
            save << "**   Initial Lagrangian   **" << std::endl;
            save << "****************************" << std::endl;
            save << *this << "\n\n";
        }
        std::cout << "Gathering MSSM inputs ..." << std::endl;
        gatherMSSMInputs();
        std::cout << "Getting to low energy Lagrangian ..." << std::endl;
        getToLowEnergyLagrangian();
        if (save) {
            save << "****************************" << std::endl;
            save << "**    Final Lagrangian    **" << std::endl;
            save << "****************************" << std::endl;
            save << *this << "\n\n";
        }
        std::cout << "Checking Hermiticity ..." << std::endl;
        checkHermiticity();

        computeFeynmanRules();
        if (save) {
            std::ostream &out = save;
            mty::Display(ComputeFeynmanRules(*this), out);
            mty::DisplayAbbreviations(out);
        }
        if (save)
            save.close();
    }
}

MSSM_Model::~MSSM_Model()
{

}

///////////////////////////////////////////////////
// Content initialization
///////////////////////////////////////////////////

void MSSM_Model::initContent()
{
    initGaugeAndFlavor();
    initGauginos();
    initLeptons();
    initSLeptons();
    initQuarks();
    initSQuarks();
    initHiggs();
    initHiggsinos();
    initSUSYMap();
}
void MSSM_Model::initGaugeAndFlavor()
{
    addGaugedGroup(mty::group::Type::SU, "C", 3, sm_input::g_s);
    addGaugedGroup(mty::group::Type::SU, "L", 2);
    addGaugedGroup(mty::group::Type::U1, "Y");
    SU3_c = gauge->getGaugedGroup(0);
    SU2_L = gauge->getGaugedGroup(1);
    U1_Y  = gauge->getGaugedGroup(2);
    addFlavorGroup("SM_flavor", 3);
    init();

    renameParticle("A_C", "G");
    renameParticle("A_L", "W");
    renameParticle("A_Y", "B");
    renameCoupling("g_L", "g");
    renameCoupling("g_Y", "g'");
    B  = getParticle("B");
    Wi = getParticle("W");
    G  = getParticle("G");
    g_s = getScalarCoupling("g_s");
    g_L = getScalarCoupling("g");
    g_Y = getScalarCoupling("g'");
}
void MSSM_Model::initGauginos()
{
    s_B  = mty::weylfermion_s("sB ; \\tilde{B}", B->getGaugeIrrep(),  Chirality::Left);
    s_Wi = mty::weylfermion_s("sW ; \\tilde{W}", Wi->getGaugeIrrep(), Chirality::Left);
    s_G  = mty::weylfermion_s("sG; \\tilde{G}", G->getGaugeIrrep(),  Chirality::Left);
    s_B->setSelfConjugate(true);
    s_Wi->setSelfConjugate(true);
    s_G->setSelfConjugate(true);
    s_B->setDrawType (drawer::ParticleType::Gaugino);
    s_Wi->setDrawType(drawer::ParticleType::Gaugino);
    s_G->setDrawType (drawer::ParticleType::Gluino);

    M1 = csl::constant_s("M_1");
    M2 = csl::constant_s("M_2");
    M3 = csl::constant_s("M_3");
    s_B->setMass(M1);
    s_Wi->setMass(M2);
    s_G->setMass(M3);
    csl::Index A = generateIndex("C", s_G);
    csl::Index i = generateIndex("L", s_Wi);
    csl::Index a = mty::DiracIndex();

    addParticles({s_B, s_Wi, s_G});
}
void MSSM_Model::initLeptons()
{
    Li = mty::weylfermion_s("L_L", *this, Chirality::Left);
    Li->setGroupRep("L", 1);
    Li->setGroupRep("Y", {-1, 2});
    Li->setFundamentalFlavorRep("SM_flavor");

    Ei = mty::weylfermion_s("E_R", *this, Chirality::Right);
    Ei->setGroupRep("Y", -1);
    Ei->setFundamentalFlavorRep("SM_flavor");

    addParticles({Li, Ei});
    flavorSpace = getVectorSpace("SM_flavor", Li);
}
void MSSM_Model::initSLeptons()
{
    s_Li = mty::scalarboson_s(
            "sL_L ; \\tilde{L_L}", 
            Li->getGaugeIrrep(), 
            Li->getFlavorIrrep());
    s_Ei = mty::scalarboson_s(
            "sE_R ; \\tilde{E_R}", 
            Ei->getGaugeIrrep(), 
            Ei->getFlavorIrrep());

    addParticles({s_Li, s_Ei});

    csl::Index i = generateIndex("L", s_Li);
    csl::Index I = flavorSpace->generateIndex();
    csl::Index J = flavorSpace->generateIndex();
    M2_s_L = csl::Tensor("M2_s_L", {flavorSpace, flavorSpace});
    M2_s_E = csl::Tensor("M2_s_E", {flavorSpace, flavorSpace});
    M2_s_L->setComplexProperty(csl::ComplexProperty::Complex);
    M2_s_E->setComplexProperty(csl::ComplexProperty::Complex);
    M2_s_L->addHermitianProperty(
            flavorSpace,
            M2_s_L({I, J}),
            M2_s_L({I, J})
            );
    M2_s_E->addHermitianProperty(
            flavorSpace,
            M2_s_E({I, J}),
            M2_s_E({I, J})
            );
    addTensorCoupling(M2_s_L);
    addTensorCoupling(M2_s_E);
    addLagrangianTerm(-M2_s_L({I, J}) * cc(s_Li({I, i})) * s_Li({J, i}));
    addLagrangianTerm(-M2_s_E({I, J}) * cc(s_Ei(I)) * s_Ei(J));
}
void MSSM_Model::initQuarks()
{
    Qi = mty::weylfermion_s("Q_L", *this, Chirality::Left);
    Qi->setGroupRep("C", {1, 0});
    Qi->setGroupRep("L", 1);
    Qi->setGroupRep("Y", {1, 6});
    Qi->setFundamentalFlavorRep("SM_flavor");

    Ui = mty::weylfermion_s("U_R", *this, Chirality::Right);
    Ui->setGroupRep("C", {1, 0});
    Ui->setGroupRep("Y", {2, 3});
    Ui->setFundamentalFlavorRep("SM_flavor");
    
    Di = mty::weylfermion_s("D_R", *this, Chirality::Right);
    Di->setGroupRep("C", {1, 0});
    Di->setGroupRep("Y", {-1, 3});
    Di->setFundamentalFlavorRep("SM_flavor");

    addParticles({Qi, Ui, Di});
}
void MSSM_Model::initSQuarks()
{
    s_Qi = mty::scalarboson_s(
            "sQ_L ; \\tilde{Q_L}", 
            Qi->getGaugeIrrep(), 
            Qi->getFlavorIrrep());
    s_Ui = mty::scalarboson_s(
            "sU_R ; \\tilde{U_R}", 
            Ui->getGaugeIrrep(), 
            Ui->getFlavorIrrep());
    s_Di = mty::scalarboson_s(
            "sD_R ; \\tilde{D_R}", 
            Di->getGaugeIrrep(), 
            Di->getFlavorIrrep());

    addParticles({s_Qi, s_Ui, s_Di});

    csl::Index A = generateIndex("C", s_Qi);
    csl::Index i = generateIndex("L", s_Qi);
    csl::Index I = flavorSpace->generateIndex();
    csl::Index J = flavorSpace->generateIndex();
    M2_s_Q = csl::Tensor("M2_s_Q", {flavorSpace, flavorSpace});
    M2_s_U = csl::Tensor("M2_s_U", {flavorSpace, flavorSpace});
    M2_s_D = csl::Tensor("M2_s_D", {flavorSpace, flavorSpace});
    M2_s_Q->setComplexProperty(csl::ComplexProperty::Complex);
    M2_s_U->setComplexProperty(csl::ComplexProperty::Complex);
    M2_s_D->setComplexProperty(csl::ComplexProperty::Complex);
    M2_s_Q->addHermitianProperty(
            flavorSpace,
            M2_s_Q({I, J}),
            M2_s_Q({I, J})
            );
    M2_s_U->addHermitianProperty(
            flavorSpace,
            M2_s_U({I, J}),
            M2_s_U({I, J})
            );
    M2_s_D->addHermitianProperty(
            flavorSpace,
            M2_s_D({I, J}),
            M2_s_D({I, J})
            );
    addTensorCoupling(M2_s_Q);
    addTensorCoupling(M2_s_U);
    addTensorCoupling(M2_s_D);
    addLagrangianTerm(-M2_s_Q({I, J}) * cc(s_Qi({I, A, i})) * s_Qi({J, A, i}));
    addLagrangianTerm(-M2_s_U({I, J}) * cc(s_Ui({I, A})) * s_Ui({J, A}));
    addLagrangianTerm(-M2_s_D({I, J}) * cc(s_Di({I, A})) * s_Di({J, A}));
}
void MSSM_Model::initHiggs()
{
    mu_h   = mssm_input::mu;
    beta_h = mssm_input::beta;
    b_h    = csl::constant_s("b");

    Hu = mty::scalarboson_s("Hu", *this);
    Hu->setGroupRep("L", 1);
    Hu->setGroupRep("Y", {1, 2});
    m_sHu = csl::constant_s("m_sHu");
    Hu->setMass(m_sHu);

    Hd = mty::scalarboson_s("Hd", *this);
    Hd->setGroupRep("L", 1);
    Hd->setGroupRep("Y", {-1, 2});
    m_sHd = csl::constant_s("m_sHd");
    Hd->setMass(m_sHd);
    addParticles({Hu, Hd});
}
void MSSM_Model::initHiggsinos()
{
    s_Hu = mty::weylfermion_s(
            "sHu ; \\tilde{Hu}", 
            Hu->getGaugeIrrep(), 
            Hd->getFlavorIrrep(),
            Chirality::Left);
    s_Hd = mty::weylfermion_s(
            "sHd ; \\tilde{Hd}", 
            Hd->getGaugeIrrep(), 
            Hd->getFlavorIrrep(),
            Chirality::Left);
    addParticles({s_Hu, s_Hd});
    csl::Index i = generateIndex("L", s_Hu);
    csl::Index j = generateIndex("L", s_Hu);
    csl::Tensor eps = i.getSpace()->getEpsilon();
    csl::Index a = mty::DiracIndex();
    csl::Expr hermitic = csl::GetHermitianConjugate(
            -mu_h * s_Hu({i, a}) * eps({i, j}) * s_Hd({j, a}) ,
            &mty::dirac4);
    addLagrangianTerm(
            -mu_h * s_Hu({i, a}) * eps({i, j}) * s_Hd({j, a}),
            true
            );
}
void MSSM_Model::initSUSYMap()
{
    // Gauge
    addSUSYPair(s_G,  G);
    addSUSYPair(s_Wi, Wi);
    addSUSYPair(s_B,  B);

    // Leptons
    addSUSYPair(Li, s_Li);
    addSUSYPair(Ei, s_Ei);

    // Quarks
    addSUSYPair(Qi, s_Qi);
    addSUSYPair(Ui, s_Ui);
    addSUSYPair(Di, s_Di);

    // Higgs
    addSUSYPair(Hu, s_Hu);
    addSUSYPair(Hd, s_Hd);
}
void MSSM_Model::addSUSYPair(
        mty::Particle const &first,
        mty::Particle const &second
        )
{
    SUSY[first]  = second;
    SUSY[second] = first;
}

///////////////////////////////////////////////////
// Interactions initialization
///////////////////////////////////////////////////

void MSSM_Model::initInteractions()
{
    initGauginoInteractions();
    initHiggsPotential();
    initYukawas();
    initTrilinears();
    initQuarticInteractions();
}
void MSSM_Model::initGauginoInteractions()
{
    initGauginoInteractions(G, SU3_c);
    initGauginoInteractions(Wi, SU2_L);
    initU1GauginoInteractions(B, U1_Y);
}
void MSSM_Model::initGauginoInteractions(
        mty::Particle const &gaugeBoson,
        mty::GaugedGroup    *gauged
        )
{
    mty::Group const *group = gauged->getGroup();
    csl::Index A = generateIndex(group, gaugeBoson);
    csl::Expr coupling = gauged->getCouplingConstant();
    mty::Irrep trivial = group->getTrivialRep();
    mty::Particle gaugino = SUSY[gaugeBoson];

    for (auto &scalar : particles) {
        mty::Irrep rep = scalar->getGroupIrrep(group);
        if (scalar->getSpinDimension() > 1 or rep == trivial)
            continue;
        csl::Tensor T = gauged->getGenerator(rep);
        csl::Space const *repSpace = T->getSpace()[1];
        mty::Particle fermion = SUSY[scalar];
        if (not fermion)
            continue;

        std::vector<csl::Index> f_index = fermion->getFullSetOfIndices();
        std::vector<csl::Index> s_index = f_index;
        csl::Index a = f_index.back();
        csl::Index b = a.rename();
        s_index.erase(s_index.end() - 1); // Dirac index

        csl::Index j;
        for (const auto &index : f_index)
            if (index.getSpace() == repSpace) {
                j = index;
                break;
            }
        csl::Index i = j.rename();
        for (auto &index : s_index)
            if (index == j) {
                index = i;
                break;
            }

        bool isLeft = (fermion->getChirality() == Chirality::Left);
        csl::Expr scal = (isLeft) ? cc(scalar(s_index)) : scalar(s_index);
        csl::Expr ferm = (isLeft) ? fermion(f_index) : cc(fermion(f_index));
        csl::Expr connexion = (isLeft) ? 
            dirac4.C_matrix({a, b}) : -dirac4.getDelta()({a, b});
        csl::Expr generator = isLeft ? T({A, i, j}) : T({A, j, i});
        addLagrangianTerm(
                csl::DeepRefreshed(csl::prod_s({
                    -csl::sqrt_s(2), coupling, generator,
                    scal, ferm, connexion, gaugino({A, b})})),
                true
                );
    }
}
void MSSM_Model::initU1GauginoInteractions(
        mty::Particle const &gaugeBoson,
        mty::GaugedGroup    *gauged
        )
{
    mty::Group const *group = gauged->getGroup();
    csl::Expr coupling = gauged->getCouplingConstant();
    mty::Irrep trivial = group->getTrivialRep();
    mty::Particle gaugino = SUSY[gaugeBoson];

    for (auto &scalar : particles) {
        mty::Irrep rep = scalar->getGroupIrrep(group);
        if (scalar->getSpinDimension() > 1 or rep == trivial)
            continue;
        mty::Particle fermion = SUSY[scalar];
        if (not fermion)
            continue;

        csl::Expr charge = getCharge(scalar, group);
        std::vector<csl::Index> f_index = fermion->getFullSetOfIndices();
        std::vector<csl::Index> s_index = f_index;
        csl::Index a = f_index.back();
        csl::Index b = a.rename();
        s_index.erase(s_index.end() - 1); // Dirac index

        bool isLeft = (fermion->getChirality() == Chirality::Left);
        csl::Expr scal = (isLeft) ? cc(scalar(s_index)) : scalar(s_index);
        csl::Expr ferm = (isLeft) ? fermion(f_index) : cc(fermion(f_index));
        csl::Expr connexion = (isLeft) ? 
            dirac4.C_matrix({a, b}) : -dirac4.getDelta()({a, b});
        addLagrangianTerm(
                csl::DeepRefreshed(csl::prod_s({
                    -csl::sqrt_s(2), coupling, charge,
                    scal, ferm, connexion, gaugino(b)})),
                true
                );
    }
}
void MSSM_Model::initHiggsPotential()
{
    // Source : https://arxiv.org/pdf/hep-ph/9709356.pdf
    // (A Supersymmetry Primer p.95)

    csl::Expr mu_2 = mu_h * cc(mu_h);

    csl::Index i = generateIndex("L", Hu);
    csl::Index j = generateIndex("L", Hu);
    csl::Tensor eps = i.getSpace()->getEpsilon();
    csl::Index a = mty::DiracIndex();

    addLagrangianTerm(
            -mu_2 * (cc(Hu(i))*Hu(i) + cc(Hd(i))*Hd(i))
            );
    addLagrangianTerm(
            -b_h * Hu(i) * eps({i, j}) * Hd(j),
            true // Adding also the complex conjugate of this expression
            );
    // addLagrangianTerm(
    //         -(g_Y*g_Y + g_L*g_L) / 8 
    //         * (cc(Hu(i))*Hu(i) - cc(Hd(i))*Hd(i))
    //         * (cc(Hu(j))*Hu(j) - cc(Hd(j))*Hd(j))
    //         );
    // addLagrangianTerm(
    //         -g_L*g_L / 2 * (cc(Hu(i))*Hd(i) * cc(Hd(j))*Hu(j))
    //         );
}
void MSSM_Model::initYukawas()
{
    // SU(3) color space
    csl::Index A = generateIndex("C", Qi);

    // SM flavor space
    csl::Index I = generateIndex("SM_flavor", Li);
    csl::Index J = generateIndex("SM_flavor", Li);

    // SU(2)L space
    csl::Index i = generateIndex("L", Li);
    csl::Index j = generateIndex("L", Li);
    csl::Tensor eps = i.getSpace()->getEpsilon();

    // Dirac space
    csl::Index a = mty::DiracIndex();
    csl::Index b = mty::DiracIndex();
    csl::Tensor C = mty::dirac4.C_matrix;

    Ye = csl::Tensor("Y_e", {flavorSpace, flavorSpace});
    Yu = csl::Tensor("Y_u", {flavorSpace, flavorSpace});
    Yd = csl::Tensor("Y_d", {flavorSpace, flavorSpace});
    Ye->setComplexProperty(csl::ComplexProperty::Complex);
    Yu->setComplexProperty(csl::ComplexProperty::Complex);
    Yd->setComplexProperty(csl::ComplexProperty::Complex);
    addTensorCoupling(Ye);
    addTensorCoupling(Yu);
    addTensorCoupling(Yd);

    // Three supersymmetric terms for Up-type coupling
    addLagrangianTerm(
            -cc(Ui({I, A, a})) 
            * Yu({I, J}) 
            * Qi({J, A, i, a})
            * eps({i, j}) 
            * Hu(j),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            -cc(s_Ui({I, A})) 
            * Yu({I, J}) 
            * Qi({J, A, i, a})
            * eps({i, j}) 
            * C({a, b})
            * s_Hu({j, b}),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            -cc(Ui({I, A, a})) 
            * Yu({I, J}) 
            * s_Qi({J, A, i})
            * eps({i, j}) 
            * s_Hu({j, a}),
            true // Add also the complex conjugate of this term
            );

    // Three supersymmetric terms for Down-type coupling
    addLagrangianTerm(
            cc(Di({I, A, a})) 
            * Yd({I, J}) 
            * Qi({J, A, i, a})
            * eps({i, j}) 
            * Hd(j),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            cc(s_Di({I, A})) 
            * Yd({I, J}) 
            * Qi({J, A, i, a})
            * eps({i, j}) 
            * C({a, b})
            * s_Hd({j, b}),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            cc(Di({I, A, a})) 
            * Yd({I, J}) 
            * s_Qi({J, A, i})
            * eps({i, j}) 
            * s_Hd({j, a}),
            true // Add also the complex conjugate of this term
            );

    // Three supersymmetric terms for Lepton coupling
    addLagrangianTerm(
            cc(Ei({I, a})) 
            * Ye({I, J}) 
            * Li({J, i, a})
            * eps({i, j}) 
            * Hd(j),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            cc(s_Ei(I)) 
            * Ye({I, J}) 
            * Li({J, i, a})
            * eps({i, j}) 
            * C({a, b})
            * s_Hd({j, b}),
            true // Add also the complex conjugate of this term
            );
    addLagrangianTerm(
            cc(Ei({I, a})) 
            * Ye({I, J}) 
            * s_Li({J, i})
            * eps({i, j}) 
            * s_Hd({j, a}),
            true // Add also the complex conjugate of this term
            );
}
void MSSM_Model::initTrilinears()
{
    // SU(3) color space
    csl::Index A = generateIndex("C", Qi);

    // SM flavor space
    csl::Index I = generateIndex("SM_flavor", Li);
    csl::Index J = generateIndex("SM_flavor", Li);

    // SU(2)L space
    csl::Index i = generateIndex("L", Li);
    csl::Index j = generateIndex("L", Li);
    csl::Parent eps = i.getSpace()->getEpsilon();

    Au = csl::Tensor("Au ; A_u", {flavorSpace, flavorSpace});
    Ad = csl::Tensor("Ad ; A_d", {flavorSpace, flavorSpace});
    Ae = csl::Tensor("Ae ; A_e", {flavorSpace, flavorSpace});
    Au->setComplexProperty(csl::ComplexProperty::Complex);
    Ad->setComplexProperty(csl::ComplexProperty::Complex);
    Ae->setComplexProperty(csl::ComplexProperty::Complex);

    // Up-type tri-linear from Au
    addLagrangianTerm(
            -cc(s_Ui({I, A})) 
            * Au({I, J}) 
            * s_Qi({J, A, i})
            * eps({i, j})
            * Hu(j),
            true // Add also the complex conjugate of this term
            );
    // Down-type tri-linear from Ad
    addLagrangianTerm(
            cc(s_Di({I, A})) 
            * Ad({I, J}) 
            * s_Qi({J, A, i})
            * eps({i, j})
            * Hd(j),
            true // Add also the complex conjugate of this term
            );
    // Lepton tri-linear from Ae
    addLagrangianTerm(
            cc(s_Ei({I})) 
            * Ae({I, J}) 
            * s_Li({J, i})
            * eps({i, j})
            * Hd(j),
            true // Add also the complex conjugate of this term
            );
    
    // Up-type tri-linear from the super potential
    addLagrangianTerm(
            cc(mu_h)
            * cc(s_Ui({J, A}))
            * Yu({J, I}) 
            * s_Qi({I, A, i})
            * cc(Hd(i)),
            true // Add also the complex conjugate of this term
            );
    // Down-type tri-linear from the super potential
    addLagrangianTerm(
            cc(mu_h)
            * cc(s_Di({J, A}))
            * Yd({J, I})
            * s_Qi({I, A, i})
            * cc(Hu(i)),
            true // Add also the complex conjugate of this term
            );
    // Lepton tri-linear from the super potential
    addLagrangianTerm(
            cc(mu_h)
            * cc(s_Ei({J}))
            * Ye({J, I}) 
            * s_Li({I, i}) 
            * cc(Hu(i)),
            true // Add also the complex conjugate of this term
            );
}
void MSSM_Model::initQuarticInteractions()
{
    initQuarticDTerms();
    L.mergeTerms();
    initQuarticFTerms();
}
void MSSM_Model::initQuarticDTerms()
{
    for (size_t i = 0; i != gauge->size(); ++i) {
        for (size_t p1 = 0; p1 != particles.size(); ++p1) {
            if (particles[p1]->getSpinDimension() != 1
                or particles[p1]->getParticleType() 
                    == mty::ParticleType::GhostBoson)
                continue;
            auto rep1 = particles[p1]->getGroupIrrep((*gauge)[i]);
            bool sign1 = SUSY[particles[p1]]->isFermionic()
                && SUSY[particles[p1]]->getChirality() == Chirality::Right;
            for (size_t p2 = 0; p2 < particles.size(); ++p2) {
                if (particles[p2]->getSpinDimension() != 1
                    or particles[p2]->getParticleType() 
                        == mty::ParticleType::GhostBoson)
                    continue;
                auto rep2 = particles[p2]->getGroupIrrep((*gauge)[i]);
                if (rep1 != rep2 
                        and (*gauge)[i]->getType() != mty::group::Type::U1)
                    continue;
                bool sign2 = SUSY[particles[p2]]->isFermionic()
                    && SUSY[particles[p2]]->getChirality() == Chirality::Right;

                csl::Index mu = mty::MinkowskiIndex();
                csl::Expr field1 = particles[p1]->getInstance();
                csl::Expr field2 = particles[p2]->getInstance();
                csl::Expr quadratic1 = 
                        csl::GetComplexConjugate(field1)
                        * gauge->getGaugedGroup(i)->covariantDerivative(
                            field1,
                            mu);
                csl::Expr quadratic2 = 
                        csl::GetComplexConjugate(field2)
                        * gauge->getGaugedGroup(i)->covariantDerivative(
                            field2,
                            +mu);
                csl::Index A;
                csl::ForEachLeaf(quadratic1, [&](csl::Expr &leaf)
                {
                    if (!leaf->isIndexed())
                        return;
                    for (const auto &index : leaf->getIndexStructureView())
                        if (index == mu) {
                            A = leaf->getIndexStructureView()[0];
                            leaf = CSL_1;
                            return;
                        }
                });
                csl::Index B;
                csl::ForEachLeaf(quadratic2, [&](csl::Expr &leaf)
                {
                    if (!leaf->isIndexed())
                        return;
                    for (const auto &index : leaf->getIndexStructureView())
                        if (index == mu) {
                            B = leaf->getIndexStructureView()[0];
                            leaf = CSL_1;
                            return;
                        }
                });
                csl::Replace(quadratic2, B, A);
                csl::Expr sign = (sign1 ^ sign2) ? CSL_M_1 : CSL_1;
                addLagrangianTerm(
                        csl::Refreshed(sign * CSL_HALF 
                            * quadratic1 * quadratic2
                        ));
            }
        }
    }
}
void MSSM_Model::initQuarticFTerms()
{
    initQuarticSLeptons();
    initQuarticSQuarks();
    initQuarticSLeptonHiggs();
    initQuarticSQuarkHiggs();
    initQuarticSQuarkSLeptons();
}
#define DEFINE_GAUGE_INDICES\
    csl::Index A = generateIndex("C", Qi); \
    csl::Index B = generateIndex("C", Qi); \
 \
    csl::Index I = generateIndex("SM_flavor", Li); \
    csl::Index J = generateIndex("SM_flavor", Li); \
    csl::Index K = generateIndex("SM_flavor", Li); \
    csl::Index L = generateIndex("SM_flavor", Li); \
 \
    csl::Index i = generateIndex("L", Li); \
    csl::Index j = generateIndex("L", Li); \
    csl::Index k = generateIndex("L", Li); \
    csl::Index l = generateIndex("L", Li); \
    csl::Tensor eps = i.getSpace()->getEpsilon(); \
 \
    csl::Index a = mty::DiracIndex();

void MSSM_Model::initQuarticSLeptons()
{
    DEFINE_GAUGE_INDICES
    // ~L~L~E~E interaction
    addLagrangianTerm(
            -Ye({I, J}) 
            * cc(Ye({L, K})) 
            * cc(s_Li({K, i}))
            * s_Li({J, i})
            * cc(s_Ei({I}))
            * s_Ei({L})
            );
}
void MSSM_Model::initQuarticSQuarks()
{
    DEFINE_GAUGE_INDICES
    // ~Q~Q~U~U interaction
    addLagrangianTerm(
            -Yu({I, J}) 
            * cc(Yu({L, K})) 
            * cc(s_Qi({K, A, i}))
            * s_Qi({J, A, i})
            * cc(s_Ui({I, B}))
            * s_Ui({L, B})
            );
    // ~Q~Q~D~D interaction
    addLagrangianTerm(
            -Yd({I, J}) 
            * cc(Yd({L, K})) 
            * cc(s_Qi({K, A, i}))
            * s_Qi({J, A, i})
            * cc(s_Di({I, B}))
            * s_Di({L, B})
            );
}
void MSSM_Model::initQuarticSLeptonHiggs()
{
    DEFINE_GAUGE_INDICES
    // ~L~LH_dH_d interaction
    addLagrangianTerm(
            -Ye({I, J}) 
            * cc(Ye({I, K})) 
            * cc(s_Li({K, k})) 
            * s_Li({J, i})
            * eps({i, j}) * Hd(j)
            * eps({k, l}) * cc(Hd(l))
            );
    // ~E~EH_dH_d interaction
    addLagrangianTerm(
            -Ye({I, J}) 
            * cc(Ye({L, J})) 
            * cc(s_Ei({I}))
            * s_Ei({L})
            * Hd(i) * cc(Hd(i))
            );
}
void MSSM_Model::initQuarticSQuarkHiggs()
{
    DEFINE_GAUGE_INDICES
    // ~Q~QH_uH_u interaction
    addLagrangianTerm(
            -Yu({I, J}) 
            * cc(Yu({I, K})) 
            * cc(s_Qi({K, A, k})) 
            * s_Qi({J, A, i})
            * eps({i, j}) * Hu(j)
            * eps({k, l}) * cc(Hu(l))
            );
    // ~Q~QH_dH_d interaction
    addLagrangianTerm(
            -Yd({I, J}) 
            * cc(Yd({I, K})) 
            * cc(s_Qi({K, A, k})) 
            * s_Qi({J, A, i})
            * eps({i, j}) * Hd(j)
            * eps({k, l}) * cc(Hd(l))
            );

    // ~U~UH_uH_u interaction
    addLagrangianTerm(
            -Yu({I, J}) 
            * cc(Yu({L, J})) 
            * cc(s_Ui({I, A}))
            * s_Ui({L, A})
            * Hu(i) * cc(Hu(i))
            );
    // ~D~DH_dH_d interaction
    addLagrangianTerm(
            -Yd({I, J}) 
            * cc(Yd({L, J})) 
            * cc(s_Di({I, A}))
            * s_Di({L, A})
            * Hd(i) * cc(Hd(i))
            );

    // ~U~DH_uH_d interaction
    addLagrangianTerm(
            Yu({I, J}) 
            * cc(Yd({L, J})) 
            * cc(s_Ui({I, A})) 
            * s_Di({L, A})
            * Hu(i) * cc(Hd(i)),
            true // Adding complex conjugate
            );
}
void MSSM_Model::initQuarticSQuarkSLeptons()
{
    DEFINE_GAUGE_INDICES
    // ~D~Q~L~E interaction
    addLagrangianTerm(
            Yd({I, J}) 
            * cc(Ye({L, K})) 
            * cc(s_Di({I, A})) 
            * s_Qi({J, A, i})
            * cc(s_Li({K, i}))
            * s_Ei({L}),
            true // Adding complex conjugate
            );
}

#undef DEFINE_GAUGE_INDICES

///////////////////////////////////////////////////
// Gathering inputs
///////////////////////////////////////////////////

void MSSM_Model::gatherMSSMInputs()
{
    M2_s_Q->setTensor(mty::mssm_input::MSQ2);
    M2_s_L->setTensor(mty::mssm_input::MSL2);
    M2_s_U->setTensor(mty::mssm_input::MSu2);
    M2_s_D->setTensor(mty::mssm_input::MSd2);
    M2_s_E->setTensor(mty::mssm_input::MSe2);

    Au->setTensor(mty::mssm_input::Tu);
    Ad->setTensor(mty::mssm_input::Td);
    Ae->setTensor(mty::mssm_input::Te);
}

void MSSM_Model::applyValues()
{
    M2_s_Q->setTensor(applyValues(M2_s_Q->getTensor()));
    M2_s_L->setTensor(applyValues(M2_s_L->getTensor()));
    M2_s_U->setTensor(applyValues(M2_s_U->getTensor()));
    M2_s_D->setTensor(applyValues(M2_s_D->getTensor()));
    M2_s_E->setTensor(applyValues(M2_s_E->getTensor()));
    Au->setTensor(applyValues(Au->getTensor()));
    Ad->setTensor(applyValues(Ad->getTensor()));
    Ae->setTensor(applyValues(Ae->getTensor()));
}
csl::Expr MSSM_Model::applyValues(csl::Expr tensor)
{
    csl::ForEachLeaf(tensor, [&](csl::Expr &el)
    {
        if (csl::IsConstant(el) or csl::IsVariable(el)) {
            if (el->getValued())
                csl::Evaluate(el, csl::eval::literal);
            else
                el = CSL_0;
        }
    });
    return tensor;
}

///////////////////////////////////////////////////
// Low energy effective theory for MSSM
///////////////////////////////////////////////////

void MSSM_Model::getToLowEnergyLagrangian()
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
    adjustCouplingConstants();
    std::cout << "Breaking flavor symmetry ...\n";
    breakSMFlavorSymmetry();
    std::cout << "Diagonalizing MSSM mass matrices ...\n";
    L.mergeTerms();
    std::cout << "Diagonalizing Neutralinos ..." << std::endl;
    diagonalizeNeutralinos();
    std::cout << "Diagonalizing Charginos ..." << std::endl;
    diagonalizeCharginos();
    std::cout << "Promoting Majorana fermions ..." << std::endl;
    promoteMajoranas();
    std::cout << "Diagonalizing SFermions ..." << std::endl;
    csl::ScopedProperty p(&mty::InteractionTerm::abbreviateFactors, true);
    diagonalizeSFermions();
    renameSFermions();
    generateDiracFermions();
    gatherMasses();

    promoteToGoldstone("G^+", "W");
    promoteToGoldstone("G^0", "Z");
}
void MSSM_Model::breakSU2LGaugeSymmetry()
{
    breakU1GaugeSymmetry("Y");
    breakGaugeSymmetry(
            "L",

            {"Hu", "Hd", "W", "c_W", 
            "Q_L", "L_L", 
            "sHu", "sHd", "sW", 
            "sQ_L", "sL_L"},

            {{"Hup", "Hu0"},
            {"Hd0", "Hdm"},
            {"W_1", "W_2", "W_3"},
            {"c_W_1", "c_W_2", "c_W_3"},
            {"U_L", "D_L"},
            {"Nu_L  ; \\Nu_L", "E_L"},

            {"sHup  ; \\tilde{H}_u^+",   "sHu0 ; \\tilde{H}_u^0"},
            {"sHd0  ; \\tilde{H}_d^0",   "sHdm ; \\tilde{H}_d^-"},
            {"sW1   ; \\tilde{W}",       "sW2  ; \\tilde{W}^2", 
                "sW3 ; \\tilde{W}^3"},
            {"sU_L  ; \\tilde{U}_L",     "sD_L ; \\tilde{D}_L"},
            {"sNu_L ; \\tilde{\\Nu}_L",  "sE_L ; \\tilde{E}_L"}});

    getParticle("sHu0")->setSelfConjugate(true);
    getParticle("sHd0")->setSelfConjugate(true);
}
void MSSM_Model::replaceWBoson()
{
    Particle W1   = GetParticle(*this, "W_1");
    Particle W2   = GetParticle(*this, "W_2");
    Particle W_SM = W1->generateSimilar("W");
    W_SM->setSelfConjugate(false);

    Particle cW1 = getParticle("c_W_1");
    Particle cW2 = getParticle("c_W_2");
    Particle cWp = W_SM->getGhostBoson();
    cWp->setName("c_Wp ; c_{+}");
    Particle cWm = ghostboson_s("c_Wm; c_{-}", W_SM, true);
    W_SM->setConjugatedGhostBoson(cWm);

    csl::Index mu = MinkowskiIndex();
    csl::Index nu = MinkowskiIndex();
    csl::Expr W_p = W_SM(+mu);
    csl::Expr W_m = csl::GetComplexConjugate(W_SM(+mu));
    csl::Expr F_W_p = W_SM({+mu,+nu});
    csl::Expr F_W_m = csl::GetComplexConjugate(W_SM({+mu, +nu}));

    auto W1_expr = [](csl::Expr const &Wp, csl::Expr const &Wm) {
        return (Wp + Wm) / csl::sqrt_s(2);
    };
    auto W2_expr = [](csl::Expr const &Wp, csl::Expr const &Wm) {
        return CSL_I * (Wp - Wm) / csl::sqrt_s(2);
    };
    replace(W1, W1_expr(W_p, W_m));
    replace(W2, W2_expr(W_p, W_m));
    replace(W1->getFieldStrength(), W1_expr(F_W_p, F_W_m));
    replace(W2->getFieldStrength(), W2_expr(F_W_p, F_W_m));
    replace(cW1, W1_expr(cWp, cWm));
    replace(cW2, W2_expr(cWp, cWm));

    Particle s_W1   = GetParticle(*this, "sW1");
    Particle s_W2   = GetParticle(*this, "sW2");
    Particle s_W  = GenerateSimilarParticle("sWp ; \\tilde{W}^+", s_W1);
    Particle s_Wc = GenerateSimilarParticle("sWm ; \\tilde{W}^-", s_W2);
    s_W ->setSelfConjugate(false);
    s_Wc->setSelfConjugate(false);
    csl::Index alpha = mty::DiracIndex();
    W_p = s_W(alpha);
    W_m = s_Wc(alpha);

    replace(s_W1, W1_expr(W_p, W_m));
    replace(s_W2, W2_expr(W_p, W_m));
}

void MSSM_Model::expandAroundVEVs()
{
    v_h = sm_input::v;
    beta_h = csl::constant_s("beta");
    csl::Expr vu = v_h * csl::sin_s(beta_h);
    csl::Expr vd = v_h * csl::cos_s(beta_h);

    Particle Hu0 = GetParticle(*this, "Hu0");
    Particle Hup = GetParticle(*this, "Hup");
    Particle Hdm = GetParticle(*this, "Hdm");
    Particle Hd0 = GetParticle(*this, "Hd0");

    Particle phi_u_c = scalarboson_s("phi_u; \\phi _u^+", *this);
    Particle phi_d_c = scalarboson_s("phi_d; \\phi _d^+", *this);
    Particle rho_u  = scalarboson_s("rho_u; \\rho _u", *this);
    Particle rho_d  = scalarboson_s("rho_d; \\rho _d", *this);
    Particle eta_u  = scalarboson_s("eta_u; \\eta _u", *this);
    Particle eta_d  = scalarboson_s("eta_d; \\eta _d", *this);
    SetSelfConjugate(rho_u, true);
    SetSelfConjugate(rho_d, true);
    SetSelfConjugate(eta_u, true);
    SetSelfConjugate(eta_d, true);

    Replaced(*this,
            Hu0,
            (rho_u + CSL_I*eta_u + vu)/csl::sqrt_s(2));
    Replaced(*this,
            Hup,
            phi_u_c);

    Replaced(*this,
            Hdm,
            cc(phi_d_c));
    Replaced(*this,
            Hd0,
            (rho_d + CSL_I*eta_d + vd)/csl::sqrt_s(2));
    csl::Expr mu_2 = mu_h * csl::GetComplexConjugate(mu_h);
    M_A0 = csl::constant_s("M_A");
    Replaced(*this,
            b_h,
            csl::sin_s(beta_h)*csl::cos_s(beta_h) * (M_A0 * M_A0));
}
void MSSM_Model::diagonalize2By2Matrices()
{
    diagonalizeSymbolically("B");
    renameParticle("W_3", "Z");
    renameParticle("B", "A");

    csl::Expr mu_2 = cc(mu_h) * mu_h;
    csl::Expr MZ2 = csl::pow_s(sm_input::M_Z, 2);
    // Replaced(*this,
    //         g_L*g_L + g_Y*g_Y,
    //         csl::pow_s(sm_input::e_em / csl::sin_s(sm_input::theta_W), 2)
    //             * (1 + csl::pow_s(csl::tan_s(sm_input::theta_W), 2)));
    csl::Expr c2beta = csl::cos_s(2*beta_h); 
    Replaced(*this,
            m_sHu,
            csl::sqrt_s(M_A0*M_A0 - m_sHd*m_sHd - 2*mu_2));
    Replaced(*this,
            m_sHd,
            csl::sqrt_s(
            csl::Expanded(-MZ2 * c2beta / 2)
            + csl::Expanded(M_A0*M_A0 * (1 - c2beta) / 2)
            - mu_2));
    // Replaced(*this,
    //         csl::pow_s(csl::cos_s(beta_h), 2),
    //         1 - csl::pow_s(csl::sin_s(beta_h), 2)
    //         );
    mty::Particle eta_u = getParticle("eta_u");
    mty::Particle eta_d = getParticle("eta_d");
    mty::Particle G0 = scalarboson_s("G0; G^0", *this);
    mty::Particle A0 = scalarboson_s("A0; A^0", *this);
    mty::SetSelfConjugate(G0, true);
    mty::SetSelfConjugate(A0, true);
    rotateFields(
           {eta_u, eta_d}, 
           {G0, A0},
           {{csl::sin_s(beta_h), csl::cos_s(beta_h)},
            {-csl::cos_s(beta_h), csl::sin_s(beta_h)}},
            true, 1 // diagonalize, one massless state
           ); 

    mty::Particle phi_u = getParticle("phi_u");
    mty::Particle phi_d = getParticle("phi_d");
    mty::Particle Gp = scalarboson_s("Gp; G^+", *this);
    mty::Particle Hp = scalarboson_s("Hp; H^+", *this);
    rotateFields(
           {phi_u, phi_d}, 
           {Gp, Hp},
           {{csl::sin_s(beta_h), csl::cos_s(beta_h)},
            {-csl::cos_s(beta_h), csl::sin_s(beta_h)}},
            true, 1 // diagonalize, one massless state
           ); 

    mty::Particle rho_u = getParticle("rho_u");
    mty::Particle rho_d = getParticle("rho_d");
    mty::Particle h = scalarboson_s("h0; h^0", *this);
    mty::Particle H = scalarboson_s("H0; H^0", *this);
    mty::SetSelfConjugate(h, true);
    mty::SetSelfConjugate(H, true);
    alpha_h = mssm_input::alpha;
    rotateFields(
           {rho_u, rho_d}, 
           {h, H},
           {{csl::cos_s(alpha_h), csl::sin_s(alpha_h)},
            {-csl::sin_s(alpha_h), csl::cos_s(alpha_h)}},
            true // diagonalize, no massless state
           ); 

    Replaced(*this, 
            getParticle("W")->getMass(),
            sm_input::M_W);
    Replaced(*this, 
            getParticle("Z")->getMass(),
            sm_input::M_Z);
}
void MSSM_Model::diagonalizeYukawas()
{
    m_e   = sm_input::m_e;   
    m_mu  = sm_input::m_mu;  
    m_tau = sm_input::m_tau; 
    m_u   = sm_input::m_u;   
    m_c   = sm_input::m_c;   
    m_t   = sm_input::m_t;   
    m_d   = sm_input::m_d;   
    m_s   = sm_input::m_s;   
    m_b   = sm_input::m_b;   

    const csl::Space* flavorSpace = csl::GetSpace(Ye);
    csl::Tensor M_e = csl::tensor_s(
            "M_e",
            {flavorSpace, flavorSpace},
            csl::matrix_s(
                {{m_e, 0, 0},
                 {0, m_mu, 0},
                 {0, 0, m_tau}}
                 )
            );

    csl::Tensor M_u = csl::tensor_s(
            "M_u",
            {flavorSpace, flavorSpace},
            csl::matrix_s(
                {{m_u, 0, 0},
                 {0, m_c, 0},
                 {0, 0, m_t}}
                 )
            );

    csl::Tensor M_d = csl::tensor_s(
            "M_d",
            {flavorSpace, flavorSpace},
            csl::matrix_s(
                {{m_d, 0, 0},
                 {0, m_s, 0},
                 {0, 0, m_b}}
                 )
            );


    csl::Index f_i = csl::GetIndex(flavorSpace);
    csl::Index f_j = csl::GetIndex(flavorSpace);
    csl::Index f_k = csl::GetIndex(flavorSpace);
    csl::Index f_l = csl::GetIndex(flavorSpace);
    csl::Tensor delta_flav  = mty::Delta(flavorSpace);

    mty::buildCKM(flavorSpace);

    replace(
            Ye,
            csl::sqrt_s(2) / (v_h * csl::cos_s(beta_h)) * M_e({f_i, f_j}));
    replace(
            Yu,
            csl::sqrt_s(2) / (v_h * csl::sin_s(beta_h)) * M_u({f_i, f_j}));
    replace(
            Yd({f_i, f_j}),
            csl::sqrt_s(2) / (v_h * csl::cos_s(beta_h)) 
            * csl::prod_s(
                {V_CKM({f_i, f_k}), 
                M_d({f_k, f_l}),
                GetHermitianConjugate(V_CKM({f_l, f_j}), flavorSpace)}
                , true)
            );

    mty::Particle D_L = GetParticle(*this, "D_L");
    mty::Particle D_R = GetParticle(*this, "D_R");
    mty::Particle sD_L = GetParticle(*this, "sD_L");
    mty::Particle sD_R = GetParticle(*this, "sD_R");
    csl::Index a1  = DiracIndex();
    csl::Index A   = generateIndex("C", D_L);

    replace(
            D_L({f_j, A, a1}),
            V_CKM({f_j, f_k}) * D_L({f_k, A, a1})
            );
    replace(
            D_R({f_i, A, a1}),
            V_CKM({f_i, f_j}) * D_R({f_j, A, a1})
            );
    replace(
            sD_L({f_j, A}),
            V_CKM({f_j, f_k}) * sD_L({f_k, A})
            );
    replace(
            sD_R({f_i, A}),
            V_CKM({f_i, f_j}) * sD_R({f_j, A})
            );

    Replaced(*this, 
            v_h,
            (2 * sm_input::M_W * csl::sin_s(sm_input::theta_W)) 
            / sm_input::e_em
            );
}
void MSSM_Model::adjustCouplingConstants()
{
    //csl::Expr c2 = csl::pow_s(csl::cos_s(beta_h), 2);
    //csl::Expr s2 = csl::pow_s(csl::sin_s(beta_h), 2);
    //replace(c2, 1 - s2);

    csl::Expr e = sm_input::e_em;
    csl::Expr thetaW = sm_input::theta_W;
    replace(
            g_L*g_L + g_Y*g_Y,
            csl::pow_s(g_L, 2) * (1 + csl::pow_s(csl::tan_s(thetaW), 2)));
    replace(
            g_Y, 
            e / csl::cos_s(thetaW));
    replace(
            g_L, 
            e / csl::sin_s(thetaW));
    // replace(
    //         csl::cos_s(thetaW),
    //         sm_input::M_W / sm_input::M_Z);
}
void MSSM_Model::breakSMFlavorSymmetry()
{
    L.mergeTerms();
    BreakFlavorSymmetry(
            *this,
            "SM_flavor",

            {"U_L", "U_R", "D_L", "D_R", "E_L", "E_R", "Nu_L",
            "sU_L", "sU_R", "sD_L", "sD_R", "sE_L", "sE_R", "sNu_L"},

            {{"u_L", "c_L", "t_L"},
            {"u_R", "c_R", "t_R"},
            {"d_L", "s_L", "b_L"},
            {"d_R", "s_R", "b_R"},
            {"e_L", "mu_L; \\mu_L", "tau_L; \\tau_L"},
            {"e_R", "mu_R; \\mu_R", "tau_R; \\tau_R"},
            {"nu_e; \\nu_{eL}", "nu_mu; \\nu_{\\mu L}", "nu_tau; \\nu_{\\tau L}"},
            {"su_L;\\tilde{u}_L", "sc_L;\\tilde{c}_L", "st_L;\\tilde{t}_L"},
            {"su_R;\\tilde{u}_R", "sc_R;\\tilde{c}_R", "st_R;\\tilde{t}_R"},
            {"sd_L;\\tilde{d}_L", "ss_L;\\tilde{s}_L", "sb_L;\\tilde{b}_L"},
            {"sd_R;\\tilde{d}_R", "ss_R;\\tilde{s}_R", "sb_R;\\tilde{b}_R"},
            {"se_L;\\tilde{e}_L", "smu_L; \\tilde{\\mu}_L", 
                "stau_L; \\tilde{\\tau}_L"},
            {"se_R;\\tilde{e}_R", "smu_R; \\tilde{\\mu}_R", 
                "stau_R; \\tilde{\\tau}_R"},
            {"snu_e; \\tilde{\\nu}_{eL}", "snu_mu; \\tilde{\\nu}_{\\mu L}", 
                "snu_tau; \\tilde{\\nu}_{\\tau L}"}});
}
void MSSM_Model::diagonalizeNeutralinos()
{
    mty::Particle N1_init = getParticle("sB");
    mty::Particle N2_init = getParticle("sW3");
    mty::Particle N3_init = getParticle("sHd0");
    mty::Particle N4_init = getParticle("sHu0");

    mty::Particle N1 = N1_init->generateSimilar("N_1");
    mty::Particle N2 = N1_init->generateSimilar("N_2");
    mty::Particle N3 = N1_init->generateSimilar("N_3");
    mty::Particle N4 = N1_init->generateSimilar("N_4");
    csl::Expr N_B1 = csl::constant_s("N_B1", csl::ComplexProperty::Complex);
    csl::Expr N_W1 = csl::constant_s("N_W1", csl::ComplexProperty::Complex);
    csl::Expr N_d1 = csl::constant_s("N_d1", csl::ComplexProperty::Complex);
    csl::Expr N_u1 = csl::constant_s("N_u1", csl::ComplexProperty::Complex);
    csl::Expr N_B2 = csl::constant_s("N_B2", csl::ComplexProperty::Complex);
    csl::Expr N_W2 = csl::constant_s("N_W2", csl::ComplexProperty::Complex);
    csl::Expr N_d2 = csl::constant_s("N_d2", csl::ComplexProperty::Complex);
    csl::Expr N_u2 = csl::constant_s("N_u2", csl::ComplexProperty::Complex);
    csl::Expr N_B3 = csl::constant_s("N_B3", csl::ComplexProperty::Complex);
    csl::Expr N_W3 = csl::constant_s("N_W3", csl::ComplexProperty::Complex);
    csl::Expr N_d3 = csl::constant_s("N_d3", csl::ComplexProperty::Complex);
    csl::Expr N_u3 = csl::constant_s("N_u3", csl::ComplexProperty::Complex);
    csl::Expr N_B4 = csl::constant_s("N_B4", csl::ComplexProperty::Complex);
    csl::Expr N_W4 = csl::constant_s("N_W4", csl::ComplexProperty::Complex);
    csl::Expr N_d4 = csl::constant_s("N_d4", csl::ComplexProperty::Complex);
    csl::Expr N_u4 = csl::constant_s("N_u4", csl::ComplexProperty::Complex);

    rotateFields(
           {N1_init, N2_init, N3_init, N4_init}, 
           {N1, N2, N3, N4},
           {{N_B1, N_B2, N_B3, N_B4},
           {N_W1, N_W2, N_W3, N_W4},
           {N_d1, N_d2, N_d3, N_d4},
           {N_u1, N_u2, N_u3, N_u4}},
           true // Diagonalizes mass terms on the go
           ); 
}
void MSSM_Model::diagonalizeCharginos()
{
    mty::Particle C1_init = getParticle("sWp");
    mty::Particle C2_init = getParticle("sHup");
    mty::Particle C3_init = getParticle("sWm");
    mty::Particle C4_init = getParticle("sHdm");

    mty::Particle C1p = C1_init->generateSimilar("C1p");
    mty::Particle C2p = C1_init->generateSimilar("C2p");
    mty::Particle C1m = C1_init->generateSimilar("C1m");
    mty::Particle C2m = C1_init->generateSimilar("C2m");

    csl::Expr V_Wp1 = csl::constant_s("V_Wp1", csl::ComplexProperty::Complex);
    csl::Expr V_Wp2 = csl::constant_s("V_Wp2", csl::ComplexProperty::Complex);
    csl::Expr U_Wm1 = csl::constant_s("U_Wm1", csl::ComplexProperty::Complex);
    csl::Expr U_Wm2 = csl::constant_s("U_Wm2", csl::ComplexProperty::Complex);

    csl::Expr V_u1 = csl::constant_s("V_u1", csl::ComplexProperty::Complex);
    csl::Expr V_u2 = csl::constant_s("V_u2", csl::ComplexProperty::Complex);
    csl::Expr U_d1 = csl::constant_s("U_d1", csl::ComplexProperty::Complex);
    csl::Expr U_d2 = csl::constant_s("U_d2", csl::ComplexProperty::Complex);

    rotateFields(
           {C1_init, C2_init}, 
           {C1p,     C2p},
           {{V_Wp1, V_Wp2},
           {V_u1, V_u2}},

           {C3_init, C4_init}, 
           {C1m,     C2m},
           {{U_Wm1, U_Wm2},
           {U_d1, U_d2}}
           );

    csl::Index a = mty::DiracIndex();
    csl::Index b = mty::DiracIndex();
    mty::Particle C1 = mty::diracfermion_s("C_1; C_1^+", *this);
    mty::Particle C2 = mty::diracfermion_s("C_2; C_2^+", *this);
    C1->setMass(C1p->getMass());
    C2->setMass(C2p->getMass());
    auto C = mty::dirac4.C_matrix;
    replace(cc(C1m(a)), -C({a, b}) * C1->getWeylFermion(Chirality::Right)(b));
    replace(cc(C2m(a)), -C({a, b}) * C2->getWeylFermion(Chirality::Right)(b));
    replace(C1m, -C({b, a}) * cc(C1->getWeylFermion(Chirality::Right)(b)));
    replace(C2m, -C({b, a}) * cc(C2->getWeylFermion(Chirality::Right)(b)));
    replace(C1p, C1->getWeylFermion(Chirality::Left)(a));
    replace(C2p, C2->getWeylFermion(Chirality::Left)(a));
}

void MSSM_Model::promoteMajoranas()
{
    promoteToMajorana("sG");
    promoteToMajorana("N_1");
    promoteToMajorana("N_2");
    promoteToMajorana("N_3");
    promoteToMajorana("N_4");
}

void MSSM_Model::sortSfermions(std::vector<mty::Particle> &parts)
{
    const static std::string order = "em uct dsb"; // t is for top and tau
    auto comp = [&](Particle const &A, Particle const &B) {
        auto nameA = A->getName();
        auto nameB = B->getName();
        if (nameA.size() < 2) return true;
        if (nameB.size() < 2) return true;
        bool A_left = nameA.find('L') != std::string::npos;
        bool B_left = nameB.find('L') != std::string::npos;
        if (A_left & !B_left) return true;
        if (!A_left & B_left) return false;
        size_t A_order = order.find(nameA[1]);
        size_t B_order = order.find(nameB[1]);
        if (A_order != std::string::npos && B_order != std::string::npos) {
            return A_order < B_order;
        }
        return nameA < nameB;
    };
    std::sort(parts.begin(), parts.end(), comp);
}

void MSSM_Model::diagonalizeSFermions()
{
    bool diagonalizedSymbolically;
    do {
        diagonalizedSymbolically = false;
        std::vector<MassBlock> massBlocks = getMassBlocks();
        std::sort(
                massBlocks.begin(),
                massBlocks.end(),
                [&](MassBlock const &A, MassBlock const &B)
                {
                    return A.particles.size() > B.particles.size();
                });
        for (auto &block : massBlocks) {
            mty::QuantumField firstField(
                    L.mass[block.positions[0]]->getContent()[0]);
            if (firstField.getSpinDimension() != 1)
                continue;
            if (block.particles.size() >= 2) {
                sortSfermions(block.particles);
                std::cout << "Diagonalizing block of size " 
                    << block.particles.size() << " : " << std::endl;
                for (const auto &part : block.particles)
                    std::cout << part->getName() << "  ";
                std::cout << " ... " << std::endl;
                rotateFields(block.particles, true);
                diagonalizedSymbolically = true;
                if (diagonalizedSymbolically) {
                    std::cout << "Interaction Lagrangian has know "
                        << L.size() << " terms." << std::endl;
                    break;
                }
                break;
            }
        }
    } while (diagonalizedSymbolically);
    std::cout << "Merging terms ..." << std::endl;
    do {
        diagonalizedSymbolically = false;
        std::vector<MassBlock> massBlocks = getMassBlocks();
        for (const auto &block : massBlocks) {
            mty::QuantumField firstField(
                    L.mass[block.positions[0]]->getContent()[0]);
            if (firstField.getSpinDimension() != 1)
                continue;
            if (block.particles.size() == 2) {
                std::cout << "Diagonalizing block of size " 
                    << block.particles.size() << " : " << std::endl;
                for (const auto &part : block.particles)
                    std::cout << part->getName() << "  ";
                std::cout << " ... " << std::endl;
                diagonalizedSymbolically 
                    = diagonalizeSymbolically(firstField.getName());
                if (diagonalizedSymbolically) {
                    L.mergeTerms();
                    std::cout << "Interaction Lagrangian has know "
                        << L.size() << " terms." << std::endl;
                    break;
                }
            }
        }
    } while (diagonalizedSymbolically);
}
void MSSM_Model::renameSFermions()
{
    std::vector<std::string> names = 
        {"su", "sc", "st", "sd", "ss", "sb", "se", "smu", "stau"};
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

void MSSM_Model::generateDiracFermions()
{
    constexpr std::array diracPairs = {
        std::pair {"e_L", "e_R"},
        std::pair {"mu_L", "mu_R"},
        std::pair {"tau_L", "tau_R"},
        std::pair {"u_L", "u_R"},
        std::pair {"d_L", "d_R"},
        std::pair {"s_L", "s_R"},
        std::pair {"c_L", "c_R"},
        std::pair {"t_L", "t_R"},
        std::pair {"b_L", "b_R"},
    };
    for (const auto &[namePsiL, namePsiR] : diracPairs) {
        if (findParticle(namePsiL) && findParticle(namePsiR)) {
            const auto psiL = getParticle(namePsiL);
            const auto psiR = getParticle(namePsiR);
            if (psiL->getMass() == CSL_0 && psiR->getMass() == CSL_0) {
                diracFermionEmbedding(namePsiL, namePsiR);
            }
        }
    }
}

std::ostream &operator<<(
        std::ostream &out,
        MSSM_Model const &model
        )
{
    return out << *static_cast<Model const*>(&model);
}

MSSM_HEM::MSSM_HEM(
        std::string const &saveFile
        )
    :MSSM_Model(saveFile, false)
{
    std::ofstream save;
    if (!saveFile.empty())
        save.open(saveFile);
    std::cout << "Initializing gauge and particle content ..." 
        << std::endl;
    initContent();
    std::cout << "Initializing interactions ..." << std::endl;
    initInteractions();
    if (save) {
        save << "****************************" << std::endl;
        save << "**   Initial Lagrangian   **" << std::endl;
        save << "****************************" << std::endl;
        save << *this << "\n\n";
    }
    promoteToMajorana("sB");
    promoteToMajorana("sW");
    promoteToMajorana("sG");
    std::cout << "Gathering MSSM inputs ..." << std::endl;
    gatherMSSMInputs();
    // std::cout << "Checking Hermiticity ..." << std::endl;
    // checkHermiticity();

    computeFeynmanRules();
    if (save) {
        std::ostream &out = save;
        mty::Display(ComputeFeynmanRules(*this), out);
        mty::DisplayAbbreviations(out);
    }
    if (save)
        save.close();
}


} // End of namespace mty
