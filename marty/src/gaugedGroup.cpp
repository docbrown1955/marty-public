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

#include "gaugedGroup.h"
#include "diracology.h"
#include "mrtUtils.h"
#include "gauge.h"
#include "quantumField.h"
#include "vectorField.h"
#include "ghostField.h"

using namespace std;
using namespace csl;

namespace mty::gauge {

std::ostream& operator<<(std::ostream& fout, mty::gauge::GroupType type)
{
    switch(type) {
        case mty::gauge::GroupType::U1: fout << "U1"; break;
        case mty::gauge::GroupType::SU: fout << "SU"; break;
        case mty::gauge::GroupType::SO: fout << "SO"; break;
        case mty::gauge::GroupType::Sp: fout << "Sp"; break;
        case mty::gauge::GroupType::E6: fout << "E6"; break;
        case mty::gauge::GroupType::E7: fout << "E7"; break;
        case mty::gauge::GroupType::E8: fout << "E8"; break;
        case mty::gauge::GroupType::F4: fout << "F4"; break;
        case mty::gauge::GroupType::G2: fout << "G2"; break;
        default:
            CallHEPError(mty::error::NotImplementedError,
                    "Type " + toString((int)type) + "not recognized in "
                    + "operator<<(ofstream&, mty::gauge::Type)");
    }
    return fout;
}

} // end of namespace mty::gauge

