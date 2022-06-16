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
#ifndef COLORSPACE_H_INCLUDED
#define COLORSPACE_H_INCLUDED

#include "../../csl/csl.h"
#include "group.h"
#include "semiSimpleAlgebra.h"
#include <functional>

namespace mty {
class TraceIdentity;
}
namespace color {

struct TraceIndices {
    csl::Expr               factor = CSL_1;
    std::vector<csl::Index> indices;
    size_t                  symmetry = 1;

    IMPLEMENTS_STD_VECTOR(csl::Index, indices)

    bool isSymmetrized() const
    {
        return indices.size() <= 2 or symmetry == indices.size() - 1;
    }

    friend std::ostream &operator<<(std::ostream &      out,
                                    TraceIndices const &trace);
};

/*!
 * \brief Class inherited from csl::Space that is used for vector spaces of
 * group representations, in particular for generators.
 */
class ColorSpace : public csl::Space {

  public:
    ColorSpace(const mty::SemiSimpleGroup *t_group,
               const std::string &         t_name,
               int                         t_dim,
               bool                        adjoint);

    void printCode(std::ostream &out, int indentSize) const override;

    void printDefinition(std::ostream &out,
                         int           indentSize,
                         bool          header = false) const override;

    std::string getName() const override;

    bool
    hasSpecialTraceProperty(const csl::vector_expr &tensors) const override;

    csl::Expr calculateTrace(csl::vector_expr tensors) const override;

    mty::SemiSimpleGroup const *getGroup() const
    {
        return group;
    }

    bool isInstance(csl::Expr const &tensor) const;

    void applyE(size_t j, std::vector<TraceIndices> &terms) const;

    std::vector<TraceIndices> symmetrize(TraceIndices const &init) const;

    csl::Expr applySingleTraceIdentity(TraceIndices const &      trace,
                                       mty::TraceIdentity const &id) const;

    csl::Expr applyTraceIdentity(
        TraceIndices const &                   trace,
        std::vector<mty::TraceIdentity> const &identities) const;

    csl::Expr symmetrizedTrace(TraceIndices const &trace) const;

    void mergeTraces(std::vector<TraceIndices> &traces) const;

    bool contractCloseIndices(TraceIndices &   indices,
                              csl::Expr const &CR,
                              csl::Expr const &CA) const;

    std::vector<TraceIndices> commuteTo(TraceIndices const &indices,
                                        size_t              init,
                                        size_t              targetPos) const;

    std::vector<TraceIndices> contractFarIndices(TraceIndices const &indices,
                                                 csl::Expr const &   CR,
                                                 csl::Expr const &   CA) const;

    std::vector<TraceIndices>
    contractStructureConstants(TraceIndices const &indices,
                               csl::Expr const &   CR,
                               csl::Expr const &   CA) const;

    using Modifier
        = std::function<std::vector<TraceIndices>(TraceIndices const &init)>;

    bool applyModifier(std::vector<TraceIndices> &indices,
                       Modifier const &           f) const;

    bool contractFarIndices(std::vector<TraceIndices> &indices,
                            csl::Expr const &          CR,
                            csl::Expr const &          CA) const;

    bool contractStructureConstants(std::vector<TraceIndices> &indices,
                                    csl::Expr const &          CR,
                                    csl::Expr const &          CA) const;

    std::vector<TraceIndices>
    contractIndices(std::vector<csl::Index> const &indices,
                    mty::Irrep const &             irrep) const;

    csl::Tensor getF() const;

    csl::Tensor getD(size_t p) const;

    csl::Tensor getDTilde() const;

  private:
    std::vector<csl::Index>
    exprToIndices(std::vector<csl::Expr> const &tensors) const;

    std::tuple<csl::Index, csl::Index, int>
    getFreeIndices(const csl::Expr &tensorA, const csl::Expr &tensorB) const;

    std::tuple<csl::Index, csl::Index, int>
    getFreeIndicesAdjoint(const csl::Expr &tensorA,
                          const csl::Expr &tensorB) const;

  private:
    const mty::SemiSimpleGroup *group;

    const mty::SemiSimpleAlgebra *algebra;
};

} // namespace color

#endif
