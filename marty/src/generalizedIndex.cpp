#include "generalizedIndex.h"
#include "diracology.h"
#include "mrtInterface.h"

namespace mty {

    ///////////////////////////////////////////////////
    // GeneralizedIndexBase class
    ///////////////////////////////////////////////////

    GeneralizedIndexBase::GeneralizedIndexBase(csl::Index const &t_index)
        :indices({t_index})
    {}

    GeneralizedIndexBase::GeneralizedIndexBase(
            std::vector<csl::Index> const &t_indices
            )
        :indices(t_indices)
    {}

    bool GeneralizedIndexBase::contains(
            csl::Index const &I
            ) const
    {
        return std::find(indices.begin(), indices.end(), I) != indices.end();
    }

    bool GeneralizedIndexBase::hasCommonIndexWith(
            GeneralizedIndexBase const &other
            ) const
    {
        for (const auto &i : indices)
            for (const auto &j : other.indices)
                if (i == j) {
                    return true;
                }
        return false;
    }

    csl::Index GeneralizedIndexBase::getCommonIndexWith(
            GeneralizedIndexBase const &other
            ) const
    {
        for (const auto &i : indices)
            for (const auto &j : other.indices)
                if (i == j) {
                    return i;
                }
        return csl::Index{};
    }

    std::vector<IndexChain> GeneralizedIndexBase::contraction(
            GeneralizedIndexBase const&
            ) const
    {
        CallHEPError(mty::error::NotImplementedError,
                "No contraction property found.")
        return {};
    }

    std::vector<IndexChain> GeneralizedIndexBase::swap(
            GeneralizedIndexBase const &other
            ) const
    {
        return { IndexChain{{other.copy(), copy()}} };
    }

    bool GeneralizedIndexBase::compare(GeneralizedIndexBase const &other) const
    {
        if (typeid(*this) != typeid(other))
            return false;
        if (indices.size() != other.indices.size())
            return false;
        for (size_t i = 0; i != indices.size(); ++i)
            if (indices[i] != other.indices[i])
                return false;
        return true;
    }

    int GeneralizedIndexBase::permuteIndices() 
    {
        int nPerms = 0;
        for (size_t i = 0; i+1 < indices.size(); ++i) {
            size_t i_min = i;
            for (size_t j = i+1; j != indices.size(); ++j) {
                if (indices[j] < indices[i_min])
                    i_min = j;
            }
            if (i != i_min) {
                ++nPerms;
                std::swap(indices[i], indices[i_min]);
            }
        }
        return nPerms;
    }

    ///////////////////////////////////////////////////
    // GenGammaIndex class
    ///////////////////////////////////////////////////

    bool GenGammaIndex::hasContractionPropertyWith(
            GeneralizedIndexBase const &other,
            bool                        consecutive
            ) const
    {
        return consecutive
            && other.isGamma()
            && hasCommonIndexWith(other)
            && (size() == 1 || other.size() == 1);
    }

    std::optional<csl::Expr> GenGammaIndex::getValue() const
    {
        for (size_t i = 0; i+1 < indices.size(); ++i) 
            for (size_t j = i+1; j != indices.size(); ++j) 
                if (indices[i] == indices[j])
                    return CSL_0;
        return std::nullopt;
    }

