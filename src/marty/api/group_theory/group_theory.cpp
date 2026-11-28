#include "group_theory.h"

#include "marty/api/common/error.h"
#include "marty/core/group.h"
#include "marty/core/semiSimpleAlgebra.h"

#include <functional>
#include <limits>
#include <map>
#include <mutex>
#include <numeric>
#include <sstream>

namespace mty::api::group_theory {
namespace {

template <class F>
auto guarded(F &&f)
{
    try {
        return f();
    }
    catch (...) {
        rethrowAsApiError();
    }
}

std::vector<int> highestWeight(mty::Irrep const &irrep)
{
    return irrep.getHighestWeight().getLabels();
}

long long checkedInt(long long value, char const *what)
{
    if (value > std::numeric_limits<int>::max()
        or value < std::numeric_limits<int>::min())
        throw ValueError(std::string(what)
                         + " is out of range: " + std::to_string(value));
    return value;
}

Rational toRational(csl::Expr const &expr)
{
    if (csl::IsIntFraction(expr))
        return Rational(expr->getNum(), expr->getDenom());
    if (csl::IsInteger(expr))
        return Rational(std::llround(expr->evaluateScalar()), 1);
    std::ostringstream sout;
    sout << expr;
    throw ValueError("Expected an exact rational, got " + sout.str());
}

std::shared_ptr<const Group>
cached(std::string const                                   &key,
       std::function<std::shared_ptr<const Group>()> const &make)
{
    static std::mutex                                          mutex;
    static std::map<std::string, std::shared_ptr<const Group>> groups;
    std::lock_guard<std::mutex>                                lock(mutex);
    auto found = groups.find(key);
    if (found != groups.end())
        return found->second;
    auto group = make();
    groups.emplace(key, group);
    return group;
}

} // namespace

Rational::Rational(long long n, long long d)
{
    if (d == 0)
        throw ValueError("Rational with zero denominator");
    if (d < 0) {
        n = -n;
        d = -d;
    }
    long long g = std::gcd(n < 0 ? -n : n, d);
    if (g == 0)
        g = 1;
    num = n / g;
    den = d / g;
}

bool Rational::operator==(Rational const &other) const
{
    return num == other.num and den == other.den;
}

bool Rational::operator<(Rational const &other) const
{
    return num * other.den < other.num * den;
}

int Decomposition::dim() const
{
    int total = 0;
    for (auto const &[irrep, mult] : terms)
        total += mult * irrep.dim();
    return total;
}

Irrep::Irrep(std::shared_ptr<const Group> group, mty::Irrep irrep)
    : group_(std::move(group)), irrep_(std::move(irrep))
{
}

std::shared_ptr<const Group> const &Irrep::group() const
{
    return group_;
}

int Irrep::dim() const
{
    return irrep_.getDim();
}

std::vector<int> Irrep::labels() const
{
    return guarded([&] {
        if (group_->isU1())
            throw ValueError("U(1) irreps are labelled by their charge");
        return irrep_.getHighestWeight().getLabels();
    });
}

Rational Irrep::charge() const
{
    return guarded([&] { return toRational(irrep_.getCharge()); });
}

Rational Irrep::casimir() const
{
    return guarded([&] {
        if (group_->isU1())
            throw NotImplementedError("Casimir of U(1) irreps is not "
                                      "implemented in the core");
        auto algebra = group_->core()->getAlgebra();
        return toRational(
            algebra->getQuadraticCasimir(irrep_.getHighestWeight()));
    });
}

Rational Irrep::dynkinIndex() const
{
    return guarded([&] {
        if (group_->isU1())
            throw NotImplementedError("Dynkin index of U(1) irreps is not "
                                      "implemented in the core");
        Rational c = casimir();
        Rational scaled(c.num * dim(), c.den);
        return Rational(scaled.num, scaled.den * group_->dim());
    });
}

Irrep Irrep::conjugate() const
{
    return guarded([&] { return Irrep(group_, irrep_.getConjugatedRep()); });
}

bool Irrep::isSelfConjugate() const
{
    return highestWeight(irrep_) == highestWeight(irrep_.getConjugatedRep());
}

bool Irrep::operator==(Irrep const &other) const
{
    return irrep_ == other.irrep_;
}

bool Irrep::operator<(Irrep const &other) const
{
    return irrep_ < other.irrep_;
}

Group::Group(std::shared_ptr<mty::AbstractGroup> core,
             std::string                         name,
             mty::group::Type                    kind,
             int                                 size)
    : core_(std::move(core)), name_(std::move(name)), kind_(kind), size_(size)
{
}

std::shared_ptr<const Group> Group::su(int n)
{
    return guarded([&] {
        if (n < 2)
            throw ValueError("SU(n) requires n >= 2, got "
                             + std::to_string(n));
        return cached("SU" + std::to_string(n), [n] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::SU>(n),
                          "SU(" + std::to_string(n) + ")",
                          mty::group::Type::SU,
                          n));
        });
    });
}

