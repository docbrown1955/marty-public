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

#include "libraryevaluator.h"
#include "abreviation.h"
#include "algo.h"
#include "error.h"
#include "hardFactor.h"
#include "interface.h"
#include "libraryexpander.h"
#include "libraryfunction.h"
#include "librarygenerator.h"
#include "literal.h"
#include "progressBar.h"
#include "replace.h"
#include "scopedProperty.h"
#include "utils.h"
#include <cmath>

namespace csl {

///////////////////////////////////////////////////
/*************************************************/
// Class LibEval                                 //
/*************************************************/
///////////////////////////////////////////////////

LibEval::LibEval(Expr const &t_init, ID_t t_id)
    : init(make_init(t_init)), expr(make_expr(t_id)), id(t_id)
{
}

Expr LibEval::getInit() const
{
    return init;
}

Expr LibEval::getExpr() const
{
    return expr;
}

LibEval::ID_t LibEval::getID() const
{
    return id;
}

std::vector<Expr> const &LibEval::getUnEval() const
{
    return unEval;
}

std::vector<Expr> &LibEval::getUnEval()
{
    return unEval;
}

std::vector<csl::Tensor> const &LibEval::getTensors() const
{
    return tensors;
}

std::vector<csl::Tensor> &LibEval::getTensors()
{
    return tensors;
}

void LibEval::setInit(Expr const &t_init)
{
    init = make_init(t_init);
}

void LibEval::setExpr(Expr const &t_expr)
{
    expr = t_expr;
}

bool LibEval::operator==(LibEval const &other) const
{
    return id == other.id;
}

bool LibEval::operator==(Expr const &other) const
{
    return compare_expr(init, other);
}

bool LibEval::operator!=(LibEval const &other) const
{
    return !(*this == other);
}

bool LibEval::operator!=(Expr const &other) const
{
    return !(*this == other);
}

bool LibEval::isInstance(Expr const &expr)
{
    std::string_view exprName = expr->getName();
    if (exprName.size() < LibEval::name.size() + 1)
        return false;
    for (size_t i = 0; i != LibEval::name.size(); ++i)
        if (LibEval::name[i] != exprName[i])
            return false;
    return true;
}

Expr LibEval::make_init(Expr const &t_init)
{
    return csl::DeepRefreshed(t_init);
}

Expr LibEval::make_expr(ID_t t_id)
{
    return csl::constant_s(name + "_" + idToString(t_id),
                           csl::ComplexProperty::Complex);
}

bool LibEval::compare_expr(Expr const &A, Expr const &B)
{
    auto correctType = [](Expr const &expr) {
        auto type = expr->getType();
        return type == csl::Type::Sum or type == csl::Type::Prod;
    };
    // auto hasNumericalFactor = [](Expr const &expr) {
    //     return !(expr->size() == 0
    //             || (csl::IsProd(expr) && !csl::IsNumerical(expr[0]))
    //             );
    // };
    if (A->size() == 0 && B->size() == 0)
        return A == B;
    const Expr &termA = A; //(!hasNumericalFactor(A)) ? A : csl::GetTerm(A);
    const Expr &termB = B; //(!hasNumericalFactor(B)) ? B : csl::GetTerm(B);
    if (!A->isIndexed() or not correctType(A))
        return termA == termB;

    return termA->compareWithDummy(termB.get());
}

std::string LibEval::idToString(ID_t id)
{
    static const int nDigits    = 4;
    int              nDigits_id = (id == 0) ? 1 : 1 + std::log10(id);
    std::string      res;
    res.reserve(nDigits);
    for (int i = 0; i < nDigits - nDigits_id; ++i)
        res += '0';
    res += toString(id);
    return res;
}

csl::IndexStructure getFullStructure(Expr const &expr)
{
    csl::IndexStructure res;
    csl::VisitEachLeaf(expr, [&](Expr const &sub) {
        if (not csl::IsIndicialTensor(sub))
            return;
        csl::IndexStructure const &structure = sub->getIndexStructureView();
        for (const auto &index : structure) {
            size_t match = 0;
            for (const auto &otherIndex : res)
                if (index == otherIndex)
                    ++match;
            if (match < 2)
                res += index;
        }
    });
    return res;
}

void LibEval::replaceTensors()
{
    if (!indices.empty())
        return;
    auto transfoTensor = [&](Expr &tensor) {
        tensors.push_back(
            std::dynamic_pointer_cast<csl::TensorParent>(tensor->getParent()));
        std::string name = LibraryGenerator::regularName(tensor->getName());
        if (tensor->getParent()->getType() == cslParent::Metric
            or tensor->getParent()->getType() == cslParent::Delta
            or tensor->getParent()->getType() == cslParent::Epsilon)
            name += "_"
                    + LibraryGenerator::regularName(
                        tensor->getParent()->getSpace()[0]->getName());
        name += "[{";
        csl::IndexStructure structure = tensor->getIndexStructure();
        bool                first     = true;
        for (size_t i = 0; i != structure.size(); ++i) {
            if (!first)
                name += ", ";
            if (structure[i].getType() != cslIndex::Fixed)
                name += structure[i].getName();
            else
                name += toString(std::abs(structure[i].getValue()));
            first = false;
        }
        name += "}]";
        tensor = csl::constant_s(name);
    };
    std::array<std::string, 9> nameIndices
        = {"i", "j", "k", "l", "m", "n", "o", "p", "q"};
    size_t index        = 0;
    size_t pos          = size_t(-1);
    auto   getIndexName = [&]() {
        if (++pos == nameIndices.size()) {
            pos = 0;
            ++index;
        }
        if (index == 0)
            return nameIndices[pos];
        return nameIndices[pos] + "_" + toString(index);
    };
    // indices = init->getIndexStructure();
    indices = getFullStructure(init);
    csl::IndexStructure metricIndices;
    for (size_t i = 0; i != indices.size(); ++i) {
        for (size_t j = i + 1; j < indices.size(); ++j) {
            if (indices[i] != indices[j])
                continue;
            if (indices[i].getSpace()->getSignedIndex()) {
                indices[i].setName(getIndexName());
                indices[i].setID(0);
                Replace(init, indices[j], indices[i]);
                csl::Index newIndex(indices[j]);
                newIndex.setName(getIndexName());
                newIndex.setID(0);
                Replace(init, indices[j].getFlipped(), newIndex);
                indices[j] = newIndex;
                indices[i].setSign(true);
                indices[j].setSign(true);
                metricIndices += indices[i];
                metricIndices += indices[j];
            }
            else {
                indices[i].setName(getIndexName());
                indices[i].setID(0);
                Replace(init, indices[j], indices[i]);
                indices.erase(indices.begin() + j);
                break;
            }
        }
    }
    csl::Transform(init, [&](Expr &el) {
        if (el == CSL_I) {
            el = LibraryGenerator::imaginary;
            return false;
        }
        if (not IsIndicialTensor(el)) {
            if (el->getPrimaryType() == csl::PrimaryType::Literal
                and not isInstance(el))
                unEval.push_back(el);
            return false;
        }
        transfoTensor(el);
        return true;
    });
    std::vector<Expr> args = (init->getType() == csl::Type::Prod)
                                 ? init->getVectorArgument()
                                 : std::vector<Expr>{init};
    for (size_t i = 0; i != metricIndices.size(); i += 2) {
        auto g      = metricIndices[i].getSpace()->getMetric();
        Expr g_expr = g({metricIndices[i], metricIndices[i + 1]});
        transfoTensor(g_expr);
        args.push_back(g_expr);
    }
    init = csl::prod_s(args, true);
}

void LibEval::print(std::ostream &out) const
{
    out << expr << " = " << init;
}

void LibEval::printLib(std::ostream &out,
                       LibraryMode   libMode,
                       int           indent,
                       bool)
{
    std::string type;
    if (libMode == LibraryMode::CppLib)
<<<<<<< HEAD:src/csl/libraryevaluator.cpp
        type = (expr->isReal()) ? LibraryGenerator::realUsing
                                : LibraryGenerator::complexUsing;
    else
        type = (expr->isReal()) ? LibraryGenerator::crealUsing
                                : LibraryGenerator::ccomplexUsing;
    if (indices.size() == 0 and not init->isIndexed()) {
        printExpression(out,
                        init,
                        libMode,
                        indent,
                        "const " + type + " " + toString(expr) + " = ");
=======
        type = (expr->isReal()) ?  LibraryGenerator::realUsing : LibraryGenerator::complexUsing;
    else
        type = (expr->isReal()) ?  LibraryGenerator::crealUsing : LibraryGenerator::ccomplexUsing;
    if (indices.size() == 0 and not init->isIndexed()) {
        printExpression(
                out,
                init,
                libMode,
                indent,
                "const " + type + " " + toString(expr) + " = ");
>>>>>>> Full C libraries working:csl/src/libraryevaluator.cpp
        return;
    }

    CSL_ASSERT_SPEC(init->getType() == csl::Type::Prod,
                    CSLError::TypeError,
                    "Type " + toString(init->getType())
                        + " not recognized in "
                          "library printing of indicial expression.");

<<<<<<< HEAD:src/csl/libraryevaluator.cpp
    out << LibraryGenerator::indent(indent) << type << ' ' << expr
        << " = 0;\n";
=======
    out << LibraryGenerator::indent(indent) << type << ' ' << expr << " = 0;\n";
>>>>>>> Full C libraries working:csl/src/libraryevaluator.cpp
    replaceTensors();
    std::vector<std::string> indexNames(indices.size());
    for (size_t i = 0; i != indexNames.size(); ++i) {
        indexNames[i] = toString(indices[i].getName());
        out << LibraryGenerator::indent(indent) << "for (size_t "
            << indexNames[i] << " = 0; " << indexNames[i]
            << " != " << indices[i].getSpace()->getDim() << "; ++"
            << indexNames[i] << ") {\n";
    }
    printExpression(out, init, libMode, indent + 1, toString(expr) + " += ");
    out << LibraryGenerator::indent(indent);
    for (size_t i = 0; i != indexNames.size(); ++i) {
        out << "}";
    }
    out << '\n';
}

bool isTensorName(std::string_view name)
{
    bool c1 = false;
    for (size_t i = 0; i < name.size(); ++i) {
        if (!c1 and i < name.size() - 1 and name[i] == '['
            and name[i + 1] == '{')
            c1 = true;
        if (c1 and i < name.size() - 1 and name[i] == '}'
            and name[i + 1] == ']')
            return true;
    }
    return false;
}

<<<<<<< HEAD:src/csl/libraryevaluator.cpp
void LibEval::printExpression(std::ostream      &out,
                              Expr              &expression,
                              LibraryMode        libMode,
                              int                indent,
                              std::string const &beginning,
                              std::string const &end)
=======
void LibEval::printExpression(
        std::ostream      &out,
        Expr              &expression,
        LibraryMode        libMode,
        int                indent,
        std::string const &beginning,
        std::string const &end
        )
>>>>>>> Full C/C++ library generation:csl/src/libraryevaluator.cpp
{
    csl::Transform(expression, [&](Expr &sub) {
        const auto name    = sub->getName();
        const auto regName = LibraryGenerator::regularName(name);
        if (name != regName and !isTensorName(name)) {
            const bool conjug = sub->isComplexConjugate();
            sub = csl::constant_s(regName, csl::ComplexProperty::Complex);
            if (conjug)
                sub = csl::GetComplexConjugate(sub);
            return true;
        }
        return false;
    });
    std::array<char, 7> delimiters = {'+', '*', '-', '/', '(', ')', ' '};
    std::ostringstream  sout;
    sout.precision(LibraryGenerator::nDigits);
<<<<<<< HEAD:src/csl/libraryevaluator.cpp
    Expr symb_i = (libMode == LibraryMode::CppLib) ? csl::constant_s("_i_")
                                                   : csl::constant_s("_Complex"
                                                                     "_I");
    csl::ForEachLeaf(expression, [&](Expr &el) {
=======
    Expr symb_i = (libMode == LibraryMode::CppLib) ?
        csl::constant_s("_i_")
        : csl::constant_s("_Complex_I");
    csl::ForEachLeaf(expression, [&](Expr &el)
    {
>>>>>>> Full C/C++ library generation:csl/src/libraryevaluator.cpp
        if (el == CSL_I)
            el = symb_i;
    });
    expression->print(1, sout, libMode);
    std::vector<std::string> lines;
    std::string              str = sout.str();

    size_t base = 0;
    size_t max  = 79 - LibraryGenerator::nSpaceIndent * indent;
    for (size_t i = max - beginning.size(); i < str.size(); i += max) {
        for (size_t j = i; j-- > base;)
            if (std::find(delimiters.begin(), delimiters.end(), str[j])
                != delimiters.end()) {
                if (j > base and str[j] == '-' and str[j - 1] == 'e')
                    continue;
                if (j == str.size() - 1)
                    j = str.size();
                lines.push_back(
                    std::string(str.begin() + base, str.begin() + j));
                base = j;
                i    = j;
                break;
            }
    }
    if (base < str.size())
        lines.push_back(std::string(str.begin() + base, str.end()));
    for (size_t i = 0; i != lines.size(); ++i) {
<<<<<<< HEAD:src/csl/libraryevaluator.cpp
        out << LibraryGenerator::indent(indent)
            << ((i == 0) ? beginning : "  ") << lines[i]
            << ((i == lines.size() - 1) ? (end + ";") : std::string{}) << "\n";
=======
        out << LibraryGenerator::indent(indent) << ((i == 0) ? beginning : "  ") 
            << lines[i] << ((i == lines.size()-1) ? (end + ";") : std::string{}) 
            << "\n";
>>>>>>> Full C/C++ library generation:csl/src/libraryevaluator.cpp
    }
}

std::ostream &operator<<(std::ostream &out, LibEval const &eval)
{
    eval.print(out);
    return out;
}

///////////////////////////////////////////////////
/*************************************************/
// Class LibSession                              //
/*************************************************/
///////////////////////////////////////////////////

LibEvalSession::LibEvalSession()
{
}

Expr LibEvalSession::addEval(Expr init)
{
    CSL_ASSERT_SPEC(csl::Abbrev::getFreeStructure(init).size() == 0,
                    CSLError::TypeError,
                    "Adding LibEval for an expression that have free indices: "
                        + toString(init));
    csl::Refresh(init);
    if (init->getType() == csl::Type::Sum)
        for (const auto &arg : *init)
            CSL_ASSERT_SPEC(csl::Abbrev::getFreeStructure(arg).size() == 0,
                            CSLError::TypeError,
                            "Adding LibEval for an expression that have free "
                            "indices: "
                                + toString(init));
    auto term = init; // GetTerm(init);
    if (term->size() == 0) {
        return csl::Copy(init);
    }
    auto pos = std::find(eval.begin(), eval.end(), term);
    if (pos != eval.end()) {
        return Copy(pos->getExpr());
    }
    csl::DeepRefresh(term);
    LibEval newEval(term, maxID++);
    eval.push_back(newEval);
    return Copy(newEval.getExpr()); // * init->getNumericalFactor();
}

std::vector<LibEval> &LibEvalSession::getEval()
{
    return eval;
}

std::vector<LibEval> const &LibEvalSession::getEval() const
{
    return eval;
}

std::vector<Expr> const &LibEvalSession::getUnEval() const
{
    return unEval;
}

std::vector<Expr> &LibEvalSession::getUnEval()
{
    return unEval;
}

std::vector<csl::Tensor> const &LibEvalSession::getTensors() const
{
    return tensors;
}

std::vector<csl::Tensor> &LibEvalSession::getTensors()
{
    return tensors;
}

void LibEvalSession::setEval(std::vector<LibEval> const &t_eval)
{
    eval = t_eval;
}

void LibEvalSession::setEval(std::vector<LibEval> &&t_eval)
{
    eval = std::move(t_eval);
}

LibEval findEvalDicho(int                                  id,
                      std::vector<LibEval>::const_iterator first,
                      std::vector<LibEval>::const_iterator last)
{
    if (first == last) {
        throw 4;
        CSL_ASSERT_SPEC(first != last,
                        CSLError::RuntimeError,
                        "LibEval not found for id " + toString(id) + ".");
    }
    if (first + 1 == last)
        return *first;

// Macros to avoid any stack variable for the recursion
#define DIFF std::distance(first, last) / 2
#define EVAL *(first + DIFF)
#define EVALID (*(first + DIFF)).getID()

    if (EVALID == id)
        return EVAL;
    if (EVALID > id)
        return findEvalDicho(id, first, first + DIFF);
    return findEvalDicho(id, first + DIFF, last);

#undef DIFF
#undef EVAL
#undef EVALID
}

LibEval exprToEval(Expr const &expr, std::vector<LibEval> const &eval)
{
    std::string_view name = expr->getName();
    char             buffer[5]{0, 0, 0, 0, 0};
    for (size_t i = LibEval::name.size() + 1; i != name.size(); ++i)
        buffer[i - LibEval::name.size() - 1] = name[i];
    int id = std::atoi(buffer);
    // lib_log << "Searching " << expr << std::endl;
    // lib_log << "ID : " << id << std::endl;
    // lib_log << eval.size() << " evals" << std::endl;
    try {
        return findEvalDicho(id, eval.begin(), eval.end());
    }
    catch (int a) {
        if (a == 4) {
            // lib_log << eval.size() << " evals: " << std::endl;
            // for (const auto e : eval)
            // lib_log << e << std::endl;
        }
        throw a;
    }
}

LibEvalSession::Perf LibEvalSession::getPerf(Expr &init)
{
    std::vector<LibEval> newEval;
    std::vector<int>     nOccurences;
    csl::DeepRefresh(init);
    csl::VisitEachLeaf(init, [&](Expr const &expr) {
        if (LibEval::isInstance(expr))
            getPerf(exprToEval(expr, eval), newEval, nOccurences);
    });
    size_t nOp = 0;
    for (const auto &nEval : newEval)
        nOp += (nEval.init->size() == 0) ? 0 : nEval.init->size() - 1;
    // for (size_t i = 0; i != newEval.size(); ++i)
    //     if (nOccurences[i] <= 1
    //             and newEval[i].expr != init
    //             and !newEval[i].init->isIndexed()
    //             and newEval[i].init->size() < 5) {
    //         if (init->dependsExplicitlyOn(newEval[i].expr.get()))
    //             init = Replaced(
    //                     init,
    //                     newEval[i].expr,
    //                     newEval[i].init
    //                     );
    //         for (size_t j = 0; j != newEval.size(); ++j)
    //             if (i != j)
    //                 if (newEval[j].init->dependsExplicitlyOn(
    //                             newEval[i].expr.get())) {
    //                     newEval[j].init = Replaced(
    //                             newEval[j].init,
    //                             newEval[i].expr,
    //                             newEval[i].init
    //                             );
    //                 }
    //         newEval.erase(newEval.begin() + i);
    //         nOccurences.erase(nOccurences.begin() + i);
    //         --i;
    //     }
    eval = std::move(newEval);
    std::sort(
        eval.begin(), eval.end(), [&](LibEval const &A, LibEval const &B) {
            return A.getID() < B.getID();
        });
    auto last = std::unique(eval.begin(), eval.end());
    eval.erase(last, eval.end());

    return {eval.size(), nOp, eval};
}

void LibEvalSession::getPerf(LibEval const        &init,
                             std::vector<LibEval> &newEvals,
                             std::vector<int>     &nOccurences)
{
    auto pos = std::find(newEvals.begin(), newEvals.end(), init);
    if (pos == newEvals.end()) {
        newEvals.push_back(init);
        nOccurences.push_back(0);
    }
    else {
        ++nOccurences[std::distance(newEvals.begin(), pos)];
    }
    Expr recursive = init.init;
    csl::VisitEachLeaf(recursive, [&](Expr const &expr) {
        if (LibEval::isInstance(expr))
            getPerf(exprToEval(expr, eval), newEvals, nOccurences);
    });
}

void LibEvalSession::merge()
{
    csl::ProgressBar bar(eval.size());
    for (size_t i = 0; i + 1 < eval.size(); ++i) {
        bar.progress(i + 1);
        CSL_ASSERT_SPEC(eval[i].getExpr() != eval[i].init,
                        CSLError::RuntimeError,
                        "Expression and abreviation of LibEval should be "
                        "different for"
                        " " + toString(eval[i].getExpr()));
        for (size_t j = i + 1; j < eval.size(); ++j)
            if (eval[i] == eval[j]) {
                // lib_log << "Merging : \n";
                // lib_log << eval[i] << std::endl;
                // lib_log << eval[j] << std::endl;
                for (size_t k = 0; k != eval.size(); ++k)
                    Replace(eval[k].init, eval[j].expr, eval[i].expr);
                eval.erase(eval.begin() + j);
                --j;
            }
    }
}

void LibEvalSession::removeUnique()
{
}

void LibEvalSession::simplify()
{
    // simplifyProds();
    // simplifySums();
    merge();
}

bool independent(LibEval const &A, LibEval const &B)
{
    return (not A.getInit()->dependsExplicitlyOn(B.getExpr().get())
            and not B.getInit()->dependsExplicitlyOn(A.getExpr().get()));
}

void LibEvalSession::simplifyProds(size_t min_factors)
{
    csl::ProgressBar bar(eval.size());
    for (size_t i = 0; i != eval.size(); ++i) {
        bar.progress(i);
        auto eval1 = eval[i];
        if (eval1.init->getType() != csl::Type::Prod)
            continue;
        for (size_t j = i + 1; j < eval.size(); ++j) {
            auto eval2 = eval[j];
            if (eval2.init->getType() != csl::Type::Prod)
                continue;
            if (not independent(eval1, eval2))
                continue;
            Expr factored = Factored(Copy(eval1.init) + Copy(eval2.init));
            if (factored->getType() != csl::Type::Prod)
                continue;
            std::vector<Expr> factoredArgs;
            for (const auto &arg : *factored)
                if (LibEval::isInstance(arg))
                    factoredArgs.push_back(arg);
            if (factoredArgs.size() < min_factors)
                continue;
            Expr commonFactor = addEval(prod_s(factoredArgs));
            if (GetTerm(commonFactor) != eval1.getExpr()) {
                for (const auto &factor : factoredArgs) {
                    eval1.init = eval1.init->suppressTerm(factor.get());
                }
                eval1.init = commonFactor * eval1.init;
            }
            if (GetTerm(commonFactor) != eval2.getExpr()) {
                for (const auto &factor : factoredArgs) {
                    eval2.init = eval2.init->suppressTerm(factor.get());
                }
                eval2.init = commonFactor * eval2.init;
            }
            assert(eval1.init != eval1.getExpr());
            assert(eval2.init != eval2.getExpr());
            eval[j] = eval2;
            break;
        }
        eval[i] = eval1;
    }
}

void LibEvalSession::simplifySums()
{
    csl::ProgressBar bar(eval.size());
    for (size_t i = 0; i != eval.size(); ++i) {
        bar.progress(i);
        auto eval1 = eval[i];
        if (eval1.init->getType() != csl::Type::Sum)
            continue;
        auto args_1 = eval1.init->getVectorArgument();
        for (auto &arg : args_1)
            arg = GetTerm(arg);
        Expr prod_1 = prod_s(args_1);
        for (size_t j = i + 1; j < eval.size(); ++j) {
            auto eval2 = eval[j];
            if (eval2.init->getType() != csl::Type::Sum)
                continue;
            if (not independent(eval1, eval2))
                continue;
            auto args_2 = eval2.init->getVectorArgument();
            for (auto &arg : args_2)
                arg = GetTerm(arg);
            Expr prod_2   = prod_s(args_2);
            Expr factored = Factored(prod_1 + prod_2);
            if (factored->getType() != csl::Type::Prod)
                continue;
            std::vector<Expr> factoredArgs;
            for (const auto &arg : *factored)
                if (LibEval::isInstance(arg))
                    factoredArgs.push_back(arg);
            if (factoredArgs.size() < 2)
                continue;
            Expr commonFactor = addEval(sum_s(factoredArgs));
            if (GetTerm(commonFactor) != eval1.getExpr()) {
                for (const auto &factor : factoredArgs) {
                    eval1.init = eval1.init - factor;
                }
                eval1.init = eval1.init + commonFactor;
            }
            if (GetTerm(commonFactor) != eval2.getExpr()) {
                for (const auto &factor : factoredArgs) {
                    eval2.init = eval2.init - factor;
                }
                eval2.init = eval2.init + commonFactor;
            }
            assert(eval1.init != eval1.getExpr());
            assert(eval2.init != eval2.getExpr());
            eval[j] = eval2;
            break;
        }
        eval[i] = eval1;
    }
}

LibEvalSession LibEvalSession::parseExpression(Expr &expr,
                                               bool  findIntermediates)
{
    LibEvalSession session;
    session.parse(expr, findIntermediates);

    return session;
}

csl::IndexStructure LibEvalSession::getFreeStructure(Expr const &t_init)
{
    return csl::Abbrev::getFreeStructure(t_init);
}

csl::IndexStructure
LibEvalSession::getFreeStructure(csl::IndexStructure const &t_init)
{
    return csl::Abbrev::getFreeStructure(t_init);
}

size_t getNLeafs(csl::Expr const &expr)
{
    size_t n = 0;
    csl::VisitEachLeaf(expr, [&n](csl::Expr const &) { ++n; });
    return n;
}

void LibEvalSession::parse(
    Expr                                             &expr,
    bool                                              findIntermediates,
    std::map<csl::AbstractParent const *, csl::Expr> &parsedAbbrevs)
{
    csl::ScopedProperty prop(&csl::option::canonicalSumNumericalFactor, false);
    csl::Evaluate(expr, csl::eval::numerical | csl::eval::literal);
    csl::ForEachNodeReversed(expr, [&](Expr &el) {
        if (el->getType() == csl::Type::Prod and el->isIndexed()) {
            parseProduct(el, findIntermediates, parsedAbbrevs);
        }
    });
    csl::DeepRefresh(expr);
    csl::ForEachNodeReversed(expr, [&](Expr &el) {
        if (getNLeafs(el) > maxLeafs
            && getFreeStructure(el->getIndexStructure()).size() == 0) {
            el = addEval(el);
        }
        else if (csl::Abbrev::isAnAbbreviation(el)) {
            auto parent = el->getParent_info();
            auto pos    = parsedAbbrevs.find(parent);
            if (pos != parsedAbbrevs.end()) {
                if (el->isComplexConjugate())
                    el = GetComplexConjugate(pos->second);
                else
                    el = pos->second;
            }
            else {
                csl::Expr encaps = parent->getEncapsulated();
                parse(encaps, findIntermediates, parsedAbbrevs);
                encaps                = addEval(encaps);
                parsedAbbrevs[parent] = encaps;
                if (el->isComplexConjugate())
                    el = GetComplexConjugate(encaps);
                else
                    el = encaps;
            }
        }
        else if (!csl::IsProd(el) && !csl::IsSum(el) && el->size() > 0) {
            el = addEval(el);
        }
    });
    csl::Evaluate(expr, csl::eval::indicial);
}

void LibEvalSession::parse(Expr &expr, bool findIntermediates)
{
    std::map<csl::AbstractParent const *, csl::Expr> parsedAbbrevs;
    parse(expr, findIntermediates, parsedAbbrevs);
}

Expr LibEvalSession::expandIProd(Expr const &iprod)
{
    CSL_ASSERT_SPEC(iprod->getType() == csl::Type::Prod and iprod->isIndexed(),
                    CSLError::TypeError,
                    "Expected IProd, " + toString(iprod->getType())
                        + " given.");

    auto nCommonIndices
        = [&](csl::IndexStructure const &A, csl::IndexStructure const &B) {
              size_t n = 0;
              for (size_t iA = 0; iA != A.size(); ++iA)
                  for (size_t iB = 0; iB != B.size(); ++iB)
                      if (A[iA] == B[iB])
                          ++n;
              return n;
          };

    std::vector<Expr> sum;
    std::vector<Expr> scalar;
    sum.reserve(iprod->size());
    scalar.reserve(iprod->size());
    for (const auto &arg : *iprod)
        if (arg->getType() == csl::Type::Sum and arg->isIndexed())
            sum.push_back(arg);
        else
            scalar.push_back(arg);
    if (sum.empty())
        return iprod;

    std::vector<csl::IndexStructure> structure;
    structure.reserve(sum.size());
    for (const auto &arg : sum)
        structure.push_back(arg->getIndexStructure());

    struct maxCommon {
        size_t i;
        size_t j;
        size_t max;
    };

    while (sum.size() > 1) {
        maxCommon maxi{0, 0, 0};
        for (size_t i = 0; i != structure.size(); ++i)
            for (size_t j = i + 1; j < structure.size(); ++j)
                if (auto n = nCommonIndices(structure[i], structure[j]);
                    n > maxi.max)
                    maxi = {i, j, n};

        if (maxi.i == maxi.j)
            maxi.j = 1;
        structure[maxi.i] += structure[maxi.j];
        structure.erase(structure.begin() + maxi.j);
        sum[maxi.i] = Expanded(prod_s(sum[maxi.i], sum[maxi.j], true));
        sum.erase(sum.begin() + maxi.j);
    }
    scalar.push_back(sum[0]);
    Expr res = ExpandedIf(
        prod_s(scalar),
        [&](Expr const &expr) { return expr->isIndexed(); },
        true);
    return res;
}

void LibEvalSession::parseProduct(
    Expr                                             &iprod,
    bool                                              findIntermediates,
    std::map<csl::AbstractParent const *, csl::Expr> &parsedAbbrevs)
{
    CSL_ASSERT_SPEC(iprod->getType() == csl::Type::Prod and iprod->isIndexed(),
                    CSLError::TypeError,
                    "Expecting IProd, " + toString(iprod->getType())
                        + " given.");

    // static size_t n = 0;
    // lib_log << "Parsing prod (" << n << ")\n" << iprod << std::endl;
    std::vector<Expr> args = iprod->getVectorArgument();
    size_t            pos  = 0;
    do {
        Expr              first;
        std::vector<Expr> scalar;
        int               firstScalar = -1;
        for (size_t i = pos; i != args.size(); ++i) {
            if (not first and args[i]->isIndexed()) {
                pos   = i;
                first = args[i];
            }
            else if (not args[i]->isIndexed()) {
                scalar.push_back(args[i]);
                if (firstScalar == -1)
                    firstScalar = i;
                args[i] = CSL_1;
            }
        }
        if (findIntermediates and scalar.size() > 0) {
            args[firstScalar] = addEval(prod_s(scalar, true));
        }
        else if (scalar.size() > 0)
            args[firstScalar] = prod_s(scalar, true);
        if (first) {
            csl::IndexStructure structure = csl::Abbrev::getFreeStructure(
                args[pos]->getIndexStructure());
            std::vector<Expr> subArgs(1, args[pos]);
            args[pos]    = CSL_1;
            size_t index = pos;
            while (structure.size() > 0 and ++index < args.size()) {
                if (args[index] == CSL_1)
                    continue;
                csl::IndexStructure otherStruct
                    = csl::Abbrev::getFreeStructure(
                        args[index]->getIndexStructure());
                size_t totalSize = structure.size() + otherStruct.size();
                csl::IndexStructure merged
                    = csl::Abbrev::getFreeStructure(structure + otherStruct);
                if (merged.size() < totalSize) {
                    structure = merged;
                    subArgs.push_back(args[index]);
                    args[index] = CSL_1;
                    index       = pos;
                }
            }
            if (structure.empty()) {
                Expr prod = prod_s(subArgs, true);
                if (not expand) {
                    args[pos] = (findIntermediates) ? addEval(prod) : prod;
                    continue;
                }
                Expr expanded = DeepCopy(prod);
                csl::Transform(expanded, [&](Expr &node) {
                    if (node->getType() == csl::Type::Prod
                        and node->isIndexed()) {
                        auto ex = expandIProd(node);
                        if (ex.get() != node.get()) {
                            node = ex;
                            return true;
                        }
                        return false;
                    }
                    return false;
                });
                if (findIntermediates and expanded == prod)
                    args[pos] = addEval(prod);
                else if (expanded != prod) {
                    parse(expanded, findIntermediates, parsedAbbrevs);
                    args[pos] = expanded;
                }
                else
                    args[pos] = prod;
            }
            else {
                args[pos] = prod_s(subArgs);
            }
        }
        else
            break;

    } while (++pos < args.size());

    iprod = prod_s(args);
    // lib_log << "RES parse prod (" << n++ << ") = " << iprod << std::endl;
}

void LibEvalSession::gatherUnEvalAndTensors(Expr const &result)
{
    auto gather = [&](LibEval &ev) {
        ev.replaceTensors();
        auto const &reg = ev.getUnEval();
        auto const &ten = ev.getTensors();
        for (const auto &e : reg)
            addUnEval(e);
        for (const auto &t : ten)
            addTensor(t);
    };
    for (auto &ev : eval) {
        gather(ev);
    }
    LibEval last(result, maxID++);
    gather(last);
}

void LibEvalSession::addUnEval(Expr const &expr) const
{
    auto pos = std::find(unEval.begin(), unEval.end(), expr);
    if (pos == unEval.end())
        unEval.push_back(expr);
}

void LibEvalSession::addTensor(csl::Tensor const &parent) const
{
    auto pos = std::find(tensors.begin(), tensors.end(), parent);
    if (pos == tensors.end())
        tensors.push_back(parent);
}

void LibEvalSession::print(std::ostream &out) const
{
    for (const auto &ev : eval)
        out << ev << '\n';
}

void LibEvalSession::printCLib(Expr         &init,
                               Perf         &perf,
                               std::ostream &out,
                               bool          onlyDep)
{
    for (auto &eval : perf.evals) {
        eval.printLib(out, LibraryMode::CLib);
    }
    if (not onlyDep)
<<<<<<< HEAD:src/csl/libraryevaluator.cpp
        LibEval::printExpression(out,
                                 init,
                                 LibraryMode::CLib,
                                 1,
                                 "return create_ccomplex_return(",
                                 ")");
=======
        LibEval::printExpression(out, init, LibraryMode::CLib, 1, 
                "return create_ccomplex_return(",
                ")");
>>>>>>> Full C/C++ library generation:csl/src/libraryevaluator.cpp
}

void LibEvalSession::printLib(Expr         &init,
                              Perf         &perf,
                              std::ostream &out,
                              bool          onlyDep)
{
    for (auto &eval : perf.evals) {
        eval.printLib(out, LibraryMode::CppLib);
    }
    if (not onlyDep)
        LibEval::printExpression(out, init, LibraryMode::CppLib, 1, "return ");
}

} // End of namespace csl
