#pragma once

namespace sgl {

    class GExpr;
    class IndexChain;

    void fixFierz(GExpr &expr);
    bool areCanonical(IndexChain const &A, IndexChain const &B);
    void applyGeneralFierz(GExpr &expr, bool twice = false);
    void applyGeneralFierzTwice(GExpr &expr);
}

