#include "bilinear.h"
#include "mrtInterface.h"

namespace mty {


    ///////////////////////////////////////////////////
    // Class Bilinear
    ///////////////////////////////////////////////////

    void Bilinear::simplify() 
    {
        applyOnAll([](IndexChain const &c) { return c.simplify(); }, m_chains);
        mergeChains(m_chains);
    }

    std::vector<IndexChain> Bilinear::diracEquation(
            GenGammaIndex    const &gamma,
            GenMomentumIndex const &slashed,
            bool                    left,
            bool                    particle
            )
    {
        csl::Expr m = (left) ? psiL.getMass() : psiR.getMass();
        if (!particle)
            m = -m;
        if (gamma.size() == 1) {
            return {IndexChain{m, {}}};
        }
        if (gamma.size() == 2) {
            bool sign = (left == (gamma[0] == slashed[0]));
            csl::Expr s = (sign) ? CSL_M_1 : CSL_1;
            csl::Index nu = (left ^ sign) ? gamma[0] : gamma[1];
            return {
                IndexChain{
                    s, 
                    {std::make_shared<GenMomentumIndex>(slashed.getP(), nu)}
                },
                IndexChain{
                    -s*m,
                    {std::make_shared<GenGammaIndex>(nu)}
                }
            };
        }
        CallHEPError(
                mty::error::NotImplementedError,
                "Dirac equation for anti-symmetric product of omre than 2 "
                "indices is not defined yet."
                )
        return {};
    }

    std::vector<IndexChain> Bilinear::applyEOM(
            IndexChain       const &chain,
            GenMomentumIndex const &slashed, 
            bool                    left
            )
    {
        std::vector<IndexChain> commuted = chain.externalize(slashed[0], left);
        auto p = slashed.getP();
        bool eomApplied = false;
        std::vector<IndexChain> finalRes;
        finalRes.reserve(commuted.size());
        for (const auto &c : commuted) {
            auto pos = (left) ? c.firstGamma() : (c.lastGamma() + 1).base();
            if (pos != c.end()) {
                bool found = false;
                for (size_t i = 0; i != c.size(); ++i) {
                    const auto &I = c[i];
                    if (I->isMomentum() 
                            && toMomentumIndex(*I).getP() == p
                            && I->hasCommonIndexWith(**pos)) {
                        std::vector<IndexChain> res = diracEquation(
                                toGammaIndex(**pos),
                                toMomentumIndex(*I),
                                left,
                                ((left) ? psiL : psiR).isParticle()
                                );
                        size_t posGamma = std::distance(c.begin(), pos);
                        if (posGamma > i)
                            --posGamma;
                        IndexChain newChain(c);
                        newChain.erase(newChain.begin() + i);
                        newChain.erase(newChain.begin() + posGamma);
                        for (auto &cNew : res) {
                            cNew.multiply(newChain.getFactor());
                            cNew.insert(
                                    cNew.begin(), 
                                    newChain.begin(),
                                    newChain.begin() + posGamma
                                    );
                            cNew.insert(
                                    cNew.end(), 
                                    newChain.begin() + posGamma,
                                    newChain.end()
                                    );
                        }
                        found = true;
                        eomApplied = true;
                        for (const auto &c : res) {
                            finalRes.push_back(c);
                        }
                    }
                }
                if (!found)
                    finalRes.push_back(c);
            }
            else
                finalRes.push_back(c);
        }
        return (eomApplied) ? finalRes : std::vector<IndexChain>{};
    }

    void Bilinear::applyEOM(bool left)
    {
        csl::Tensor p = (left) ? psiL.getPoint() : psiR.getPoint();
        std::vector<IndexChain> res;
        res.reserve(m_chains.size());
        for (const auto &c : m_chains) {
            if (auto pIndex = hasSlashed(p, c); pIndex) {
                auto interm = applyEOM(c, pIndex.value(), left);
                res.insert(
                        res.end(),
                        std::make_move_iterator(interm.begin()),
                        std::make_move_iterator(interm.end())
                        );
            }
            else
                res.push_back(c);
        }
        m_chains = std::move(res);
    }

    void Bilinear::applyEOM()
    {
        if (psiL.isOnShell())
            applyEOM(true);
        if (psiR.isOnShell())
            applyEOM(false);
        simplify();
    }

    void Bilinear::print(std::ostream &out) const 
    {
        out << "Bilinear for : ";
        out << psiL.getName() << "(" << psiL.getPoint()->getName() << ")^(*) ";
        out << psiR.getName() << "(" << psiR.getPoint()->getName() << ")";
        out << '\n';
        for (const auto &c : m_chains) {
            c.print(out);
            out << '\n';
        }
    }

    ///////////////////////////////////////////////////
    // Multilinear class
    ///////////////////////////////////////////////////

    void Multilinear::applyEOM() 
    {
        for (auto &b : bilinears)
            b.applyEOM();
    }

