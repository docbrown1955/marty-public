#include "modelwidthlib.h"
#include <fstream>
#include <iostream>

using namespace modelwidthlib;

int main()
{

    param_t       params;
    std::ofstream out("data.txt");
    for (const auto &gamma_func : f_G) {
        out << gamma_func(params).real() << '\t';
    }
    out << std::endl;

    return 0;
}