    std::vector<IndexChain> GenGammaIndex::contraction(
            GeneralizedIndexBase const &other
            ) const
    {
        if (isGammaMu()) {
            if (size() == 1 && other.size() == 1) {
                if (indices[0].getType() == cslIndex::Fixed) {
                    int indexValue = std::abs(indices[0].getValue());
                    csl::Expr square = csl::Minkowski.getMetric()->getTensor()
                        ->getArgument({indexValue, indexValue});
                    return {IndexChain{square, {}}};
                }
                return {IndexChain{csl::DMinko, {}}};
            }
            if (size() == 1) {
                size_t n = other.size();
                int sign = 1;
                auto newIndex = other.copy();
                for (size_t i = 0; i != newIndex->size(); ++i) {
                    if ((*newIndex)[i] == indices[0]) {
                        newIndex->erase(newIndex->begin() + i);
                        break;
                    }
                    sign *= -1;
                }
                csl::Expr factor = n * (csl::DMinko + 1 - n);
                return {IndexChain{n, {newIndex}}};
            }
            if (size() % 2 == 0) {
                auto res = other.contraction(*this);
                for (auto &chain : res)
                    chain.multiply(-1);
                return res;
            }
        }
        if (isGamma5()) {
            auto const &otherGamma 
                = *dynamic_cast<GenGammaIndex const*>(&other);
            if (otherGamma.isGamma5()) {
                return {IndexChain{CSL_1, {}}};
            }
            if (otherGamma.isP_L()) {
                return {IndexChain{
                    -CSL_1, 
                    {std::make_shared<GenGammaIndex>(
                            mty::dirac4.flippedChirality(other[0])
                            )
                    }}};
            }
            if (otherGamma.isP_R()) {
                return {IndexChain{
                    CSL_1, 
                    {std::make_shared<GenGammaIndex>(
                            mty::dirac4.flippedChirality(other[0])
                            )
                    }}};
            }
        }
        if (isP_L()) {
            auto const &otherGamma 
                = *dynamic_cast<GenGammaIndex const*>(&other);
            if (otherGamma.isP_L()) {
                return {IndexChain{CSL_1, {copy()} }};
            }
            if (otherGamma.isP_R()) {
                return {IndexChain{CSL_0, {}}};
            }
        }
        if (isP_R()) {
            auto const &otherGamma 
                = *dynamic_cast<GenGammaIndex const*>(&other);
            if (otherGamma.isP_R()) {
                return {IndexChain{CSL_1, {copy()} }};
            }
        }
        return other.contraction(*this);
    }

    bool GenGammaIndex::commutesWith(
            GeneralizedIndexBase const &other
            ) const
    {
        return !other.isGamma();
    }

    std::vector<IndexChain> GenGammaIndex::swap(
            GeneralizedIndexBase const &other
            ) const
    {
        if (!other.isGamma())
            return GeneralizedIndexBase::swap(other);
        return swap(*dynamic_cast<GenGammaIndex const*>(&other));
    }

    std::vector<IndexChain> GenGammaIndex::swap(
            GenGammaIndex const &other
            ) const
    {
        if (isGammaMu() && other.isGammaMu()) {
            auto g = csl::Minkowski.getMetric();
            auto mu = indices[0];
            auto nu = other.indices[0];
            return {
                IndexChain{2, {GenMetricIndex({mu, nu}).copy()}},
                IndexChain{-1, {other.copy(), copy()}}
            };
        }
        if ((isGammaMu() && other.isGamma5())
                || (isGamma5() && other.isGammaMu())) {
            return {
                IndexChain{-1, {other.copy(), copy()}}
            };
        }
        if (isGammaMu() && other.isProjector()) {
            auto flipped = gammaindex_s(
                    mty::dirac4.flippedChirality(other.indices[0])
                    );
            return {
                IndexChain{1, {flipped, copy()}}
            };
        }
        if (isProjector() && other.isGammaMu()) {
            auto flipped = gammaindex_s(
                    mty::dirac4.flippedChirality(indices[0])
                    );
            return {
                IndexChain{1, {other.copy(), flipped}}
            };
        }
        if (isGammaMu() && other.isSigma()) {
            auto g   = csl::Minkowski.getMetric();
            auto mu  = indices[0];
            auto nu  = other.indices[0];
            auto rho = other.indices[1];
            return {
                IndexChain{1, {other.copy(), copy()}},
                IndexChain{2,  {metricindex_s({mu, nu}), gammaindex_s(rho)}},
                IndexChain{-2, {metricindex_s({mu, rho}), gammaindex_s(nu)}},
            };
        }
        if (isSigma() && other.isGammaMu()) {
            auto g   = csl::Minkowski.getMetric();
            auto mu  = other.indices[0];
            auto nu  = indices[0];
            auto rho = indices[1];
            return {
                IndexChain{1, {other.copy(), copy()}},
                IndexChain{-2, {metricindex_s({mu, nu}), gammaindex_s(rho)}},
                IndexChain{2,  {metricindex_s({mu, rho}), gammaindex_s(nu)}},
            };
        }
        if (isSigma() && other.isSigma()) {
            auto g   = csl::Minkowski.getMetric();
            auto mu  = indices[0];
            auto nu  = indices[1];
            auto rho   = other.indices[0];
            auto sigma = other.indices[1];
            return {
                IndexChain{1, {other.copy(), copy()}},
                IndexChain{2,  {metricindex_s({mu, sigma}), gammaindex_s({nu, rho})}},
                IndexChain{2,  {metricindex_s({nu, rho}), gammaindex_s({mu, sigma})}},
                IndexChain{-2, {metricindex_s({nu, sigma}), gammaindex_s({mu, rho})}},
                IndexChain{-2, {metricindex_s({mu, rho}), gammaindex_s({nu, sigma})}},
            };
        }
        return GeneralizedIndexBase::swap(other);
    }

