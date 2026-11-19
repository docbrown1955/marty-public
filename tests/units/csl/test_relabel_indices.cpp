#include <csl.h>
#include <gtest/gtest.h>
#include <tuple>
#include <vector>

using namespace csl;

namespace {

std::vector<Index> indicesOf(Expr const &expr)
{
    auto structure = expr->getIndexStructure();
    return structure.getIndex();
}

} // namespace

// Expressions without indices

TEST(RelabelIndices, expressionWithoutIndicesIsUnchanged)
{
    auto a    = variable_s("a");
    auto b    = variable_s("b");
    auto expr = a + 2 * b;
    EXPECT_EQ(RelabelIndices(expr)->printLaTeX(0), expr->printLaTeX(0));
}

// Free indices

TEST(RelabelIndices, singleFreeIndexTakesFirstName)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor z{"z", &S};
    Index  f       = S.generateIndex("beta");
    auto   renamed = RelabelIndices(z(f));
    auto   indices = indicesOf(renamed);
    ASSERT_EQ(indices.size(), 1u);
    EXPECT_EQ(std::string(indices[0].getName()), "alpha");
    EXPECT_EQ(indices[0].getID(), 0u);
}

TEST(RelabelIndices, distinctFreeIndicesTakeDistinctNames)
{
    Space                 S{"S", 4, {"alpha", "beta"}};
    Tensor                z{"z", &S};
    Tensor                w{"w", &S};
    Index                 f1      = S.generateIndex("beta");
    Index                 f2      = S.generateIndex("beta");
    auto                  renamed = RelabelIndices(z(f1) * w(f2));
    std::set<std::string> names;
    for (auto const &index : indicesOf(renamed))
        names.insert(std::string(index.getName()));
    EXPECT_EQ(names, (std::set<std::string>{"alpha", "beta"}));
}

TEST(RelabelIndices, sameFreeIndexInSumKeepsOneLabel)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  f = S.generateIndex("beta");
    EXPECT_EQ(printLatex(x(f) + y(f)), "x_{alpha} + y_{alpha}");
}

// Dummy indices

TEST(RelabelIndices, dummyPairSharesOneLabel)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p = S.generateIndex("alpha");
    EXPECT_EQ(printLatex(x(p) * y(p)), "x_{alpha}y_{alpha}");
}

TEST(RelabelIndices, dummyPairKeepsOppositeSignsOnBothOccurrences)
{
    Space const &S = buildMinkowski();
    Tensor       x{"x", &S};
    Tensor       y{"y", &S};
    Index        p     = S.generateIndex("mu");
    Index        q     = p.getFlipped();
    auto         input = indicesOf(x(p) * y(q));
    ASSERT_EQ(input.size(), 2u);
    ASSERT_NE(input[0].getSign(), input[1].getSign());
    auto renamed = RelabelIndices(x(p) * y(q));
    auto indices = indicesOf(renamed);
    ASSERT_EQ(indices.size(), 2u);
    EXPECT_EQ(indices[0].getName(), indices[1].getName());
    EXPECT_NE(indices[0].getSign(), indices[1].getSign());
}

TEST(RelabelIndices, distinctPairsTakeNamesInOrder)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p1 = S.generateIndex("alpha");
    Index  p2 = S.generateIndex("alpha");
    EXPECT_EQ(printLatex(x(p1) * y(p1) * x(p2) * y(p2)),
              "x_{alpha}x_{beta}y_{alpha}y_{beta}");
}

TEST(RelabelIndices, poolWrapAroundAddsSuffixesFromOne)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Expr   expr = CSL_1;
    for (int k = 0; k != 5; ++k) {
        Index p = S.generateIndex("alpha");
        expr    = expr * x(p) * y(p);
    }
    std::string latex = printLatex(expr);
    for (auto const &label : {"x_{alpha}",
                              "x_{beta}",
                              "x_{alpha_{1}}",
                              "x_{beta_{1}}",
                              "x_{alpha_{2}}"}) {
        EXPECT_NE(latex.find(label), std::string::npos) << latex;
    }
    EXPECT_EQ(latex.find("x_{alpha_{3}}"), std::string::npos) << latex;
}

TEST(RelabelIndices, sameNameDifferentIdsAreDifferentIdentities)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  f = S.generateIndex("alpha");
    Index  p = S.generateIndex("alpha");
    EXPECT_EQ(printLatex(x(f) * y(p) * x(p)), "x_{alpha}x_{beta}y_{beta}");
}

