#include "patternMatch.h"
#include "numerical.h"
#include "literal.h"
#include "abreviation.h"
#include "operations.h"
#include "hardFactor.h"

namespace csl::matcher {

size_t dichoFinder(
        csl::Expr          const &expr,
        std::vector<Node*> const &v
        )
{
    size_t first = 0;
    size_t last = v.size();
    while (last != first) {
        size_t mid = (first + last) / 2;
        auto const &midExpr = v[mid]->expr;
        if (expr < midExpr)
            last = mid;
        else if (midExpr < expr) {
            if (mid == first)
                ++first;
            else
                first = mid;
        }
        else
            return mid;
        if (first + 1 == mid) {
            return (expr < v[first]->expr) ? first : mid;
        }
    }
    return first;
}

void compress(csl::Expr &expr, size_t nIter)
{
    for (size_t iter = 0; iter != nIter; ++iter) {
        Tree sumTree(Node::ExprType::Sum);
        Tree prodTree(Node::ExprType::Prod);
        std::vector<Tree*> trees = { &sumTree, &prodTree };
        csl::VisitEachNodeReversed(expr, [&](csl::Expr const &node) { 
            if (csl::IsSum(node)) 
                sumTree.parse(node->getVectorArgument());
            else if (csl::IsProd(node)) 
                prodTree.parse(node->getVectorArgument());
        });
        sumTree.removeSingle();
        prodTree.removeSingle();
        Tree::findAllAbbreviations(trees);
        csl::ForEachNode(expr, [&](csl::Expr &node) { 
            std::optional<csl::Expr> ab 
                = Tree::getChainAbbreviationFor(node, trees);
            if (ab) {
                node = ab.value();
            }
        });
        csl::DeepRefresh(expr);
        csl::DeepHardFactor(expr);
    }
    csl::Abbrev::removeAbbreviations("SubSum");
    csl::Abbrev::removeAbbreviations("SubProd");
}

///////////////////////////////////////////////////
// Class Node
///////////////////////////////////////////////////

    size_t Node::distance(Node const *first, Node const *last) 
    {
        size_t d = 0;
        while (first != last) {
            last = last->parent;
            ++d;
        }
        return d;
    }

    size_t Node::nLeafs(csl::Expr const &expr) 
    {
        size_t n = 0;
        csl::VisitEachLeaf(expr, [&n](csl::Expr const&) { ++n; });
        return n;
    }

    Node::iterator Node::insert(csl::Expr const &t_expr) 
    {
        const size_t pos = dichoFinder(t_expr, children);
        const auto insertPos = children.begin() + pos;
        if (insertPos != children.end() && (**insertPos).expr == t_expr) {
            ++(**insertPos).nOccurences;
            return insertPos;
        }
        return children.insert(insertPos, build(t_expr, this));
    }

    std::pair<Node*, std::vector<csl::Expr>::const_iterator> 
        Node::findBestMatch(
            std::vector<csl::Expr>::const_iterator first,
            std::vector<csl::Expr>::const_iterator last
            )
    {
        if (first == last)
            return {nullptr, last};
        for (Node *c : children) {
            if (c->expr == *first) {
                ++first;
                const auto res = c->findBestMatch(first, last);
                if (res.first)
                    return res;
                else 
                    return {c, first};
            }
        }
        return {nullptr, first};
    }

    std::vector<csl::Expr> Node::getArgs() const
    {
        size_t nArgs = 0;
        auto iter = this;
        while (iter) {
            iter = iter->parent;
            ++nArgs;
        }
        --nArgs;
        std::vector<csl::Expr> args(nArgs);
        int index = nArgs - 1;
        iter = this;
        while (index >= 0) {
            args[index] = iter->expr;
            iter = iter->parent;
            ++nArgs;
            --index;
        }
        return args;
    }

    csl::Expr Node::getChainExpr(
            std::vector<csl::Expr> const &args,
            ExprType type
            ) 
    {
        return (type == ExprType::Sum) ? 
            csl::sum_s(args, true) 
            : csl::prod_s(args, true);
    }

    csl::Expr Node::getChainExpr(ExprType type) const 
    {
        return getChainExpr(getArgs(), type);
    }

    bool Node::isAbbreviated() const {
        if (isRoot())
            return false;
        if (children.size() == 1 && children[0]->nOccurences == nOccurences)
            return false;
        return true;
    }

    csl::Expr Node::makeAbbreviation(
            csl::Expr const &expr,
            ExprType         type
            )
    {
        if (type == ExprType::Sum) {
            return csl::Abbrev::makeAbbreviation(
                    "SubSum",
                    expr
                    );
        }
        else {
            return csl::Abbrev::makeAbbreviation(
                    "SubProd",
                    expr
                    );
        }
    }

    csl::Expr Node::makeExpression(
            std::vector<csl::Expr> const &args,
            ExprType                      type
            )
    {
        if (type == ExprType::Sum) {
            return csl::sum_s(args);
        }
        else {
            return csl::prod_s(args);
        }
    }

    csl::Expr Node::getAbbreviation() const 
    {
        return abbreviation;
    }

    csl::Expr Node::getChainAbbreviation() const 
    {
        return chainAbbreviation;
    }

    void Node::setAbbreviation(
            std::vector<Tree*> &trees,
            ExprType type
            ) 
    {
        if (abbreviation)
            return;
        auto opt = Tree::getChainAbbreviationFor(expr, trees);
        abbreviation = opt.value_or(expr);
        if (!parent || parent->isRoot()) {
            chainAbbreviation = abbreviation;
            return;
        }
        parent->setAbbreviation(trees, type);
        csl::Expr left = parent->getChainAbbreviation();
        csl::Expr chain = makeExpression({left, abbreviation}, type);
        if (isAbbreviated()) {
            chainAbbreviation = makeAbbreviation(chain, type);
        }
        else {
            chainAbbreviation = chain;
        }
    }