    csl::Expr GenGammaIndex::orderIndices()
    {
        int nPerms = permuteIndices();
        return csl::pow_s(-1, nPerms);
    }

    bool GenGammaIndex::isGammaMu() const 
    {
        return size() == 1 && !isProjector() && !isGamma5();
    }

    bool GenGammaIndex::isSigma() const
    {
        return size() == 2;
    }

    bool GenGammaIndex::isGamma5() const 
    {
        return size() == 1 && mty::dirac4.isGammaChir(indices[0]);
    }

    bool GenGammaIndex::isProjector() const
    {
        return isP_L() || isP_R();
    }

    bool GenGammaIndex::isP_L() const
    {
        return size() == 1 && mty::dirac4.isP_L(indices[0]);
    }

    bool GenGammaIndex::isP_R() const
    {
        return size() == 1 && mty::dirac4.isP_R(indices[0]);
    }

    void GenGammaIndex::print(std::ostream &out) const
    {
        out << '[';
        for (size_t i = 0; i != size(); ++i) {
            out << indices[i];
            if (i+1 != size())
                out << "  ";
        }
        out << ']';
    }

    ///////////////////////////////////////////////////
    // GenMetricIndex class
    ///////////////////////////////////////////////////

    bool GenMetricIndex::hasContractionPropertyWith(
            GeneralizedIndexBase const &other,
            bool                    
            ) const
    {
        return hasCommonIndexWith(other);
    }

    std::optional<csl::Expr> GenMetricIndex::getValue() const
    {
        if (indices[0] == indices[1])
            return csl::Minkowski.getMetric()(indices);
        return std::nullopt;
    }

    std::vector<IndexChain> GenMetricIndex::contraction(
            GeneralizedIndexBase const &other
            ) const
    {
        auto copy = other.copy();
        for (auto &i : *copy) {
            if (i == indices[0]) {
                i = indices[1];
                return {IndexChain{CSL_1, {copy}}};
            }
            if (i == indices[1]) {
                i = indices[0];
                return {IndexChain{CSL_1, {copy}}};
            }
        }
        CallHEPError(mty::error::RuntimeError,
                "Contraction not found for metric index.")
        return {};
    }

    csl::Expr GenMetricIndex::orderIndices()
    {
        permuteIndices();
        return CSL_1;
    }

    void GenMetricIndex::print(std::ostream &out) const
    {
        out << '(';
        for (size_t i = 0; i != size(); ++i) {
            out << indices[i];
            if (i+1 != size())
                out << "  ";
        }
        out << ')';
    }

    ///////////////////////////////////////////////////
    // GenEpsilon class
    ///////////////////////////////////////////////////

