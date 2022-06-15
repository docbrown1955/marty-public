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

/**
 * @file csldatahandler.h
 * @brief Contains the handler of CSL data for doc brown's debugger.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include "../../csl/csl.h"
#include <functional>
#include <map>
#include <vector>

namespace JSON {
class Node;
}
namespace mty::doc {

class CSLDataHandler {

  public:
    std::vector<std::shared_ptr<csl::Space>> getOwnSpaces() const
    {
        return ownSpaces;
    }

    csl::Expr read(csl::Expr const &expr, bool abbrev = false);
    void      readJSON(std::string const &fileName);

    std::vector<csl::Expr>
    readSequence(std::vector<csl::Expr>::const_iterator first,
                 std::vector<csl::Expr>::const_iterator last)
    {
        std::vector<csl::Expr> res(last - first);
        auto                   res_iter = res.begin();
        while (first != last) {
            *res_iter = read(*first, false);
            ++res_iter;
            ++first;
        }
        return res;
    }

    template <class ExprType, class Translator>
    csl::Expr read(ExprType const &expr, Translator translator)
    {
        return read(translator(expr), false);
    }

    template <class Iterator, class Translator>
    std::vector<csl::Expr>
    readSequence(Iterator first, Iterator last, Translator translator)
    {
        std::vector<csl::Expr> res(last - first);
        auto                   res_iter = res.begin();
        while (first != last) {
            *res_iter = read(*first, translator);
            ++res_iter;
            ++first;
        }
        return res;
    }

    void print(std::ostream &out = std::cout) const;
    void printCode(std::ostream &out = std::cout) const;

    void printJSON(std::string const &fileName) const;

  protected:
    void printComBlock(std::string_view com,
                       std::ostream &   out,
                       std::string_view indent) const;

    void printCSLDefinition(csl::Tensor   tensor,
                            std::ostream &out,
                            size_t        indentSize) const;

    void parseAbbreviations(csl::Expr const &expr);
    void parseTensors(csl::Expr const &expr);
    void parseLiterals(csl::Expr const &expr);
    void parseIndices(csl::Expr &expr);

    std::unique_ptr<JSON::Node>
    getSpaceList(std::vector<csl::Space const *> const &spaces) const;

    csl::Space const *treatSpace(std::string const &name, int dim);
    csl::Tensor       treatTensor(std::string const &                    name,
                                  std::vector<csl::Space const *> const &spaces,
                                  bool isComplex = false);
    csl::Expr treatLiteral(std::string const &name, bool isComplex = false);
    void      treatIndices(csl::Space const *space, size_t N);

  protected:
    std::vector<csl::Expr> expressions;

    std::vector<csl::Space const *> spaces;
    std::vector<csl::Parent>        abbreviations;
    std::vector<csl::Tensor>        tensors;
    std::vector<csl::Expr>          literal;

    std::map<csl::Space const *, size_t>     nIndices;
    std::vector<std::shared_ptr<csl::Space>> ownSpaces;
};

} // namespace mty::doc
