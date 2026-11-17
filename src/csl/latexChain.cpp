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

#include "latexChain.h"
#include "indicial.h"
#include "space.h"
#include <algorithm>
#include <limits>

namespace csl {

namespace {

constexpr size_t noNode = std::numeric_limits<size_t>::max();

struct ChainNode {
    Expr                expr;
    std::vector<Index>  ports; // matrix indices: 1 for vectors, 2 for matrices
    std::vector<size_t> partner;     // node at the other end of each port
    std::vector<size_t> partnerPort; // port of that node
    std::vector<Index>  extras;      // other indices, printed as superscripts
    std::vector<size_t> portPos;     // position of each port in the index list
};

struct ChainStep {
    size_t node;
    bool   transposed;
};

// Walks from start leaving through exitPort. The walk ends at a vector (open
// chain) or when it returns to start (closed trace).
std::vector<ChainStep>
walk(std::vector<ChainNode> const &nodes, size_t start, size_t exitPort)
{
    std::vector<ChainStep> seq{{start, false}};
    size_t                 cur  = start;
    size_t                 exit = exitPort;
    while (true) {
        size_t next  = nodes[cur].partner[exit];
        size_t entry = nodes[cur].partnerPort[exit];
        if (next == start) {
            seq[0].transposed = (entry == 1);
            return seq;
        }
        seq.push_back({next, entry == 1});
        if (nodes[next].ports.size() == 1)
            return seq;
        exit = 1 - entry;
        cur  = next;
    }
}

std::string latexChainFactor(ChainNode const &node, bool transposed)
{
    auto const &element = static_cast<TensorElement const &>(*node.expr);
    std::string core    = element.getLatexDisplayName();
    if (not node.extras.empty()) {
        bool isField = element.getType() == Type::TensorFieldElement;
        core += isField ? "_{" : "^{";
        for (auto const &index : node.extras)
            core += index.printLaTeX();
        core += "}";
    }
    core += element.getLatexPointSuffix();
    return transposed ? latexScriptBase(core, true) + "^{T}" : core;
}

// True if the non-commuting factors of the walk keep their order in the
// product, so that the chain multiplies them in the same order.
bool keepsOrder(std::vector<ChainNode> const &nodes,
                std::vector<ChainStep> const &seq,
                std::vector<size_t> const    &positionOf)
{
    for (size_t a = 0; a != seq.size(); ++a) {
        for (size_t b = a + 1; b != seq.size(); ++b) {
            size_t na = seq[a].node;
            size_t nb = seq[b].node;
            if (nodes[na].expr->getCommutable()
                or nodes[nb].expr->getCommutable())
                continue;
            if (positionOf[na] > positionOf[nb])
                return false;
        }
    }
    return true;
}

// The chain is printed at the position of its first factor. A non-commuting
// factor of the product lying between the chain factors would then be moved
// past them.
bool blockKeepsOrder(vector_expr const         &factors,
                     std::vector<size_t> const &positions)
{
    bool hasNonCommuting
        = std::any_of(positions.begin(), positions.end(), [&](size_t p) {
              return not factors[p]->getCommutable();
          });
    if (not hasNonCommuting)
        return true;
    for (size_t p = positions.front() + 1; p < positions.back(); ++p) {
        if (std::find(positions.begin(), positions.end(), p) == positions.end()
            and not factors[p]->getCommutable())
            return false;
    }
    return true;
}

} // namespace

std::vector<LatexMatrixChain> getMatrixChains(vector_expr const &factors)
{
    std::vector<std::vector<Index>> indexOf(factors.size());
    std::vector<ChainNode>          nodes;
    std::vector<size_t>             positionOf;
    std::vector<size_t>             nodeAt(factors.size(), noNode);
    for (size_t p = 0; p != factors.size(); ++p) {
        if (factors[p]->getType() != Type::TensorElement
            and factors[p]->getType() != Type::TensorFieldElement)
            continue;
        auto structure = factors[p]->getIndexStructure();
        indexOf[p]     = structure.getIndex();

        std::vector<Index>  ports;
        std::vector<Index>  extras;
        std::vector<size_t> portPos;
        for (size_t k = 0; k != indexOf[p].size(); ++k) {
            auto const &index = indexOf[p][k];
            auto        space = index.getSpace();
            if (space != nullptr and space->getMatrixPrinting()) {
                ports.push_back(index);
                portPos.push_back(k);
            }
            else
                extras.push_back(index);
        }
        if (ports.empty() or ports.size() > 2)
            continue;
        nodeAt[p] = nodes.size();
        positionOf.push_back(p);
        nodes.push_back({factors[p],
                         ports,
                         std::vector<size_t>(ports.size(), noNode),
                         std::vector<size_t>(ports.size(), noNode),
                         extras,
                         portPos});
    }

    // The partner of a port is the only other occurrence of its index in the
    // product (a contraction). Index equality compares space, ID and type and
    // ignores the sign, which is what a contraction needs.
    for (size_t n = 0; n != nodes.size(); ++n) {
        for (size_t port = 0; port != nodes[n].ports.size(); ++port) {
            size_t occurrences = 0;
            size_t otherPos    = noNode;
            size_t otherIndex  = noNode;
            for (size_t q = 0; q != factors.size(); ++q) {
                for (size_t r = 0; r != indexOf[q].size(); ++r) {
                    if (indexOf[q][r] == nodes[n].ports[port]
                        and not(q == positionOf[n]
                                and r == nodes[n].portPos[port])) {
                        ++occurrences;
                        otherPos   = q;
                        otherIndex = r;
                    }
                }
            }
            if (occurrences != 1 or nodeAt[otherPos] == noNode)
                continue;
            size_t      partnerNode = nodeAt[otherPos];
            auto const &partnerPos  = nodes[partnerNode].portPos;
            nodes[n].partner[port]  = partnerNode;
            nodes[n].partnerPort[port]
                = std::find(partnerPos.begin(), partnerPos.end(), otherIndex)
                  - partnerPos.begin();
        }
    }

    std::vector<LatexMatrixChain> chains;
    std::vector<bool>             visited(nodes.size(), false);
    for (size_t first = 0; first != nodes.size(); ++first) {
        if (visited[first])
            continue;

        std::vector<size_t> members{first};
        for (size_t m = 0; m != members.size(); ++m) {
            for (size_t next : nodes[members[m]].partner) {
                if (next != noNode
                    and std::find(members.begin(), members.end(), next)
                            == members.end())
                    members.push_back(next);
            }
        }
        for (auto n : members)
            visited[n] = true;

        bool complete
            = std::none_of(members.begin(), members.end(), [&](size_t n) {
                  return std::any_of(nodes[n].partner.begin(),
                                     nodes[n].partner.end(),
                                     [](size_t x) { return x == noNode; });
              });
        if (not complete)
            continue;

        // A component is either a closed trace (no vector) or an open chain
        // with exactly two vector ends.
        std::vector<size_t> vectors;
        for (auto n : members) {
            if (nodes[n].ports.size() == 1)
                vectors.push_back(n);
        }
        bool isTrace = vectors.empty();
        if (not isTrace and vectors.size() != 2)
            continue;

        auto byLatexName = [&](size_t l, size_t r) {
            return nodes[l].expr->getLatexName()
                   < nodes[r].expr->getLatexName();
        };
        std::vector<size_t> starts = isTrace ? members : vectors;
        std::sort(starts.begin(), starts.end(), byLatexName);

        // Try both orientations (and both directions of a trace) and keep the
        // one with the fewest transposed factors among the valid ones.
        std::vector<ChainStep> best;
        int                    bestTransposes = -1;
        for (auto s : starts) {
            for (size_t exitPort = 0; exitPort != nodes[s].ports.size();
                 ++exitPort) {
                auto seq = walk(nodes, s, exitPort);
                if (not keepsOrder(nodes, seq, positionOf))
                    continue;
                int transposes
                    = std::count_if(seq.begin(), seq.end(), [](ChainStep st) {
                          return st.transposed;
                      });
                if (bestTransposes < 0 or transposes < bestTransposes) {
                    best           = seq;
                    bestTransposes = transposes;
                }
            }
        }
        if (bestTransposes < 0)
            continue;

        std::vector<size_t> positions;
        for (auto n : members)
            positions.push_back(positionOf[n]);
        std::sort(positions.begin(), positions.end());
        if (not blockKeepsOrder(factors, positions))
            continue;

        std::string latex;
        if (isTrace) {
            std::string inner;
            for (auto const &step : best)
                inner += latexChainFactor(nodes[step.node], step.transposed);
            latex = "\\mathrm{Tr}\\left(" + inner + "\\right)";
        }
        else {
            for (size_t k = 0; k != best.size(); ++k) {
                auto const &element = static_cast<TensorElement const &>(
                    *nodes[best[k].node].expr);
                bool transposed = best[k].transposed;
                if (k == 0)
                    transposed = not element.isRowForMatrixRep();
                else if (k + 1 == best.size())
                    transposed = element.isRowForMatrixRep();
                latex += latexChainFactor(nodes[best[k].node], transposed);
            }
        }
        chains.push_back({positions, latex});
    }
    return chains;
}

} // namespace csl
