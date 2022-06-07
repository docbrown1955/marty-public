#include "NMFV.h"

namespace mty {

NMFV_Model::NMFV_Model(
        std::vector<MixingInfo> const &mixings,
        std::string const &t_saveFile
        )
    :mty::PMSSM_Model(t_saveFile, false)
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
    // std::cout << "Checking Hermiticity ..." << std::endl;
    // checkHermiticity();
}

void NMFV_Model::getToLowEnergyLagrangian()
{
    std::cout << "Breaking SU(2)_L symmetry ...\n";
    breakSU2LGaugeSymmetry();
    replaceWBoson();
    std::cout << "Expanding around Higgses VEV's ...\n";
    expandAroundVEVs();
    L.mergeTerms();
    std::cout << "Diagonalizing SM mass matrices ...\n";
    diagonalize2By2Matrices();
    diagonalizeYukawas();
    L.mergeTerms();
    adjustCouplingConstants();
    std::cout << "Breaking flavor symmetry ...\n";
    breakSMFlavorSymmetry();
    approximateQuarkMasses();
    approximateCKM();
    std::cout << "Diagonalizing MSSM mass matrices ...\n";
    L.mergeTerms();
    approximateSFermionMixings();
    std::cout << "Diagonalizing Neutralinos ..." << std::endl;
    diagonalizeNeutralinos();
    std::cout << "Diagonalizing Charginos ..." << std::endl;
    diagonalizeCharginos();
    std::cout << "Promoting Majorana fermions ..." << std::endl;
    promoteMajoranas();

    // Partial NMFV without quartic scalar interactions
    for (std::size_t i = 0; i != L.interaction.size(); ++i) {
        if (L.interaction[i]->getContent().size() == 4) {
            L.interaction.erase(L.interaction.begin() + i);
            --i;
        }
    }

    std::cout << "Diagonalizing SFermions ..." << std::endl;
    diagonalizeSFermions();
    renameSFermions();
    generateDiracFermions();
    gatherMasses();

    promoteToGoldstone("Gp", "W");
    promoteToGoldstone("G0", "Z");

    refresh();
}


void NMFV_Model::addPairMixing(
        Mixing mix, 
        std::size_t i, 
        std::size_t j
        )  
{
    using name_container = std::array<const char*, 3>;
    constexpr name_container ULname = { "su_L", "sc_L", "st_L" };
    constexpr name_container URname = { "su_R", "sc_R", "st_R" };
    constexpr name_container DLname = { "sd_L", "ss_L", "sb_L" };
    constexpr name_container DRname = { "sd_R", "ss_R", "sb_R" };
    constexpr name_container NLname = { "snu_e", "snu_mu", "snu_tau" };
    constexpr name_container ELname = { "se_L", "smu_L", "stau_L" };
    constexpr name_container ERname = { "se_R", "smu_R", "stau_R" };
    switch(mix) {
        case Mixing::QL:
            addAllowedMixing({ULname[i], ULname[j]});
            addAllowedMixing({DLname[i], DLname[j]});
            break;
        case Mixing::UR:
            addAllowedMixing({URname[i], URname[j]});
            break;
        case Mixing::DR:
            addAllowedMixing({DRname[i], DRname[j]});
            break;
        case Mixing::LL:
            addAllowedMixing({ELname[i], ELname[j]});
            addAllowedMixing({NLname[i], NLname[j]});
            break;
        case Mixing::ER:
            addAllowedMixing({ERname[i], ERname[j]});
            break;
        case Mixing::Au:
            addAllowedMixing({URname[i], ULname[j]});
            break;
        case Mixing::Ad:
            addAllowedMixing({DRname[i], DLname[j]});
            break;
        case Mixing::Ae:
            addAllowedMixing({ERname[i], ELname[j]});
            break;
        default:
            std::cerr << "Error: mixing " << mix << " is not defined." << '\n';
    }
}

void NMFV_Model::addMixing(MixingInfo mix)
{
    const Mixing mixing = mix.mixing;
    std::vector<std::size_t> particles = mix.mixedParticles;
    for (std::size_t pos : particles)
        HEPAssert(pos < 3,
                mty::error::IndexError,
                "Generation index for mixings must be between 0 and 2.")
    std::sort(particles.begin(), particles.end());
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

    for (std::size_t i = 0; i != particles.size(); ++i) {
        for (std::size_t j = i+1; j < particles.size(); ++j) {
            const std::size_t i_eff = particles[i];
            const std::size_t j_eff = particles[j];
            addPairMixing(mixing, i_eff, j_eff);
            std::string deltaName = "del_" + toString(mixing) + "_"
                + std::to_string(10*i_eff+j_eff);
            csl::Expr delta = csl::constant_s(
                    deltaName, csl::ComplexProperty::Complex);
            M[i_eff][j_eff] = delta/2;
            M[j_eff][i_eff] = delta/2;
        }
    }
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
