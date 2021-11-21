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

/*! \file vector.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief Objects handling vectors, matrices and higher-dimensional tensors.
 */
#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "abstract.h"
#include "literal.h"
#include "numerical.h"

namespace csl {

class AbstractVectorial: public Abstract{

    protected:

    int dim;

    int nArgs;

    csl::vector_expr argument;

    std::vector<int> shape;

    public:

    AbstractVectorial();

    ~AbstractVectorial(){};

    std::string const &getName() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int           mode = 0,
            std::ostream &out  = std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    std::vector<Parent> getSubSymbols() const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    int getDim() const override { return dim;}

    csl::PrimaryType getPrimaryType() const override{
        return csl::PrimaryType::Vectorial;
    }

    int getNArgs(int axis=0) const override;

    size_t size() const override;

    csl::vector_expr::iterator begin() override;
    csl::vector_expr::const_iterator begin() const override
    {
        return argument.begin();
    }

    csl::vector_expr::iterator end() override;
    csl::vector_expr::const_iterator end() const override
    {
        return argument.end();
    }

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getComplexConjugate() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    Expr const &getArgument(int iArg=0) const override;

    Expr& getArgument(int iArg=0) override;

    Expr& getArgument(const std::vector<int>& indices) override;

    Expr const &getArgument(const std::vector<int>& indices) const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    bool dependsOn(Parent_info expr) const override;

    bool dependsExplicitlyOn(Parent_info expr) const override;

    bool commutesWith(Expr_info expr, int sign=-1) const override;

    const csl::vector_expr& getVectorArgument() const override;

    std::vector<int> getShape() const override;

    void setComplexProperty(csl::ComplexProperty t_prop) override;

    void setArgument(const Expr& expr, int iArg=0) override;

    void setArgument(const Expr& expr,
                     const std::vector<int>& indices) override;

    void setVectorArgument(const csl::vector_expr& t_argument) override;

    bool matchShape(Expr_info expr, bool exact=false) const override;

    bool exactMatchShape(Expr_info expr) const;

    Expr getSum() const override;

    Expr getProduct() const override;

    Expr getSubVectorial(const std::vector<int>& exceptions) const override;

    Expr getSubVectorial(
            const std::vector<std::vector<int>>& keepIndices) const override;

    Expr getSubVectorial(
            std::vector<std::vector<int>>::const_iterator begin,
            std::vector<std::vector<int>>::const_iterator end) const override;

    Expr getVectorialModulus() const override;

    Expr dot(const Expr& expr) const override;

    Expr addition_own(const Expr& expr) const override;

    Expr multiplication_own(const Expr& expr, bool side=1) const override;

    Expr tensordot(const Expr& expr) const override;

    Expr trace(int axis1, int axis2) const override;

    Expr symmetrise() const override;

    Expr antisymmetrise() const override;

    std::optional<Expr> expand(bool full = false,
                               bool inplace = false) const override;

    std::optional<Expr> expand_if(
            std::function<bool(Expr const&)> const& f,
            bool full = false,
            bool inplace = false) const override;

    std::optional<Expr> factor(bool full = false) const override;

    std::optional<Expr> factor(Expr_info expr,
                               bool      full = false) const override;

    bool operator==(Expr_info expr) const override;

    Expr& operator[](int iArg) override;

    Expr const &operator[](int iArg) const override;
};

class Vector: public AbstractVectorial{

    public:

    Vector();

    explicit Vector(int t_nElements);

    Vector(int t_nElements, const Expr& expr, const Expr& index=CSL_0);

    explicit Vector(const csl::vector_expr& t_argument);

    ~Vector(){};

    csl::Type getType() const override { return csl::Type::Vector;}

    using AbstractVectorial::getSubVectorial;
    Expr getSubVectorial(int iExcept) const override;

    void setElementary(bool t_elementary) override;

    bool operator<(const Abstract* expr) const override;
};

Expr vector_s(int t_nElements);

Expr vector_s(int t_nElements,const Expr& expr);

Expr vector_s(int t_nElements,const Expr& expr, const Expr& index);

Expr vector_s(const csl::vector_expr& t_argument);

Expr vector_s(const std::initializer_list<Expr>& t_argument);

class Matrix: public AbstractVectorial{

    public:

    Matrix();

    explicit Matrix(int t_nArgs);

    Matrix(int t_x_nArgs, int t_y_nArgs);

    Matrix(int t_x_nArgs, int t_y_nArgs,
           const Expr& expr, 
           const Expr& index_x=CSL_0, const Expr& index_y=CSL_0);

    explicit Matrix(const csl::vector_expr& t_argument);

    explicit Matrix(const std::vector<csl::vector_expr>& t_argument);

    ~Matrix(){};

    csl::Type getType() const override {
       return csl::Type::Matrix;
    }

    Expr determinant() const override;

    using AbstractVectorial::trace;
    Expr trace() const override;

    Expr transpose() const override;

    Expr transposedCoMatrix() const;

    Expr hermitian() const override;

    Expr inverseMatrix() const override;

    Expr symmetrise() const override;

    Expr antisymmetrise() const override;

    using AbstractVectorial::getSubVectorial;
    Expr getSubVectorial(int iExcept, int jExcept) const override;

    bool operator<(const Abstract* expr) const override;
};

Expr matrix_s(int t_nArgs);

Expr matrix_s(int t_x_nArgs, int t_y_nArgs);

Expr matrix_s(int t_x_nArgs, int t_y_nArgs,
             const Expr& expr,
             const Expr& index_x, const Expr& index_y);

Expr matrix_s(int t_x_nArgs, int t_y_nArgs,
             const Expr& expr);

Expr matrix_s(const csl::vector_expr& t_argument);

Expr matrix_s(const std::initializer_list<std::initializer_list<Expr> >&
        t_matrix);

Expr diagonal_s(const csl::vector_expr& diag);

Expr identity_s(int dim);

class HighDTensor: public AbstractVectorial{

    public:

    HighDTensor();

    explicit HighDTensor(const std::vector<int>& shape);

    explicit HighDTensor(const std::vector<int>& shape,
            const Expr& filler);

    explicit HighDTensor(const csl::vector_expr& t_argument);

    ~HighDTensor(){};

    csl::Type getType() const override { return csl::Type::HighDTensor;}

    bool operator<(const Abstract* expr) const override;
};

Expr highdtensor_s(const std::vector<int>& shape);

Expr highdtensor_s(const std::vector<int>& shape, const Expr& filler);

Expr highdtensor_s( const std::initializer_list<std::initializer_list<
        std::initializer_list<Expr> > >& t_tensor);

Expr highdtensor_from_args_s(const std::vector<Expr>& args);

Expr vectorialtensor_s(const std::vector<int>& shape);

Expr vectorialtensor_s(const std::vector<int>& shape, const Expr& filler);
} // End of namespace csl

#endif
