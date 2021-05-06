// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

 #include "dimensionalRegularization.h"
#include "feynmanIntegral.h"
#include "mrtError.h"
#include "mrtUtils.h"
#include "mrtOptions.h"
#include <csl.h>

namespace mty::simpli {

    struct D_info {

        inline static const csl::Expr Dm4 = csl::constant_s("Dm4");

        csl::Expr constantTerm;
        csl::Expr Dm4Term;

        bool operator==(D_info const &other) const {
            return constantTerm == other.constantTerm
                && Dm4Term == other.Dm4Term;
        }

        bool operator!=(D_info const &other) const {
            return !(*this == other);
        }

        D_info &operator+=(D_info const &other) {
            constantTerm += other.constantTerm;
            Dm4Term      += other.Dm4Term;
            return *this;
        }

        D_info &operator*=(D_info const &other) {
            Dm4Term = constantTerm * other.Dm4Term 
                    + Dm4Term * other.constantTerm;
            constantTerm *= other.constantTerm;
            return *this;
        }
    };

    static D_info parseDInfoSum(csl::Expr&, D_info init);
    static D_info parseDInfoProd(csl::Expr&, D_info init);
    static D_info parseDInfoPow(csl::Expr&, D_info init);
    static D_info parseDInfo(
            csl::Expr &expr,
            D_info init = {0, 0})
    {
        D_info res {0, 0};
        if (csl::IsSum(expr))
            res = parseDInfoSum(expr, init);
        if (csl::IsProd(expr))
            res = parseDInfoProd(expr, init);
        if (csl::IsPow(expr))
            res = parseDInfoPow(expr, init);
        if (csl::IsNumerical(expr))
            res = {expr, 0};
        if (expr == D_info::Dm4)
            res = {0, 1};
        if (IsOfType<FeynmanIntegral>(expr)
                and init.Dm4Term != CSL_0) {
            csl::Expr factor = ConvertToPtr<FeynmanIntegral>(expr)->getDivergentFactor();
            if (factor != CSL_0) {
                expr = Copy(expr) + init.Dm4Term / init.constantTerm * factor;
            }
        }

        return res;
    }
    static D_info parseDInfoSum(
            csl::Expr &sum,
            D_info init)
    {
        D_info info {0, 0};
        for (auto &arg : sum)
            info += parseDInfo(arg, init);

        if (info.Dm4Term == CSL_0)
            return {0, 0};
        return info;
    }
    static D_info parseDInfoProd(
            csl::Expr &prod,
            D_info init)
    {
        D_info info {0, 0};
        for (auto &arg : prod) {
            if (csl::AnyOfNodes(arg, [&](csl::Expr const &sub) 
                { return IsOfType<FeynmanIntegral>(sub); }))
                continue;
            D_info infoArg = parseDInfo(arg, init);
            if (info == D_info({0, 0}))
                info = infoArg;
            else if (infoArg.Dm4Term != CSL_0)
                info *= infoArg;
        }
        if (init == D_info({0, 0}))
            init = info;
        else
            init *= info;
        for (auto &arg : prod)
            if (csl::AnyOfNodes(arg, [&](csl::Expr const &sub) 
                { return IsOfType<FeynmanIntegral>(sub); }))
                parseDInfo(arg, init);

        return info;
    }
    static D_info parseDInfoPow(
            csl::Expr &pow,
            D_info /*init*/)
    {
        D_info info = parseDInfo(pow->getArgument());
        if (info.Dm4Term != CSL_0
                and csl::IsNumerical(pow->getArgument(1))) {
            info.Dm4Term *= pow->getArgument(1) * info.constantTerm;
            info.constantTerm = csl::pow_s(info.constantTerm, pow->getArgument(1));
            return info;
        }
        return {0, 0};
    }

    void addLocalTerms(csl::Expr &res)
    {
        bool integral = csl::AnyOfNodes(res, [&](csl::Expr const &sub)
        {
            return IsOfType<FeynmanIntegral>(sub);
        });
        if (!option::addLocalTerms || !integral) {
            csl::Replace(res, csl::DMinko, csl::int_s(4));
            return;
        }
        csl::Expr Dm4 = D_info::Dm4;
        csl::DeepRefresh(res);
        res = csl::Replaced(res, csl::DMinko, 4 + Dm4);
        parseDInfo(res);
        csl::DeepRefresh(res);
        csl::Replace(res, Dm4, CSL_0);
        HEPAssert(!csl::DependsOn(res, Dm4),
                mty::error::RuntimeError,
                "Epsilon dependence in " + toString(res))
    }

}
