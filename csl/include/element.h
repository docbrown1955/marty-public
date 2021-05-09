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
#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED

#include "parent.h"
#include "literal.h"
#include "numerical.h"

namespace csl {

/*! 
 * \brief Base class for all elements. Objects that are constructed by a parent
 * (see AbstractParent) when the user give it what is needed to create the
 * element (indices, space-time point etc).
 */
class AbstractElement: public Complexified{

    protected:

    /*! \brief Pointer to the parent that gave birth to this element.
     */
    std::shared_ptr<AbstractParent> parent;

    public:

    /*! Default constructor.
     */
    explicit
    AbstractElement();

    /*! \brief Intializes the parent and no name.
     * \param t_parent Parent for the Element.
     */
    explicit
    AbstractElement(const std::shared_ptr<AbstractParent>&  t_parent);

    /*! \brief Destructor */
    ~AbstractElement(){};

    /*! \return A pointer to the parent.
     */
    Parent getParent() const override;

    Parent_info getParent_info() const override;

    std::string const &getName() const override;

    std::string const &getLatexName() const override;

    bool getCommutable() const override;

    csl::ComplexProperty getComplexProperty() const override;

    std::vector<Parent> getSubSymbols() const override;

    /*! \brief Changes the Parent of the Element, as the name and the 
     * commutability property from those of \b t_parent.
     * \param t_parent New parent for the Element.
     */
    void setParent(const Parent& t_parent) override;

    void setName(const std::string& t_name) override;

    void setCommutable(bool t_commutable) override;

    void setComplexProperty(csl::ComplexProperty t_prop) override;

    int isPolynomial(Expr_info variable) const override;

    std::optional<Expr> getPolynomialTerm(
            Expr_info variable, int order) const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;
};

} // End of namespace std

#endif
