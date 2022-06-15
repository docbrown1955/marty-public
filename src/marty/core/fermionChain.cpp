// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "fermionChain.h"
#include "amplitude.h"
#include "diracology.h"
#include "mrtError.h"
#include "mrtOptions.h"
#include "mrtUtils.h"

using namespace csl;

namespace mty {

FermionChain::FermionChain(const csl::Space *t_diracSpace)
    : diracSpace(dynamic_cast<const DiracSpace *>(t_diracSpace))
{
    // HEPAssert(bool(diracSpace),
    //         mty::error::TypeError,
    //         "DiracSpace at adress " + toString(t_diracSpace) + " is invalid,
    //         "
    //         + std::string(" either because null or is not a DiracSpace.")).
}

bool FermionChain::applyOn(csl::Expr &prod, bool makeAbbreviation) const
{
    std::vector<Chain> chains = getChains(prod, diracSpace);
    if (chains.empty())
        return false;
    bool allGamma = true;
    for (const auto &chain : chains) {
        for (size_t pos : chain.pos) {
            if (not diracSpace->isGammaTensor(prod[pos])) {
                allGamma = false;
                break;
            }
        }
        if (not allGamma)
            break;
    }
    if (allGamma and !chains.empty()) {
        csl::Expr              cpyProd = csl::Copy(prod);
        std::vector<csl::Expr> chainTensors;
        chainTensors.reserve(2 * chains.size());
        for (const auto &chain : chains)
            for (size_t pos : chain.pos) {
                csl::Expr &arg = (*prod)[pos];
                chainTensors.push_back(arg);
                arg = CSL_1;
            }
        chainTensors = diracSpace->simplifyChain(chainTensors);
        if (makeAbbreviation and mty::option::applyFermionChain)
            for (auto &chain : chainTensors) {
                if (csl::AnyOfLeafs(chain, [&](csl::Expr const &sub) {
                        return diracSpace->isGammaTensor(sub);
                    }))
                    chain = chain->getNumericalFactor()
                            * csl::Abbrev::makeAbbreviation(
                                "Fc", csl::Refreshed(csl::GetTerm(chain)));
            }
        prod[chains[0].pos[0]] = csl::sum_s(chainTensors);
        csl::Refresh(prod);
        return !prod->compareWithDummy(cpyProd.get());
    }
    for (const auto &chain : chains) {
        std::vector<csl::Expr> chainTensors;
        chainTensors.reserve(chain.pos.size());
        bool allGamma = true;
        for (size_t pos : chain.pos) {
            csl::Expr &arg = (*prod)[pos];
            if (not diracSpace->isGammaTensor(arg))
                allGamma = false;
            chainTensors.push_back(arg);
            arg = CSL_1;
        }
        if (chain.cycle) {
            for (size_t i = 0; i != chainTensors.size(); ++i) {
                if (chainTensors[i]->getType() == csl::Type::TensorElement
                    and diracSpace->isDelta(chainTensors[i])) {
                    for (size_t j = 0; j != chainTensors.size(); ++j) {
                        if (i != j)
                            csl::Replace(
                                chainTensors[j],
                                chainTensors[i]->getIndexStructureView()[0],
                                chainTensors[i]->getIndexStructureView()[1]);
                    }
                    chainTensors.erase(chainTensors.begin() + i);
                }
            }
            csl::Expr trace       = diracSpace->calculateTrace(chainTensors);
            (*prod)[chain.pos[0]] = trace;
            continue;
        }
        if (allGamma) {
            chainTensors = diracSpace->simplifyChain(chainTensors);
            if (makeAbbreviation and mty::option::applyFermionChain)
                for (auto &chain : chainTensors) {
                    if (csl::AnyOfLeafs(chain, [&](csl::Expr const &sub) {
                            return diracSpace->isGammaTensor(sub);
                        }))
                        chain = chain->getNumericalFactor()
                                * csl::Abbrev::makeAbbreviation(
                                    "Fc", csl::Refreshed(csl::GetTerm(chain)));
                }
            prod[chain.pos[0]] = sum_s(chainTensors);
            continue;
        }
        csl::Expr factor = CSL_1;
        if (chainTensors.size() > 1 and not chainTensors[0]->isIndexed()) {
            factor = chainTensors[0];
            chainTensors.erase(chainTensors.begin());
        }
        if (chainTensors.size() > 0) {
            if (not makeAbbreviation) {
                (*prod)[chain.pos[0]] = factor * prod_s(chainTensors);
            }
            else if (mty::option::applyFermionChain) {
                (*prod)[chain.pos[0]] = factor
                                        * csl::Abbrev::makeAbbreviation(
                                            "Fc", prod_s(chainTensors));
            }
            else
                (*prod)[chain.pos[0]] = factor * prod_s(chainTensors);
        }
        else
            (*prod)[chain.pos[0]] = factor * prod_s(chainTensors);
    }
    csl::Refresh(prod);

    return true;
}

std::vector<FermionChain::Chain> FermionChain::getChains(
    csl::Expr const &prod, DiracSpace const *diracSpace, bool keepBorders)
{
    if (prod->size() == 0)
        return std::vector<Chain>();
    HEPAssert(prod->getType() == csl::Type::Prod,
              mty::error::TypeError,
              "FermionChain may apply on a product only, " + toString(prod)
                  + " given.");
    std::vector<csl::IndexStructure> indices(prod->size());
    for (size_t i = 0; i != prod->size(); ++i)
        if ((*prod)[i]->isIndexed()
            and (*prod)[i]->getType() != csl::Type::VectorIntegral
            and diracSpace->isGammaTensor((*prod)[i])) {
            indices[i] = (*prod)[i]->getIndexStructure();
            for (size_t j = 0; j != indices[i].size(); ++j)
                if (indices[i][j].getSpace() != diracSpace) {
                    indices[i].erase(indices[i].begin() + j);
                    --j;
                }
            if (indices[i].size() != 2 and not keepBorders)
                indices[i].clear();
        }
    std::vector<Chain> chains;
    for (size_t i = 0; i != indices.size(); ++i) {
        if (indices[i].empty())
            continue;
        csl::IndexStructure toSearch = indices[i];
        indices[i].clear();
        chains.push_back({{i}, false});
        for (size_t j = i + 1; j != indices.size(); ++j)
            if (i != j and not indices[j].empty()) {
                bool found = false;
                for (size_t k = 0; k != indices[j].size(); ++k)
                    for (size_t l = 0; l != toSearch.size(); ++l)
                        if (indices[j][k] == toSearch[l]) {
                            toSearch.erase(toSearch.begin() + l);
                            indices[j].erase(indices[j].begin() + k);
                            --k;
                            found = true;
                            break;
                        }
                if (found) {
                    chains.back().pos.push_back(j);
                    while (not indices[j].empty()) {
                        toSearch += indices[j][0];
                        indices[j].erase(indices[j].begin());
                    }
                    if (toSearch.empty()) {
                        chains.back().cycle = true;
                        break;
                    }
                    j = 0;
                }
            }
    }
    return chains;
}

std::vector<csl::Expr> &FermionChain::getTensorsInChain()
{
    return tensors;
}

std::vector<csl::Expr> const &FermionChain::getTensorsInChain() const
{
    return tensors;
}

} // namespace mty
