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
#ifndef PROPAGATOR_H_INCLUDED
#define PROPAGATOR_H_INCLUDED

#include "../../csl/csl.h"

namespace mty {

class Propagator : public csl::AbstractMultiFunc {

  public:
    static inline csl::Expr Peps = csl::constant_s("reg_prop");

    Propagator() = default;

    Propagator(csl::Expr const &impulsion,
               csl::Expr const &mass,
               csl::Expr const &width = CSL_0);

    void print(int           mode = 0,
               std::ostream &out  = std::cout,
               bool          lib  = false) const override;

    std::string printLaTeX(int mode = 0) const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr refresh() const override;

    csl::Expr deepCopy() const override;

    csl::Expr deepRefresh() const override;

    std::optional<csl::Expr> evaluate(csl::eval::mode user_mode
                                      = csl::eval::base) const override;

    std::optional<csl::Expr> derive(csl::Expr_info variable) const override;

    std::optional<csl::Expr> getComplexConjugate() const override;

    bool isIndexed() const override;

    csl::IndexStructure getIndexStructure() const override;

    bool operator==(csl::Expr_info other) const override;
};

inline csl::allocator<Propagator> alloc_propagator;

csl::Expr propagator_s(csl::Expr const &impulsion,
                       csl::Expr const &mass,
                       csl::Expr const &width = CSL_0);

class FermionPropStruct : public csl::AbstractDuoFunc {

  public:
    FermionPropStruct() = default;

    FermionPropStruct(csl::Expr const & impulsion,
                      csl::Expr const & mass,
                      csl::Index const &alpha,
                      csl::Index const &beta);

    void print(int           mode = 0,
               std::ostream &out  = std::cout,
               bool          lib  = false) const override;

    std::string printLaTeX(int mode = 0) const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr refresh() const override;

    csl::Expr deepCopy() const override;

    csl::Expr deepRefresh() const override;

    std::optional<csl::Expr> evaluate(csl::eval::mode user_mode
                                      = csl::eval::base) const override;

    bool isIndexed() const override;

    csl::IndexStructure getIndexStructure() const override;

    bool operator==(csl::Expr_info other) const override;

    std::optional<csl::Expr>
    getHermitianConjugate(const csl::Space *space) const override;

    std::optional<csl::Expr> getHermitianConjugate(
        const std::vector<const csl::Space *> &spaces) const override;

    std::optional<csl::Expr> replaceIndex(const csl::Index &indexToReplace,
                                          const csl::Index &newIndex,
                                          bool refresh = true) const override;

  private:
    csl::IndexStructure structure;
};

inline csl::allocator<FermionPropStruct> alloc_fermionprop;

csl::Expr fermionPropStruct_s(csl::Expr const & impulsion,
                              csl::Expr const & mass,
                              csl::Index const &alpha,
                              csl::Index const &beta);

} // namespace mty

#endif
