#include "gamma.h"
#include "marty/sgl/sgl.h"

#include <map>

namespace mty::gamma_api {

class IndexManager {

  public:
    static csl::Index getMinkoIndex(int mu);
    static csl::Index getDiracIndex(int mu);

  private:
    IndexManager() = delete;
    static std::string createDiracIndexName(int mu);
    static std::string createMinkoIndexName(int mu);

    static inline std::map<int, csl::Index> diracIndices = {};
    static inline std::map<int, csl::Index> minkoIndices = {};
};

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
    return "m" + std::to_string(mu);
}

std::string IndexManager::createDiracIndexName(int mu)
{
    return "d" + std::to_string(mu);
}

} // namespace mty::gamma_api
