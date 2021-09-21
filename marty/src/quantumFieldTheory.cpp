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

#include "quantumFieldTheory.h"
#include "quantumField.h"
#include "fermionicField.h"
#include "polarization.h"
#include "propagator.h"
#include "mrtError.h"
#include "mrtOptions.h"

using namespace std;
using namespace csl;

namespace mty {

csl::Expr ScalarKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X)
{
    Gauge *gauge = field.getGauge();
    Index mu = Minkowski.generateIndex();
    csl::Expr partialUp   = partialMinko(+mu, X);
    csl::Expr partialDown = partialMinko(mu, X);
    vector<Index> indices = field.getFlavorIndex();
    vector<Index> gaugeIndices = field.getGaugeIndex();
    vector<const Space*> gaugeSpaces(gaugeIndices.size());
    for (size_t i = 0; i != gaugeSpaces.size(); ++i)
        gaugeSpaces[i] = gaugeIndices[i].getSpace();
    indices.insert(indices.end(), gaugeIndices.begin(), gaugeIndices.end());
    
    csl::Expr expr = field(indices, X);
    csl::Expr kineticTerm = 
        GetHermitianConjugate(
                gauge->covariantDerivative(Copy(expr), mu),
                gaugeSpaces)
       * gauge->covariantDerivative(Copy(expr), +mu);
    if (field.isSelfConjugate())
        // 1/2 *d^mu(phi)d_mu(phi)
        kineticTerm = CSL_HALF * kineticTerm;

    return kineticTerm;
}

csl::Expr FermionKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X)
{
    Gauge *gauge = field.getGauge();
    Index mu = Minkowski.generateIndex();
    csl::Expr partialUp   = partialMinko(+mu, X);
    csl::Expr partialDown = partialMinko(mu, X);
    vector<Index> indices = field.getFlavorIndex();
    vector<Index> gaugeIndices = field.getGaugeIndex();
    vector<const Space*> gaugeSpaces(gaugeIndices.size());
    for (size_t i = 0; i != gaugeSpaces.size(); ++i)
        gaugeSpaces[i] = gaugeIndices[i].getSpace();
    indices.insert(indices.end(), gaugeIndices.begin(), gaugeIndices.end());
    

    vector<Index> psiDaggerIndex = indices;
    vector<Index> psiIndex       = indices;
    Index alpha = dirac4.generateIndex();
    Index gamm  = dirac4.generateIndex();
    psiIndex.push_back(gamm);
    psiDaggerIndex.push_back(alpha);
    csl::Expr factor = CSL_1;
    if (field.isSelfConjugate() 
            and field.getParticleType() == ParticleType::DiracFermion)
        factor = CSL_HALF;
    csl::Expr kineticTerm = factor * GetComplexConjugate(field(psiDaggerIndex, X))
        * CSL_I * gauge->covariantDerivative(field, mu, psiIndex, X)
                   * dirac4.gamma({+mu, alpha, gamm});

    return kineticTerm;
}

csl::Expr VectorKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X)
{
    Gauge *gauge = field.getGauge();
    Index mu = Minkowski.generateIndex();
    csl::Expr partialUp   = partialMinko(+mu, X);
    csl::Expr partialDown = partialMinko(mu, X);
    vector<Index> indices = field.getFlavorIndex();
    vector<Index> gaugeIndices = field.getGaugeIndex();
    vector<const Space*> gaugeSpaces(gaugeIndices.size());
    for (size_t i = 0; i != gaugeSpaces.size(); ++i)
        gaugeSpaces[i] = gaugeIndices[i].getSpace();
    indices.insert(indices.end(), gaugeIndices.begin(), gaugeIndices.end());
    

    Index nu = Minkowski.generateIndex();
    vector<Index> _mu = indices;
    _mu.push_back(mu);
    vector<Index> _pmu = indices;
    _pmu.push_back(+mu);
    vector<Index> _nu = indices;
    _nu.push_back(nu);
    vector<Index> _pnu = indices;
    _pnu.push_back(+nu);
    csl::Expr factor;
    if (field.isSelfConjugate())
        factor = CSL_1 / 4;
    else
        factor = CSL_1 / 2;
    csl::Expr kineticTerm = - factor *
        GetComplexConjugate((gauge->covariantDerivative(field, mu,  _nu,  X)))
      * (gauge->covariantDerivative(field, +mu, _pnu, X));
    factor = (field.isSelfConjugate()) ? CSL_HALF : CSL_1;

    return kineticTerm;
}

