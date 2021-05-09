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
#ifndef NUMERICAL_EVAL_H_INCLUDED
#define NUMERICAL_EVAL_H_INCLUDED

#include "numerical.h"

namespace csl {

class NumericalEval: public AbstractNumerical {

    protected:

    long double value = 0;

    long double delta_plus = 0;

    long double delta_minus = 0;

    public:

    NumericalEval();

    explicit
    NumericalEval(long double value);

    NumericalEval(long double value,
                  long double delta);

    NumericalEval(long double value,
                  long double delta_plus,
                  long double delta_minus);

    NumericalEval(NumericalEval const&) = default;

    NumericalEval& operator=(NumericalEval const& ) = default;

#ifdef DEBUG
    ~NumericalEval(){ __record_data_alloc(static_cast<int>(getType())); }
#else

    ~NumericalEval(){};
#endif


    csl::Type getType() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode=0,
            std::ostream& out=std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    long double evaluateScalar() const override;

    long double getValue() const override;

    long double getDeltaPlus() const override;

    long double getDeltaMinus() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    Expr addition_own(const Expr& expr) const override;

    Expr division_own(const Expr& expr) const override;

    Expr exponentiation_own(const Expr& expr) const override;

    std::optional<Expr> derive(Expr_info expr) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

Expr numericaleval_s(long double value,
                     long double delta_plus);

Expr numericaleval_s(long double value,
                     long double delta_plus,
                     long double delta_minus);

} // End of namespace csl

#endif
