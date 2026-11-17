#include <csl.h>
#include <gtest/gtest.h>

using namespace csl;

namespace {

std::string latex(Expr const &expr)
{
    return expr->printLaTeX(0);
}

} // namespace

// Scalars

TEST(LatexNumbers, integers)
{
    EXPECT_EQ(latex(Expr(3)), "3");
    EXPECT_EQ(latex(Expr(-3)), "-3");
    EXPECT_EQ(Expr(-3)->printLaTeX(2), "\\left(-3\\right)");
}

TEST(LatexLiterals, variables)
{
    EXPECT_EQ(latex(variable_s("x")), "x");
    EXPECT_EQ(latex(variable_s("M_W;M_{W}")), "M_{W}");
}

TEST(LatexLiterals, imaginaryUnit)
{
    EXPECT_EQ(latex(CSL_I), "i");
    EXPECT_EQ(latex(2 * CSL_I), "2i");
    EXPECT_EQ(latex(variable_s("a") + CSL_I * variable_s("b")), "a + i\\,b");
}

TEST(LatexSums, signs)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(a + b), "a + b");
    EXPECT_EQ(latex(a - b), "a - b");
    EXPECT_EQ(latex(a - 2 * b), "a - 2b");
}

TEST(LatexSums, negativeTermsOfAnyShape)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(a - b * c), "a - bc");
    EXPECT_EQ(latex(a - b / 2), "a - \\frac{b}{2}");
    EXPECT_EQ(latex(a - CSL_I * b), "a - i\\,b");
    EXPECT_EQ(latex(a + CSL_I * b), "a + i\\,b");
}

TEST(LatexSums, bracketsDependOnMode)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ((a + b)->printLaTeX(1), "a + b");
    EXPECT_EQ((a + b)->printLaTeX(2), "\\left(a + b\\right)");
}

TEST(LatexProducts, juxtapositionAndCoefficients)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(2 * a * b), "2ab");
    EXPECT_EQ(latex(-2 * a), "-2a");
    EXPECT_EQ(latex(-1 * a), "-a");
}

TEST(LatexProducts, dotBeforeBracketedSum)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(a * (b + c)), "a\\cdot\\left(b + c\\right)");
}

TEST(LatexFractions, negativePowersAndDivision)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(pow_s(a, -1)), "\\frac{1}{a}");
    EXPECT_EQ(latex(a / b), "\\frac{a}{b}");
    EXPECT_EQ(latex(a / (b + c)), "\\frac{a}{\\left(b + c\\right)}");
    EXPECT_EQ(latex(2 * a / b), "\\frac{2a}{b}");
    EXPECT_EQ(latex(a * c / b), "\\frac{ac}{b}");
}

TEST(LatexFractions, rationalCoefficientGoesToDenominator)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(a / 2), "\\frac{a}{2}");
    EXPECT_EQ(latex(a / (2 * b)), "\\frac{a}{2b}");
}

TEST(LatexNonCommuting, inversePowerOfCommutingFactorGoesToDenominator)
{
    auto x = variable_s("x");
    auto y = variable_s("y");
    EXPECT_EQ(latex(x * pow_s(y, -1)), "\\frac{x}{y}");
}

TEST(LatexNonCommuting, inversePowerOfLoneNonCommutingFactorGoesToDenominator)
{
    auto x = variable_s("x");
    auto A = variable_s("A");
    A->setCommutable(false);
    EXPECT_EQ(latex(pow_s(A, -1) * x), "\\frac{x}{A}");
}

TEST(LatexNonCommuting, inversePowerStaysInNumeratorWithNonCommutingFactor)
{
    auto A = variable_s("A");
    auto B = variable_s("B");
    A->setCommutable(false);
    B->setCommutable(false);
    EXPECT_EQ(latex(pow_s(A, -1) * B), "A^{-1}B");
    EXPECT_EQ(latex(B * pow_s(A, -1)), "BA^{-1}");
    EXPECT_EQ(latex(pow_s(A, -1) * pow_s(B, -1)), "A^{-1}B^{-1}");
}

TEST(LatexNonCommuting, mixOfCommutingDenominatorAndNonCommutingNumerator)
{
    auto x = variable_s("x");
    auto y = variable_s("y");
    auto A = variable_s("A");
    auto B = variable_s("B");
    A->setCommutable(false);
    B->setCommutable(false);
    EXPECT_EQ(latex(x * pow_s(y, -1) * pow_s(A, -1) * B),
              "\\frac{A^{-1}Bx}{y}");
}

