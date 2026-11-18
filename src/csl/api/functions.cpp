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

#include "functions.h"

#include "../abstract.h"
#include "../mathFunctions.h"

namespace csl::api {

namespace {

/// Applies a CSL unary function to a handle, translating CSL errors.
template <class Function>
ExprHandle
applyUnary(std::string_view name, ExprHandle const &e, Function function)
{
    return detail::guarded(name,
                           [&] { return ExprHandle(function(e.raw())); });
}

} // namespace

ExprHandle Sqrt(ExprHandle const &e)
{
    return applyUnary("sqrt", e, [](Expr const &x) { return sqrt_s(x); });
}

ExprHandle Abs(ExprHandle const &e)
{
    return applyUnary("abs", e, [](Expr const &x) { return abs_s(x); });
}

ExprHandle Exp(ExprHandle const &e)
{
    return applyUnary("exp", e, [](Expr const &x) { return exp_s(x); });
}

ExprHandle Log(ExprHandle const &e)
{
    return applyUnary("log", e, [](Expr const &x) { return log_s(x); });
}

ExprHandle Sin(ExprHandle const &e)
{
    return applyUnary("sin", e, [](Expr const &x) { return sin_s(x); });
}

ExprHandle Cos(ExprHandle const &e)
{
    return applyUnary("cos", e, [](Expr const &x) { return cos_s(x); });
}

ExprHandle Tan(ExprHandle const &e)
{
    return applyUnary("tan", e, [](Expr const &x) { return tan_s(x); });
}

ExprHandle Asin(ExprHandle const &e)
{
    return applyUnary("asin", e, [](Expr const &x) { return asin_s(x); });
}

ExprHandle Acos(ExprHandle const &e)
{
    return applyUnary("acos", e, [](Expr const &x) { return acos_s(x); });
}

ExprHandle Atan(ExprHandle const &e)
{
    return applyUnary("atan", e, [](Expr const &x) { return atan_s(x); });
}

ExprHandle Sinh(ExprHandle const &e)
{
    return applyUnary("sinh", e, [](Expr const &x) { return sinh_s(x); });
}

ExprHandle Cosh(ExprHandle const &e)
{
    return applyUnary("cosh", e, [](Expr const &x) { return cosh_s(x); });
}

ExprHandle Tanh(ExprHandle const &e)
{
    return applyUnary("tanh", e, [](Expr const &x) { return tanh_s(x); });
}

ExprHandle Asinh(ExprHandle const &e)
{
    return applyUnary("asinh", e, [](Expr const &x) { return asinh_s(x); });
}

ExprHandle Acosh(ExprHandle const &e)
{
    return applyUnary("acosh", e, [](Expr const &x) { return acosh_s(x); });
}

ExprHandle Atanh(ExprHandle const &e)
{
    return applyUnary("atanh", e, [](Expr const &x) { return atanh_s(x); });
}

} // namespace csl::api
