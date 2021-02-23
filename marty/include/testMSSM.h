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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#ifndef DEFINITION_MSSM_H_INCLUDED
#define DEFINITION_MSSM_H_INCLUDED

#include "marty.h"

namespace mty {

inline std::unique_ptr<mty::Model> build_mssm()
{
    std::unique_ptr<mty::Model> model;
    mty::Model &_model_ = *model;
    csl::Tensor gamma  = mty::dirac4.gamma;
    csl::Tensor gamma5 = mty::dirac4.gamma_chir;
    csl::Tensor P_L    = mty::dirac4.P_L;
    csl::Tensor P_R    = mty::dirac4.P_R;
    csl::Tensor sigma  = mty::dirac4.sigma;
    
    
    ///////////////////////////////////
    // Gauge definition              //
    ///////////////////////////////////
    
    
    csl::Expr gY = csl::constant_s("gY");
    csl::Expr gL = csl::constant_s("gL");
    csl::Expr g_s = csl::constant_s("g_s");
    model->addGaugedGroup(
        mty::group::Type::U1,
        "U1Y",
        1,
        gY
        );
    model->addGaugedGroup(
        mty::group::Type::SU,
        "SU2L",
        2,
        gL
        );
    model->addGaugedGroup(
        mty::group::Type::SU,
        "SU3c",
        3,
        g_s
        );
    mty::GaugedGroup *U1Y = model->getGauge()->getGaugedGroup(0);
    mty::GaugedGroup *SU2L = model->getGauge()->getGaugedGroup(1);
    mty::GaugedGroup *SU3c = model->getGauge()->getGaugedGroup(2);
    
    
    
    ///////////////////////////////////
    // Flavor definition             //
    ///////////////////////////////////
    
    
    model->addFlavorGroup(
        "SM_flavor", 3, 1);
    mty::FlavorGroup *SM_flavor = (*model->getFlavor())[0];
    
    model->init(false);
    
    
    ///////////////////////////////////
    // Particle masses definitions   //
    ///////////////////////////////////
    
    
    csl::Expr _beta = csl::constant_s("\\beta");
    csl::Expr v1 = csl::constant_s("v1");
    csl::Expr m_W_encaps = csl::intfraction_s(1, 2) * gL * v1 * csl::pow_s((1 + csl::pow_s(csl::tan_s(_beta), 2)), csl::intfraction_s(1, 2));
    csl::Expr m_W = csl::Abbrev::makeAbbreviation("m_W", m_W_encaps);
    csl::Expr _alpha = csl::constant_s("\\alpha");
    csl::Expr m__12_ = csl::constant_s("m_{12}");
    csl::Expr _lambda__2 = csl::constant_s("\\lambda _2");
    csl::Expr _lambda__5 = csl::constant_s("\\lambda _5");
    csl::Expr _lambda__4 = csl::constant_s("\\lambda _4");
    csl::Expr _lambda__3 = csl::constant_s("\\lambda _3");
    csl::Expr _lambda__1 = csl::constant_s("\\lambda _1");
    csl::Expr m_h_0_encaps = csl::pow_s((2 * csl::pow_s(m__12_, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) + _lambda__1 * csl::pow_s(v1, 2) * csl::pow_s(csl::sin_s(_alpha), 2) + (-2) * _lambda__3 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__4 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__5 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + _lambda__2 * csl::pow_s(v1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::cos_s(_alpha), 2) / csl::tan_s(_beta)), csl::intfraction_s(1, 2));
    csl::Expr m_h_0 = csl::Abbrev::makeAbbreviation("m_h^0", m_h_0_encaps);
    csl::Expr m_H_0_encaps = csl::pow_s((_lambda__1 * csl::pow_s(v1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) + (-2) * csl::pow_s(m__12_, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::tan_s(_beta) + 2 * _lambda__3 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + 2 * _lambda__4 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + 2 * _lambda__5 * csl::pow_s(v1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__2 * csl::pow_s(v1, 2) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::sin_s(_alpha), 2) / csl::tan_s(_beta)), csl::intfraction_s(1, 2));
    csl::Expr m_H_0 = csl::Abbrev::makeAbbreviation("m_H^0", m_H_0_encaps);
    csl::Expr e_em = csl::constant_s("e_em");
    csl::Expr m_B_mix_encaps = csl::intfraction_s(1, 2) * gL * gY * csl::pow_s((csl::pow_s(gL, 2) + csl::pow_s(gY, 2)), csl::intfraction_s(-1, 2)) * v1 * csl::pow_s((1 + csl::pow_s(csl::tan_s(_beta), 2)), csl::intfraction_s(1, 2));
    csl::Expr m_B_mix = csl::Abbrev::makeAbbreviation("m_B_mix", m_B_mix_encaps);
    csl::Expr m_A_encaps = csl::pow_s((csl::pow_s(m_B_mix, 2) + csl::intfraction_s(-1, 4) * csl::pow_s(e_em, 2) * csl::pow_s(v1, 2) + csl::intfraction_s(-1, 4) * csl::pow_s(e_em, 2) * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 2)), csl::intfraction_s(1, 2));
    csl::Expr m_A = csl::Abbrev::makeAbbreviation("m_A", m_A_encaps);
    csl::Expr theta_W = csl::constant_s("theta_W");
    csl::Expr m_Z_encaps = e_em * v1 * csl::pow_s((csl::intfraction_s(1, 2) + csl::intfraction_s(1, 4) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 4) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::intfraction_s(1, 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(1, 4) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(1, 4) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::pow_s(csl::tan_s(_beta), 2)), csl::intfraction_s(1, 2));
    csl::Expr m_Z = csl::Abbrev::makeAbbreviation("m_Z", m_Z_encaps);
    csl::Expr v = csl::constant_s("v");
    csl::Expr m_H___encaps = v1 * csl::pow_s((csl::intfraction_s(-1, 2) * _lambda__4 * csl::pow_s(v1, 2) + csl::intfraction_s(-1, 2) * _lambda__5 * csl::pow_s(v1, 2) + 2 * csl::pow_s(m__12_, 2) * csl::tan_s(_beta) + -_lambda__4 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::tan_s(_beta), 3) + csl::intfraction_s(-1, 2) * _lambda__4 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 4) + csl::intfraction_s(-1, 2) * _lambda__5 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 4) + csl::pow_s(m__12_, 2) / csl::tan_s(_beta)), csl::intfraction_s(1, 2)) / v;
    csl::Expr m_H__ = csl::Abbrev::makeAbbreviation("m_H^+", m_H___encaps);
    csl::Expr m_A_0_encaps = v1 * csl::pow_s((-_lambda__5 * csl::pow_s(v1, 2) + 2 * csl::pow_s(m__12_, 2) * csl::tan_s(_beta) + (-2) * _lambda__5 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::pow_s(m__12_, 2) * csl::pow_s(csl::tan_s(_beta), 3) + -_lambda__5 * csl::pow_s(v1, 2) * csl::pow_s(csl::tan_s(_beta), 4) + csl::pow_s(m__12_, 2) / csl::tan_s(_beta)), csl::intfraction_s(1, 2)) / v;
    csl::Expr m_A_0 = csl::Abbrev::makeAbbreviation("m_A^0", m_A_0_encaps);
    csl::Expr m_u = csl::constant_s("m_u");
    csl::Expr m_c = csl::constant_s("m_c");
    csl::Expr m_t = csl::constant_s("m_t");
    csl::Expr m_d = csl::constant_s("m_d");
    csl::Expr m_s = csl::constant_s("m_s");
    csl::Expr m_b = csl::constant_s("m_b");
    csl::Expr m_e = csl::constant_s("m_e");
    csl::Expr m__mu = csl::constant_s("m_\\mu");
    csl::Expr m__tau = csl::constant_s("m_\\tau");
    csl::Expr m_G___encaps = csl::pow_s((-1), csl::intfraction_s(1, 2)) * m_W;
    csl::Expr m_G__ = csl::Abbrev::makeAbbreviation("m_G^+", m_G___encaps);
    
    
    ///////////////////////////////////
    // Particle definitions          //
    ///////////////////////////////////
    
    
    mty::Particle g     = SU3c->buildVectorBoson();
    g->setSelfConjugate(true);
    g->setGroupRep("SU3c", {1, 1});
    mty::Particle F_g     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(g.get()));
    
    mty::Particle W     = SU2L->buildVectorBoson();
    W->setMass(m_W);
    mty::Particle F_W     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(W.get()));
    
    mty::Particle A     = U1Y->buildVectorBoson();
    A->setSelfConjugate(true);
    A->setMass(m_A);
    mty::Particle F_A     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(A.get()));
    
    mty::Particle Z     = U1Y->buildVectorBoson();
    Z->setSelfConjugate(true);
    Z->setMass(m_Z);
    mty::Particle F_Z     = mty::fieldstrength_s(dynamic_cast<mty::VectorBoson*>(Z.get()));
    
    mty::Particle c_g = g->getGaugedGroup()->buildGhost();
    
    mty::Particle h_0 = mty::scalarboson_s("h^0", _model_);
    h_0->setSelfConjugate(true);
    h_0->setMass(m_h_0);
    
    mty::Particle H_0 = mty::scalarboson_s("H^0", _model_);
    H_0->setSelfConjugate(true);
    H_0->setMass(m_H_0);
    
    mty::Particle H__ = mty::scalarboson_s("H^+", _model_);
    H__->setMass(m_H__);
    
    mty::Particle A_0 = mty::scalarboson_s("A^0", _model_);
    A_0->setSelfConjugate(true);
    A_0->setMass(m_A_0);
    
    mty::Particle G__ = mty::goldstoneboson_s("G^+", std::dynamic_pointer_cast<mty::GaugeBoson>(W));
    
    mty::Particle G_0 = mty::goldstoneboson_s("G^0", std::dynamic_pointer_cast<mty::GaugeBoson>(Z));
    
    mty::Particle u_L = mty::weylfermion_s("u_L", _model_, Chirality::Left);
    u_L->setMass(m_u);
    u_L->setGroupRep("U1Y", {1, 6});
    u_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle c_L = mty::weylfermion_s("c_L", _model_, Chirality::Left);
    c_L->setMass(m_c);
    c_L->setGroupRep("U1Y", {1, 6});
    c_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle t_L = mty::weylfermion_s("t_L", _model_, Chirality::Left);
    t_L->setMass(m_t);
    t_L->setGroupRep("U1Y", {1, 6});
    t_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle u_R = mty::weylfermion_s("u_R", _model_, Chirality::Right);
    u_R->setMass(m_u);
    u_R->setGroupRep("U1Y", {2, 3});
    u_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle c_R = mty::weylfermion_s("c_R", _model_, Chirality::Right);
    c_R->setMass(m_c);
    c_R->setGroupRep("U1Y", {2, 3});
    c_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle t_R = mty::weylfermion_s("t_R", _model_, Chirality::Right);
    t_R->setMass(m_t);
    t_R->setGroupRep("U1Y", {2, 3});
    t_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle d_L = mty::weylfermion_s("d_L", _model_, Chirality::Left);
    d_L->setMass(m_d);
    d_L->setGroupRep("U1Y", {1, 6});
    d_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle s_L = mty::weylfermion_s("s_L", _model_, Chirality::Left);
    s_L->setMass(m_s);
    s_L->setGroupRep("U1Y", {1, 6});
    s_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle b_L = mty::weylfermion_s("b_L", _model_, Chirality::Left);
    b_L->setMass(m_b);
    b_L->setGroupRep("U1Y", {1, 6});
    b_L->setGroupRep("SU3c", {1, 0});
    
    mty::Particle d_R = mty::weylfermion_s("d_R", _model_, Chirality::Right);
    d_R->setMass(m_d);
    d_R->setGroupRep("U1Y", {-1, 3});
    d_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle s_R = mty::weylfermion_s("s_R", _model_, Chirality::Right);
    s_R->setMass(m_s);
    s_R->setGroupRep("U1Y", {-1, 3});
    s_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle b_R = mty::weylfermion_s("b_R", _model_, Chirality::Right);
    b_R->setMass(m_b);
    b_R->setGroupRep("U1Y", {-1, 3});
    b_R->setGroupRep("SU3c", {1, 0});
    
    mty::Particle e_L = mty::weylfermion_s("e_L", _model_, Chirality::Left);
    e_L->setMass(m_e);
    e_L->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle _mu_L = mty::weylfermion_s("\\mu_L", _model_, Chirality::Left);
    _mu_L->setMass(m__mu);
    _mu_L->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle _tau_L = mty::weylfermion_s("\\tau_L", _model_, Chirality::Left);
    _tau_L->setMass(m__tau);
    _tau_L->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle e_R = mty::weylfermion_s("e_R", _model_, Chirality::Right);
    e_R->setMass(m_e);
    e_R->setGroupRep("U1Y", {-1, 1});
    
    mty::Particle _mu_R = mty::weylfermion_s("\\mu_R", _model_, Chirality::Right);
    _mu_R->setMass(m__mu);
    _mu_R->setGroupRep("U1Y", {-1, 1});
    
    mty::Particle _tau_R = mty::weylfermion_s("\\tau_R", _model_, Chirality::Right);
    _tau_R->setMass(m__tau);
    _tau_R->setGroupRep("U1Y", {-1, 1});
    
    mty::Particle _nu__eL_ = mty::weylfermion_s("\\nu_{eL}", _model_, Chirality::Left);
    _nu__eL_->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle _nu___mu_L_ = mty::weylfermion_s("\\nu_{\\mu L}", _model_, Chirality::Left);
    _nu___mu_L_->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle _nu___tau_L_ = mty::weylfermion_s("\\nu_{\\tau L}", _model_, Chirality::Left);
    _nu___tau_L_->setGroupRep("U1Y", {-1, 2});
    
    mty::Particle u     = mty::diracfermion_s(u_R, u_L);
    
    mty::Particle c     = mty::diracfermion_s(c_R, c_L);
    
    mty::Particle t     = mty::diracfermion_s(t_R, t_L);
    
    mty::Particle d     = mty::diracfermion_s(d_L, d_R);
    
    mty::Particle s     = mty::diracfermion_s(s_L, s_R);
    
    mty::Particle b     = mty::diracfermion_s(b_L, b_R);
    
    mty::Particle e     = mty::diracfermion_s(e_L, e_R);
    
    mty::Particle _mu     = mty::diracfermion_s(_mu_L, _mu_R);
    
    mty::Particle _tau     = mty::diracfermion_s(_tau_L, _tau_R);
    
    
    
    ///////////////////////////////////
    // Kinetic terms                 //
    ///////////////////////////////////
    
    
    model->addTerm(
        csl::intfraction_s(-1, 4) * g({+h_253, +_rho_1806, +_sigma_669}, X_0) * g({+h_253, _rho_1806, _sigma_669}, X_0),
        false);
    model->addTerm(
        csl::tderivativeelement_s(X_0, d, {_lambda_25974}, csl::GetComplexConjugate(c_g({+e_130}, X_0)), 0) * csl::tderivativeelement_s(X_0, d, {+_lambda_25974}, c_g({+e_130}, X_0), 0),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 2) * W({+_mu_4619, +_nu_4602}, X_0) * csl::GetComplexConjugate(W({_mu_4619, _nu_4602}, X_0)),
        false);
    model->addTerm(
        csl::intfraction_s(1, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2)) * csl::tderivativeelement_s(X_0, d, {+_lambda_25978}, H_0(X_0), 0) * csl::tderivativeelement_s(X_0, d, {_lambda_25978}, H_0(X_0), 0),
        false);
    model->addTerm(
        csl::intfraction_s(1, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2)) * csl::tderivativeelement_s(X_0, d, {+_lambda_25982}, h_0(X_0), 0) * csl::tderivativeelement_s(X_0, d, {_lambda_25982}, h_0(X_0), 0),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 4) * A({_mu_4623, _nu_4606}, X_0) * A({+_mu_4623, +_nu_4606}, X_0),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 4) * Z({_mu_4627, _nu_4610}, X_0) * Z({+_mu_4627, +_nu_4610}, X_0),
        false);
    model->addTerm(
        csl::tderivativeelement_s(X_0, d, {+_lambda_25986}, H__(X_0), 0) * csl::tderivativeelement_s(X_0, d, {_lambda_25986}, csl::GetComplexConjugate(H__(X_0)), 0),
        false);
    model->addTerm(
        csl::intfraction_s(1, 2) * csl::tderivativeelement_s(X_0, d, {+_lambda_25990}, A_0(X_0), 0) * csl::tderivativeelement_s(X_0, d, {_lambda_25990}, A_0(X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_tau_17551, +gam_7973, +del_4270}) * csl::GetComplexConjugate(u({+A_5400, +gam_7973}, X_0)) * csl::tderivativeelement_s(X_0, d, {_tau_17551}, u({+A_5400, +del_4270}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_tau_17555, +gam_7977, +del_4274}) * csl::GetComplexConjugate(c({+A_5404, +gam_7977}, X_0)) * csl::tderivativeelement_s(X_0, d, {_tau_17555}, c({+A_5404, +del_4274}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_tau_17559, +gam_7981, +del_4278}) * csl::GetComplexConjugate(t({+A_5408, +gam_7981}, X_0)) * csl::tderivativeelement_s(X_0, d, {_tau_17559}, t({+A_5408, +del_4278}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1810, +gam_7985, +del_4282}) * csl::GetComplexConjugate(e({+gam_7985}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1810}, e({+del_4282}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1814, +gam_7989, +del_4286}) * csl::GetComplexConjugate(_mu({+gam_7989}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1814}, _mu({+del_4286}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1818, +gam_7993, +del_4290}) * csl::GetComplexConjugate(_tau({+gam_7993}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1818}, _tau({+del_4290}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1822, +gam_7997, +del_4294}) * csl::GetComplexConjugate(_nu__eL_({+gam_7997}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1822}, _nu__eL_({+del_4294}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1826, +gam_8001, +del_4298}) * csl::GetComplexConjugate(_nu___mu_L_({+gam_8001}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1826}, _nu___mu_L_({+del_4298}, X_0), 0),
        false);
    model->addTerm(
        CSL_I * gamma({+_rho_1830, +gam_8005, +del_4302}) * csl::GetComplexConjugate(_nu___tau_L_({+gam_8005}, X_0)) * csl::tderivativeelement_s(X_0, d, {_rho_1830}, _nu___tau_L_({+del_4302}, X_0), 0),
        false);
    model->addTerm(
        -csl::tderivativeelement_s(X_0, d, {+_nu_4615}, csl::GetComplexConjugate(W({_nu_2723}, X_0)), 0) * csl::tderivativeelement_s(X_0, d, {+_rho_1835}, W({_rho_547}, X_0), 0),
        false);
    model->addTerm(
        -m_W * csl::GetComplexConjugate(W({_nu_4618}, X_0)) * csl::tderivativeelement_s(X_0, d, {+_nu_4618}, G__(X_0), 0),
        false);
    model->addTerm(
        -m_W * W({_nu_4622}, X_0) * csl::tderivativeelement_s(X_0, d, {+_nu_4622}, csl::GetComplexConjugate(G__(X_0)), 0),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 2) * csl::tderivativeelement_s(X_0, d, {+_sigma_674}, Z({_sigma_142}, X_0), 0) * csl::tderivativeelement_s(X_0, d, {+_lambda_25995}, Z({_lambda_21045}, X_0), 0),
        false);
    
    
    ///////////////////////////////////
    // Mass terms                    //
    ///////////////////////////////////
    
    
    model->addTerm(
        csl::pow_s(m_W, 2) * W({+_tau_17563}, X) * csl::GetComplexConjugate(W({_tau_17563}, X)),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(m_H_0, 2) * csl::pow_s(H_0(X), 2),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(m_h_0, 2) * csl::pow_s(h_0(X), 2),
        false);
    model->addTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(m_A, 2) * A({+_mu_4631}, X) * A({_mu_4631}, X),
        false);
    model->addTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(m_Z, 2) * Z({+_nu_4626}, X) * Z({_nu_4626}, X),
        false);
    model->addTerm(
        -csl::pow_s(m_H__, 2) * H__(X) * csl::GetComplexConjugate(H__(X)),
        false);
    model->addTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(m_A_0, 2) * csl::pow_s(A_0(X), 2),
        false);
    model->addTerm(
        -m_u * csl::GetComplexConjugate(u({+C_1843, +alpha_3492}, X_0)) * u({+C_1843, +alpha_3492}, X_0),
        false);
    model->addTerm(
        -m_c * csl::GetComplexConjugate(c({+C_1847, +alpha_3496}, X_0)) * c({+C_1847, +alpha_3496}, X_0),
        false);
    model->addTerm(
        -m_t * csl::GetComplexConjugate(t({+C_1851, +alpha_3500}, X_0)) * t({+C_1851, +alpha_3500}, X_0),
        false);
    model->addTerm(
        -m_d * csl::GetComplexConjugate(d_L({+A_5412, +gam_8009}, X_0)) * d_R({+A_5412, +gam_8009}, X),
        false);
    model->addTerm(
        -m_s * csl::GetComplexConjugate(s_L({+A_5416, +gam_8013}, X_0)) * s_R({+A_5416, +gam_8013}, X),
        false);
    model->addTerm(
        -m_b * csl::GetComplexConjugate(b_L({+A_5420, +gam_8017}, X_0)) * b_R({+A_5420, +gam_8017}, X),
        false);
    model->addTerm(
        -m_d * csl::GetComplexConjugate(d_R({+E_1646, +gam_8021}, X_0)) * d_L({+E_1646, +gam_8021}, X_0),
        false);
    model->addTerm(
        -m_s * csl::GetComplexConjugate(s_R({+E_1650, +gam_8025}, X_0)) * s_L({+E_1650, +gam_8025}, X_0),
        false);
    model->addTerm(
        -m_b * csl::GetComplexConjugate(b_R({+E_1654, +gam_8029}, X_0)) * b_L({+E_1654, +gam_8029}, X_0),
        false);
    model->addTerm(
        -m_e * csl::GetComplexConjugate(e({+eps_3488}, X_0)) * e({+eps_3488}, X_0),
        false);
    model->addTerm(
        -m__mu * csl::GetComplexConjugate(_mu({+eps_3492}, X_0)) * _mu({+eps_3492}, X_0),
        false);
    model->addTerm(
        -m__tau * csl::GetComplexConjugate(_tau({+eps_3496}, X_0)) * _tau({+eps_3496}, X_0),
        false);
    model->addTerm(
        -csl::pow_s(m_G__, 2) * G__(X) * csl::GetComplexConjugate(G__(X)),
        false);
    
    
    ///////////////////////////////////
    // Interaction terms             //
    ///////////////////////////////////
    
    
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * g_s * f_SU3c({+a_124, +b_124, +h_257}) * g({+h_257, _rho_1838, _sigma_677}, X_0) * g({+a_124, +_rho_1838}, X_0) * g({+b_124, +_sigma_677}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 4) * csl::pow_s(g_s, 2) * f_SU3c({+a_128, +b_128, +h_260}) * f_SU3c({+c_66, +d_66, +h_260}) * g({+a_128, +_rho_1842}, X_0) * g({+b_128, +_sigma_681}, X_0) * g({+c_66, _rho_1842}, X_0) * g({+d_66, _sigma_681}, X_0),
        false);
    model->addLagrangianTerm(
        CSL_I * g_s * f_SU3c({+e_134, +f_419, +g_109}) * g({+f_419, +_lambda_25998}, X_0) * c_g({+g_109}, X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_25998}, csl::GetComplexConjugate(c_g({+e_134}, X_0)), 0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * v1 * (_lambda__1 * csl::pow_s(csl::cos_s(_alpha), 3) + _lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__2 * csl::pow_s(csl::sin_s(_alpha), 3) * csl::tan_s(_beta)) * csl::pow_s(H_0(X_0), 3),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(3, 2) * v1 * (_lambda__1 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + csl::intfraction_s(-2, 3) * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + csl::intfraction_s(-2, 3) * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + csl::intfraction_s(-2, 3) * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + csl::intfraction_s(1, 3) * _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 3) + csl::intfraction_s(1, 3) * _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 3) + csl::intfraction_s(1, 3) * _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 3) + csl::intfraction_s(-1, 3) * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 3) * csl::tan_s(_beta) + csl::intfraction_s(-1, 3) * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 3) * csl::tan_s(_beta) + csl::intfraction_s(-1, 3) * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 3) * csl::tan_s(_beta) + -_lambda__2 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + csl::intfraction_s(2, 3) * _lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + csl::intfraction_s(2, 3) * _lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + csl::intfraction_s(2, 3) * _lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta)) * csl::pow_s(H_0(X_0), 2) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * v1 * (_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 3) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 3) + _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 3) + 3 * _lambda__1 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + (-2) * _lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + (-2) * _lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + (-2) * _lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) + 3 * _lambda__2 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 3) * csl::tan_s(_beta) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 3) * csl::tan_s(_beta) + _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 3) * csl::tan_s(_beta)) * H_0(X_0) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * v1 * (_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::sin_s(_alpha) + _lambda__1 * csl::pow_s(csl::sin_s(_alpha), 3) + -_lambda__2 * csl::pow_s(csl::cos_s(_alpha), 3) * csl::tan_s(_beta) + -_lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + -_lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + -_lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta)) * csl::pow_s(h_0(X_0), 3),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 8) * (_lambda__1 * csl::pow_s(csl::cos_s(_alpha), 4) + 2 * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + 2 * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + 2 * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__2 * csl::pow_s(csl::sin_s(_alpha), 4)) * csl::pow_s(H_0(X_0), 4),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * (_lambda__1 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__2 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2)) * csl::pow_s(H_0(X_0), 3) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 4) * (_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 4) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 4) + _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 4) + 3 * _lambda__1 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + 3 * _lambda__2 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + (-4) * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + (-4) * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + (-4) * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 4) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 4) + _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 4)) * csl::pow_s(H_0(X_0), 2) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::cos_s(_alpha) * csl::sin_s(_alpha) * (_lambda__2 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__1 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2)) * H_0(X_0) * csl::pow_s(h_0(X_0), 3),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 8) * (_lambda__2 * csl::pow_s(csl::cos_s(_alpha), 4) + 2 * _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + 2 * _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + 2 * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__1 * csl::pow_s(csl::sin_s(_alpha), 4)) * csl::pow_s(h_0(X_0), 4),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(e_em, 2) * A({+_nu_4630}, X_0) * A({_nu_4630}, X_0) * W({_mu_4635}, X_0) * csl::GetComplexConjugate(W({+_mu_4635}, X_0)) + csl::pow_s(e_em, 2) * A({_mu_4635}, X_0) * A({+_nu_4630}, X_0) * csl::GetComplexConjugate(W({+_mu_4635}, X_0)) * W({_nu_4630}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * A({_mu_4640}, X_0) * csl::GetComplexConjugate(W({+_mu_4640, +_nu_4636}, X_0)) * W({_nu_4636}, X_0) + csl::intfraction_s(-1, 2) * CSL_I * e_em * A({_mu_4640}, X_0) * csl::GetComplexConjugate(W({+_nu_4636, +_mu_4640}, X_0)) * W({_nu_4636}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * A({_mu_4645}, X_0) * W({+_mu_4645, +_nu_4641}, X_0) * csl::GetComplexConjugate(W({_nu_4641}, X_0)) + csl::intfraction_s(1, 2) * CSL_I * e_em * A({_mu_4645}, X_0) * W({+_nu_4641, +_mu_4645}, X_0) * csl::GetComplexConjugate(W({_nu_4641}, X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * A({+_mu_4650, +_nu_4646}, X_0) * W({_mu_4650}, X_0) * csl::GetComplexConjugate(W({_nu_4646}, X_0)) + csl::intfraction_s(-1, 2) * CSL_I * e_em * A({+_nu_4646, +_mu_4650}, X_0) * W({_mu_4650}, X_0) * csl::GetComplexConjugate(W({_nu_4646}, X_0)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(e_em, 2) * A({+_lambda_26002}, X_0) * A({_lambda_26002}, X_0) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)),
        false);
    model->addLagrangianTerm(
        -CSL_I * e_em * A({+_lambda_26006}, X_0) * H__(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26006}, csl::GetComplexConjugate(H__(X_0)), 0),
        false);
    model->addLagrangianTerm(
        CSL_I * e_em * A({_lambda_26010}, X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::tderivativeelement_s(X_0, d, {+_lambda_26010}, H__(X_0), 0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * csl::pow_s(e_em, 2) * csl::GetComplexConjugate(W({+_lambda_26014}, X_0)) * Z({_lambda_26014}, X_0) * A_0(X_0) * H__(X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * csl::pow_s(e_em, 2) * W({+_lambda_26018}, X_0) * Z({_lambda_26018}, X_0) * A_0(X_0) * csl::GetComplexConjugate(H__(X_0)) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 8) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({+_mu_4655}, X_0) * csl::GetComplexConjugate(W({_mu_4655}, X_0)) * W({_nu_4651}, X_0) * W({+_nu_4651}, X_0) + csl::intfraction_s(-1, 8) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({+_mu_4655}, X_0) * W({_mu_4655}, X_0) * W({_nu_4651}, X_0) * csl::GetComplexConjugate(W({+_nu_4651}, X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::GetComplexConjugate(W({+_mu_4660}, X_0)) * csl::GetComplexConjugate(W({_mu_4660}, X_0)) * W({+_nu_4657}, X_0) * W({_nu_4657}, X_0) + csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_mu_4660}, X_0) * csl::GetComplexConjugate(W({+_mu_4660}, X_0)) * W({+_nu_4657}, X_0) * csl::GetComplexConjugate(W({_nu_4657}, X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 8) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::GetComplexConjugate(W({+_mu_4665}, X_0)) * csl::GetComplexConjugate(W({_mu_4665}, X_0)) * W({_nu_4663}, X_0) * csl::GetComplexConjugate(W({+_nu_4663}, X_0)) + csl::intfraction_s(1, 8) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_mu_4665}, X_0) * csl::GetComplexConjugate(W({+_mu_4665}, X_0)) * csl::GetComplexConjugate(W({_nu_4663}, X_0)) * csl::GetComplexConjugate(W({+_nu_4663}, X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * csl::pow_s(csl::sin_s(theta_W), (-2)) * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * W({_lambda_26022}, X_0) * csl::GetComplexConjugate(W({+_lambda_26022}, X_0)) * H_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * csl::pow_s(csl::sin_s(theta_W), (-2)) * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * W({_lambda_26026}, X_0) * csl::GetComplexConjugate(W({+_lambda_26026}, X_0)) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2)) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_lambda_26030}, X_0) * csl::GetComplexConjugate(W({+_lambda_26030}, X_0)) * csl::pow_s(H_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2)) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_lambda_26034}, X_0) * csl::GetComplexConjugate(W({+_lambda_26034}, X_0)) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_lambda_26038}, X_0) * csl::GetComplexConjugate(W({+_lambda_26038}, X_0)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_lambda_26042}, X_0) * csl::GetComplexConjugate(W({+_lambda_26042}, X_0)) * csl::pow_s(A_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * csl::GetComplexConjugate(W({+_lambda_26046}, X_0)) * H__(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26046}, A_0(X_0), 0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * W({+_lambda_26050}, X_0) * A_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26050}, csl::GetComplexConjugate(H__(X_0)), 0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * csl::GetComplexConjugate(W({_lambda_26054}, X_0)) * A_0(X_0) * csl::tderivativeelement_s(X_0, d, {+_lambda_26054}, H__(X_0), 0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * W({_lambda_26058}, X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::tderivativeelement_s(X_0, d, {+_lambda_26058}, A_0(X_0), 0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(e_em, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::GetComplexConjugate(W({+_mu_4670}, X_0)) * W({_nu_4669}, X_0) * Z({_mu_4670}, X_0) * Z({+_nu_4669}, X_0) + -csl::pow_s(e_em, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * W({_mu_4670}, X_0) * csl::GetComplexConjugate(W({+_mu_4670}, X_0)) * Z({+_nu_4669}, X_0) * Z({_nu_4669}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * csl::GetComplexConjugate(W({+_mu_4675, +_nu_4675}, X_0)) * W({_nu_4675}, X_0) * Z({_mu_4675}, X_0) / csl::sin_s(theta_W) + csl::intfraction_s(-1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * csl::GetComplexConjugate(W({+_nu_4675, +_mu_4675}, X_0)) * W({_nu_4675}, X_0) * Z({_mu_4675}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * W({+_mu_4680, +_nu_4680}, X_0) * csl::GetComplexConjugate(W({_nu_4680}, X_0)) * Z({_mu_4680}, X_0) / csl::sin_s(theta_W) + csl::intfraction_s(1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * W({+_nu_4680, +_mu_4680}, X_0) * csl::GetComplexConjugate(W({_nu_4680}, X_0)) * Z({_mu_4680}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * CSL_I * e_em * csl::cos_s(theta_W) * W({_mu_4685}, X_0) * W({_nu_4685}, X_0) * Z({+_mu_4685, +_nu_4685}, X_0) / csl::sin_s(theta_W) + csl::intfraction_s(-1, 4) * CSL_I * e_em * csl::cos_s(theta_W) * W({_mu_4685}, X_0) * W({_nu_4685}, X_0) * Z({+_nu_4685, +_mu_4685}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * W({_mu_4690}, X_0) * csl::GetComplexConjugate(W({_nu_4690}, X_0)) * Z({+_mu_4690, +_nu_4690}, X_0) / csl::sin_s(theta_W) + csl::intfraction_s(-1, 2) * CSL_I * e_em * csl::cos_s(theta_W) * W({_mu_4690}, X_0) * csl::GetComplexConjugate(W({_nu_4690}, X_0)) * Z({+_nu_4690, +_mu_4690}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 4) * CSL_I * e_em * csl::cos_s(theta_W) * csl::GetComplexConjugate(W({_mu_4695}, X_0)) * csl::GetComplexConjugate(W({_nu_4695}, X_0)) * Z({+_mu_4695, +_nu_4695}, X_0) / csl::sin_s(theta_W) + csl::intfraction_s(1, 4) * CSL_I * e_em * csl::cos_s(theta_W) * csl::GetComplexConjugate(W({_mu_4695}, X_0)) * csl::GetComplexConjugate(W({_nu_4695}, X_0)) * Z({+_nu_4695, +_mu_4695}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::cos_s(_alpha) + csl::intfraction_s(1, 2) * csl::cos_s(_alpha) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::cos_s(_alpha) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::sin_s(_alpha) * csl::tan_s(_beta) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::sin_s(_alpha) * csl::pow_s(csl::sin_s(theta_W), 2) * csl::tan_s(_beta) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::sin_s(_alpha) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::tan_s(_beta)) * Z({+_lambda_26062}, X_0) * Z({_lambda_26062}, X_0) * H_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::sin_s(_alpha) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::sin_s(_alpha) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::sin_s(_alpha) * csl::pow_s(csl::sin_s(theta_W), (-2)) + -csl::cos_s(_alpha) * csl::tan_s(_beta) + csl::intfraction_s(-1, 2) * csl::cos_s(_alpha) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) * csl::tan_s(_beta) + csl::intfraction_s(-1, 2) * csl::cos_s(_alpha) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::tan_s(_beta)) * Z({+_lambda_26066}, X_0) * Z({_lambda_26066}, X_0) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::sin_s(theta_W), (-2))) * Z({_lambda_26070}, X_0) * Z({+_lambda_26070}, X_0) * csl::pow_s(H_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * (csl::pow_s(csl::cos_s(_alpha), 2) + csl::pow_s(csl::sin_s(_alpha), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::sin_s(theta_W), (-2))) * Z({_lambda_26074}, X_0) * Z({+_lambda_26074}, X_0) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::pow_s(e_em, 2) * csl::cos_s(theta_W) * A({+_nu_4700}, X_0) * csl::GetComplexConjugate(W({+_mu_4700}, X_0)) * W({_nu_4700}, X_0) * Z({_mu_4700}, X_0) / csl::sin_s(theta_W) + csl::pow_s(e_em, 2) * csl::cos_s(theta_W) * A({+_nu_4700}, X_0) * W({+_mu_4700}, X_0) * csl::GetComplexConjugate(W({_nu_4700}, X_0)) * Z({_mu_4700}, X_0) / csl::sin_s(theta_W) + (-2) * csl::pow_s(e_em, 2) * csl::cos_s(theta_W) * A({+_nu_4700}, X_0) * W({_mu_4700}, X_0) * csl::GetComplexConjugate(W({+_mu_4700}, X_0)) * Z({_nu_4700}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * csl::pow_s(e_em, 2) * A({_lambda_26078}, X_0) * csl::GetComplexConjugate(W({+_lambda_26078}, X_0)) * A_0(X_0) * H__(X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * csl::pow_s(e_em, 2) * A({_lambda_26082}, X_0) * W({+_lambda_26082}, X_0) * A_0(X_0) * csl::GetComplexConjugate(H__(X_0)) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-4)) * csl::pow_s(v1, 4) * (_lambda__2 + 2 * _lambda__3 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__4 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__5 * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__1 * csl::pow_s(csl::tan_s(_beta), 4)) * csl::pow_s(H__(X_0), 2) * csl::pow_s(csl::GetComplexConjugate(H__(X_0)), 2),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(v, (-2)) * csl::pow_s(v1, 3) * (_lambda__3 * csl::cos_s(_alpha) + _lambda__2 * csl::sin_s(_alpha) * csl::tan_s(_beta) + -_lambda__4 * csl::sin_s(_alpha) * csl::tan_s(_beta) + -_lambda__5 * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)) * H_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::pow_s(v, (-2)) * csl::pow_s(v1, 3) * (_lambda__3 * csl::sin_s(_alpha) + -_lambda__2 * csl::cos_s(_alpha) * csl::tan_s(_beta) + _lambda__4 * csl::cos_s(_alpha) * csl::tan_s(_beta) + _lambda__5 * csl::cos_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__4 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) + _lambda__2 * csl::pow_s(csl::sin_s(_alpha), 2) + (-2) * _lambda__4 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::pow_s(H_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__2 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + -_lambda__3 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + -_lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::tan_s(_beta) + -_lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::tan_s(_beta) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + -_lambda__1 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)) * H_0(X_0) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__2 * csl::pow_s(csl::cos_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) + 2 * _lambda__4 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + 2 * _lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__1 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2)) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 8) * csl::pow_s(v, (-4)) * csl::pow_s(v1, 4) * (_lambda__2 + 2 * _lambda__3 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__4 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__5 * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__1 * csl::pow_s(csl::tan_s(_beta), 4)) * csl::pow_s(A_0(X_0), 4),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 3) * (_lambda__3 * csl::cos_s(_alpha) + _lambda__4 * csl::cos_s(_alpha) + -_lambda__5 * csl::cos_s(_alpha) + _lambda__2 * csl::sin_s(_alpha) * csl::tan_s(_beta) + (-2) * _lambda__5 * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + (-2) * _lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3) + _lambda__4 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3) + -_lambda__5 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3)) * csl::pow_s(A_0(X_0), 2) * H_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 3) * (_lambda__3 * csl::sin_s(_alpha) + _lambda__4 * csl::sin_s(_alpha) + -_lambda__5 * csl::sin_s(_alpha) + -_lambda__2 * csl::cos_s(_alpha) * csl::tan_s(_beta) + 2 * _lambda__5 * csl::cos_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + (-2) * _lambda__5 * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__3 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3) + -_lambda__4 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3) + _lambda__5 * csl::cos_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 3)) * csl::pow_s(A_0(X_0), 2) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 4) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) + -_lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) + _lambda__2 * csl::pow_s(csl::sin_s(_alpha), 2) + (-4) * _lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__1 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2)) * csl::pow_s(A_0(X_0), 2) * csl::pow_s(H_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__2 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + -_lambda__3 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + -_lambda__4 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + _lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) + (-2) * _lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::tan_s(_beta) + 2 * _lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::tan_s(_beta) + -_lambda__1 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__3 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__4 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::pow_s(csl::tan_s(_beta), 2)) * csl::pow_s(A_0(X_0), 2) * H_0(X_0) * h_0(X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 4) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (_lambda__2 * csl::pow_s(csl::cos_s(_alpha), 2) + _lambda__3 * csl::pow_s(csl::sin_s(_alpha), 2) + _lambda__4 * csl::pow_s(csl::sin_s(_alpha), 2) + -_lambda__5 * csl::pow_s(csl::sin_s(_alpha), 2) + 4 * _lambda__5 * csl::cos_s(_alpha) * csl::sin_s(_alpha) * csl::tan_s(_beta) + _lambda__3 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__4 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + -_lambda__5 * csl::pow_s(csl::cos_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__1 * csl::pow_s(csl::sin_s(_alpha), 2) * csl::pow_s(csl::tan_s(_beta), 2)) * csl::pow_s(A_0(X_0), 2) * csl::pow_s(h_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(v, (-4)) * csl::pow_s(v1, 4) * (_lambda__2 + 2 * _lambda__3 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__4 * csl::pow_s(csl::tan_s(_beta), 2) + 2 * _lambda__5 * csl::pow_s(csl::tan_s(_beta), 2) + _lambda__1 * csl::pow_s(csl::tan_s(_beta), 4)) * csl::pow_s(A_0(X_0), 2) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({+_lambda_26086}, X_0)) * Z({_lambda_26086}, X_0) * H__(X_0) * H_0(X_0) / (v * csl::cos_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({+_lambda_26090}, X_0)) * Z({_lambda_26090}, X_0) * H__(X_0) * h_0(X_0) / (v * csl::cos_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * W({+_lambda_26094}, X_0) * Z({_lambda_26094}, X_0) * csl::GetComplexConjugate(H__(X_0)) * H_0(X_0) / (v * csl::cos_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * W({+_lambda_26098}, X_0) * Z({_lambda_26098}, X_0) * csl::GetComplexConjugate(H__(X_0)) * h_0(X_0) / (v * csl::cos_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({+_lambda_26102}, X_0)) * H__(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26102}, H_0(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({+_lambda_26106}, X_0)) * H__(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26106}, h_0(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * W({+_lambda_26110}, X_0) * H_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26110}, csl::GetComplexConjugate(H__(X_0)), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * W({+_lambda_26114}, X_0) * h_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26114}, csl::GetComplexConjugate(H__(X_0)), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({_lambda_26118}, X_0)) * H_0(X_0) * csl::tderivativeelement_s(X_0, d, {+_lambda_26118}, H__(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * csl::GetComplexConjugate(W({_lambda_26122}, X_0)) * h_0(X_0) * csl::tderivativeelement_s(X_0, d, {+_lambda_26122}, H__(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * W({_lambda_26126}, X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::tderivativeelement_s(X_0, d, {+_lambda_26126}, H_0(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * W({_lambda_26130}, X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::tderivativeelement_s(X_0, d, {+_lambda_26130}, h_0(X_0), 0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(e_em, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W) + csl::sin_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::sin_s(theta_W)) * A({_lambda_26134}, X_0) * Z({+_lambda_26134}, X_0) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * csl::pow_s(e_em, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (1 + csl::intfraction_s(-1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(-1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(-1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(-1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::pow_s(csl::tan_s(_beta), 2)) * Z({+_lambda_26138}, X_0) * Z({_lambda_26138}, X_0) * H__(X_0) * csl::GetComplexConjugate(H__(X_0)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * CSL_I * e_em * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W) + csl::sin_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::sin_s(theta_W)) * Z({+_lambda_26142}, X_0) * H__(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26142}, csl::GetComplexConjugate(H__(X_0)), 0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * CSL_I * e_em * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W) + csl::sin_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) * csl::pow_s(csl::tan_s(_beta), 2) / csl::sin_s(theta_W)) * Z({_lambda_26146}, X_0) * csl::GetComplexConjugate(H__(X_0)) * csl::tderivativeelement_s(X_0, d, {+_lambda_26146}, H__(X_0), 0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * A({_lambda_26150}, X_0) * csl::GetComplexConjugate(W({+_lambda_26150}, X_0)) * H__(X_0) * H_0(X_0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * A({_lambda_26154}, X_0) * csl::GetComplexConjugate(W({+_lambda_26154}, X_0)) * H__(X_0) * h_0(X_0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::sin_s(_alpha) + -csl::cos_s(_alpha) * csl::tan_s(_beta)) * A({_lambda_26158}, X_0) * W({+_lambda_26158}, X_0) * csl::GetComplexConjugate(H__(X_0)) * H_0(X_0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * csl::pow_s(e_em, 2) * v1 * (csl::cos_s(_alpha) + csl::sin_s(_alpha) * csl::tan_s(_beta)) * A({_lambda_26162}, X_0) * W({+_lambda_26162}, X_0) * csl::GetComplexConjugate(H__(X_0)) * h_0(X_0) / (v * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 4) * csl::pow_s(e_em, 2) * csl::pow_s(v, (-2)) * csl::pow_s(v1, 2) * (1 + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) + csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), (-2)) * csl::pow_s(csl::sin_s(theta_W), 2) * csl::pow_s(csl::tan_s(_beta), 2) + csl::intfraction_s(1, 2) * csl::pow_s(csl::cos_s(theta_W), 2) * csl::pow_s(csl::sin_s(theta_W), (-2)) * csl::pow_s(csl::tan_s(_beta), 2)) * Z({+_lambda_26166}, X_0) * Z({_lambda_26166}, X_0) * csl::pow_s(A_0(X_0), 2),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * v1 * (csl::sin_s(_alpha) * csl::sin_s(theta_W) / csl::cos_s(theta_W) + csl::cos_s(theta_W) * csl::sin_s(_alpha) / csl::sin_s(theta_W) + -csl::cos_s(_alpha) * csl::tan_s(_beta) / (csl::cos_s(theta_W) * csl::sin_s(theta_W))) * Z({+_lambda_26170}, X_0) * A_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26170}, H_0(X_0), 0) / v,
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * v1 * (csl::cos_s(_alpha) * csl::sin_s(theta_W) / csl::cos_s(theta_W) + csl::cos_s(_alpha) * csl::cos_s(theta_W) / csl::sin_s(theta_W) + csl::sin_s(_alpha) * csl::tan_s(_beta) / (csl::cos_s(theta_W) * csl::sin_s(theta_W))) * Z({+_lambda_26174}, X_0) * A_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26174}, h_0(X_0), 0) / v,
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * v1 * (csl::sin_s(_alpha) * csl::sin_s(theta_W) / csl::cos_s(theta_W) + csl::cos_s(theta_W) * csl::sin_s(_alpha) / csl::sin_s(theta_W) + -csl::cos_s(_alpha) * csl::tan_s(_beta) / (csl::cos_s(theta_W) * csl::sin_s(theta_W))) * Z({+_lambda_26178}, X_0) * H_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26178}, A_0(X_0), 0) / v,
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * v1 * (csl::cos_s(_alpha) * csl::sin_s(theta_W) / csl::cos_s(theta_W) + csl::cos_s(_alpha) * csl::cos_s(theta_W) / csl::sin_s(theta_W) + csl::sin_s(_alpha) * csl::tan_s(_beta) / (csl::cos_s(theta_W) * csl::sin_s(theta_W))) * Z({+_lambda_26182}, X_0) * h_0(X_0) * csl::tderivativeelement_s(X_0, d, {_lambda_26182}, A_0(X_0), 0) / v,
        false);
    model->addLagrangianTerm(
        g_s * T({+f_423, +A_5424, +B_361}) * gamma({+_tau_17567, +gam_8033, +del_4306}) * csl::GetComplexConjugate(u({+A_5424, +gam_8033}, X_0)) * g({+f_423, _tau_17567}, X_0) * u({+B_361, +del_4306}, X_0),
        false);
    model->addLagrangianTerm(
        g_s * T({+f_427, +A_5428, +B_365}) * gamma({+_tau_17571, +gam_8037, +del_4310}) * csl::GetComplexConjugate(c({+A_5428, +gam_8037}, X_0)) * c({+B_365, +del_4310}, X_0) * g({+f_427, _tau_17571}, X_0),
        false);
    model->addLagrangianTerm(
        g_s * T({+f_431, +A_5432, +B_369}) * gamma({+_tau_17575, +gam_8041, +del_4314}) * csl::GetComplexConjugate(t({+A_5432, +gam_8041}, X_0)) * g({+f_431, _tau_17575}, X_0) * t({+B_369, +del_4314}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(2, 3) * e_em * gamma({+_tau_17579, +gam_8045, +del_4318}) * csl::GetComplexConjugate(u({+A_5436, +gam_8045}, X_0)) * A({_tau_17579}, X_0) * u({+A_5436, +del_4318}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(2, 3) * e_em * gamma({+_tau_17583, +gam_8049, +del_4322}) * csl::GetComplexConjugate(c({+A_5440, +gam_8049}, X_0)) * A({_tau_17583}, X_0) * c({+A_5440, +del_4322}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(2, 3) * e_em * gamma({+_tau_17587, +gam_8053, +del_4326}) * csl::GetComplexConjugate(t({+A_5444, +gam_8053}, X_0)) * A({_tau_17587}, X_0) * t({+A_5444, +del_4326}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 6) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + (-3) * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17591, +gam_8057, +del_4330}) * csl::GetComplexConjugate(u_L({+A_5448, +gam_8057}, X_0)) * Z({_tau_17591}, X_0) * u_L({+A_5448, +del_4330}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 6) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + (-3) * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17595, +gam_8061, +del_4334}) * csl::GetComplexConjugate(c_L({+A_5452, +gam_8061}, X_0)) * Z({_tau_17595}, X_0) * c_L({+A_5452, +del_4334}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 6) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + (-3) * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17599, +gam_8065, +del_4338}) * csl::GetComplexConjugate(t_L({+A_5456, +gam_8065}, X_0)) * Z({_tau_17599}, X_0) * t_L({+A_5456, +del_4338}, X_0),
        false);
    model->addLagrangianTerm(
        -m_u * csl::sin_s(_alpha) * csl::GetComplexConjugate(u({+C_1855, +alpha_3504}, X_0)) * u({+C_1855, +alpha_3504}, X_0) * H_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_c * csl::sin_s(_alpha) * csl::GetComplexConjugate(c({+C_1859, +alpha_3508}, X_0)) * c({+C_1859, +alpha_3508}, X_0) * H_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_t * csl::sin_s(_alpha) * csl::GetComplexConjugate(t({+C_1863, +alpha_3512}, X_0)) * t({+C_1863, +alpha_3512}, X_0) * H_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_u * csl::cos_s(_alpha) * csl::GetComplexConjugate(u({+C_1867, +alpha_3516}, X_0)) * u({+C_1867, +alpha_3516}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_c * csl::cos_s(_alpha) * csl::GetComplexConjugate(c({+C_1871, +alpha_3520}, X_0)) * c({+C_1871, +alpha_3520}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_t * csl::cos_s(_alpha) * csl::GetComplexConjugate(t({+C_1875, +alpha_3524}, X_0)) * t({+C_1875, +alpha_3524}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ud_mod * e_em * gamma({+_tau_17603, +gam_8069, +del_4342}) * csl::GetComplexConjugate(d_L({+A_5460, +gam_8069}, X_0)) * csl::GetComplexConjugate(W({_tau_17603}, X_0)) * u_L({+A_5460, +del_4342}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cd_mod * e_em * csl::exp_s(-CSL_I * delta_wolf) * gamma({+_tau_17607, +gam_8073, +del_4346}) * csl::GetComplexConjugate(d_L({+A_5464, +gam_8073}, X_0)) * csl::GetComplexConjugate(W({_tau_17607}, X_0)) * c_L({+A_5464, +del_4346}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_td_mod * e_em * csl::exp_s(-CSL_I * delta_wolf) * gamma({+_tau_17611, +gam_8077, +del_4350}) * csl::GetComplexConjugate(d_L({+A_5468, +gam_8077}, X_0)) * csl::GetComplexConjugate(W({_tau_17611}, X_0)) * t_L({+A_5468, +del_4350}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_us_mod * e_em * gamma({+_tau_17615, +gam_8081, +del_4354}) * csl::GetComplexConjugate(s_L({+A_5472, +gam_8081}, X_0)) * csl::GetComplexConjugate(W({_tau_17615}, X_0)) * u_L({+A_5472, +del_4354}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cs_mod * e_em * csl::exp_s(-CSL_I * delta_wolf) * gamma({+_tau_17619, +gam_8085, +del_4358}) * csl::GetComplexConjugate(s_L({+A_5476, +gam_8085}, X_0)) * csl::GetComplexConjugate(W({_tau_17619}, X_0)) * c_L({+A_5476, +del_4358}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ts_mod * e_em * csl::exp_s(-CSL_I * delta_wolf) * gamma({+_tau_17623, +gam_8089, +del_4362}) * csl::GetComplexConjugate(s_L({+A_5480, +gam_8089}, X_0)) * csl::GetComplexConjugate(W({_tau_17623}, X_0)) * t_L({+A_5480, +del_4362}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ub_mod * e_em * csl::exp_s(CSL_I * delta_wolf) * gamma({+_tau_17627, +gam_8093, +del_4366}) * csl::GetComplexConjugate(b_L({+A_5484, +gam_8093}, X_0)) * csl::GetComplexConjugate(W({_tau_17627}, X_0)) * u_L({+A_5484, +del_4366}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cb_mod * e_em * gamma({+_tau_17631, +gam_8097, +del_4370}) * csl::GetComplexConjugate(b_L({+A_5488, +gam_8097}, X_0)) * csl::GetComplexConjugate(W({_tau_17631}, X_0)) * c_L({+A_5488, +del_4370}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_tb_mod * e_em * gamma({+_tau_17635, +gam_8101, +del_4374}) * csl::GetComplexConjugate(b_L({+A_5492, +gam_8101}, X_0)) * csl::GetComplexConjugate(W({_tau_17635}, X_0)) * t_L({+A_5492, +del_4374}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ud_mod * e_em * gamma({+_tau_17639, +gam_8105, +del_4378}) * csl::GetComplexConjugate(u_L({+A_5496, +gam_8105}, X_0)) * W({_tau_17639}, X_0) * d_L({+A_5496, +del_4378}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cd_mod * e_em * csl::exp_s(CSL_I * delta_wolf) * gamma({+_tau_17643, +gam_8109, +del_4382}) * csl::GetComplexConjugate(c_L({+A_5500, +gam_8109}, X_0)) * W({_tau_17643}, X_0) * d_L({+A_5500, +del_4382}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_td_mod * e_em * csl::exp_s(CSL_I * delta_wolf) * gamma({+_tau_17647, +gam_8113, +del_4386}) * csl::GetComplexConjugate(t_L({+A_5504, +gam_8113}, X_0)) * W({_tau_17647}, X_0) * d_L({+A_5504, +del_4386}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_us_mod * e_em * gamma({+_tau_17651, +gam_8117, +del_4390}) * csl::GetComplexConjugate(u_L({+A_5508, +gam_8117}, X_0)) * W({_tau_17651}, X_0) * s_L({+A_5508, +del_4390}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cs_mod * e_em * csl::exp_s(CSL_I * delta_wolf) * gamma({+_tau_17655, +gam_8121, +del_4394}) * csl::GetComplexConjugate(c_L({+A_5512, +gam_8121}, X_0)) * W({_tau_17655}, X_0) * s_L({+A_5512, +del_4394}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ts_mod * e_em * csl::exp_s(CSL_I * delta_wolf) * gamma({+_tau_17659, +gam_8125, +del_4398}) * csl::GetComplexConjugate(t_L({+A_5516, +gam_8125}, X_0)) * W({_tau_17659}, X_0) * s_L({+A_5516, +del_4398}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_ub_mod * e_em * csl::exp_s(-CSL_I * delta_wolf) * gamma({+_tau_17663, +gam_8129, +del_4402}) * csl::GetComplexConjugate(u_L({+A_5520, +gam_8129}, X_0)) * W({_tau_17663}, X_0) * b_L({+A_5520, +del_4402}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_cb_mod * e_em * gamma({+_tau_17667, +gam_8133, +del_4406}) * csl::GetComplexConjugate(c_L({+A_5524, +gam_8133}, X_0)) * W({_tau_17667}, X_0) * b_L({+A_5524, +del_4406}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * V_tb_mod * e_em * gamma({+_tau_17671, +gam_8137, +del_4410}) * csl::GetComplexConjugate(t_L({+A_5528, +gam_8137}, X_0)) * W({_tau_17671}, X_0) * b_L({+A_5528, +del_4410}, X) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_u * gamma5({+alpha_3528, +beta_435}) * csl::GetComplexConjugate(u({+G_1614, +alpha_3528}, X_0)) * u({+G_1614, +beta_435}, X_0) * A_0(X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_c * gamma5({+alpha_3532, +gam_8141}) * csl::GetComplexConjugate(c({+G_1618, +alpha_3532}, X_0)) * c({+G_1618, +gam_8141}, X_0) * A_0(X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_t * gamma5({+alpha_3536, +del_4414}) * csl::GetComplexConjugate(t({+G_1622, +alpha_3536}, X_0)) * t({+G_1622, +del_4414}, X_0) * A_0(X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ud_mod * m_d * csl::GetComplexConjugate(u_L({+A_5532, +gam_8145}, X_0)) * H__(X_0) * d_R({+A_5532, +gam_8145}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_us_mod * m_s * csl::GetComplexConjugate(u_L({+A_5536, +gam_8149}, X_0)) * H__(X_0) * s_R({+A_5536, +gam_8149}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ub_mod * m_b * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(u_L({+A_5540, +gam_8153}, X_0)) * H__(X_0) * b_R({+A_5540, +gam_8153}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cd_mod * m_d * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(c_L({+A_5544, +gam_8157}, X_0)) * H__(X_0) * d_R({+A_5544, +gam_8157}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cs_mod * m_s * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(c_L({+A_5548, +gam_8161}, X_0)) * H__(X_0) * s_R({+A_5548, +gam_8161}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cb_mod * m_b * csl::GetComplexConjugate(c_L({+A_5552, +gam_8165}, X_0)) * H__(X_0) * b_R({+A_5552, +gam_8165}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_td_mod * m_d * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(t_L({+A_5556, +gam_8169}, X_0)) * H__(X_0) * d_R({+A_5556, +gam_8169}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ts_mod * m_s * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(t_L({+A_5560, +gam_8173}, X_0)) * H__(X_0) * s_R({+A_5560, +gam_8173}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_tb_mod * m_b * csl::GetComplexConjugate(t_L({+A_5564, +gam_8177}, X_0)) * H__(X_0) * b_R({+A_5564, +gam_8177}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ud_mod * m_d * csl::GetComplexConjugate(d_R({+A_5568, +gam_8181}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_L({+A_5568, +gam_8181}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_us_mod * m_s * csl::GetComplexConjugate(s_R({+A_5572, +gam_8185}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_L({+A_5572, +gam_8185}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ub_mod * m_b * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(b_R({+A_5576, +gam_8189}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_L({+A_5576, +gam_8189}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cd_mod * m_d * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(d_R({+A_5580, +gam_8193}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_L({+A_5580, +gam_8193}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cs_mod * m_s * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(s_R({+A_5584, +gam_8197}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_L({+A_5584, +gam_8197}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cb_mod * m_b * csl::GetComplexConjugate(b_R({+A_5588, +gam_8201}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_L({+A_5588, +gam_8201}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_td_mod * m_d * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(d_R({+A_5592, +gam_8205}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_L({+A_5592, +gam_8205}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ts_mod * m_s * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(s_R({+A_5596, +gam_8209}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_L({+A_5596, +gam_8209}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * V_tb_mod * m_b * csl::GetComplexConjugate(b_R({+A_5600, +gam_8213}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_L({+A_5600, +gam_8213}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-2, 3) * e_em * csl::sin_s(theta_W) * gamma({+_mu_4708, +eps_3500, +eta_761}) * csl::GetComplexConjugate(u_R({+C_1879, +eps_3500}, X_0)) * Z({_mu_4708}, X_0) * u_R({+C_1879, +eta_761}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-2, 3) * e_em * csl::sin_s(theta_W) * gamma({+_mu_4712, +eps_3504, +eta_765}) * csl::GetComplexConjugate(c_R({+C_1883, +eps_3504}, X_0)) * Z({_mu_4712}, X_0) * c_R({+C_1883, +eta_765}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-2, 3) * e_em * csl::sin_s(theta_W) * gamma({+_mu_4716, +eps_3508, +eta_769}) * csl::GetComplexConjugate(t_R({+C_1887, +eps_3508}, X_0)) * Z({_mu_4716}, X_0) * t_R({+C_1887, +eta_769}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ud_mod * m_u * csl::GetComplexConjugate(u_R({+G_1626, +alpha_3540}, X_0)) * H__(X_0) * d_L({+G_1626, +alpha_3540}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cd_mod * m_c * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(c_R({+G_1630, +alpha_3544}, X_0)) * H__(X_0) * d_L({+G_1630, +alpha_3544}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_td_mod * m_t * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(t_R({+G_1634, +alpha_3548}, X_0)) * H__(X_0) * d_L({+G_1634, +alpha_3548}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_us_mod * m_u * csl::GetComplexConjugate(u_R({+G_1638, +alpha_3552}, X_0)) * H__(X_0) * s_L({+G_1638, +alpha_3552}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cs_mod * m_c * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(c_R({+G_1642, +alpha_3556}, X_0)) * H__(X_0) * s_L({+G_1642, +alpha_3556}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ts_mod * m_t * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(t_R({+G_1646, +alpha_3560}, X_0)) * H__(X_0) * s_L({+G_1646, +alpha_3560}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ub_mod * m_u * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(u_R({+G_1650, +alpha_3564}, X_0)) * H__(X_0) * b_L({+G_1650, +alpha_3564}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cb_mod * m_c * csl::GetComplexConjugate(c_R({+G_1654, +alpha_3568}, X_0)) * H__(X_0) * b_L({+G_1654, +alpha_3568}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_tb_mod * m_t * csl::GetComplexConjugate(t_R({+G_1658, +alpha_3572}, X_0)) * H__(X_0) * b_L({+G_1658, +alpha_3572}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ud_mod * m_u * csl::GetComplexConjugate(d_L({+G_1662, +alpha_3576}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_R({+G_1662, +alpha_3576}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cd_mod * m_c * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(d_L({+G_1666, +alpha_3580}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_R({+G_1666, +alpha_3580}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_td_mod * m_t * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(d_L({+G_1670, +alpha_3584}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_R({+G_1670, +alpha_3584}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_us_mod * m_u * csl::GetComplexConjugate(s_L({+G_1674, +alpha_3588}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_R({+G_1674, +alpha_3588}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cs_mod * m_c * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(s_L({+G_1678, +alpha_3592}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_R({+G_1678, +alpha_3592}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ts_mod * m_t * csl::exp_s(-CSL_I * delta_wolf) * csl::GetComplexConjugate(s_L({+G_1682, +alpha_3596}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_R({+G_1682, +alpha_3596}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_ub_mod * m_u * csl::exp_s(CSL_I * delta_wolf) * csl::GetComplexConjugate(b_L({+G_1686, +alpha_3600}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * u_R({+G_1686, +alpha_3600}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_cb_mod * m_c * csl::GetComplexConjugate(b_L({+G_1690, +alpha_3604}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * c_R({+G_1690, +alpha_3604}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(1, 2)) * V_tb_mod * m_t * csl::GetComplexConjugate(b_L({+G_1694, +alpha_3608}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * t_R({+G_1694, +alpha_3608}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        3 * g_s * T({+f_435, +A_5604, +B_373}) * gamma({+_tau_17675, +gam_8217, +del_4418}) * csl::GetComplexConjugate(d({+A_5604, +gam_8217}, X_0)) * d({+B_373, +del_4418}, X_0) * g({+f_435, _tau_17675}, X_0),
        false);
    model->addLagrangianTerm(
        3 * g_s * T({+f_439, +A_5608, +B_377}) * gamma({+_tau_17679, +gam_8221, +del_4422}) * csl::GetComplexConjugate(s({+A_5608, +gam_8221}, X_0)) * g({+f_439, _tau_17679}, X_0) * s({+B_377, +del_4422}, X_0),
        false);
    model->addLagrangianTerm(
        3 * g_s * T({+f_443, +A_5612, +B_381}) * gamma({+_tau_17683, +gam_8225, +del_4426}) * csl::GetComplexConjugate(b({+A_5612, +gam_8225}, X_0)) * b({+B_381, +del_4426}, X_0) * g({+f_443, _tau_17683}, X_0),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_tau_17687, +gam_8229, +del_4430}) * csl::GetComplexConjugate(d({+A_5616, +gam_8229}, X_0)) * A({_tau_17687}, X_0) * d({+A_5616, +del_4430}, X_0),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_tau_17691, +gam_8233, +del_4434}) * csl::GetComplexConjugate(s({+A_5620, +gam_8233}, X_0)) * A({_tau_17691}, X_0) * s({+A_5620, +del_4434}, X_0),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_tau_17695, +gam_8237, +del_4438}) * csl::GetComplexConjugate(b({+A_5624, +gam_8237}, X_0)) * A({_tau_17695}, X_0) * b({+A_5624, +del_4438}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + 3 * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17699, +gam_8241, +del_4442}) * csl::GetComplexConjugate(d_L({+A_5628, +gam_8241}, X_0)) * Z({_tau_17699}, X_0) * d_L({+A_5628, +del_4442}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + 3 * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17703, +gam_8245, +del_4446}) * csl::GetComplexConjugate(s_L({+A_5632, +gam_8245}, X_0)) * Z({_tau_17703}, X_0) * s_L({+A_5632, +del_4446}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(-1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + 3 * csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_tau_17707, +gam_8249, +del_4450}) * csl::GetComplexConjugate(b_L({+A_5636, +gam_8249}, X_0)) * Z({_tau_17707}, X_0) * b_L({+A_5636, +del_4450}, X_0),
        false);
    model->addLagrangianTerm(
        -m_d * csl::sin_s(_alpha) * csl::GetComplexConjugate(d_L({+A_5640, +gam_8253}, X_0)) * H_0(X_0) * d_R({+A_5640, +gam_8253}, X) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_s * csl::sin_s(_alpha) * csl::GetComplexConjugate(s_L({+A_5644, +gam_8257}, X_0)) * H_0(X_0) * s_R({+A_5644, +gam_8257}, X) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_b * csl::sin_s(_alpha) * csl::GetComplexConjugate(b_L({+A_5648, +gam_8261}, X_0)) * H_0(X_0) * b_R({+A_5648, +gam_8261}, X) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_d * csl::cos_s(_alpha) * csl::GetComplexConjugate(d_L({+A_5652, +gam_8265}, X_0)) * d_R({+A_5652, +gam_8265}, X) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_s * csl::cos_s(_alpha) * csl::GetComplexConjugate(s_L({+A_5656, +gam_8269}, X_0)) * h_0(X_0) * s_R({+A_5656, +gam_8269}, X) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_b * csl::cos_s(_alpha) * csl::GetComplexConjugate(b_L({+A_5660, +gam_8273}, X_0)) * b_R({+A_5660, +gam_8273}, X) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_d * csl::sin_s(_alpha) * csl::GetComplexConjugate(d_R({+E_1658, +gam_8277}, X_0)) * H_0(X_0) * d_L({+E_1658, +gam_8277}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_s * csl::sin_s(_alpha) * csl::GetComplexConjugate(s_R({+E_1662, +gam_8281}, X_0)) * H_0(X_0) * s_L({+E_1662, +gam_8281}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_b * csl::sin_s(_alpha) * csl::GetComplexConjugate(b_R({+E_1666, +gam_8285}, X_0)) * H_0(X_0) * b_L({+E_1666, +gam_8285}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_d * csl::cos_s(_alpha) * csl::GetComplexConjugate(d_R({+E_1670, +gam_8289}, X_0)) * d_L({+E_1670, +gam_8289}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_s * csl::cos_s(_alpha) * csl::GetComplexConjugate(s_R({+E_1674, +gam_8293}, X_0)) * h_0(X_0) * s_L({+E_1674, +gam_8293}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_b * csl::cos_s(_alpha) * csl::GetComplexConjugate(b_R({+E_1678, +gam_8297}, X_0)) * b_L({+E_1678, +gam_8297}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_d * csl::GetComplexConjugate(d_L({+A_5664, +gam_8301}, X_0)) * A_0(X_0) * d_R({+A_5664, +gam_8301}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_s * csl::GetComplexConjugate(s_L({+A_5668, +gam_8305}, X_0)) * A_0(X_0) * s_R({+A_5668, +gam_8305}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_b * csl::GetComplexConjugate(b_L({+A_5672, +gam_8309}, X_0)) * A_0(X_0) * b_R({+A_5672, +gam_8309}, X) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        CSL_I * m_d * csl::GetComplexConjugate(d_R({+A_5676, +gam_8313}, X_0)) * A_0(X_0) * d_L({+A_5676, +gam_8313}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        CSL_I * m_s * csl::GetComplexConjugate(s_R({+A_5680, +gam_8317}, X_0)) * A_0(X_0) * s_L({+A_5680, +gam_8317}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        CSL_I * m_b * csl::GetComplexConjugate(b_R({+A_5684, +gam_8321}, X_0)) * A_0(X_0) * b_L({+A_5684, +gam_8321}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_nu_4706, +alpha_3612, +beta_439}) * csl::GetComplexConjugate(d_R({+E_1682, +alpha_3612}, X_0)) * Z({_nu_4706}, X_0) * d_R({+E_1682, +beta_439}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_nu_4710, +alpha_3616, +beta_443}) * csl::GetComplexConjugate(s_R({+E_1686, +alpha_3616}, X_0)) * Z({_nu_4710}, X_0) * s_R({+E_1686, +beta_443}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_nu_4714, +alpha_3620, +beta_447}) * csl::GetComplexConjugate(b_R({+E_1690, +alpha_3620}, X_0)) * Z({_nu_4714}, X_0) * b_R({+E_1690, +beta_447}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_rho_1846, +gam_8325, +del_4454}) * csl::GetComplexConjugate(e({+gam_8325}, X_0)) * A({_rho_1846}, X_0) * e({+del_4454}, X_0),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_rho_1850, +gam_8329, +del_4458}) * csl::GetComplexConjugate(_mu({+gam_8329}, X_0)) * A({_rho_1850}, X_0) * _mu({+del_4458}, X_0),
        false);
    model->addLagrangianTerm(
        -e_em * gamma({+_rho_1854, +gam_8333, +del_4462}) * csl::GetComplexConjugate(_tau({+gam_8333}, X_0)) * A({_rho_1854}, X_0) * _tau({+del_4462}, X_0),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1858, +gam_8337, +del_4466}) * csl::GetComplexConjugate(e_L({+gam_8337}, X_0)) * csl::GetComplexConjugate(W({_rho_1858}, X_0)) * _nu__eL_({+del_4466}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1862, +gam_8341, +del_4470}) * csl::GetComplexConjugate(_mu_L({+gam_8341}, X_0)) * csl::GetComplexConjugate(W({_rho_1862}, X_0)) * _nu___mu_L_({+del_4470}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1866, +gam_8345, +del_4474}) * csl::GetComplexConjugate(_tau_L({+gam_8345}, X_0)) * csl::GetComplexConjugate(W({_rho_1866}, X_0)) * _nu___tau_L_({+del_4474}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1870, +gam_8349, +del_4478}) * csl::GetComplexConjugate(_nu__eL_({+gam_8349}, X_0)) * W({_rho_1870}, X_0) * e_L({+del_4478}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1874, +gam_8353, +del_4482}) * csl::GetComplexConjugate(_nu___mu_L_({+gam_8353}, X_0)) * W({_rho_1874}, X_0) * _mu_L({+del_4482}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::pow_s(2, csl::intfraction_s(-1, 2)) * e_em * gamma({+_rho_1878, +gam_8357, +del_4486}) * csl::GetComplexConjugate(_nu___tau_L_({+gam_8357}, X_0)) * W({_rho_1878}, X_0) * _tau_L({+del_4486}, X_0) / csl::sin_s(theta_W),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_rho_1882, +gam_8361, +del_4490}) * csl::GetComplexConjugate(e_L({+gam_8361}, X_0)) * Z({_rho_1882}, X_0) * e_L({+del_4490}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_rho_1886, +gam_8365, +del_4494}) * csl::GetComplexConjugate(_mu_L({+gam_8365}, X_0)) * Z({_rho_1886}, X_0) * _mu_L({+del_4494}, X_0),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * (csl::sin_s(theta_W) / csl::cos_s(theta_W) + -csl::cos_s(theta_W) / csl::sin_s(theta_W)) * gamma({+_rho_1890, +gam_8369, +del_4498}) * csl::GetComplexConjugate(_tau_L({+gam_8369}, X_0)) * Z({_rho_1890}, X_0) * _tau_L({+del_4498}, X_0),
        false);
    model->addLagrangianTerm(
        -m_e * csl::sin_s(_alpha) * csl::GetComplexConjugate(e({+eps_3512}, X_0)) * e({+eps_3512}, X_0) * H_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m__mu * csl::sin_s(_alpha) * csl::GetComplexConjugate(_mu({+eps_3516}, X_0)) * H_0(X_0) * _mu({+eps_3516}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m__tau * csl::sin_s(_alpha) * csl::GetComplexConjugate(_tau({+eps_3520}, X_0)) * H_0(X_0) * _tau({+eps_3520}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m_e * csl::cos_s(_alpha) * csl::GetComplexConjugate(e({+eps_3524}, X_0)) * e({+eps_3524}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m__mu * csl::cos_s(_alpha) * csl::GetComplexConjugate(_mu({+eps_3528}, X_0)) * _mu({+eps_3528}, X_0) * h_0(X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -m__tau * csl::cos_s(_alpha) * csl::GetComplexConjugate(_tau({+eps_3532}, X_0)) * h_0(X_0) * _tau({+eps_3532}, X_0) / (v1 * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m_e * gamma5({+eps_3537, +eps_3539}) * csl::GetComplexConjugate(e({+eps_3537}, X_0)) * e({+eps_3539}, X_0) * A_0(X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m__mu * gamma5({+eps_3544, +eta_773}) * csl::GetComplexConjugate(_mu({+eps_3544}, X_0)) * A_0(X_0) * _mu({+eta_773}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -CSL_I * m__tau * gamma5({+eps_3548, +alpha_3624}) * csl::GetComplexConjugate(_tau({+eps_3548}, X_0)) * A_0(X_0) * _tau({+alpha_3624}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_sigma_685, +eps_3552, +eta_777}) * csl::GetComplexConjugate(e_R({+eps_3552}, X_0)) * Z({_sigma_685}, X_0) * e_R({+eta_777}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_sigma_689, +eps_3556, +eta_781}) * csl::GetComplexConjugate(_mu_R({+eps_3556}, X_0)) * Z({_sigma_689}, X_0) * _mu_R({+eta_781}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        e_em * csl::sin_s(theta_W) * gamma({+_sigma_693, +eps_3560, +eta_785}) * csl::GetComplexConjugate(_tau_R({+eps_3560}, X_0)) * Z({_sigma_693}, X_0) * _tau_R({+eta_785}, X_0) / csl::cos_s(theta_W),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m_e * csl::GetComplexConjugate(_nu__eL_({+eps_3564}, X_0)) * H__(X_0) * e_R({+eps_3564}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m__mu * csl::GetComplexConjugate(_nu___mu_L_({+eps_3568}, X_0)) * H__(X_0) * _mu_R({+eps_3568}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m__tau * csl::GetComplexConjugate(_nu___tau_L_({+eps_3572}, X_0)) * H__(X_0) * _tau_R({+eps_3572}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m_e * csl::GetComplexConjugate(e_R({+eps_3576}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * _nu__eL_({+eps_3576}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m__mu * csl::GetComplexConjugate(_mu_R({+eps_3580}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * _nu___mu_L_({+eps_3580}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        -csl::pow_s(2, csl::intfraction_s(1, 2)) * m__tau * csl::GetComplexConjugate(_tau_R({+eps_3584}, X_0)) * csl::GetComplexConjugate(H__(X_0)) * _nu___tau_L_({+eps_3584}, X_0) / (v * csl::tan_s(_beta)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * gamma({+_rho_1894, +gam_8373, +del_4502}) * csl::GetComplexConjugate(_nu__eL_({+gam_8373}, X_0)) * Z({_rho_1894}, X_0) * _nu__eL_({+del_4502}, X_0) / (csl::cos_s(theta_W) * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * gamma({+_rho_1898, +gam_8377, +del_4506}) * csl::GetComplexConjugate(_nu___mu_L_({+gam_8377}, X_0)) * Z({_rho_1898}, X_0) * _nu___mu_L_({+del_4506}, X_0) / (csl::cos_s(theta_W) * csl::sin_s(theta_W)),
        false);
    model->addLagrangianTerm(
        csl::intfraction_s(1, 2) * e_em * gamma({+_rho_1902, +gam_8381, +del_4510}) * csl::GetComplexConjugate(_nu___tau_L_({+gam_8381}, X_0)) * Z({_rho_1902}, X_0) * _nu___tau_L_({+del_4510}, X_0) / (csl::cos_s(theta_W) * csl::sin_s(theta_W)),
        false);
    return model;
}

inline mty::Model &get_mssm()
{
    static std::unique_ptr<mty::Model> model = build_mssm();
    return *model;
}

} // End of namespace mty

#endif