csl::Expr GhostKineticTerm(
        QuantumFieldParent& field,
        csl::Tensor      & X)
{
    Gauge *gauge = field.getGauge();
    Index mu = Minkowski.generateIndex();
    csl::Expr partialUp   = partialMinko(+mu, X);
    csl::Expr partialDown = partialMinko(mu, X);
    vector<Index> indices = field.getFlavorIndex();
    vector<Index> gaugeIndices = field.getGaugeIndex();
    vector<const Space*> gaugeSpaces(gaugeIndices.size());
    for (size_t i = 0; i != gaugeSpaces.size(); ++i)
        gaugeSpaces[i] = gaugeIndices[i].getSpace();
    indices.insert(indices.end(), gaugeIndices.begin(), gaugeIndices.end());
    
    csl::Expr expr = field(indices, X);
    csl::Expr kineticTerm = 
        GetComplexConjugate(partialMinko(mu, X) * expr)
       * gauge->covariantDerivative(Copy(expr), +mu);

    return kineticTerm;
}

csl::Expr ExponentialFactor(csl::Tensor    & X,
                       csl::Tensor    & Y,
                       csl::Tensor    & P)
{
    csl::Index mu = P->getSpace()[0]->generateIndex();
    return csl::exp_s(CSL_I * P(+mu) * (Y(mu) - X(mu)));
}

csl::Expr ReplaceXiGauge(csl::Expr const &initialMass)
{
    bool simplified = false;
    csl::Expr newMass = csl::DeepCopy(initialMass);
    csl::ForEachLeaf(newMass, [&](csl::Expr &sub) {
        if (sub->getName() == "xi") {
            csl::Expr value = csl::Evaluated(sub, csl::eval::literal);
            if (value != CSL_UNDEF) {
                sub = value;
                simplified = true;
            }
        }
    });

    if (simplified) {
        return csl::DeepRefreshed(newMass);
    }
    return newMass;
}

csl::Expr StandardDenominator(csl::Tensor    & P,
                         csl::Expr       const& mass,
                         csl::Expr       const& width,
                         bool              external)
{
    if (external)
        return CSL_M_1;
    csl::Index mu = P->getSpace()[0]->generateIndex();
    return propagator_s(P(mu), mass, width);
}

csl::Expr StandardDenominator(csl::Tensor    & P,
                         csl::Expr       const& mass,
                         bool              external)
{
    if (external)
        return CSL_M_1;
    return StandardDenominator(P, mass, CSL_0);
}

csl::Expr NullPropagator(
        QuantumField const&,
        QuantumField const&,
        csl::Tensor      &,
        bool)
{
    return CSL_0;
}

csl::Expr ScalarPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size());
    for (size_t i = 0; i != structA.size(); ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    csl::Expr mass = A.getMass();
    if (!external) {
        mass = ReplaceXiGauge(mass);
    }

    return 
        CSL_I 
        * csl::prod_s(deltaFactor, true)
        * ExponentialFactor(pointA, pointB, P)
        * StandardDenominator(P, mass, A.getWidth(), external);
}

