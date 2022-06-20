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
#ifndef TENSORFIELD_H_INCLUDED
#define TENSORFIELD_H_INCLUDED

#include "field.h"
#include "indicial.h"
#include "parent.h"

namespace csl {

class TensorField;
class TDerivative;

class TensorFieldParent : public TensorParent {

  public:
    TensorFieldParent();

    explicit TensorFieldParent(const std::string &t_name);

    TensorFieldParent(const TensorFieldParent &field) = default;

    TensorFieldParent(const std::string &t_name, const Space *t_spaceField);

    TensorFieldParent(const std::string                &t_name,
                      const Space                      *t_spaceField,
                      const std::vector<const Space *> &spaces);

    template <class... Args>
    friend TensorField tensorfield_s(Args &&...args);

    template <class... Args>
    friend TensorField tensorfield_s(std::string const &name,
                                     csl::Space const  *spaceField,
                                     std::vector<const Space *> const &indices,
                                     Args &&...args);

    ~TensorFieldParent(){};

    std::vector<Parent>
    breakSpace(const Space                      *broken,
               const std::vector<const Space *> &newSpace,
               const std::vector<size_t>        &pieces) const override;

    cslParent::PrimaryType getPrimaryType() const override;

    cslParent::Type getType() const override;

    void printDefinition(std::ostream &out        = std::cout,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    const Space *getFieldSpace() const override;

    void setFieldSpace(const Space *t_space) override;

    virtual Expr operator()(const Tensor &point) override;

    virtual Expr operator()(Index index, const Tensor &point) override;

    virtual Expr operator()(const std::vector<int> &indices,
                            const Tensor           &point) override;

    virtual Expr operator()(std::vector<Index> indices,
                            const Tensor      &point) override;

  protected:
    void checkFieldRequest(TensorParent *point);

    void checkIndicialAndFieldRequest(TensorParent *point);

    void checkIndicialAndFieldRequest(const Index &index, TensorParent *point);

    void checkIndicialAndFieldRequest(const std::vector<Index> &indices,
                                      TensorParent             *point);

  protected:
    const Space *spaceField;
};

template <class... Args>
std::shared_ptr<TensorFieldParent> tensorfield_s(Args &&...args)
{
    return csl::make_shared<TensorFieldParent>(std::forward<Args>(args)...);
}

template <class... Args>
std::shared_ptr<TensorFieldParent>
tensorfield_s(std::string const                &name,
              csl::Space const                 *spaceField,
              std::vector<const Space *> const &indices,
              Args &&...args)
{
    return csl::make_shared<TensorFieldParent>(
        name, spaceField, indices, std::forward<Args>(args)...);
}

class TensorField : public std::shared_ptr<TensorFieldParent> {

  public:
    INHERIT_SHARED_PTR_CONSTRUCTOR(TensorField, TensorFieldParent)

    TensorField(const std::string &t_name, const Space *t_spaceField)
        : TensorField(tensorfield_s(t_name, t_spaceField))
    {
    }

    TensorField(const std::string                &t_name,
                const Space                      *t_spaceField,
                const std::vector<const Space *> &spaces)
        : TensorField(tensorfield_s(t_name, t_spaceField, spaces))
    {
    }

    template <class... Args>
    inline Expr operator()(Args &&...args)
    {
        return (**this)(std::forward<Args>(args)...);
    }

    template <class... Args>
    inline Expr operator()(const std::vector<int> &indices, Args &&...args)
    {
        return (**this)(indices, std::forward<Args>(args)...);
    }

    template <class... Args>
    inline Expr operator()(const std::vector<Index> &indices, Args &&...args)
    {
        return (**this)(indices, std::forward<Args>(args)...);
    }
};

DEFINE_SHARED_PTR_OPERATOR(TensorField)

class TensorFieldElement : public TensorElement {

  protected:
    mutable Tensor point;

  public:
    TensorFieldElement(const Tensor &t_vector, const Parent &t_parent);

    TensorFieldElement(const Tensor             &t_vector,
                       const Parent             &t_parent,
                       const std::vector<Index> &indices);

    TensorFieldElement(const Tensor         &t_vector,
                       const Parent         &t_parent,
                       const IndexStructure &indices);

  public:
    template <typename... Args>
    friend Expr tensorfieldelement_s(Args &&...args);

    ~TensorFieldElement(){};

    csl::PrimaryType getPrimaryType() const override;

    csl::Type getType() const override;

    bool isIndexed() const override
    {
        return (not index.empty());
    }

    csl::Tensor getPoint() const override;

    void setPoint(csl::Tensor const &t_point) override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    void print(int           mode = 0,
               std::ostream &out  = std::cout,
               bool          lib  = false) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    std::vector<csl::Parent> getSubSymbols() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    bool commutesWith(Expr_info expr, int sign = -1) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(Parent_info t_parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool compareWithDummy(Expr_info               other,
                          std::map<Index, Index> &constraints,
                          bool keepAllCosntraints = false) const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;
};

template <typename... Args>
Expr tensorfieldelement_s(Args &&...args)
{
    return csl::make_shared<TensorFieldElement>(std::forward<Args>(args)...)
        ->getCanonicalPermutation();
}

class TDerivativeElement : public Operator<TensorFieldElement> {

