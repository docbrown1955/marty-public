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

#pragma once

#include "abstract.h"

namespace csl::matcher {

struct Node;
struct Tree;
size_t dichoFinder(csl::Expr const &expr, std::vector<Node *> const &v);

void compress_impl(std::vector<csl::Expr> &expr, size_t nIter = 1);
void compress(std::vector<csl::Expr> &expr, size_t nIter = 1);

inline void compress(csl::Expr &expr, size_t nIter = 1)
{
    std::vector<csl::Expr> vec{expr};
    compress(vec, nIter);
    expr = vec[0];
}

inline void compress_impl(csl::Expr &expr, size_t nIter = 1)
{
    std::vector<csl::Expr> vec{expr};
    compress_impl(vec, nIter);
    expr = vec[0];
}

struct Node {

    using Container      = std::vector<Node *>;
    using iterator       = Container::iterator;
    using const_iterator = Container::const_iterator;

    enum class ExprType { Sum, Prod };

    static inline bool   useDifferedStart = false;
    static inline size_t maxLeaf          = 10;

    bool empty() const
    {
        return children.empty();
    }
    auto size() const
    {
        return children.size();
    }
    auto begin()
    {
        return children.begin();
    }
    auto end()
    {
        return children.end();
    }
    auto begin() const
    {
        return children.begin();
    }
    auto end() const
    {
        return children.end();
    }

    bool isRoot() const
    {
        return !parent;
    }
    bool isAbbreviated() const;

    static csl::Expr makeAbbreviation(csl::Expr const &expr, ExprType type);

    static csl::Expr makeExpression(std::vector<csl::Expr> const &args,
                                    ExprType                      type);

    static size_t distance(Node const *first, Node const *last);
    static size_t nLeafs(csl::Expr const &expr);

    iterator insert(csl::Expr const &t_expr);

    std::pair<Node *, std::vector<csl::Expr>::const_iterator>
    findBestMatch(std::vector<csl::Expr>::const_iterator first,
                  std::vector<csl::Expr>::const_iterator last);

    std::vector<csl::Expr> getArgs() const;
    static csl::Expr       getChainExpr(std::vector<csl::Expr> const &args,
                                        ExprType                      type);
    csl::Expr              getChainExpr(ExprType type) const;
    csl::Expr              getAbbreviation() const;
    csl::Expr              getChainAbbreviation() const;

    void setAbbreviation(std::vector<Tree *> &trees, ExprType type);

    void parse(std::vector<csl::Expr>::const_iterator first,
               std::vector<csl::Expr>::const_iterator last);

    static Node *build(csl::Expr const &t_expr, Node *t_parent = nullptr);

    static void destroy(Node *&node);
    static void removeSingle(Node *&node);

    void print(int indent = 0) const;

    csl::Expr           expr;
    Node *              parent;
    csl::Expr           abbreviation{nullptr};
    csl::Expr           chainAbbreviation{nullptr};
    size_t              nOccurences{1};
    std::vector<Node *> children{};
};

struct Tree {

    Tree(Node::ExprType t_type);
    ~Tree();

    std::pair<Node *, std::vector<csl::Expr>::const_iterator>
    findBestMatch(std::vector<csl::Expr> const &vec, size_t minElements = 2);

    static std::optional<csl::Expr>
    getChainAbbreviationFor(csl::Expr const &init, std::vector<Tree *> &trees);

    static void findAllAbbreviations(std::vector<Tree *> &trees);

    static void
    findAllAbbreviations(Node *node, Tree *t, std::vector<Tree *> &trees);

    void parse(std::vector<csl::Expr> const &vec);
    void removeSingle();
    void print();

    Node::ExprType type;
    Node *         root;
};

} // namespace csl::matcher
