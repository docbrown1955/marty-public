#include <csl.h>
#include <gtest/gtest.h>
#include <string>

using namespace csl;

namespace {

// Number of metric/delta elements whose parent disagrees with the index signs
// of a signed space (Metric: equal signs, Delta: opposite signs).
size_t countInconsistentMetricDelta(Expr const &expr)
{
    size_t wrong = 0;
    VisitEachNode(expr, [&](Expr const &node) {
        if (node->getType() != Type::TensorElement)
            return;
        Parent_info      parent    = node->getParent_info();
        IndexStructure   structure = node->getIndexStructure();
        if (not parent or structure.size() != 2 or not structure[0].getSpace()
            or not structure[0].getSpace()->getSignedIndex())
            return;
        bool sameSign = structure[0].getSign() == structure[1].getSign();
        if (parent->getType() == cslParent::Metric and not sameSign)
            ++wrong;
        if (parent->getType() == cslParent::Delta and sameSign)
            ++wrong;
    });
    return wrong;
}

void expectConsistent(Expr const &expr, std::string const &context)
{
    EXPECT_EQ(countInconsistentMetricDelta(expr), 0u)
        << context << ": " << printLatex(expr);
}

IndexRenaming flipRenaming(Index const &index)
{
    IndexRenaming renaming;
    renaming.renamed[IndexRenaming::Key{
        index.getSpace(), index.getID(), std::string(index.getName())}]
        = index.getFlipped();
    return renaming;
}

Expr metricOn(Space const &S, Index a, Index b)
{
    return S.getMetric()(std::vector<Index>{a, b});
}

Expr deltaOn(Space const &S, Index a, Index b)
{
    return S.getDelta()(std::vector<Index>{a, b});
}

} // namespace

TEST(MetricDeltaInvariant, constructedElementsMatchTheirSigns)
{
    Space const &S   = buildMinkowski();
    Index        mu  = S.generateIndex("mu");
    Index        nu  = S.generateIndex("nu");
    Index        rho = S.generateIndex("rho");
    expectConsistent(metricOn(S, mu, nu), "metric");
    expectConsistent(metricOn(S, mu, nu.getFlipped()), "metric flipped");
    expectConsistent(deltaOn(S, mu, nu), "delta");
    expectConsistent(deltaOn(S, mu, nu.getFlipped()), "delta flipped");
    expectConsistent(metricOn(S, mu, nu) * deltaOn(S, nu.getFlipped(), rho),
                     "product");
}

TEST(MetricDeltaInvariant, directIndexWriteReselectsMetricAsDelta)
{
    Space const &S  = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr         g  = Copy(metricOn(S, mu, nu));
    g->getIndexStructureView()[1] = nu.getFlipped();
    Expr refreshed = DeepRefreshed(g);
    expectConsistent(refreshed, "metric after direct index write, refreshed");
    EXPECT_EQ(printLatex(refreshed), printLatex(deltaOn(S, mu, nu.getFlipped())));
}

TEST(MetricDeltaInvariant, directIndexWriteReselectsDeltaAsMetric)
{
    Space const &S  = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr         d  = Copy(deltaOn(S, mu, nu.getFlipped()));
    d->getIndexStructureView()[1] = nu;
    Expr refreshed = DeepRefreshed(d);
    expectConsistent(refreshed, "delta after direct index write, refreshed");
    EXPECT_EQ(printLatex(refreshed), printLatex(metricOn(S, mu, nu)));
}

TEST(MetricDeltaInvariant, replaceFlipsMetricIntoDelta)
{
    Space const &S  = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr         g  = Copy(metricOn(S, mu, nu));
    Replace(g, nu, nu.getFlipped());
    expectConsistent(g, "metric after Replace");
    EXPECT_EQ(printLatex(g), printLatex(deltaOn(S, mu, nu.getFlipped())));
}

TEST(MetricDeltaInvariant, setIndexStructureReselectsParent)
{
    Space const &S  = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr         d  = Copy(deltaOn(S, mu, nu.getFlipped()));
    d->setIndexStructure(IndexStructure{mu, nu});
    expectConsistent(d, "delta after setIndexStructure");
}

TEST(MetricDeltaInvariant, renamingKeepsProductsConsistent)
{
    Space const &S   = buildMinkowski();
    Index        mu  = S.generateIndex("mu");
    Index        nu  = S.generateIndex("nu");
    Index        rho = S.generateIndex("rho");
    Expr expr = metricOn(S, mu, nu) * deltaOn(S, nu.getFlipped(), rho)
                + deltaOn(S, mu, rho);
    Expr renamed = ApplyIndexRenaming(expr, flipRenaming(nu));
    expectConsistent(renamed, "ApplyIndexRenaming");
    expectConsistent(RelabelIndices(renamed), "RelabelIndices");
}

TEST(MetricDeltaInvariant, refreshingAndFactoringKeepsConsistency)
{
    Space const &S   = buildMinkowski();
    Index        mu  = S.generateIndex("mu");
    Index        nu  = S.generateIndex("nu");
    Index        rho = S.generateIndex("rho");
    Expr         g   = Copy(metricOn(S, mu, nu));
    g->getIndexStructureView()[0] = mu.getFlipped();
    Expr product = g * deltaOn(S, nu.getFlipped(), rho);
    Expand(product);
    Expr value = DeepRefreshed(DeepHardFactored(product));
    expectConsistent(value, "DeepRefreshed(DeepHardFactored(Expand))");
}
