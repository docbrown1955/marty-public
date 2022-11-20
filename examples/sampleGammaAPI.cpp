#include <marty.h>
#include <marty/api/gamma.h>

namespace gapi        = mty::gamma_api;
constexpr auto g      = mty::gamma_api::g;
constexpr auto eps    = mty::gamma_api::eps;
constexpr auto G      = mty::gamma_api::gamma;
constexpr auto G5     = mty::gamma_api::gamma5;
constexpr auto PL     = mty::gamma_api::P_L;
constexpr auto PR     = mty::gamma_api::P_R;
constexpr auto fchain = mty::gamma_api::chain;

void testChainSimplification(gapi::Expr const &chain)
{
    std::cout << "Initial chain:    " << chain << std::endl;
    std::cout << "Simplified chain: " << gapi::simplified(chain) << std::endl;
}

void testFierzSimplification(gapi::Expr const &chain1,
                             gapi::Expr const &chain2)
{
    std::cout << "Initial chains:   " << chain1 << " X " << chain2
              << std::endl;
    std::cout << "Simplified chain: "
              << gapi::simplified(gapi::applyDoubleFierz(chain1, chain2))
              << std::endl;
}

int main()
{
    gapi::keepSymbolic4D(false);

    std::cout << G(1) << std::endl;
    std::cout << g(0, 1) << std::endl;
    std::cout << g(0, 0) << std::endl;
    std::cout << eps(0, 1, 2, 3) << std::endl;
    std::cout << eps(0, 0, 2, 3) << std::endl;

    testChainSimplification(fchain({G(1), G5(), G(1)}, 0, 1));
    testChainSimplification(fchain({G(1), G(2)}, 0, 0));
    testChainSimplification(fchain({G(1), G(2), G(3), G(4), G5()}, 0, 0));

    testFierzSimplification(fchain({G(1), PL()}, 0, 1),
                            fchain({G(1), PR()}, 2, 3));
    testFierzSimplification(fchain({G(1), PL()}, 0, 1),
                            fchain({G(1), PL()}, 2, 3));
    testFierzSimplification(fchain({G(1), G(2), G(3), PL()}, 0, 1),
                            fchain({G(1), G(2), G(3), PL()}, 2, 3));
    testFierzSimplification(fchain({G(1), G(2), G(3), PL()}, 0, 1),
                            fchain({G(1), G(2), G(3), PR()}, 2, 3));

    return 0;
}
