#include "booleanOperators.h"
#include "cslcomplex.h"
#include "error.h"
#include "librarydependency.h"
#include <algorithm>

namespace csl {

BooleanOperator::BooleanOperator(Type             t_type,
                                 csl::Expr const &A,
                                 csl::Expr const &B,
                                 csl::Expr const &exprIfTrue,
                                 csl::Expr const &exprIfFalse)
    : AbstractMultiFunc({A, B, exprIfTrue, exprIfFalse}), type(t_type)
{
}

BooleanOperator::BooleanOperator(Type             t_type,
                                 csl::Expr const &A,
                                 csl::Expr const &exprIfTrue,
                                 csl::Expr const &exprIfFalse)
    : AbstractMultiFunc({A, exprIfTrue, exprIfFalse}), type(t_type)
{
}

bool BooleanOperator::isReal() const
{
    return std::all_of(argument.end() - 2,
                       argument.end(),
                       [](csl::Expr const &arg) { return arg->isReal(); });
}

bool BooleanOperator::isPurelyImaginary() const
{
    return std::all_of(argument.end() - 2,
                       argument.end(),
                       [](csl::Expr const &arg) { return arg->isReal(); });
}

void BooleanOperator::applyToArgs(std::function<void(csl::Expr &)> const &f)
{
    const size_t first = size() - 2;
    f(argument[first]);
    f(argument[first + 1]);
}

std::optional<csl::Expr> BooleanOperator::getRealPart() const
{
    if (isReal())
        return std::nullopt;
    auto cpy = *this;
    cpy.applyToArgs([](csl::Expr &arg) { arg = csl::real_s(arg); });
    return cpy.copy();
}

csl::Expr BooleanOperator::getImaginaryPart() const
{
    if (isReal())
        return CSL_0;
    auto cpy = *this;
    cpy.applyToArgs([](csl::Expr &arg) { arg = csl::imaginary_s(arg); });
    return cpy.copy();
}

std::optional<csl::Expr> BooleanOperator::getComplexModulus() const
{
    auto cpy = *this;
    cpy.applyToArgs(
        [](csl::Expr &arg) { arg = arg->getComplexModulus().value_or(arg); });
    return cpy.copy();
}

std::optional<csl::Expr> BooleanOperator::getComplexArgument() const
{
    auto cpy = *this;
    cpy.applyToArgs(
        [](csl::Expr &arg) { arg = arg->getComplexArgument().value_or(arg); });
    return cpy.copy();
}

void BooleanOperator::print(int           mode,
                            std::ostream &out,
                            LibraryMode   libMode) const
{
    const size_t first = size() - 2;

    auto print_one_arg = [&](csl::Expr const &expr) {
        out << "(";
        expr->print(0, out, libMode);
        out << ")";
    };
    auto print_two_args
        = [&](csl::Expr const &left, csl::Expr const &right, char const *sep) {
              out << "(";
              left->print(0, out, libMode);
              out << ") " << sep << " (";
              right->print(0, out, libMode);
              out << ")";
          };

    if (libMode != LibraryMode::NoLib) {
        out << "( (";
        switch (type) {
        case True:
            out << "(bool)";
            print_one_arg(argument[0]);
            break;
        case False:
            out << "!(bool)";
            print_one_arg(argument[0]);
            break;
        case EqualTo:
            print_two_args(argument[0], argument[1], "==");
            break;
        case DifferentThan:
            print_two_args(argument[0], argument[1], "!=");
            break;
        case LessThan:
            print_two_args(argument[0], argument[1], "<");
            break;
        case LessThanOrEqualTo:
            print_two_args(argument[0], argument[1], "<=");
            break;
        case GreaterThan:
            print_two_args(argument[0], argument[1], ">");
            break;
        case GreaterThanOrEqualTo:
            print_two_args(argument[0], argument[1], ">=");
            break;
        default:
            CALL_SMERROR(CSLError::TypeError);
        }
        out << ") ? ";
        print_two_args(argument[first], argument[first + 1], ":");
        out << ")";
    }
    else {
        out << "Boolean(";
        out << "condition: ";
        switch (type) {
        case True:
            out << "[" << argument[0] << "]";
            break;
        case False:
            out << "not [" << argument[0] << "]";
            break;
        case EqualTo:
            out << "[" << argument[0] << "] == [" << argument[1] << "]";
            break;
        case DifferentThan:
            out << "[" << argument[0] << "] != [" << argument[1] << "]";
            break;
        case LessThan:
            out << "[" << argument[0] << "] < [" << argument[1] << "]";
            break;
        case LessThanOrEqualTo:
            out << "[" << argument[0] << "] <= [" << argument[1] << "]";
            break;
        case GreaterThan:
            out << "[" << argument[0] << "] > [" << argument[1] << "]";
            break;
        case GreaterThanOrEqualTo:
            out << "[" << argument[0] << "] >= [" << argument[1] << "]";
            break;
        default:
            CALL_SMERROR(CSLError::TypeError);
        }
        out << " ; true: [" << argument[first] << "]";
        out << " ; false: [" << argument[first + 1] << "]";
        out << ")";
    }
    if (mode == 0) {
        out << '\n';
    }
}

void BooleanOperator::printCode(int mode, std::ostream &out) const
{
    out << "csl::booleanoperator_s(";
    out << type << ", ";
    argument[0]->printCode(1, out);
    out << ", ";
    argument[1]->printCode(1, out);
    out << ", ";
    argument[2]->printCode(1, out);
    if (size() == 4) {
        out << ", ";
        argument[3]->printCode(1, out);
    }
    out << ")";
    if (mode == 0)
        out << '\n';
}

std::string BooleanOperator::printLaTeX(int mode) const
{
    std::ostringstream sout;
    print(mode, sout);
    return sout.str();
}

LibDependency BooleanOperator::getLibDependency() const
{
    LibDependency dep;
    dep.addInclude("stdbool.h", "stdbool.h");
    return dep;
}

std::optional<Expr> BooleanOperator::evaluate(csl::eval::mode user_mode) const
{
    bool evaluated = false;
    auto copy      = copy_unique();
    for (auto &arg : *copy) {
        auto eval = arg->evaluate(user_mode);
        if (eval) {
            evaluated = true;
            arg       = eval.value();
        }
    }
    if (evaluated)
        return std::shared_ptr<csl::Abstract>(std::move(copy));
    return std::nullopt;
}

unique_Expr BooleanOperator::copy_unique() const
{
    if (type == BooleanOperator::True || type == BooleanOperator::False) {
        return std::make_unique<BooleanOperator>(
            type, argument[0], argument[1], argument[2]);
    }
    return std::make_unique<BooleanOperator>(
        type, argument[0], argument[1], argument[2], argument[3]);
}

Expr BooleanOperator::deepCopy() const
{
    auto cpy = copy_unique();
    for (auto &arg : *cpy) {
        arg = arg->deepCopy();
    }
    return cpy;
}

Expr BooleanOperator::refresh() const
{
    if (type == BooleanOperator::True || type == BooleanOperator::False) {
        return booleanOperator_s(type, argument[0], argument[1], argument[2]);
    }
    return booleanOperator_s(
        type, argument[0], argument[1], argument[2], argument[3]);
}

Expr BooleanOperator::deepRefresh() const
{
    auto cpy = copy_unique();
    for (auto &arg : *cpy) {
        arg = arg->deepRefresh();
    }
    return cpy->refresh();
}

bool BooleanOperator::operator==(Expr_info expr) const
{
    if (expr->getType() != csl::Type::BooleanOperator) {
        return false;
    }
    auto boolean = dynamic_cast<BooleanOperator const *>(expr);
    if (boolean->type != type || size() != boolean->size()) {
        return false;
    }
    for (size_t i = 0; i != size(); ++i) {
        if (argument[i] != (*boolean)[i]) {
            return false;
        }
    }
    return true;
}

csl::Expr booleanOperator_s(BooleanOperator::Type type,
                            csl::Expr const      &A,
                            csl::Expr const      &B,
                            csl::Expr const      &ifTrue,
                            csl::Expr const      &ifFalse)
{
    if (A == B) {
        if (type == BooleanOperator::EqualTo
            || type == BooleanOperator::LessThanOrEqualTo
            || type == BooleanOperator::GreaterThanOrEqualTo) {
            return ifTrue;
        }
        else if (type == BooleanOperator::DifferentThan
                 || type == BooleanOperator::LessThan
                 || type == BooleanOperator::GreaterThan) {
            return ifFalse;
        }
    }
    return std::make_shared<BooleanOperator>(type, A, B, ifTrue, ifFalse);
}

csl::Expr booleanOperator_s(BooleanOperator::Type type,
                            csl::Expr const      &A,
                            csl::Expr const      &ifTrue,
                            csl::Expr const      &ifFalse)
{
    if (A == CSL_1 && type == BooleanOperator::True) {
        return ifTrue;
    }
    else if (csl::IsNumerical(A) && type == BooleanOperator::False) {
        return ifFalse;
    }
    return std::make_shared<BooleanOperator>(type, A, ifTrue, ifFalse);
}

std::ostream &operator<<(std::ostream &out, BooleanOperator::Type type)
{
    switch (type) {
    case BooleanOperator::True:
        out << "True";
        break;
    case BooleanOperator::False:
        out << "False";
        break;
    case BooleanOperator::EqualTo:
        out << "EqualTo";
        break;
    case BooleanOperator::DifferentThan:
        out << "DifferentThan";
        break;
    case BooleanOperator::LessThan:
        out << "LessThan";
        break;
    case BooleanOperator::LessThanOrEqualTo:
        out << "LessThanOrEqualTo";
        break;
    case BooleanOperator::GreaterThan:
        out << "GreaterThan";
        break;
    case BooleanOperator::GreaterThanOrEqualTo:
        out << "GreaterThanOrEqualTo";
        break;
    default:
        CALL_SMERROR(CSLError::TypeError);
    }
    return out;
}

} // namespace csl