namespace mty {

csl::Expr normalization(mty::gauge::GroupType type)
{
    switch(type) {
        case mty::gauge::GroupType::SU: return SUGauged::a;
        case mty::gauge::GroupType::SO: return SOGauged::a;
        case mty::gauge::GroupType::Sp: return SpGauged::a;
        case mty::gauge::GroupType::E6: return E6Gauged::a;
        case mty::gauge::GroupType::E7: return E7Gauged::a;
        case mty::gauge::GroupType::E8: return E8Gauged::a;
        case mty::gauge::GroupType::F4: return F4Gauged::a;
        case mty::gauge::GroupType::G2: return G2Gauged::a;
        default:
            CallHEPError(mty::error::TypeError,
                    "Group of type " + toString(type) + " has no defined "
                    "normalization convention.");
    }
    return CSL_UNDEF;
}

///////////////////////////////////////////////////
/*************************************************/
// Class GaugedGroup                             //
/*************************************************/
///////////////////////////////////////////////////

GaugedGroup::GaugedGroup(SemiSimpleGroup* t_group, 
                         Gauge* t_gauge,
                         std::string const& t_nameBoson,
                         bool addGhost)
    :gauge(t_gauge), 
    group(t_group), 
    broken(false),
    nameBoson(t_nameBoson), 
    ghostEnabled(addGhost)
{
    group->setGaugedGroup(this);
    coupling = constant_s("g_");
    coupling->setComplexProperty(csl::ComplexProperty::Real);
}

GaugedGroup::GaugedGroup(SemiSimpleGroup* t_group,
                         Gauge* t_gauge,
                         std::string const& bosonName,
                         bool addGhost,
                         const csl::Expr& t_coupling)
    :GaugedGroup(t_group, t_gauge, bosonName, addGhost)
{
    coupling = t_coupling;
}

GaugedGroup::~GaugedGroup()
{
    for (auto s : vectorSpaces)
        delete s.second;
}

SemiSimpleGroup* GaugedGroup::getGroup() const
{
    return group;
}

bool GaugedGroup::isBroken() const
{
    return broken;
}

mty::gauge::Choice GaugedGroup::getGaugeChoice() const
{
    return boson->getGaugeChoice();
}

void GaugedGroup::setGaugeChoice(gauge::Type t_choice)
{
    boson->setGaugeChoice(t_choice);
}

csl::Expr GaugedGroup::getCouplingConstant() const
{
    return coupling;
}

mty::Generator GaugedGroup::getF() const
{
    return f;
}

csl::Tensor GaugedGroup::getD(size_t p) const
{
    if (auto pos = d.find(p); pos != d.end())
        return pos->second;
    std::vector<csl::Space const*> spaces(p, adjointSpace);
    csl::Tensor d_p("d", spaces);
    d_p->setFullySymmetric();
    if (p == 3)
        d_p->addTraceLessNess(spaces[0]); // d_AAB = 0 (~ Tr(T_B))
    d[p] = d_p;
    std::vector<csl::Index> indices(spaces.size());
    for (auto &i : indices)
        i = spaces[0]->generateIndex();
    d_p->addSelfContraction(
            d_p(indices), d_p(indices),
            csl::constant_s("d_" + getName() + "_" + std::to_string(10*p + p))
            );
    return d_p;
}

csl::Tensor GaugedGroup::getDTilde() const
{
    CallHEPError(mty::error::NotImplementedError,
            "Getting specific spinor invariant tensor for a wrong algebra.");
    return nullptr;
}

void GaugedGroup::setCouplingConstant(const csl::Expr& t_coupling)
{
    HEPAssert(t_coupling->getType() == csl::Type::Constant,
            mty::error::TypeError,
            "Expression for coupling constant of gauge group should be "
            + (string)"a constant not a " + toString(t_coupling->getType()));
    coupling = t_coupling;
}

mty::Generator GaugedGroup::getGenerator(const Irrep& field)
{
    if (not hasGeneratorForRep(field)) {
        generateRepGenerator(field);
        setGeneratorProperties(field,
                               generator[field]);
    }
    return generator[field];
}

mty::Generator GaugedGroup::getGenerator(const QuantumFieldParent& field)
{
    Irrep irrepField = field.getGroupIrrep(group);
    return getGenerator(irrepField);
}

mty::Generator GaugedGroup::getGenerator(std::vector<int> const &labels)
{
    return getGenerator(group->highestWeightRep(labels));
}

void GaugedGroup::setBroken(bool t_broken)
{
    broken = t_broken;
}

Particle GaugedGroup::buildVectorBoson()
{
    if (boson)
        return boson;
    if (nameBoson.empty())
        nameBoson = "A_" + group->getName();
    GaugeIrrep irrep = gauge->getTrivialRep();
    bool groupInGauge = false;
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i] == group) {
            irrep[i] = group->getAdjointRep();
            groupInGauge = true;
            break;
        }
    HEPAssert(groupInGauge,
            mty::error::RuntimeError,
            "Group not found in gauge groups trying to gauge U(1).");

    // Name = "A"
    // Spin = 2 -> spin 1 particle
    Particle unique_boson = gaugeboson_s(nameBoson, irrep, this);
    boson = std::dynamic_pointer_cast<GaugeBoson>(unique_boson);

    return unique_boson;
}

Particle GaugedGroup::buildGhost()
{
    if (ghost)
        return ghost;
    if (not ghostEnabled)
        return nullptr;
    GaugeIrrep irrep = gauge->getTrivialRep();
    bool groupInGauge = false;
    for (size_t i = 0; i != gauge->size(); ++i)
        if ((*gauge)[i] == group) {
            irrep[i] = group->getAdjointRep();
            groupInGauge = true;
            break;
        }
    HEPAssert(groupInGauge,
            mty::error::RuntimeError,
            "Group not found in gauge groups trying to gauge U(1).");

    // Name = "A"
    // Spin = 2 -> spin 1 particle
    Particle unique_ghost = ghostboson_s(mty::Particle(boson));
    ghost = std::dynamic_pointer_cast<GhostBoson>(unique_ghost);

    return unique_ghost;
}

AlgebraState GaugedGroup::getHighestWeight(const csl::Space* space) const
{
    return group->getHighestWeight(space);
}

