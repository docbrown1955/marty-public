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

 #include "interface.h"
#include "replace.h"
#include "indicial.h"
#include "utils.h"
#include "abreviation.h"
#include "algo.h"
#include "error.h"
#include "support.h"

namespace csl {

    ///////////////////////////////////////////////////
    // Expression replacements
    ///////////////////////////////////////////////////

    void Replace(
            csl::Expr       &expr,
            csl::Expr const &from,
            csl::Expr const &to
            )
    {
        Replace(
                expr,
                std::vector<csl::Expr>{from},
                std::vector<csl::Expr>{to}
                );
    }

    void Replace(
            csl::Expr         &expr,
            csl::Parent const &from,
            csl::Parent const &to
            )
    {
        csl::Replace(
                expr,
                std::vector<csl::Parent>{from},
                std::vector<csl::Parent>{to}
                );
    }

    void Replace(
            csl::Expr &expr,
            csl::Parent const &from,
            csl::Expr   const &to_init
            )
    {
        csl::Replace(
                expr,
                std::vector<csl::Parent>{from},
                std::vector<csl::Expr>{to_init}
                );
    }

    void Replace(
            csl::Expr         &expr,
            std::vector<csl::Parent> const &from,
            std::vector<csl::Parent> const &to
            )
    {
        if (from.empty()) return;
        CSL_ASSERT_SPEC(
                from.size() == to.size(),
                CSLError::ValueError,
                "Expecting vectors of the same size, vectors of size "
                + toString(from.size()) + " and " + toString(to.size())
                + " given."
                )
        csl::WeakDeepCopy(expr);
        applyThroughAbbreviations(
                expr, 
                [&](csl::Expr const &sub, bool isPredicate) {
                    return tensorReplacement(
                            sub, from, to, isPredicate);
                });
    }

    void Replace(
            csl::Expr       &expr,
            std::vector<csl::Expr> const &from,
            std::vector<csl::Expr> const &to
            )
    {
        if (from.empty()) return;
        CSL_ASSERT_SPEC(
                from.size() == to.size(),
                CSLError::ValueError,
                "Expecting vectors of the same size, vectors of size "
                + toString(from.size()) + " and " + toString(to.size())
                + " given."
                )
        if (from[0]->getFreeIndexStructure().size() > 0) {
            ReplaceIndicial(expr, from, to);
            return;
        }
        csl::WeakDeepCopy(expr);
        auto conditionalCC = [&](csl::Expr const &sub) {
            if (sub->isComplexConjugate())
                return CSL_UNDEF;
            return csl::GetComplexConjugate(sub);
        };
        std::vector<csl::Expr> ccFrom(from.size());
        std::transform(from.begin(), from.end(), ccFrom.begin(), conditionalCC);
        applyThroughAbbreviations(
                expr, 
                [&](csl::Expr const &sub, bool isPredicate) {
                    return scalarReplacement(
                            sub, from, ccFrom, to, isPredicate);
                });
    }

    void ReplaceIndicial(
            csl::Expr       &expr,
            std::vector<csl::Expr> const &from,
            std::vector<csl::Expr> const &to
            )
    {
        if (from.empty()) return;
        CSL_ASSERT_SPEC(
                from.size() == to.size(),
                CSLError::ValueError,
                "Expecting vectors of the same size, vectors of size "
                + toString(from.size()) + " and " + toString(to.size())
                + " given."
                )
        csl::WeakDeepCopy(expr);

        // Getting parents from from
        std::vector<csl::Parent_info> parentFrom(from.size());
        std::transform(
                from.begin(), from.end(), parentFrom.begin(),
                [&](csl::Expr const &sub) {
                    return sub->getParent_info();
                });

        // Asserting the correctness of replacements
        for (std::size_t i = 0; i != from.size(); ++i) {
            CSL_ASSERT_SPEC(parentFrom[i],
                    CSLError::TypeError,
                    "Expected indicial tensor in replace, " + toString(expr) 
                    + " given.");
            CSL_ASSERT_SPEC(from[i]->getFreeIndexStructure() 
                        == to[i]->getFreeIndexStructure(),
                        CSLError::ValueError,
                        "Expected same index structures in replace for "
                        + toString(from[i]) + " and " + toString(to[i]));
        }

        applyThroughAbbreviations(
                expr, 
                [&](csl::Expr const &sub, bool isPredicate) {
                    return indicialReplacement(
                            sub, from, parentFrom, to, isPredicate);
                });
    }

