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

#include "interface_indicial.h"
#include "algo.h"
#include "indicial.h"
#include "space.h"

using namespace std;

namespace csl {

void AddProperty(csl::Tensor &tensor,
                 Expr const & A,
                 Expr const & B,
                 Expr const & res)
{
    tensor->addSelfContraction(A, B, res);
}

void AddProperty(csl::Tensor &            tensor,
                 std::vector<Expr> const &product,
                 Expr const &             res)
{
    tensor->addContractionProperty(product, res);
}

const csl::Space *GetSpace(csl::Tensor const &tensor, int pos)
{
    auto spaces = tensor->getSpace();
    CSL_ASSERT_SPEC(pos >= 0 and pos < int(spaces.size()),
                    CSLError::IndexError,
                    "Space in pos " + toString(pos)
                        + " out of bounds for tensor with "
                        + toString(spaces.size()) + " indices.");

    return spaces[0];
}

csl::Index GetIndex(csl::Tensor const &tensor, int pos)
{
    return GetSpace(tensor, pos)->generateIndex();
}

csl::Index GetIndex(csl::Space const *space)
{
    return space->generateIndex();
}

csl::Index GetIndex(csl::Space const &space)
{
    return space.generateIndex();
}

std::vector<csl::Index>
GetIndices(size_t N, csl::Tensor const &tensor, int pos)
{
    return GetIndices(N, GetSpace(tensor, pos));
}

std::vector<csl::Index> GetIndices(size_t N, csl::Space const *space)
{
    std::vector<csl::Index> indices(N);
    for (auto &i : indices)
        i = space->generateIndex();

    return indices;
}

std::vector<csl::Index> GetIndices(size_t N, csl::Space const &space)
{
    return GetIndices(N, &space);
}

std::vector<csl::Index> GetFullSetOfIndicesFor(csl::Tensor const &tensor)
{
    std::vector<const csl::Space *> spaces = tensor->getSpace();
    std::vector<csl::Index>         indices(spaces.size());
    for (size_t i = 0; i != indices.size(); ++i) {
        indices[i] = spaces[i]->generateIndex();
    }

    return indices;
}

csl::Tensor Unitary(std::string const &name, csl::Space const *space)
{
    csl::Tensor U     = csl::tensor_s(name, {space, space});
    csl::Tensor delta = space->getDelta();
    csl::Index  i     = space->generateIndex();
    csl::Index  j     = space->generateIndex();
    csl::Index  k     = space->generateIndex();

    U->setComplexProperty(csl::ComplexProperty::Complex);
    U->addSelfContraction(
        U({i, j}), U({k, j}), delta({i, k}), [](Expr_info A, Expr_info B) {
            return A->isComplexConjugate() xor B->isComplexConjugate();
        });
    U->addSelfContraction(
        U({j, i}), U({j, k}), delta({i, k}), [](Expr_info A, Expr_info B) {
            return A->isComplexConjugate() xor B->isComplexConjugate();
        });

    return U;
}

bool TestIndexSanity(Expr const &expr, bool verbose)
{
    bool insane = csl::AnyOfNodes(expr, [&](Expr const &expr) {
        if (expr->getType() != csl::Type::Prod
            and expr->getType() != csl::Type::Sum)
            return false;
        if (expr->getType() == csl::Type::Sum) {
            for (size_t i = 0; i != expr->size(); ++i)
                for (size_t j = i + 1; j < expr->size(); ++j)
                    if (expr[i]->getIndexStructure()
                        != expr[j]->getIndexStructure()) {
                        if (verbose) {
                            std::cout << "Bad index sum for terms: "
                                      << std::endl;
                            std::cout << expr[i] << std::endl;
                            std::cout << expr[j] << std::endl;
                        }
                        return true;
                    }
            return false;
        }
        csl::IndexStructure structure;
        for (const auto &arg : expr)
            structure += arg->getIndexStructure();
        for (size_t i = 0; i != structure.size(); ++i) {
            bool found = false;
            for (size_t j = i + 1; j < structure.size(); ++j) {
                if (structure[i] == structure[j]) {
                    if (!found)
                        found = true;
                    else {
                        if (verbose) {
                            std::cout << "Index " << structure[i]
                                      << " appearing at least 3 times"
                                      << std::endl;
                        }
                        return true;
                    }
                }
            }
        }
        return false;
    });

    return not insane;
}

} // namespace csl