csl::Expr GaugedGroup::getEffectiveQuadraticCasimir(mty::Irrep const &rep) const
{
    auto defining = group->getAlgebra()->getDefiningRep();
    csl::Expr res = normalization(getType())
        * group->getAlgebra()->getQuadraticCasimir(rep.getHighestWeight())
        / group->getAlgebra()->getQuadraticCasimir(defining.getHighestWeight())
        * rep.getDim() / defining.getDim();
    return res;
}

csl::Expr GaugedGroup::getCA() const
{
    Irrep adjoint = group->getAdjointRep();
    return getEffectiveQuadraticCasimir(adjoint);
}

csl::Expr GaugedGroup::getCF() const
{
    auto defining = group->getAlgebra()->getDefiningRep();
    return getEffectiveQuadraticCasimir(defining);
}

csl::Expr GaugedGroup::covariantDerivative(const csl::Expr& field,
                                      const Index& mu)
{
    HEPAssert(IsOfType<QuantumField>(field),
            mty::error::TypeError,
            "Expression \"" + std::string(field->getName()) 
            + "\" should be a quantum field"
            + " in GaugedGroup::covariantDerivative().");

    const vector<Index>& 
        fieldIndices = field->getIndexStructureView().getIndexView();
    Parent fieldParent = field->getParent();
    Tensor  point     = field->getPoint();

    return covariantDerivative(
            *ConvertToPtr<QuantumFieldParent>(fieldParent.get()),
            mu,
            fieldIndices,
            point);
}

bool GaugedGroup::isFieldInTrivialRep(const QuantumFieldParent& field) const
{
    return (field.getGroupIrrep(group) == group->getTrivialRep());
}

bool GaugedGroup::hasGeneratorForRep(const Irrep& irrep) const
{
    return (generator.find(irrep) != generator.end());
}

void GaugedGroup::setGeneratorProperties(
        Irrep const&,
        mty::Generator&)
{

}

std::string getGeneratorName(
        std::string const &init,
        std::string const &groupName,
        std::vector<int> const &labels
        )
{
    std::ostringstream sout;
    sout << init << "_" << groupName << "_";
    for (int label : labels)
        sout << label;

    return sout.str();
}

void GaugedGroup::generateRepGenerator(const Irrep& irrep)
{
    const Space* fieldSpace = group->getVectorSpace(irrep);
    generator[irrep] = mty::Generator(
            this,
            irrep,
            getGeneratorName(
                nameGenerator, 
                getName(),
                irrep.getHighestWeight().getLabels()
                ),
            {adjointSpace, fieldSpace, fieldSpace}
            );
    if (fieldSpace == adjointSpace) {
        // T^A_{B,C} == f_ABC structure constants
        generator[irrep]->setName("f");
        generator[irrep]->setFullyAntiSymmetric();
    }
}

void GaugedGroup::initF()
{
    Irrep adjoint = group->getAdjointRep();
    adjointSpace = group->getVectorSpace(adjoint);
    d[2] = adjointSpace->getDelta();
    if (adjointSpace) {
        generateRepGenerator(adjoint);
        f = generator[adjoint];
        setGeneratorProperties(adjoint, f);
        f->setComplexProperty(csl::ComplexProperty::Real);
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class U1Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


U1Gauged::U1Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson)
    :GaugedGroup(t_group, gauge, t_nameBoson, false)
{

}

U1Gauged::U1Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   const csl::Expr& t_coupling)
    :GaugedGroup(t_group, gauge, t_nameBoson, false, t_coupling)
{

}

mty::gauge::GroupType U1Gauged::getType() const
{
    return mty::gauge::GroupType::U1;
}

csl::Expr U1Gauged::covariantDerivative(QuantumFieldParent&   field,
                                   const Index&          mu,
                                   const vector<Index>&  fieldIndices,
                                   Tensor&              point)
{
    Irrep irrepField = field.getGroupIrrep(group);
    vector<int> labels = irrepField.getHighestWeight().getLabels();
    int num   = labels[0];
    int denom = labels[1];

    csl::Expr gaugeCoupling =  -CSL_I
                        * intfraction_s(num, denom)
                        * coupling 
                        * (*boson)({mu}, point);

    return gaugeCoupling * field(fieldIndices, point);
}


