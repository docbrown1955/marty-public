#include "parsefactors.h"

namespace mty::experimental::ufo
{

    csl::Expr getConstantFactor(
        csl::Expr const &prod)
    {
        if (!csl::IsProd(prod))
        {
            CallHEPError(mty::error::RuntimeError,
                         "Expected a product, " + toString(prod) + " given.")
        }
        std::vector<csl::Expr> factors;
        factors.reserve(prod->size());
        for (const auto &arg : prod)
        {
            if (!(arg->isIndexed() || csl::IsIndicialTensor(arg)))
            {
                factors.push_back(arg);
            }
        }
        return csl::prod_s(factors);
    }

    bool containsVectorSpace(
        csl::Expr const &expr,
        csl::Space const *space)
    {
        auto isTensorInSpace = [&space](csl::Expr const &element)
        {
            if (!csl::IsIndicialTensor(element))
                return false;
            for (const auto &index : element->getIndexStructureView())
            {
                if (index.getSpace() == space)
                    return true;
            }
            return false;
        };
        return csl::AnyOfLeafs(expr, isTensorInSpace);
    }

    bool containsLorentzFactor(csl::Expr const &expr)
    {
        return containsVectorSpace(expr, &csl::Minkowski) || containsVectorSpace(expr, &mty::dirac4);
    }

    bool containsDiracFactor(csl::Expr const &expr)
    {
        return containsVectorSpace(expr, &mty::dirac4);
    }

    bool containsColorFactor(
        csl::Expr const &expr,
        std::vector<csl::Space const *> const &colorSpaces)
    {
        return std::any_of(begin(colorSpaces), end(colorSpaces),
                           [&](csl::Space const *space)
                           {
                               return containsVectorSpace(expr, space);
                           });
    }

    FactorDecomposition decomposeExpression(
        csl::Expr const &expr,
        std::vector<csl::Space const *> const &colorSpaces)
    {
        if (!csl::IsProd(expr))
        {
            CallHEPError(mty::error::RuntimeError,
                         "Expected product, " + toString(expr) + " given.")
        }
        std::vector<csl::Expr> factors;
        std::vector<csl::Expr> lorentz;
        std::vector<csl::Expr> dirac;
        std::vector<csl::Expr> color;
        factors.reserve(expr->size());
        lorentz.reserve(expr->size());
        dirac.reserve(expr->size());
        color.reserve(expr->size());
        for (const auto &arg : expr)
        {
            if (ufo::containsDiracFactor(arg))
            {
                dirac.push_back(arg);
            }
            else if (ufo::containsLorentzFactor(arg))
            {
                lorentz.push_back(arg);
            }
            else if (ufo::containsColorFactor(arg, colorSpaces))
            {
                color.push_back(arg);
            }
            else if (arg->isIndexed())
            {
                CallHEPError(mty::error::RuntimeError,
                             "Non-SM term " + toString(expr) + " given. This module can "
                                                               "only process the SM gauged group.")
            }
            else
            {
                factors.push_back(arg);
            }
        }
        FactorDecomposition decompo;
        decompo.coupling = csl::prod_s(factors);
        decompo.lorentz = csl::prod_s(lorentz);
        decompo.dirac = csl::prod_s(dirac);
        decompo.color = csl::prod_s(color);
        return decompo;
    }

} // namespace mty::experimental::ufo
