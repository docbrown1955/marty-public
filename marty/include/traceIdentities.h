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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief
 */
#ifndef TRACE_IDENTITIES_H_INCLUDED
#define TRACE_IDENTITIES_H_INCLUDED

#include <csl.h>
#include "group.h"

namespace mty {

class TraceIdentity {

public:

    struct Tensor {
        std::size_t finalSize;
        std::vector<std::size_t> indices;
        
        Tensor(std::size_t t_finalSize)
            :finalSize(t_finalSize)
        {
            indices.reserve(finalSize);
        }

        Tensor(Tensor const &other) = default;
        Tensor(Tensor &&other) = default;
        Tensor &operator=(Tensor const &other) = default;
        Tensor &operator=(Tensor &&other) = default;
        ~Tensor() = default;

        IMPLEMENTS_STD_VECTOR(std::size_t, indices);
    };

    IMPLEMENTS_STD_VECTOR(std::vector<Tensor>, terms)

    TraceIdentity(std::vector<std::size_t> const &t_tensorSize);

    TraceIdentity &operator*=(csl::Expr const &t_factor);
    TraceIdentity &operator/=(csl::Expr const &t_factor);

    bool isSame(TraceIdentity const &other) const;

    TraceIdentity &operator+=(TraceIdentity const &other);

    friend std::ostream &operator<<(
            std::ostream        &out,
            TraceIdentity const &identity
            );

private:

    std::vector<std::vector<Tensor>> step(
            std::vector<Tensor> const &term) const;

    std::size_t totalIndices(std::vector<Tensor> const &term) const;

    bool isFull(Tensor const &tensor) const;

    bool isFull(std::vector<Tensor> const &tensors) const;

public:

    csl::Expr factor = CSL_1;

private:

    std::size_t n;
    std::vector<std::size_t> tensorSize;
    std::vector<std::vector<Tensor>> terms;
};

struct PartitionPair {
    std::size_t ni;
    std::size_t mi;
};

std::ostream &operator<<(
        std::ostream                     &out,
        std::vector<PartitionPair> const &partition
        );

std::vector<std::vector<PartitionPair>> evenPartition(std::size_t n);

std::vector<TraceIdentity> traceIdentity(
        algebra::Type type,
        std::size_t n
        );
std::vector<TraceIdentity> traceIdentity(
        algebra::Type type,
        std::size_t l,
        std::size_t n
        );

std::vector<TraceIdentity> ATraceIdentity(
        std::size_t l,
        std::size_t n
        );
std::vector<TraceIdentity> BTraceIdentity(
        std::size_t l,
        std::size_t n
        );
std::vector<TraceIdentity> CTraceIdentity(
        std::size_t l,
        std::size_t n
        );
std::vector<TraceIdentity> DTraceIdentity(
        std::size_t l,
        std::size_t n
        );
std::vector<TraceIdentity> E6TraceIdentity(
        std::size_t n
        );
std::vector<TraceIdentity> E7TraceIdentity(
        std::size_t n
        );
std::vector<TraceIdentity> E8TraceIdentity(
        std::size_t n
        );
std::vector<TraceIdentity> F4TraceIdentity(
        std::size_t n
        );
std::vector<TraceIdentity> G2TraceIdentity(
        std::size_t n
        );

}

#endif