    void Replace(
            csl::Expr                      &expr,
            std::vector<csl::Parent> const &from,
            std::vector<csl::Expr>   const &to_init,
            bool                            refresh
            )
    {
        if (from.empty()) return;
        CSL_ASSERT_SPEC(
                from.size() == to_init.size(),
                CSLError::ValueError,
                "Expecting vectors of the same size, vectors of size "
                + toString(from.size()) + " and " + toString(to_init.size())
                + " given."
                )
        // Getting copies with dummy indices reset from to_init
        csl::WeakDeepCopy(expr);
        std::vector<csl::Expr> to(to_init.size());
        std::transform(
                to_init.begin(), to_init.end(), to.begin(),
                [&](csl::Expr const &expr) {
                    csl::Expr cpy = csl::DeepCopy(expr);
                    csl::ResetDummyIndices(cpy);
                    csl::DeepRefresh(cpy);
                    return cpy;
                });
        
        ResetDummyIndices(expr);

        applyThroughAbbreviations(
                expr, 
                [&](csl::Expr const &sub, bool isPredicate) {
                    return tensorExpressionReplacement(
                            sub, from, to, isPredicate);
                });
        if (refresh)
            csl::DeepRefresh(expr);
    }


    ///////////////////////////////////////////////////
    // Index replacements
    ///////////////////////////////////////////////////

    csl::Expr Replaced(
            csl::Expr  const &expr,
            csl::Index const &from,
            csl::Index const &to,
            bool              refresh
            )
    {
        return expr->replaceIndex(from, to, refresh).value_or(expr);
    }

    csl::Expr Replaced(
            csl::Expr               const &expr,
            std::vector<csl::Index> const &from,
            std::vector<csl::Index> const &to,
            bool                           refresh
            )
    {
        bool commonIndex = std::any_of(
                begin(from), end(from),
                [&to](csl::Index const &i) {
                    return i.getType() != cslIndex::Fixed
                        && end(to) != std::find(begin(to), end(to), i);
                });
        if (commonIndex) {
            // If a common index exists between from and to, we first 
            // pass through an intermediate step with intermediate indices
            std::vector<csl::Index> intermediate(from);
            for (auto &i : intermediate)
                i = i.rename();
            auto const &interm = intermediate;
            return csl::Replaced(
                    csl::Replaced(expr, from, interm, refresh),
                    interm, 
                    to,
                    refresh
                    );
        }
        return expr->replaceIndices(from, to, refresh).value_or(expr);
    }

    csl::Expr Replaced(
            csl::Expr           const &expr,
            csl::IndexStructure const &from,
            csl::IndexStructure const &to,
            bool                       refresh
            )
    {
        return csl::Replaced(expr, from.getIndex(), to.getIndex(), refresh);
    }

    ///////////////////////////////////////////////////
    // Abbreviation treatment
    ///////////////////////////////////////////////////

    bool hasWeakDependency(
            csl::Expr                             const &expr,
            std::function<bool(csl::Expr const&)> const &predicate
            )
    {
        return csl::AnyOfNodes(expr, [&](csl::Expr const &sub) {
            if (predicate(sub))
                return true;
            csl::Parent_info parent = sub->getParent_info();
            if (parent && parent->isAnAbbreviation()) {
                csl::Expr const &encaps = parent->getEncapsulated();
                return hasWeakDependency(encaps, predicate);
            }
            return false;
        });
    }

    void applyThroughAbbreviations(
            csl::Expr &expr,
            replacementRule const &rule
            )
    {
        const auto predicate = ruleToPredicate(rule);
        csl::Transform(expr, [&](csl::Expr &sub) {
            if (auto opt = rule(sub, false); opt) {
                sub = opt.value();
                return true;
            }
            csl::Parent_info parent = sub->getParent_info();
            if (parent && parent->isAnAbbreviation()) {
                if (hasWeakDependency(sub, predicate)) {
                    std::string const &name = parent->getBaseName();
                    csl::Expr encaps = parent->getExactEncapsulated(sub.get());
                    applyThroughAbbreviations(encaps, rule);
                    sub = csl::Abbrev::makeAbbreviation(name, encaps);
                    return true;
                }
            }
            return false;
        });
    }

    ///////////////////////////////////////////////////
    // Replacement rules
    ///////////////////////////////////////////////////