// Free and dummy indices together

TEST(RelabelIndices, freeIndicesTakeNamesBeforeDummies)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Tensor z{"z", &S};
    Index  f = S.generateIndex("beta");
    Index  p = S.generateIndex("alpha");
    EXPECT_EQ(printLatex(x(p) * y(p) * z(f)), "x_{beta}y_{beta}z_{alpha}");
}

// Several spaces (disjoint index sets)

TEST(RelabelIndices, spacesKeepTheirOwnSequences)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Space  L{"L", 4, {"mu", "nu"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Tensor u{"u", &L};
    Tensor v{"v", &L};
    Index  p = S.generateIndex("beta");
    Index  m = L.generateIndex("nu");
    Index  n = L.generateIndex("nu");
    EXPECT_EQ(printLatex(x(p) * y(p) * u(m) * v(m) * u(n) * v(n)),
              "u_{mu}u_{nu}v_{mu}v_{nu}x_{alpha}y_{alpha}");
}

TEST(RelabelIndices, freeIndexOfOneSpaceAndDummyOfAnother)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Space  L{"L", 4, {"mu", "nu"}};
    Tensor G{"G;\\gamma", std::vector<Space const *>{&L, &S, &S}};
    Tensor H{"H", std::vector<Space const *>{&S, &S}};
    Index  mu = L.generateIndex("nu");
    Index  i  = S.generateIndex("alpha");
    Index  j  = S.generateIndex("alpha");
    auto expr = G(std::vector<Index>{mu, i, j}) * H(std::vector<Index>{i, j});
    auto renamed = RelabelIndices(expr);
    std::set<std::string> free;
    for (auto const &index : indicesOf(renamed)) {
        if (index.getType() == cslIndex::Free)
            free.insert(std::string(index.getName()));
    }
    EXPECT_EQ(free, (std::set<std::string>{"mu"}));
}

// IDs and input

TEST(RelabelIndices, resetsAllIdsToZero)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p       = S.generateIndex("alpha");
    Index  q       = S.generateIndex("alpha");
    Index  f       = S.generateIndex("beta");
    auto   renamed = RelabelIndices(x(p) * y(p) * x(q) * y(q) * x(f));
    for (auto const &factor : renamed->getFactors())
        for (auto const &index : indicesOf(factor))
            EXPECT_EQ(index.getID(), 0u);
}

TEST(RelabelIndices, inputIsNotModified)
{
    Space       S{"S", 4, {"alpha", "beta"}};
    Tensor      x{"x", &S};
    Tensor      y{"y", &S};
    Index       p        = S.generateIndex("alpha");
    Index       f        = S.generateIndex("beta");
    auto        expr     = x(p) * y(p) * x(f);
    std::string before   = expr->printLaTeX(0);
    auto        idBefore = p.getID();
    RelabelIndices(expr);
    EXPECT_EQ(expr->printLaTeX(0), before);
    EXPECT_EQ(p.getID(), idBefore);
    EXPECT_EQ(std::string(f.getName()), "beta");
}

TEST(RelabelIndices, relabellingTwiceGivesTheSameLabels)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p1   = S.generateIndex("alpha");
    Index  p2   = S.generateIndex("alpha");
    Index  p3   = S.generateIndex("alpha");
    auto   expr = x(p1) * y(p1) * x(p2) * y(p2) * x(p3) * y(p3);
    auto   once = RelabelIndices(expr);
    EXPECT_EQ(RelabelIndices(once)->printLaTeX(0), once->printLaTeX(0));
}

// Derivative nodes carry their own index

TEST(RelabelIndices, derivativeIndexIsRelabelledWithItsPair)
{
    Space       S{"S", 4, {"alpha", "beta"}};
    Tensor      x{"x", &S};
    Tensor      phi{"phi", &S};
    TDerivative D{"D", &S};
    Index       p       = S.generateIndex("alpha");
    auto        renamed = RelabelIndices(D(p, phi) * x(p));
    for (auto const &factor : renamed->getFactors())
        for (auto const &index : indicesOf(factor)) {
            EXPECT_EQ(std::string(index.getName()), "alpha");
            EXPECT_EQ(index.getID(), 0u);
        }
}

// Fixed indices

// Non-modification of the input and of shared subexpressions

