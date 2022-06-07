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

#include "lagrangian.h"
#include "mrtError.h"
#include "mrtUtils.h"
#include "quantumField.h"

using namespace std;
using namespace csl;

int int_log_sa(int x, int a)
    // returns rounded log_sa(x) 
{
    return round(log(x)/log(a));
}

namespace mty {

mty::interaction::Type determineTermType(const InteractionTerm& term)
{
    if (term.size() > 2)
        return mty::interaction::Interaction;
    for (const auto& arg : term.getContent())
        if (arg.hasDerivative())
            return mty::interaction::Kinetic;
    return mty::interaction::Mass;
}


///////////////////////////////////////////////////
/*************************************************/
// Class Lagrangian                              //
/*************************************************/
///////////////////////////////////////////////////

Lagrangian::Lagrangian()
{}

bool Lagrangian::contains(const TermType& searchTerm) const
{
    return contains(*searchTerm);
}
bool Lagrangian::contains(const InteractionTerm& searchTerm) const
{
    for (const auto& t : kinetic)
        if (*t == searchTerm)
            return true;
    for (const auto& t : mass)
        if (*t == searchTerm)
            return true;
    for (const auto& t : interaction)
        if (*t == searchTerm)
            return true;
    return false;
}

csl::Expr Lagrangian::getExpression() const
{
    if (expressionComputed)
        return expression;
    csl::vector_expr terms(fullSize());
    std::function<csl::Expr(TermType const&)> termToExpr
        = [](TermType const& term) {
            return term->getFullExpression();
        };
    auto iter = std::transform(kinetic.begin(),
                               kinetic.end(),
                               terms.begin(),
                               termToExpr);
    iter = std::transform(mass.begin(),
                          mass.end(),
                          iter,
                          termToExpr);
    std::transform(interaction.begin(),
                   interaction.end(),
                   iter,
                   termToExpr);

    expression = sum_s(terms, true);
    expressionComputed = true;
    return expression;
}

csl::Tensor Lagrangian::getPoint() const
{
    if (not kinetic.empty())
        return kinetic[0]->getPoint();
    if (not mass.empty())
        return mass[0]->getPoint();
    if (not interaction.empty())
        return interaction[0]->getPoint();

    return mty::defaultSpaceTimePoint;
}

void Lagrangian::mergeTerms()
{
    mergeTerms(kinetic);
    mergeTerms(mass);
    mergeTerms(interaction);
}

void Lagrangian::removeParticle(Particle const &particle)
{
    for (std::size_t i = 0; i != kinetic.size(); ++i)
        if (kinetic[i]->containsExactly(particle.get())) {
            kinetic.erase(kinetic.begin() + i);
            --i;
        }
    for (std::size_t i = 0; i != mass.size(); ++i)
        if (mass[i]->containsExactly(particle.get())) {
            mass.erase(mass.begin() + i);
            --i;
        }
    for (std::size_t i = 0; i != interaction.size(); ++i)
        if (interaction[i]->containsExactly(particle.get())) {
            interaction.erase(interaction.begin() + i);
            --i;
        }
}

void Lagrangian::mergeTerms(vector<TermType>& terms)
{
    // Refreshing first all the terms
    std::vector<csl::Expr> linear;
    for (std::size_t i = 0; i != terms.size(); ++i) {
        std::vector<mty::QuantumField> const &content = terms[i]->getContent();
        if (content.size() == 1) {
            linear.push_back(terms[i]->getTerm());
            terms.erase(terms.begin() + i);
            --i;
            continue;
        }
    }

    csl::ProgressBar bar(terms.size(), "Merging all interaction terms");
    for (std::size_t i = 0; i + 1 < terms.size(); ++i) {
        if (terms.size() > 5000)
            bar.progress(i);
        bool merged = false;
        std::vector<csl::Expr> expressions;
        for (std::size_t j = i+1; j < terms.size(); ++j)
            if (terms[i]->hasSameContent(*terms[j])) {
                if (merged)
                    expressions.push_back(
                            csl::Expanded(terms[j]->getTerm()));
                else {
                    merged = true;
                    expressions = {
                        csl::Expanded(terms[i]->getTerm()),
                        csl::Expanded(terms[j]->getTerm())
                    };
                }
                terms.erase(terms.begin() + j);
                --j;
            }
        if (merged) {
            vector<TermType> newTerms
                = InteractionTerm::createAndDispatch(
                        csl::DeepRefreshed(sum_s(expressions))
                        );
            if (newTerms.empty()) {
                terms.erase(terms.begin() + i);
                --i;
            }
            else if (newTerms.size() == 1) {
                terms[i] = newTerms[0];
                if (newTerms.size() > 1)
                    CallHEPError(mty::error::RuntimeError,
                            "More than 1 term created with same content, "
                            + static_cast<string>("should not happen."));
            }
        }
    }

    auto linearSum = csl::sum_s(linear);
    if (csl::IsSum(linearSum))
        linear = linearSum->getVectorArgument();
    else if (linearSum != CSL_0)
        linear = {linearSum};
    else
        linear.clear();
    if (!linear.empty()) {
        std::cerr << "Warning : the following linear terms have been "
            << "removed from the theory. You should probably check that "
            << "they are zero.\n";
        for (const auto &lin : linear)
            std::cerr << "Linear : " << lin << std::endl;
        std::cerr << std::endl;
    }
}

void Lagrangian::push_back(const csl::Expr& newTerm)
{
    csl::Expr cpy = csl::DeepCopy(newTerm);
    ensurePoint(cpy);
    vector<TermType> terms 
        = InteractionTerm::createAndDispatch(cpy);
    for (const auto& term : terms)
        push_back(term);
}

void Lagrangian::push_back(InteractionTerm const &term)
{
    TermType newTerm = std::make_shared<InteractionTerm>(term);
    push_back(newTerm);
}

void Lagrangian::push_back(TermType const &newTerm)
{
    ensurePoint(*newTerm);

    mty::interaction::Type type = determineTermType(*newTerm);
    switch(type) {
        case mty::interaction::Kinetic:     kinetic.    push_back(newTerm); break;
        case mty::interaction::Mass:        mass.       push_back(newTerm); break;
        case mty::interaction::Interaction: interaction.push_back(newTerm); break;
    }
}

void Lagrangian::ensurePoint(csl::Expr &expr)
{
    csl::ForEachLeaf(expr, [&](csl::Expr &sub) {
        if (csl::IsField(sub)) {
            sub->setPoint(getPoint());
        }
    });
}

void Lagrangian::ensurePoint(mty::InteractionTerm &term)
{
    term.setPoint(getPoint());
}

std::ostream& operator<<(std::ostream& fout, const Lagrangian& L)
{
    fout << L.kinetic.size() << " kinetic terms:\n";
    std::size_t i = 0;
    for (const auto& term : L.kinetic)
        fout << i++ << " (" << term->size() << ") : " 
            << term->getTerm() << "\n\n";
    fout << L.mass.size() << " mass terms:\n";
    i = 0;
    for (const auto& term : L.mass)
        fout << i++ << " (" << term->size() 
            << ") : " << term->getTerm() << "\n\n";
    fout << L.interaction.size() << " interaction terms:\n";
    i = 0;
    for (const auto& term : L.interaction)
        fout << i++ << " (" << term->size() << ") : " 
            << term->getTerm() << "\n\n";

    return fout;
}

} // End of namespace mty
