#include "comparedata.h"
#include "testutility.h"
#include <marty.h>
#include <marty/models/sm.h>

int main()
{
    for (const auto &val : mty::sm_input::all_params) {
        std::cout << val << " = " << csl::Evaluated(val, csl::eval::all)
                  << std::endl;
    }
    mty::sm_input::undefineNumericalValues();
    mty::sm_input::redefineNumericalValues();
    for (const auto &val : mty::sm_input::all_params) {
        std::cout << val << " = " << csl::Evaluated(val, csl::eval::all)
                  << std::endl;
    }

    mty::SM_Model sm;
    sm.computeModelWidths(mty::TreeLevel);

    [[maybe_unused]] int sysres = system("rm -rf libs/modelwidthLib");
    mty::Library         widthLib("modelwidthLib", "libs");
    widthLib.generateSpectrum(sm);
    defineLibPath(widthLib);
    widthLib.print();

    sysres = system("cp libsrc/example_modelwidthlib.cpp "
                    "libs/modelwidthLib/script");
    sysres = system("cd libs/modelwidthLib; make && "
                    "bin/example_modelwidthlib.x");
    return assert_equal(
        "data/output/modelwidthlib.txt", "libs/modelwidthLib/data.txt", 1e-5);
}