///////////////////////////////////////////////////
/*************************************************/
// Class NonAbelianGauged                        //
/*************************************************/
///////////////////////////////////////////////////


NonAbelianGauged::NonAbelianGauged(SemiSimpleGroup* t_group,
                                   Gauge* gauge,
                                   std::string const& t_nameBoson,
                                   bool addGhost)
    :GaugedGroup(t_group, gauge, t_nameBoson, addGhost)
{

}

NonAbelianGauged::NonAbelianGauged(SemiSimpleGroup* t_group,
                                   Gauge* gauge,
                                   std::string const& t_nameBoson,
                                   bool addGhost,
                                   const csl::Expr& t_coupling)
    :GaugedGroup(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{

}

csl::Expr NonAbelianGauged::covariantDerivative(QuantumFieldParent&   field,
                                           const Index&          mu,
                                           const vector<Index>&  fieldIndices,
                                           Tensor&              point)
{
    Irrep irrepField = field.getGroupIrrep(group);
    if (irrepField == group->getTrivialRep())
        return CSL_0;
    
    Tensor T = getGenerator(field);
    const Space* spaceIrrepField = group->getVectorSpace(irrepField);
    Index A = adjointSpace->generateIndex();
    Index a;
    Index b = spaceIrrepField->generateIndex();
    int posGaugeIndex = -1;
    for (size_t i = 0; i != fieldIndices.size(); ++i)
        if (fieldIndices[i].getSpace() == spaceIrrepField) {
            posGaugeIndex = i;
            a = fieldIndices[i];
            break;
        }
    HEPAssert(posGaugeIndex != -1,
            mty::error::RuntimeError,
            "Gauge index not found in gauge coupling for " 
            + std::string(field.getName()));

    csl::Expr gaugeCoupling = (-CSL_I)
                        * coupling 
                        * (*boson)({+A, mu}, point)
                        * T({+A, a, b});
    if (spaceIrrepField == adjointSpace) {
        gaugeCoupling *= -CSL_I;
    }

    vector<Index> newFieldIndices(fieldIndices);
    newFieldIndices[posGaugeIndex] = b;

    return gaugeCoupling * field(newFieldIndices, point);
}

void NonAbelianGauged::setGeneratorProperties(
        Irrep const& rep,
        mty::Generator& T)
{
    const Space* space = T->getSpace()[1]; 
    csl::Index A = adjointSpace->generateIndex();
    csl::Index B = adjointSpace->generateIndex();
    csl::Index C = adjointSpace->generateIndex();
    csl::Index i = space->generateIndex();
    csl::Index j = space->generateIndex();
    csl::Index k = space->generateIndex();
    csl::Index l = space->generateIndex();
    csl::Tensor delta_adj = adjointSpace->getDelta();
    csl::Tensor delta     = space->getDelta();
    // Quadratic Casimir
    csl::Expr res = getEffectiveQuadraticCasimir(rep);
    if (space == adjointSpace)
        res *= -1;
    T->addSelfContraction(
            T({A, i, j}), 
            T({B, j, i}),
            res * delta_adj({A, B})
            );
    T->addTraceLessNess(space);

    if (space != adjointSpace) {
        T->setComplexProperty(csl::ComplexProperty::Complex);
        T->addHermitianProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );
        T->addSelfContraction(
                f({A, B, C}),
                T({C, i, j}),
                - CSL_I * T({A, i, k}) * T({B, k, j})
                + CSL_I * T({B, i, k}) * T({A, k, j})
                );
    }
}

bool NonAbelianGauged::isAdjointGenerator(
        mty::Generator const &tensor
        ) const
{
    return tensor.get() == f.get();
}

