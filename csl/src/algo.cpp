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

#include "algo.h"
#include "utils.h"
#include "indicial.h"

namespace csl {

Expr FindLeaf(Expr const& init,
              Expr const& value,
              size_t      depth)
{
    if (init->size() == 0) {
        if (*init == value.get())
            return init;
        return nullptr;
    }
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (Expr expr = FindLeaf((*init)[i], value, depth-1);
                    expr != nullptr)
                return expr;
    }
    return nullptr;
}

Expr FindNode(Expr const& init,
              Expr const& value,
              size_t      depth)
{
    if (*init == value)
        return init;
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (Expr expr = FindNode((*init)[i], value, depth-1);
                    expr != nullptr)
                return expr;
    }
    return nullptr;
}

Expr FindIfLeaf(Expr const&                             init,
                std::function<bool(Expr const&)> const& f,
                size_t                                  depth)
{
    if (init->size() == 0) {
        if (f(init))
            return init;
        return nullptr;
    }
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (Expr expr = FindIfLeaf((*init)[i], f, depth-1);
                    expr != nullptr)
                return expr;
    }
    return nullptr;
}

Expr FindIfNode(Expr                             const& init,
                std::function<bool(Expr const&)> const& f,
                size_t                                  depth)
{
    if (f(init))
        return init;
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (Expr expr = FindIfNode((*init)[i], f, depth-1);
                    expr != nullptr)
                return expr;
    }
    return nullptr;
}

bool AnyOfLeafs(Expr_info                             init,
                std::function<bool(Expr_info)> const& f,
                int                                   depth)
{
    if (init->size() == 0)
        return f(init);
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (AnyOfLeafs((*init)[i].get(), f, depth-1))
                return true;
    }
    return false;
}

bool AnyOfLeafs(Expr                             const& init,
                std::function<bool(Expr const&)> const& f,
                int                                     depth)
{
    if (init->size() == 0)
        return f(init);
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (AnyOfLeafs((*init)[i], f, depth-1))
                return true;
    }
    return false;
}

bool AllOfLeafs(Expr_info                             init,
                std::function<bool(Expr_info)> const& f,
                int                                   depth)
{
    if (init->size() == 0)
        return f(init);
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (not AllOfLeafs((*init)[i].get(), f, depth-1))
                return false;
    }
    return true;
}

bool AllOfLeafs(Expr                             const& init,
                std::function<bool(Expr const&)> const& f,
                int                                     depth)
{
    if (init->size() == 0)
        return f(init);
    else if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (not AllOfLeafs((*init)[i], f, depth-1))
                return false;
    }
    return true;
}

bool AnyOfNodes(Expr_info                             init,
                std::function<bool(Expr_info)> const& f,
                int                                   depth)
{
    if (f(init))
        return true;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (AnyOfNodes((*init)[i].get(), f, depth-1))
                return true;
    }
    return false;
}

bool AnyOfNodes(Expr                             const& init,
                std::function<bool(Expr const&)> const& f,
                int                                     depth)
{
    if (f(init))
        return true;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (AnyOfNodes((*init)[i], f, depth-1))
                return true;
    }
    return false;
}

bool AllOfNodes(Expr                             const& init,
                std::function<bool(Expr const&)> const& f,
                int                                     depth)
{
    if (not f(init))
        return false;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (not AllOfNodes((*init)[i], f, depth-1))
                return false;
    }
    return true;
}

bool AllOfNodes(Expr_info                             init,
                std::function<bool(Expr_info)> const& f,
                int                                   depth)
{
    if (not f(init))
        return false;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (not AllOfNodes((*init)[i].get(), f, depth-1))
                return false;
    }
    return true;
}

void VisitEachNode(Expr                             const& init,
                   std::function<void(Expr const&)> const& f,
                   int                                     depth)
{
    f(init);
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachNode((*init)[i], f, depth-1);
    }
}
void VisitEachNodeReversed(Expr                             const& init,
                   std::function<void(Expr const&)> const& f,
                   int                                     depth)
{
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachNodeReversed((*init)[i], f, depth-1);
    }
    f(init);
}

void VisitEachNode(Expr_info                             init,
                   std::function<void(Expr_info)> const& f,
                   int                                   depth)
{
    f(init);
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachNode((*init)[i].get(), f, depth-1);
    }
}

void VisitEachLeaf(Expr                             const& init,
                   std::function<void(Expr const&)> const& f,
                   int                                     depth)
{
    if (init->size() == 0)
        f(init);
    else if (depth != 0){
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachLeaf((*init)[i], f, depth-1);
    }
}

void VisitEachLeaf(Expr_info                             init,
                   std::function<void(Expr_info)> const& f,
                   int                                   depth)
{
    f(init);
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachLeaf((*init)[i].get(), f, depth-1);
    }
}

