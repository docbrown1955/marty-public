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
#ifndef FERMION_CURRENT_H_INCLUDED
#define FERMION_CURRENT_H_INCLUDED

#include "diracology.h"

namespace mty {

struct ParticleInsertion {
    ParticleInsertion() = default;
    ParticleInsertion(Particle t_p) : p(t_p)
    {
    }
    ParticleInsertion(Particle t_p, bool t_c) : p(t_p), conjugate(t_c)
    {
    }
    mty::Particle p;
    bool          conjugate{false};
};

class FermionCurrent {

  public:
    enum Type {

        Scalar,       // Id
        PseudoScalar, // gamma_chir
        LScalar,      // P_L
        RScalar,      // P_R
        Vector,       // gamma_mu
        Axial,        // gamma_mu gamma_chir
        LVector,      // gamma_mu P_L
        RVector,      // gamma_mu P_R
        Magnetic,     // gamma_mu gamma_nu
        AMagnetic,    // gamma_mu gamma_nu gamma_chir
        LMagnetic,    // gamma_mu gamma_nu P_L
        RMagnetic,    // gamma_mu gamma_nu P_R
    };

  private:
    FermionCurrent(std::vector<csl::Expr> const &t_tensors,
                   const mty::DiracSpace *       t_diracSpace = &mty::dirac4);

  public:
    friend std::vector<FermionCurrent> parseChains(
        csl::Expr &product, DiracSpace const *diracSpace, bool findAbrev);

    Type getType() const;
    void setType(Type t_type);

    csl::Expr getExpression() const;

    std::vector<csl::Expr> &      getTensors();
    std::vector<csl::Expr> const &getTensors() const;

    csl::Index getFirst() const;
    csl::Index getLast() const;

    csl::Expr getFactor() const;
    csl::Expr getLeft() const;
    csl::Expr getRight() const;

    void setFactor(csl::Expr const &t_factor);
    void setLeft(csl::Expr const &t_left);
    void setRight(csl::Expr const &t_right);

    Type determineTypeOfChain(std::vector<csl::Expr> const &tensors) const;
    void determineBorderIndices(std::vector<csl::Expr> const &tensors);

    bool operator==(FermionCurrent const &other) const;
    bool operator!=(FermionCurrent const &other) const;
    bool operator<(FermionCurrent const &other) const;
    bool operator>(FermionCurrent const &other) const;
    bool operator<=(FermionCurrent const &other) const;
    bool operator>=(FermionCurrent const &other) const;

    friend std::ostream &operator<<(std::ostream &        out,
                                    FermionCurrent const &current);

  private:
    const mty::DiracSpace *diracSpace;
    Type                   type;
    csl::Index             first;
    csl::Index             last;
    csl::Expr              factor;
    csl::Expr              left;
    csl::Expr              right;
    std::vector<csl::Expr> tensors;
};

std::vector<FermionCurrent> parseChains(csl::Expr &       product,
                                        DiracSpace const *diracSpace,
                                        bool              findAbrev = false);

std::ostream &operator<<(std::ostream &out, FermionCurrent::Type type);

} // namespace mty

#endif
