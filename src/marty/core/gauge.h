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
 * @version 2.0

 * \brief
 */
#ifndef GAUGE_H_INCLUDED
#define GAUGE_H_INCLUDED

#include "gaugedGroup.h"
#include "group.h"

namespace mty {

class GaugeIrrep;
class SumGaugeIrrep;
class GaugedGroup;
class QuantumField;
class Particle;
class Gauge {

    using iterator = std::vector<std::unique_ptr<SemiSimpleGroup>>::iterator;
    using const_iterator
        = std::vector<std::unique_ptr<SemiSimpleGroup>>::const_iterator;
    using reverse_iterator
        = std::vector<std::unique_ptr<SemiSimpleGroup>>::reverse_iterator;
    using const_reverse_iterator = std::vector<
        std::unique_ptr<SemiSimpleGroup>>::const_reverse_iterator;

  protected:
    std::string name;

    std::vector<std::unique_ptr<SemiSimpleGroup>> groups;

    std::vector<std::unique_ptr<GaugedGroup>> gaugedGroups;

  public:
    explicit Gauge(std::string const &t_name = "");

    ~Gauge();

    void addGroup(std::unique_ptr<SemiSimpleGroup> &t_group,
                  const csl::Expr                  &coupling = nullptr);

    void addGroup(std::unique_ptr<SemiSimpleGroup> &t_group,
                  std::string const                &nameBoson,
                  const csl::Expr                  &coupling = nullptr);

    void addGroup(std::unique_ptr<SemiSimpleGroup> &t_group,
                  std::string const                &nameBoson,
                  bool                              addGhost,
                  const csl::Expr                  &coupling = nullptr);

    void addGroup(group::Type type, std::string const &name, int dim = -1)
    {
        auto group = createGroup(type, name, dim);
        addGroup(group);
    }

    GaugeIrrep
    getRepresentation(const std::vector<std::vector<int>> &highestWeights);

    GaugeIrrep getTrivialRep();

    bool containsTrivialRep(std::vector<QuantumField> const &fields) const;

    SumGaugeIrrep tensorProduct(const GaugeIrrep &A,
                                const GaugeIrrep &B) const;

    csl::Expr covariantDerivative(QuantumFieldParent            &field,
                                  const csl::Index              &mu,
                                  const std::vector<csl::Index> &fieldIndices,
                                  csl::Tensor                   &point);

    std::optional<csl::Expr> getCoupling(const std::string &t_name) const;

    std::vector<Particle> getVectorBosons() const;

    std::vector<Particle> getGhosts() const;

    GaugedGroup *getGaugedGroup(size_t pos) const;

    void setGaugeChoice(size_t pos, gauge::Type choice);

    size_t size() const;

    iterator begin();

    const_iterator begin() const;

    reverse_iterator rbegin();

    const_reverse_iterator rbegin() const;

    iterator end();

    const_iterator end() const;

    reverse_iterator rend();

    const_reverse_iterator rend() const;

    SemiSimpleGroup *operator[](int i) const;

    friend std::ostream &operator<<(std::ostream &fout, const Gauge &obj);
};

class GaugeIrrep {

    IMPLEMENTS_STD_VECTOR(Irrep, rep);

  protected:
    Gauge                  *gauge;
    std::vector<mty::Irrep> rep;

  public:
    GaugeIrrep() : gauge(nullptr)
    {
    }

    explicit GaugeIrrep(Gauge *gauge);

    explicit GaugeIrrep(Gauge *t_gauge, const std::vector<Irrep> &t_rep);

    ~GaugeIrrep() {};

    bool isTrivial() const;

    friend std::ostream &operator<<(std::ostream     &fout,
                                    const GaugeIrrep &irrep);

    GaugeIrrep getConjugatedRep() const;

    Gauge *getGauge() const;

    SumGaugeIrrep operator*(const GaugeIrrep &other) const;

    SumGaugeIrrep operator*(const SumGaugeIrrep &other) const;

    bool operator==(const GaugeIrrep &other) const;
};

class SumGaugeIrrep : public std::vector<GaugeIrrep> {

  protected:
    Gauge *gauge;

  public:
    SumGaugeIrrep();
    explicit SumGaugeIrrep(Gauge                                 *t_gauge,
                           const std::vector<std::vector<Irrep>> &irreps = {});
    explicit SumGaugeIrrep(const std::vector<GaugeIrrep> &irreps);
    SumGaugeIrrep(const SumGaugeIrrep &other) = default;
    ~SumGaugeIrrep() {};

    int  find(const GaugeIrrep &irrep) const;
    bool containsTrivialRep() const;

    SumGaugeIrrep operator*(const GaugeIrrep &other) const;
    SumGaugeIrrep operator*(const SumGaugeIrrep &other) const;
    SumGaugeIrrep operator+(const SumGaugeIrrep &other) const;

    friend std::ostream &operator<<(std::ostream        &fout,
                                    const SumGaugeIrrep &irrep);
};

} // End of namespace mty

#endif
