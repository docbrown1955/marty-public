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

#include "comparison.h"
#include "indicial.h"
#include "utils.h"
#include "index.h"

using namespace std;

namespace csl {

bool Comparator::dummyComparisonActive = false;
bool Comparator::dummyVecIntComparisonActive = false;
bool Comparator::freeIndexComparisonActive = false;
//const std::string Comparator::dummyName() = "ARBITRARY";
std::map<int,Expr> Comparator::arbitrary = std::map<int,Expr>();
std::map<int,Expr> Comparator::correspondance
            = std::map<int,Expr>();
std::map<Index,Index> Comparator::indexCorrespondance
            = std::map<Index,Index>();

Arbitrary::Arbitrary(int n, csl::Type t_type)
    :AbstractLiteral(),
    number(n),
    name(Comparator::dummyName()),
    type(t_type)
{}

csl::PrimaryType Arbitrary::getPrimaryType() const
{
    return csl::PrimaryType::Arbitrary;
}

csl::Type Arbitrary::getType() const
{
    return type;
}

std::string const &Arbitrary::getName() const
{
    return name;
}

bool Arbitrary::isArbitrary() const
{
    return true;
}

long long int Arbitrary::getNum() const
{
    return number;
}

void Arbitrary::print(
        int mode,
        std::ostream& out,
        bool) const
{
    out<<name;
    if (mode > 0)
        out<<endl;
}

string Arbitrary::printLaTeX(int) const
{
    return name;
}

optional<Expr> Arbitrary::evaluate(
        csl::eval::mode
        ) const
{
    callError(cslError::ArbitraryEvaluated,
            "Arbitrary::valuate()");
    return nullopt;
}

unique_Expr Arbitrary::copy_unique() const
{
    return make_unique<Arbitrary>(*this);
}

bool Arbitrary::operator==(Expr_info expr) const
{
    return Comparator::compare(expr, this);
}

void Comparator::setFreeIndexComparisonActive(bool t_activ)
{
    freeIndexComparisonActive = t_activ;
}

Expr Comparator::dummy(int n, csl::Type type)
{
    // If the arbitrary does not exist we create it
    if (arbitrary.find(n) == arbitrary.end())
        arbitrary[n] = csl::make_shared<Arbitrary>(n, type);

    return arbitrary[n];
}

string Comparator::dummyName()
{
    return "###Dummy###";
}

bool Comparator::compare(Expr_info expr, Expr_info dummy)
{
    // Two arbitraries are equal iif they are the same.
    if (expr->getPrimaryType() == csl::PrimaryType::Arbitrary) {
        if (expr->getType() == dummy->getType())
            return (dummy->getNum() == expr->getNum());
        else
            return false;
    }

    // If the dummy expression is not active, a regular expr cannot be equal
    // to a dummy expr.
    if (not dummyComparisonActive)
        return false;
    const int number = dummy->getNum();
    // Check for previous correspondance
    if (correspondance.find(number) == correspondance.end()) {
        // No correspondance here
        if (dummy->getType() == csl::Type::NoType 
                or dummy->getType() == expr->getType()) {
            correspondance[number] = expr->copy();
            return true;
        }
        else
            return false;
    }
    return (expr->operator==(correspondance[number]));
}

bool Comparator::compare(const Index& A, const Index& B)
{
    if (not freeIndexComparisonActive
            or A.getType() == cslIndex::Fixed
            or B.getType() == cslIndex::Fixed)
        return A == B;
    if (indexCorrespondance.find(A) == indexCorrespondance.end()) {
        if (indexCorrespondance.find(B) != indexCorrespondance.end())
            return false;
        indexCorrespondance[A] = B;
        return true;
    }
    else {
        freeIndexComparisonActive = false;
        if (indexCorrespondance[A] == B)
        if (indexCorrespondance.find(B) == indexCorrespondance.end()) {
            indexCorrespondance[B] = A;
            return true;
        }
        freeIndexComparisonActive = true;
    }

    return false;

    if (not freeIndexComparisonActive)
        return A == B;

    // We check existing correspondances
    if (indexCorrespondance.find(A) == indexCorrespondance.end()) {
        if (indexCorrespondance.find(B) != indexCorrespondance.end())
            // If A has no correspondance but B has A cannot be equal to B
            return false;
        // We set the correspondance A -> B and return true
        indexCorrespondance[A] = B;
        return true;
    }
    else {
        // Deactivating freeIndexComparison here in order to compare exactly.
        freeIndexComparisonActive = false;
        if (indexCorrespondance[A] == B) 
            if (indexCorrespondance.find(B) == indexCorrespondance.end()) {
                indexCorrespondance[B] = A;
                return true;
            }
        freeIndexComparisonActive = true;
    }
    
    return false;
}

bool Comparator::dummyComparison(Expr_info expr, Expr_info dummyExpr)
{
    // Function called by expr when comparing to dummyExpr.
    dummyComparisonActive = true;
    freeIndexComparisonActive = true;
    bool rep = expr->operator==(dummyExpr);
    clear();

    return rep;
}

bool Comparator::dummyComparison(Expr const& expr, Expr const& dummyExpr)
{
    return dummyComparison(expr.get(), dummyExpr.get());
}

bool Comparator::dummyVecIntComparison(Expr_info A, Expr_info B)
{
    dummyVecIntComparisonActive = true;
    bool rep = (A == B);
    dummyVecIntComparisonActive = false;
    return rep;
}

bool Comparator::freeIndexComparison(Expr_info A, Expr_info B)
{
    freeIndexComparisonActive = true;
    bool rep = A->operator==(B);
    // rep = rep and indexCorrespondance.empty();
    indexCorrespondance.clear();
    freeIndexComparisonActive = false;

    return rep;
}

void Comparator::clear()
{
    arbitrary           .clear();
    correspondance      .clear();
    indexCorrespondance .clear();
    dummyComparisonActive       = false;
    dummyVecIntComparisonActive = false;
    freeIndexComparisonActive   = false;
}

void Comparator::setDummyComparisonActive(bool state)
{
    dummyComparisonActive = state;
}

void Comparator::setDummyVecIntComparisonActive(bool state)
{
    dummyVecIntComparisonActive = state;
}
} // End of namespace csl