TEST(LatexPowers, bracesAndBases)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(pow_s(a, 2)), "a^{2}");
    EXPECT_EQ(latex(pow_s(a, pow_s(b, c))), "a^{b^{c}}");
    EXPECT_EQ(latex(pow_s(a + b, 2)), "\\left(a + b\\right)^{2}");
    EXPECT_EQ(latex(pow_s(a, -2)), "\\frac{1}{a^{2}}");
    EXPECT_EQ(latex(sqrt_s(a)), "\\sqrt{a}");
    EXPECT_EQ(latex(sqrt_s(a + b)), "\\sqrt{a + b}");
}

TEST(LatexFunctions, commonFunctions)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(abs_s(a)), "\\left|a\\right|");
    EXPECT_EQ(latex(cos_s(a)), "\\cos\\left(a\\right)");
    EXPECT_EQ(latex(exp_s(a + b)), "e^{a + b}");
}

TEST(LatexOutput, noTrailingNewlineAtTopLevel)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    for (Expr const &e : {Expr(3), a, a + b, a * b, pow_s(a, -1), cos_s(a)}) {
        auto out = e->printLaTeX(0);
        ASSERT_FALSE(out.empty());
        EXPECT_NE(out.back(), '\n') << out;
    }
}

// Implicit tensor chains: y^T M_1 ... M_n x

namespace {

struct Mat {
    Tensor &tensor;
    Expr    operator()(Index const &a, Index const &b) const
    {
        return tensor(std::vector<Index>{a, b});
    }
};

struct ChainSetup {
    Space  V{"V", 3};
    Tensor y{"y", &V};
    Tensor x{"x", &V};
    Tensor M1T{"M1;M_{1}", {&V, &V}};
    Tensor M2T{"M2;M_{2}", {&V, &V}};
    Tensor M3T{"M3;M_{3}", {&V, &V}};
    Mat    M1{M1T};
    Mat    M2{M2T};
    Mat    M3{M3T};
    Index  i = V.generateIndex("i");
    Index  j = V.generateIndex("j");
    Index  k = V.generateIndex("k");
    Index  l = V.generateIndex("l");

    ChainSetup()
    {
        V.setMatrixPrinting(true);
    }
};

} // namespace

TEST(LatexMatrixChains, defaultSpaceUsesIndexNotation)
{
    Space  W{"W", 3};
    Tensor y{"y", &W};
    Tensor M{"M", {&W, &W}};
    Index  i = W.generateIndex("i");
    Index  j = W.generateIndex("j");
    EXPECT_FALSE(W.getMatrixPrinting());
    EXPECT_EQ(latex(y(i) * M(std::vector<Index>{i, j})), "M_{ij}y_{i}");
}

TEST(LatexMatrixChains, extraIndicesArePrintedAsSuperscripts)
{
    ChainSetup s;
    Space      lorentz{"lorentz", 4};
    Tensor G{"G;\\gamma", std::vector<const Space *>{&lorentz, &s.V, &s.V}};
    Index  mu = lorentz.generateIndex("\\mu");
    EXPECT_EQ(latex(s.y(s.i) * G(std::vector<Index>{mu, s.i, s.j}) * s.x(s.j)),
              "y^{T}\\gamma^{\\mu}x");
}

TEST(LatexMatrixChains, scalarProductOfVectors)
{
    ChainSetup s;
    EXPECT_EQ(latex(s.y(s.i) * s.x(s.i)), "x^{T}y");
}

TEST(LatexMatrixChains, singleMatrix)
{
    ChainSetup s;
    EXPECT_EQ(latex(s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j)), "y^{T}M_{1}x");
}

TEST(LatexMatrixChains, severalMatricesInMatrixOrder)
{
    ChainSetup s;
    auto expr = s.y(s.i) * s.M1(s.i, s.j) * s.M2(s.j, s.k) * s.M3(s.k, s.l)
                * s.x(s.l);
    EXPECT_EQ(latex(expr), "y^{T}M_{1}M_{2}M_{3}x");
}

TEST(LatexMatrixChains, reorderedFactorsArePrintedInMatrixOrder)
{
    ChainSetup s;
    auto expr = s.M3(s.k, s.l) * s.x(s.l) * s.M1(s.i, s.j) * s.M2(s.j, s.k)
                * s.y(s.i);
    EXPECT_EQ(latex(expr), "y^{T}M_{1}M_{2}M_{3}x");
}

TEST(LatexMatrixChains, coefficientAndScalarFactorsStayOutside)
{
    ChainSetup s;
    auto       a     = variable_s("a");
    auto       chain = s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j);
    EXPECT_EQ(latex(2 * chain), "2y^{T}M_{1}x");
    EXPECT_EQ(latex(a * chain), "ay^{T}M_{1}x");
}

