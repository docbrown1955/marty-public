#include "gamma.h"
#include "marty/sgl/sgl.h"
#include "marty/sgl/sgloptions.h"

#include <iostream>
#include <map>
#include <set>

namespace mty::gamma_api {

class IndexManager {

  public:
    static csl::Index getMinkoIndex(int mu);
    static csl::Index getDiracIndex(int mu);
    static Expr       renameIndices(Expr expr);

    static bool isMinkoIndexName(std::string_view indexName);

  private:
    IndexManager() = delete;
    static std::string createDiracIndexName(int mu);
    static std::string createMinkoIndexName(int mu);
    static void        renameIndicesImpl(Expr                             &expr,
                                         std::map<csl::Index, csl::Index> &renaming,
                                         std::set<int> &goodIndices);

    static constexpr char                   minkoIndexPrefix = 'm';
    static constexpr char                   diracIndexPrefix = 'd';
    static inline std::map<int, csl::Index> diracIndices     = {};
    static inline std::map<int, csl::Index> minkoIndices     = {};
};

Expr DMinko = sgl::cslexpr_s(csl::DMinko);

void keepSymbolic4D(bool symbolic)
{
    sgl::option::keepEvanescentOperators = symbolic;
}

Expr g(int mu, int nu)
{
    return sgl::metricindex_s(IndexManager::getMinkoIndex(mu),
                              IndexManager::getMinkoIndex(nu));
}

Expr eps(int mu, int nu, int rho, int sig)
{
    return sgl::epsilonindex_s(IndexManager::getMinkoIndex(mu),
                               IndexManager::getMinkoIndex(nu),
                               IndexManager::getMinkoIndex(rho),
                               IndexManager::getMinkoIndex(sig));
}

Expr gamma(int mu)
{
    return sgl::gammaindex_s(IndexManager::getMinkoIndex(mu));
}

Expr gamma(int mu, int nu)
{
    return sgl::gammaindex_s(
        {IndexManager::getMinkoIndex(mu), IndexManager::getMinkoIndex(nu)});
}

Expr gamma5()
{
    return sgl::gammaindex_s(5);
}

Expr P_L()
{
    return sgl::gammaindex_s(6);
}

Expr P_R()
{
    return sgl::gammaindex_s(7);
}

Expr C()
{
    return sgl::gammaindex_s(8);
}

Expr current(std::vector<Expr> const &gammas, int iLeft, int iRight)
{
    return sgl::indexchain_s(gammas,
                             IndexManager::getDiracIndex(iLeft),
                             IndexManager::getDiracIndex(iRight));
}

Expr trace(std::vector<Expr> const &gammas)
{
    return simplified(current(gammas, 0, 0));
}

Expr relative_chain_order(Expr expr)
{
    auto mustSwap = [](Expr const &l, Expr const &r) {
        auto lchain = dynamic_cast<sgl::IndexChain const *>(l.get());
        auto rchain = dynamic_cast<sgl::IndexChain const *>(r.get());
        if (!lchain || !rchain) {
            return false;
        }
        auto [al, bl] = lchain->getBorderIndices();
        auto [ar, br] = rchain->getBorderIndices();
        return csl::IndexStructure({ar, br}) < csl::IndexStructure({al, bl});
    };
    if (dynamic_cast<sgl::Prod const *>(expr.get())) {
        for (std::size_t i = 0; i != expr->size(); ++i) {
            for (std::size_t j = i + 1; j < expr->size(); ++j) {
                if (mustSwap(expr->argument(i), expr->argument(j))) {
                    std::swap(expr->argument(i), expr->argument(j));
                }
            }
        }
    }
    else {
        for (std::size_t i = 0; i != expr->size(); ++i) {
            relative_chain_order(expr->argument(i));
        }
    }
    return expr;
}

Expr simplified(Expr const &expr)
{
    Expr clean = IndexManager::renameIndices(relative_chain_order(
        sgl::CSLSimplified(sgl::Simplified(expr, false))));
    return IndexManager::renameIndices(sgl::Simplified(clean, false));
}

Expr ordered(Expr const &expr)
{
    auto cpy = expr->copy();
    sgl::OrderChains(cpy);
    Expr clean = IndexManager::renameIndices(
        relative_chain_order(sgl::CSLSimplified(cpy)));
    return clean;
}

Expr project(Expr const &expr, FierzBasis basis)
{
    Expr res = expr->copy();
    sgl::transform<sgl::IndexChain>(res, [&](sgl::GExpr &el) {
        auto current = sgl::ConvertTo<sgl::IndexChain>(el);
        if (!current->isOnBasis(basis == FierzBasis::Chiral)) {
            auto transfo
                = current->projectOnBasis(basis == FierzBasis::Chiral);
            if (transfo) {
                el = transfo.value();
                return true;
            }
        }
        return false;
    });
    return simplified(res);
}

std::pair<Expr, sgl::IndexChain const *> getIndexChainAssert(Expr const &expr);

Expr applySingleFierz(Expr const &chain1, Expr const &chain2, FierzBasis basis)
{
    auto [fl, left]  = getIndexChainAssert(chain1);
    auto [fr, right] = getIndexChainAssert(chain2);
    return fl * fr
           * IndexManager::renameIndices(
               project(simplified(left->applyGeneralFierz(
                           *right, basis == FierzBasis::Chiral)),
                       basis));
}

Expr applyDoubleFierz(Expr const &chain1, Expr const &chain2, FierzBasis basis)
{
    auto [fl, left]  = getIndexChainAssert(chain1);
    auto [fr, right] = getIndexChainAssert(chain2);
    return fl * fr
           * IndexManager::renameIndices(
               project(simplified(left->applyGeneralFierzTwice(
                           *right, basis == FierzBasis::Chiral)),
                       basis));
}

std::pair<Expr, sgl::IndexChain const *> getIndexChainAssert(Expr const &expr)
{
    if (!expr.get()) {
        throw GammaAPIError("Invalid expression error (null).");
    }
    auto ptr = dynamic_cast<sgl::IndexChain const *>(expr.get());
    if (!ptr) {
        if (auto prod = dynamic_cast<sgl::Prod const *>(expr.get());
            !prod || prod->size() != 2) {
            throw GammaAPIError("Expression given is not a pure gamma-index "
                                "current: ",
                                expr);
        }
        else {
            auto expr
                = dynamic_cast<sgl::CSLExpr const *>(prod->argument(0).get());
            auto current = dynamic_cast<sgl::IndexChain const *>(
                prod->argument(1).get());
            if (!expr || !current) {
                throw GammaAPIError("Expression given is not a pure "
                                    "gamma-index "
                                    "current: ",
                                    expr);
            }
            return {expr->copy(), current};
        }
    }
    return {sgl::cslexpr_s(CSL_1), ptr};
}

////
// Standard printing
////

std::string generateString(Expr const &expr)
{
    std::ostringstream sout;
    sout << expr;
    return sout.str();
}

////
// Latex conversion
////

static std::string generateLatexImpl(Expr const &expr);

static std::string generateLatex(csl::Expr const &expr)
{
    return expr->printLaTeX(0);
}

static std::string generateLatex(sgl::AbstractMultiFunction const *func,
                                 std::string const                &sep)
{
    std::string res;
    for (std::size_t i = 0; i != func->size(); ++i) {
        res += generateLatexImpl(func->argument(i));
        if (!sep.empty() && i + 1 < func->size()) {
            res += sep;
        }
    }
    return res;
}

static std::string generateLatex(csl::Index const &index)
{
    std::string_view name = index.getName();
    return "\\mu_{" + std::string(name.begin() + 1, name.end()) + "}";
}

static std::string generateLatex(std::vector<csl::Index> const &indices)
{
    std::string res;
    for (const auto &index : indices) {
        res += generateLatex(index);
    }
    return res;
}

static std::string generateLatex(sgl::GammaIndex const *index)
{
    if (index->isDelta()) {
        return "";
    }
    if (index->isGamma5()) {
        return "\\gamma^{5}";
    }
    if (index->isGammaMu()) {
        return "\\gamma^{" + generateLatex(index->indices()) + "}";
    }
    if (index->isSigma()) {
        return "\\gamma^{" + generateLatex(index->indices()) + "}";
    }
    if (index->isP_L()) {
        return "P_L";
    }
    if (index->isP_R()) {
        return "P_R";
    }
    if (index->isC()) {
        return "C";
    }
    throw GammaAPIError(
        "Gamma index ", index->copy(), " is invalid for latex printing.");
}

static std::string generateLatex(sgl::MetricIndex const *index)
{
    return "g^{" + generateLatex(index->indices()) + "}";
}

static std::string generateLatex(sgl::EpsilonIndex const *index)
{
    return "\\epsilon^{" + generateLatex(index->indices()) + "}";
}

static std::string generateLatex(sgl::IndexChain const *current)
{
    const auto [a, b] = current->getBorderIndices();
    if (a != b) {
        std::string_view a_str = a.getName();
        std::string_view b_str = b.getName();
        std::string      indexString
            = std::string(a_str.begin() + 1, a_str.end())
              + std::string(b_str.begin() + 1, b_str.end());
        return "\\left(" + generateLatex(current, "") + "\\right)_{"
               + indexString + "}";
    }
    else {
        return "\\mathrm{Tr}\\left(" + generateLatex(current, "") + "\\right)";
    }
}

static std::string generateLatexImpl(Expr const &expr)
{
    if (auto p = dynamic_cast<sgl::Sum const *>(expr.get()); p) {
        return "&" + generateLatex(p, "\\\\& + ");
    }
    if (auto p = dynamic_cast<sgl::Prod const *>(expr.get()); p) {
        return generateLatex(p, "");
    }
    if (auto p = dynamic_cast<sgl::IndexChain const *>(expr.get()); p) {
        return generateLatex(p);
    }
    if (auto p = dynamic_cast<sgl::GammaIndex const *>(expr.get()); p) {
        return generateLatex(p);
    }
    if (auto p = dynamic_cast<sgl::MetricIndex const *>(expr.get()); p) {
        return generateLatex(p);
    }
    if (auto p = dynamic_cast<sgl::EpsilonIndex const *>(expr.get()); p) {
        return generateLatex(p);
    }
    if (auto p = dynamic_cast<sgl::CSLExpr const *>(expr.get()); p) {
        return generateLatex(p->expr());
    }
    throw GammaAPIError(
        "Expression ", expr, " is invalid for latex printing.");
}

std::string generateLatex(Expr const &expr)
{
    return generateLatexImpl(expr);
}

////
// IndexManager implementation
////

csl::Index IndexManager::getMinkoIndex(int mu)
{
    auto pos = minkoIndices.find(mu);
    if (pos == minkoIndices.end()) {
        minkoIndices[mu]
            = csl::Index(createMinkoIndexName(mu), &csl::Minkowski);
    }
    return minkoIndices[mu];
}

csl::Index IndexManager::getDiracIndex(int mu)
{
    auto pos = diracIndices.find(mu);
    if (pos == diracIndices.end()) {
        diracIndices[mu] = csl::Index(createDiracIndexName(mu), &mty::dirac4);
    }
    return diracIndices[mu];
}

std::string IndexManager::createMinkoIndexName(int mu)
{
    return IndexManager::minkoIndexPrefix + std::to_string(mu);
}

std::string IndexManager::createDiracIndexName(int mu)
{
    return IndexManager::diracIndexPrefix + std::to_string(mu);
}

bool IndexManager::isMinkoIndexName(std::string_view indexName)
{
    if (indexName.empty()) {
        return false;
    }
    return indexName[0] == IndexManager::minkoIndexPrefix
           && std::any_of(indexName.begin() + 1, indexName.end(), [](char c) {
                  return isdigit(c);
              });
}

Expr IndexManager::renameIndices(Expr expr)
{
    std::map<csl::Index, csl::Index> renaming;
    std::set<int>                    goodIndices;
    IndexManager::renameIndicesImpl(expr, renaming, goodIndices);
    int maxIndex = 1;
    for (auto &[oldIndex, newIndex] : renaming) {
        while (goodIndices.find(maxIndex) != goodIndices.end()) {
            ++maxIndex;
        }
        if (newIndex == csl::Index{}) {
            newIndex = IndexManager::getMinkoIndex(maxIndex++);
        }
        expr->replace(oldIndex, newIndex);
        expr->replace(+oldIndex, +newIndex);
    }
    return expr;
}

void IndexManager::renameIndicesImpl(
    Expr                             &expr,
    std::map<csl::Index, csl::Index> &renaming,
    std::set<int>                    &goodIndices)
{
    for (std::size_t i = 0; i != expr->size(); ++i) {
        IndexManager::renameIndicesImpl(
            expr->argument(i), renaming, goodIndices);
    }
    if (dynamic_cast<sgl::AbstractGeneralizedIndex const *>(expr.get())) {
        for (csl::Index const &i : expr->indices()) {
            if (i.getType() == cslIndex::Type::Fixed) {
                continue;
            }
            if (auto pos = renaming.find(i); pos == renaming.end()) {
                if (IndexManager::isMinkoIndexName(i.getName())) {
                    goodIndices.insert(std::atoi(i.getName().data() + 1));
                }
                else {
                    // Ensure we use only lower indices in the map
                    renaming[-i] = csl::Index{};
                }
            }
        }
    }
}

} // namespace mty::gamma_api