void VisitEachNodeCut(
        Expr                             const& init,
        std::function<bool(Expr const&)> const& f,
        int                                     depth
        )
{
    if (f(init))
        return;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachNode((*init)[i], f, depth-1);
    }
}

void VisitEachNodeCut(
        Expr_info                             init,
        std::function<bool(Expr_info)> const& f,
        int                                   depth
        )
{
    if (f(init))
        return;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            VisitEachNode((*init)[i].get(), f, depth-1);
    }
}

bool isUnique(csl::Expr const &expr)
{
    return csl::AllOfNodes(expr, [&](csl::Expr const &sub) {
        return sub.use_count() == 1;
    });
}

size_t Count(csl::Expr const &expr, csl::Expr const &search)
{
    size_t count = 0;
    csl::VisitEachNode(expr, [&](csl::Expr const &sub) {
        if (sub == search) 
            ++count;
    });
    return count;
}

size_t CountIf(
        csl::Expr const &expr, 
        std::function<bool(csl::Expr const&)> const &f
        )
{
    size_t count = 0;
    csl::VisitEachNode(expr, [&](csl::Expr const &sub) {
        if (f(sub))
            ++count;
    });
    return count;
}

size_t CountNodes(Expr const &init)
{
    size_t count = 0;
    csl::VisitEachNode(init,
    [&count](Expr const &)
    {
        ++count;
    });

    return count;
}
size_t CountLeafs(Expr const &init)
{
    size_t count = 0;
    csl::VisitEachLeaf(init,
    [&count](Expr const &)
    {
        ++count;
    });

    return count;
}

void ForEachNode(Expr                            & init,
                 std::function<void(Expr&)> const& f,
                 int                               depth)
{
    f(init);
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            ForEachNode((*init)[i], f, depth-1);
    }
}

void ForEachNodeReversed(Expr                            & init,
                 std::function<void(Expr&)> const& f,
                 int                               depth)
{
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            ForEachNodeReversed((*init)[i], f, depth-1);
    }
    f(init);
}

void ForEachNodeCut(Expr                            & init,
                     std::function<bool(Expr&)> const& f,
                     int                               depth)
{
    if (f(init))
        return;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            ForEachNodeCut((*init)[i], f, depth-1);
    }
}

void ForEachLeaf(Expr                            & init,
                 std::function<void(Expr&)> const& f,
                 int                               depth)
{
    if (init->size() == 0)
        f(init);
    else if (depth != 0){
        for (size_t i = 0; i != init->size(); ++i)
            ForEachLeaf((*init)[i], f, depth-1);
    }
}

void ForEachNode(std::vector<Expr>                 init,
                 std::function<void(Expr&)> const& f,
                 int                               depth)
{
    for (auto iter = init.begin(); iter != init.end(); ++iter) {
        ForEachNode(*iter, f, depth);
    }
}

void ForEachLeaf(std::vector<Expr>                 init,
                 std::function<void(Expr&)> const& f,
                 int                               depth)
{
    for (auto iter = init.begin(); iter != init.end(); ++iter) {
        ForEachLeaf(*iter, f, depth);
    }
}

bool FirstOfNode(Expr                              & init,
                 std::function<bool(Expr&)> const& f)
{
    if (f(init))
        return true;
    for (size_t i = 0; i != init->size(); ++i)
        if (FirstOfNode((*init)[i], f))
            return true;
    return false;
}

bool FirstOfLeaf(Expr                            & init,
                 std::function<bool(Expr&)> const& f)
{
    if (init->size() == 0)
        return f(init);
    for (size_t i = 0; i != init->size(); ++i)
        if (FirstOfLeaf((*init)[i], f))
            return true;
    return false;
}

bool VisitFirstOfNode(Expr                       const  & init,
                 std::function<bool(Expr const&)> const& f)
{
    if (f(init))
        return true;
    for (size_t i = 0; i != init->size(); ++i)
        if (VisitFirstOfNode((*init)[i], f))
            return true;
    return false;
}

bool VisitFirstOfLeaf(Expr                       const& init,
                 std::function<bool(Expr const&)> const& f)
{
    if (init->size() == 0)
        return f(init);
    for (size_t i = 0; i != init->size(); ++i)
        if (VisitFirstOfLeaf((*init)[i], f))
            return true;
    return false;
}


bool Transform(Expr                            & init,
               std::function<bool(Expr&)> const& f,
               int                               depth)
{
    bool transformed = false;
    if (depth != 0) {
        for (size_t i = 0; i != init->size(); ++i)
            if (Transform((*init)[i], f, depth-1))
                transformed = true;
    }
    if ((transformed = (f(init) or transformed)))
        init = Refreshed(init);
    return transformed;
}

} // End of namespace csl
