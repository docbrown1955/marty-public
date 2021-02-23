#pragma once

#include "csl.h"
#include <map>
#include <vector>
#include <functional>

namespace JSON {
    class Node;
}
namespace mty::doc {

class CSLDataHandler {

public:

    std::vector<std::shared_ptr<csl::Space>> getOwnSpaces() const {
        return ownSpaces;
    }

    csl::Expr read(
            csl::Expr const &expr, 
            bool             abbrev = false
            );
    void readJSON(std::string const &fileName);

    std::vector<csl::Expr> readSequence(
            std::vector<csl::Expr>::const_iterator first,
            std::vector<csl::Expr>::const_iterator last
            )
    {
        std::vector<csl::Expr> res(last - first);
        auto res_iter = res.begin();
        while (first != last) {
            *res_iter = read(*first, false);
            ++res_iter;
            ++first;
        }
        return res;
    }

    template<class ExprType, class Translator>
    csl::Expr read(
            ExprType const &expr,
            Translator      translator
            )
    {
        return read(translator(expr), false);
    }

    template<class Iterator, class Translator>
    std::vector<csl::Expr> readSequence(
            Iterator   first,
            Iterator   last,
            Translator translator
            )
    {
        std::vector<csl::Expr> res(last - first);
        auto res_iter = res.begin();
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

    void printComBlock(
            std::string_view com,
            std::ostream    &out,
            std::string_view indent
            ) const;

    void printCSLDefinition(
            csl::Tensor  tensor,
            std::ostream &out,
            size_t        indentSize
            ) const;

    void parseAbbreviations(csl::Expr const &expr);
    void parseTensors(csl::Expr const &expr);
    void parseLiterals(csl::Expr const &expr);
    void parseIndices(csl::Expr &expr);

    std::unique_ptr<JSON::Node> getSpaceList(
            std::vector<csl::Space const*> const &spaces
            ) const;

    csl::Space const *treatSpace(
            std::string const &name,
            int                dim
            );
    csl::Tensor treatTensor(
            std::string const &name,
            std::vector<csl::Space const*> const &spaces,
            bool isComplex = false
            );
    csl::Expr treatLiteral(
            std::string const &name,
            bool isComplex = false
            );
    void treatIndices(
            csl::Space const *space,
            size_t            N
            );
    
protected:

    std::vector<csl::Expr> expressions;

    std::vector<csl::Space const*> spaces;
    std::vector<csl::Parent>       abbreviations;
    std::vector<csl::Tensor>       tensors;
    std::vector<csl::Expr>         literal;      

    std::map<csl::Space const*, size_t> nIndices;
    std::vector<std::shared_ptr<csl::Space>> ownSpaces;
};


}
