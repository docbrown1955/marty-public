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

/**
 * @file MSSM.h
 * @brief File containing MSSM input parameter and the base MSSM_Model class 
 * for SUSY models.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-01
 */
#ifndef MSSM_H_INCLUDED
#define MSSM_H_INCLUDED

#include "model.h"
#include "MSSMData.h"

/**
 * @brief Namespace containing all MSSM input parameters that may be modified
 * with LHA files.
 *
 * @details All input parameters follow the conventions of
 * https://arxiv.org/pdf/hep-ph/0311123.pdf and
 * https://arxiv.org/pdf/0801.0045.pdf
 *
 * @sa lha.h, lhaBuiltIn.h, mty::lha.
 */
namespace mty::mssm_input {



    /**
     * @brief Common scalar mass.
     */
    inline csl::Expr m_0 = csl::constant_s("m_0");
    /**
     * @brief Common gaugino mass.
     */
    inline csl::Expr m_1 = csl::constant_s("m_1");
    /**
     * @brief Gravitino mass.
     */
    inline csl::Expr m_3 = csl::constant_s("m_3");
    /**
     * @brief Scale of SUSY breaking.
     */
    inline csl::Expr Lambda = csl::constant_s("\\Lambda");
    /**
     * @brief Overall messenger scale.
     */
    inline csl::Expr Mmess = csl::constant_s("M_m");
    /**
     * @brief Messenger index.
     */
    inline csl::Expr N_5 = csl::constant_s("N_5");
    /**
     * @brief Gravitino mass factor.
     */
    inline csl::Expr cgrav = csl::constant_s("c_g");
    /**
     * @brief Ratio of Higgs vacuum expectation values.
     */
    inline csl::Expr tanb = csl::constant_s("tanb");
    /**
     * @brief mu parameter.
     */
    inline csl::Expr mu = csl::constant_s("mu");
    /**
     * @brief Sign of the bilinear Higgs term in the superpotential.
     */
    inline csl::Expr sgn_mu = csl::constant_s("sgn_mu");
    /**
     * @brief Common trilinear coupling.
     */
    inline csl::Expr A_0 = csl::constant_s("A_0");

    /**
     * @brief Bino mass.
     */
    inline csl::Expr M1 = csl::constant_s("M_1");
    /**
     * @brief Wino mass.
     */
    inline csl::Expr M2 = csl::constant_s("M_2");
    /**
     * @brief Gluino mass.
     */
    inline csl::Expr M3 = csl::constant_s("M_3");

    /**
     * @brief Top trilinear coupling.
     */
    inline csl::Expr At = csl::constant_s("A_t");
    /**
     * @brief Bottom trilinear coupling.
     */
    inline csl::Expr Ab = csl::constant_s("A_b");
    /**
     * @brief Tau trilinear coupling.
     */
    inline csl::Expr Atau = csl::constant_s("A_\\tau");

    /**
     * @brief Down-type Higgs mass squared.
     */
    inline csl::Expr MH1 = csl::constant_s("M2_H1");
    /**
     * @brief Up-type Higgs mass squared.
     */
    inline csl::Expr MH2 = csl::constant_s("M2_H2");
    /**
     * @brief Tree-level pseudoscalar Higgs mass parameter squared.
     */
    inline csl::Expr MA = csl::constant_s("M_A");
    /**
     * @brief Pseudoscalar Higgs pole mass.
     */
    inline csl::Expr MA0 = csl::constant_s("M_A0");
    /**
     * @brief Charged Higgs pole mass.
     */
    inline csl::Expr MHp = csl::constant_s("M_Hp");

    /**
     * @brief \f$ U(1)_Y \f$ messenger index.
     */
    inline csl::Expr N1 = csl::constant_s("N_1");
    /**
     * @brief \f$ SU(2)_L \f$ messenger index.
     */
    inline csl::Expr N2 = csl::constant_s("N_2");
    /**
     * @brief \f$ SU(3)_c \f$ messenger index.
     */
    inline csl::Expr N3 = csl::constant_s("N_3");

    /**
     * @brief Left first gen. scalar slepton mass.
     */
    inline csl::Expr MeL = csl::constant_s("M_eL");
    /**
     * @brief Left second gen. scalar slepton mass.
     */
    inline csl::Expr MmuL = csl::constant_s("M_\\muL");
    /**
     * @brief Left third gen. scalar slepton mass.
     */
    inline csl::Expr MtauL = csl::constant_s("M_\\tauL");
    /**
     * @brief Right first gen. scalar slepton mass.
     */
    inline csl::Expr MeR = csl::constant_s("M_eR");
    /**
     * @brief Right second gen. scalar slepton mass.
     */
    inline csl::Expr MmuR = csl::constant_s("M_\\muR");
    /**
     * @brief Right third gen. scalar slepton mass.
     */
    inline csl::Expr MtauR = csl::constant_s("M_\\tauR");

