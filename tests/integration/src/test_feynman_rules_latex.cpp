#include "marty.h"
#include "marty/models/qed.h"
#include "marty/models/sm.h"
#include <gtest/gtest.h>

using namespace mty;

namespace {

std::string blockFor(std::string const &text, std::string const &marker)
{
    return text.substr(text.find(marker));
}

size_t countOccurrences(std::string const &text, std::string const &pattern)
{
    size_t count = 0;
    for (size_t pos = text.find(pattern); pos != std::string::npos;
         pos        = text.find(pattern, pos + pattern.size()))
        ++count;
    return count;
}

} // namespace

TEST(FeynmanRuleLatex, singleRuleIsACenteredDiagramWithItsValue)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string latex = LatexFeynmanRule(rules[0]);

    EXPECT_EQ(latex.rfind("\\begin{center}", 0), 0u);
    EXPECT_NE(latex.find("\\begin{tikzpicture}"), std::string::npos);
    EXPECT_NE(latex.find("\\end{center}"), std::string::npos);
    EXPECT_NE(latex.find("\\quad= " + LatexFeynmanRuleValue(rules[0]) + "$"),
              std::string::npos);
}

TEST(FeynmanRuleLatex, valueLineComesAfterTheDiagramAndBeforeTheEnd)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string latex = LatexFeynmanRule(rules[0]);
    std::string value = "\\quad= " + LatexFeynmanRuleValue(rules[0]) + "$";
    std::string tail  = value + "\n\\end{center}\n";

    ASSERT_GE(latex.size(), tail.size());
    EXPECT_EQ(latex.substr(latex.size() - tail.size()), tail);
    EXPECT_LT(latex.find("\\end{tikzpicture}"), latex.find(value));
}

TEST(FeynmanRuleLatex, everyEdgeIsLabelledWithItsField)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string latex = LatexFeynmanRule(rules[0]);
    for (auto const &field : rules[0].getFieldProduct())
        EXPECT_NE(latex.find(field.getLatexDisplayName()), std::string::npos)
            << "missing label " << field.getLatexDisplayName();
}

TEST(FeynmanRuleLatex, fermionEdgesUseBar)
{
    QED_Model qed;
    for (auto const &rule : qed.getFeynmanRules()) {
        bool hasConjugateFermion = false;
        for (auto const &field : rule.getFieldProduct())
            if (field.getConjugated() and not field.isBosonic())
                hasConjugateFermion = true;
        if (!hasConjugateFermion)
            continue;
        std::string latex = LatexFeynmanRule(rule);
        EXPECT_NE(latex.find("\\bar{"), std::string::npos);
        EXPECT_EQ(latex.find("^\\star"), std::string::npos);
        return;
    }
    FAIL() << "no rule with a conjugate fermion found in QED";
}

TEST(FeynmanRuleLatex, emptyListGivesAnEmptyDocument)
{
    EXPECT_EQ(LatexFeynmanRules({}), "");
}

TEST(FeynmanRuleLatex, documentHasOneHeaderAndBlockPerRule)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string document = LatexFeynmanRules(rules);

    EXPECT_EQ(countOccurrences(document, "Feynman vertex for "), rules.size());
    EXPECT_EQ(countOccurrences(document, "\\begin{center}"), rules.size());
    EXPECT_EQ(countOccurrences(document, "\\end{center}"), rules.size());
}

TEST(FeynmanRuleLatex, headerNamesTheFieldsOfTheRule)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string document = LatexFeynmanRules({rules[0]});
    std::string header   = blockFor(document, "Feynman vertex for ");
    for (auto const &field : rules[0].getFieldProduct())
        EXPECT_NE(header.find(field.getLatexDisplayName()), std::string::npos)
            << "header misses " << field.getLatexDisplayName();
}

TEST(FeynmanRuleLatex, singleRuleBlockIsInTheDocument)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    std::string value
        = "\\quad= " + LatexFeynmanRuleValue(rules[0]) + "$\n\\end{center}";
    EXPECT_NE(LatexFeynmanRules({rules[0]}).find(value), std::string::npos);
}

TEST(FeynmanRuleLatex, photonHeaderHasNoPolarizationIndex)
{
    QED_Model qed;
    bool      foundPhoton = false;
    for (auto const &rule : qed.getFeynmanRules()) {
        for (auto const &field : rule.getFieldProduct()) {
            if (field.getLatexName() != "A")
                continue;
            foundPhoton = true;
            std::string header = blockFor(LatexFeynmanRules({rule}),
                                          "Feynman vertex for ");
            EXPECT_NE(header.find("A(p_"), std::string::npos) << header;
            EXPECT_EQ(header.find("A_{"), std::string::npos) << header;
            break;
        }
        if (foundPhoton)
            return;
    }
    FAIL() << "no photon vertex found in QED";
}

TEST(FeynmanRuleLatex, vertexValueUsesTheLabelsOfTheFieldProduct)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    for (auto const &rule : rules) {
        csl::Expr product = CSL_1;
        for (auto const &field : rule.getFieldProduct()) {
            csl::Expr factor = csl::make_shared<QuantumField>(field);
            product          = product * factor;
        }
        csl::IndexRenaming renaming;
        csl::RelabelIndices(product, true, renaming);
        csl::Expr value = csl::ApplyIndexRenaming(
            csl::ExpandAbbreviations(rule.getExpr()), renaming);
        std::string expected = csl::RelabelIndices(value, false)->printLaTeX(0);
        EXPECT_EQ(LatexFeynmanRuleValue(rule), expected);
    }
}

