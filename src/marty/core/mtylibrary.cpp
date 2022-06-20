#include "mtylibrary.h"

#ifndef MARTY_INSTALL_PATH
#define MARTY_INSTALL_PATH / usr / local
#endif
#define XSTRINGIFY(s) #s
#define STRINGIFY(s) XSTRINGIFY(s)

namespace mty {
void Library::setPathToMarty()
{
    pathToMarty = STRINGIFY(MARTY_INSTALL_PATH);
}

} // namespace mty
