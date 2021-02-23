#include "mrtInterface.h"
#include "quantumField.h"
#include "polarization.h"
#include "mrtUtils.h"
#include "mrtError.h"
#include "fierz.h"

namespace mty::fierz {

    bool isFermionic(csl::Expr const &arg)
    {
        return (
                (IsOfType<PolarizationField>(arg)
                    && ConvertToPtr<PolarizationField>(arg)->getQuantumParent()
                            ->isFermionic())
                || (IsOfType<QuantumField>(arg)
                    && ConvertToPtr<QuantumField>(arg)->isFermionic())
                );
    }

    std::vector<csl::Index> spinorIndex(
            csl::Expr const &prod,
            FierzMode        mode
            )
    {
        std::vector<csl::Index> res;
        res.reserve(2);
        bool conjugated = (mode == Left);
        for (const auto &arg : prod) {
            if (isFermionic(arg) && arg->isComplexConjugate() == conjugated) {
                res.push_back(arg->getIndexStructureView().back());
            }
        }
        return res;
    }

    size_t nBasisElement() {
        return 4;
    }

    std::pair<csl::Expr, csl::Expr> basisElements(
            size_t            i,
            csl::Index const &a,
            csl::Index const &b,
            csl::Index const &c,
            csl::Index const &d
            )
    {
        csl::Index e = a.rename();
        csl::Index f = a.rename();
        csl::Index mu = MinkowskiIndex();
        csl::Index nu = MinkowskiIndex();
        switch(i) {
            case 0:
                return {
                    DiracPL()({a, b}), 
                    DiracPL()({c, d})
                };
            case 1:
                return {
                    DiracPR()({a, b}), 
                    DiracPR()({c, d})
                };
            case 2:
                return {
                    DiracGamma()({mu, a, e})  * DiracPL()({e, b}),
                    DiracGamma()({+mu, c, f}) * DiracPR()({f, d})
                };
            case 3:
                return {
                    DiracGamma()({mu, a, e})  * DiracPR()({e, b}),
                    DiracGamma()({+mu, c, f}) * DiracPL()({f, d})
                };
            case 4:
                return {
                    DiracSigma()({mu, nu, a, b}),
                    DiracSigma()({+mu, +nu, c, d})
                };
            default:
                CallHEPError(
                        mty::error::IndexError,
                        "Basis of size " + toString(nBasisElement())
                        + " has no element " + toString(i) + "."
                        )
        }
        return {CSL_UNDEF, CSL_UNDEF};
    }

    void insertIdentity(
            csl::Expr        &prod,
            csl::Index const &a1,
            csl::Index const &a2,
            csl::Index const &b1,
            csl::Index const &b2
            )
    {
        const size_t N = nBasisElement();
        std::vector<csl::Expr> terms(N);
        for (size_t i = 0; i != N; ++i) {
            std::vector<csl::Expr> prodArgs = prod->getVectorArgument();
            auto [left, right] = basisElements(i, a1, a2, b1, b2);
            prodArgs.push_back(left);
            prodArgs.push_back(right);
            terms[i] = csl::prod_s(prodArgs);
        }
        prod = csl::sum_s(terms);
    }

    void doApplyFierz(
            csl::Expr &prod,
            csl::Index const &alpha,
            csl::Index const &beta,
            FierzMode         mode
            )
    {
        bool conjugated = (mode == Left);
        csl::Index alpha_prime = alpha.rename();
        csl::Index beta_prime  = beta.rename();
        // Replacing indices in the right spinors
        for (auto &arg : prod) {
            if (isFermionic(arg) && arg->isComplexConjugate() == conjugated) {
                auto &structure = arg->getIndexStructureView();
                if (structure.size() > 0 && structure.back() == alpha)
                    structure.back() = alpha_prime;
                if (structure.size() > 0 && structure.back() == beta)
                    structure.back() = beta_prime;
            }
        }
        if (mode == Left)
            insertIdentity(prod, alpha_prime, beta, beta_prime, alpha);
        else
            insertIdentity(prod, beta, alpha_prime, alpha, beta_prime);
    }

    void applyFierz(
            csl::Expr &prod,
            FierzMode  mode
            )
    {
        std::vector<csl::Index> indices = spinorIndex(prod, mode);
        if (indices.empty())
            return;
        HEPAssert(
                indices.size() == 2,
                mty::error::RuntimeError,
                "Expression " + toString(prod) + " not recognized for Fierz "
                "identity, should be a simple product of two fermion bilinears."
                )
        doApplyFierz(prod, indices[0], indices[1], mode);
    }

} // namespace mty::fierz
