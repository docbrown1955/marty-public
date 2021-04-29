#include <marty>

int main() {

    ///////////////////////////////////////////////////
    // Setting the type of model we want, between 
    // 1 and 4 (inclusive)
    // Type 2 is the MSSM-like 2HDM
    ///////////////////////////////////////////////////

    constexpr int type = 2;

    mty::TwoHDM_Model<type> THDM;
    std::cout << THDM << std::endl;
    
    return 0;
}