csl::Expr FermionPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    if (A.isComplexConjugate() && !B.isComplexConjugate())
        return -FermionPropagator(B, A, P, external);
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }
    csl::Index alpha = structA[structA.size()-1];
    csl::Index beta  = structB[structB.size()-1];
    const DiracSpace* diracSpace 
        = dynamic_cast<const DiracSpace*>(alpha.getSpace());

    if (!mty::option::enableAntiChiralProps
            and A.isChiral()
            and B.isChiral()
            and A.getChirality() != B.getChirality())
        return CSL_0;

    csl::Expr projectors = CSL_1;
    auto chirA = A.getChirality();
    if (A.isComplexConjugate() xor A.isExternal())
        chirA = !chirA;
    Chirality chirB = B.getChirality();
    if (B.isExternal() xor B.isComplexConjugate())
        chirB = !chirB;
    if (A.isChiral()) {
        csl::Index gam = diracSpace->generateIndex();
        if (chirA == Chirality::Left) {
            projectors = diracSpace->P_L({alpha, gam});
        }
        else {
            projectors = diracSpace->P_R({alpha, gam});
        }
        alpha = gam;
    }
    else if (B.isChiral()) {
        csl::Index gam = diracSpace->generateIndex();
        if (chirB == Chirality::Left) {
            projectors = projectors * diracSpace->P_L({gam, beta});
        }
        else {
            projectors = projectors * diracSpace->P_R({gam, beta});
        }
        beta = gam;
    }

    if (external and A.isChiral() and B.isChiral() and chirA != chirB)
        return CSL_0;

    if (A.isComplexConjugate()) {
        Index gam = alpha.rename();
        projectors = projectors * -dirac4.C_matrix({alpha, gam});
        alpha = gam;
    }
    if (!B.isComplexConjugate()) {
        Index gam = beta.rename();
        projectors = projectors * -dirac4.C_matrix({gam, beta});
        beta = gam;
    }

    csl::Tensor delta = diracSpace->getDelta();
    csl::Expr m = (A.isChiral() and B.isChiral() and chirA != chirB) ? 
        CSL_0 : A.getMass() * delta({alpha, beta});
    csl::Expr p = (A.isChiral() and B.isChiral() and chirA == chirB) ?
        CSL_0 : slashed_s(P, alpha, beta, diracSpace);
    csl::Expr diracStructure = (external) ?  delta({alpha, beta}) : (p + m);

    const csl::Expr res = 
        CSL_I
        * projectors
        * csl::prod_s(deltaFactor)
        * diracStructure
        * ExponentialFactor(pointA, pointB, P)
        * StandardDenominator(P, A.getMass(), A.getWidth(), external);
    return res;
}

csl::Expr VectorPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    csl::Index mu = structA[structA.size()-1];
    csl::Index nu = structB[structB.size()-1];
    csl::Index rho = mu.getSpace()->generateIndex();
    csl::Tensor g = mu.getSpace()->getMetric();
    csl::Expr prop_mu_nu = g({mu, nu});
    csl::Expr m = A.getMass();
    if (not external) {
        csl::Expr xi = ReplaceXiGauge(A.getXiGauge());
        csl::Expr side;
        if (xi == CSL_INF)
            side = P(mu)*P(nu) / (m*m);
        else
            side = (1 - xi) * (P(mu)*P(nu)) 
                 * propagator_s(P(mu), m * sqrt_s(xi));
        prop_mu_nu = prop_mu_nu - side;
    }
    auto res = 
        - CSL_I 
        * csl::prod_s(deltaFactor)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        * StandardDenominator(P, A.getMass(), A.getWidth(), external);
    return res;
}

csl::Expr FieldStrengthPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    // <A_mu F_nu,rho^(*)> or <F_mu,nu A_rho^(*)>
    csl::IndexStructure structA = A.getIndexStructureView();
    csl::IndexStructure structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();
    bool fieldStrengthFirst = structA.size() > structB.size();
    csl::Expr sign = CSL_1;
    if (fieldStrengthFirst) {
        sign = CSL_M_1;
        std::swap(structA, structB);
    }

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    // Prop for struct A_mu F_nu,rho
    csl::Index mu  = structA[structA.size()-1];
    csl::Index nu  = structB[structB.size()-2];
    csl::Index rho = structB[structB.size()-1];
    csl::Index sigma = mu.getSpace()->generateIndex();
    csl::Tensor g = mu.getSpace()->getMetric();
    csl::Expr prop_mu_nu = g({mu, rho})*P(nu) - g({mu, nu})*P(rho);
    csl::Expr m = A.getMass();

    return sign 
        * csl::prod_s(deltaFactor, true)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        * StandardDenominator(P, A.getMass(), A.getWidth(), external);
}

csl::Expr FieldStrengthSquaredPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size()-2);
    for (size_t i = 0; i != structA.size()-2; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    // Prop for struct F_mu,nu F_rho,sigma
    csl::Index mu  = structA[structA.size()-2];
    csl::Index nu  = structA[structB.size()-1];
    csl::Index rho   = structB[structB.size()-2];
    csl::Index sigma = structB[structB.size()-1];
    csl::Tensor g = mu.getSpace()->getMetric();

    csl::Expr prop_mu_nu = g({mu, sigma})*P(nu)*P(rho)
                    - g({mu, rho})*P(nu)*P(sigma)
                    + g({nu, rho})*P(mu)*P(sigma)
                    - g({nu, sigma})*P(mu)*P(rho);
    csl::Expr m = A.getMass();

    return 
          CSL_I 
        * csl::prod_s(deltaFactor, true)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        * StandardDenominator(P, A.getMass(), A.getWidth(), external);
}

