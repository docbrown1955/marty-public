#include <csl>

using csl::Expr;

int main() {

    Expr metricTensor = csl::matrix_s(
            {{CSL_1, CSL_0},
             {CSL_0, CSL_M_1}});

    csl::Space minko2D("Minko2D", 2, "g", metricTensor);
    csl::Space dirac2D("Dirac2D", 2);

    Expr gammaTensor = csl::highdtensor_s(
            {
            {{CSL_0, CSL_1},
             {CSL_1, CSL_0}},

            {{-CSL_I, CSL_0},
             {CSL_0,  CSL_I}}
            }
            );
    csl::Tensor gamma = csl::tensor_s(
            "\\gamma",
            {&minko2D, &dirac2D, &dirac2D},
            gammaTensor);
    csl::SetComplexProperty(gamma, csl::ComplexProperty::Complex);

    csl::Tensor delta = csl::GetDelta(&dirac2D);

    csl::Index mu = csl::GenerateIndex(&minko2D, "\\mu");
    csl::Index a1 = csl::GenerateIndex(&dirac2D, "a");
    csl::Index a2 = csl::GenerateIndex(&dirac2D, "a");
    csl::Index a3 = csl::GenerateIndex(&dirac2D, "a");
    csl::Index a4 = csl::GenerateIndex(&dirac2D, "a");

    Expr toSimplify = gamma({+mu, a1, a2}) * csl::GetHermitianConjugate(
            gamma({mu, a4, a3}) * gamma({0, a3, a2}), &dirac2D);
    std::cout << toSimplify << std::endl;

    csl::AddSelfContraction(
            gamma,
            gamma({0, a1, a2}),
            gamma({0, a2, a3}),
            delta({a1, a3}));
    csl::AddSelfContraction(
            gamma,
            gamma({1, a1, a2}),
            gamma({1, a2, a3}),
            -delta({a1, a3}));
    csl::AddSelfContraction(
            gamma,
            gamma({mu, a1, a2}),
            gamma({+mu, a2, a3}),
            2 * delta({a1, a3}));
    csl::AddHermitianProperty(
            gamma,
            &dirac2D,
            gamma({mu, a1, a2}),
            gamma({0, a2, a3})*gamma({mu, a3, a4})*gamma({0, a4, a1})
            );

    toSimplify = gamma({+mu, a1, a2}) * csl::GetHermitianConjugate(
            gamma({mu, a4, a3}) * gamma({0, a3, a2}), &dirac2D);
    std::cout << toSimplify << std::endl;

    return 0;
}
