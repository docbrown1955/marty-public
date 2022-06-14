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

#include "traceIdentities.h"

namespace mty {

TraceIdentity::TraceIdentity(std::vector<size_t> const &t_tensorSize)
    :tensorSize(t_tensorSize)
{
    n = 0;
    for (size_t ni : tensorSize)
        n += ni;

    std::vector<size_t> indices(n);
    for (size_t i = 0; i != n; ++i)
        indices[i] = i;

    auto oldTerms = terms;
    oldTerms.push_back({});
    oldTerms[0].reserve(tensorSize.size());
    for (size_t size : tensorSize)
        oldTerms[0].push_back(Tensor(size));
    while (!oldTerms.empty()) {
        std::vector<std::vector<Tensor>> newTerms;
        newTerms.reserve(oldTerms.size() * n);
        for (const auto &term : oldTerms) {
            std::vector<std::vector<Tensor>>
                interm = step(term);
            if (interm.empty())
                terms.push_back(std::move(term));
            else {
                newTerms.insert(
                        newTerms.end(),
                        std::make_move_iterator(interm.begin()),
                        std::make_move_iterator(interm.end())
                        );
            }
        }
        oldTerms = std::move(newTerms);
    }
}

std::vector<std::vector<TraceIdentity::Tensor>> TraceIdentity::step(
        std::vector<Tensor> const &term) const
{
    if (isFull(term))
        return {};
    std::vector<size_t> explored;
    size_t nVacant = 0;
    size_t iLast = totalIndices(term);
    std::vector<std::vector<TraceIdentity::Tensor>> res;
    res.reserve(term.size());
    explored.reserve(term.size());
    for (size_t i = 0; i != term.size(); ++i) {
        Tensor const &tensor = term[i];
        if (isFull(tensor))
            continue;
        ++nVacant;
        if (tensor.empty()) {
            auto pos = std::find(
                    explored.begin(),
                    explored.end(),
                    tensor.finalSize
                    );
            if (pos != explored.end())
                continue;
            else
                explored.push_back(tensor.finalSize);
        }
        res.push_back(term);
        res.back()[i].push_back(iLast);
    }
    if (nVacant == 1 and iLast + 1 < n)
        // Here res.size() == 1 for sure
        // And one and only one tensor is not full
        for (auto &tensor : res[0])
            if (not isFull(tensor))
                while (++iLast < n)
                    tensor.push_back(iLast);

    return res;
}

size_t TraceIdentity::totalIndices(
        std::vector<Tensor> const &term) const
{
    size_t ni = 0;
    for (const auto &tensor : term)
        ni += tensor.size();
    return ni;
}

bool TraceIdentity::isFull(Tensor const &tensor) const
{
    return tensor.finalSize == tensor.size();
}

bool TraceIdentity::isFull(std::vector<Tensor> const &tensors) const
{
    for (const auto &tensor : tensors)
        if (!isFull(tensor))
            return false;
    return true;
}

TraceIdentity &TraceIdentity::operator*=(csl::Expr const &t_factor)
{
    factor *= t_factor;
    return *this;
}
TraceIdentity &TraceIdentity::operator/=(csl::Expr const &t_factor)
{
    factor /= t_factor;
    return *this;
}

bool TraceIdentity::isSame(TraceIdentity const &other) const
{
    return other.tensorSize.size() == tensorSize.size()
        and std::is_permutation(
                tensorSize.begin(),
                tensorSize.end(),
                other.tensorSize.begin(),
                other.tensorSize.end()
                );
}

TraceIdentity &TraceIdentity::operator+=(TraceIdentity const &other)
{
    HEPAssert(isSame(other),
            mty::error::ValueError,
            "Cannot merge two different trace identites. Please check that "
            "they are same before merging.");
    factor += other.factor;
    return *this;
}

std::ostream &operator<<(
        std::ostream        &out,
        TraceIdentity const &identity
        )
{
    out << "Trace identity of " << identity.n << " indices.\n";
    out << "Factor = " << identity.factor << "\n";
    out << identity.terms.size() << " different terms: \n";
    size_t n = 0;
    for (const auto &term : identity.terms) {
        out << "(" << n++ << ")" << " : ";
        for (size_t i = 0; i != term.size(); ++i) {
            for (size_t j = 0; j != term[i].size(); ++j) {
                out << term[i][j];
                if (j != term[i].size() - 1)
                    out << "'";
            }
            if (i != term.size()-1)
                out << "..";
        }
        out << '\n';
    }

    return out;
}

std::ostream &operator<<(
        std::ostream                     &out,
        std::vector<PartitionPair> const &partition
        )
{
    size_t n = 0;
    for (const auto &p : partition)
        n += p.ni * p.mi;
    out << "Partition of " << n << " : ";
    for (const auto &p : partition)
        out << '{' << p.ni << ", " << p.mi << '}' << ' ';
    return out;
}

std::vector<std::vector<PartitionPair>> evenPartition(size_t n)
{
    HEPAssert(n % 2 == 0,
            mty::error::ValueError,
            "Cannot parition odd number " + toString(n) + " here.");
    if (n == 0)
        return {};
    if (n == 2)
        return {{{2, 1}}};
    if (n == 4)
        return {{{2, 2}}, {{4, 1}}};

    std::vector<std::vector<PartitionPair>> partition;
    for (size_t ni = 2; ni <= n; ni += 2)
        for (size_t mi = 1; ni*mi <= n; ++mi) {
            if (ni*mi == n) {
                partition.push_back({{ni, mi}});
                break;
            }
            std::vector<std::vector<PartitionPair>> subPartition 
                = evenPartition(n - ni * mi);
            for (auto &nSet : subPartition) {
                if (nSet[0].ni >= ni)
                    continue;
                std::vector<PartitionPair> newPartition = {{ni, mi}};
                newPartition.insert(
                        newPartition.end(),
                        nSet.begin(),
                        nSet.end()
                        );
                partition.push_back(std::move(newPartition));
            }
        }
    return partition;
}

std::vector<TraceIdentity> traceIdentity(
        algebra::Type type,
        size_t n
        )
{
    switch(type) {
        case algebra::Type::E6: return E6TraceIdentity(n);
        case algebra::Type::E7: return E7TraceIdentity(n);
        case algebra::Type::E8: return E8TraceIdentity(n);
        case algebra::Type::F4: return F4TraceIdentity(n);
        case algebra::Type::G2: return G2TraceIdentity(n);
        default:
            CallHEPError(mty::error::ValueError,
                    "Trace identity for algebra " + toString(type) + " needs"
                    " the dimension of the algebra (the order l).")
    }
    return {};
}
std::vector<TraceIdentity> traceIdentity(
        algebra::Type type,
        size_t l,
        size_t n
        )
{
    switch(type) {
        case algebra::Type::A:  return ATraceIdentity(l, n);
        case algebra::Type::B:  return BTraceIdentity(l, n);
        case algebra::Type::C:  return CTraceIdentity(l, n);
        case algebra::Type::D:  return DTraceIdentity(l, n);
        case algebra::Type::E6: return E6TraceIdentity(n);
        case algebra::Type::E7: return E7TraceIdentity(n);
        case algebra::Type::E8: return E8TraceIdentity(n);
        case algebra::Type::F4: return F4TraceIdentity(n);
        case algebra::Type::G2: return G2TraceIdentity(n);
        default:
            CallHEPError(mty::error::ValueError,
                    "Algebra " + toString(type) + " unknown.")
    }
    return {};
}

std::vector<TraceIdentity> ATraceIdentity(
        size_t l,
        size_t n
        )
{
    if (n % 2 == 1)
        return {};
    HEPAssert(n > l + 1,
            mty::error::ValueError,
            "Trace identity for SU(" + toString(l+1) + ") with "
            + toString(n) + " generators does not exist.");
    std::vector<std::vector<PartitionPair>> partitions
        = evenPartition(n);
    partitions.erase(partitions.end() - 1); // Last partition is (n, 1)
    std::vector<TraceIdentity> traces;
    traces.reserve(partitions.size());
    for (const auto &partition : partitions) {
        std::vector<size_t> tensorSizes;
        tensorSizes.reserve(n / 2); // Max number of tensors
        csl::Expr factor = n;
        for (const auto &[ni, mi] : partition) {
            factor *= std::pow(-1, mi) 
                / (csl::pow_s(ni, mi) * csl::factorial(mi));
            for (size_t i = 0; i != mi; ++i)
                tensorSizes.push_back(ni);
        }
        traces.push_back(TraceIdentity(tensorSizes));
        traces.back().factor = csl::Evaluated(factor);
    }

    return traces;
}
std::vector<TraceIdentity> BTraceIdentity(
        size_t l,
        size_t n
        )
{
    HEPAssert(n > 2*l + 1,
            mty::error::ValueError,
            "Trace identity for SU(" + toString(l+1) + ") with "
            + toString(n) + " generators does not exist.");

    return ATraceIdentity(l, n);
}
std::vector<TraceIdentity> CTraceIdentity(
        size_t l,
        size_t n
        )
{
    HEPAssert(n >= 2*l,
            mty::error::ValueError,
            "Trace identity for SU(" + toString(l+1) + ") with "
            + toString(n) + " generators does not exist.");
    HEPAssert(n != 2*l,
            mty::error::NotImplementedError,
            "Trace Identity 2N of SO(2N) has not yet been implemented.");

    return ATraceIdentity(l, n);
}
std::vector<TraceIdentity> DTraceIdentity(
        size_t l,
        size_t n
        )
{
    HEPAssert(n > 2*l,
            mty::error::ValueError,
            "Trace identity for SU(" + toString(l+1) + ") with "
            + toString(n) + " generators does not exist.");

    return ATraceIdentity(l, n);
}

std::vector<TraceIdentity> E6TraceIdentity(size_t n)
{
    std::vector<TraceIdentity> T;
    switch(n) {
        case 4:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 2}));
            T[0] /= 12;
            return T;
        case 7:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 5}));
            T[0] *= csl::intfraction_s(7, 24);
            return T;
        case 10:
            T.reserve(4);
            T.push_back(TraceIdentity({2, 8}));
            T[0] *= csl::intfraction_s(9, 24);
            T.push_back(TraceIdentity({2, 2, 6}));
            T[1] *= csl::intfraction_s(-7, 144);
            T.push_back(TraceIdentity({5, 5}));
            T[2] *= csl::intfraction_s(7, 40);
            T.push_back(TraceIdentity({2, 2, 2, 2, 2}));
            T[3] *= csl::intfraction_s(42, std::pow(12, 5));
            return T;
        case 11:
            T.reserve(3);
            T.push_back(TraceIdentity({5, 6}));
            T[0] *= csl::intfraction_s(11, 36);
            T.push_back(TraceIdentity({2, 9}));
            T[1] *= csl::intfraction_s(605, 12 * 126);
            T.push_back(TraceIdentity({2, 2, 2, 5}));
            T[2] *= csl::intfraction_s(-55, 2*std::pow(12, 3));
            return T;
        default:
            return {};
    }
}
std::vector<TraceIdentity> E7TraceIdentity(size_t n)
{
    std::vector<TraceIdentity> T;
    switch(n) {
        case 4:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 2}));
            T[0] /= 24;
            return T;
        default:
            return {};
    }
}
std::vector<TraceIdentity> E8TraceIdentity(size_t n)
{
    std::vector<TraceIdentity> T;
    switch(n) {
        case 4:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 2}));
            T[0] *= csl::intfraction_s(36, 60*60);
            return T;
        case 6:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 2, 2}));
            T[0] *= csl::intfraction_s(30, std::pow(60, 3));
            return T;
        case 10:
            T.reserve(2);
            T.push_back(TraceIdentity({2, 8}));
            T[0] *= csl::intfraction_s(15, 4*60);
            T.push_back(TraceIdentity({2, 2, 2, 2, 2}));
            T[1] *= csl::intfraction_s(315, 4*std::pow(60, 5));
            return T;
        default:
            return {};
    }
}
std::vector<TraceIdentity> F4TraceIdentity(size_t n)
{
    std::vector<TraceIdentity> T;
    switch(n) {
        case 4:
            T.reserve(1);
            T.push_back(TraceIdentity({2, 2}));
            T[0] *= csl::intfraction_s(3, 6*6);
            return T;
        case 10:
            T.reserve(3);
            T.push_back(TraceIdentity({2, 8}));
            T[0] *= csl::intfraction_s(9, 4*6);
            T.push_back(TraceIdentity({2, 2, 6}));
            T[1] *= csl::intfraction_s(7, 4*6);
            T.push_back(TraceIdentity({2, 2, 2, 2, 2}));
            T[2] *= csl::intfraction_s(21, 16*6);
            return T;
        default:
            return {};
    }
}
std::vector<TraceIdentity> G2TraceIdentity(size_t n)
{
    std::vector<TraceIdentity> T;
    if (n == 4) {
        T.reserve(1);
        T.push_back(TraceIdentity({2, 2}));
        T[0] /= 4;
        return T;
    }
    else if (n >= 8)
        return BTraceIdentity(3, n);
    return {};
}

} // End of namespace mty