TEST(LatexMatrixChains, orientationFollowsTheStoredIndices)
{
    ChainSetup s;
    // y_i M1_{ji} x_j = x^T M1 y
    EXPECT_EQ(latex(s.y(s.i) * s.M1(s.j, s.i) * s.x(s.j)), "x^{T}M_{1}y");
}

TEST(LatexMatrixChains, nonCommutingFactorInOrderIsPrinted)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    auto expr = s.y(s.i) * s.M1(s.i, s.j) * s.M2(s.j, s.k) * s.x(s.k);
    EXPECT_NO_THROW(latex(expr));
}

TEST(LatexMatrixChains, reorderedNonCommutingFactorsArePrintedTransposed)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    s.M3T->setCommutable(false);
    // The matrix chain y M3 M2 x would multiply M3 first, but M2 comes first
    // in the product. The transposed chain x^T M2^T M3^T y keeps that order.
    auto expr = s.y(s.i) * s.M2(s.j, s.k) * s.M3(s.i, s.j) * s.x(s.k);
    EXPECT_EQ(latex(expr), "x^{T}{M_{2}}^{T}{M_{3}}^{T}y");
}

TEST(LatexMatrixChains, nonCommutingFactorsInOrderArePrinted)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    s.M3T->setCommutable(false);
    auto expr = s.y(s.i) * s.M2(s.i, s.j) * s.M3(s.j, s.k) * s.x(s.k);
    EXPECT_NO_THROW(latex(expr));
}

TEST(LatexMatrixChains, disabledSpaceFallsBackToIndexNotation)
{
    ChainSetup s;
    s.V.setMatrixPrinting(false);
    EXPECT_EQ(latex(s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j)),
              "x_{j}y_{i}{M_{1}}_{ij}");
    s.V.setMatrixPrinting(true);
    EXPECT_EQ(latex(s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j)), "y^{T}M_{1}x");
}

TEST(LatexMatrixChains, openChainFallsBackToIndexNotation)
{
    ChainSetup s;
    EXPECT_EQ(latex(s.y(s.i) * s.M1(s.i, s.j)), "y_{i}{M_{1}}_{ij}");
}

// Traces: Tr(M_1 ... M_n) for closed chains of matrices

TEST(LatexTraces, singleMatrix)
{
    ChainSetup s;
    EXPECT_EQ(latex(s.M1(s.i, s.i)), "\\mathrm{Tr}\\left(M_{1}\\right)");
}

TEST(LatexTraces, productOfTwoInMatrixOrder)
{
    ChainSetup s;
    EXPECT_EQ(latex(s.M1(s.i, s.j) * s.M2(s.j, s.i)),
              "\\mathrm{Tr}\\left(M_{1}M_{2}\\right)");
}

TEST(LatexTraces, productOfThree)
{
    ChainSetup s;
    auto       expr = s.M3(s.k, s.i) * s.M1(s.i, s.j) * s.M2(s.j, s.k);
    EXPECT_EQ(latex(expr), "\\mathrm{Tr}\\left(M_{1}M_{2}M_{3}\\right)");
}

TEST(LatexTraces, transposedFactor)
{
    ChainSetup s;
    // M1_{ij} M2_{ij} = Tr(M1^T M2)
    EXPECT_EQ(latex(s.M1(s.i, s.j) * s.M2(s.i, s.j)),
              "\\mathrm{Tr}\\left({M_{1}}^{T}M_{2}\\right)");
}

TEST(LatexTraces, singleNonCommutingFactorIsPrinted)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    EXPECT_NO_THROW(latex(s.M1(s.i, s.j) * s.M2(s.j, s.i)));
}

TEST(LatexTraces, nonCommutingTraceIsPrinted)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    s.M3T->setCommutable(false);
    // A trace is invariant under cyclic permutations: no swap is needed.
    EXPECT_NO_THROW(latex(s.M3(s.k, s.j) * s.M2(s.j, s.k)));
}

TEST(LatexTraces, disabledSpaceFallsBackToIndexNotation)
{
    ChainSetup s;
    s.V.setMatrixPrinting(false);
    EXPECT_EQ(latex(s.M1(s.i, s.i)), "{M_{1}}_{ii}");
}

// Composite expressions mixing several patterns

TEST(LatexComposite, chainMinusFraction)
{
    ChainSetup s;
    auto       a    = variable_s("a");
    auto       b    = variable_s("b");
    auto       expr = s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j) - a / b;
    EXPECT_EQ(latex(expr), "-\\frac{a}{b} + y^{T}M_{1}x");
}

