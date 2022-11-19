#include <marty.h>
#include <marty/api/gamma.h>

namespace gapi = mty::gamma_api;

int main()
{
    std::cout << gapi::gamma(1) << std::endl;
    std::cout << gapi::g(0, 1) << std::endl;
    std::cout << gapi::g(0, 0) << std::endl;
    std::cout << gapi::eps(0, 1, 2, 3) << std::endl;
    std::cout << gapi::eps(0, 0, 2, 3) << std::endl;

    std::cout << gapi::chain(
        {gapi::gamma(1), gapi::gamma(5), gapi::gamma(1)}, 0, 1)
              << std::endl;
    return 0;
}