csl::Expr IntegratedScalarPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size());
    for (size_t i = 0; i != structA.size(); ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }
    csl::Expr mass = ReplaceXiGauge(A.getMass());

    return
        -CSL_I 
        * csl::prod_s(deltaFactor, true)
        * ExponentialFactor(pointA, pointB, P)
        / ((mass + A.getWidth()) * (mass - A.getWidth()));
}

csl::Expr IntegratedFermionPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool                external)
{
    if (!A.isSelfConjugate() && A.isComplexConjugate())
        return -IntegratedFermionPropagator(B, A, P, external);
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();
    bool reverted = A.isComplexConjugate() and !B.isComplexConjugate();
    if (reverted) {
        std::swap(pointA, pointB);
    }

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }
    csl::Index alpha = structA[structA.size()-1];
    csl::Index beta  = structB[structB.size()-1];
    const DiracSpace* diracSpace 
        = dynamic_cast<const DiracSpace*>(alpha.getSpace());

    csl::Expr projectors = CSL_1;
    if (A.isChiral()) {
        Chirality chir = A.getChirality();
        if (A.isExternal())
            chir = !chir;
        csl::Index gam = diracSpace->generateIndex();
        if (chir == Chirality::Left) {
            projectors = diracSpace->P_L({alpha, gam});
        }
        else {
            projectors = diracSpace->P_R({alpha, gam});
        }
        alpha = gam;
    }
    else if (B.isChiral()) {
        Chirality chir = !B.getChirality();
        if (B.isExternal())
            chir = !chir;
        csl::Index gam = diracSpace->generateIndex();
        if (chir == Chirality::Left) {
            projectors = projectors * diracSpace->P_L({gam, beta});
        }
        else {
            projectors = projectors * diracSpace->P_R({gam, beta});
        }
        beta = gam;
    }
    if (A.isComplexConjugate() == B.isComplexConjugate()) {
        // Majorana
        if (A.isComplexConjugate()) {
            //Psi^star Psi^star
            csl::Index gam = diracSpace->generateIndex();
            projectors = projectors * diracSpace->C_matrix({alpha, gam});
            alpha = gam;
        }
        else {
            //Psi Psi
            csl::Index gam = diracSpace->generateIndex();
            projectors = projectors * diracSpace->C_matrix({gam, beta});
            beta = gam;
        }
    }
    if (A.isComplexConjugate()) {
        Index gam = alpha.rename();
        projectors = projectors * -dirac4.C_matrix({alpha, gam});
        alpha = gam;
    }
    if (!B.isComplexConjugate()) {
        Index gam = beta.rename();
        projectors = projectors * -dirac4.C_matrix({gam, beta});
        beta = gam;
    }


    csl::Tensor delta = diracSpace->getDelta();
    csl::Expr m = (A.isChiral() 
            and A.getChirality() == B.getChirality()) ? 
        CSL_0 : A.getMass() * delta({alpha, beta});
    csl::Expr p = (A.isChiral() 
            and B.isChiral() 
            and A.getChirality() != B.getChirality()) ?
        CSL_0 : slashed_s(P, alpha, beta, diracSpace);
    csl::Expr diracStructure = (external) ?  delta({alpha, beta})
        : (p + m);

    Expr sign = (reverted and !external) ? -1 : 1;
    return 
        - sign * CSL_I 
        * projectors
        * csl::prod_s(deltaFactor)
        * diracStructure
        * ExponentialFactor(pointA, pointB, P)
        / ((A.getMass() + A.getWidth()) * (A.getMass() - A.getWidth()));
}

csl::Expr IntegratedVectorPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    csl::Index mu = structA[structA.size()-1];
    csl::Index nu = structB[structB.size()-1];
    csl::Index rho = mu.getSpace()->generateIndex();
    csl::Tensor g = mu.getSpace()->getMetric();
    csl::Expr prop_mu_nu = g({mu, nu});

    return 
        CSL_I 
        * csl::prod_s(deltaFactor)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        / ((A.getMass() + A.getWidth()) * (A.getMass() - A.getWidth()));
}

csl::Expr IntegratedFieldStrengthPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool)
{
    // <A_mu F_nu,rho^(*)> or <F_mu,nu A_rho^(*)>
    csl::IndexStructure structA = A.getIndexStructureView();
    csl::IndexStructure structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();
    bool fieldStrengthFirst = structA.size() > structB.size();
    csl::Expr sign = CSL_1;
    if (fieldStrengthFirst) {
        sign = CSL_M_1;
        std::swap(structA, structB);
    }

    csl::vector_expr deltaFactor(structA.size()-1);
    for (size_t i = 0; i != structA.size()-1; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    // Prop for struct A_mu F_nu,rho
    csl::Index mu  = structA[structA.size()-1];
    csl::Index nu  = structB[structB.size()-2];
    csl::Index rho = structB[structB.size()-1];
    csl::Index sigma = mu.getSpace()->generateIndex();
    csl::Tensor g = mu.getSpace()->getMetric();
    csl::Expr prop_mu_nu = g({mu, rho})*P(nu) - g({mu, nu})*P(rho);

    return - sign 
        * csl::prod_s(deltaFactor, true)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        / ((A.getMass() + A.getWidth()) * (A.getMass() - A.getWidth()));
}

csl::Expr IntegratedFieldStrengthSquaredPropagator(
        QuantumField const& A,
        QuantumField const& B,
        csl::Tensor      & P,
        bool)
{
    csl::IndexStructure const& structA = A.getIndexStructureView();
    csl::IndexStructure const& structB = B.getIndexStructureView();
    csl::Tensor        pointA  = A.getPoint();
    csl::Tensor        pointB  = B.getPoint();

    csl::vector_expr deltaFactor(structA.size()-2);
    for (size_t i = 0; i != structA.size()-2; ++i) {
        csl::Space const* space = structA[i].getSpace();
        deltaFactor[i] = space->getDelta()({structA[i], structB[i]});
    }

    // Prop for struct F_mu,nu F_rho,sigma
    csl::Index mu  = structA[structA.size()-2];
    csl::Index nu  = structA[structB.size()-1];
    csl::Index rho   = structB[structB.size()-2];
    csl::Index sigma = structB[structB.size()-1];
    csl::Tensor g = mu.getSpace()->getMetric();

    csl::Expr prop_mu_nu = g({mu, sigma})*P(nu)*P(rho)
                    - g({mu, rho})*P(nu)*P(sigma)
                    + g({nu, rho})*P(mu)*P(sigma)
                    - g({nu, sigma})*P(mu)*P(rho);
    return 
        - CSL_I 
        * csl::prod_s(deltaFactor, true)
        * prop_mu_nu
        * ExponentialFactor(pointA, pointB, P)
        / ((A.getMass() + A.getWidth()) * (A.getMass() - A.getWidth()));
}

csl::Expr ExternalLeg(
        QuantumField   const& field,
        const csl::Tensor&    impulsion,
        bool                  ruleMode,
        bool                  lock
        )
{
    HEPAssert(field.isExternal(),
            mty::error::ValueError,
            "Trying to get LSZ insertion from non external field.");
    return ExternalLeg(*field.getQuantumParent(),
                       impulsion,
                       field.getPoint(),
                       field.getIndexStructureView().getIndexView(),
                       field.isParticle(),
                       field.isIncoming(),
                       field.isOnShell(),
                       field.getPartnerShip(),
                       ruleMode,
                       lock);
}

csl::Expr ExternalLeg(QuantumFieldParent & field,
                 csl::Tensor          P,
                 csl::Tensor          X,
                 vector<Index>        indices,
                 bool                 particle,
                 bool                 incoming,
                 bool                 onshell,
                 PartnerShip   const &partnerShip,
                 bool                 ruleMode,
                 bool                 lockConjugation
                 )
{
    const int spinDim = field.getSpinDimension();

    Index rho = Minkowski.generateIndex();
    csl::Expr expSign   = (incoming) ? -CSL_1 : CSL_1;
    csl::Expr common = CSL_I * csl::vectorintegral_s(X)
        * csl::exp_s(expSign * CSL_I * X(rho) * P(+rho));
    if (spinDim == 3)
        common = -common;
    if (ruleMode || mty::option::amputateExternalLegs)
        return common;
    Index lambda = Euclid_R2.generateIndex();
    for (auto& i : indices)
        if (i.getSpace()->getSignedIndex())
            i.flipSign();
    csl::Expr polarTensor = field(lambda, indices, P);
    auto ptr = ConvertToPtr<PolarizationField>(polarTensor);
    if (lockConjugation)
        ptr->setConjugationLock(true);
    ptr->setExternal(true);
    ptr->setParticle(particle);
    ptr->setIncoming(incoming);
    ptr->setOnShell(onshell);
    if (spinDim == 2)
        ptr->setPartnerShip(partnerShip);
    return polarTensor * common;
}

csl::Expr MajoranaMassTerm(
        csl::Expr         const &mass, 
        QuantumFieldParent *field)
{
    vector<Index> index1 = field->getFullSetOfIndices();
    vector<Index> index2 = index1;
    auto a = index1.back();
    auto b = a.rename();
    index2.back() = b;
    auto C = dirac4.C_matrix({a, b});
    csl::Expr psi     = (*field)(index1);
    csl::Expr psi_bar = GetComplexConjugate((*field)(index1));
    return -CSL_HALF * mass * (
            (*field)(index1)*C*(*field)(index2)
            + csl::GetComplexConjugate((*field)(index1))
                *C*csl::GetComplexConjugate((*field)(index2))
            );
}

csl::Expr MajoranaMassTerm(
        csl::Expr         const &mass, 
        QuantumFieldParent *fieldL,
        QuantumFieldParent *fieldR)
{
    if (&fieldL == &fieldR)
        return MajoranaMassTerm(mass, fieldL);
    vector<Index> index1 = fieldL->getFullSetOfIndices();
    vector<Index> index2 = index1;
    auto a = index1.back();
    auto b = a.rename();
    index2.back() = b;
    auto C = dirac4.C_matrix({a, b});
    return - mass * (
            (*fieldR)(index1)*C*(*fieldL)(index2)
            + csl::GetComplexConjugate((*fieldL)(index1))
                *C*csl::GetComplexConjugate((*fieldR)(index2))
            );
}

csl::Expr MassTerm(
        csl::Expr const    &mass,
        QuantumFieldParent *field
        )
{
    if (mass == CSL_0)
        return CSL_0;
    if (field->getParticleType() == ParticleType::WeylFermion) {
        // HEPAssert(field->isSelfConjugate(),
        //         mty::error::PhysicsError,
        //         "Cannot create a mass term for a non-Majorana 2 component "
        //         "fermion.")
        return MajoranaMassTerm(mass, field);
    }
    return MassTerm(mass, field, field);
}

csl::Expr MassTerm(
        csl::Expr const&         mass,
              QuantumFieldParent* fieldL,
              QuantumFieldParent* fieldR)
{
    if (mass == CSL_0)
        return CSL_0;
    if (!(fieldL->getGaugeIrrep()*fieldR->getGaugeIrrep().getConjugatedRep())
            .containsTrivialRep()) {
        return MajoranaMassTerm(mass, fieldL, fieldR);
    }
    if (fieldL->getChirality() != Chirality::None 
            && fieldR->getChirality() != Chirality::None
            && fieldL->getChirality() == fieldR->getChirality()) {
        return MajoranaMassTerm(mass, fieldL, fieldR);
    }
    csl::Expr factor = 
        (fieldL->isSelfConjugate() and !IsOfType<WeylFermion>(fieldL)) ? 
        CSL_HALF : CSL_1;
    if (fieldL->isBosonic())
        factor = factor * mass * mass;
    else
        factor = factor * mass;
    if (fieldL->getSpinDimension() == 3)
        factor *= -1;
    vector<Index> index1 = fieldL->getFullSetOfIndices();
    vector<Index> index2(index1);
    for (auto& i : index2)
        i = i.getFlipped();

    auto massTerm = -factor * 
            GetComplexConjugate((*fieldL)(index1))*(*fieldR)(index2);
    if (fieldL == fieldR) {
        return massTerm;
    }
    else {
        return massTerm + csl::GetHermitianConjugate(massTerm, &mty::dirac4);
    }
}

}
