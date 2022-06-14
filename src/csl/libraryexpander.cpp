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

#include "libraryexpander.h"
#include "error.h"
#include "algo.h"
#include "utils.h"
#include "interface.h"
#include "space.h"

namespace csl {

void LibraryExpander::apply(
        Expr             &expr,
        csl::Space const *space
        )
{
    csl::Transform(expr, [&](Expr &el)
    {
        if (el->getType() != csl::Type::Prod or not el->isIndexed())
            return false;
        bool sum = false;
        for (const auto& arg : *el)
            if (arg->getType() == csl::Type::Sum) {
                sum = true;
                break;
            }
        if (not sum)
            return false;
        return LibraryExpander::expand(el, space);
    });
}

bool LibraryExpander::toExpand(
        Expr const       &term,
        csl::Space const *space
        )
{
    csl::Tensor g = space->getMetric();
    csl::Tensor del = space->getDelta();
    if (term->getType() != csl::Type::Sum or !term->isIndexed())
        return IsIndicialTensor(term)
            and (term->getParent() == g or term->getParent() == del);
    return csl::AllOfLeafs(term, [&](Expr const &el)
    {
        return (!el->isIndexed()
                or el->getParent() == g
                or el->getParent() == del);
    });
}

Structure LibraryExpander::parseTerm(
        Expr       const &term,
        csl::Space const *space
        )
{
    Structure s;
    csl::Tensor g = space->getMetric();
    csl::Tensor del = space->getDelta();

    auto apply = [&](Expr const &el)
    {
        if (not el->isIndexed()) {
            s.mult(el);
            return;
        }
        csl::Parent parent = el->getParent();
        CSL_ASSERT_SPEC(parent == g or parent == del,
                CSLError::RuntimeError,
                toString(el)
                + " encoutered, should have only metric or delta.");
        csl::IndexStructure const &indices = el->getIndexStructureView();
        s.mult({indices[0], indices[1]});
    };

    if (term->size() > 0)
        for (const auto& el : *term) {
            apply(el);
        }
    else
        apply(term);

    return s;
}

std::vector<Structure> LibraryExpander::parse(
        Expr       const &isum,
        csl::Space const *space
        )
{
    if (IsIndicialTensor(isum)) {
        // Special case of metric or delta
        csl::Parent parent = isum->getParent();
        CSL_ASSERT_SPEC(parent == space->getMetric()
                or parent == space->getDelta(),
                CSLError::TypeError,
                "g or delta expected, " + toString(isum) + " given.");
        return {parseTerm(isum, space)};
    }
    CSL_ASSERT_SPEC(isum->getType() == csl::Type::Sum
            and isum->isIndexed(),
            CSLError::TypeError,
            "ISum expected, " + toString(isum) + " given.");
    std::vector<Structure> structures;
    structures.reserve(isum->size());
    for (const auto& term : *isum)
        structures.push_back(parseTerm(term, space));

    return structures;
}

size_t LibraryExpander::nCommonIndices(
        Structure const &s1,
        Structure const &s2
        )
{
    size_t nCommon = 0;
    for (const auto& p1 : s1.indices)
        for (const auto& p2 : s2.indices) {
            nCommon += (p1.getFirst() == p2.getFirst());
            nCommon += (p1.getFirst() == p2.getSecond());
            nCommon += (p1.getSecond() == p2.getFirst());
            nCommon += (p1.getSecond() == p2.getSecond());
        }

    return nCommon;
}

std::ostream &operator<<(
        std::ostream    &out,
        Structure const &s
        )
{
    if (s.factor != CSL_1)
        out << s.factor << ".";
    for (const auto& p : s.indices)
        out << "(" << p.getFirst() << ", " << p.getSecond() << ") ";
    return out;
}

void LibraryExpander::merge(
        std::vector<Structure>       &A,
        std::vector<Structure> const &B
        )
{
    std::vector<Structure> res;
    res.reserve(A.size() * B.size());
    for (const auto& a : A)
        for (const auto& b : B) {
            auto c = a * b;
            bool found = false;
            for (auto iter = res.begin(); iter != res.end(); ++iter)
                if (c == *iter) {
                    iter->add(c.factor);
                    if (iter->isZero())
                        res.erase(iter);
                    found = true;
                    break;
                }
            if (!found)
                res.push_back(c);
        }

    A = std::move(res);
}

std::vector<Structure> LibraryExpander::merge(
        std::vector<std::vector<Structure>> &init,
        csl::Space                    const *
        )
{
    if (init.empty())
        return {};
    if (init.size() == 1)
        return init[0];

    std::vector<size_t> indicesLeft(init.size()-1);
    for (size_t i = 0; i != indicesLeft.size(); ++i) 
        indicesLeft[i] = i+1;

    std::vector<Structure> res = std::move(init[0]);
    while (!indicesLeft.empty()) {
        size_t maxCommon = 0;
        size_t iMaxCommon = indicesLeft[0];
        if (!res.empty())
            for (size_t i : indicesLeft)
                if (!init[i].empty())
                    if (size_t n = nCommonIndices(res[0], init[i][0]);
                            n > maxCommon) {
                        maxCommon = n;
                        iMaxCommon = i;
                    }
        merge(res, init[iMaxCommon]);
        for (auto iter = indicesLeft.begin(); iter != indicesLeft.end(); ++iter)
            if (*iter == iMaxCommon) {
                indicesLeft.erase(iter);
                break;
            }
    }
    return res;
}

Expr LibraryExpander::toExpr(
        Structure  const &init,
        csl::Space const *space
        )
{
    csl::Tensor g = space->getMetric();
    std::vector<Expr> args;
    args.reserve(init.indices.size() + 1);
    args.push_back(init.factor);
    for (const auto& p : init.indices)
        args.push_back(g({p.getFirst(), p.getSecond()}));

    return prod_s(args);
}

Expr LibraryExpander::toExpr(
        std::vector<Structure> const &init,
        csl::Space             const *space
        )
{
    std::vector<Expr> res;
    res.reserve(init.size());
    for (const auto& s : init)
        res.push_back(toExpr(s, space));

    return sum_s(res);
}

bool LibraryExpander::expand(
        Expr             &iprod,
        csl::Space const *space
        )
{
    CSL_ASSERT_SPEC(iprod->getType() == csl::Type::Prod
            and iprod->isIndexed(),
            CSLError::TypeError,
            "IProd expected, " + toString(iprod) + " given.");

    std::vector<std::vector<Structure>> structures;
    structures.reserve(iprod->size());
    for (auto& arg : *iprod)
        if (toExpand(arg, space)) {
            structures.emplace_back(parse(arg, space));
            arg = CSL_1;
        }
    if (structures.empty())
        return false;
    if (structures.size() == 1) {
        iprod = Expanded(toExpr(structures[0], space) * iprod);
        return false;
    }
    
    std::vector<Structure> res = merge(structures, space);

    iprod = Refreshed(iprod) * toExpr(res, space);
    return true;
}

Structure::Structure()
    :factor(CSL_1)
{
    indices.reserve(reserveStorage);
}

Structure::Structure(Structure const &other)
    :Structure()
{
    factor = csl::Copy(other.factor);
    for (const auto& p : other.indices)
        indices.push_back(p);
}

Structure& Structure::operator=(Structure const &other)
{
    factor = csl::Copy(other.factor);
    for (const auto& p : other.indices)
        indices.push_back(p);

    return *this;
}

void Structure::add(Expr const &t_factor)
{
    factor = factor + t_factor;
}

void Structure::mult(Expr const &t_factor)
{
    factor = factor * t_factor;
}

void Structure::mult(ipair const &p)
{
    mult(p, nullptr);
}

void Structure::mult(
        ipair const &p,
        Expr  const &t_factor
        )
{
    for (size_t i = 0; i != indices.size(); ++i) {
        if (int res = commonIndex(indices[i], p); res != -1) {
            switch(res) {
                case 0:
                    indices[i].setFirst(p.getSecond());
                    break;
                case 1:
                    indices[i].setFirst(p.getFirst());
                    break;
                case 2:
                    indices[i].setSecond(p.getSecond());
                    break;
                case 3:
                    indices[i].setSecond(p.getFirst());
                    break;
            }
            if (indices[i].getFirst() == indices[i].getSecond()) {
                auto space = indices[i].getFirst().getSpace();
                Expr trace = space->getMetric()->getTrace();
                factor = factor * trace;
                indices.erase(indices.begin() + i);
            }
            if (t_factor)
                factor = factor * t_factor;
            simplify();
            return;
        }
    }
    if (t_factor)
        factor = factor * t_factor;
    for (size_t i = 0; i != indices.size(); ++i) 
        if (p < indices[i]) {
            indices.insert(indices.begin() + i, p);
            return;
        }
    simplify();
    indices.push_back(p);
}

void Structure::simplify()
{
    for (size_t i = 0; i != indices.size(); ++i) {
        for (size_t j = i + 1; j < indices.size(); ++j) 
            if (int res = commonIndex(indices[i], indices[j]); res != -1) {
                switch(res) {
                    case 0:
                        indices[i].setFirst(indices[j].getSecond());
                        break;
                    case 1:
                        indices[i].setFirst(indices[j].getFirst());
                        break;
                    case 2:
                        indices[i].setSecond(indices[j].getSecond());
                        break;
                    case 3:
                        indices[i].setSecond(indices[j].getFirst());
                        break;
                }
                indices.erase(indices.begin() + j);
                --j;
                if (indices[i].getFirst() == indices[i].getSecond()) {
                    auto space = indices[i].getFirst().getSpace();
                    Expr trace = space->getMetric()->getTrace();
                    factor = factor * trace;
                    indices.erase(indices.begin() + i);
                }
            }
    }
}

Structure& Structure::operator*=(Structure const &other)
{
    for (const auto& p : other.indices)
        mult(p);
    factor = factor * other.factor;
    return *this;
}

Structure Structure::operator*(Structure const &other) const
{
    Structure copy(*this);
    return (copy *= other);
}

bool Structure::operator==(Structure const &other) const
{
    if (indices.size() != other.indices.size())
        return false;
    for (size_t i = 0; i != indices.size(); ++i) 
        if (indices[i] != other.indices[i])
            return false;
    return true;
}

int Structure::commonIndex(
        ipair const &p1,
        ipair const &p2
        )
{
    if (p1.getFirst() == p2.getFirst())
        return 0;
    if (p1.getFirst() == p2.getSecond())
        return 1;
    if (p1.getSecond() == p2.getFirst())
        return 2;
    if (p1.getSecond() == p2.getSecond())
        return 3;
    return -1;
}

}
