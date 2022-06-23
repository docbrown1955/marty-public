#include "vectorlike.h"

namespace vl {

    std::vector<std::string> VectorLike_Model::getVLQMasses(
            Model const &model
            )
    {
        std::vector<std::string> masses;
        masses.reserve(10);
        if (model.model5)
            masses.push_back("M_Ds");
        if (model.model6)
            masses.push_back("M_Us");
        if (model.model7)                 
            masses.push_back("M_Td");
        if (model.model8)                
            masses.push_back("M_Tu");
        if (model.model9)               
            masses.push_back("M_QV");
        if (model.model10)             
            masses.push_back("M_Qu");
        if (model.model11)             
            masses.push_back("M_Qd");
        return masses;
    }

    VectorLike_Model::VectorLike_Model(Model const &model)
        :SM_Model(false)
    {
        if (model.model5)
            addVectorLike("Ds", 0, -CSL_1/3,    csl::constant_s("M_Ds"));
        if (model.model6)
            addVectorLike("Us", 0, CSL_2/3,    csl::constant_s("M_Us"));
        if (model.model7)
            addVectorLike("Td", 2, -CSL_1/3,   csl::constant_s("M_Td"));
        if (model.model8)
            addVectorLike("Tu", 2, CSL_2/3,   csl::constant_s("M_Tu"));
        if (model.model9)
            addVectorLike("VQV", 1, CSL_1/6,    csl::constant_s("M_QV"));
        if (model.model10)
            addVectorLike("VQu", 1, 7*CSL_1/6, csl::constant_s("M_Qu"));
        if (model.model11)
            addVectorLike("VQd", 1, -5*CSL_1/6, csl::constant_s("M_Qd"));
        init();
        renameSMQuarkMasses();
        if (model.model7) {
            auto TL1 = getParticle("Td_L_1");
            auto TR1 = getParticle("Td_R_1");
            auto TL2 = getParticle("Td_L_2");
            auto TR2 = getParticle("Td_R_2");
            auto UL = TL1->generateSimilar("Utd_L");
            auto UR = TR1->generateSimilar("Utd_R");
            auto YL = TL1->generateSimilar("Ytd_L");
            auto YR = TR1->generateSimilar("Ytd_R");
            csl::Index a = mty::DiracIndex();
            csl::Index c = generateIndex("SU3c", "Td_L_1");
            csl::Expr sqrt2 = csl::sqrt_s(2);
            replace(TL1, (UL({c,a}) + YL({c,a})) / sqrt2);
            replace(TR1, (UR({c,a}) + YR({c,a})) / sqrt2);
            replace(TL2, CSL_I * (UL({c,a}) - YL({c,a})) / sqrt2);
            replace(TR2, CSL_I * (UR({c,a}) - YR({c,a})) / sqrt2);
            renameParticle("Td_L_3", "Dtd_L");
            renameParticle("Td_R_3", "Dtd_R");
        }
        if (model.model8) {
            auto TL1 = getParticle("Tu_L_1");
            auto TR1 = getParticle("Tu_R_1");
            auto TL2 = getParticle("Tu_L_2");
            auto TR2 = getParticle("Tu_R_2");
            auto XL = TL1->generateSimilar("Xtu_L");
            auto XR = TR1->generateSimilar("Xtu_R");
            auto DL = TL1->generateSimilar("Dtu_L");
            auto DR = TR1->generateSimilar("Dtu_R");
            csl::Index a = mty::DiracIndex();
            csl::Index c = generateIndex("SU3c", "Tu_L_1");
            csl::Expr sqrt2 = csl::sqrt_s(2);
            replace(TL1, (XL({c,a}) + DL({c,a})) / sqrt2);
            replace(TR1, (XR({c,a}) + DR({c,a})) / sqrt2);
            replace(TL2, CSL_I * (XL({c,a}) - DL({c,a})) / sqrt2);
            replace(TR2, CSL_I * (XR({c,a}) - DR({c,a})) / sqrt2);
            renameParticle("Tu_L_3", "Utu_L");
            renameParticle("Tu_R_3", "Utu_R");
        }
        if (model.model9) {
            renameParticle("VQV_L_1", "U_L");
            renameParticle("VQV_R_1", "U_R");
            renameParticle("VQV_L_2", "Du_L");
            renameParticle("VQV_R_2", "Du_R");
        }
        if (model.model10) {
            renameParticle("VQu_L_1", "X_L");
            renameParticle("VQu_R_1", "X_R");
            renameParticle("VQu_L_2", "Ux_L");
            renameParticle("VQu_R_2", "Ux_R");
        }
        if (model.model11) {
            renameParticle("VQd_L_1", "Dy_L");
            renameParticle("VQd_R_1", "Dy_R");
            renameParticle("VQd_L_2", "Y_L");
            renameParticle("VQd_R_2", "Y_R");
        }
        std::vector<std::string> uL = {"u_L", "c_L", "t_L"};
        std::vector<std::string> uR = {"u_R", "c_R", "t_R"};
        if (model.model6) {
            uL.push_back("Us_L");
            uR.push_back("Us_R");
        }
        if (model.model7) {
            uL.push_back("Utd_L");
            uR.push_back("Utd_R");
        }
        if (model.model8) {
            uL.push_back("Utu_L");
            uR.push_back("Utu_R");
        }
        if (model.model9) {
            uL.push_back("U_L");
            uR.push_back("U_R");
        }
        if (model.model10) {
            uL.push_back("Ux_L");
            uR.push_back("Ux_R");
        }
        birotateFields(uL, uR);

        std::vector<std::string> dL = {"d_L", "s_L", "b_L"};
        std::vector<std::string> dR = {"d_R", "s_R", "b_R"};
        if (model.model5) {
            dL.push_back("Ds_L");
            dR.push_back("Ds_R");
        }
        if (model.model7) {
            dL.push_back("Dtd_L");
            dR.push_back("Dtd_R");
        }
        if (model.model8) {
            dL.push_back("Dtu_L");
            dR.push_back("Dtu_R");
        }
        if (model.model9) {
            dL.push_back("Du_L");
            dR.push_back("Du_R");
        }
        if (model.model11) {
            dL.push_back("Dy_L");
            dR.push_back("Dy_R");
        }
        birotateFields(dL, dR);
        refresh();
    }