    bool GenEpsilonIndex::hasContractionPropertyWith(
            GeneralizedIndexBase const &other,
            bool                    
            ) const
    {
        return hasCommonIndexWith(other);
    }

    std::optional<csl::Expr> GenEpsilonIndex::getValue() const
    {
        for (size_t i = 0; i+1 < indices.size(); ++i) 
            for (size_t j = i+1; j != indices.size(); ++j) 
                if (indices[i] == indices[j])
                    return CSL_0;
        return std::nullopt;
    }

    csl::Expr GenEpsilonIndex::orderIndices() 
    {
        int nPerms = permuteIndices();
        return csl::pow_s(-1, nPerms);
    }

    void GenEpsilonIndex::print(std::ostream &out) const
    {
        out << '{';
        for (size_t i = 0; i != size(); ++i) {
            out << indices[i];
            if (i+1 != size())
                out << "  ";
        }
        out << '}';
    }

    ///////////////////////////////////////////////////
    // GenMomentumIndex class
    ///////////////////////////////////////////////////

    bool GenMomentumIndex::hasContractionPropertyWith(
            GeneralizedIndexBase const &other,
            bool                       
            ) const
    {
        if (!other.isMomentum() or !hasCommonIndexWith(other))
            return false;
        return true;
    }

    std::vector<IndexChain> GenMomentumIndex::contraction(
            GeneralizedIndexBase const &other
            ) const
    {
        csl::Index mu = MinkowskiIndex();
        return {IndexChain{p(mu)*toMomentumIndex(other).getP()(+mu), {}}};
    }

    void GenMomentumIndex::print(std::ostream &out) const
    {
        out << p->getName() << "(" << indices[0] << ")";
    }

    ///////////////////////////////////////////////////
    // IndexChain class
    ///////////////////////////////////////////////////

    IndexChain::IndexChain()
        :factor(CSL_1)
    {}

    IndexChain::IndexChain(
            csl::Expr             const &t_factor,
            std::vector<GenIndex> const &t_indices
            )
        :factor(csl::DeepCopy(t_factor)),
        indices(t_indices.size())
    {
        for (size_t i = 0; i != indices.size(); ++i) {
            indices[i] = t_indices[i]->copy();
        }
    }

    IndexChain::IndexChain(
            std::vector<GenIndex> const &t_indices
            )
        :IndexChain(CSL_1, t_indices)
    {}

    void IndexChain::sum(csl::Expr const &t_factor)
    {
        factor += csl::DeepCopy(t_factor);
    }

    void IndexChain::multiply(csl::Expr const &t_factor)
    {
        factor *= csl::DeepCopy(t_factor);
    }

    void IndexChain::refresh() 
    {
        for (size_t i = 0; i != indices.size(); ++i) {
            auto maybe_value = indices[i]->getValue();
            if (maybe_value) {
                factor *= maybe_value.value();
                indices.erase(indices.begin() + i);
                --i;
            }
            else {
                csl::Expr f = indices[i]->orderIndices();
                if (f != CSL_1)
                    factor *= f;
            }
            if (isZero()) {
                indices.clear();
                return;
            }
        }
        csl::DeepHardFactor(factor);
    }

    bool IndexChain::isProportionnalTo(IndexChain const &other) const
    {
        if (indices.size() != other.indices.size())
            return false;
        for (size_t i = 0; i != indices.size(); ++i) 
            if (!indices[i]->compare(*other.indices[i]))
                return false;
        return true;
    }

    std::vector<IndexChain> IndexChain::simplify_impl() const
    {
        std::vector<Action> actions = {
            [&](IndexChain const &c) { return c.externalize(5, false); },
            [&](IndexChain const &c) { return c.externalize(6, false); },
            [&](IndexChain const &c) { return c.externalize(7, false); },
            [&](IndexChain const &c) { return c.applyContractionProperties(); },
            [&](IndexChain const &c) { return c.contractDistant(); },
        };
        std::vector<IndexChain> res {{*this}};
        for (const auto &action : actions)
            applyOnAll(action, res);
        if (res.size() == 1 && compare(res[0]))
            return {};
        return res;
    }