TEST(LatexComposite, sumRaisedToPowerWithFraction)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(pow_s(a + pow_s(b, -1), 2)),
              "\\left(a + \\frac{1}{b}\\right)^{2}");
}

TEST(LatexComposite, coefficientFunctionsAndDenominator)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(2 * abs_s(a) * cos_s(b) / c),
              "\\frac{2\\left|a\\right|\\cos\\left(b\\right)}{c}");
}

TEST(LatexComposite, fractionInExponent)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    EXPECT_EQ(latex(pow_s(a, b / c)), "a^{\\frac{b}{c}}");
}

TEST(LatexComposite, coefficientTimesChainOverRational)
{
    ChainSetup s;
    auto       a    = variable_s("a");
    auto       expr = a * s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j) / 2;
    EXPECT_EQ(latex(expr), "\\frac{ay^{T}M_{1}x}{2}");
}

TEST(LatexComposite, negativeChainTerm)
{
    ChainSetup s;
    auto       a    = variable_s("a");
    auto       expr = a - s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j);
    EXPECT_EQ(latex(expr), "a - y^{T}M_{1}x");
}

TEST(LatexComposite, imaginaryTimesChain)
{
    ChainSetup s;
    auto       expr = CSL_I * s.y(s.i) * s.M1(s.i, s.j) * s.x(s.j);
    EXPECT_EQ(latex(expr), "i\\,y^{T}M_{1}x");
}

TEST(LatexComposite, traceAndFraction)
{
    ChainSetup s;
    auto       a    = variable_s("a");
    auto       b    = variable_s("b");
    auto       expr = s.M1(s.i, s.j) * s.M2(s.j, s.i) + a / b;
    EXPECT_EQ(latex(expr),
              "\\frac{a}{b} + \\mathrm{Tr}\\left(M_{1}M_{2}\\right)");
}

TEST(LatexComposite, reorderedNonCommutingChainInsideSumIsTransposed)
{
    ChainSetup s;
    s.M2T->setCommutable(false);
    s.M3T->setCommutable(false);
    auto a    = variable_s("a");
    auto expr = a + s.y(s.i) * s.M2(s.j, s.k) * s.M3(s.i, s.j) * s.x(s.k);
    EXPECT_EQ(latex(expr), "a + x^{T}{M_{2}}^{T}{M_{3}}^{T}y");
}

TEST(LatexPrintLatex, dummyPairsAreRelabelledInOrder)
{
    Space  S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p1   = S.generateIndex("alpha");
    Index  p2   = S.generateIndex("alpha");
    Index  p3   = S.generateIndex("alpha");
    auto   expr = x(p1) * y(p1) * x(p2) * y(p2) * x(p3) * y(p3);
    EXPECT_EQ(printLatex(expr),
              "x_{\\alpha}x_{\\beta}x_{\\alpha_{1}}y_{\\alpha}y_{\\beta}y_{"
              "\\alpha_{1}}");
}

TEST(LatexPrintLatex, freeIndexNamesAreSkipped)
{
    Space  S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Tensor z{"z", &S};
    Index  p = S.generateIndex("alpha");
    Index  f = S.generateIndex("alpha");
    EXPECT_EQ(printLatex(x(p) * y(p) * z(f)),
              "x_{\\beta}y_{\\beta}z_{\\alpha}");
}

TEST(LatexNames, prefixedNameCarriesLatexForm)
{
    EXPECT_EQ(latex(variable_s(prefixedNameWithLatex("p_", "1"))), "p_{1}");
    EXPECT_EQ(latex(variable_s(prefixedNameWithLatex("M_", "W"))), "M_{W}");
    EXPECT_EQ(latex(variable_s(prefixedNameWithLatex("c_", "W;\\gamma"))),
              "c_{\\gamma}");
    EXPECT_EQ(getRegularAndLatexNames(prefixedNameWithLatex("V_", "W")).first,
              "V_W");
}

TEST(LatexPrintLatex, expressionWithoutDummiesIsUnchanged)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(printLatex(a + b), "a + b");
}

TEST(LatexPrintLatex, relabelDoesNotModifyTheInput)
{
    Space  S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    Tensor x{"x", &S};
    Tensor y{"y", &S};
    Index  p       = S.generateIndex("alpha");
    auto   expr    = x(p) * y(p);
    auto   renamed = RelabelIndices(expr);
    EXPECT_EQ(latex(expr), "x_{\\alpha}y_{\\alpha}");
    EXPECT_EQ(renamed->printLaTeX(0), "x_{\\alpha}y_{\\alpha}");
}

