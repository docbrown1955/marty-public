#ifndef DEFINITION_____H_INCLUDED
#define DEFINITION_____H_INCLUDED

#include "marty.h"

namespace mty {

inline std::unique_ptr<mty::Model> build_QCD()
{
    std::unique_ptr<mty::Model> model = std::make_unique<mty::Model>();
    mty::Model &_model_ = *model;
    csl::Tensor gamma  = mty::dirac4.gamma;
    csl::Tensor gamma5 = mty::dirac4.gamma_chir;
    csl::Tensor P_L    = mty::dirac4.P_L;
    csl::Tensor P_R    = mty::dirac4.P_R;
    csl::Tensor sigma  = mty::dirac4.sigma;
    
    
    ///////////////////////////////////
    // Gauge definition              //
    ///////////////////////////////////
    
    
    csl::Expr g = csl::constant_s("g");
    model->addGaugedGroup(
        mty::group::Type::SU,
        "SU3_c",
        3,
        g
        );
    mty::GaugedGroup *SU3_c = model->getGauge()->getGaugedGroup(0);
    
    model->init(false);
    
    
    ///////////////////////////////////
    // Particle masses definitions   //
    ///////////////////////////////////
    
    
    csl::Expr m_u = csl::constant_s("m_u");
    csl::Expr m_d = csl::constant_s("m_d");
    csl::Expr m_X = csl::constant_s("m_X");
    
    
    ///////////////////////////////////
    // Particle definitions          //
    ///////////////////////////////////
    
    
    mty::Particle G     = SU3_c->buildVectorBoson();
    G->setSelfConjugate(true);
    G->setGroupRep("SU3_c", {1, 1});
    _model_.addParticle(G, false);
    mty::Particle F_F_G     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(F_G.get()));
    
    mty::Particle c_G = mty::ghostboson_s("c_G ; c_G", std::dynamic_pointer_cast<mty::VectorBoson>(G));
    model->addParticle(c_G, false);
    
    mty::Particle u_L = mty::weylfermion_s("u_L ; u_L", _model_, Chirality::Left);
    u_L->setMass(m_u);
    u_L->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(u_L, false);
    
    mty::Particle u_R = mty::weylfermion_s("u_R ; u_R", _model_, Chirality::Right);
    u_R->setMass(m_u);
    u_R->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(u_R, false);
    
    mty::Particle d_L = mty::weylfermion_s("d_L ; d_L", _model_, Chirality::Left);
    d_L->setMass(m_d);
    d_L->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(d_L, false);
    
    mty::Particle d_R = mty::weylfermion_s("d_R ; d_R", _model_, Chirality::Right);
    d_R->setMass(m_d);
    d_R->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(d_R, false);
    
    mty::Particle X_L = mty::weylfermion_s("X_L ; X_L", _model_, Chirality::Left);
    X_L->setMass(m_X);
    X_L->setGroupRep("SU3_c", {2, 0});
    _model_.addParticle(X_L, false);
    
    mty::Particle X_R = mty::weylfermion_s("X_R ; X_R", _model_, Chirality::Right);
    X_R->setMass(m_X);
    X_R->setGroupRep("SU3_c", {2, 0});
    _model_.addParticle(X_R, false);
    