std::shared_ptr<const Group> Group::so(int n)
{
    return guarded([&] {
        // The core aborts on SO(3) and below, so reject them here.
        if (n < 4)
            throw ValueError("SO(n) requires n >= 4, got "
                             + std::to_string(n));
        return cached("SO" + std::to_string(n), [n] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::SO>(n),
                          "SO(" + std::to_string(n) + ")",
                          mty::group::Type::SO,
                          n));
        });
    });
}

std::shared_ptr<const Group> Group::sp(int n)
{
    return guarded([&] {
        if (n < 4 || n % 2 != 0)
            throw ValueError("Sp(n) requires an even fundamental dimension "
                             ">= 4, got "
                             + std::to_string(n));
        return cached("Sp" + std::to_string(n), [n] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::Sp>(n),
                          "Sp(" + std::to_string(n) + ")",
                          mty::group::Type::Sp,
                          n));
        });
    });
}

std::shared_ptr<const Group> Group::e6()
{
    return guarded([&] {
        return cached("E6", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::E6>(),
                          "E6",
                          mty::group::Type::E6));
        });
    });
}

std::shared_ptr<const Group> Group::e7()
{
    return guarded([&] {
        return cached("E7", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::E7>(),
                          "E7",
                          mty::group::Type::E7));
        });
    });
}

std::shared_ptr<const Group> Group::e8()
{
    return guarded([&] {
        return cached("E8", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::E8>(),
                          "E8",
                          mty::group::Type::E8));
        });
    });
}

std::shared_ptr<const Group> Group::f4()
{
    return guarded([&] {
        return cached("F4", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::F4>(),
                          "F4",
                          mty::group::Type::F4));
        });
    });
}

std::shared_ptr<const Group> Group::g2()
{
    return guarded([&] {
        return cached("G2", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::G2>(),
                          "G2",
                          mty::group::Type::G2));
        });
    });
}

std::shared_ptr<const Group> Group::u(int n)
{
    return guarded([&] {
        if (n != 1)
            throw ValueError("U(n) is not supported, only U(1) is, got n = "
                             + std::to_string(n));
        return cached("U1", [] {
            return std::shared_ptr<const Group>(
                new Group(std::make_shared<mty::group::U1>(),
                          "U(1)",
                          mty::group::Type::U,
                          1));
        });
    });
}

