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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Contains **interface functions** for the user.
 */
#ifndef HEP_INTERFACE_CURRENT_H_INCLUDED
#define HEP_INTERFACE_CURRENT_H_INCLUDED

#include "mrtInterface.h"
#include "amplitude.h"

namespace mty {

///////////////////////////////////////////////////
/*************************************************/
// Computation utilities                         //
/*************************************************/
///////////////////////////////////////////////////

inline void DisableParticle(Particle const& particle)
{
    DisableParticle(*mty::Model::current, particle);
}

inline void EnableParticle(Particle const& particle)
{
    EnableParticle(*mty::Model::current, particle);
}

inline void DisableParticle(std::string const& particle)
{
    DisableParticle(*mty::Model::current, particle);
}

inline void EnableParticle(std::string const& particle)
{
    EnableParticle(*mty::Model::current, particle);
}

inline 
Amplitude ComputeAmplitude(
            mty::Order             order,
            std::vector<Insertion> insertions,
            bool                   ruleMode = false
            )
{
    return ComputeAmplitude(
            order,
            *mty::Model::current,
            insertions,
            ruleMode
            );
}

inline 
Amplitude SelfEnergy(
            mty::Order        order,
            mty::Particle    &particle)
{
    return SelfEnergy(order, particle, *mty::Model::current);
}

inline 
Amplitude SelfEnergy(
            mty::Particle    &particle)
{
    return SelfEnergy(particle, *mty::Model::current);
}


inline
Amplitude ComputeAmplitude(
            mty::Order                       order,
            std::vector<Insertion>           insertions,
            std::vector<csl::Tensor>& impulsions,
            bool                             ruleMode = false
            )
{
    return ComputeAmplitude(
            order,
            *mty::Model::current,
            insertions,
            impulsions,
            ruleMode
            );
}

inline std::vector<FeynmanRule> ComputeFeynmanRules()
{
    return ComputeFeynmanRules(*mty::Model::current);
}

///////////////////////////////////////////////////
/*************************************************/
// Model building utilities                      //
/*************************************************/
///////////////////////////////////////////////////

template<class ...Args>
inline void AddGaugedGroup(Args&& ...args)
{
    AddGroup(*Model::current, std::forward<Args>(args)...);
}

inline void AddParticle(
        mty::Particle& particle,
        bool           initTerms = true
        )
{
    AddParticle(*mty::Model::current, particle, initTerms);
}

inline void AddTerm(
        csl::Expr const& term,
        bool        addCC = false
        )
{
    AddTerm(*mty::Model::current, term, addCC);
}

inline void AddTerm(
        std::vector<csl::Expr> const& terms,
        bool                     addCC = false
        )
{
    AddTerm(*mty::Model::current, terms, addCC);
}

inline mty::Particle GetParticle(
        std::string const& name
        )
{
    return GetParticle(*mty::Model::current, name);
}

inline void PromoteGoldstone(
        mty::Particle& goldstone,
        mty::Particle& gaugeBoson
        )
{
    PromoteGoldstone(*mty::Model::current, goldstone, gaugeBoson);
}

inline void PromoteGoldstone(
        char const* goldstone,
        char const* gaugeBoson)
{
    PromoteGoldstone(*mty::Model::current, goldstone, gaugeBoson);
}

inline csl::Expr GetCoupling(std::string const& nameCoupling)
{
    return GetCoupling(*mty::Model::current, nameCoupling);
}

inline csl::Tensor GetYukawa(std::string const& nameCoupling)
{
    return GetYukawa(*mty::Model::current, nameCoupling);
}

inline void Rename(
        std::string const& oldName,
        std::string const& newName
        )
{
    Rename(*mty::Model::current, oldName, newName);
}

inline void Init()
{
    Init(*mty::Model::current);
}

inline void Replaced(
        csl::Expr const& init,
        csl::Expr const& target
        )
{
    Replaced(*mty::Model::current, init, target);
}

inline void Replaced(
        csl::Tensor& init,
        csl::Expr   const& target
        )
{
    Replaced(*mty::Model::current, init, target);
}

inline void Replaced(
        csl::Tensor& init,
        csl::Tensor& target
        )
{
    Replaced(*mty::Model::current, init, target);
}

inline void Replaced(
        Particle   const& particle,
        csl::Expr       const& newTerm
        )
{
    Replaced(*mty::Model::current, particle, newTerm);
}

inline void Rotate(
        std::vector<mty::Particle> const& fields,
        std::vector<mty::Particle> const& newFields,
        std::vector<std::vector<csl::Expr>> const& rotation,
        bool diagonalizeMasses = false
        )
{
    Rotate(
            *mty::Model::current,
            fields,
            newFields,
            rotation,
            diagonalizeMasses
            );
}

inline void Rotate(
        std::vector<std::string> const& fields,
        std::vector<std::string> const& newFields,
        std::vector<std::vector<csl::Expr>> const& rotation,
        bool diagonalizeMasses = false
        )
{
    Rotate(
            *mty::Model::current,
            fields,
            newFields,
            rotation,
            diagonalizeMasses
          );
}

inline void DiagonalizeMassMatrices()
{
    DiagonalizeMassMatrices(*mty::Model::current);
}

inline void BreakGaugeSymmetry(
        std::string                           const& brokenGroup,
        std::vector<Particle>                 const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    BreakGaugeSymmetry(
            *mty::Model::current,
            brokenGroup,
            brokenFields,
            newNames
            );
}

inline void BreakGaugeSymmetry(
        std::string                           const& brokenGroup,
        std::initializer_list<std::string>    const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    BreakGaugeSymmetry(
            *mty::Model::current,
            brokenGroup,
            brokenFields,
            newNames
            );
}

inline void BreakFlavorSymmetry(
        std::string                           const& brokenFlavor,
        std::initializer_list<std::string>    const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    BreakFlavorSymmetry(
            *mty::Model::current,
            brokenFlavor,
            brokenFields,
            newNames
            );
}

inline void BreakFlavorSymmetry(
        std::string                           const& brokenFlavor,
        std::vector<mty::Particle>            const& brokenFields,
        std::vector<std::vector<std::string>> const& newNames
        )
{
    BreakFlavorSymmetry(
            *mty::Model::current,
            brokenFlavor,
            brokenFields,
            newNames
            );
}

inline void Refreshed()
{
    Refreshed(*mty::Model::current);
}

inline void IntegrateOutParticle(
        mty::Particle & particle,
        size_t maxDim = size_t(-1),
        size_t nLoops = 0,
        size_t maxDimOperator = 6)
{
    IntegrateOutParticle(
            *mty::Model::current,
            particle,
            maxDim,
            nLoops,
            maxDimOperator
            );
}

///////////////////////////////////////////////////
/*************************************************/
// Group theory                                  //
/*************************************************/
///////////////////////////////////////////////////

inline void SetGaugeChoice(
        std::string const& nameParticle,
        gauge::Type choice
        )
{
    SetGaugeChoice(
            *mty::Model::current,
            nameParticle,
            choice
            );
}

}

#endif
