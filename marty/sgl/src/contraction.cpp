#include "contraction.h"
#include "indexchain.h"
#include "metricindex.h"
#include "indexchain.h"
#include "debuglog.h"
#include "exceptions.h"
#include "cslexpr.h"

namespace sgl {

    GExpr contraction(
            GammaIndex        Mu,
            GammaIndex        Nu,
            csl::Index const &a,
            csl::Index const &b
            )
    {
        if (Mu.isSigma() && Nu.isSigma()) {
            SCOPELOG
            LOG("Sigma contraction", Mu.copy(), Nu.copy())
            int sign = 1;
            if (Mu.indices()[0] == Nu.indices()[1]) {
                sign *= -1;
                std::swap(Nu.indices()[0], Nu.indices()[1]);
            }
            else if (Mu.indices()[1] == Nu.indices()[0]) {
                sign *= -1;
                std::swap(Mu.indices()[0], Mu.indices()[1]);
            }
            else if (Mu.indices()[1] == Nu.indices()[1]) {
                std::swap(Nu.indices()[0], Nu.indices()[1]);
                std::swap(Mu.indices()[0], Mu.indices()[1]);
            }
            csl::Index nu  = Mu.indices()[1];
            csl::Index rho = Nu.indices()[1];
            LOG("Remaining indices :", nu, rho)
            bool fullContraction = (nu == rho);
            if (fullContraction) {
                return cslexpr_s(sign) * sgl::DMinko * (1-sgl::DMinko) * indexchain_s(a, b);
            }
            return cslexpr_s(sign)*((2 - sgl::DMinko) 
                * indexchain_s(gammaindex_s({nu, rho}), a, b)
                + (1 - sgl::DMinko) * metricindex_s(nu, rho) * indexchain_s(a, b));
        }
        if (Mu.isGammaMu() && Nu.isSigma()) {
            bool firstContracted = Mu.indices()[0] == Nu.indices()[0];
            int sign = (firstContracted) ? 1 : -1;
            csl::Index rho = Nu.indices()[firstContracted];
            return cslexpr_s(sign) * (sgl::DMinko - 1) 
                * indexchain_s(gammaindex_s(rho), a, b);
        }
        if (Mu.isSigma() && Nu.isGammaMu()) {
            return -contraction(Nu, Mu, a, b);
        }
        if (Mu.isGammaMu() && Nu.isGammaMu()) {
            return sgl::DMinko * indexchain_s(a, b);
        }
        if (Mu.isGamma5() && Nu.isGamma5()) {
            return indexchain_s(a, b);
        }
        if ((Mu.isGamma5() && Nu.isP_L()) 
                || (Mu.isP_L() && Nu.isGamma5())) {
            return -indexchain_s(gammaindex_s(6), a, b);
        }
        if ((Mu.isGamma5() && Nu.isP_R())
                || (Mu.isP_R() && Nu.isGamma5())) {
            return indexchain_s(gammaindex_s(7), a, b);
        }
        if (Mu.isP_L() && Nu.isP_L()) {
            return indexchain_s(gammaindex_s(6), a, b);
        }
        if ((Mu.isP_L() && Nu.isP_R()) || (Mu.isP_R() && Nu.isP_L())) {
            return cslexpr_s(CSL_0);
        }
        if (Mu.isP_R() && Nu.isP_R()) {
            return indexchain_s(gammaindex_s(7), a, b);
        }
        if (Mu.isC() && Nu.isC()) {
            return -indexchain_s(a, b);
        }
        throw Exception::MathError;
    }

    static GExpr g(
          csl::Index const &mu, 
          csl::Index const &nu
          )
    {
        return sgl::metricindex_s(mu, nu);
    }

    static GExpr epsilonCombinatorial(unsigned int i) 
    {
        if (i > 4)
            throw Exception::ValueError;
        if (i == 0)
            return cslexpr_s(1);
        return (sgl::DMinko - 4 + i) * epsilonCombinatorial(i - 1);
    }

    static GExpr epsilonContraction()
    {
        return epsilonCombinatorial(4);
    }

    static GExpr epsilonContraction(
            csl::Index const &mu,
            csl::Index const &nu
            )
    {
        return epsilonCombinatorial(3) * g(mu, nu);
    }

    static GExpr epsilonContraction(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &rho,
            csl::Index const &sigma
            )
    {
        return epsilonCombinatorial(2) * (
                g(mu, rho)*g(nu, sigma) 
                - g(mu, sigma)*g(nu, rho)
                );
    }

    static GExpr epsilonContraction(
            csl::Index const &mu,
            csl::Index const &nu,
            csl::Index const &rho,
            csl::Index const &sigma,
            csl::Index const &lambda,
            csl::Index const &tau
            )
    {
        return epsilonCombinatorial(1) * (
                g(mu, sigma)*g(nu, lambda)*g(rho, tau)
                - g(mu, sigma)*g(nu, tau)*g(rho, lambda)
                - g(mu, lambda)*g(nu, sigma)*g(rho, tau)
                + g(mu, lambda)*g(nu, tau)*g(rho, sigma)
                - g(mu, tau)*g(nu, lambda)*g(rho, sigma)
                + g(mu, tau)*g(nu, sigma)*g(rho, lambda)
                );
    }

