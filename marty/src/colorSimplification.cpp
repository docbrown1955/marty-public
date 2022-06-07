#include "colorSimplification.h"
#include "gaugedGroup.h"

namespace mty {

static std::set<color::ColorSpace const*> getColorSpaces(
        csl::Expr const &expr
        )
{
    std::set<color::ColorSpace const*> colors;
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub)) {
            std::vector<csl::Space const*> space = sub->getParent()->getSpace();
            if (space.size() != 3
                    or space[0] == space[1]
                    or space[1] != space[2])
                return;
            auto const &pointed = *space[0];
            if (typeid(color::ColorSpace) == typeid(pointed)
                    and typeid(color::ColorSpace) == typeid(pointed))
                colors.insert(
                        static_cast<color::ColorSpace const*>(space[1]));
        }
    });
    return colors;
}

static std::set<color::ColorSpace const*> getAdjointSpaces(
        csl::Expr const &expr
        )
{
    std::set<color::ColorSpace const*> colors;
    csl::VisitEachLeaf(expr, [&](csl::Expr const &sub)
    {
        if (csl::IsIndicialTensor(sub)) {
            std::vector<csl::Space const*> space = sub->getParent()->getSpace();
            if (space.size() != 3
                    or space[0] != space[1]
                    or space[1] != space[2])
                return;
            auto const &pointed = *space[0];
            if (typeid(color::ColorSpace) == typeid(pointed)
                    and typeid(color::ColorSpace) == typeid(pointed))
                colors.insert(
                        static_cast<color::ColorSpace const*>(space[1]));
        }
    });
    return colors;
}

static std::vector<std::size_t> getPos_fABC(
        csl::Expr &prod,
        color::ColorSpace const *space
        )
{
    std::vector<std::size_t> pos_fABC;
    const std::size_t sz = prod->size();
    csl::Tensor f = space->getF();
    pos_fABC.reserve(sz);
    for (std::size_t i = 0; i != sz; ++i) {
        if (csl::IsIndicialTensor(prod[i])
                && f.get() == prod[i]->getParent_info()) {
            pos_fABC.push_back(i);
        }
    }
    return pos_fABC;
}

static bool hasCommonIndex(
        csl::Expr const &tensorA,
        csl::Expr const &tensorB
        )
{
    for (const auto &i : tensorA->getIndexStructureView())
        for (const auto &j : tensorB->getIndexStructureView())
            if (i == j)
                return true;
    return false;
}

static bool isFTrace(
        csl::Expr const &tensorA,
        csl::Expr const &tensorB,
        csl::Expr const &tensorC
        )
{
    return hasCommonIndex(tensorA, tensorB)
        && hasCommonIndex(tensorB, tensorC)
        && hasCommonIndex(tensorC, tensorA);
}

static int placeCommonIndex(
        csl::IndexStructure &si,
        csl::IndexStructure &sj,
        std::size_t posIni,
        std::size_t posInj
        )
{
    int sign = 1;
    for (std::size_t i = 0; i != si.size(); ++i) { // index i1
        if (auto posj = sj.find(si[i]); posj != sj.end()) { 
            if (i != posIni) {
                sign *= -1;
                std::swap(si[i], si[1]);
            }
            if (posj - sj.begin() != static_cast<long>(posInj)) {
                sign *= -1;
                std::swap(sj[0], *posj);
            }
        }
    }
    return sign;
}

static csl::Expr fTrace(
        csl::Expr const &fi,
        csl::Expr const &fj,
        csl::Expr const &fk,
        mty::GaugedGroup const *group
        )
{
    // Equation (44) of hep-ph/9802376v1
    // Sorting indices as 
    // f_{i1 i2 a} * f_{i2 i3 b} * f_{i3 i1 c} to determine the sign
    csl::IndexStructure si = fi->getIndexStructure();
    csl::IndexStructure sj = fj->getIndexStructure();
    csl::IndexStructure sk = fk->getIndexStructure();
    int sign = 1;
    sign *= placeCommonIndex(si, sj, 1, 0);
    sign *= placeCommonIndex(sj, sk, 1, 0);
    sign *= placeCommonIndex(sk, si, 1, 0);
    csl::Index a = si[2];
    csl::Index b = sj[2];
    csl::Index c = sk[2];
    csl::Parent f = fi->getParent();
    csl::Expr CA = group->getCA();
    return sign * CSL_HALF * CA * f({a, b, c});
}

static bool CalculateFColorTrace_impl(
        csl::Expr               &prod,
        color::ColorSpace const *space
        )
{
    HEPAssert(csl::IsProd(prod),
            mty::error::TypeError,
            "Expected a product, " + toString(prod) + " given.")
    std::vector<std::size_t> pos_fABC = getPos_fABC(prod, space);
    if (pos_fABC.size() < 3)
        return false;
    for (std::size_t i = 0; i != pos_fABC.size(); ++i) {
        auto &fi = prod[pos_fABC[i]];
        for (std::size_t j = i+1; j < pos_fABC.size(); ++j) {
            auto &fj = prod[pos_fABC[j]];
            for (std::size_t k = j+1; k < pos_fABC.size(); ++k) {
                auto &fk = prod[pos_fABC[k]];
                if (isFTrace(fi, fj, fk)) {
                    fi = fTrace(fi, fj, fk, space->getGroup()->getGaugedGroup());
                    fj = fk = CSL_1;
                    return true;
                }
            }
        }
    }
    return false;
}

csl::Expr CalculateFColorTrace(
        csl::Expr const &expr
        )
{
    csl::Expr res = csl::DeepCopy(expr);
    auto colors = getAdjointSpaces(expr);
    csl::Transform(res, [&](csl::Expr &sub) {
        if (csl::IsProd(sub)) {
            for (const auto &c : colors)
                return CalculateFColorTrace_impl(sub, c);
        }
        return false;
    });
    return res;
}


csl::Expr CalculateTrace(const mty::SemiSimpleGroup* colorGroup,
                    const csl::Expr&                 expr)
{
    csl::ScopedProperty p(&csl::option::applyChainContractions, true);
    std::vector<const csl::Space*> spaces = colorGroup->getAllVectorSpace();
    for (const auto& space : spaces)
        space->keepCycles = true;
    csl::Expr res = DeepRefreshed(expr);
    for (const auto& space : spaces)
        space->keepCycles = false;

    return res;
}

csl::Expr CalculateColorTrace(csl::Expr const &init)
{
    csl::ScopedProperty p1(&csl::option::applyChainContractions, true);
    auto colors = getColorSpaces(init);
    for (auto &c : colors)
        c->keepCycles = true;
    csl::Expr res = csl::DeepRefreshed(init);
    for (auto &c : colors)
        c->keepCycles = false;

    res = CalculateFColorTrace(res);

    return res;
}

} // namespace mty
