#pragma once

#include "csl.h"

namespace mty {

    class GeneralizedIndexBase;
    class IndexChain;
    using GenIndex = std::shared_ptr<GeneralizedIndexBase>;

    class GeneralizedIndexBase {

    public:

        IMPLEMENTS_STD_VECTOR(csl::Index, indices)

        GeneralizedIndexBase() = default;
        GeneralizedIndexBase(csl::Index const &t_index);
        GeneralizedIndexBase(
                std::vector<csl::Index> const &t_indices
                );

        virtual ~GeneralizedIndexBase() = default;

        bool contains(csl::Index const &I) const;

        bool hasCommonIndexWith(GeneralizedIndexBase const &other) const;

        csl::Index getCommonIndexWith(GeneralizedIndexBase const &other) const;

        virtual
        std::optional<csl::Expr> getValue() const {
            return std::nullopt;
        }

        virtual 
        bool hasContractionPropertyWith(
                GeneralizedIndexBase const &other,
                bool                        consecutive
                ) const = 0;

        virtual 
        std::vector<IndexChain> contraction(
                GeneralizedIndexBase const &other
                ) const;

        virtual bool isGamma()    const { return false; }
        virtual bool isMetric()   const { return false; }
        virtual bool isEpsilon()  const { return false; }
        virtual bool isMomentum() const { return false; }

        virtual GenIndex copy() const = 0;

        virtual bool commutesWith(
                GeneralizedIndexBase const &
                ) const 
        {
            return true;
        }

        bool compare(GeneralizedIndexBase const &other) const;

        int permuteIndices();

        virtual csl::Expr orderIndices() { return CSL_1; };

        virtual std::vector<IndexChain> swap(
                GeneralizedIndexBase const &other
                ) const;

        virtual void print(std::ostream &out) const = 0;

    protected:

        std::vector<csl::Index> indices;
    };

    class GenGammaIndex;
    class GenMetricIndex;
    class GenEpsilonIndex;
    struct ChainExpression;

    class GenGammaIndex: public GeneralizedIndexBase {
    public:
        using GeneralizedIndexBase::GeneralizedIndexBase;

        bool hasContractionPropertyWith(
                GeneralizedIndexBase const &other,
                bool                        consecutive
                ) const override;

        std::vector<IndexChain> contraction(
                GeneralizedIndexBase const &other
                ) const override;

        std::optional<csl::Expr> getValue() const override;

        bool isGamma() const override { 
            return true; 
        }

        GenIndex copy() const override {
            return std::make_shared<GenGammaIndex>(*this);
        }

        bool commutesWith(
                GeneralizedIndexBase const &other
                ) const override;

        csl::Expr orderIndices() override;

        std::vector<IndexChain> swap(GenGammaIndex const &other) const;
        std::vector<IndexChain> swap(
                GeneralizedIndexBase const &other
                ) const override;

        bool isGammaMu() const;
        bool isSigma() const;
        bool isGamma5() const;
        bool isProjector() const;
        bool isP_L() const;
        bool isP_R() const;

        void print(std::ostream &out) const override;
    };

    class GenMetricIndex: public GeneralizedIndexBase {
    public:
        using GeneralizedIndexBase::GeneralizedIndexBase;

        bool hasContractionPropertyWith(
                GeneralizedIndexBase const &other,
                bool                        consecutive
                ) const override;

        std::optional<csl::Expr> getValue() const override;

        std::vector<IndexChain> contraction(
                GeneralizedIndexBase const &other
                ) const override;
            
        csl::Expr orderIndices() override;

        bool isMetric() const override { 
            return true; 
        }

        GenIndex copy() const override {
            return std::make_shared<GenMetricIndex>(*this);
        }

        void print(std::ostream &out) const override;
    };

    class GenEpsilonIndex: public GeneralizedIndexBase {
    public:
        using GeneralizedIndexBase::GeneralizedIndexBase;

        bool hasContractionPropertyWith(
                GeneralizedIndexBase const &other,
                bool                        consecutive
                ) const override;

        std::optional<csl::Expr> getValue() const override;

        bool isEpsilon() const override { 
            return true; 
        }

        csl::Expr orderIndices() override;

        GenIndex copy() const override {
            return std::make_shared<GenEpsilonIndex>(*this);
        }

        void print(std::ostream &out) const override;
    };

    class GenMomentumIndex: public GeneralizedIndexBase {
    public:
        template<class ...Args>
            GenMomentumIndex(
                    csl::Tensor const &t_p,
                    Args &&...args
                    )
            :GeneralizedIndexBase(std::forward<Args>(args)...),
            p(t_p)
        {}

        bool hasContractionPropertyWith(
                GeneralizedIndexBase const &other,
                bool                        consecutive
                ) const override;

        std::vector<IndexChain> contraction(
                GeneralizedIndexBase const &other
                ) const override;

        csl::Tensor getP() const { return p; }

        bool isMomentum() const override { 
            return true; 
        }

        GenIndex copy() const override {
            return std::make_shared<GenMomentumIndex>(*this);
        }

        void print(std::ostream &out) const override;

    protected:

        mutable csl::Tensor p;
    };

    class IndexChain {

    public:

        using Action = std::function<
            std::vector<IndexChain>(IndexChain const&)
            >;
        IMPLEMENTS_STD_VECTOR(GenIndex, indices);

