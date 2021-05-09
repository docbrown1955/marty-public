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
#ifndef FIELD_H_INCLUDED
#define FIELD_H_INCLUDED

#include "parent.h"
#include "indicial.h"

namespace csl {

class FieldParent: public AbstractParent{

    protected:

    const Space* spaceField;

    public:

    FieldParent();

    explicit FieldParent(const std::string& t_name);

    FieldParent(const FieldParent& field) = default;

    FieldParent(const std::string& t_name, const Space* t_spaceField);

    ~FieldParent(){};

    cslParent::PrimaryType getPrimaryType() const override;

    const Space* getFieldSpace() const override;

    protected:

    void checkFieldRequest(TensorParent* point);
};

class ScalarFieldParent: public FieldParent{

    public:

    ScalarFieldParent();

    explicit ScalarFieldParent(const std::string& t_name);

    ScalarFieldParent(const ScalarFieldParent& field) = default;

    ScalarFieldParent(const std::string& t_name, const Space* t_spaceField);

    ~ScalarFieldParent(){};

    template<class ...Args>
        friend
        std::shared_ptr<ScalarFieldParent> sfparent_(Args&& ...args)
        {
            return csl::make_shared<ScalarFieldParent>(args...);
        }

    cslParent::Type getType() const override;

    Expr operator()(const Tensor& point) override;
};

class AbstractField: public AbstractElement{

    protected:

    mutable Tensor point;

    public:

    AbstractField(const Tensor& t_vector,
                  const Parent&  t_parent);

    ~AbstractField(){};

    csl::PrimaryType getPrimaryType() const override;

    Tensor getPoint() const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(Parent_info t_parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    void setPoint(const Tensor& t_vector) override;

    protected:

    const Space* getSpace() const;
}; 

class ScalarField: public AbstractField{

    public:

    ScalarField(const Tensor& t_vector,
                const Parent&  t_parent);

    explicit
    ScalarField(const Expr& otherField);

    explicit
    ScalarField(const Abstract* otherField);

    ~ScalarField(){};

    csl::Type getType() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode=0) const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    bool commutesWith(Expr_info expr, int sign=-1) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;
};

} // End of namespace csl

#endif
