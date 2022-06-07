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

#include <sstream>
#include "scopedProperty.h"
#include "abstract.h"
#include "interface.h"
#include "abreviation.h"
#include "lock.h"
#include "algo.h"

namespace csl {

std::string Lock::lockNameOf(ID_t id)
{
    std::ostringstream sout;
    sout << "LK_" << id;
    return sout.str();
}

void Lock::lock(
        Expr            &init,
        ID_t             id,
        predicate const &f
        )
{
    lockName[id] = lockNameOf(id);
    if (doLock(init, id, f))
        init = csl::Abbrev::makeAbbreviation(
                "LK_" + toString(id),
                init
                );
}

void Lock::lock(
        Expr            &init,
        predicate const &f
        )
{
    lock(init, 0, f);
}

void Lock::unlock(
        Expr &init,
        ID_t  id
        )
{
    doUnlock(init, id);
}

void Lock::unlock(
        Expr &init
        )
{
    unlock(init, 0);
}

bool Lock::doLock(
        Expr            &init,
        ID_t             id,
        predicate const &f
        )
{
    if (init == CSL_0)
        return false;
    if (csl::Size(init) == 0) {
        return !f(init);
    }
    else if (csl::IsSum(init) or csl::IsProd(init)) {
        Expr (*maker)(std::vector<Expr> const&, bool);
        if (csl::IsProd(init))
            maker = csl::prod_s;
        else
            maker = csl::sum_s;
        std::vector<Expr> toAbbreviate;
        std::vector<Expr> other;
        toAbbreviate.reserve(init->size());
        other       .reserve(1+init->size());
        for (auto &arg : init) {
            if (doLock(arg, id, f))  {
                toAbbreviate.push_back(arg);
            }
            else {
                other.push_back(arg);
            }
        }
        if (toAbbreviate.empty()) {
            return false;
        }
        if (other.empty())
            return true;
        csl::ScopedProperty prop(&csl::Abbrev::avoidDuplicates, false);
        auto abbreviated = maker(toAbbreviate, true);
        other.push_back(
                csl::Abbrev::makeAbbreviation(
                    lockName[id],
                    abbreviated
                    ));
        init = maker(other, true);
        return false;
    }
    else {
        bool locked = true;
        if (!init->isAnOperator()) {
            for (std::size_t i = 0; i != csl::Size(init); ++i)
                if (!doLock(init[i], id, f)) {
                    locked = false;
                    break;
                }
        }
        else {
            auto newCondition = [&](Expr const &sub) {
                  return f(sub) or init->operatorAppliesOn(sub.get());
            };
            for (std::size_t i = 0; i != csl::Size(init); ++i)
                if (!doLock(init[i], id, newCondition)) {
                    locked = false;
                    break;
                }
        }
        return locked;
    }
}

void Lock::doUnlock(
        Expr &init,
        ID_t  id
        )
{
    const auto name = lockName[id];
    csl::Abbrev::enableGenericEvaluation(name);
    csl::Evaluate(init);
    csl::Abbrev::disableGenericEvaluation(name);
    csl::Abbrev::removeAbbreviations(name);
}

} // End of namespace mty
