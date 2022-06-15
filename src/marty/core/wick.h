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

/*! \file wick.h
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief csl compatible expression representing the operator \<\> of
 * correlators in quantum field theory. Applies only on QuantumField, and uses
 * WickCalculator to evaluate to expression (applying wick theorem).
 */
#ifndef WICK_H_INCLUDED
#define WICK_H_INCLUDED

#include "quantumField.h"

namespace mty {

/*!
 *
 * \brief csl compatible expression representing the operator \<\> of
 * correlators in quantum field theory. Applies only on QuantumField, and uses
 * WickCalculator to evaluate to expression (applying wick theorem).
 * \details Inherits from Operator<AbstractFunc>, i.e. Wick is an Operator
 * (see operator.h) that contains a single argument.
 */
class Wick : public csl::Operator<csl::AbstractFunc> {

  public:
    /*!
     * \brief Default constructor.
     */
    Wick();

    /*!
     * \brief Constructor with one parameter.
     * \param expr csl::Expr initializing the argument of the operator.
     */
    explicit Wick(const csl::Expr &expr);

    /*!
     * \brief Destructor
     */
    ~Wick(){};

    csl::Type getType() const override;

    void print(int           mode = 0,
               std::ostream &out  = std::cout,
               bool          lib  = false) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    std::optional<csl::Expr> evaluate(csl::eval::mode user_mode
                                      = csl::eval::base) const override;

    /*!
     * \brief Wick applies on an expression if and only of it contains quantum
     * fields.
     * \param expr csl::Expr from which we check the dependancy.
     * \return \b True  if \b expr contains quantum fields.
     * \return \b False else.
     */
    bool operatorAppliesOn(csl::Expr_info expr) const override;

    csl::Expr getOperand() const override;

    void setOperand(const csl::Expr &operand) override;

    bool operator==(csl::Expr_info other) const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr deepCopy() const override;

    csl::Expr refresh() const override;

    csl::Expr deepRefresh() const override;

  private:
    /*!
     * \brief Counts the number of quantum fields in an expression. Counts also
     * when fields appear to an integer power.
     * \param expr csl::Expr in which we search quantum fields. Must be a
     * single field or a power of field. \return The number of quantum fields
     * in expr if it is a field or power of fields. \return -1 else.
     */
    static int numberOfFields(const csl::Expr &expr);

    /*!
     * \brief Counts the total number of fields in the (possible) product, and
     * replaces the argument by CSL_0 if the number is odd.
     */
    bool checkEvenNumberOfFields() const;

    /*!
     * \brief Checks if the argument contains **only** quantum fields or powers
     * of quantum fields in a product (Prod). If no, the member \b rightForm is
     * set to false and further evaluation of wick contraction cannot be
     * performed.
     */
    void checkRightForm();

    void expandToRightForm();

  protected:
    /*!
     * \brief Boolean that tells if the argument of the operator has the right
     * form to be contracted (see checkRightForm()).
     */
    mutable bool rightForm = false;
};

/*!
 * \brief Tries to create a Wick object with argument \b expr. Applies operator
 * (see Operator::applyOperator()) to eliminate constants (with respect to the
 * Wick object) and returns and resulting csl::Expr.
 * \param expr Initial argument of the Wick operator.
 * \return The resulting expression when the operator rules have been applied.
 */
csl::Expr wick_s(const csl::Expr &expr);

} // End of namespace mty

#endif /* WICK_H_INCLUDED */