    std::vector<IndexChain> IndexChain::simplify() const
    {
        auto res = recursiveAction([](IndexChain const &c) {
            return c.simplify_impl();
        });
        for (auto &chain : res) {
            chain.refresh();
            chain.sort();
        }
        mergeChains(res);
        return res;
    }

    void IndexChain::sort() 
    {
        for (size_t i = 0; i != indices.size(); ++i) {
            size_t i_min = i;
            for (size_t j = i+1; j < indices.size(); ++j) {
                if (mty::compare(indices[j], indices[i_min]))
                    i_min = j;
            }
            if (i_min != i) {
                auto I_save = indices[i_min];
                for (size_t k = i_min; k > i ; --k)
                    indices[k] = indices[k-1];
                indices[i] = I_save;
            }
        }
    }

    IndexChain::iterator IndexChain::firstGamma()
    {
        auto iter = begin();
        while (iter != end()) {
            if ((**iter).isGamma())
                break;
            ++iter;
        }
        return iter;
    }
    IndexChain::const_iterator IndexChain::firstGamma() const
    {
        auto iter = begin();
        while (iter != end()) {
            if ((**iter).isGamma())
                break;
            ++iter;
        }
        return iter;
    }

    IndexChain::reverse_iterator IndexChain::lastGamma()
    {
        auto iter = rbegin();
        while (iter != rend()) {
            if ((**iter).isGamma())
                break;
            ++iter;
        }
        return iter;
    }
    IndexChain::const_reverse_iterator IndexChain::lastGamma() const
    {
        auto iter = rbegin();
        while (iter != rend()) {
            if ((**iter).isGamma())
                break;
            ++iter;
        }
        return iter;
    }

    void IndexChain::push_front(GenIndex const &index)
    {
        indices.insert(indices.begin(), index);
    }

    void IndexChain::merge(
            iterator                     pos,
            std::move_iterator<iterator> first,
            std::move_iterator<iterator> last
            )
    {
        indices.insert(pos, first, last);
    }
    void IndexChain::merge(
            iterator       pos,
            const_iterator first,
            const_iterator last
            )
    {
        indices.insert(pos, first, last);
    }

    void IndexChain::merge(
            std::move_iterator<iterator> first,
            std::move_iterator<iterator> last
            )
    {
        merge(end(), first, last);
    }
    void IndexChain::merge(
            const_iterator first,
            const_iterator last
            )
    {
        merge(end(), first, last);
    }

    void IndexChain::merge(
            std::vector<GenIndex>::iterator iter,
            IndexChain               const &other
            )
    {
        merge(iter, other.begin(), other.end());
        multiply(other.factor);
    }

    void IndexChain::merge(
            std::vector<GenIndex>::iterator iter,
            IndexChain                    &&other
            )
    {
        merge(
                iter,
                std::make_move_iterator(other.begin()),
                std::make_move_iterator(other.end())
                );
        multiply(other.factor);
    }

    void IndexChain::merge(IndexChain const &other)
    {
        merge(end(), other);
    }

    void IndexChain::merge(IndexChain &&other)
    {
        merge(end(), other);
    }

    IndexChain IndexChain::copy() const {
        return IndexChain{factor, indices};
    }

    std::vector<IndexChain> IndexChain::recursiveAction(
            Action const &f
            ) const
    {
        std::vector<IndexChain> res = f(*this);
        if (res.empty())
            return {*this};
        std::vector<IndexChain> recursiveRes;
        recursiveRes.reserve(4*res.size());
        for (auto &chain : res) {
            auto interm = chain.recursiveAction(f);
            recursiveRes.insert(
                    recursiveRes.end(),
                    std::make_move_iterator(interm.begin()),
                    std::make_move_iterator(interm.end())
                    );
        }
        return recursiveRes;
    }