namespace {

using IndexKey = std::tuple<std::string, unsigned, int>;

std::vector<IndexKey> indexKeys(Expr const &expr)
{
    std::vector<IndexKey> keys;
    Expr                  copy = expr;
    ForEachNode(copy, [&keys](Expr &sub) {
        if (not IsIndicialTensor(sub))
            return;
        auto structure = sub->getIndexStructure();
        for (auto const &index : structure.getIndex())
            keys.emplace_back(std::string(index.getName()),
                              index.getID(),
                              static_cast<int>(index.getType()));
    });
    return keys;
}

} // namespace

TEST(RelabelIndices, sharedSubexpressionIsNotModified)
{
    Space       S{"S", 4, {"alpha", "beta"}};
    Tensor      x{"x", &S};
    Tensor      y{"y", &S};
    Tensor      z{"z", &S};
    Tensor      w{"w", &S};
    Index       p          = S.generateIndex("alpha");
    Index       f          = S.generateIndex("beta");
    Expr        sub        = x(p) * y(p);
    Expr        expr       = sub * z(f) + sub * w(f);
    std::string subBefore  = sub->printLaTeX(0);
    std::string exprBefore = expr->printLaTeX(0);
    auto        subKeys    = indexKeys(sub);
    auto        exprKeys   = indexKeys(expr);

    auto renamed = RelabelIndices(expr);

    EXPECT_EQ(sub->printLaTeX(0), subBefore);
    EXPECT_EQ(expr->printLaTeX(0), exprBefore);
    EXPECT_EQ(indexKeys(sub), subKeys);
    EXPECT_EQ(indexKeys(expr), exprKeys);
    EXPECT_NE(renamed.get(), expr.get());
}

TEST(RelabelIndices, otherExpressionsSharingTheInputStayValid)
{
    Space       S{"S", 4, {"alpha", "beta"}};
    Tensor      x{"x", &S};
    Tensor      y{"y", &S};
    Tensor      z{"z", &S};
    Index       p            = S.generateIndex("alpha");
    Index       f            = S.generateIndex("beta");
    Expr        sub          = x(p) * y(p);
    Expr        first        = sub * z(f);
    Expr        second       = sub;
    std::string secondBefore = second->printLaTeX(0);
    auto        secondKeys   = indexKeys(second);

    auto renamed = RelabelIndices(first);

    EXPECT_EQ(second->printLaTeX(0), secondBefore);
    EXPECT_EQ(indexKeys(second), secondKeys);
    EXPECT_EQ(printLatex(second), "x_{alpha}y_{alpha}");
    EXPECT_EQ(printLatex(renamed), printLatex(first));
}

TEST(RelabelIndices, userIndicesKeepNameIdAndType)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Tensor z{"z", &S};
    Index  p           = S.generateIndex("alpha");
    Index  f           = S.generateIndex("beta");
    auto   pIdBefore   = p.getID();
    auto   fIdBefore   = f.getID();
    auto   pTypeBefore = p.getType();
    auto   fTypeBefore = f.getType();
    RelabelIndices(x(p) * y(p) * z(f));
    EXPECT_EQ(p.getID(), pIdBefore);
    EXPECT_EQ(f.getID(), fIdBefore);
    EXPECT_EQ(p.getType(), pTypeBefore);
    EXPECT_EQ(f.getType(), fTypeBefore);
    EXPECT_EQ(std::string(p.getName()), "alpha");
    EXPECT_EQ(std::string(f.getName()), "beta");
}

TEST(RelabelIndices, repeatedSubexpressionInTheSameExpressionStaysValid)
{
    Space       S{"S", 4, {"alpha", "beta"}};
    Tensor      x{"x", &S};
    Tensor      y{"y", &S};
    Index       p         = S.generateIndex("alpha");
    Expr        sub       = x(p) * y(p);
    Expr        expr      = sub * sub;
    std::string subBefore = sub->printLaTeX(0);
    auto        subKeys   = indexKeys(sub);

    RelabelIndices(expr);

    EXPECT_EQ(sub->printLaTeX(0), subBefore);
    EXPECT_EQ(indexKeys(sub), subKeys);
}

TEST(RelabelIndices, fixedIndexIsNotRenamed)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Index  f = S.generateIndex("beta");
    f.setValue(1);
    EXPECT_EQ(printLatex(x(f)), "x_{1}");
}

// Keeping free indices

namespace {

std::set<std::string> namesOfType(Expr const &expr, cslIndex::Type type)
{
    std::set<std::string> names;
    for (auto const &index : indicesOf(expr))
        if (index.getType() == type)
            names.insert(std::string(index.getName()));
    return names;
}

} // namespace

