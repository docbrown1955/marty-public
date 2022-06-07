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

/**
 * @file lagrangian.h
 * @brief Contains the Lagrangian class, that encapsulates all interaction 
 * terms for a model.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-02
 */
#ifndef LAGRANGIAN_H_INCLUDED
#define LAGRANGIAN_H_INCLUDED

#include "quantumField.h"
#include "interactionTerm.h"

namespace mty::interaction {

enum Type {
    Kinetic,
    Mass,
    Interaction,
};

}

namespace mty {

class Expander;

mty::interaction::Type determineTermType(const InteractionTerm& term);

/*! 
 * \brief Interaction lagrangian of a model, allows to get all diagrams
 * for a particular process by expanding it the exponential way.
 */
class Lagrangian {

    public:

    using TermType = InteractionTerm::TermType;

    protected:

    mutable bool expressionComputed = false;

    mutable csl::Expr expression;

    public:

    std::vector<TermType> kinetic;
    
    std::vector<TermType> mass;

    std::vector<TermType> interaction;

    public:

    Lagrangian();

    Lagrangian(const Lagrangian& other) = default;

    ~Lagrangian(){};

    bool contains(const TermType& searchTerm) const;
    bool contains(const InteractionTerm& searchTerm) const;

    csl::Expr getExpression() const;

    csl::Tensor getPoint() const;

    void mergeTerms();

    void removeParticle(Particle const &particle);

    csl::Expr operator()(const csl::Expr& point);

    std::size_t size() const;

    std::size_t fullSize() const;

    bool empty() const;

    bool totalEmpty() const;

    TermType operator[](std::size_t pos) const;

    TermType& operator[](std::size_t pos);

    std::vector<TermType>::iterator begin();

    std::vector<TermType>::iterator end();

    std::vector<TermType>::const_iterator begin() const;

    std::vector<TermType>::const_iterator end()   const;

    void push_back(const csl::Expr& newTerm);

    void push_back(InteractionTerm const &newTerm);

    void push_back(TermType const &newTerm);

    friend
    std::ostream& operator<<(std::ostream& fout, const Lagrangian& L);

    static 
    void mergeTerms(std::vector<TermType>& terms);

    protected:

    void ensurePoint(csl::Expr &expr);

    void ensurePoint(InteractionTerm &term);
};

inline 
std::size_t Lagrangian::size() const
{
    return interaction.size();
}

inline
std::size_t Lagrangian::fullSize() const
{
    return kinetic.size()
         + mass.size()
         + interaction.size();
}

inline 
bool Lagrangian::empty() const
{
    return begin() == end();
}

inline
bool Lagrangian::totalEmpty() const
{
    return kinetic.empty() and mass.empty() and interaction.empty();
}

inline
Lagrangian::TermType Lagrangian::operator[](std::size_t pos) const
{
    return interaction[pos];
}

inline 
Lagrangian::TermType& Lagrangian::operator[](std::size_t pos)
{
    return interaction[pos];
}

inline 
std::vector<Lagrangian::TermType>::iterator Lagrangian::begin() 
{
    return interaction.begin();
}

inline 
std::vector<Lagrangian::TermType>::iterator Lagrangian::end() 
{
    return interaction.end();
}

inline 
std::vector<Lagrangian::TermType>::const_iterator Lagrangian::begin() const
{
    return interaction.begin();
}

inline 
std::vector<Lagrangian::TermType>::const_iterator Lagrangian::end() const
{
    return interaction.end();
}

} // End of namespace mty

#endif /* LAGRANGIAN_H */
