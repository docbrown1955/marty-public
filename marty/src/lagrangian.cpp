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
    for (size_t i = 0; i != kinetic.size(); ++i)
        if (kinetic[i]->containsExactly(particle.get())) {
            kinetic.erase(kinetic.begin() + i);
            --i;
        }
    for (size_t i = 0; i != mass.size(); ++i)
        if (mass[i]->containsExactly(particle.get())) {
            mass.erase(mass.begin() + i);
            --i;
        }
    for (size_t i = 0; i != interaction.size(); ++i)
        if (interaction[i]->containsExactly(particle.get())) {
            interaction.erase(interaction.begin() + i);
            --i;
        }
}

void Lagrangian::mergeTerms(vector<TermType>& terms)
{
    // Refreshing first all the terms
    std::vector<csl::Expr> linear;
    vector<TermType> refreshed;
    refreshed.reserve(terms.size());
    csl::ProgressBar bar(terms.size(), "Extracting all interaction terms");
    for (size_t i = 0; i != terms.size(); ++i) {
        if (terms.size() > 5000)
            bar.progress(i);
        std::vector<mty::QuantumField> const &content = terms[i]->getContent();
        csl::Expr expr = terms[i]->getTerm();
        if (content.size() == 1) {
            linear.push_back(expr);
            continue;
        }
        vector<TermType> foo = 
            InteractionTerm::createAndDispatch(expr);
        for (auto& term : foo)
            refreshed.push_back(term);
    }

    // Merging identical terms
    if (refreshed.size() > 1) {
        csl::ProgressBar bar(terms.size(), "Merging all interaction terms");
        for (size_t i = 0; i + 1 < refreshed.size(); ++i) {
            if (refreshed.size() > 5000)
                bar.progress(i);
            bool merged = false;
            std::vector<csl::Expr> expressions;
            for (size_t j = i+1; j < refreshed.size(); ++j)
                if (refreshed[i]->hasSameContent(*refreshed[j])) {
                    if (merged)
                        expressions.push_back(
                                csl::Expanded(refreshed[j]->getTerm()));
                    else {
                        merged = true;
                        expressions = {
                            csl::Expanded(refreshed[i]->getTerm()),
                            csl::Expanded(refreshed[j]->getTerm())
                        };
                    }
                    refreshed.erase(refreshed.begin() + j);
                    --j;
                }
            if (merged) {
                vector<TermType> terms
                    = InteractionTerm::createAndDispatch(
                            sum_s(expressions));
                if (terms.empty()) {
                    refreshed.erase(refreshed.begin() + i);
                    --i;
                }
                else if (terms.size() == 1) {
                    refreshed[i] = terms[0];
                    if (terms.size() > 1)
                        CallHEPError(mty::error::RuntimeError,
                                "More than 1 term created with same content, "
                                + static_cast<string>("should not happen."));
                }
            }
        }
    }

    terms = std::move(refreshed);
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
    vector<TermType> terms 
        = InteractionTerm::createAndDispatch(newTerm);
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
    if (not totalEmpty()) {
        Tensor selfPoint = getPoint();
        Tensor termPoint = newTerm->getPoint();
        if (not termPoint) {
            // No field in the interaction newTerm: constant
            return;
        }
        if (selfPoint.get() != termPoint.get()) {
            newTerm->setPoint(selfPoint);
        }
    }

    mty::interaction::Type type = determineTermType(*newTerm);
    switch(type) {
        case mty::interaction::Kinetic:     kinetic.    push_back(newTerm); break;
        case mty::interaction::Mass:        mass.       push_back(newTerm); break;
        case mty::interaction::Interaction: interaction.push_back(newTerm); break;
    }
}

std::ostream& operator<<(std::ostream& fout, const Lagrangian& L)
{
    cout << L.kinetic.size() << " kinetic terms:\n";
    size_t i = 0;
    for (const auto& term : L.kinetic)
        fout << i++ << " (" << term->size() << ") : " 
            << term->getTerm() << "\n\n";
    cout << L.mass.size() << " mass terms:\n";
    i = 0;
    for (const auto& term : L.mass)
        fout << i++ << " (" << term->size() 
            << ") : " << term->getTerm() << "\n\n";
    cout << L.interaction.size() << " interaction terms:\n";
    i = 0;
    for (const auto& term : L.interaction)
        fout << i++ << " (" << term->size() << ") : " 
            << term->getTerm() << "\n\n";

    return fout;
}

} // End of namespace mty
