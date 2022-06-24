#include <marty.h>
#include "testutility.h"

class DataHandler {

public:

    csl::Tensor getP(std::string const &name)
    {
        if (name.empty()) 
            return nullptr;
        if (auto pos = p.find(name); pos != p.end()) {
            return pos->second;
        }
        return p[name] = csl::Tensor(name, &csl::Minkowski);
    }

    csl::Expr getM(std::string const &name)
    {
        if (name.empty())
            return CSL_0;
        if (auto pos = m.find(name); pos != m.end()) {
            return pos->second;
        }
        return m[name] = csl::constant_s(name);
    }

    void print() const 
    {
        std::cout << p.size() << " momenta" << '\n';
        for (const auto &pi : p)
            std::cout << pi.second->getName() << '\n';
        std::cout << m.size() << " masses" << '\n';
        for (const auto &mi : m)
            std::cout << mi.second->getName() << '\n';
    }

private:

    std::map<std::string, csl::Tensor> p;
    std::map<std::string, csl::Expr>   m;
};

csl::Expr squared(csl::Tensor p)
{
    if (p) {
        csl::Index mu = mty::MinkowskiIndex();
        return p(mu) * p(+mu);
    }
    return CSL_0;
}

csl::Expr squared(csl::Expr const &m)
{
    return csl::pow_s(m, 2);
}

template<class ...Momenta>
csl::Expr propagator(
        DataHandler       &data,
        std::string const &p,
        std::string const &m,
        Momenta          &&...ki
        )
{
    csl::Index mu = mty::MinkowskiIndex();
    return mty::propagator_s(
            data.getP(p)(mu) - csl::sum_s({data.getP(ki)(mu) ...}), 
            data.getM(m)
            );
}

csl::Expr indexed(csl::Tensor p)
{
    return p ? p(mty::MinkowskiIndex()) : CSL_0;
}

template<class ...Momenta>
csl::Expr numerator(
        DataHandler &data,
        Momenta    &&...momenta
        )
{
    return csl::prod_s({indexed(data.getP(momenta)) ...});
}

template<class ...Args>
csl::Expr integral(
        DataHandler       &data,
        std::string const &variable,
        Args             &&...args
        )
{
    return csl::vectorintegral_s(
            csl::prod_s({std::forward<Args>(args)...}),
            data.getP(variable)
            );
}

csl::Expr reduced(csl::Expr const &init)
{
    csl::Expr cpy = csl::DeepCopy(init);
    csl::ForEachNode(cpy, [&](csl::Expr &node) {
        if (csl::IsVectorIntegral(node)) {
            std::cout << node << '\n';
            node = mty::FeynmanIntegral::replaceIntegral(node);
        }
    });
    return csl::DeepRefreshed(cpy);
}

auto areEqual(
        std::function<bool(csl::Expr const&, csl::Expr const&)> f = std::equal_to<csl::Expr>{}
        )
{
    return [f = f](csl::Expr const &A, csl::Expr const &B)
    {
        if (f(A, B)) {
            return Result { true, A, " and ", B, " are equal !" };
        }
        return Result { false, A, " and ", B, " are different !" };
    };
}

bool hardComparison(csl::Expr const &A, csl::Expr const &B)
{
    auto freeA = A->getFreeIndexStructure();
    auto freeB = B->getFreeIndexStructure();
    if (freeA.size() != freeB.size())
        return false;
    csl::Expr B_transf = csl::Replaced(B, freeB, freeA);
    return A->compareWithDummy(B_transf.get());
}

int main() {

    mty::option::displayIntegralArgs = false;

    DataHandler data;
    auto mmu = mty::MinkowskiIndices(10);

    csl::Expr I1 = integral(
            data, 
            "p",
            numerator(data, "p"),
            propagator(data, "p", "m1")
            );
    csl::Expr I2 = integral(
            data, 
            "p",
            numerator(data, "p"),
            propagator(data, "p", "m1"),
            propagator(data, "p", "m2", "k")
            );
    auto k = data.getP("k");
    auto m1 = data.getM("m1");
    auto m2 = data.getM("m2");
    csl::Expr res2 = -k(mmu[0]) * mty::feynmanintegral_s(
            mty::IntegralType::B, 3, 
            {squared(k), squared(m1), squared(m2)}
            );
    csl::Expr I3 = integral(
            data, 
            "p",
            numerator(data, "p"),
            propagator(data, "p", "m1"),
            propagator(data, "p", "m2")
            );
    csl::Expr I4 = integral(
            data, 
            "p",
            numerator(data, "p"),
            propagator(data, "p", "m1"),
            propagator(data, "p", "m2"),
            propagator(data, "p", "m3")
            );
    csl::Expr I5 = integral(
            data, 
            "p",
            numerator(data, "p"),
            propagator(data, "p", "m1"),
            propagator(data, "p", "m2", "k1"),
            propagator(data, "p", "m3", "k2")
            );
    auto k1 = data.getP("k1");
    auto k2 = data.getP("k2");
    auto k3 = data.getP("k3");
    auto m3 = data.getM("m3");
    std::vector<csl::Expr> argC = {
        squared(k1), 
        (k1(mmu[0]) - k2(mmu[0])) * (k1(+mmu[0]) - k2(+mmu[0])),
        squared(k2),
        squared(m1), 
        squared(m2),
        squared(m3)
    };
    auto Ci = [&](int i, auto &&args) {
        return mty::feynmanintegral_s(mty::IntegralType::C, i, args);
    };
    csl::Expr res5 =
        -k1(mmu[0]) * Ci(3, argC)
        -k2(mmu[0]) * Ci(6, argC);

    auto g = csl::Minkowski.getMetric();

    const auto tester = areEqual(hardComparison);

    int res = test(buildMessage("Reduction of ", I1, " to 0"), tester, reduced(I1), CSL_0);
    res |= test(buildMessage("Reduction of ", I2, " to 0"), tester, reduced(I2), res2);
    res |= test(buildMessage("Reduction of ", I3, " to 0"), tester, reduced(I3), CSL_0);
    res |= test(buildMessage("Reduction of ", I4, " to 0"), tester, reduced(I4), CSL_0);
    res |= test(buildMessage("Reduction of ", I5, " to 0"), tester, reduced(I5), res5);

    return res;
}
