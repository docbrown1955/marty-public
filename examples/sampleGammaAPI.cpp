#include <marty.h>
#include <marty/api/gamma.h>

namespace gapi                      = mty::gamma_api;
constexpr auto g                    = mty::gamma_api::g;
constexpr auto eps                  = mty::gamma_api::eps;
constexpr gapi::Expr (*G)(int)      = mty::gamma_api::gamma;
constexpr gapi::Expr (*S)(int, int) = mty::gamma_api::gamma;
constexpr auto G5                   = mty::gamma_api::gamma5;
constexpr auto PL                   = mty::gamma_api::P_L;
constexpr auto PR                   = mty::gamma_api::P_R;
constexpr auto fcurrent               = mty::gamma_api::current;

void logLatexEquation(gapi::Expr const &lhs, gapi::Expr const &rhs)
{
    static std::ofstream fout("equations.tex");
    fout << "\n";
    fout << "\\begin{equation}\n";
    fout << "\t\\begin{split}\n";
    fout << "\t\t" << gapi::generateLatex(lhs) << " = "
         << gapi::generateLatex(rhs) << '\n';
    fout << "\t\\end{split}\n";
    fout << "\\end{equation}\n";
}

void testChainSimplification(gapi::Expr const &chain)
{
    std::cout << "Initial chain:    " << chain << std::endl;
    std::cout << "Simplified chain: " << gapi::simplified(chain) << std::endl;
    logLatexEquation(chain, gapi::simplified(chain));
}

void testFierzSimplification(gapi::Expr const &chain1,
                             gapi::Expr const &chain2)
{
    std::cout << "Initial chains:   " << chain1 << " X " << chain2
              << std::endl;
    gapi::Expr res = gapi::simplified(gapi::applySingleFierz(chain1, chain2));
    std::cout << "Simplified chain: " << res << std::endl;
    std::cout << sgl::sgl_to_csl(
        res, sgl::buildTensorSet(&csl::Minkowski, &mty::dirac4))
              << std::endl;
    logLatexEquation(chain1 * chain2, res);
}

void testFierzSimplification2(gapi::Expr const &chain1,
                              gapi::Expr const &chain2)
{
    std::cout << "Initial chains:   " << chain1 << " X " << chain2
              << std::endl;
    gapi::Expr res = gapi::simplified(gapi::applyDoubleFierz(chain1, chain2));
    std::cout << "Simplified chain: " << res << std::endl;
    logLatexEquation(chain1 * chain2, res);
}

int main()
{
    gapi::keepSymbolic4D(false);

    std::cout << G(1) << std::endl;
    std::cout << g(0, 1) << std::endl;
    std::cout << g(0, 0) << std::endl;
    std::cout << eps(0, 1, 2, 3) << std::endl;
    std::cout << eps(0, 0, 2, 3) << std::endl;

    testChainSimplification(fcurrent({G(1), G5(), G(1)}, 0, 1));
    testChainSimplification(fcurrent({G(1), G(2)}, 0, 0));
    testChainSimplification(fcurrent({G(1), G(2), G(3), G(4), G5()}, 0, 0));

    testFierzSimplification2(fcurrent({G(1), PL()}, 0, 1),
                             fcurrent({G(1), PR()}, 2, 3));
    testFierzSimplification2(fcurrent({G(1), PL()}, 0, 1),
                             fcurrent({G(1), PL()}, 2, 3));
    testFierzSimplification2(fcurrent({G(1), G(2), G(3), PL()}, 0, 1),
                             fcurrent({G(1), G(2), G(3), PL()}, 2, 3));
    testFierzSimplification2(fcurrent({G(1), G(2), G(3), PL()}, 0, 1),
                             fcurrent({G(1), G(2), G(3), PR()}, 2, 3));
    testFierzSimplification(fcurrent({PL()}, 0, 1), fcurrent({PR()}, 2, 3));
    testFierzSimplification(fcurrent({PL()}, 0, 1), fcurrent({PL()}, 2, 3));

    testChainSimplification(fcurrent({S(0, 1), S(2, 3), PL()}, 0, 0));
    testFierzSimplification(fcurrent({S(0, 1)}, 0, 1),
                            fcurrent({S(0, 1), PL()}, 2, 3));
    testFierzSimplification(fcurrent({S(0, 1), PL()}, 0, 1),
                            fcurrent({S(0, 1), PL()}, 2, 3));

    return 0;
}