    std::optional<csl::Expr> scalarReplacement(
            csl::Expr              const &expr,
            std::vector<csl::Expr> const &from, 
            std::vector<csl::Expr> const &ccFrom, 
            std::vector<csl::Expr> const &to,
            bool                          isPredicate
            )
    {
        auto posF = find(begin(from), end(from), expr);
        if (posF != end(from)) {
            if (isPredicate)
                return CSL_UNDEF;
            return to[std::distance(begin(from), posF)];
        }
        auto posCCF = find(begin(ccFrom), end(ccFrom), expr);
        if (posCCF != end(ccFrom)) {
            if (isPredicate)
                return CSL_UNDEF;
            return csl::GetComplexConjugate(
                    to[std::distance(begin(ccFrom), posCCF)]
                    );
        }
        if (csl::IsPow(expr)) {
            const auto &arg = expr[0];
            auto pos = std::find_if(begin(from), end(from), 
                    [&](csl::Expr const &f) {
                        return csl::IsPow(f) && f[0] == arg;
                    });
            if (pos != end(from)) {
                const csl::Expr ratio = expr[1] / (*pos)[1];
                const std::size_t i = std::distance(begin(from), pos);
                if (ratio->isInteger() && ratio->evaluateScalar() > 0) {
                    if (isPredicate)
                        return CSL_UNDEF;
                    return csl::pow_s(to[i], ratio);
                }
            }
        }
        return std::nullopt;
    }

    std::optional<csl::Expr> indicialReplacement(
            csl::Expr                     const &expr,
            std::vector<csl::Expr>        const &from, 
            std::vector<csl::Parent_info> const &parentFrom, 
            std::vector<csl::Expr>        const &to,
            bool                                 isPredicate
            )
    {
        auto parent = expr->getParent_info();
        auto posF = end(parentFrom);
        for (std::size_t i = 0; i != from.size(); ++i) {
            if (parent != parentFrom[i])
                continue;
            // If replacement from complex conjugate, expr must also be
            if (from[i]->isComplexConjugate() && !expr->isComplexConjugate())
                continue;
            posF = begin(parentFrom) + i;
            break;
        }
        if (posF != end(parentFrom)) {
            if (isPredicate)
                return CSL_UNDEF;
            const std::size_t i = std::distance(begin(parentFrom), posF);
            csl::Expr const &res = to[i];
            if (res == CSL_0)
                return CSL_0;
            IndexStructure structureToSave = expr->getIndexStructure();
            Expr newSub = csl::DeepCopy(res);
            RenameIndices(newSub);
            if (expr->isComplexConjugate() != from[i]->isComplexConjugate())
                newSub = GetComplexConjugate(newSub);
            if (not structureToSave.empty())
                csl::ApplyIndices(
                        newSub, 
                        from[i]->getIndexStructure(),
                        structureToSave
                        );
            return newSub;
        }
        return std::nullopt;
    }

    std::optional<csl::Expr> tensorReplacement(
            csl::Expr                const &expr,
            std::vector<csl::Parent> const &from,
            std::vector<csl::Parent> const &to,
            bool                            isPredicate
            )
    {
        const auto parent = expr->getParent_info();
        const auto point  = expr->getPoint().get(); 
        auto pos = std::find_if(
                begin(from), end(from), 
                [&](csl::Parent const &p) { 
                    return p && p.get() == parent; 
                });
        auto posPoint = std::find_if(
                begin(from), end(from), 
                [&](csl::Parent const &p) { 
                    return p && p.get() == point; 
                });
        if (pos == end(from) && posPoint == end(from))
            return std::nullopt;
        csl::Expr res = csl::Copy(expr);
        if (pos != end(from)) {
            if (isPredicate)
                return CSL_UNDEF;
            const std::size_t i = std::distance(begin(from), pos);
            res->setParent(to[i]);
        }
        if (csl::IsField(expr) && posPoint != end(from)) {
            if (isPredicate)
                return CSL_UNDEF;
            const std::size_t i = std::distance(begin(from), posPoint);
            const auto tensorPoint = 
                std::dynamic_pointer_cast<TensorParent>(to[i]);
            CSL_ASSERT_SPEC(
                    tensorPoint,
                    CSLError::TypeError,
                    "Expecting a tensor for replacing point " 
                    + point->getName() + ", " + to[i]->getName() + " given.")
            res->setPoint(tensorPoint);
        }
        return res;
    }