std::shared_ptr<const Group>
Group::wrap(std::shared_ptr<mty::AbstractGroup> core)
{
    return guarded([&] {
        auto kind = core->getType();
        if (kind == mty::group::Type::U1)
            kind = mty::group::Type::U;
        std::string name;
        int         size = -1;
        switch (kind) {
        case mty::group::Type::U:
            name = "U(1)";
            size = 1;
            break;
        case mty::group::Type::SU:
        case mty::group::Type::SO:
        case mty::group::Type::Sp:
            size = core->getDim();
            name = (kind == mty::group::Type::SU   ? "SU("
                    : kind == mty::group::Type::SO ? "SO("
                                                   : "Sp(")
                   + std::to_string(size) + ")";
            break;
        case mty::group::Type::E6:
            name = "E6";
            break;
        case mty::group::Type::E7:
            name = "E7";
            break;
        case mty::group::Type::E8:
            name = "E8";
            break;
        case mty::group::Type::F4:
            name = "F4";
            break;
        case mty::group::Type::G2:
            name = "G2";
            break;
        default:
            throw NotImplementedError("the API does not expose this group "
                                      "kind");
        }
        return std::shared_ptr<const Group>(
            new Group(std::move(core), name, kind, size));
    });
}

std::string Group::name() const
{
    return name_;
}

int Group::rank() const
{
    return guarded([&] { return core_->getAlgebra()->getOrderL(); });
}

int Group::dim() const
{
    return guarded([&] { return core_->getAdjointRep().getDim(); });
}

bool Group::isU1() const
{
    return name_ == "U(1)";
}

Irrep Group::irrep(std::vector<int> const &labels) const
{
    return guarded([&] {
        if (isU1())
            throw ValueError("U(1) irreps are created with u1Irrep()");
        if (static_cast<int>(labels.size()) != rank())
            throw ValueError(name_ + " irreps take " + std::to_string(rank())
                             + " Dynkin labels, got "
                             + std::to_string(labels.size()));
        for (int label : labels)
            if (label < 0)
                throw ValueError("Dynkin labels must be non-negative");
        auto core = core_->getAlgebra()->highestWeightRep(labels);
        return Irrep(shared_from_this(), core);
    });
}

Irrep Group::u1Irrep(Rational const &charge) const
{
    return guarded([&] {
        if (!isU1())
            throw ValueError(name_ + " has no charge labels");
        auto num = static_cast<int>(
            checkedInt(charge.num, "U(1) charge numerator"));
        auto den = static_cast<int>(
            checkedInt(charge.den, "U(1) charge denominator"));
        auto core = core_->getAlgebra()->highestWeightRep(
            std::vector<int>{num, den});
        return Irrep(shared_from_this(), core);
    });
}

Irrep Group::fundamental() const
{
    return guarded([&] {
        // The core's G2 defining representation is the 14; the 7 is {1,0}.
        if (name_ == "G2")
            return Irrep(shared_from_this(),
                         core_->getAlgebra()->highestWeightRep({1, 0}));
        return Irrep(shared_from_this(),
                     core_->getAlgebra()->getDefiningRep());
    });
}

Irrep Group::adjoint() const
{
    return guarded(
        [&] { return Irrep(shared_from_this(), core_->getAdjointRep()); });
}

Irrep Group::trivial() const
{
    return guarded([&] {
        return Irrep(shared_from_this(), core_->getAlgebra()->getTrivialRep());
    });
}

Decomposition Group::tensorProduct(Irrep const &a, Irrep const &b) const
{
    return guarded([&] {
        if (a.group() != shared_from_this() or b.group() != shared_from_this())
            throw TypeError("Irreps do not belong to group " + name_);
        // The core operator< compares dimensions only, so irreps are keyed
        // by their highest weight (10 and 10bar of SU(5) stay distinct).
        std::map<std::vector<int>, std::pair<mty::Irrep, int>> counts;
        for (auto const &irrep :
             core_->getAlgebra()->tensorProduct(a.core(), b.core())) {
            auto &entry = counts[highestWeight(irrep)];
            entry.first = irrep;
            ++entry.second;
        }
        Decomposition result;
        for (auto const &[key, entry] : counts)
            result.terms.emplace_back(Irrep(shared_from_this(), entry.first),
                                      entry.second);
        return result;
    });
}

} // namespace mty::api::group_theory
