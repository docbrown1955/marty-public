#pragma once

#include "multifunction.h"

namespace sgl {

    class Sum: public AbstractMultiFunction {
    public:
        using AbstractMultiFunction::AbstractMultiFunction;

        bool isZero() const override;

        GExpr copy() const override;
        GExpr refresh() const override;

        bool contains(csl::Index const &index) const override;
        void replace(csl::Index const &i, csl::Index const &j) override;

        csl::Expr toCSL(TensorSet const&) const override;

        void merge();
        void print(std::ostream &out) const override;
    };

    class Prod: public AbstractMultiFunction {
    public:
        using AbstractMultiFunction::AbstractMultiFunction;

        bool isZero() const override;

        csl::Expr getFactor() const override;
        GExpr getTerm() const override;

        GExpr copy() const override;
        GExpr refresh() const override;

        bool contains(csl::Index const &index) const override;
        void replace(csl::Index const &i, csl::Index const &j) override;

        csl::Expr toCSL(TensorSet const&) const override;

        void print(std::ostream &out) const override;

        void merge();
        void applyProperties();
        GExpr expand();
    };

    GExpr sum_s(std::vector<GExpr> args);
    GExpr prod_s(std::vector<GExpr> args);
}