    std::vector<IndexChain> IndexChain::externalize(
            csl::Index const &index,
            bool              left
            ) const
    {
        return recursiveAction([&index, &left](IndexChain const &chain) {
            return chain.externalize_impl(index, left);
        });
    }

    std::vector<IndexChain> IndexChain::externalize_impl(
            csl::Index const &index,
            bool              left
            ) const
    {
        auto first  = begin();
        auto rfirst = rbegin();
        for (; first != end(); ++first, ++rfirst) {
            if (left && (**first).contains(index) && (**first).isGamma()) {
                if (first == begin())
                    continue;
                auto previous = first - 1;
                if (!(**previous).isGamma() || (**previous).contains(index))
                    continue;
                auto res = (**previous).swap(**first);
                for (auto &chain : res) {
                    chain.multiply(factor);
                    chain.merge(chain.begin(), begin(), previous);
                    chain.merge(chain.end(),   first+1, end());
                }
                return res;
            }
            else if (!left && (**rfirst).contains(index) && (**rfirst).isGamma()){
                if (rfirst == rbegin())
                    continue;
                auto rprevious = rfirst - 1;
                if (!(**rprevious).isGamma() || (**rprevious).contains(index))
                    continue;
                auto res = (**rfirst).swap(**rprevious);
                for (auto &chain : res) {
                    chain.multiply(factor);
                    chain.merge(chain.begin(), begin(), rfirst.base()-1);
                    chain.merge(chain.end(), rprevious.base(), end());
                }
                return res;
            }
        }
        return {};
    }

    std::vector<IndexChain> IndexChain::contractDistant(
            csl::Index const &index
            ) const
    {
        return recursiveAction([&index](IndexChain const &chain) {
            return chain.contractDistant_impl(index);
        });
    }

    std::vector<IndexChain> IndexChain::contractDistant_impl(
            csl::Index const &index
            ) const
    {
        constexpr size_t npos = -1;
        size_t pos1 = npos;
        size_t pos2 = npos;
        for (size_t i = 0; i != indices.size(); ++i) {
            if (indices[i]->contains(index)) {
                if (pos1 == npos) {
                    pos1 = i;
                }
                else {
                    pos2 = i;
                    break;
                }
            }
        }
        if (pos1 == npos || pos2 == npos)
            return {};
        if (!indices[pos1]->isGamma() || !indices[pos2]->isGamma())
            return {};
        if (!indices[pos1]->hasContractionPropertyWith(*indices[pos2], true))
            return {};
        std::vector<IndexChain> res = (pos2 == pos1 + 1) ?
            indices[pos1]->contraction(*indices[pos1 + 1])
            :indices[pos1]->swap(*indices[pos1 + 1]);
        for (auto &chain : res) {
            chain.multiply(factor);
            chain.insert(chain.begin(), begin(), begin() + pos1);
            chain.insert(chain.end(),   begin()+pos1+2, end());
        }
        return res;
    }

    std::vector<IndexChain> IndexChain::contractDistant() const
    {
        if (size() < 2)
            return {*this};
        for (size_t ir = size(); ir --> 1 ;) {
            for (size_t jr = ir; jr --> 0 ;) {
                if (indices[ir]->hasContractionPropertyWith(
                            *indices[jr], true)) {
                    csl::Index common = indices[ir]->getCommonIndexWith(
                            *indices[jr]);
                    return contractDistant(common);
                }
            }
        } 
        return {*this};
    }

    void IndexChain::print(std::ostream &out) const 
    {
        out << factor << " * ";
        for (const auto &genIndex : indices) {
            genIndex->print(out);
            out << ' ';
        }
    }

    bool IndexChain::compare(IndexChain const &other) const
    {
        if (factor != other.factor || size() != other.size())
            return false;
        for (size_t i = 0; i != indices.size(); ++i) 
            if (!indices[i]->compare(*other.indices[i])) {
                return false;
            }
        return true;
    }

