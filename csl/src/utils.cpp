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

#include "utils.h"
#include "operations.h"
#include "mathFunctions.h"
#include "cslcomplex.h"
#include "indicial.h"
#include "replace.h"
#include "field.h"
#include "tensorField.h"
#include "pseudoIntegral.h"
#include "space.h"
#include "comparison.h"
#include "commutation.h"
#include "algo.h"

using namespace std;

namespace csl {

std::vector<Expr> InverseTaylorExpand(
        Expr const& init,
        Expr const& big,
        size_t order)
{
    Expr eps = csl::variable_s("eps");
    Expr copy = Replaced(init, big, 1 / eps);
    std::vector<Expr> res = TaylorExpand(copy, eps, order);
    for (auto& expr : res) {
        expr = Replaced(expr, eps, 1 / big);
    }
    return res;
}

std::vector<Expr> TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order)
{
    Expr copy = DeepCopy(init);
    csl::Transform(copy, [&](Expr& expr)
    {
        if (expr->getType() == csl::Type::Sum) {
            int order = expr->getOrderOf(eps.get());
            if (order < 0) {
                for (auto& arg : *expr)
                    arg = arg * pow_s(eps, -order);
                expr = pow_s(eps, order) * expr;
                return true;
            }
        }
        return false;
    });

    return internal_TaylorExpand(copy, eps, order);
}

std::vector<Expr> internal_TaylorExpand(
        Expr const& init,
        Expr const& eps,
        size_t order)
{
    Expr f = init;
    Expr numFactor = CSL_1;
    Expr powerFactor = CSL_1;
    std::vector<Expr> res(order + 1);
    for (size_t i = 0; i != order+1; ++i) {
        Expr eval = Replaced(DeepCopy(f), eps, CSL_0);
        res[i] = powerFactor / numFactor * eval;
        if (i != order) {
            numFactor = numFactor * int_s(i+1);
            powerFactor = powerFactor * eps;
            f = f->derive(eps.get()).value_or(f);
        }
    }

    return res;
}

Expr CopySelf(Abstract* expr)
{
    return expr->copy();
}

Expr Copy(const Abstract* expr)
{    
    return expr->copy();
}

Expr Copy(const Expr& expr)
{
    return Copy(expr.get());
}

Expr DeepCopy(const Expr& expr)
{
    return DeepCopy(expr.get());
}

Expr DeepCopy(const Abstract* expr)
{
    return expr->deepCopy();
}

void WeakDeepCopy(Expr& expr)
{
    csl::ForEachNodeCut(expr, [&](csl::Expr &sub) {
        if (sub.use_count() > 1) {
            sub = csl::DeepCopy(sub);
            return true;
        }
        return false;
    });
}

Expr Refreshed(const Abstract* expr)
{
    return expr->refresh();
}

Expr Refreshed(const Expr& expr)
{
    return expr->refresh();
}

Expr DeepRefreshed(const Expr& expr)
{
    return expr->deepRefresh();
}


} // End of namespace csl
