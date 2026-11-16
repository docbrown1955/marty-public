#include <csl.h>
#include <gtest/gtest.h>
#include <string>

using namespace csl;

namespace {

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

TEST(SignedRefresh, metricWithFlippedIndexBecomesDelta)
{
    Space const &S = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr flipped = ApplyIndexRenaming(metricOn(S, mu, nu), flipRenaming(nu));
    Expr refreshed = DeepRefreshed(flipped);
    Expr delta = deltaOn(S, mu, nu.getFlipped());
    EXPECT_EQ(printLatex(refreshed), printLatex(delta));
    EXPECT_EQ(refreshed, delta);
}

TEST(SignedRefresh, deltaWithEqualSignsBecomesMetric)
{
    Space const &S = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr flipped = ApplyIndexRenaming(deltaOn(S, mu, nu.getFlipped()),
                                      flipRenaming(nu.getFlipped()));
    Expr refreshed = DeepRefreshed(flipped);
    Expr metric = metricOn(S, mu, nu);
    EXPECT_EQ(printLatex(refreshed), printLatex(metric));
    EXPECT_EQ(refreshed, metric);
}

TEST(SignedRefresh, renamingFlipsMetricIntoDeltaInOutput)
{
    Space const &S = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr flipped = ApplyIndexRenaming(metricOn(S, mu, nu), flipRenaming(nu));
    Expr refreshed = DeepRefreshed(flipped);
    EXPECT_EQ(refreshed->printLaTeX(0),
              deltaOn(S, mu, nu.getFlipped())->printLaTeX(0));
    EXPECT_EQ(printLatex(refreshed),
              printLatex(deltaOn(S, mu, nu.getFlipped())));
}

TEST(SignedRefresh, renamingFlipsDeltaIntoMetricInOutput)
{
    Space const &S = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr flipped = ApplyIndexRenaming(deltaOn(S, mu, nu.getFlipped()),
                                      flipRenaming(nu.getFlipped()));
    Expr refreshed = DeepRefreshed(flipped);
    EXPECT_EQ(refreshed->printLaTeX(0), metricOn(S, mu, nu)->printLaTeX(0));
}

TEST(SignedRefresh, relabelledFlippedMetricPrintsDelta)
{
    Space const &S = buildMinkowski();
    Index        mu = S.generateIndex("mu");
    Index        nu = S.generateIndex("nu");
    Expr flipped = ApplyIndexRenaming(metricOn(S, mu, nu), flipRenaming(nu));
    Expr refreshed = DeepRefreshed(RelabelIndices(flipped));
    EXPECT_EQ(printLatex(refreshed),
              printLatex(deltaOn(S, mu, nu.getFlipped())));
}