    void VectorLike_Model::addVectorLike(
            std::string const &name,
            int                irrepSU2,
            csl::Expr   const &hyperCharge,
            csl::Expr   const &mass
            )
    {
        HEPAssert(csl::IsInteger(hyperCharge) || csl::IsIntFraction(hyperCharge),
                mty::error::TypeError,
                "Expecting integer or rational for the hypercharge, "
                + toString(hyperCharge) + " given.")
        const int num = hyperCharge->getNum();
        const int denom = hyperCharge->getDenom();

        mty::Particle VLQ_L = mty::weylfermion_s(
                name + "_L", *this, mty::Chirality::Left);
        mty::Particle VLQ_R = mty::weylfermion_s(
                name + "_R", *this, mty::Chirality::Right);
        VLQ_L->setGroupRep("SU3c", {1, 0});
        VLQ_L->setGroupRep("SU2L", irrepSU2);
        VLQ_L->setGroupRep("U1Y", {num, denom});
        VLQ_R->setGroupRep("SU3c", {1, 0});
        VLQ_R->setGroupRep("SU2L", irrepSU2);
        VLQ_R->setGroupRep("U1Y", {num, denom});
        addParticles({VLQ_L, VLQ_R});
        addFermionicMass(VLQ_L, VLQ_R, mass);

        mty::Particle QL = getParticle("Q");
        mty::Particle DR = getParticle("D_R");
        mty::Particle UR = getParticle("U_R");
        mty::Particle H  = getParticle("H");

        csl::Tensor eps = getVectorSpace("SU2L", "H")->getEpsilon();
        csl::Tensor sig_half = getGenerator("SU2L", "H");
        std::vector<csl::Index> i = generateIndices(10, "SU2L", "H");
        csl::Index a = mty::DiracIndex();
        csl::Index c = generateIndex("SU3c", "Q");
        csl::Index p = generateIndex("SU2L", "W");

        csl::Tensor Ld("Ld_" + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor Lu("Lu_" + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor Lx("Lx_"  + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor Ly("Ly_"  + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor L5u("L5u_" + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor L5d("L5d_" + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Tensor LTu("LTu_" + name, getVectorSpace("SM_flavor", "E_R"));
        LTu->setComplexProperty(csl::ComplexProperty::Complex);
        csl::Tensor LTd("LTd_" + name, getVectorSpace("SM_flavor", "E_R"));
        csl::Index I = generateIndex("SM_flavor", "E_R");

        csl::Expr sqrt2 = csl::sqrt_s(2);

        if (irrepSU2 == 2) {
            // Triplet
            if (num == 2 && denom == 3) { // Tu
                addTensorCoupling(LTu);
                addLagrangianTerm(
                        -2*LTu(I)/sqrt2 * GetComplexConjugate(QL({I,i[0],c,a}))
                            * VLQ_R({p,c,a}) * sig_half({p,i[0],i[1]})
                            * eps({i[1], i[2]}) * GetComplexConjugate(H(i[2])),
                        true
                        );
            }
            else if (num == -1 && denom == 3) { // Td
                addTensorCoupling(LTd);
                addLagrangianTerm(
                        -2*LTd(I)/sqrt2 * GetComplexConjugate(QL({I,i[0],c,a}))
                            * VLQ_R({p,c,a}) * sig_half({p,i[0],i[1]})
                            * H(i[1]),
                        true
                        );
            }
        }
        if (irrepSU2 == 1) {
            if (num == 1 && denom == 6) {
                // QV
                addTensorCoupling(Ld);
                addTensorCoupling(Lu);
                addLagrangianTerm(
                        -Ld(I) * GetComplexConjugate(VLQ_L({i[0],c,a}))
                            * DR({I,c,a}) * H(i[0]),
                        true
                        );
                addLagrangianTerm(
                        -Lu(I) * GetComplexConjugate(VLQ_L({i[0],c,a}))
                            * UR({I,c,a}) * eps({i[0], i[1]})
                            * GetComplexConjugate(H(i[1])),
                        true
                        );
            }
            else if (num == 7 && denom == 6) {
                // Qu 
                addTensorCoupling(Lx);
                addLagrangianTerm(
                        -Lx(I) * GetComplexConjugate(VLQ_L({i[0],c,a}))
                            * UR({I,c,a}) * H(i[0]),
                        true
                        );
            }
            else if (num == -5 && denom == 6) {
                // Qd 
                addTensorCoupling(Ly);
                addLagrangianTerm(
                        -Ly(I) * GetComplexConjugate(VLQ_L({i[0],c,a}))
                            * DR({I,c,a}) * eps({i[0], i[1]})
                            * GetComplexConjugate(H(i[1])),
                        true
                        );
            }
        }
        else if (irrepSU2 == 0) {
            if (num == -1 && denom == 3) {
                // Model 5
                addTensorCoupling(L5d);
                addLagrangianTerm(
                        -L5d(I) * GetComplexConjugate(QL({I, i[0],c,a}))
                            * VLQ_R({c,a}) * H(i[0]),
                        true
                        );
            }
            else if (num == 2 && denom == 3) {
                // Model 6
                addTensorCoupling(L5u);
                addLagrangianTerm(
                        -L5u(I) * GetComplexConjugate(QL({I, i[0],c,a}))
                            * VLQ_R({c,a}) * eps({i[0], i[1]}) 
                            * csl::GetComplexConjugate(H(i[1])),
                        true
                        );
            }
        }
    }

    void VectorLike_Model::renameSMQuarkMasses()
    {
        using namespace mty::sm_input;

        csl::Expr m_d_SM = csl::constant_s("m_d_SM");
        csl::Expr m_s_SM = csl::constant_s("m_s_SM");
        csl::Expr m_b_SM = csl::constant_s("m_b_SM");
        replace(m_d, m_d_SM);
        replace(m_s, m_s_SM);
        replace(m_b, m_b_SM);

        csl::Expr m_u_SM = csl::constant_s("m_u_SM");
        csl::Expr m_c_SM = csl::constant_s("m_c_SM");
        csl::Expr m_t_SM = csl::constant_s("m_t_SM");
        replace(m_u, m_u_SM);
        replace(m_c, m_c_SM);
        replace(m_t, m_t_SM);
    }
}
