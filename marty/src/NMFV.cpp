#include "NMFV.h"

namespace mty {

NMFV_Model::NMFV_Model(
        std::vector<MixingInfo> const &mixings,
        std::string const &t_slhaFile,
        std::string const &t_saveFile
        )
    :mty::PMSSM_Model(t_slhaFile, t_saveFile, false)
{
    std::cout << "Initializing gauge and particle content ..." << std::endl;
    initContent();
    std::cout << "Initializing interactions ..." << std::endl;
    initInteractions();
    std::cout << "Gathering MSSM inputs ..." << std::endl;
    gatherMSSMInputs();
    approximateYukawa();
    approximateInputMatrices();

    /////////////////////
    // NMFV-specific part
    /////////////////////

    for (const auto &mix : mixings)
        addMixing(mix);
    
    /////////////////////

    std::cout << "Getting to low energy Lagrangian ..." << std::endl;
    getToLowEnergyLagrangian();
    std::cout << "Checking Hermiticity ..." << std::endl;
    checkHermiticity();
}

void NMFV_Model::addMixing(MixingInfo mix)
{
    addAllowedMixing({"se_L", "smu_L", "se_R", "smu_R"});
    const size_t i = mix.i;
    const size_t j = mix.j;
    const Mixing mixing = mix.mixing;
    HEPAssert(i < 3 && j < 3,
            mty::error::IndexError,
            "Generation index for mixings must be between 0 and 2.")
    if (j < i) {
        return addMixing(mix);
    }
    csl::Expr M;
    switch(mixing) {
        case Mixing::QL: M = M2_s_Q->getTensor(); break;
        case Mixing::UR: M = M2_s_U->getTensor(); break;
        case Mixing::DR: M = M2_s_D->getTensor(); break;
        case Mixing::LL: M = M2_s_L->getTensor(); break;
        case Mixing::ER: M = M2_s_E->getTensor(); break;
        case Mixing::Au: M = Au->getTensor();     break;
        case Mixing::Ad: M = Ad->getTensor();     break;
        case Mixing::Ae: M = Ae->getTensor();     break;
    }

    std::string deltaName = "del_" + toString(mixing) + "_"
        + std::to_string(10*i+j);
    csl::Expr delta = csl::constant_s(deltaName, csl::ComplexProperty::Complex);
    M[i][j] = delta/2;
    M[j][i] = delta/2;
}

std::ostream &operator<<(
        std::ostream      &out,
        NMFV_Model::Mixing mixing
        )
{
    switch(mixing) {
        case NMFV_Model::Mixing::QL: out << "QL"; break;
        case NMFV_Model::Mixing::UR: out << "UR"; break;
        case NMFV_Model::Mixing::DR: out << "DR"; break;
        case NMFV_Model::Mixing::LL: out << "LL"; break;
        case NMFV_Model::Mixing::ER: out << "ER"; break;
        case NMFV_Model::Mixing::Au: out << "Au"; break;
        case NMFV_Model::Mixing::Ad: out << "Ad"; break;
        case NMFV_Model::Mixing::Ae: out << "Ae"; break;
    }

    return out;
}

}