    void Multilinear::applyFierz(
            size_t i1,
            size_t j1,
            size_t k1,
            size_t i2,
            size_t j2,
            size_t k2
            )
    {
        Bilinear b1 = bilinears[i1];
        Bilinear b2 = bilinears[i2];
        bilinears.erase(bilinears.begin() + i1);
        bilinears.erase(bilinears.begin() + i2 - (i2 > i1));

        auto p1 = b1.psiL;
        auto p2 = b1.psiR;
        auto p3 = b2.psiL;
        auto p4 = b2.psiR;

        IndexChain chain1 = b1.chains()[j1];
        IndexChain chain2 = b2.chains()[j2];
        b1.chains().erase(b1.chains().begin() + j1);
        b2.chains().erase(b2.chains().begin() + j2);

        std::vector<std::pair<Bilinear, Bilinear>> newBilinears;
        newBilinears.reserve(5);
        for (size_t i = 0; i != 5; ++i) {
            auto [c1_A, c1_B] = cutAt(chain1, k1);
            auto [c2_A, c2_B] = cutAt(chain2, k2);

            applyChiralBasisElement(i, c1_A, c1_B);
            c1_A.insert(c1_A.end(), c2_A.begin(), c2_A.end());
            c1_B.insert(c1_B.end(), c2_B.begin(), c2_B.end());
            newBilinears.push_back({
                    Bilinear{p1, p4, {c1_A}},
                    Bilinear{p3, p2, {c1_B}}
                    });
            std::cout << "BEFORE EOM" << std::endl;
            newBilinears.back().first.print();
            newBilinears.back().second.print();
            newBilinears.back().first.applyEOM();
            newBilinears.back().second.applyEOM();
            std::cout << "AFTER EOM" << std::endl;
            newBilinears.back().first.print();
            newBilinears.back().second.print();
        }
    }

    ///////////////////////////////////////////////////
    // Functions
    ///////////////////////////////////////////////////

    std::optional<GenMomentumIndex> hasSlashed(
            csl::Tensor const &p,
            IndexChain  const &chain
            )
    {
        csl::Index mu;
        size_t posMom = -1;
        for (size_t i = 0; i != chain.size(); ++i) {
            auto const &I = chain[i];
            if (I->isMomentum() 
                    && toMomentumIndex(*I).getP().get() == p.get()) {
                mu = (*I)[0];
                posMom = i;
                break;
            }
        }
        if (posMom == static_cast<size_t>(-1))
            return std::nullopt;
        for (const auto &I : chain) {
            if (I->isGamma() && I->contains(mu))
                return toMomentumIndex(*chain[posMom]);
        }
        return std::nullopt;
    }

    std::pair<IndexChain, IndexChain> cutAt(
            IndexChain const &init,
            size_t            pos
            )
    {
        constexpr size_t npos = -1;
        size_t nGamma = 0;
        size_t posCut = npos;
        size_t lastGamma = npos;
        for (size_t i = 0; i != init.size(); ++i) {
            if (init[i]->isGamma()) {
                ++nGamma;
                lastGamma = i;
            }
            if (posCut == npos && nGamma > pos) {
                posCut = i;
            }
        }
        if (posCut == npos) {
            posCut = lastGamma + 1;
        }

        std::pair<IndexChain, IndexChain> res;
        res.first.setFactor(init.getFactor());
        for (size_t i = 0; i != init.size(); ++i) {
            if (init[i]->isGamma()) {
                if (i < posCut)
                    res.first.push_back(init[i]);
                else
                    res.second.push_back(init[i]);
            }
        }
        for (size_t i = 0; i != init.size(); ++i) {
            if (!init[i]->isGamma()) {
                bool found = false;
                for (const auto &index : res.first) {
                    if (init[i]->hasCommonIndexWith(*index)) {
                        res.first.push_back(init[i]);
                        found = true;
                        break;
                    }
                }
                if (!found)
                    res.second.push_back(init[i]);
            }
        }
        res.first.refresh();
        res.second.refresh();

        return res;
    }

    void applyChiralBasisElement(
            size_t      i,
            IndexChain &A,
            IndexChain &B
            )
    {
        csl::Index mu = mty::MinkowskiIndex();
        csl::Index nu = mty::MinkowskiIndex();
        switch(i) {
            case 0:
                A.push_back(gammaindex_s(6));
                B.push_back(gammaindex_s(6));
                break;
            case 1:
                A.push_back(gammaindex_s(7));
                B.push_back(gammaindex_s(7));
                break;
            case 2:
                A.push_back(gammaindex_s(mu));
                A.push_back(gammaindex_s(7));
                B.push_back(gammaindex_s(+mu));
                B.push_back(gammaindex_s(6));
                break;
            case 3:
                A.push_back(gammaindex_s(mu));
                A.push_back(gammaindex_s(6));
                B.push_back(gammaindex_s(+mu));
                B.push_back(gammaindex_s(7));
                break;
            case 4:
                A.multiply(CSL_1 / 4);
                A.push_back(gammaindex_s({mu, nu}));
                B.push_back(gammaindex_s({+mu, +nu}));
                break;
            default:
                CallHEPError(mty::error::IndexError, "Index " + toString(i)
                        + " is out of bounds for chiral basis.")
        }
    }

} // namespace mty
