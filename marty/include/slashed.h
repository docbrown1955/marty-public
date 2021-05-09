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

/*!
 * @file slashed.h
 * @author Grégoire Uhlrich
 * @version 1.3
 
 * \brief Contains the slashed object entering expressions.
 */
#pragma once
#include "diracology.h"

namespace mty {

class Slashed: public csl::TensorElement {

    public:

    Slashed();

    ~Slashed() override;

    bool isIndexed() const override { return true; }

    bool empty() const override {
        return argument.empty();
    }
    size_t size() const override {
        return argument.size();
    }

    csl::Expr const &getArgument(int i = 0) const override {
        return argument[i];
    }
    csl::Expr& getArgument(int i = 0) override {
        return argument[i];
    }
    csl::Expr const &operator[](int i) const override {
        return argument[i];
    }
    csl::Expr &operator[](int i) override {
        return argument[i];
    }

    std::vector<csl::Expr> const &getVectorArgument() const override {
        return argument;
    }
    void setArgument(const csl::Expr& t_argument, int iArg=0) override {
        argument[iArg] = t_argument;
    }
    void setVectorArgument(const csl::vector_expr& t_argument) override {
        argument = t_argument;
    }

    void print(int mode = 0,
               std::ostream& out = std::cout,
               bool lib = false) const override;

    std::string printLaTeX(int mode = 0) const override;

    void printLib(int mode,
                  std::ostream& out) const;

    csl::LibDependency getLibDependency() const override;

    std::optional<csl::Expr> evaluate(csl::eval::mode user_mode
            = csl::eval::base) const override;

    csl::IndexStructure getIndexStructure() const override;

    csl::unique_Expr copy_unique() const override;

    csl::Expr deepCopy() const override;

    csl::Expr deepRefresh() const override;

    bool operator==(csl::Expr_info other) const override;

    bool operator<(csl::Expr_info other) const override;

    csl::Expr getDivergentFactor() const;
        
protected:

    std::vector<csl::Expr> argument;
};

}