int NonAbelianGauged::getGeneratorDim(
        mty::Generator const &tensor
        ) const
{
    return tensor->getSpace()[1]->getDim();
}

///////////////////////////////////////////////////
/*************************************************/
// Class SUGauged                                //
/*************************************************/
///////////////////////////////////////////////////


SUGauged::SUGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
    init_f_d_ABC();
}

SUGauged::SUGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
    init_f_d_ABC();
}

mty::gauge::GroupType SUGauged::getType() const
{
    return mty::gauge::GroupType::SU;
}

void SUGauged::init_f_d_ABC()
{
    csl::Tensor d_ABC = getD(3);
    d_ABC->addTraceLessNess(adjointSpace);

    const size_t N = group->getAlgebra()->getOrderL() + 1;
    if (N == 2) {
        Irrep adjoint = group->getAdjointRep();
        f = mty::Generator(
                this,
                adjoint,
                "f_" + getName(),
                {adjointSpace, adjointSpace, adjointSpace}
                );
        f->setComplexProperty(csl::ComplexProperty::Real);
        f->setFullyAntiSymmetric();
        f->setTensor(adjointSpace->getEpsilon()->getTensor());
        csl::Index i = adjointSpace->generateIndex();
        csl::Index j = adjointSpace->generateIndex();
        csl::Index k = adjointSpace->generateIndex();
        csl::Index l = adjointSpace->generateIndex();
        csl::Tensor delta = adjointSpace->getDelta();
        generator[adjoint] = f;
        setGeneratorProperties(adjoint, f);
    }
    else if (N == 3) {
        f->setTensor(gell_mann::f);
        d_ABC->setTensor(gell_mann::d3);
    }
}

void SUGauged::setGeneratorProperties(
        Irrep const& rep,
        mty::Generator& T)
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    const csl::Expr N = int_s(group->getAlgebra()->getOrderL() + 1);
    int orderL = group->getAlgebra()->getOrderL();
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor delta     = space->getDelta();

        // Hermitian generator
        T->addHermitianProperty(
                T->getSpace()[1],
                T({A, i, j}),
                T({A, i, j})
                );
        T->addComplexProperty(
                T({A, i, j}),
                T({A, j, i})
                );
        // Projection relation
        // T^A_ij T^A_kl = 1/2 * ( d_il*d_jk - 1/N d_ij*d_kl )
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}), 
                    T({A, k, l}),
                    a * delta({i, l})*delta({j, k})
                    - a / N * delta({i, j})*delta({k, l})
                    );
    
        if (N == 2 and getGeneratorDim(T) == orderL+1) {
            T->setTensor(
                    csl::highdtensor_from_args_s(
                        {CSL_1/2 * pauli::sigma1,
                         CSL_1/2 * pauli::sigma2,
                         CSL_1/2 * pauli::sigma3}
                        )
                    );
        }
        else if (N == 3 and getGeneratorDim(T) == orderL+1) {
            T->setTensor(
                    csl::highdtensor_from_args_s(
                        {CSL_1/2 * gell_mann::lambda1,
                         CSL_1/2 * gell_mann::lambda2,
                         CSL_1/2 * gell_mann::lambda3,
                         CSL_1/2 * gell_mann::lambda4,
                         CSL_1/2 * gell_mann::lambda5,
                         CSL_1/2 * gell_mann::lambda6,
                         CSL_1/2 * gell_mann::lambda7,
                         CSL_1/2 * gell_mann::lambda8}
                        )
                    );
        }
   }
}


///////////////////////////////////////////////////
/*************************************************/
// Class SOGauged                                //
/*************************************************/
///////////////////////////////////////////////////


SOGauged::SOGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
    initDTilde();
}

SOGauged::SOGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
    initDTilde();
}

mty::gauge::GroupType SOGauged::getType() const
{
    return mty::gauge::GroupType::SO;
}

void SOGauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T)
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor delta     = space->getDelta();

        // Imaginary generator
        T->setComplexProperty(csl::ComplexProperty::Imaginary);
        // Symmetric generator
        T->addTransposedProperty(
                space,
                T({A, i, j}),
                T({A, i, j})
                );

        // Projection relation
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 2 * delta({i, l}) * delta({j, k})
                    - a / 2 * delta({i, k}) * delta({j, l})
                    );
    }
}

void SOGauged::initDTilde()
{
    int m = group->getDim();
    if (m % 2 == 1)
        return;
    m /= 2;
    d_tilde = csl::tensor_s(
            "~d", 
            std::vector<csl::Space const*>(m, adjointSpace));
    d_tilde->setFullySymmetric();
    std::vector<csl::Index> indices(m);
    for (auto &index : indices)
        index = adjointSpace->generateIndex();
    d_tilde->addSelfContraction(
            d_tilde(indices),
            d_tilde(indices),
            csl::pow_s(2, -m)*csl::factorial(2*m) / csl::int_s(4)
            );
}

csl::Tensor SOGauged::getDTilde() const
{
    return d_tilde;
}

///////////////////////////////////////////////////
/*************************************************/
// Class SpGauged                                //
/*************************************************/
///////////////////////////////////////////////////


SpGauged::SpGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
}

SpGauged::SpGauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
}

mty::gauge::GroupType SpGauged::getType() const
{
    return mty::gauge::GroupType::Sp;
}

void SpGauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T
        )
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor g         = space->getMetric();

        // Real generator
        T->setComplexProperty(csl::ComplexProperty::Real);

        T->addTransposedProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );

        // Projection relation
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 2 * g({i, l}) * g({j, k})
                    - a / 2 * g({i, k}) * g({j, l})
                    );
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class E6Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


E6Gauged::E6Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
}

E6Gauged::E6Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
}

mty::gauge::GroupType E6Gauged::getType() const
{
    return mty::gauge::GroupType::E6;
}

void E6Gauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T
        )
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Index m = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor delta     = space->getDelta();

        T->addHermitianProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );
        // Projection relation
        csl::Tensor d = d_invariant[rep];
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 6 * delta({i, l}) * delta({j, k})
                    + a / 18 * delta({i, k}) * delta({j, l})
                    - 5 / (3*alpha) * d({i, k, m}) * d({m, j, l})
                    );
    }
}

void E6Gauged::generateRepGenerator(
        mty::Irrep const &irrep
        ) 
{
    const Space* fieldSpace = group->getVectorSpace(irrep);
    if (fieldSpace == adjointSpace) {
        csl::Index i = fieldSpace->generateIndex();
        csl::Index j = fieldSpace->generateIndex();
        csl::Index k = fieldSpace->generateIndex();
        csl::Index l = fieldSpace->generateIndex();
        csl::Tensor delta = fieldSpace->getDelta();
        csl::Tensor d(
                "d",
                {fieldSpace, fieldSpace, fieldSpace}
                );
        d->setComplexProperty(csl::ComplexProperty::Real);
        d->setFullySymmetric();

        d->addSelfContraction(
                d({i, j, k}),
                d({l, +j, +k}),
                delta({i, l})
                );
        d->addSelfContraction(
                d({i, j, k}),
                d({+k, +j, l}),
                alpha * delta({i, l})
                );
        d_invariant[irrep] = d;
    }
    NonAbelianGauged::generateRepGenerator(irrep);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E7Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


E7Gauged::E7Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
}

E7Gauged::E7Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
}

mty::gauge::GroupType E7Gauged::getType() const
{
    return mty::gauge::GroupType::E7;
}

void E7Gauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T
        )
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor g         = space->getMetric();

        T->addHermitianProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );
        // Projection relation
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 24 * g({i, l}) * g({j, k})
                    + a / 24 * g({i, k}) * g({j, l})
                    - 1 / (alpha * 12) * d_invariant[rep]({i, j, k, l})
                    );
    }
}

