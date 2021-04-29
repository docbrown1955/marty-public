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

#include "SM.h"
#include "fermionicField.h"
#include "diracology.h"
#include "group.h"

using namespace mty::sm_input;
using namespace csl;

namespace mty {

    SM_Model::SM_Model()
        :Model()
    {
        // SM_Model gauge (after EW breaking) = SU(3)_c x U(1)_em
        gauge = std::make_unique<Gauge>();

        auto U1  = mty::createGroup(group::Type::U1, "U1_em");
        auto SU3 = mty::createGroup(group::Type::SU, "SU3_c", 3);
        auto U1_ptr = U1.get();
        auto SU3_ptr = SU3.get();

        gauge->addGroup(U1,  "A", csl::sqrt_s(4*CSL_PI*alpha_em));
        gauge->addGroup(SU3, "g", csl::sqrt_s(4*CSL_PI*alpha_s));

        init();

        mty::Particle e_L = mty::weylfermion_s(
                "e_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Left);

        mty::Particle e_R = mty::weylfermion_s(
                "e_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Right);

        addParticle(e_L);
        addParticle(e_R);
        addFermionicMass(e_L, e_R, m_e);

        mty::Particle nu_e_L = mty::weylfermion_s(
                "nu_e_L",
                gauge->getTrivialRep()
                );

        addParticle(nu_e_L);

        mty::Particle mu_L = mty::weylfermion_s(
                "mu_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Left);

        mty::Particle mu_R = mty::weylfermion_s(
                "mu_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Right);

        addParticle(mu_L);
        addParticle(mu_R);
        addFermionicMass(mu_L, mu_R, m_mu);

        mty::Particle nu_mu_L = mty::weylfermion_s(
                "nu_mu_L",
                gauge->getTrivialRep()
                );

        addParticle(nu_mu_L);

        mty::Particle tau_L = mty::weylfermion_s(
                "tau_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Left);

        mty::Particle tau_R = mty::weylfermion_s(
                "tau_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 1}),
                    SU3_ptr->getTrivialRep()}),
                Chirality::Right);

        addParticle(tau_L);
        addParticle(tau_R);
        addFermionicMass(tau_L, tau_R, m_tau);

        mty::Particle nu_tau_L = mty::weylfermion_s(
                "nu_tau_L",
                gauge->getTrivialRep()
                );

        addParticle(nu_tau_L);

        mty::Particle u_L = mty::weylfermion_s(
                "u_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle u_R = mty::weylfermion_s(
                "u_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(u_L);
        addParticle(u_R);
        addFermionicMass(u_L, u_R, m_u);

        mty::Particle d_L = mty::weylfermion_s(
                "d_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle d_R = mty::weylfermion_s(
                "d_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(d_L);
        addParticle(d_R);
        addFermionicMass(d_L, d_R, m_d);

        mty::Particle c_L = mty::weylfermion_s(
                "c_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle c_R = mty::weylfermion_s(
                "c_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(c_L);
        addParticle(c_R);
        addFermionicMass(c_L, c_R, m_c);

        mty::Particle s_L = mty::weylfermion_s(
                "s_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle s_R = mty::weylfermion_s(
                "s_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(s_L);
        addParticle(s_R);
        addFermionicMass(s_L, s_R, m_s);

        mty::Particle b_L = mty::weylfermion_s(
                "b_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle b_R = mty::weylfermion_s(
                "b_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({-1, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(b_L);
        addParticle(b_R);
        addFermionicMass(b_L, b_R, m_b);

        mty::Particle t_L = mty::weylfermion_s(
                "t_L",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Left);

        mty::Particle t_R = mty::weylfermion_s(
                "t_R",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({2, 3}),
                    SU3_ptr->highestWeightRep({1, 0})}),
                Chirality::Right);

        addParticle(t_L);
        addParticle(t_R);
        addFermionicMass(t_L, t_R, m_t);

        mty::Particle Z = mty::vectorboson_s(
                "Z",
                gauge.get(),
                true);

        mty::Particle W = mty::vectorboson_s(
                "W",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({1, 1}),
                    SU3_ptr->getTrivialRep()}),
                false);

        mty::Particle G0 = mty::scalarboson_s(
                "G^0",
                gauge.get(),
                true);

        mty::Particle Gc = mty::scalarboson_s(
                "G^+",
                mty::GaugeIrrep(
                    gauge.get(),
                    {U1_ptr->highestWeightRep({1, 1}),
                    SU3_ptr->getTrivialRep()}),
                false);

        mty::Particle h = mty::scalarboson_s(
                "h",
                gauge->getTrivialRep(),
                true
                );

        Z->setMass(M_Z);
        G0->setMass(M_Z);
        W->setMass(M_W);
        Gc->setMass(M_W);
        h->setMass(m_h);

        auto cc = [](csl::Expr const& init) 
        {
            return csl::GetComplexConjugate(init);
        };

        addParticle(Z);
        addParticle(W);
        addParticle(G0);
        addParticle(Gc);
        addParticle(h);
    
        csl::Index mu = csl::Minkowski.generateIndex();
        csl::Index nu = csl::Minkowski.generateIndex();
        csl::Tensor X  = L.getPoint();
        mty::Particle A = getParticle("A");
        mty::Particle F_Z = Z->getFieldStrength();
        mty::Particle F_W = W->getFieldStrength();
        mty::Particle F_A = A->getFieldStrength();
        csl::Expr s2_tw = csl::pow_s(csl::sin_s(theta_W), 2);
        csl::Expr sin_tw = csl::sin_s(theta_W);
        csl::Expr cot_tw = csl::cos_s(theta_W) / csl::sin_s(theta_W);
        csl::Expr cot2_tw = csl::pow_s(cot_tw, 2);
        csl::Expr cos_tw = csl::cos_s(theta_W);
        csl::Expr e = csl::sqrt_s(4*CSL_PI*alpha_em);

        // Source : Schwartz p.586 

        // h^3
        addLagrangianTerm(-e * csl::pow_s(m_h, csl::int_s(2)) 
                / (4 * M_W * sin_tw) 
                * csl::pow_s(h(X), csl::int_s(3)));

        // h^4
        addLagrangianTerm(- csl::pow_s(e * m_h / M_W, csl::int_s(2)) 
                / (32 * s2_tw) 
                * csl::pow_s(h(X), csl::int_s(4)));

        // hWW
        addLagrangianTerm(M_W*e/(csl::sqrt_s(s2_tw))
                *h(X)*W(+mu, X)*cc(W(mu, X)));

        // hZZ
        addLagrangianTerm(pow_s(M_Z, 2)*e/(2*M_W*csl::sqrt_s(s2_tw))
                *h(X)*Z(+mu, X)*Z(mu, X));

        // hhWW
        addLagrangianTerm(e*e/(2*s2_tw)
                *h(X)*h(X)*W(+mu, X)*cc(W(mu, X)));

        // hhZZ
        addLagrangianTerm(pow_s(M_Z, 2)*e*e/(4*M_W*M_W*s2_tw)
                *h(X)*h(X)*Z(+mu, X)*Z(mu, X));

        // WWZ 
        csl::Expr g = CSL_I * e * cot_tw;
        addLagrangianTerm(g  * F_Z({mu, nu}, X) * W(+mu, X) * cc(W(+nu, X)));
        addLagrangianTerm(-g * F_W({mu, nu}, X) * Z(+mu, X) * cc(W(+nu, X)));
        addLagrangianTerm(g  * cc(F_W({mu, nu}, X)) * Z(+mu, X) * W(+nu, X));

        // WWA
        g = CSL_I * e;
        addLagrangianTerm(g  * F_A({mu, nu}, X) * W(+mu, X) * cc(W(+nu, X)));
        addLagrangianTerm(-g * F_W({mu, nu}, X) * A(+mu, X) * cc(W(+nu, X)));
        addLagrangianTerm(g  * cc(F_W({mu, nu}, X)) * A(+mu, X) * W(+nu, X));

        // WWWW
        g = CSL_HALF 
            * csl::pow_s(e, csl::int_s(2)) 
            / s2_tw;
        addLagrangianTerm(g  * W(+mu, X) * W(mu, X) * cc(W(+nu, X)) * cc(W(nu, X)));
        addLagrangianTerm(-g * W(+mu, X) * W(nu, X) * cc(W(+nu, X)) * cc(W(mu, X)));

        // WWZZ
        g = pow_s(e ,  2) * cot2_tw;
        addLagrangianTerm(g * Z(+mu, X) * W(mu, X)  * Z(+nu, X) * cc(W(nu, X)));
        addLagrangianTerm(g * Z(+mu, X) * W(+nu, X) * Z(mu, X)  * cc(W(nu, X)));

        // WWAA
        g = pow_s(e ,  2);
        addLagrangianTerm(g * A(+mu, X) * W(mu, X)  * A(+nu, X) * cc(W(nu, X)));
        addLagrangianTerm(g * A(+mu, X) * W(+nu, X) * A(mu, X)  * cc(W(nu, X)));

        // WWAZ
        g = pow_s(e ,  2) * cot_tw;
        addLagrangianTerm(g    * W(+mu, X)     * cc(W(+nu, X)) * A(mu, X)  * Z(nu, X));
        addLagrangianTerm(g    * cc(W(+mu, X)) * W(+nu, X)     * A(mu, X)  * Z(nu, X));
        addLagrangianTerm(-2*g * W(+mu, X)     * cc(W(mu, X))  * A(+nu, X) * Z(nu, X));

        // Source for Goldstone interactions : arxiv:1209.6213.pdf
        
        auto D = [&](csl::Index mu, csl::Expr const &arg) 
        {
            return csl::tderivativeelement_s(
                    mty::defaultSpaceTimePoint,
                    mty::partialMinko_shared,
                    mu,
                    arg,
                    false);
        };
        // hG^+W^-
        g = CSL_1/2 * e / csl::sqrt_s(s2_tw);
        addLagrangianTerm(g * cc(W(mu)) * (
                    CSL_I * D(+mu, Gc) * h 
                    - CSL_I*D(+mu, h) * Gc
                    ),
                true);
        // hG^0Z
        addLagrangianTerm(-CSL_I * g / cos_tw * Z(mu) * (
                    CSL_I * D(+mu, G0) * h 
                    - CSL_I*D(+mu, h) * G0
                    ));

        //G^+G^-h
        g = -CSL_1/2 * e / (M_W * csl::sqrt_s(s2_tw)) * m_h*m_h;
        addLagrangianTerm(g * cc(Gc) * Gc * h);

        // G^0G^0h
        addLagrangianTerm(g * G0 * G0 * h);

        // G+G-hh
        g = -CSL_1/4 * csl::pow_s(e * m_h/ M_W, 2) / s2_tw;
        addLagrangianTerm(g / 2 * cc(Gc) * Gc * h * h);

        // G^0G^0hh
        addLagrangianTerm(g / 4 * cc(G0) * G0 * h * h);

        // Source: Schwartz p.594 to 597
        using containor = std::vector<std::tuple<Particle, Particle, bool>>;
        csl::Expr v = 2*M_W/e * csl::sqrt_s(s2_tw);
        csl::Expr J_3 = CSL_0;
        csl::Expr J_EM = CSL_0;
        auto ffb_coupling = [&](Particle& f_left,
                                Particle& boson,
                                Particle& f_right)
        {
            std::vector<csl::Index> indices = f_left->getFullSetOfIndices();
            std::vector<csl::Index> indices2(indices);
            for (auto& index : indices2)
                index.flipSign();
            indices2.back() = indices2.back().rename();
            csl::Index alpha = indices.back();
            csl::Index beta  = indices2.back();
            return GetComplexConjugate(f_left(indices, X))
                * mty::dirac4.gamma({+mu, alpha, beta})
                * boson(mu, X)
                * f_right(indices2, X);
        };

        gatherMasses();
        for (auto& [left, right, up_type] : 
                containor(
                {{e_L,      e_R,     0},
                 {mu_L,     mu_R,    0},
                 {tau_L,    tau_R,   0},
                 {nu_e_L,   nullptr, 1},
                 {nu_mu_L,  nullptr, 1},
                 {nu_tau_L, nullptr, 1},
                 {u_L,      u_R,     1},
                 {d_L,      d_R,     0},
                 {c_L,      c_R,     1},
                 {s_L,      s_R,     0},
                 {t_L,      t_R,     1},
                 {b_L,      b_R,     0}})) 
        {
            csl::Expr m = left->getMass();
            if (m != CSL_0) {
                QuantumFieldParent& diracF = *right->getDiracParent();
                std::vector<csl::Index> indices = diracF.getFullSetOfIndices();
                std::vector<csl::Index> indices2(indices);
                for (auto& index : indices2)
                    index.flipSign();
                addLagrangianTerm(h(X) / v * m 
                        * GetComplexConjugate(diracF(indices, X))
                        * diracF(indices2, X));
            }
            J_3 = J_3 
                + ((up_type) ? CSL_HALF : CSL_M_HALF) 
                * ffb_coupling(left, Z, left);
            csl::Expr emcharge = getCharge(left, "U1_em");
            if (emcharge != CSL_0) {
                J_EM = J_EM + emcharge*ffb_coupling(left, Z, left);
                J_EM = J_EM + emcharge*ffb_coupling(right, Z, right);
            }
        }

        addLagrangianTerm(e / csl::sqrt_s(s2_tw) / cos_tw * J_3);
        addLagrangianTerm(-e / cot_tw * J_EM);

        g = e / (csl::sqrt_s(2*s2_tw));
        addLagrangianTerm(-g * ffb_coupling(nu_e_L, W, e_L), true);
        addLagrangianTerm(-g * ffb_coupling(nu_mu_L, W, mu_L), true);
        addLagrangianTerm(-g * ffb_coupling(nu_tau_L, W, tau_L), true);

        addLagrangianTerm(g * V_ud * ffb_coupling(u_L, W, d_L), true);
        addLagrangianTerm(g * V_us * ffb_coupling(u_L, W, s_L), true);
        addLagrangianTerm(g * V_ub * ffb_coupling(u_L, W, b_L), true);
        addLagrangianTerm(g * V_cd * ffb_coupling(c_L, W, d_L), true);
        addLagrangianTerm(g * V_cs * ffb_coupling(c_L, W, s_L), true);
        addLagrangianTerm(g * V_cb * ffb_coupling(c_L, W, b_L), true);
        addLagrangianTerm(g * V_td * ffb_coupling(t_L, W, d_L), true);
        addLagrangianTerm(g * V_ts * ffb_coupling(t_L, W, s_L), true);
        addLagrangianTerm(g * V_tb * ffb_coupling(t_L, W, b_L), true);

        L.mergeTerms();
}

} // End of namespace mty