    void Node::parse(
            std::vector<csl::Expr>::const_iterator first,
            std::vector<csl::Expr>::const_iterator last
            )
    {
        if (first == last || static_cast<size_t>(last - first) > maxLeaf)
            return;
        if ((**first).isIndexed())
            return;
        if (nLeafs(*first) > maxLeaf)
            return;
        auto iter = insert(*first);
        ++first;
        (**iter).parse(first, last);
        if (useDifferedStart)
            parse(first, last);
    }

    Node *Node::build(
            csl::Expr const &t_expr, 
            Node            *t_parent
            ) 
    {
        return new Node{t_expr, t_parent};
    }

    void Node::destroy(Node *&node) 
    {
        for (auto &c : node->children) {
            destroy(c);
        }
        delete node;
        node = nullptr;
    }

    void Node::removeSingle(Node *&node) 
    {
        if (node->parent && node->nOccurences == 1) {
            destroy(node);
        }
        else {
            for (Node *&c : node->children) {
                removeSingle(c);
            }
            auto last = std::remove_if(
                    node->children.begin(), 
                    node->children.end(),
                    [](Node * subNode) { 
                        return !subNode; 
                    });
            node->children.erase(last, node->children.end());
        }
    }

    void Node::print(int indent) const {
        if (parent) {
            for (int i = 0; i != indent; ++i) 
                std::cout << ' ';
            std::cout << expr << " ; (" << nOccurences << ")" << std::endl;
            if (abbreviation) {

                std::cout << "                   AB : " << abbreviation << " = " << csl::Evaluated(abbreviation, csl::eval::abbreviation);
            std::cout << std::endl;
                std::cout << "                   AB : " << chainAbbreviation << " = " << csl::Evaluated(chainAbbreviation, csl::eval::abbreviation);
            std::cout << std::endl;
            }
            indent += 2;
        }
        else 
            std::cout << "Tree : " << std::endl;
        for (const auto &c : children)
            c->print(indent);
    }

///////////////////////////////////////////////////
// Class Tree
///////////////////////////////////////////////////

Tree::Tree(Node::ExprType t_type)
    :type(t_type),
    root(Node::build(CSL_UNDEF))
{}

Tree::~Tree() 
{
    Node::destroy(root);
}

std::pair<Node*, std::vector<csl::Expr>::const_iterator>
    Tree::findBestMatch(
        std::vector<csl::Expr> const &vec,
        size_t minElements
        ) 
{
    auto res = root->findBestMatch(vec.begin(), vec.end());
    if (res.first && Node::distance(root, res.first) < minElements)
        res.first = nullptr;
    return res;
}

std::optional<csl::Expr> Tree::getChainAbbreviationFor(
        csl::Expr    const &init,
        std::vector<Tree*> &trees
        )
{
    if (init->size() == 0)
        return std::nullopt;
    if (csl::IsSum(init)) {
        std::vector<csl::Expr> const &args = init->getVectorArgument();
        for (auto &t : trees) {
            if (t->type == Node::ExprType::Sum) {
                auto match = t->findBestMatch(args);
                if (match.first) {
                    match.first->setAbbreviation(trees, Node::ExprType::Sum);
                    if (match.second == args.end()) {
                        return match.first->getChainAbbreviation();
                    }
                    std::vector<csl::Expr> newArgs(1 + args.end() - match.second);
                    newArgs[0] = match.first->getChainAbbreviation();
                    for (size_t i = 1; i != newArgs.size(); ++i) {
                        newArgs[i] = args[(match.second - args.begin()) + i - 1];
                    }
                    return csl::sum_s(newArgs, true);
                }
            }
        }
    }
    else if (csl::IsProd(init)) {
        std::vector<csl::Expr> const &args = init->getVectorArgument();
        for (auto &t : trees) {
            if (t->type == Node::ExprType::Prod) {
                auto match = t->findBestMatch(args);
                if (match.first) {
                    match.first->setAbbreviation(trees, Node::ExprType::Prod);
                    if (match.second == args.end()) {
                        return match.first->getChainAbbreviation();
                    }
                    std::vector<csl::Expr> newArgs(1 + args.end() - match.second);
                    newArgs[0] = match.first->getChainAbbreviation();
                    for (size_t i = 1; i != newArgs.size(); ++i) {
                        newArgs[i] = args[(match.second - args.begin()) + i - 1];
                    }
                    return csl::prod_s(newArgs, true);
                }
            }
        }
    }
    return std::nullopt;
}

void Tree::findAllAbbreviations(
        Node               *node,
        Tree               *t,
        std::vector<Tree*> &trees)
{
    if (node->empty()) {
        node->setAbbreviation(trees, t->type);
    }
    else 
        for (Node *subNode : node->children)
            findAllAbbreviations(subNode, t, trees);
}

void Tree::findAllAbbreviations(std::vector<Tree*> &trees)
{
    for (auto &t : trees) {
        Node *root = t->root;
        for (Node *node : root->children)
            findAllAbbreviations(node, t, trees);
    }
}

void Tree::parse(std::vector<csl::Expr> const &vec) 
{
    root->parse(vec.begin(), vec.end());
}

void Tree::removeSingle()
{
    Node::removeSingle(root);
}

void Tree::print() 
{
    root->print();
}

} // namespace csl::matcher
