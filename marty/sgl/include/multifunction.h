#pragma once

#include "abstractgammasym.h"

namespace sgl {

    class AbstractMultiFunction: public AbstractGammaSym {

    public:

        AbstractMultiFunction() = default;
        AbstractMultiFunction(GExpr const &arg);
        AbstractMultiFunction(std::vector<GExpr> const &arg);

        bool contains(csl::Index const &index) const override;

        size_t size() const override;
        GExpr const &argument(size_t i) const override;
        GExpr &argument(size_t i) override;
        iterator begin() override;
        const_iterator begin() const override;
        iterator end() override;
        const_iterator end() const override;

        void printArgs(std::ostream &out, char sep) const;

    protected:

        std::vector<GExpr> m_argument;
    };

    class AbstractLiteral: public AbstractGammaSym {

    public:

        size_t size() const override;
        GExpr const &argument(size_t i) const override;
        GExpr &argument(size_t i) override;
        iterator begin() override;
        const_iterator begin() const override;
        iterator end() override;
        const_iterator end() const override;
    };

} // namesapce sgl
