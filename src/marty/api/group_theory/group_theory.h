#ifndef MARTY_API_GROUP_THEORY_H_INCLUDED
#define MARTY_API_GROUP_THEORY_H_INCLUDED

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "marty/core/representation.h"

/**
 * @file group_theory.h
 * @brief Typed API over the simple gauge groups of the MARTY core: group
 * construction, irreps, tensor products, Casimirs and Dynkin indices.
 *
 * Normalisation: Casimirs are the core values (SemiSimpleAlgebra::
 * getQuadraticCasimir), with a per-algebra root scale: the adjoint Casimir is 1
 * for SU(N) and 2 for E6, E7 and E8. The Dynkin index is T(r) = C2(r) dim(r) /
 * dim(G), so that T(fund SU(3)) = 1/6. This differs from the core getIndex for
 * SU(N) (1/2 for the fundamental); the two are not yet aligned.
 * E8 labels follow the MARTY manual: 248 = (0,...,0,1,0).
 *
 * Irreps keep their group alive through shared ownership, so an Irrep may
 * outlive the Group it was created from.
 *
 * Invalid arguments raise the typed errors of mty::api (see common/error.h).
 */

namespace mty {
class AbstractGroup;
}

namespace mty::api::group_theory {

class Group;
class Irrep;

/// Exact rational in lowest terms with a positive denominator.
struct Rational {
    long long num = 0;
    long long den = 1;

    Rational() = default;
    Rational(long long n, long long d);

    bool operator==(Rational const &other) const;
    bool operator<(Rational const &other) const;
};

class Irrep {
  public:
    Irrep(std::shared_ptr<const Group> group, mty::Irrep irrep);

    std::shared_ptr<const Group> const &group() const;

    int dim() const;
    /// Dynkin labels of the highest weight. Not defined for U(1).
    std::vector<int> labels() const;
    /// U(1) charge as an exact rational.
    Rational charge() const;

    /// Casimir in the core normalisation. NotImplementedError for U(1).
    Rational casimir() const;
    /// Dynkin index T(r) = C2(r) dim(r) / dim(G). NotImplementedError for
    /// U(1).
    Rational dynkinIndex() const;

    Irrep conjugate() const;
    bool  isSelfConjugate() const;

    bool operator==(Irrep const &other) const;
    bool operator<(Irrep const &other) const;

    mty::Irrep const &core() const
    {
        return irrep_;
    }

  private:
    std::shared_ptr<const Group> group_;
    mty::Irrep                   irrep_;
};

/// Decomposition of a tensor product: distinct irreps with multiplicities,
/// sorted by their highest-weight labels.
struct Decomposition {
    std::vector<std::pair<Irrep, int>> terms;

    /// Sum of multiplicity times dimension.
    int dim() const;
};

class Group : public std::enable_shared_from_this<Group> {
  public:
    static std::shared_ptr<const Group> su(int n);
    static std::shared_ptr<const Group> so(int n);
    static std::shared_ptr<const Group> sp(int n);
    static std::shared_ptr<const Group> e6();
    static std::shared_ptr<const Group> e7();
    static std::shared_ptr<const Group> e8();
    static std::shared_ptr<const Group> f4();
    static std::shared_ptr<const Group> g2();
    static std::shared_ptr<const Group> u1();

    std::string name() const;
    /// Rank of the group (order of the Cartan algebra).
    int rank() const;
    /// Dimension of the group, read from the adjoint representation.
    int  dim() const;
    bool isU1() const;

    /// Irrep from Dynkin labels. Its length must equal rank().
    Irrep irrep(std::vector<int> const &labels) const;
    /// U(1) irrep of the given charge. Only for U(1).
    Irrep u1Irrep(Rational const &charge) const;

    Irrep fundamental() const;
    Irrep adjoint() const;
    Irrep trivial() const;

    Decomposition tensorProduct(Irrep const &a, Irrep const &b) const;

    std::shared_ptr<mty::AbstractGroup> const &core() const
    {
        return core_;
    }

  private:
    Group(std::shared_ptr<mty::AbstractGroup> core, std::string name);

    std::shared_ptr<mty::AbstractGroup> core_;
    std::string                         name_;
};

} // namespace mty::api::group_theory

#endif
