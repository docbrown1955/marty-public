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

#include "gauge.h"
#include "mrtError.h"
#include "gaugedGroup.h"
#include "diracology.h"
#include "quantumField.h"

using namespace std;
using namespace csl;

namespace mty {

Gauge::Gauge(string const& t_name): name(t_name)
{
	
}

Gauge::~Gauge()
{
    // for (auto& group : groups)
    //     if (group)
    //         delete group;
}

void Gauge::addGroup(unique_ptr<SemiSimpleGroup>& t_group,
                     const csl::Expr&        coupling)
{
    addGroup(t_group, "", coupling);
}

void Gauge::addGroup(unique_ptr<SemiSimpleGroup>& t_group,
                     std::string const& nameBoson,
                     const csl::Expr&        coupling)
{
    csl::Expr actualCoupling = (coupling) ? 
        coupling : constant_s("g_" + t_group->getName());
    groups.push_back(std::move(t_group));
    gaugedGroups.push_back(
            createGaugedGroup(
                groups[groups.size()-1].get(),
                this,
                nameBoson,
                true,
                actualCoupling));
}

void Gauge::addGroup(unique_ptr<SemiSimpleGroup>& t_group,
                     std::string const& nameBoson,
                     bool               addGhost,
                     const csl::Expr&        coupling)
{
    csl::Expr actualCoupling = (coupling) ? 
        coupling : constant_s("g_" + t_group->getName());
    groups.push_back(std::move(t_group));
    gaugedGroups.push_back(
            createGaugedGroup(
                groups[groups.size()-1].get(),
                this,
                nameBoson,
                addGhost,
                actualCoupling));
}

GaugeIrrep Gauge::getRepresentation(
        const vector<vector<int> >& highestWeights)
{
    vector<Irrep> reps(0);
    for (size_t i = 0; i != groups.size(); ++i) 
        reps.push_back(groups[i]->highestWeightRep(highestWeights[i]));

    return GaugeIrrep(this,reps);
}

bool Gauge::containsTrivialRep(
        std::vector<QuantumField> const &fields
        ) const
{
    if (fields.empty())
        return true;
    for (size_t i = 0; i != gaugedGroups.size(); ++i) {
        if (gaugedGroups[i]->isBroken())
            continue;
        SumIrrep decompo {fields[0].getGroupIrrep(groups[i].get())};
        for (size_t j = 1; j != fields.size(); ++j) {
            decompo = groups[i]->getAlgebra()->tensorProduct(
                    decompo,
                    fields[j].getGroupIrrep(groups[i].get())
                    );
        }
        bool hasTrivial = false;
        if (groups[i]->getType() == mty::group::Type::U1) {
            for (const auto &rep : decompo)
                if (rep.getCharge() == CSL_0) {
                    hasTrivial = true;
                    break;
                }
        }
        else {
            for (const auto &rep : decompo) 
                if (rep.getDim() == 1) {
                    hasTrivial = true;
                    break;
                }
        }
        if (!hasTrivial)
            return false;
    }
    return true;
}

GaugeIrrep Gauge::getTrivialRep()
{
    GaugeIrrep rep(this);
    for (size_t i = 0; i != rep.size(); ++i) 
        rep[i] = groups[i]->getTrivialRep();

    return rep;
}

SumGaugeIrrep Gauge::tensorProduct(const GaugeIrrep& A,
                                   const GaugeIrrep& B) const
{
    return A*B;
}

csl::Expr Gauge::covariantDerivative(QuantumFieldParent&       field,
                                const Index&              mu,
                                const std::vector<Index>& fieldIndices,
                                Tensor&                  point)
{
    csl::Expr cov = CSL_0;
    for (const auto& gauged : gaugedGroups)
        cov = cov + gauged->covariantDerivative(field, mu, fieldIndices, point);

    if (field.hasFieldStrength()) {
        auto indices(fieldIndices);
        indices.insert(indices.end()-1, mu);
        auto F_munu = field.getFieldStrength()(indices, point);
        return F_munu + cov;
    }
    return partialMinko(mu, point)*field(fieldIndices, point) + cov;
}

csl::Expr Gauge::covariantDerivative(const csl::Expr& field,
                                const Index& mu)
{
    csl::Expr cov = CSL_0;
    for (const auto& gauged : gaugedGroups)
        cov = cov + gauged->covariantDerivative(field, mu);
    Tensor point = field->getPoint();

    return partialMinko(mu, point)*field + cov;
}

optional<csl::Expr> Gauge::getCoupling(const string& t_name) const
{
    for (const auto& gauged : gaugedGroups)
        if (gauged->getCouplingConstant()->getName() == t_name)
            return gauged->getCouplingConstant();
    return nullopt;
}

vector<Particle> Gauge::getVectorBosons() const
{
    vector<Particle> bosons;
    bosons.reserve(gaugedGroups.size());
    for (const auto& gauged : gaugedGroups)
        bosons.push_back(gauged->buildVectorBoson());

    return bosons;
}

vector<Particle> Gauge::getGhosts() const
{
    vector<Particle> ghosts;
    ghosts.reserve(gaugedGroups.size());
    for (const auto& gauged : gaugedGroups)
        if (auto ghost = gauged->buildGhost(); ghost)
            ghosts.push_back(ghost);

    return ghosts;
}

GaugedGroup* Gauge::getGaugedGroup(size_t pos) const
{
    HEPAssert(pos < gaugedGroups.size(),
            mty::error::IndexError,
            "Index " + toString(pos) + " out of bounds of gauge with size "
            + toString(gaugedGroups.size()));
    return gaugedGroups[pos].get();
}

void Gauge::setGaugeChoice(size_t pos,
                           gauge::Type choice)
{
    HEPAssert(pos < gaugedGroups.size(),
            mty::error::IndexError,
            "Index " + toString(pos) + " out of bounds of gauge with size "
            + toString(gaugedGroups.size()));
    gaugedGroups[pos]->setGaugeChoice(choice);
}

size_t Gauge::size() const
{
    return groups.size();
}

Gauge::iterator Gauge::begin() 
{
    return groups.begin();
}

Gauge::const_iterator Gauge::begin()  const
{
    return groups.begin();
}

Gauge::reverse_iterator Gauge::rbegin() 
{
    return groups.rbegin();
}

Gauge::const_reverse_iterator Gauge::rbegin()  const
{
    return groups.rbegin();
}

Gauge::iterator Gauge::end() 
{
    return groups.end();
}

Gauge::const_iterator Gauge::end()  const
{
    return groups.end();
}

Gauge::reverse_iterator Gauge::rend() 
{
    return groups.rend();
}

Gauge::const_reverse_iterator Gauge::rend()  const
{
    return groups.rend();
}

SemiSimpleGroup* Gauge::operator[](int i) const
{
    return groups[i].get();
}

ostream& operator<<(ostream& fout, const Gauge& obj)
{
    for (size_t i = 0; i != obj.size(); ++i) {
        fout << *obj[i];
        if (i != obj.size()-1)
            fout << " x ";
    }
    
    return fout;
}

GaugeIrrep::GaugeIrrep(Gauge* t_gauge): gauge(t_gauge),
    rep(vector<Irrep>(t_gauge->size()))
{}

GaugeIrrep::GaugeIrrep(Gauge* t_gauge,
                       const vector<Irrep>& t_rep)
    :gauge(t_gauge), rep(t_rep)
{}

bool GaugeIrrep::isTrivial() const
{
    return *this == gauge->getTrivialRep();
}

ostream& operator<<(ostream& fout, const GaugeIrrep& irrep)
{
    fout << "( ";
    for (size_t i = 0; i != irrep.size(); ++i) {
        if (irrep[i].getAlgebra()
                and irrep[i].getAlgebra()->getType() != algebra::Type::R)
            fout << irrep[i].getDim();
        else if (irrep[i].getAlgebra()){
            // U(1) case:
            // i^th group
            // first rep (single)
            // first term of sum (single)
            // first term of product (single)
            AlgebraState s = irrep[i][0];
            fout << s[0];
            if (s[1] != 1)
                fout << "/" << s[1];
        }

        if (i != irrep.size() - 1)
            fout << " , ";
    }
    fout << " )";
    return fout;
}

GaugeIrrep GaugeIrrep::getConjugatedRep() const
{
    vector<Irrep> newReps;
    newReps.reserve(rep.size());
    for (const auto& r : rep)
        newReps.push_back(r.getConjugatedRep());

    return GaugeIrrep(gauge, newReps);
}

Gauge* GaugeIrrep::getGauge() const
{
    return gauge;
}

SumGaugeIrrep GaugeIrrep::operator*(const GaugeIrrep& other) const
{
    SumGaugeIrrep sum;
    const size_t nGroups = rep.size();
    vector<vector<Irrep> > res(0);
    if (other.size() == 0)
        return SumGaugeIrrep(gauge, {rep});
    if (size() == 0)
        return SumGaugeIrrep(gauge, {other.rep});
    SumIrrep foo = rep[0]*other[0]; 
    for (size_t i = 0; i != foo.size(); ++i) {
        res.push_back(vector<Irrep>(nGroups));
        res[i][0] = foo[i];
    }
    for (size_t i = 1; i != rep.size(); ++i) {
        foo = rep[i]*other[i];
        for (size_t j = 0; j != res.size(); ++j) 
            res[j][i] = foo[0];
        const size_t sizeRes = res.size();
        for (size_t j = res.size(); j != sizeRes*foo.size(); ++j) {
            res.push_back(res[j%sizeRes]);
            res[j][i] = foo[j/sizeRes];
        }
    }

    return SumGaugeIrrep(gauge, res);
}

SumGaugeIrrep GaugeIrrep::operator*(const SumGaugeIrrep& other) const
{
    return other*(*this);
}

bool GaugeIrrep::operator==(const GaugeIrrep& other) const
{
    if (gauge != other.gauge)
        return false;
    for (size_t i = 0; i != size(); ++i) 
        if (not gauge->getGaugedGroup(i)->isBroken() 
                and rep[i] != other[i])
            return false;
    return true;
}

SumGaugeIrrep::SumGaugeIrrep(): vector<GaugeIrrep>()
{
}

SumGaugeIrrep::SumGaugeIrrep(const vector<GaugeIrrep>& irreps)
    :vector<GaugeIrrep>(irreps)
{
    if (irreps.size() > 0) {
        gauge = irreps[0].getGauge();
        for (size_t i = 1; i != irreps.size(); ++i) 
            HEPAssert(gauge == irreps[i].getGauge(),
                    mty::error::ValueError,
                    "Sum of irreps from different gauges.")
    }
}

SumGaugeIrrep::SumGaugeIrrep(Gauge* t_gauge,
                             const vector<vector<Irrep> >& irreps)
    :gauge(t_gauge)
{
    for (const auto& el : irreps) 
        push_back(GaugeIrrep(gauge,el));
}

int SumGaugeIrrep::find(const GaugeIrrep& irrep) const
{
    for (size_t i = 0; i != size(); ++i) 
        if ((*this)[i] == irrep)
            return i;
    return -1;
}

bool SumGaugeIrrep::containsTrivialRep() const
{
    return find(gauge->getTrivialRep()) != -1;
}

SumGaugeIrrep SumGaugeIrrep::operator*(const GaugeIrrep& other) const
{
    SumGaugeIrrep res(gauge);
    for (const auto& gaugeRep : *this) 
        res = res + gaugeRep*other;

    return res;
}

SumGaugeIrrep SumGaugeIrrep::operator*(const SumGaugeIrrep& other) const
{
    SumGaugeIrrep res(gauge);
    for (const auto& elSelf : *this) 
        for (const auto& elOther : other) 
            res = res + elSelf*elOther;

    return res;
}

SumGaugeIrrep SumGaugeIrrep::operator+(const SumGaugeIrrep& other) const
{
    SumGaugeIrrep res(*this);
    for (const auto& repOther : other) 
        res.push_back(repOther);

    return res;
}

std::ostream& operator<<(
        std::ostream&     fout,
        const SumGaugeIrrep& irrep)
{
    for (size_t i = 0; i != irrep.size(); ++i) {
        fout << irrep[i];
        if (i != irrep.size()-1)
            fout << " + ";
    }
    return fout;
}

} // End of namespace mty
