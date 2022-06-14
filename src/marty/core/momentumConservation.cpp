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

 #include "momentumConservation.h"

namespace mty {

    MomentumConservater::MomentumConservater(
            std::vector<mty::QuantumField> const &insertions,
            std::vector<csl::Tensor>       const &t_momenta
            )
        :signs(insertions.size(), true),
        momenta(t_momenta)
    {
        if (momenta.size() > signs.size()) {
            momenta.erase(momenta.begin() + signs.size(), momenta.end());
        }
        posSimplestMomenta = 0;
        for (size_t i = 1; i < momenta.size(); ++i) {
            if (momenta[i]->getName() < momenta[posSimplestMomenta]->getName())
                posSimplestMomenta = i;
        }
        for (size_t i = 0; i != insertions.size(); ++i) {
            if (!insertions[i].isIncoming())
                signs[i] = false;
        }
    }

    bool MomentumConservater::apply(csl::Expr &expr)
    {
        bool simplified = false;
        csl::ForEachNode(expr, [&](csl::Expr &sub)
        {
            if (csl::IsSum(sub)) {
                auto factors = isMomentumSum(sub);
                if (!factors.empty()) {
                    auto simpli = applyOnMomentumSum(factors);
                    if (simpli) {
                        sub = simpli.value();
                        simplified = true;
                    }
                }
            } 
        });
        if (simplified) {
            csl::DeepRefresh(expr);
        }
        return simplified;
    }

    int MomentumConservater::findMomentum(csl::Expr const &sub)
    {
        if (!csl::IsIndicialTensor(sub))
            return -1;
        const auto p = sub->getParent_info();
        for (size_t i = 0; i != momenta.size(); ++i) 
            if (momenta[i].get() == p)
                return i;
        return -1;
    }

    std::vector<csl::Expr> MomentumConservater::isMomentumSum(
            csl::Expr const &sub
            )
    {
        if (
                !csl::AllOfLeafs(sub, [&](csl::Expr const &subsub)
                {
                    return csl::IsNumerical(subsub) 
                        || csl::IsLiteral(subsub) 
                        || findMomentum(subsub) != -1;
                })
            )
        {
            return {};
        }

        std::vector<csl::Expr> factors(momenta.size(), CSL_0);
        for (const auto &term : sub) {
            std::pair<int, csl::Expr> momStruct = getMomentumStructure(term);
            if (momStruct.first == -1)
                return {};
            factors[momStruct.first] += momStruct.second;
        }
        return factors;
    }

    std::optional<csl::Expr> MomentumConservater::applyOnMomentumSum(
            std::vector<csl::Expr> &factors
            )
    {
        auto isSimplified = simplify(factors);
        if (isSimplified)
            return recoverExpr(factors);
        return std::nullopt;
    }

    std::pair<int, csl::Expr> MomentumConservater::getMomentumStructure(
            csl::Expr const &term
            )
    {
        static const std::pair<int, csl::Expr> defaultRes = {-1, CSL_UNDEF};
        if (csl::IsProd(term)) {
            std::vector<csl::Expr> args;
            int pos = -1;
            args.reserve(term->size());
            for (const auto &arg : *term) {
                if (int t_pos = findMomentum(arg); t_pos != -1) {
                    if (pos != -1)
                        return defaultRes;
                    mu = arg->getIndexStructureView()[0];
                    pos = t_pos;
                }
                else
                    args.push_back(arg);
            }
            if (pos == -1)
                return defaultRes;
            if (!signs[pos])
                args.push_back(CSL_M_1);
            return {pos, csl::prod_s(args)};
        }
        if (auto pos = findMomentum(term); pos != -1) {
            mu = term->getIndexStructureView()[0];
            if (!signs[pos])
                return {pos, CSL_M_1};
            else
                return {pos, CSL_1};
        }
        return defaultRes;
    }

    bool MomentumConservater::simplify(std::vector<csl::Expr> &factors)
    {
        auto getNZero = [](std::vector<csl::Expr> const &f) -> size_t {
            return std::count(f.begin(), f.end(), CSL_0);
        };

        auto nZeroBest = getNZero(factors);
        if (nZeroBest > factors.size() / 2)
            return false;

        auto best = factors;
        bool simplified = false;
        for (size_t i = 0; i != factors.size(); ++i) {
            if (factors[i] != CSL_0) {
                auto test = factors;
                std::for_each(
                        test.begin(), 
                        test.end(), 
                        [&](csl::Expr &el) { el = el - factors[i]; }
                        );
                auto nZero = getNZero(test);
                if (nZero > nZeroBest
                      || (nZero == nZeroBest 
                            && CSL_0 != test[posSimplestMomenta])) {
                    nZeroBest = nZero;
                    factors = test;
                    simplified = true;
                }
            }
        }
        return simplified;
    }

    csl::Expr MomentumConservater::recoverExpr(
            std::vector<csl::Expr> const &factors
            )
    {
        std::vector<csl::Expr> sumArgs;
        sumArgs.reserve(factors.size());
        for (size_t i = 0; i != factors.size(); ++i) {
            if (factors[i] != CSL_0) {
                const auto f = (signs[i]) ? factors[i] : -factors[i];
                sumArgs.push_back(f * momenta[i](mu));
            }
        }
        return csl::sum_s(sumArgs);
    }

}
