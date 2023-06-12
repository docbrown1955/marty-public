/*!
 * \file binaryOperators.h
 * \brief Contains the csl::BinaryOperator class.
 */
#include "scalarFunc.h"
#include <functional>

namespace csl {

/*!
 * \brief Binary operator with a condition and one expression if the
 * condition is true, one otherwise.
 *
 * \details Does not support indexed expressions.
 */
class BooleanOperator : public AbstractMultiFunc {

  public:
    enum Type {
        True,                 /*!< Boolean unary operator if(A). */
        False,                /*!< Boolean unary operator if (!A). */
        EqualTo,              /*!< Boolean binary operator if (A == B). */
        DifferentThan,        /*!< Boolean binary operator if (A != B). */
        LessThan,             /*!< Boolean binary operator if (A < B). */
        LessThanOrEqualTo,    /*!< Boolean binary operator if (A <= B). */
        GreaterThan,          /*!< Boolean binary operator if (A > B). */
        GreaterThanOrEqualTo, /*!< Boolean binary operator if (A >= B). */
    };

    /*!
     * \brief Constructor for unary operator (Type::True or Type::False).
     */
    BooleanOperator(Type             type,
                    csl::Expr const &A,
                    csl::Expr const &B,
                    csl::Expr const &exprIfTrue,
                    csl::Expr const &exprIfFalse);

    /*!
     * \brief Constructor for binary operators.
     *
     * \details To use for Type::EqualTo, Type::DifferentThan,
     * Type::LessThan, Type::LessThanOrEqualTo, Type::GreaterThan and
     * Type::GreaterThanOrEqualTo.
     */
    BooleanOperator(Type             type,
                    csl::Expr const &A,
                    csl::Expr const &exprIfTrue,
                    csl::Expr const &exprIfFalse);

    /*! \brief Gives the **type** of a Sum object.
     * \return 10
     */
    csl::Type getType() const override
    {
        return csl::Type::BooleanOperator;
    }

    virtual bool isIndexed() const override
    {
        return false;
    }

    bool isReal() const override;

    bool isPurelyImaginary() const override;

    std::optional<Expr> getRealPart() const override;

    Expr getImaginaryPart() const override;

    std::optional<Expr> getComplexModulus() const override;

    std::optional<Expr> getComplexArgument() const override;

    void print(int           mode    = 0,
               std::ostream &out     = std::cout,
               LibraryMode   libMode = LibraryMode::NoLib) const override;

    void printCode(int mode = 0, std::ostream &out = std::cout) const override;

    void printLaTeX(int mode = 0, std::ostream &out = std::cout) const override;

    LibDependency getLibDependency() const override;

    std::optional<Expr> evaluate(csl::eval::mode user_mode
                                 = csl::eval::base) const override;

    unique_Expr copy_unique() const override;

    Expr deepCopy() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract *expr) const override;

  protected:
    void applyToArgs(std::function<void(csl::Expr &)> const &f);

  protected:
    BooleanOperator::Type type;
};

csl::Expr booleanOperator_s(BooleanOperator::Type type,
                            csl::Expr const      &A,
                            csl::Expr const      &B,
                            csl::Expr const      &ifTrue,
                            csl::Expr const      &ifFalse);

csl::Expr booleanOperator_s(BooleanOperator::Type type,
                            csl::Expr const      &A,
                            csl::Expr const      &ifTrue,
                            csl::Expr const      &ifFalse);

std::ostream &operator<<(std::ostream &out, BooleanOperator::Type type);

} // namespace csl