    std::vector<IndexChain> IndexChain::applyContractionProperties() const
    {
        return recursiveAction([](IndexChain const &chain) {
            return chain.applyContractionProperties_impl();
        });
    }

    std::vector<IndexChain> IndexChain::applyContractionProperties_impl() const
    {
        auto iter_i = begin();
        if (iter_i == end())
            return {};
        while (iter_i != end()) {
            auto iter_j = begin();
            while (iter_j != end()) {
                if (iter_i == iter_j) {
                    ++iter_j;
                    continue;
                }
                if ((**iter_i).hasContractionPropertyWith(
                            **iter_j, iter_i+1 == iter_j
                            )) {
                    auto res = (**iter_i).contraction(**iter_j);
                    auto remnants = indices;
                    size_t pos_i = std::distance(begin(), iter_i);
                    size_t pos_j = std::distance(begin(), iter_j);
                    remnants.erase(remnants.begin() + pos_i);
                    if (pos_j > pos_i)
                        --pos_j;
                    for (auto &chain : res) {
                        chain.merge(
                                chain.begin(), 
                                remnants.begin(), 
                                remnants.begin()+pos_j);
                        chain.merge(
                                chain.end(), 
                                remnants.begin()+pos_j+1,
                                remnants.end());
                        chain.multiply(factor);
                    }
                    return res;
                }
                ++iter_j;
            }
            ++iter_i;
        }
        return {};
    }

    ///////////////////////////////////////////////////
    // Contraction properties
    ///////////////////////////////////////////////////

    void mergeChains(std::vector<IndexChain> &chains)
    {
        for (size_t i = 0; i != chains.size(); ++i) {
            for (size_t j = i+1; j != chains.size(); ++j) {
                if (chains[i].isProportionnalTo(chains[j])) {
                    chains[i].sum(chains[j].getFactor());
                    chains.erase(chains.begin() + j);
                    --j;
                }
            }
            chains[i].refresh();
        }
    }

    void applyOnAll(
            std::function<std::vector<IndexChain>(IndexChain const&)> const &f,
            std::vector<IndexChain> &chains
            )
    {
        std::vector<IndexChain> res;
        res.reserve(4*chains.size());
        for (const auto &c : chains) {
            auto interm = f(c);
            res.insert(
                    res.end(),
                    std::make_move_iterator(interm.begin()),
                    std::make_move_iterator(interm.end())
                    );
        }
        chains = std::move(res);
    }

    IndexChain::Action mergeActions(std::vector<IndexChain::Action> const &f)
    {
        return [&f](IndexChain const &c) {
            std::vector<IndexChain> res({c});
            bool modified = false;
            for (const auto &fi : f) {
                std::vector<IndexChain> newRes;
                newRes.reserve(4 * res.size());
                for (const auto &ci : res) {
                    auto interm = fi(ci);
                    if (interm.empty())
                        newRes.push_back(ci);
                    else {
                        modified = true;
                        newRes.insert(
                                newRes.end(),
                                std::make_move_iterator(interm.begin()),
                                std::make_move_iterator(interm.end())
                                );
                    }
                }
                res = std::move(newRes);
            }
            return (!modified) ? std::vector<IndexChain>{} : res;
        };
    }

    bool compare(GenIndex const &I, GenIndex const &J)
    {
        if (I->isMomentum() != J->isMomentum())
            return I->isMomentum();
        if (I->isMetric() != J->isMetric())
            return I->isMetric();
        if (I->isEpsilon() != J->isEpsilon())
            return I->isEpsilon();
        return false;
    }

    std::optional<std::pair<csl::Index, csl::Index>> diracStructure(
            csl::Expr const &tensor
            )
    {
        if (!csl::IsIndicialTensor(tensor))
            return std::nullopt;
        csl::IndexStructure const &structure = tensor->getIndexStructureView();
        csl::Index firstIndex;
        bool firstFound = false;
        for (const auto &i : structure) {
            if (i.getSpace() == &mty::dirac4) {
                if (firstFound)
                    return std::make_pair(firstIndex, i);
                else {
                    firstIndex = i;
                    firstFound = true;
                }
            }
        }
        return std::nullopt;
    }