TEST(FeynmanRuleLatex, vertexValueShowsAbbreviationDefinitions)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());

    csl::Expr a  = csl::variable_s("a");
    csl::Expr b  = csl::variable_s("b");
    FeynmanRule rule = rules[0];
    rule.setExpr(csl::Abbrev::makeAbbreviation("Cab", a * b));

    std::string latex = LatexFeynmanRule(rule);
    EXPECT_NE(latex.find("= ab$"), std::string::npos) << latex;
    EXPECT_EQ(latex.find("Cab"), std::string::npos) << latex;
}

TEST(FeynmanRuleLatex, nonPolarizationIndicesStayInTheHeader)
{
    QED_Model qed;
    for (auto const &rule : qed.getFeynmanRules()) {
        for (auto const &field : rule.getFieldProduct()) {
            if (field.isBosonic())
                continue;
            std::string document = LatexFeynmanRules({rule});
            EXPECT_NE(document.find(field.getLatexDisplayName() + "_{"),
                      std::string::npos)
                << "fermion index missing for " << field.getLatexDisplayName();
            return;
        }
    }
    FAIL() << "no fermion found in QED rules";
}

namespace {

bool isSpinIndex(QuantumField const &field, csl::Index const &index,
                 size_t position)
{
    return position == 0 and field.isBosonic()
           and index.getSpace() == &csl::Minkowski;
}

bool containsIndex(std::vector<csl::Index> const &indices,
                   csl::Index const &          index)
{
    for (auto const &other : indices)
        if (other == index)
            return true;
    return false;
}

std::vector<csl::Index> nonSpinFieldIndices(FeynmanRule const &rule)
{
    std::vector<csl::Index> indices;
    for (auto const &field : rule.getFieldProduct()) {
        auto structure = field.getIndexStructure().getIndex();
        for (size_t i = 0; i != structure.size(); ++i)
            if (not isSpinIndex(field, structure[i], i))
                indices.push_back(structure[i]);
    }
    return indices;
}

std::vector<csl::Index> allFieldIndices(FeynmanRule const &rule)
{
    std::vector<csl::Index> indices;
    for (auto const &field : rule.getFieldProduct()) {
        auto structure = field.getIndexStructure();
        indices.insert(indices.end(), structure.getIndex().begin(),
                       structure.getIndex().end());
    }
    return indices;
}

void expectFieldIndicesAreFreeIndicesOfValue(FeynmanRule const &rule)
{
    std::vector<csl::Index> fieldIndices = allFieldIndices(rule);
    std::vector<csl::Index> nonSpin      = nonSpinFieldIndices(rule);
    auto freeStructure = rule.getExpr()->getFreeIndexStructure();
    std::vector<csl::Index> freeIndices = freeStructure.getIndex();
    std::string value = LatexFeynmanRuleValue(rule);
    for (auto const &index : freeIndices)
        EXPECT_TRUE(containsIndex(fieldIndices, index))
            << "free index " << index << " of value " << value
            << " is not carried by a field";
    for (auto const &index : nonSpin)
        EXPECT_TRUE(containsIndex(freeIndices, index))
            << "field index " << index << " is not free in value " << value;
}

} // namespace

TEST(FeynmanRuleIndexSync, fieldIndicesAreTheFreeIndicesOfTheValue)
{
    QED_Model qed;
    auto const &rules = qed.getFeynmanRules();
    ASSERT_FALSE(rules.empty());
    for (auto const &rule : rules)
        expectFieldIndicesAreFreeIndicesOfValue(rule);
}

TEST(FeynmanRuleIndexSync, smRulesCarryTheFreeIndicesOfTheirValue)
{
    SM_Model sm;
    size_t   withDelta = 0;
    for (auto const &rule : sm.getFeynmanRules()) {
        if (LatexFeynmanRuleValue(rule).find("delta") != std::string::npos)
            ++withDelta;
        expectFieldIndicesAreFreeIndicesOfValue(rule);
    }
    EXPECT_GT(withDelta, 0u) << "no SM rule with a Kronecker delta found";
}

TEST(FeynmanRuleLatex, diagramLabelsShowTheFreeIndicesOfTheFields)
{
    QED_Model qed;
    for (auto const &rule : qed.getFeynmanRules()) {
        bool hasPhoton = false;
        for (auto const &field : rule.getFieldProduct())
            if (field.getLatexName() == "A")
                hasPhoton = true;
        if (not hasPhoton)
            continue;

        csl::Expr product = CSL_1;
        for (auto const &field : rule.getFieldProduct()) {
            csl::Expr factor = csl::make_shared<QuantumField>(field);
            product          = product * factor;
        }
        csl::IndexRenaming renaming;
        csl::Expr renamed = csl::RelabelIndices(product, true, renaming);
        std::vector<QuantumField> renamedFields;
        csl::ForEachLeaf(renamed, [&](csl::Expr const &el) {
            if (IsOfType<QuantumField>(el))
                renamedFields.push_back(ConvertTo<QuantumField>(el));
        });
        ASSERT_EQ(renamedFields.size(), rule.getFieldProduct().size());

        std::string latex     = LatexFeynmanRule(rule);
        std::string diagram   = latex.substr(0, latex.find("\\quad="));
        std::string value     = LatexFeynmanRuleValue(rule);
        bool        hasLabel  = false;
        for (auto const &field : renamedFields) {
            auto structure = field.getIndexStructure().getIndex();
            for (size_t i = 0; i != structure.size(); ++i) {
                if (isSpinIndex(field, structure[i], i))
                    continue;
                hasLabel          = true;
                std::string label = structure[i].printLaTeX();
                EXPECT_NE(diagram.find("_{" + label + "}"), std::string::npos)
                    << "diagram misses index " << label << "\n" << diagram;
                EXPECT_NE(value.find(label), std::string::npos)
                    << "value misses index " << label << ": " << value;
            }
        }
        EXPECT_TRUE(hasLabel);
        return;
    }
    FAIL() << "no photon vertex found in QED";
}