void E7Gauged::generateRepGenerator(
        mty::Irrep const &irrep
        ) 
{
    const Space* fieldSpace = group->getVectorSpace(irrep);
    if (fieldSpace == adjointSpace) {
        csl::Index i = fieldSpace->generateIndex();
        csl::Index j = fieldSpace->generateIndex();
        csl::Index k = fieldSpace->generateIndex();
        csl::Index l = fieldSpace->generateIndex();
        csl::Tensor d(
                "d",
                {fieldSpace, fieldSpace, fieldSpace, fieldSpace}
                );
        d->setComplexProperty(csl::ComplexProperty::Real);
        d->setFullySymmetric();
        d_invariant[irrep] = d;
    }
    NonAbelianGauged::generateRepGenerator(irrep);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E8Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


E8Gauged::E8Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
}

E8Gauged::E8Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
}

mty::gauge::GroupType E8Gauged::getType() const
{
    return mty::gauge::GroupType::E8;
}

///////////////////////////////////////////////////
/*************************************************/
// Class F4Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


F4Gauged::F4Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{

}

F4Gauged::F4Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{

}

mty::gauge::GroupType F4Gauged::getType() const
{
    return mty::gauge::GroupType::F4;
}

void F4Gauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T
        )
{
    NonAbelianGauged::setGeneratorProperties(rep, T);
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Index m = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor delta     = space->getDelta();

        T->addHermitianProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );
        // Projection relation
        csl::Tensor d = d_invariant[rep];
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 9 * delta({i, l}) * delta({j, k})
                    - a / 9 * delta({i, k}) * delta({j, l})
                    - 7 / (9*alpha) * d({i, k, m}) * d({m, j, l})
                    + 7 / (9*alpha) * d({i, l, m}) * d({m, j, k})
                    );
    }
}

void F4Gauged::generateRepGenerator(
        mty::Irrep const &irrep
        ) 
{
    const Space* fieldSpace = group->getVectorSpace(irrep);
    if (fieldSpace == adjointSpace) {
        csl::Index i = fieldSpace->generateIndex();
        csl::Index j = fieldSpace->generateIndex();
        csl::Index k = fieldSpace->generateIndex();
        csl::Index l = fieldSpace->generateIndex();
        csl::Tensor delta = fieldSpace->getDelta();
        csl::Tensor d(
                "d",
                {fieldSpace, fieldSpace, fieldSpace}
                );
        d->setComplexProperty(csl::ComplexProperty::Real);
        d->setFullySymmetric();
        d->addSelfContraction(
                d({i, j, k}),
                d({+k, +j, l}),
                alpha * delta({i, l})
                );
        d_invariant[irrep] = d;
    }
    NonAbelianGauged::generateRepGenerator(irrep);
}

///////////////////////////////////////////////////
/*************************************************/
// Class G2Gauged                                //
/*************************************************/
///////////////////////////////////////////////////


G2Gauged::G2Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost)
{
    initF();
}

G2Gauged::G2Gauged(SemiSimpleGroup* t_group,
                   Gauge* gauge,
                   std::string const& t_nameBoson,
                   bool addGhost,
                   const csl::Expr& t_coupling)
    :NonAbelianGauged(t_group, gauge, t_nameBoson, addGhost, t_coupling)
{
    initF();
}

mty::gauge::GroupType G2Gauged::getType() const
{
    return mty::gauge::GroupType::G2;
}