TEST(LatexPrintLatex, freeIndicesAreRenamedToo)
{
    Space  S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    Tensor z{"z", &S};
    Index  f = S.generateIndex("beta");
    EXPECT_EQ(printLatex(z(f)), "z_{\\alpha}");
}

TEST(LatexNames, kroneckerDeltaHasLatexForm)
{
    Space S{"S", 4, {"alpha;\\alpha", "beta;\\beta"}};
    Index a = S.generateIndex("alpha");
    Index b = S.generateIndex("beta");
    EXPECT_EQ(printLatex(S.getDelta()(std::vector<Index>{a, b})),
              "\\delta_{\\alpha\\beta}");
}

TEST(EvaluatedAbbreviations, abbreviationIsReplacedByItsDefinition)
{
    auto a  = variable_s("a");
    auto b  = variable_s("b");
    Expr ab = Abbrev::makeAbbreviation("Cab", a * b);
    EXPECT_EQ(printLatex(Evaluated(3 * ab, eval::abbreviation)),
              printLatex(3 * a * b));
}

TEST(EvaluatedAbbreviations, nestedAbbreviationsAreExpandedFully)
{
    auto a     = variable_s("a");
    auto b     = variable_s("b");
    Expr inner = Abbrev::makeAbbreviation("Cin", a + b);
    Expr outer = Abbrev::makeAbbreviation("Cout", 2 * inner);
    EXPECT_EQ(printLatex(Evaluated(outer, eval::abbreviation)),
              printLatex(2 * (a + b)));
}

TEST(EvaluatedAbbreviations, expressionWithoutAbbreviationsIsUnchanged)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(printLatex(Evaluated(a + b, eval::abbreviation)), "a + b");
}

TEST(EvaluatedAbbreviations, inputIsNotModified)
{
    auto        a        = variable_s("a");
    auto        b        = variable_s("b");
    Expr        ab       = Abbrev::makeAbbreviation("Cab", a * b);
    std::string before   = printLatex(ab);
    Expr        expanded = Evaluated(ab, eval::abbreviation);
    EXPECT_EQ(printLatex(expanded), printLatex(a * b));
    EXPECT_EQ(printLatex(ab), before);
}

TEST(LatexSignedIndices, metricWithEqualSignsIsG)
{
    Index mu = Minkowski.generateIndex("\\mu");
    Index nu = Minkowski.generateIndex("\\nu");
    EXPECT_EQ(printLatex((*Minkowski.getMetric())(std::vector<Index>{mu, nu})),
              "g^{\\mu\\nu}");
}

TEST(LatexSignedIndices, freeDeltaIndicesAreRelabelledUpper)
{
    Index mu = Minkowski.generateIndex("\\mu");
    Index nu = Minkowski.generateIndex("\\nu");
    nu.flipSign();
    EXPECT_EQ(printLatex((*Minkowski.getDelta())(std::vector<Index>{mu, nu})),
              "\\delta^{\\mu\\nu}");
}

TEST(LatexSignedIndices, unsignedSpaceKeepsSubscripts)
{
    Space  S{"S", 4, {"alpha", "beta"}};
    Tensor x{"x", &S};
    Index  p = S.generateIndex("alpha");
    EXPECT_EQ(x(p)->printLaTeX(0), "x_{alpha}");
}

TEST(LatexConjugate, complexVariableUsesStarInLatexAndTerminal)
{
    Expr c = GetComplexConjugate(variable_s("c", ComplexProperty::Complex));
    EXPECT_EQ(c->printLaTeX(0), "c^{*}");
    std::ostringstream terminal;
    c->print(1, terminal);
    EXPECT_EQ(terminal.str(), "c*");
}

TEST(LatexComposite, productOfSums)
{
    auto a = variable_s("a");
    auto b = variable_s("b");
    auto c = variable_s("c");
    auto d = variable_s("d");
    EXPECT_EQ(latex((a + b) * (c + d)),
              "\\left(a + b\\right)\\cdot\\left(c + d\\right)");
}

TEST(LatexComposite, productOfIndexedExpressions)
{
    ChainSetup s1;
    ChainSetup s2;
    s1.V.setMatrixPrinting(true);
    s2.V.setMatrixPrinting(false);
    auto a = variable_s("a");
    auto b = variable_s("b");
    EXPECT_EQ(latex(a * s1.y(s1.i) * s2.M1(s2.i, s2.j) * s1.M1(s1.i, s1.j)
                    * s1.x(s1.j) * s2.M2(s2.j, s2.i)),
              "ay^{T}M_{1}x{M_{1}}_{ij}{M_{2}}_{ji}");
}