  protected:
    Expr operand = CSL_1;

  public:
    TDerivativeElement(const Tensor &t_vector,
                       const Parent &t_parent,
                       const Index  &index);

    TDerivativeElement(const Tensor &t_vector,
                       const Parent &t_parent,
                       const Index  &index,
                       const Expr   &operand,
                       bool          empty);

  public:
    ~TDerivativeElement(){};

    csl::Type getType() const override;

    size_t size() const override;

    void print(int           mode = 0,
               std::ostream &out  = std::cout,
               bool          lib  = false) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    std::string printLaTeX(int mode = 0) const override;

    std::vector<csl::Parent> getSubSymbols() const override;

    std::optional<Expr> factor(bool full = false) const override;

    std::optional<Expr> factor(Expr_info expr,
                               bool      full = false) const override;

    std::optional<Expr> collect(std::vector<Expr> const &factors,
                                bool full = false) const override;

    std::optional<Expr> expand(bool full    = false,
                               bool inPlace = false) const override;

    std::optional<Expr> expand_if(std::function<bool(Expr const &)> const &f,
                                  bool full    = false,
                                  bool inPlace = false) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool operatorAppliesOn(Expr_info expr) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsOn(Parent_info t_parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Parent_info expr) const override;

    Expr const &getArgument(int i) const override;

    Expr &getArgument(int i) override;

    void setArgument(Expr const &arg, int i) override;

    Expr getOperand() const override;

    void setOperand(const Expr &t_operand) override;

    void setPoint(const Tensor &newPoint) override;

    IndexStructure getIndexStructure() const override;

    void setIndexStructure(const IndexStructure &t_struct) override;

    /*!
     * \brief Checks for index contractions in the product. Applies Einstein's
     * summation convention and sums automatically repeated indices. This
     * function is called after the construction in order to leave a correct
     * expression.
     */
    void selfCheckIndexStructure();

    void replaceIndexInPlace(Index const &oldIndex,
                             Index const &newIndex) override;

    std::optional<Expr> replaceIndex(const Index &indexToReplace,
                                     const Index &newIndex,
                                     bool refresh = true) const override;

    std::optional<Expr>
    replaceIndices(std::vector<Index> const &indexToReplace,
                   std::vector<Index> const &newIndex,
                   bool                      refresh = true,
                   bool                      flipped = false) const override;

    csl::vector_expr
    breakSpace(const Space                      *brokenSpace,
               const std::vector<const Space *> &newSpaces,
               const std::vector<std::string>   &indexNames) const override;

    std::optional<Expr> getComplexConjugate() const override;

    bool compareWithDummy(Expr_info               other,
                          std::map<Index, Index> &constraints,
                          bool keepAllCosntraints = false) const override;

    bool operator==(Expr_info other) const override;

    bool operator<(const Abstract *expr) const override;

    Expr const &operator[](int i) const override;

    Expr &operator[](int i) override;

    friend Expr tderivativeelement_s(const Tensor &t_vector,
                                     const Parent &t_parent,
                                     const Index  &t_index)
    {
        return csl::make_shared<TDerivativeElement>(
            t_vector, t_parent, t_index);
    }

    friend Expr tderivativeelement_s(const Tensor &t_vector,
                                     const Parent &t_parent,
                                     const Index  &t_index,
                                     const Expr   &t_operand,
                                     bool          t_empty)
    {
        return csl::make_shared<TDerivativeElement>(
                   t_vector, t_parent, t_index)
            ->applyOperator(t_operand, t_empty);
    }
};

Expr tderivativeelement_s(const Tensor &t_vector,
                          const Parent &t_parent,
                          const Index  &t_index);

Expr tderivativeelement_s(const Tensor &t_vector,
                          const Parent &t_parent,
                          const Index  &t_index,
                          const Expr   &t_operand,
                          bool          t_empty);

class TDerivativeParent : public TensorFieldParent {

  public:
    TDerivativeParent();

    TDerivativeParent(const std::string &t_name, const Space *t_space);

    TDerivativeParent(const TDerivativeParent &other) = default;

    void printDefinition(std::ostream &out,
                         int           indentSize = 4,
                         bool          header     = false) const override;

    template <class... Args>
    friend TDerivative tderivative_s(Args &&...args);

    ~TDerivativeParent(){};

    Expr operator()(Index index, const Tensor &point) override;
};

template <class... Args>
std::shared_ptr<TDerivativeParent> tderivative_s(Args &&...args)
{
    return csl::make_shared<TDerivativeParent>(std::forward<Args>(args)...);
}

class TDerivative : public std::shared_ptr<TDerivativeParent> {

  public:
    INHERIT_SHARED_PTR_CONSTRUCTOR(TDerivative, TDerivativeParent)

    TDerivative(const std::string &t_name, const Space *t_space)
        : TDerivative(tderivative_s(t_name, t_space))
    {
    }

    template <class... Args>
    inline Expr operator()(Args &&...args)
    {
        return (**this)(std::forward<Args>(args)...);
    }
};

DEFINE_SHARED_PTR_OPERATOR(TDerivative)

} // End of namespace csl

#endif
