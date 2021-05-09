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
#ifndef COMMUTATION_H_INCLUDED
#define COMMUTATION_H_INCLUDED

#include "scalarFunc.h"

namespace csl {

/*!
 * \brief Returns the result of the (anit-)commutation of \b A and \b B. In most 
 * cases it returns CSL_0, and returns CSL_UNDEF else. Useful to test if two 
 * objects commute.
 *
 * \param A Left hand side.
 * \param B Right hand side.
 * \param sign Sign of the commutator, -1 for commutation (default), 1 for
 * anti-commutation.
 *
 * \return CSL_0 if \b A and \b B commute.
 * \return CSL_UNDEF else.
 */
Expr Commutation(const Expr& A, const Expr& B, int sign=-1);

Expr Commutation(Expr_info A, Expr_info B, int sign=-1);

/*!
 * \brief Returns the result of the (anit-)commutation of \b A and \b B. In most 
 * cases it returns CSL_0, and returns CSL_UNDEF else. Useful to test if two 
 * objects commute. In the special of this function, \b A is an object that 
 * is not commutable (attribute Abstract::commutable) and \b B is commutable.
 *
 * \param A Left hand side.
 * \param B Right hand side.
 * \param sign Sign of the commutator, -1 for commutation (default), 1 for
 * anti-commutation.
 *
 * \return CSL_0 if \b A and \b B commute.
 * \return CSL_UNDEF else.
 */
Expr WeakCommutation(const Expr& A, const Expr& B, int sign=-1);

Expr WeakCommutation(Expr_info A, Expr_info B, int sign=-1);

/*!
 * 
 * \brief Object that represents a commutator or an anti-commutator, thus is 
 * derived from AbstractDuoFunc that handles two arguments -> [A,B].
 */
class Commutator: public AbstractDuoFunc{

    protected:

    /*!
     * \brief Sign of the commutator. +1 for anti-commutator, -1 for commutator.
     */
    int sign;

    public:

    /*!
     * \brief Default constructor.
     */
    Commutator();

    /*!
     * \brief Initializes the left- and right- hand sides of the Commutator,
     * constructs [A,B] (sign == -1) or {A,B} (sign == 1).
     *
     * \param A Left hand side.
     * \param B Right hand side.
     * \param t_sign Sign of the Commutator.
     */
    Commutator(const Expr& A, const Expr& B, int t_sign=-1);

    /*!
     * \brief Destructor.
     */
    ~Commutator(){};

    /*! \brief Gives the **type** of a Commutator object.
     * \return csl::Type::Commutator.                
     */
    csl::Type getType() const override; 

    /*!
     * \return The sign of the (anti-)commutator.
     */
    int getSign() const override;
                                                                              
    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;
                                                                              
    std::string printLaTeX(int mode=0) const override;
                                                                              
    long double evaluateScalar() const override;
                                                                              
    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;
                                                                              
    std::optional<Expr> derive(Expr_info expr) const override;
                                                                              
    int getParity(Expr_info t_variable) const override;
    
    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

/*!
 * \brief Tries to create a Commutator object of \b A and \b B of sign \b sign.
 * \details If the result is CSL_0, this function returns CSL_0. If \b A and/or
 * \b B are products, the functions expands the give at the end a more 
 * complicated expression but with just (anti-)commutators of single elements,
 * expressions of type [{E,F}] with E and F neither sums nor products.
 *
 * \param A Left hand side of the Commutator.
 * \param B Right hand side of the Commutator.
 * \param t_sign Sign of the Commutator.
 *
 * \return [A,B] (or {A,B}) if the result is not defined (in particular not 
 * CSL_0).
 * \return The result of [A,B] (or {A,B}) if there is special things to do.
 */
Expr commutator_(const Expr& A, const Expr& B, int t_sign=-1);
} // End of namespace csl

#endif
