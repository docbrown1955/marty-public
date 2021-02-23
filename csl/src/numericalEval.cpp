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

#include "numericalEval.h"
#include "numerical.h"
#include "interface.h"
#include "literal.h"
#include "error.h"

namespace csl {

NumericalEval::NumericalEval()
{

}

NumericalEval::NumericalEval(long double t_value)
    :value(t_value)
{

}

NumericalEval::NumericalEval(long double t_value,
                             long double t_delta)
    :value(t_value),
    delta_plus(t_delta),
    delta_minus(t_delta)
{

}

NumericalEval::NumericalEval(long double t_value,
                             long double t_delta_plus,
                             long double t_delta_minus)
    :value(t_value),
    delta_plus(t_delta_plus),
    delta_minus(t_delta_minus)
{

}

csl::Type NumericalEval::getType() const
{
    return csl::Type::NumericalEval;
}

void NumericalEval::print(
        int mode,
        std::ostream& out,
        bool lib) const
{
    if (lib) {
        autonumber_s(value)->print(mode, out, lib);
        return;
    }
    if (value < 0) {
        out << "-";
        NumericalEval(-value, delta_minus, delta_plus).print(mode, out);
        return;
    }
    if (delta_plus != 0 or delta_minus != 0)
        out << "(";
    out << value;
    if (delta_plus == delta_minus)
        out << " +- " << delta_plus;
    else
        out << " ^{+ " << delta_plus << "}"
            << " _{- " << delta_minus << "}";
    if (delta_plus != 0 or delta_minus != 0)
        out << ")";

    if (mode == 0)
        out << std::endl;
}

void NumericalEval::printCode(
        int,
        std::ostream &out
        ) const
{
    out << "csl::numericaleval_s(" << value << ", " << delta_minus
        << ", " << delta_plus << ")";
}

std::string NumericalEval::printLaTeX(int mode) const
{
    if (value < 0) {
        return "-" + NumericalEval(-value, delta_minus, delta_plus)
            .printLaTeX(mode);
    }

    std::ostringstream sout;
    if (delta_plus != 0 or delta_minus != 0)
        sout << "(";
    sout << value;
    if (delta_plus == delta_minus)
        sout << " \\pm " << delta_plus;
    else
        sout << " ^{+ " << delta_plus << "}"
            << " _{- " << delta_minus << "}";
    if (delta_plus != 0 or delta_minus != 0)
        sout << ")";

    if (mode == 0)
        sout << std::endl;

    return sout.str();
}

long double NumericalEval::evaluateScalar() const
{
    return value;
}

long double NumericalEval::getValue() const
{
    return value;
}

long double NumericalEval::getDeltaPlus() const
{
    return delta_plus;
}

long double NumericalEval::getDeltaMinus() const
{
    return delta_minus;
}

std::optional<Expr> NumericalEval::evaluate(
        csl::eval::mode) const
{
    return std::nullopt;
}

unique_Expr NumericalEval::copy_unique() const
{
    return std::make_unique<NumericalEval>(*this);
}

Expr NumericalEval::addition_own(const Expr& other) const
{
    if (other->getType() ==  csl::Type::Complex)
        return other->addition_own(copy());

    long double new_delta_plus 
        = sqrt(delta_plus * delta_plus
             + other->getDeltaPlus() * other->getDeltaPlus());

    long double new_delta_minus 
        = sqrt(delta_minus * delta_minus
             + other->getDeltaMinus() * other->getDeltaMinus());

    return csl::make_shared<NumericalEval>(
            value + other->evaluateScalar(),
            new_delta_plus,
            new_delta_minus);
}

Expr NumericalEval::multiplication_own(Expr const& other, bool) const
{
    if (other->getType() ==  csl::Type::Complex)
        return other->multiplication_own(copy());

    long double new_delta_plus 
        = sqrt(pow(delta_plus, 2) * other->evaluateScalar()
             + pow(other->getDeltaPlus(), 2) * value);

    long double new_delta_minus 
        = sqrt(pow(delta_minus, 2) * other->evaluateScalar()
             + pow(other->getDeltaMinus(), 2) * value);

    long double new_value = value * other->evaluateScalar();
    return csl::make_shared<NumericalEval>(
            new_value,
            new_delta_plus,
            new_delta_minus);
}

Expr NumericalEval::division_own(const Expr& other) const
{
    if (other->getType() == csl::Type::Complex)
        return GetComplexConjugate(other)->multiplication_own(copy())
            / pow_s(GetComplexModulus(other), int_s(2));
    if (other->evaluateScalar() == 0)
        return CSL_INF;

    long double new_delta_plus 
        = sqrt(pow(delta_plus / other->evaluateScalar(), 2)
             + pow(value * other->getDeltaMinus() 
                 / (pow(other->evaluateScalar(), 2)), 2));

    long double new_delta_minus 
        = sqrt(pow(delta_minus / other->evaluateScalar(), 2)
             + pow(value * other->getDeltaPlus() 
                            / (pow(other->evaluateScalar(), 2))
                    , 2));

    long double new_value = value / other->evaluateScalar();
    return csl::make_shared<NumericalEval>(
            new_value,
            new_delta_plus,
            new_delta_minus);
}

Expr NumericalEval::exponentiation_own(const Expr& other) const
{
    CSL_ASSERT_SPEC(other->getType() != csl::Type::Complex,
            CSLError::NotImplementedError,
            "Cannot exponentiate a NumericalEval with a Complex number.");
    long double eval = other->evaluateScalar();
    if (eval == 0)
        return CSL_1;
    if (eval < 0)
        return CSL_1 / exponentiation_own(-other);

    long double new_delta_plus
        = sqrt(
                pow(sqrt(eval)*pow(value, eval-1)*delta_plus, 2) + 
                pow(other->getDeltaPlus()*log(value)*pow(value, eval), 2)
              );

    long double new_delta_minus
        = sqrt(
                pow(sqrt(eval)*pow(value, eval-1)*delta_minus, 2) + 
                pow(other->getDeltaMinus()*log(value)*pow(value, eval), 2)
              );

    return csl::make_shared<NumericalEval>(
            pow(value, eval),
            new_delta_plus,
            new_delta_minus);
}

std::optional<Expr> NumericalEval::derive(Expr_info) const
{
    return CSL_0;
}

bool NumericalEval::operator==(Expr_info) const
{
    return false;
}

Expr numericaleval_s(long double value,
                    long double delta_plus)
{
    return numericaleval_s(value, delta_plus, delta_plus);
}

Expr numericaleval_s(long double value,
                    long double delta_plus,
                    long double delta_minus)
{
    delta_minus = std::abs(delta_minus);
    if (delta_plus == 0 and delta_minus == 0)
        return autonumber_s(value);

    return csl::make_shared<NumericalEval>(
            value,
            delta_plus,
            delta_minus);
}

} // End of namespace csl
