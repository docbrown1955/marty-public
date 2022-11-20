#include <marty.h>
#include <marty/api/gamma.h>

namespace gapi = mty::gamma_api;

void testChainSimplification(gapi::Expr const &chain)
{
    std::cout << "Initial chain:    " << chain << std::endl;
    std::cout << "Simplified chain: " << gapi::simplified(chain) << std::endl;
}

void testFierzSimplification(gapi::Expr const &chain1, gapi::Expr const &chain2)
{
    std::cout << "Initial chains:   " << chain1 << " X " << chain2 << std::endl;
    std::cout << "Simplified chain: " << gapi::simplified(gapi::applyDoubleFierz(chain1, chain2)) << std::endl;
}

int main()
{
    gapi::keepSymbolic4D(false);

    std::cout << gapi::gamma(1) << std::endl;
    std::cout << gapi::g(0, 1) << std::endl;
    std::cout << gapi::g(0, 0) << std::endl;
    std::cout << gapi::eps(0, 1, 2, 3) << std::endl;
    std::cout << gapi::eps(0, 0, 2, 3) << std::endl;

    testChainSimplification(gapi::chain(
        {gapi::gamma(1), gapi::gamma5(), gapi::gamma(1)}, 
	0, 1));
    testChainSimplification(gapi::chain(
        {gapi::gamma(1), gapi::gamma(2)}, 
	0, 0));
    testChainSimplification(gapi::chain(
        {gapi::gamma(1), gapi::gamma(2), gapi::gamma(3), gapi::gamma(4), gapi::gamma5()},
       	0, 0));

    testFierzSimplification(
        gapi::chain({gapi::gamma(1), gapi::P_L()}, 0, 1),
	gapi::chain({gapi::gamma(1), gapi::P_R()}, 2, 3)
    );
    testFierzSimplification(
        gapi::chain({gapi::gamma(1), gapi::P_L()}, 0, 1),
	gapi::chain({gapi::gamma(1), gapi::P_L()}, 2, 3)
    );


    return 0;
}