    GExpr epsilonContraction(
            std::vector<csl::Index> A,
            std::vector<csl::Index> B
            )
    {
        if (A.size() != 4 || B.size() != 4)
            throw Exception::TypeError;
        SCOPELOG
        LOG("Epsilon contraction")
        LOG(csl::IndexStructure(A))
        LOG(csl::IndexStructure(B))
        int nPerm = 0;
        size_t nContraction = 0;
        bool contractionFound;
        do {
            contractionFound = false;
            for (size_t i = nContraction; i != A.size(); ++i) {
                for (size_t j = nContraction; j != B.size(); ++j) {
                    if (A[i] == B[j]) {
                        if (i != nContraction) {
                            std::swap(A[i], A[nContraction]);
                            ++nPerm;
                        }
                        if (j != nContraction) {
                            std::swap(B[j], B[nContraction]);
                            ++nPerm;
                        }
                        ++nContraction;
                        contractionFound = true;
                        break;
                    }
                }
                if (contractionFound)
                    break;
            }
        } while (contractionFound);
        LOG(nContraction, "contractions")
        LOG(nPerm, "permutations")
        GExpr sign = cslexpr_s((nPerm & 1) ? 1 : -1);
        GExpr res;
        switch(nContraction) {
            case 1:
                res = sign*epsilonContraction(
                        A[1], A[2], A[3],
                        B[1], B[2], B[3]
                        );
                break;
            case 2:
                res = sign*epsilonContraction(
                        A[2], A[3],
                        B[2], B[3]
                        );
                break;
            case 3:
                res = sign*epsilonContraction(
                        A[3], 
                        B[3]
                        );
                break;
            case 4:
                res = sign*epsilonContraction();
                break;
            default:
                throw Exception::MathError;
        }
        LOG("Res :", res)
        return res;
    }

    GExpr momentumGammaContraction(
            IndexChain  const &init,
            csl::Tensor        p,
            csl::Index  const &mu,
            csl::Index  const &nu,
            size_t             i,
            size_t             j
            )
    {
        SCOPELOG
        LOG("Momentum contraction :", init.copy(), '\n', p->getName(), mu, nu, i, j)
        auto rho = mu.rename();
        auto [expr, chain] = init.cut(i, j-i+1);
        auto const &m_argument = init.m_argument;
        if (init.isGammaMu(m_argument[i]) && init.isGammaMu(m_argument[j])) {
            LOG("Res :", (p(rho)*p(+rho)) * expr * chain.copy())
            return (p(rho)*p(+rho)) * expr * chain.copy();
        }
        if (init.isSigma(m_argument[i]) && init.isGammaMu(m_argument[j])) {
            int sign = (m_argument[i]->indices()[0] == mu) ? 1 : -1;
            auto sigma = m_argument[i]->indices()[sign == 1 ? 1 : 0];
            auto chain1 = chain;
            chain1.m_argument = std::vector<GExpr>{gammaindex_s(+rho)};
            auto chain2 = chain;
            chain2.m_argument = std::vector<GExpr>{gammaindex_s(sigma)};
            LOG("Res :", (
                sign*momentumindex_s(p, sigma)
                * momentumindex_s(p, rho)
                * expr * chain1.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))
                * expr * chain2.copy()))
            return 
                sign*momentumindex_s(p, sigma)
                * momentumindex_s(p, rho)
                * expr * chain1.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))
                * expr * chain2.copy();
        }
        if (init.isGammaMu(m_argument[i]) && init.isSigma(m_argument[j])) {
            int sign = (m_argument[j]->indices()[1] == mu) ? 1 : -1;
            auto sigma = m_argument[j]->indices()[sign == 1 ? 0 : 1];
            auto chain1 = chain;
            chain1.m_argument.push_back(gammaindex_s(+rho));
            auto chain2 = chain;
            chain2.m_argument.push_back(gammaindex_s(sigma));
            LOG("Res :", (
                sign*momentumindex_s(p, sigma)
                * momentumindex_s(p, rho)
                * expr * chain1.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))
                * expr * chain2.copy()))
            return 
                sign*momentumindex_s(p, sigma)
                * momentumindex_s(p, rho)
                * expr * chain1.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))
                * expr * chain2.copy();
        }
        if (init.isSigma(m_argument[i]) && init.isSigma(m_argument[j])) {
            int sign1 = (m_argument[i]->indices()[0] == mu) ? 1 : -1;
            int sign2 = (m_argument[j]->indices()[0] == nu) ? 1 : -1;
            auto sigma = m_argument[i]->indices()[sign1 == 1 ? 1 : 0];
            auto lambd = m_argument[j]->indices()[sign2 == 1 ? 1 : 0];
            int sign = sign1 * sign2;
            auto chain1 = chain;
            chain1.m_argument = std::vector<GExpr>{
                gammaindex_s(+rho), gammaindex_s(lambd)
            };
            auto chain2 = chain;
            chain2.m_argument = std::vector<GExpr>{
                gammaindex_s(+rho), gammaindex_s(sigma)
            };
            auto chain3 = chain;
            chain3.m_argument = std::vector<GExpr>{
                gammaindex_s(sigma), gammaindex_s(lambd)
            };
            LOG("Res :", (
                sign*momentumindex_s(p,sigma)*momentumindex_s(p,lambd)*chain.copy()
                + sign*momentumindex_s(p,sigma)*momentumindex_s(p,rho)*chain1.copy()
                - sign*momentumindex_s(p,lambd)*momentumindex_s(p,rho)*chain2.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))*chain3.copy()))
            return sign*momentumindex_s(p,sigma)*momentumindex_s(p,lambd)*chain.copy()
                + sign*momentumindex_s(p,sigma)*momentumindex_s(p,rho)*chain1.copy()
                - sign*momentumindex_s(p,lambd)*momentumindex_s(p,rho)*chain2.copy()
                - sign*cslexpr_s(p(rho)*p(+rho))*chain3.copy();
        }
        throw Exception::MathError;
    }

}