        IndexChain();
        IndexChain(
                csl::Expr             const &t_factor,
                std::vector<GenIndex> const &indices = std::vector<GenIndex>{}
                );
        IndexChain(std::vector<GenIndex> const &indices);

        csl::Expr getFactor() const { return factor; }

        void setFactor(csl::Expr const &t_factor) { factor = t_factor; }

        bool isZero() const { return factor == CSL_0; }

        bool isCycle() const { return cycle; }

        void setCycle(bool t_cycle) { cycle = t_cycle; }

        void multiply(csl::Expr const &t_factor);

        void sum(csl::Expr const &t_factor);

        void sort();

        void refresh();

        bool isProportionnalTo(IndexChain const &other) const;

        std::vector<IndexChain> simplify_impl() const;
        std::vector<IndexChain> simplify() const;

        iterator       firstGamma();
        const_iterator firstGamma() const;
        reverse_iterator       lastGamma();
        const_reverse_iterator lastGamma() const;

        void reserve(size_t sz) { indices.reserve(sz); }

        void push_front(GenIndex const &index);

        void merge(
                iterator                     pos,
                std::move_iterator<iterator> first,
                std::move_iterator<iterator> last
                );
        void merge(
                std::move_iterator<iterator> first,
                std::move_iterator<iterator> end
                );
        void merge(
                iterator       pos,
                const_iterator first,
                const_iterator end
                );
        void merge(
                const_iterator first,
                const_iterator end
                );
        void merge(IndexChain &&other);
        void merge(
                iterator     iter,
                IndexChain &&other
                );

        void merge(IndexChain const &other);
        void merge(
                iterator          iter,
                IndexChain const &other
                );

        std::vector<IndexChain> recursiveAction(
                Action const &f
                ) const;

        std::vector<IndexChain> externalize_impl(
                csl::Index const &index,
                bool              left
                ) const;

        std::vector<IndexChain> externalize(
                csl::Index const &index,
                bool              left
                ) const;

        std::vector<IndexChain> contractDistant(
                csl::Index const &index
                ) const;

        std::vector<IndexChain> contractDistant_impl(
                csl::Index const &index
                ) const;

        std::vector<IndexChain> applyStandardRecursion() const;
        std::vector<IndexChain> applyChiralRecursion() const;

        std::vector<IndexChain> calculateTrace_impl() const;

        std::vector<IndexChain> calculateTrace() const;

        std::vector<IndexChain> contractDistant() const;

        std::vector<IndexChain> applyContractionProperties_impl() const;

        std::vector<IndexChain> applyContractionProperties() const;

        IndexChain copy() const;

        void print(std::ostream &out) const;

        bool compare(IndexChain const &other) const;

    private:

        csl::Expr factor;
        std::vector<GenIndex> indices;
        bool cycle = false;
    };

    void applyOnAll(
            std::function<std::vector<IndexChain>(IndexChain const&)> const &f,
            std::vector<IndexChain> &chains
            );

    void mergeChains(std::vector<IndexChain> &chains);

    IndexChain::Action mergeActions(std::vector<IndexChain::Action> const &f);

    std::optional<std::pair<csl::Index, csl::Index>> diracStructure(
            csl::Expr const &tensor
            );

    void sortChain(
            std::vector<csl::Expr> &args,
            std::vector<std::pair<csl::Index, csl::Index>> &indices
            );

    bool compare(GenIndex const &I, GenIndex const &J);

    std::pair<csl::Expr, GenIndex> exprToIndex(csl::Expr const &tensor);

    IndexChain productToIndexChain(csl::Expr const &prod);

    struct ChainExpression {

        IMPLEMENTS_STD_VECTOR(IndexChain, chains);
        std::vector<IndexChain> chains;
    };

    inline std::shared_ptr<GenGammaIndex> gammaindex_s(
            csl::Index const &index
            )
    {
        return std::make_shared<GenGammaIndex>(index);
    }
    inline std::shared_ptr<GenGammaIndex> gammaindex_s(
            std::vector<csl::Index> const &indices
            )
    {
        return std::make_shared<GenGammaIndex>(indices);
    }
    inline std::shared_ptr<GenEpsilonIndex> epsilonindex_s(
            std::vector<csl::Index> const &indices
            )
    {
        return std::make_shared<GenEpsilonIndex>(indices);
    }
    inline std::shared_ptr<GenMetricIndex> metricindex_s(
            std::vector<csl::Index> const &indices
            )
    {
        return std::make_shared<GenMetricIndex>(indices);
    }
    inline std::shared_ptr<GenMomentumIndex> momentumindex_s(
            csl::Tensor const &p,
            csl::Index  const &index
            )
    {
        return std::make_shared<GenMomentumIndex>(p, index);
    }

    inline GenGammaIndex const &toGammaIndex(
            GeneralizedIndexBase const &I
            ) 
    {
        return *dynamic_cast<GenGammaIndex const*>(&I);
    }
    inline GenMetricIndex const &toMetricIndex(
            GeneralizedIndexBase const &I
            ) 
    {
        return *dynamic_cast<GenMetricIndex const*>(&I);
    }
    inline GenEpsilonIndex const &toEpsilonIndex(
            GeneralizedIndexBase const &I
            ) 
    {
        return *dynamic_cast<GenEpsilonIndex const*>(&I);
    }
    inline GenMomentumIndex const &toMomentumIndex(
            GeneralizedIndexBase const &I
            ) 
    {
        return *dynamic_cast<GenMomentumIndex const*>(&I);
    }
} // namespace mty
