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
 * @file
 * @author Grégoire Uhlrich
 * @version 1.3

 * \brief
 */
#ifndef LIBRARY_EVALUATOR_H_INCLUDED
#define LIBRARY_EVALUATOR_H_INCLUDED

#include "abstract.h"
#include "index.h"
#include <string>

namespace csl {

class LibEval {

  public:
    using ID_t = long long int;
    friend class LibEvalSession;

    inline static const std::string name = "IT";

    LibEval(Expr const &t_init, ID_t t_id);

    LibEval(LibEval const &other) = default;
    LibEval &operator=(LibEval const &other) = default;

    Expr                            getInit() const;
    Expr                            getExpr() const;
    ID_t                            getID() const;
    std::vector<Expr> const &       getUnEval() const;
    std::vector<Expr> &             getUnEval();
    std::vector<csl::Tensor> const &getTensors() const;
    std::vector<csl::Tensor> &      getTensors();

    void setInit(Expr const &t_init);
    void setExpr(Expr const &t_expr);
    void setID(ID_t t_id);

    bool operator==(LibEval const &other) const;
    bool operator==(Expr const &other) const;
    bool operator!=(LibEval const &other) const;
    bool operator!=(Expr const &other) const;

    static bool isInstance(Expr const &expr);

    void replaceTensors();

    void print(std::ostream &out) const;

    void printLib(std::ostream &out,
                  int           indent            = 1,
                  bool          complexParameters = false);

    static void printExpression(std::ostream &     out,
                                Expr &             expr,
                                int                indent,
                                std::string const &begin);

    friend std::ostream &operator<<(std::ostream &out, LibEval const &eval);

  private:
    static Expr make_init(Expr const &t_init);

    static Expr make_expr(ID_t t_id);

    static bool compare_expr(Expr const &A, Expr const &B);

    static std::string idToString(ID_t id);

  private:
    Expr                             init;
    Expr                             expr;
    ID_t                             id;
    mutable std::vector<Expr>        unEval;
    mutable std::vector<csl::Tensor> tensors;
    csl::IndexStructure              indices;
};

class LibEvalSession {

  public:
    using ID_t = LibEval::ID_t;

    static inline size_t maxLeafs = 20;

    struct Perf {

        size_t               nEval;
        size_t               nOperations;
        std::vector<LibEval> evals;
    };

    LibEvalSession();

    Expr addEval(Expr init);

    std::vector<LibEval> &          getEval();
    std::vector<LibEval> const &    getEval() const;
    std::vector<Expr> const &       getUnEval() const;
    std::vector<Expr> &             getUnEval();
    std::vector<csl::Tensor> const &getTensors() const;
    std::vector<csl::Tensor> &      getTensors();

    void setEval(std::vector<LibEval> const &t_eval);
    void setEval(std::vector<LibEval> &&t_eval);

    Perf getPerf(Expr &init);

    void merge();
    void removeUnique();

    void simplify();
    void simplifyProds(size_t min_factors = 2);
    void simplifySums();

    static LibEvalSession parseExpression(Expr &expr,
                                          bool  findIntermediates = true);

    void
    parse(Expr &                                            expr,
          bool                                              findIntermediates,
          std::map<csl::AbstractParent const *, csl::Expr> &parsedAbbrevs);

    void parse(Expr &expr, bool findIntermediates = true);

    void parseProduct(
        Expr &                                            iprod,
        bool                                              findIntermediates,
        std::map<csl::AbstractParent const *, csl::Expr> &parsedAbbrevs);

    void gatherUnEvalAndTensors(Expr const &result);

    void print(std::ostream &out) const;

    void
    printLib(Expr &init, Perf &perf, std::ostream &out, bool onylDep = false);

  public:
    static csl::IndexStructure getFreeStructure(Expr const &t_init);

    static csl::IndexStructure
    getFreeStructure(csl::IndexStructure const &t_init);

  protected:
    void getPerf(LibEval const &       init,
                 std::vector<LibEval> &newEvals,
                 std::vector<int> &    nOccurences);

    static Expr expandIProd(Expr const &iprod);
    void        addUnEval(Expr const &expr) const;
    void        addTensor(csl::Tensor const &parent) const;

  public:
    bool expand = false;

  protected:
    ID_t maxID = 0;

    std::vector<LibEval> eval;

    std::map<csl::AbstractParent const *, Expr> abbreviations;

    mutable std::vector<Expr> unEval;

    mutable std::vector<csl::Tensor> tensors;
};

inline std::ostream &operator<<(std::ostream &out, LibEvalSession::Perf perf)
{
    return out << perf.nEval << " evals, " << perf.nOperations
               << " operations.\n";
}

} // End of namespace csl

#endif
