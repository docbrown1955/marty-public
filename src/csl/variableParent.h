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
 * @version 2.0

 * \brief
 */
#ifndef VARIABLE_PARENT_H_INCLUDED
#define VARIABLE_PARENT_H_INCLUDED

#include "abstract.h"
#include "parent.h"

namespace csl {

class LiteralParent : public AbstractParent {

  public:
    explicit LiteralParent(std::string const &t_name = "");

    LiteralParent(std::string const &t_name, long double t_value);

    LiteralParent(std::string const &t_name, Expr const &t_value);

    // Assignment and copy are forbidden
    LiteralParent(const LiteralParent &)            = delete;
    LiteralParent &operator=(const LiteralParent &) = delete;

    ~LiteralParent()
    {
    }

    cslParent::PrimaryType getPrimaryType() const override
    {
        return cslParent::Literal;
    }

    bool isValued() const override;

    Expr getValue() const;

    void setValue(long double t_value);

    void setValue(Expr const &t_value);

    virtual Expr generateInstance() = 0;

  private:
    virtual bool isInstance(Expr_info expr) const = 0;

  protected:
    bool valued;

    Expr value;
};

class ConstantParent : public LiteralParent {

  public:
    explicit ConstantParent(std::string const &t_name = "");

    ConstantParent(std::string const &t_name, long double t_value);

    ConstantParent(std::string const &t_name, Expr const &t_value);

    // Assignment and copy are forbidden
    ConstantParent(const ConstantParent &) = delete;

    ConstantParent &operator=(const ConstantParent &) = delete;

    ~ConstantParent()
    {
    }

    cslParent::Type getType() const override
    {
        return cslParent::Constant;
    }

    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    Expr generateInstance() override;

  private:
    bool isInstance(Expr_info expr) const override;
};

class VariableParent : public LiteralParent {

  public:
    explicit VariableParent(std::string const &t_name = "");

    VariableParent(std::string const &t_name, long double t_value);

    VariableParent(std::string const &t_name, Expr const &t_value);

    // Assignment and copy are forbidden
    VariableParent(const VariableParent &)            = delete;
    VariableParent &operator=(const VariableParent &) = delete;

    ~VariableParent()
    {
    }

    cslParent::Type getType() const override
    {
        return cslParent::Variable;
    }

    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    bool isElementary() const;

    bool isAllDependencies() const;

    void setElementary(bool t_elementary);

    void setAllDependencies(bool t_allDependencies);

    void addDependency(Expr_info expr);

    void removeDependency(Expr_info expr);

    bool dependsOn(Expr_info expr) const override;

    bool commutesWith(Expr_info expr, int sign = -1);

    Expr generateInstance() override;

  private:
    bool isInstance(Expr_info expr) const override;

  private:
    bool elementary;

    // If not elementary, the variable can a priori depend on other variables.
    // If allDependencies = true, dependencies is a list of all variables of
    // which *this DOES NOT depend, *this depends on all the rest by default.
    // IF allDependencies = false, dependecies is a list of all variables of
    // which *this DOES depend, *this is by default independant of all the
    // rest.
    bool allDependencies;

    std::vector<unique_Expr_c> dependencies;
};

} // namespace csl

#endif
