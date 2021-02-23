#pragma once

#include "polarization.h"
#include "generalizedIndex.h"

namespace mty {

class Bilinear {

public:

    mty::PolarizationField getPsiL() const { return psiL; }
    mty::PolarizationField getPsiR() const { return psiR; }

    void setPsiL(mty::PolarizationField const &p) { psiL = p; }
    void setPsiR(mty::PolarizationField const &p) { psiR = p; }

    std::vector<IndexChain> const &chains() const { return m_chains; }
    std::vector<IndexChain>       &chains()       { return m_chains; }

    void simplify();

    std::vector<IndexChain> diracEquation(
            GenGammaIndex    const &gamma,
            GenMomentumIndex const &mom,
            bool                    left,
            bool                    particle
            );
    std::vector<IndexChain> applyEOM(
            IndexChain       const &chain,
            GenMomentumIndex const &slashed,
            bool                    left
            );
    void applyEOM(bool left);
    void applyEOM();

    void print(std::ostream &out = std::cout) const;

//private:

    mty::PolarizationField psiL;
    mty::PolarizationField psiR;
    std::vector<IndexChain> m_chains;
};

std::optional<GenMomentumIndex> hasSlashed(
        csl::Tensor const &p, 
        IndexChain  const &chain
        );

class Multilinear {

public:

    IMPLEMENTS_STD_VECTOR(Bilinear, bilinears)

    Multilinear() = default;
    Multilinear(Bilinear const &bil)
        :bilinears({bil})
    {}
    Multilinear(std::vector<Bilinear> const &t_bilinears)
        :bilinears(t_bilinears)
    {}

    void applyEOM();

    void applyFierz(
            size_t i1,
            size_t j1,
            size_t k1,
            size_t i2,
            size_t j2,
            size_t k2
            );

private:

    std::vector<Bilinear> bilinears;
};

std::pair<IndexChain, IndexChain> cutAt(
        IndexChain const &init,
        size_t pos
        );

void applyChiralBasisElement(
        size_t      i,
        IndexChain &A,
        IndexChain &B
        );

} // namespace mty
