#include <iostream>
#include <marty/lha/lha.h>

void assert_eq_double(double a, std::optional<double> b)
{
    if (!b) 
    {
        std::cerr << "Error in assert_eq_double(): " 
                  << "No valid value to compare!\n";
        exit(1);
    }
    if (std::abs(a - b.value()) > 1e-10)
    {
        std::cerr << "Error in assert_eq_double(): " 
		  << a << " and " << b.value() << " are different!\n";
        exit(1);
    }
}

int main() {

    mty::lha::LHAFileData lhaData = mty::lha::Reader::readFile("data/input/test.lha");
    std::cout << lhaData << std::endl; 

    // STANDARD BLOCK
    std::cout << "CHECK STANDARD BLOCK\n";
    assert_eq_double(1.0,   lhaData.getValue("STANDARD", 0));
    assert_eq_double(2.5e0, lhaData.getValue("STANDARD", 1));

    // SINGLE BLOCK
    std::cout << "CHECK SINGLE BLOCK\n";
    assert_eq_double(5.67e-3, lhaData.getValue("SINGLE", 0));

    // MATRIX BLOCK
    std::cout << "CHECK MATRIX BLOCK\n";
    assert_eq_double(1.0,    lhaData.getValue("MATRIX", 0, 0));
    assert_eq_double(2.0,    lhaData.getValue("MATRIX", 0, 1));
    assert_eq_double(3.0e+5, lhaData.getValue("MATRIX", 1, 0));
    assert_eq_double(4.0,    lhaData.getValue("MATRIX", 1, 1));

    return 0;
}
