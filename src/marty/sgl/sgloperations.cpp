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

#include "sgloperations.h"
#include "cslexpr.h"
#include "debuglog.h"
#include "tensorset.h"
#include "typecast.h"

namespace sgl {

bool Sum::isZero() const
{
    return std::all_of(m_argument.begin(),
                       m_argument.end(),
                       [](GExpr const &arg) { return arg->isZero(); });
}

GExpr Sum::copy() const
{
    return std::make_shared<Sum>(m_argument);
}

GExpr Sum::refresh() const
{
    return sum_s(m_argument);
}

csl::Expr Sum::toCSL(TensorSet const &tensors) const
{
    std::vector<csl::Expr> args;
    args.reserve(size());
    for (const auto &arg : m_argument)
        args.push_back(arg->toCSL(tensors));
    return csl::sum_s(args);
}

bool Sum::contains(csl::Index const &index) const
{
    return std::any_of(m_argument.begin(),
                       m_argument.end(),
                       [&](GExpr const &arg) { return arg->contains(index); });
}

void Sum::replace(csl::Index const &i, csl::Index const &j)
{
    std::for_each(m_argument.begin(), m_argument.end(), [&](GExpr &arg) {
        arg = arg->copy();
        arg->replace(i, j);
    });
}

void Sum::merge()
{
    std::vector<GExpr> newArgs;
    newArgs.reserve(m_argument.size());
    for (const auto &arg : m_argument) {
        if (IsType<Sum>(arg)) {
            for (const auto &subarg : *arg) {
                newArgs.push_back(subarg);
            }
        }
        else {
            newArgs.push_back(arg);
        }
    }
    std::vector<csl::Expr> exprs;
    exprs.reserve(m_argument.size());
    for (size_t i = 0; i != newArgs.size(); ++i) {
        if (IsType<CSLExpr>(newArgs[i])) {
            exprs.push_back(newArgs[i]->expr());
            newArgs.erase(newArgs.begin() + i);
            --i;
        }
    }
    csl::Expr res = csl::sum_s(exprs);
    if (res != CSL_0) {
        newArgs.insert(newArgs.begin(), cslexpr_s(res));
    }
    m_argument = std::move(newArgs);
}

void Sum::print(std::ostream &out) const
{
    for (size_t i = 0; i != m_argument.size(); ++i) {
        m_argument[i]->print(out);
        if (i < m_argument.size() - 1) {
            out << " + ";
        }
    }
}

bool Prod::isZero() const
{
    return std::any_of(m_argument.begin(),
                       m_argument.end(),
                       [](GExpr const &arg) { return arg->isZero(); });
}

csl::Expr Prod::getFactor() const
{
    if (m_argument.size() > 1 && IsType<CSLExpr>(m_argument[0]))
        return m_argument[0]->expr();
    return CSL_1;
}

GExpr Prod::getTerm() const
{
    if (getFactor() != CSL_1) {
        std::vector<GExpr> newArgs(m_argument.begin() + 1, m_argument.end());
        return sgl::prod_s(newArgs);
    }
    return copy();
}

GExpr Prod::copy() const
{
    return std::make_shared<Prod>(m_argument);
}

GExpr Prod::refresh() const
{
    return prod_s(m_argument);
}

csl::Expr Prod::toCSL(TensorSet const &tensors) const
{
    std::vector<csl::Expr> args;
    args.reserve(size());
    for (const auto &arg : m_argument)
        args.push_back(arg->toCSL(tensors));
    return csl::prod_s(args);
}

bool Prod::contains(csl::Index const &index) const
{
    return std::any_of(m_argument.begin(),
                       m_argument.end(),
                       [&](GExpr const &arg) { return arg->contains(index); });
}

void Prod::replace(csl::Index const &i, csl::Index const &j)
{
    for (auto &arg : m_argument)
        if (arg->contains(i)) {
            arg = arg->copy();
            arg->replace(i, j);
            return;
        }
}

void Prod::merge()
{
    std::vector<GExpr> newArgs;
    newArgs.reserve(m_argument.size());
    for (const auto &arg : m_argument) {
        if (IsType<Prod>(arg)) {
            for (const auto &subarg : *arg) {
                newArgs.push_back(subarg);
            }
        }
        else {
            newArgs.push_back(arg);
        }
    }
    std::vector<csl::Expr> exprs;
    exprs.reserve(m_argument.size());
    for (size_t i = 0; i != newArgs.size(); ++i) {
        if (IsType<CSLExpr>(newArgs[i])) {
            exprs.push_back(newArgs[i]->expr());
            newArgs.erase(newArgs.begin() + i);
            --i;
        }
    }
    for (size_t i = 0; i != newArgs.size(); ++i) {
        if (auto f = newArgs[i]->getFactor(); f != CSL_1) {
            exprs.push_back(f);
            newArgs[i] = newArgs[i]->getTerm();
            if (IsType<CSLExpr>(newArgs[i]) && newArgs[i]->expr() == CSL_1) {
                newArgs.erase(newArgs.begin() + i);
                --i;
            }
        }
    }
    csl::Expr res = csl::prod_s(exprs);
    if (res == CSL_0) {
        m_argument = {cslexpr_s(CSL_0)};
        return;
    }
    else if (res != CSL_1) {
        newArgs.insert(newArgs.begin(), cslexpr_s(res));
    }
    m_argument = std::move(newArgs);
}

void Prod::print(std::ostream &out) const
{
    for (size_t i = 0; i != m_argument.size(); ++i) {
        bool bracket = IsType<Sum>(m_argument[i]);
        if (bracket)
            out << '(';
        m_argument[i]->print(out);
        if (bracket)
            out << ')';
        if (i < m_argument.size() - 1) {
            out << '*';
        }
    }
}

void Prod::applyProperties()
{
    for (size_t i = 0; i != m_argument.size(); ++i) {
        for (size_t j = 0; j < m_argument.size(); ++j) {
            if (i != j && m_argument[i]->hasPropertyWith(m_argument[j])) {
                GExpr res     = m_argument[i]->propertyWith(m_argument[j]);
                m_argument[i] = res;
                m_argument.erase(m_argument.begin() + j);
                applyProperties();
                return;
            }
        }
    }
}

GExpr Prod::expand()
{
    SCOPELOG
    LOG("Expanding", copy())
    if (m_argument.size() == 1)
        return m_argument[0];
    for (size_t i = 0; i != m_argument.size(); ++i) {
        if (IsType<Sum>(m_argument[i])) {
            LOG("Sum found :", m_argument[i])
            std::vector<GExpr> terms(m_argument[i]->size());
            for (size_t k = 0; k != terms.size(); ++k) {
                std::vector<GExpr> args = m_argument;
                args[i]                 = m_argument[i]->argument(k);
                terms[k]                = sgl::prod_s(args);
                LOG("Term :", terms[k])
            }
            LOG("Res :", sgl::sum_s(terms))
            return sgl::sum_s(terms);
        }
    }
    return self();
}

GExpr sum_s(std::vector<GExpr> args)
{
    auto res = std::make_shared<Sum>(args);
    res->merge();
    if (res->size() == 1)
        return res->argument(0);
    else if (res->size() == 0)
        return cslexpr_s(CSL_0);
    return res;
}

GExpr prod_s(std::vector<GExpr> args)
{
    auto res = std::make_shared<Prod>(args);
    res->merge();
    if (res->size() == 1)
        return res->argument(0);
    else if (res->size() == 0)
        return cslexpr_s(CSL_1);
    res->applyProperties();
    res->merge();
    return res->expand();
}
} // namespace sgl
