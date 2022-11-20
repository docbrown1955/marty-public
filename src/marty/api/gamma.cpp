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

Expr sigma(int mu, int nu)
{
    return CSL_I
           * sgl::gammaindex_s({IndexManager::getMinkoIndex(mu),
                                IndexManager::getMinkoIndex(nu)});
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

Expr chain(std::vector<Expr> const &gammas, int iLeft, int iRight)
{
    return sgl::indexchain_s(gammas,
                             IndexManager::getDiracIndex(iLeft),
                             IndexManager::getDiracIndex(iRight));
}

Expr simplified(Expr const &expr)
{
    return IndexManager::renameIndices(
        sgl::CSLSimplified(sgl::Simplified(expr, false)));
}

sgl::IndexChain const *getIndexChainAssert(Expr const &expr);

Expr applySingleFierz(Expr const &chain1, Expr const &chain2)
{
    auto left  = getIndexChainAssert(chain1);
    auto right = getIndexChainAssert(chain2);
    return IndexManager::renameIndices(left->applyGeneralFierz(*right));
}

Expr applyDoubleFierz(Expr const &chain1, Expr const &chain2)
{
    auto left  = getIndexChainAssert(chain1);
    auto right = getIndexChainAssert(chain2);
    return IndexManager::renameIndices(left->applyGeneralFierzTwice(*right));
}

sgl::IndexChain const *getIndexChainAssert(Expr const &expr)
{
    if (!expr.get()) {
        throw GammaAPIError("Invalid expression error (null).");
    }
    auto ptr = dynamic_cast<sgl::IndexChain const *>(expr.get());
    if (!ptr) {
        throw GammaAPIError("Expression given is not a pure gamma-index "
                            "chain: ",
                            expr);
    }
    return ptr;
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