    mty::Particle u     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    u_L),
         std::dynamic_pointer_cast<mty::WeylFermion>(u_R));
    u->setMass(m_u);
    u->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(u, false);
    
    mty::Particle d     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    d_L),
         std::dynamic_pointer_cast<mty::WeylFermion>(d_R));
    d->setMass(m_d);
    d->setGroupRep("SU3_c", {1, 0});
    _model_.addParticle(d, false);
    
    mty::Particle X     = mty::diracfermion_s(
    std::dynamic_pointer_cast<mty::WeylFermion>(    X_L),
         std::dynamic_pointer_cast<mty::WeylFermion>(X_R));
    X->setMass(m_X);
    X->setGroupRep("SU3_c", {2, 0});
    _model_.addParticle(X, false);
    
    ///////////////////////////////////
    // Spaces                        //
    ///////////////////////////////////
    
    static std::unique_ptr<csl::Space> dirac_uptr = std::make_unique<csl::Space>(
        "dirac",
        4,
        std::vector<std::string>({"alpha", "beta", "gam", "del", "eps", "eta", })
        );
    csl::Space *dirac = dirac_uptr.get();
    static std::unique_ptr<csl::Space> Minko_uptr = std::make_unique<csl::Space>(
        "Minko",
        4,
        "g",
        csl::matrix_s({csl::vector_s({1, 0, 0, 0}), 
csl::vector_s({0, -1, 0, 0}), 
csl::vector_s({0, 0, -1, 0}), 
csl::vector_s({0, 0, 0, -1})}),
        std::vector<std::string>({"\\mu", "\\nu", "\\rho", "\\sigma", "\\lambda", "\\tau", })
        );
    csl::Space *Minko = Minko_uptr.get();
    csl::Space const *SU3_c_1_1 = SU3_c->getGroup()->getVectorSpace(SU3_c->getGroup()->highestWeightRep({1, 1, }));
    csl::Space const *SU3_c_1_0 = SU3_c->getGroup()->getVectorSpace(SU3_c->getGroup()->highestWeightRep({1, 0, }));
    csl::Space const *SU3_c_2_0 = SU3_c->getGroup()->getVectorSpace(SU3_c->getGroup()->highestWeightRep({2, 0, }));
    

    ///////////////////////////////////
    // Tensors                       //
    ///////////////////////////////////
    
    csl::Tensor X("X", {Minko});
    csl::TDerivative d_der("d", &Minko);
    mty::Generator f_SU3_c_1_1 = SU3_c->getGenerator({1, 1});
    csl::Tensor V_0("V_0", {Minko});
    mty::Generator T_SU3_c_20_SU3_c_2_0 = SU3_c->getGenerator({2, 0});
    mty::Generator T_SU3_c_10_SU3_c_1_0 = SU3_c->getGenerator({1, 0});
    csl::Tensor V_0("V_0", {Minko});
    csl::Tensor V_0("V_0", {Minko});
    csl::Tensor V_0("V_0", {Minko});
    csl::Tensor V_0("V_0", {Minko});
    csl::Tensor V_0("V_0", {Minko});
    

    ///////////////////////////////////
    // Variable / Constants          //
    ///////////////////////////////////
    
    

    ///////////////////////////////////
    // Indices                       //
    ///////////////////////////////////
    
    std::vector<csl::Index> i_dirac = dirac.generateIndices(2);
    std::vector<csl::Index> i_Minko = Minko.generateIndices(2);
    std::vector<csl::Index> i_SU3_c_1_1 = SU3_c_1_1.generateIndices(5);
    std::vector<csl::Index> i_SU3_c_1_0 = SU3_c_1_0.generateIndices(2);
    std::vector<csl::Index> i_SU3_c_2_0 = SU3_c_2_0.generateIndices(2);
    

    
    
    ///////////////////////////////////
    // Kinetic terms                 //
    ///////////////////////////////////
    
    
    model->addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 4), F_G({+i_SU3_c_1_1[ 0 ], +i_Minko[ 0 ], +i_Minko[ 1 ]}, X), F_G({+i_SU3_c_1_1[ 0 ], i_Minko[ 0 ], i_Minko[ 1 ]}, X)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({csl::tderivativeelement_s(X, d_der, {i_Minko[ 0 ]}, csl::GetComplexConjugate(c_G({+i_SU3_c_1_1[ 0 ]}, X)), 0), csl::tderivativeelement_s(X, d_der, {+i_Minko[ 0 ]}, c_G({+i_SU3_c_1_1[ 0 ]}, X), 0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({CSL_I, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), csl::GetComplexConjugate(u({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)), csl::tderivativeelement_s(X, d_der, {i_Minko[ 0 ]}, u({+i_SU3_c_1_0[ 0 ], +i_dirac[ 1 ]}, X), 0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({CSL_I, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), csl::GetComplexConjugate(d({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)), csl::tderivativeelement_s(X, d_der, {i_Minko[ 0 ]}, d({+i_SU3_c_1_0[ 0 ], +i_dirac[ 1 ]}, X), 0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({CSL_I, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), csl::GetComplexConjugate(X({+i_SU3_c_2_0[ 0 ], +i_dirac[ 0 ]}, X)), csl::tderivativeelement_s(X, d_der, {i_Minko[ 0 ]}, X({+i_SU3_c_2_0[ 0 ], +i_dirac[ 1 ]}, X), 0)}),
        false);
    
    
    ///////////////////////////////////
    // Mass terms                    //
    ///////////////////////////////////
    
    
    model->addLagrangianTerm(
        csl::prod_s({-1
, m_u, csl::GetComplexConjugate(u({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)), u({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({-1
, m_d, csl::GetComplexConjugate(d({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)), d({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, X)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({-1
, m_X, csl::GetComplexConjugate(X({+i_SU3_c_2_0[ 0 ], +i_dirac[ 0 ]}, X)), X({+i_SU3_c_2_0[ 0 ], +i_dirac[ 0 ]}, X)}),
        false);
    
    
    ///////////////////////////////////
    // Interaction terms             //
    ///////////////////////////////////
    
    
    model->addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 4), csl::pow_s(g, 2), f_SU3_c_1_1({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_1[ 1 ], +i_SU3_c_1_1[ 2 ]})
, f_SU3_c_1_1({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_1[ 3 ], +i_SU3_c_1_1[ 4 ]})
, G({+i_SU3_c_1_1[ 1 ], +i_Minko[ 0 ]}, V_0), G({+i_SU3_c_1_1[ 2 ], +i_Minko[ 1 ]}, V_0), G({+i_SU3_c_1_1[ 3 ], i_Minko[ 0 ]}, V_0), G({+i_SU3_c_1_1[ 4 ], i_Minko[ 1 ]}, V_0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({csl::intfraction_s(-1, 2), g, f_SU3_c_1_1({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_1[ 1 ], +i_SU3_c_1_1[ 2 ]})
, G({+i_SU3_c_1_1[ 1 ], i_Minko[ 0 ]}, V_0), G({+i_SU3_c_1_1[ 2 ], i_Minko[ 1 ]}, V_0), F_G({+i_SU3_c_1_1[ 0 ], +i_Minko[ 0 ], +i_Minko[ 1 ]}, V_0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({g, f_SU3_c_1_1({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_1[ 1 ], +i_SU3_c_1_1[ 2 ]})
, G({+i_SU3_c_1_1[ 0 ], +i_Minko[ 0 ]}, V_0), csl::tderivativeelement_s(V_0, d_der, {i_Minko[ 0 ]}, csl::GetComplexConjugate(c_G({+i_SU3_c_1_1[ 2 ]}, V_0)), 0), c_G({+i_SU3_c_1_1[ 1 ]}, V_0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({g, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), T_SU3_c_10_SU3_c_1_0({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_0[ 0 ], +i_SU3_c_1_0[ 1 ]})
, G({+i_SU3_c_1_1[ 0 ], i_Minko[ 0 ]}, V_0), csl::GetComplexConjugate(u({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, V_0)), u({+i_SU3_c_1_0[ 1 ], +i_dirac[ 1 ]}, V_0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({g, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), T_SU3_c_10_SU3_c_1_0({+i_SU3_c_1_1[ 0 ], +i_SU3_c_1_0[ 0 ], +i_SU3_c_1_0[ 1 ]})
, G({+i_SU3_c_1_1[ 0 ], i_Minko[ 0 ]}, V_0), csl::GetComplexConjugate(d({+i_SU3_c_1_0[ 0 ], +i_dirac[ 0 ]}, V_0)), d({+i_SU3_c_1_0[ 1 ], +i_dirac[ 1 ]}, V_0)}),
        false);
    model->addLagrangianTerm(
        csl::prod_s({g, gamma({+i_Minko[ 0 ], +i_dirac[ 0 ], +i_dirac[ 1 ]}), T_SU3_c_20_SU3_c_2_0({+i_SU3_c_1_1[ 0 ], +i_SU3_c_2_0[ 0 ], +i_SU3_c_2_0[ 1 ]})
, G({+i_SU3_c_1_1[ 0 ], i_Minko[ 0 ]}, V_0), csl::GetComplexConjugate(X({+i_SU3_c_2_0[ 0 ], +i_dirac[ 0 ]}, V_0)), X({+i_SU3_c_2_0[ 1 ], +i_dirac[ 1 ]}, V_0)}),
        false);
    return model;
}

inline mty::Model &get_QCD()
{
    static std::unique_ptr<mty::Model> model = build_QCD();
    return *model;
}

} // End of namespace mty

#endif