    std::optional<csl::Expr> tensorExpressionReplacement(
            csl::Expr                const &expr,
            std::vector<csl::Parent> const &from,
            std::vector<csl::Expr>   const &to,
            bool                            isPredicate
            )
    {
        csl::Parent_info parent = expr->getParent_info();
        auto pos = std::find_if(
                begin(from), end(from),
                [&](csl::Parent const &p) {
                    return p.get() == parent;
                });
        if (pos != end(from)) {
            if (isPredicate)
                return CSL_UNDEF;
            const std::size_t i = std::distance(begin(from), pos);
            if (to[i] == CSL_0)
                return CSL_0;
            IndexStructure structureToSave 
                = expr->getFreeIndexStructure();
            csl::Expr res;
            if (expr->isComplexConjugate())
                res = GetComplexConjugate(DeepCopy(to[i]));
            else
                res = DeepCopy(to[i]);
            if (not structureToSave.empty())
                ApplyIndices(res, structureToSave);
            return res;
        }
        return std::nullopt;
    }

    ///////////////////////////////////////////////////
    // Helper functions for indicial replacements
    ///////////////////////////////////////////////////

    void ResetDummyIndices(csl::Expr& expr)
    {
        csl::ForEachNode(expr, [&](csl::Expr &sub) {
            if (csl::IsIndicialTensor(sub)) {
                sub = csl::Copy(sub);
                IndexStructure index = sub->getIndexStructure();
                for (auto& i : index)
                    if (not (i.getType() == cslIndex::Fixed))
                        i.setFree(true);
                sub->setIndexStructure(index);
                csl::Refresh(sub);
            }
        });
    }

    void ApplyIndices(
            csl::Expr                 &expr,
            csl::IndexStructure const &freeStructure
            )
    {
        csl::IndexStructure freeExprStructure = expr->getFreeIndexStructure();
        if (freeStructure.size() != freeExprStructure.size()) {
            expr->print();
            std::cout << expr->getType() << std::endl;
            std::cout << freeStructure << std::endl;
            std::cout << expr->getIndexStructure() << std::endl;
            std::cout << freeExprStructure << std::endl;
            callError(cslError::UndefinedBehaviour,
                    "ApplyIndices(const csl::Expr&, const IndexStructure&)");
        }
        csl::Replace(expr, freeExprStructure, freeStructure);
        csl::RenameIndices(expr);
    }

    void ApplyIndices(
            csl::Expr                 &expr,
            csl::IndexStructure const &from,
            csl::IndexStructure const &to
            )
    {
        if (from.size() != to.size()) {
            expr->print();
            std::cout << expr->getType() << std::endl;
            std::cout << from << std::endl;
            std::cout << expr->getIndexStructure() << std::endl;
            std::cout << to << std::endl;
            callError(cslError::UndefinedBehaviour,
                    "ApplyIndices(const csl::Expr&, const IndexStructure&)");
        }
        csl::Replace(expr, from, to);
    }

    void renameIndex(
            csl::Index                       &index,
            std::map<csl::Index, csl::Index> &mapping
            )
    {
        if (auto pos = find_if(mapping.begin(), mapping.end(),
                    [&index](const std::pair<Index,Index>& p) {
                        return p.second.getName() == index.getName()
                            and p.second.getID() == index.getID()
                            and p.second.getSpace() == index.getSpace();
                    });
                pos != mapping.end())
            return;

        if (auto pos = find_if(mapping.begin(), mapping.end(),
                    [&index](const std::pair<Index,Index>& p) {
                        return p.first.getName() == index.getName()
                            and p.first.getID() == index.getID()
                            and p.first.getSpace() == index.getSpace();
                    });
                pos != mapping.end()) {
            Index newIndex = index;
            newIndex.setName(std::string((*pos).second.getName()));
            newIndex.setID((*pos).second.getID());
            newIndex.setFree(index.getFree());
            index = newIndex;
            return;
        }
        std::string name = std::string(index.getName());
        Index newIndex = index.getSpace()->generateIndex(name);
        newIndex.setSign(index.getSign());
        mapping[index] = newIndex;
        newIndex.setFree(index.getFree());
        index = newIndex;
    }

    void RenameIndices(csl::Expr& expr)
    {
        csl::WeakDeepCopy(expr);
        std::map<csl::Index, csl::Index> mapping;
        csl::ForEachNode(expr, [&mapping](csl::Expr &sub) {
            if (csl::IsIndicialTensor(sub)) {
                sub = csl::Copy(sub);
                csl::IndexStructure &index = sub->getIndexStructureView();
                for (auto& i : index)
                    if (i.getType() == cslIndex::Dummy) 
                        renameIndex(i, mapping);
            }
        });
    }

    csl::Expr RenamedIndices(csl::Expr const& expr)
    {
        csl::Expr res = expr;
        csl::RenameIndices(res);
        return res;
    }

} // namespace csl