TEST(RelabelIndices, freeIndicesKeepTheirNamesWhenNotRelabelled)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor z{"z", &S};
    Index  f       = S.generateIndex("alpha");
    Index  p       = S.generateIndex("alpha");
    auto   renamed = RelabelIndices(z(f) * x(p) * x(p), false);
    EXPECT_EQ(namesOfType(renamed, cslIndex::Free),
              (std::set<std::string>{"alpha"}));
    EXPECT_EQ(namesOfType(renamed, cslIndex::Dummy),
              (std::set<std::string>{"beta"}));
}

TEST(RelabelIndices, dummyPairsAvoidKeptFreeNames)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Tensor z{"z", &S};
    Index  f       = S.generateIndex("alpha");
    Index  p       = S.generateIndex("alpha");
    auto   renamed = RelabelIndices(z(f) * x(p) * x(p), false);
    EXPECT_EQ(renamed->printLaTeX(0), "x_{beta}x_{beta}z_{alpha}");
}

// Renaming map shared between expressions

TEST(RelabelIndices, renamingAppliedToAnotherExpressionGivesSameLabels)
{
    Space         S{"S", 4, {"alpha", "beta"}};
    Tensor        x{"x", &S};
    Tensor        w{"w", &S};
    Index         f      = S.generateIndex("beta");
    Index         p      = S.generateIndex("alpha");
    Expr          first  = w(f) * x(p);
    Expr          second = x(p) * w(f);
    IndexRenaming renaming;
    auto          relabelled = RelabelIndices(first, true, renaming);
    auto          applied    = ApplyIndexRenaming(second, renaming);
    EXPECT_EQ(applied->printLaTeX(0), relabelled->printLaTeX(0));
    EXPECT_EQ(relabelled->printLaTeX(0), "w_{alpha}x_{beta}");
}

TEST(RelabelIndices, renamingKeepsOnlyFreeIndices)
{
    Space         S{"S", 4, {"alpha", "beta"}};
    Tensor        x{"x", &S};
    Tensor        w{"w", &S};
    Index         f    = S.generateIndex("beta");
    Index         p    = S.generateIndex("alpha");
    Expr          expr = w(f) * x(p) * x(p);
    IndexRenaming renaming;
    RelabelIndices(expr, true, renaming);
    ASSERT_EQ(renaming.renamed.size(), 1U);
    EXPECT_EQ(
        renaming.renamed.begin()->first,
        IndexRenaming::Key(f.getSpace(), f.getID(), std::string(f.getName())));
}

TEST(RelabelIndices, applyRenamingDoesNotRenameDummyIndices)
{
    Space         S{"S", 4, {"alpha", "beta"}};
    Tensor        x{"x", &S};
    Tensor        w{"w", &S};
    Index         f = S.generateIndex("beta");
    IndexRenaming renaming;
    RelabelIndices(w(f), true, renaming);
    Expr expr = x(f) * x(f);
    EXPECT_EQ(ApplyIndexRenaming(expr, renaming)->printLaTeX(0),
              expr->printLaTeX(0));
}

TEST(RelabelIndices, applyingRenamingWithoutMatchingIndicesIsUnchanged)
{
    Space         S{"S", 4, {"alpha", "beta"}};
    Tensor        x{"x", &S};
    Tensor        w{"w", &S};
    Index         f = S.generateIndex("beta");
    Index         g = S.generateIndex("beta");
    IndexRenaming renaming;
    RelabelIndices(w(f), true, renaming);
    Expr expr = x(g) * x(g);
    EXPECT_EQ(ApplyIndexRenaming(expr, renaming)->printLaTeX(0),
              expr->printLaTeX(0));
    IndexRenaming empty;
    EXPECT_EQ(ApplyIndexRenaming(w(f), empty)->printLaTeX(0),
              w(f)->printLaTeX(0));
}

TEST(RelabelIndices, applyRenamingDoesNotModifyTheInput)
{
    Space         S{"S", 4, {"alpha", "beta"}};
    Tensor        x{"x", &S};
    Tensor        w{"w", &S};
    Index         f    = S.generateIndex("beta");
    Index         p    = S.generateIndex("alpha");
    Expr          expr = w(f) * x(p) * x(p);
    IndexRenaming renaming;
    RelabelIndices(expr, true, renaming);
    std::string before = expr->printLaTeX(0);
    ApplyIndexRenaming(expr, renaming);
    EXPECT_EQ(expr->printLaTeX(0), before);
    EXPECT_EQ(std::string(f.getName()), "beta");
}
