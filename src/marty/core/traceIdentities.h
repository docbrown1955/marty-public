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
 * @version 2.0

 * \brief
 */
#ifndef TRACE_IDENTITIES_H_INCLUDED
#define TRACE_IDENTITIES_H_INCLUDED

#include "../../csl/csl.h"
#include "group.h"

namespace mty {

class TraceIdentity {

  public:
    struct Tensor {
        size_t              finalSize;
        std::vector<size_t> indices;

        Tensor(size_t t_finalSize) : finalSize(t_finalSize)
        {
            indices.reserve(finalSize);
        }

        Tensor(Tensor const &other) = default;
        Tensor(Tensor &&other)      = default;
        Tensor &operator=(Tensor const &other) = default;
        Tensor &operator=(Tensor &&other) = default;
        ~Tensor()                         = default;

        IMPLEMENTS_STD_VECTOR(size_t, indices);
    };

    IMPLEMENTS_STD_VECTOR(std::vector<Tensor>, terms)

    TraceIdentity(std::vector<size_t> const &t_tensorSize);

    TraceIdentity &operator*=(csl::Expr const &t_factor);
    TraceIdentity &operator/=(csl::Expr const &t_factor);

    bool isSame(TraceIdentity const &other) const;

    TraceIdentity &operator+=(TraceIdentity const &other);

    friend std::ostream &operator<<(std::ostream &       out,
                                    TraceIdentity const &identity);

  private:
    std::vector<std::vector<Tensor>>
    step(std::vector<Tensor> const &term) const;

    size_t totalIndices(std::vector<Tensor> const &term) const;

    bool isFull(Tensor const &tensor) const;

    bool isFull(std::vector<Tensor> const &tensors) const;

  public:
    csl::Expr factor = CSL_1;

  private:
    size_t                           n;
    std::vector<size_t>              tensorSize;
    std::vector<std::vector<Tensor>> terms;
};

struct PartitionPair {
    size_t ni;
    size_t mi;
};

std::ostream &operator<<(std::ostream &                    out,
                         std::vector<PartitionPair> const &partition);

std::vector<std::vector<PartitionPair>> evenPartition(size_t n);

std::vector<TraceIdentity> traceIdentity(algebra::Type type, size_t n);
std::vector<TraceIdentity>
traceIdentity(algebra::Type type, size_t l, size_t n);

std::vector<TraceIdentity> ATraceIdentity(size_t l, size_t n);
std::vector<TraceIdentity> BTraceIdentity(size_t l, size_t n);
std::vector<TraceIdentity> CTraceIdentity(size_t l, size_t n);
std::vector<TraceIdentity> DTraceIdentity(size_t l, size_t n);
std::vector<TraceIdentity> E6TraceIdentity(size_t n);
std::vector<TraceIdentity> E7TraceIdentity(size_t n);
std::vector<TraceIdentity> E8TraceIdentity(size_t n);
std::vector<TraceIdentity> F4TraceIdentity(size_t n);
std::vector<TraceIdentity> G2TraceIdentity(size_t n);

} // namespace mty

#endif