void G2Gauged::setGeneratorProperties(
        mty::Irrep const &rep,
        mty::Generator      &T
        )
{
    if (!isAdjointGenerator(T)) {
        const Space* space = T->getSpace()[1]; 
        csl::Index A = adjointSpace->generateIndex();
        csl::Index B = adjointSpace->generateIndex();
        csl::Index C = adjointSpace->generateIndex();
        csl::Index i = space->generateIndex();
        csl::Index j = space->generateIndex();
        csl::Index k = space->generateIndex();
        csl::Index l = space->generateIndex();
        csl::Index m = space->generateIndex();
        csl::Tensor delta_adj = adjointSpace->getDelta();
        csl::Tensor delta     = space->getDelta();

        T->addHermitianProperty(
                space,
                T({A, i, j}),
                T({A, j, i})
                );
        // Projection relation
        csl::Tensor f_inv(
                "d",
                {space, space, space}
                );
        f_inv->setComplexProperty(csl::ComplexProperty::Real);
        f_inv->setFullySymmetric();
        f_inv->addSelfContraction(
                f_inv({i, j, k}),
                f_inv({+k, +j, l}),
                alpha * delta({i, l})
                );
        if (rep == group->getAlgebra()->getDefiningRep())
            T->addSelfContraction(
                    T({A, i, j}),
                    T({A, k, l}),
                    a / 2 * delta({i, l}) * delta({j, k})
                    - a / 2 * delta({i, k}) * delta({j, l})
                    - 1 / alpha * f_inv({i, j, m}) * f_inv({m, k, l})
                    );
    }
    NonAbelianGauged::setGeneratorProperties(rep, T);
}

void G2Gauged::generateRepGenerator(
        mty::Irrep const &irrep
        ) 
{
    const Space* fieldSpace = group->getVectorSpace(irrep);
    if (fieldSpace == adjointSpace) {
        csl::Index i = fieldSpace->generateIndex();
        csl::Index j = fieldSpace->generateIndex();
        csl::Index k = fieldSpace->generateIndex();
        csl::Index l = fieldSpace->generateIndex();
        csl::Tensor delta = fieldSpace->getDelta();
        mty::Generator f(
                this, 
                irrep,
                "f",
                {fieldSpace, fieldSpace, fieldSpace}
                );
        f->setComplexProperty(csl::ComplexProperty::Real);
        f->setFullyAntiSymmetric();
        f->addSelfContraction(
                f({i, j, k}),
                f({+k, +j, l}),
                alpha * delta({i, l})
                );
        f_invariant[irrep] = f;
    }
    NonAbelianGauged::generateRepGenerator(irrep);
}

///////////////////////////////////////////////////
/*************************************************/
// Other functions                               //
/*************************************************/
///////////////////////////////////////////////////

unique_ptr<GaugedGroup> createGaugedGroup(
        SemiSimpleGroup* group,
        Gauge* gauge,
        std::string nameBoson,
        bool addGhost,
        const csl::Expr& coupling)
{
    unique_ptr<GaugedGroup> gauged;
    if (nameBoson.empty())
        nameBoson = "A_" + group->getName();

    switch(group->getType()) {
        case group::Type::U1:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<U1Gauged>(
                        group, gauge, nameBoson, coupling);
            else
                gauged = make_unique<U1Gauged>(
                        group, gauge, nameBoson);
        break;

        case group::Type::SU:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<SUGauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<SUGauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::SO:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<SOGauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<SOGauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::Sp:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<SpGauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<SpGauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::E6:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<E6Gauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<E6Gauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::E7:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<E7Gauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<E7Gauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::E8:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<E8Gauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<E8Gauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::F4:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<F4Gauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<F4Gauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        case group::Type::G2:
            if (coupling != CSL_UNDEF) 
                gauged = make_unique<G2Gauged>(
                        group, gauge, nameBoson, addGhost, coupling);
            else
                gauged = make_unique<G2Gauged>(
                        group, gauge, nameBoson, addGhost);
        break;

        default:
        HEPAssert(false,
                mty::error::NotImplementedError,
                "Group " + toString(group->getType()) + 
                " cannot be gauged yet.");
    }
    return gauged;
}

} // End of namespace mty