    void sortChain(
            std::vector<csl::Expr> &args,
            std::vector<std::pair<csl::Index, csl::Index>> &indices
            )
    {
        if (args.empty())
            return;
        HEPAssert(
                args.size() == indices.size(),
                mty::error::RuntimeError,
                "Parameters of different sizes.")
        size_t firstArg = 0;
        for (size_t i = 1; i != args.size(); ++i) {
            csl::Index first = indices[i].first;
            bool found = false;
            for (size_t j = 0; j != args.size(); ++j) {
                if (i != j && first == indices[j].second) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                firstArg = i;
                break;
            }
        }
        if (firstArg != 0) {
            std::swap(args[0], args[firstArg]);
            std::swap(indices[0], indices[firstArg]);
        }
        csl::Index second = indices[0].second;
        for (size_t j = 1; j+1 != args.size(); ++j) {
            size_t next = j;
            for (size_t k = j; k != args.size(); ++k) {
                if (indices[k].first == second) {
                    second = indices[k].second;
                    next = k;
                    break;
                }
            }
            if (next != j) {
                std::swap(args[j], args[next]);
                std::swap(indices[j], indices[next]);
            }
        }
    }

    std::pair<csl::Expr, GenIndex> exprToIndex(csl::Expr const &tensor)
    {
        if (mty::dirac4.isGammaMu(tensor)
                || mty::dirac4.isProjector(tensor)
                || mty::dirac4.isGammaChir(tensor)) {
            return {
                CSL_1,
                std::make_shared<GenGammaIndex>(
                    mty::dirac4.getSpaceTimeIndex(tensor)
                    )
            };
        }
        else if (mty::dirac4.isSigma(tensor)) {
            return {
                CSL_I,
                std::make_shared<GenGammaIndex>(
                    std::vector<csl::Index>{
                      tensor->getIndexStructureView()[0],
                      tensor->getIndexStructureView()[1]
                    })
            };
        }
        else if (csl::IsIndicialTensor(tensor)) {
            return {
                CSL_1,
                std::make_shared<GenMomentumIndex>(
                        std::dynamic_pointer_cast<csl::TensorParent>(
                            tensor->getParent()
                            ),
                        tensor->getIndexStructureView()[0]
                    )
            };
        }
        else {
            CallHEPError(mty::error::TypeError,
                    "Tensor " + toString(tensor) + " not recognized for "
                    "generalized index.")
            return {CSL_UNDEF, nullptr};
        }
    }

    IndexChain productToIndexChain(csl::Expr const &prod)
    {
        const size_t sz = prod->size();
        std::vector<csl::Expr> factors;
        std::vector<csl::Expr> matrices;
        std::vector<std::pair<csl::Index, csl::Index>> indices;
        factors.reserve(sz);
        matrices.reserve(sz);
        for (size_t i = 0; i != sz; ++i) {
            csl::Expr const &arg = prod[i];
            auto optIndices = diracStructure(arg);
            if (optIndices) {
                matrices.push_back(arg);
                indices.push_back({optIndices->first, optIndices->second});
            }
            else {
                if (csl::IsIndicialTensor(arg) 
                        && arg->getParent()->getSpace().size() == 1
                        && arg->getParent()->getSpace()[0] == &csl::Minkowski) {
                    matrices.push_back(arg);
                    indices.push_back({csl::Index(), csl::Index()});
                }
                else
                    factors.push_back(arg);
            }
        }
        sortChain(matrices, indices);
        IndexChain res{csl::prod_s(factors)};
        for (const auto &m : matrices) {
            auto [factor, index] = exprToIndex(m);
            res.push_back(index);
            res.multiply(factor);
        }
        return res;
    }
}