    /**
     * @brief Left first gen. scalar squark mass.
     */
    inline csl::Expr Mq1L = csl::constant_s("M_q1L");
    /**
     * @brief Left second gen. scalar squark mass.
     */
    inline csl::Expr Mq2L = csl::constant_s("M_q2L");
    /**
     * @brief Left third gen. scalar squark mass.
     */
    inline csl::Expr Mq3L = csl::constant_s("M_q3L");
    /**
     * @brief Right up scalar squark mass.
     */
    inline csl::Expr MquR = csl::constant_s("M_quR");
    /**
     * @brief Right charm scalar squark mass.
     */
    inline csl::Expr MqcR = csl::constant_s("M_qcR");
    /**
     * @brief Right top scalar squark mass.
     */
    inline csl::Expr MqtR = csl::constant_s("M_qtR");
    /**
     * @brief Right down scalar squark mass.
     */
    inline csl::Expr MqdR = csl::constant_s("M_qdR");
    /**
     * @brief Right strange scalar squark mass.
     */
    inline csl::Expr MqsR = csl::constant_s("M_qsR");
    /**
     * @brief Right bottom scalar squark mass.
     */
    inline csl::Expr MqbR = csl::constant_s("M_qbR");

#define CST(arg) csl::constant_s(arg)
#define MAT(name) \
    csl::matrix_s({\
        {CST(#name"_11"), CST(#name"_12"), CST(#name"_13")},\
        {CST(#name"_21"), CST(#name"_22"), CST(#name"_23")},\
        {CST(#name"_31"), CST(#name"_32"), CST(#name"_33")}\
    })
#define MAT_SPEC(name, u, c, t) \
    csl::matrix_s({\
        {CST(#name"_"#u#u), CST(#name"_"#u#c), CST(#name"_"#u#t)},\
        {CST(#name"_"#c#u), CST(#name"_"#c#c), CST(#name"_"#c#t)},\
        {CST(#name"_"#t#u), CST(#name"_"#t#c), CST(#name"_"#t#t)}\
    })

    /**
     * @brief Left squark soft SUSY-brekaing mass matrix.
     */
    inline csl::Expr MSQ2 = MAT(MSQ2);
    /**
     * @brief Left slepton soft SUSY-brekaing mass matrix.
     */
    inline csl::Expr MSL2 = MAT(MSL2);
    /**
     * @brief Right up-type squark soft SUSY-brekaing mass matrix.
     */
    inline csl::Expr MSu2 = MAT_SPEC(MSu2, u, c, t);
    /**
     * @brief Right down-type squark soft SUSY-brekaing mass matrix.
     */
    inline csl::Expr MSd2 = MAT_SPEC(MSd2, d, s, b);
    /**
     * @brief Right selectron soft SUSY-brekaing mass matrix.
     */
    inline csl::Expr MSe2 = MAT_SPEC(MSe2, e, mu, tau);

    /**
     * @brief Up-type squark soft SUSY-breaking trilinear couplings
     */
    inline csl::Expr Tu = MAT_SPEC(T_u, u, c, t);
    /**
     * @brief Down-type squark soft SUSY-breaking trilinear couplings
     */
    inline csl::Expr Td = MAT_SPEC(T_d, d, s, b);
    /**
     * @brief Slepton soft SUSY-breaking trilinear couplings
     */
    inline csl::Expr Te = MAT_SPEC(T_e, e, mu, tau);

#undef CST
#undef MAT
#undef MAT_SPEC
}

namespace mty {

/**
 * @brief Base class for MSSM models. If taken as is, represents the 
 * unconstrained MSSM with 105 parameters (+SM).
 */
class MSSM_Model: public mty::Model, protected MSSM_Data {

public:
    MSSM_Model(
            std::string const &flhaFile = "",
            std::string const &saveFile = "",
            bool               init = true
            );

    ~MSSM_Model() override;

protected:

    csl::Expr cc(csl::Expr const &init) {
        return csl::GetComplexConjugate(init);
    }

    void initContent();
        void initGaugeAndFlavor();
        void initGauginos();
        void initLeptons();
        void initSLeptons();
        void initQuarks();
        void initSQuarks();
        void initHiggs();
        void initHiggsinos();
        void initSUSYMap();
        void addSUSYPair(
                mty::Particle const &first,
                mty::Particle const &second
                );

    void initInteractions();
        void initGauginoInteractions();
        void initGauginoInteractions(
                mty::Particle const &gaugeBoson,
                mty::GaugedGroup    *gauged
                );
        void initU1GauginoInteractions(
                mty::Particle const &gaugeBoson,
                mty::GaugedGroup    *gauged
                );
        void initHiggsPotential();
        void initYukawas();
        void initTrilinears();
        void initQuarticInteractions();
            void initQuarticDTerms();
            void initQuarticFTerms();
                void initQuarticSLeptons();
                void initQuarticSQuarks();
                void initQuarticSLeptonHiggs();
                void initQuarticSQuarkHiggs();
                void initQuarticSQuarkSLeptons();

    void gatherMSSMInputs();
        void readFLHA(std::ifstream &flha);
        void applyValues();
        csl::Expr applyValues(csl::Expr tensor);

    void getToLowEnergyLagrangian();
        void breakSU2LGaugeSymmetry();
        void replaceWBoson();
        void expandAroundVEVs();
        void diagonalize2By2Matrices();
        void diagonalizeYukawas();
        void adjustCouplingConstants();
        void breakSMFlavorSymmetry();
        void diagonalizeNeutralinos();
        void diagonalizeCharginos();
        void promoteMajoranas();
        void diagonalizeSFermions();
        void renameSFermions();

    friend
    std::ostream &operator<<(
            std::ostream &out,
            MSSM_Model const &model
            );

private:

    std::string flhaFile;
    std::string saveFile;
};

} // End of namespace mty

#endif
