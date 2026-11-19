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
 * @file symbolic_algorithms.cpp
 * @brief Golden values of the CSL callback algorithms, run from C++.
 *
 * The CSL algorithms (AnyOfLeafs, FindIfNode, VisitEachNodeCut, DistributedIf,
 * ...) take C++ callbacks, so they are not exposed through the facade. This
 * test runs them directly on the same expressions and predicates as
 * test_algorithms.py, and stores one "<case>\t<value>" line per case in
 * tests/api/data/symbolic/algorithms.txt. The Python duplicates must match.
 * With MARTY_UPDATE_GOLDEN=1 the file is rewritten; otherwise it is compared.
 */

#include "csl/algo.h"
#include "csl/api/expr.h"
#include "csl/api/functions.h"
#include "csl/api/transform.h"
#include "csl/interface.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using csl::api::ExprHandle;

namespace {

using Predicate = std::function<bool(csl::Expr const &)>;

Predicate isVariable(std::string name)
{
    return [name = std::move(name)](csl::Expr const &e) {
        ExprHandle h(e);
        return h.type() == csl::Type::Variable
               and csl::api::symbolName(h) == name;
    };
}

Predicate isType(csl::Type kind)
{
    return [kind](csl::Expr const &e) { return ExprHandle(e).type() == kind; };
}

std::string text(bool value)
{
    return value ? "True" : "False";
}

std::string latex(csl::Expr const &expr)
{
    return expr == nullptr ? "None" : csl::api::latex(ExprHandle(expr));
}

ExprHandle sample()
{
    auto x = ExprHandle::variable("x");
    auto y = ExprHandle::variable("y");
    auto z = ExprHandle::variable("z");
    return csl::api::Sin(x * y) + z;
}

std::vector<std::pair<std::string, std::string>> goldenValues()
{
    auto       x    = ExprHandle::variable("x");
    auto       y    = ExprHandle::variable("y");
    auto       z    = ExprHandle::variable("z");
    auto const expr = sample().raw();

    int visited = 0;
    csl::VisitEachNodeCut(
        expr,
        [&](csl::Expr const &e) {
            ++visited;
            return ExprHandle(e).type() == csl::Type::Prod;
        },
        -1);

    return {
        {"any_leafs_y", text(csl::AnyOfLeafs(expr, isVariable("y"), -1))},
        {"any_leafs_w", text(csl::AnyOfLeafs(expr, isVariable("w"), -1))},
        {"all_leafs_variables",
         text(csl::AllOfLeafs(expr, isType(csl::Type::Variable), -1))},
        {"all_leafs_depth0", text(csl::AllOfLeafs(expr, isVariable("x"), 0))},
        {"any_nodes_prod",
         text(csl::AnyOfNodes(expr, isType(csl::Type::Prod), -1))},
        {"all_nodes_prod",
         text(csl::AllOfNodes(expr, isType(csl::Type::Prod), -1))},
        {"find_leaf_z", latex(csl::FindIfLeaf(expr, isVariable("z"), -1))},
        {"find_leaf_none", latex(csl::FindIfLeaf(expr, isVariable("w"), -1))},
        {"find_node_prod",
         latex(csl::FindIfNode(expr, isType(csl::Type::Prod), -1))},
        {"find_node_root",
         latex(csl::FindIfNode(expr, isType(csl::Type::Sum), -1))},
        {"visit_cut_count", std::to_string(visited)},
        {"visit_first_leaf_y",
         text(csl::VisitFirstOfLeaf(expr, isVariable("y")))},
        {"visit_first_node_w",
         text(csl::VisitFirstOfNode(expr, isVariable("w")))},
        {"expand_product_nonfull",
         latex(csl::api::expanded((x + y) * z, false).raw())},
        {"expand_square_nonfull",
         latex(csl::api::expanded((x + y) * (x + y), false).raw())},
        {"rebuilt_sum", latex(csl::api::rebuilt(x + y, {x, z}).raw())},
        {"distribute_if_z",
         latex(
             csl::DistributedIf(((x + y) * z).raw(), isVariable("z"), false))},
        {"distribute_if_none",
         latex(
             csl::DistributedIf(((x + y) * z).raw(), isVariable("w"), false))},
        {"distribute_if_full",
         latex(csl::DistributedIf(
             ((x + y) * (z + x)).raw(), isType(csl::Type::Sum), true))},
        {"expand_if_sums",
         latex(csl::ExpandedIf(((x + y) * (z + x)).raw(),
                               isType(csl::Type::Sum),
                               false,
                               false))},
        {"expand_if_contains_x",
         latex(csl::ExpandedIf(
             ((x + y) * (z + y)).raw(), isVariable("x"), false, false))},
        {"expand_if_square_full",
         latex(csl::ExpandedIf(
             ((x + y) * (x + y)).raw(), isType(csl::Type::Sum), true, false))},
        {"expand_if_inplace",
         latex(csl::ExpandedIf(
             ((x + y) * (z + y)).raw(), isVariable("x"), false, true))},
    };
}

} // namespace

TEST(SymbolicAlgorithmsApi, GoldenValuesStable)
{
    std::string const path
        = std::string(MARTY_API_DATA_PATH) + "/symbolic/algorithms.txt";

    std::vector<std::string> actual = {
        "# Golden values of the CSL callback algorithms (csl/algo.h,",
        "# csl/interface.h), run from C++ on the expressions of",
        "# tests/api/src/symbolic_algorithms.cpp. One '<case>\\t<value>' line",
        "# per case, matched by name in test_algorithms.py.",
        "# Rewrite only for an intended change:",
        "# MARTY_UPDATE_GOLDEN=1 symbolic_algorithms.x",
    };
    for (auto const &[name, value] : goldenValues())
        actual.push_back(name + "\t" + value);

    char const *update = std::getenv("MARTY_UPDATE_GOLDEN");
    if (update != nullptr && std::string(update) == "1") {
        std::filesystem::create_directories(
            std::filesystem::path(path).parent_path());
        std::ofstream out(path, std::ios::trunc);
        ASSERT_TRUE(out) << "cannot write " << path;
        for (auto const &line : actual)
            out << line << '\n';
        return;
    }

    std::vector<std::string> expected;
    std::ifstream            in(path);
    for (std::string line; std::getline(in, line);)
        expected.push_back(line);

    ASSERT_EQ(expected.size(), actual.size())
        << path << " must hold the header and one line per case";
    for (std::size_t n = 0; n < actual.size(); ++n)
        EXPECT_EQ(expected[n], actual[n]) << path << ", line " << n + 1;
}
