// This file is part of MARTY.
// 
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

#include "flhaBlock.h"
#include "mrtError.h"

namespace mty::flha {

    BlockName stringToBlock(std::string const& block)
    {
        if (block == "SMINPUTS")
            return SMINPUTS;
        if (block == "MINPAR")
            return MINPAR;
        if (block == "EXTPAR")
            return EXTPAR;
        if (block == "MSQ2")
            return MSQ2;
        if (block == "MSL2")
            return MSL2;
        if (block == "MSU2")
            return MSu2;
        if (block == "MSD2")
            return MSd2;
        if (block == "MSE2")
            return MSe2;
        if (block == "TUIN")
            return TUIN;
        if (block == "TDIN")
            return TDIN;
        if (block == "TEIN")
            return TEIN;
        return None;
    }

    std::vector<Block> initFLHAFlags()
    {
        std::set<InputFlag> SMINPUTS_flags;
        SMINPUTS_flags.insert(InputFlag(1, mty::sm_input::alpha_em));
        SMINPUTS_flags.insert(InputFlag(2, mty::sm_input::G_F));
        SMINPUTS_flags.insert(InputFlag(3, mty::sm_input::alpha_s));
        SMINPUTS_flags.insert(InputFlag(4, mty::sm_input::M_Z));
        SMINPUTS_flags.insert(InputFlag(5, mty::sm_input::m_b));
        SMINPUTS_flags.insert(InputFlag(6, mty::sm_input::m_t));
        SMINPUTS_flags.insert(InputFlag(7, mty::sm_input::m_tau));

        Block SMINPUTS = std::make_pair(flha::SMINPUTS, SMINPUTS_flags);

        std::set<InputFlag> MINPAR_flags;
        MINPAR_flags.insert(InputFlag(1, mty::mssm_input::m_0));
        MINPAR_flags.insert(InputFlag(2, mty::mssm_input::m_1));
        MINPAR_flags.insert(InputFlag(3, mty::mssm_input::tanb));
        MINPAR_flags.insert(InputFlag(4, mty::mssm_input::sgn_mu));
        MINPAR_flags.insert(InputFlag(5, mty::mssm_input::A_0));

        Block MINPAR = std::make_pair(flha::MINPAR, MINPAR_flags);

        std::set<InputFlag> EXTPAR_flags;
        EXTPAR_flags.insert(InputFlag(25, mssm_input::tanb));
        EXTPAR_flags.insert(InputFlag(1, mssm_input::M1));
        EXTPAR_flags.insert(InputFlag(2, mssm_input::M2));
        EXTPAR_flags.insert(InputFlag(3, mssm_input::M3));
        EXTPAR_flags.insert(InputFlag(11, mssm_input::At));
        EXTPAR_flags.insert(InputFlag(12, mssm_input::Ab));
        EXTPAR_flags.insert(InputFlag(13, mssm_input::Atau));
        EXTPAR_flags.insert(InputFlag(23, mssm_input::MA));
        EXTPAR_flags.insert(InputFlag(26, mssm_input::mu));
        EXTPAR_flags.insert(InputFlag(31, mssm_input::MeL));
        EXTPAR_flags.insert(InputFlag(32, mssm_input::MmuL));
        EXTPAR_flags.insert(InputFlag(33, mssm_input::MtauL));
        EXTPAR_flags.insert(InputFlag(34, mssm_input::MeR));
        EXTPAR_flags.insert(InputFlag(35, mssm_input::MmuR));
        EXTPAR_flags.insert(InputFlag(36, mssm_input::MtauR));
        EXTPAR_flags.insert(InputFlag(41, mssm_input::Mq1L));
        EXTPAR_flags.insert(InputFlag(42, mssm_input::Mq2L));
        EXTPAR_flags.insert(InputFlag(43, mssm_input::Mq3L));
        EXTPAR_flags.insert(InputFlag(44, mssm_input::MquR));
        EXTPAR_flags.insert(InputFlag(45, mssm_input::MqcR));
        EXTPAR_flags.insert(InputFlag(46, mssm_input::MqtR));
        EXTPAR_flags.insert(InputFlag(47, mssm_input::MqdR));
        EXTPAR_flags.insert(InputFlag(48, mssm_input::MqsR));
        EXTPAR_flags.insert(InputFlag(49, mssm_input::MqbR));

        Block EXTPAR = std::make_pair(flha::EXTPAR, EXTPAR_flags);

        std::set<InputFlag> MSQ2_flags;
        MSQ2_flags.insert(InputFlag(0, mty::mssm_input::MSQ2));
        Block MSQ2(flha::MSQ2, MSQ2_flags);

        std::set<InputFlag> MSL2_flags;
        MSL2_flags.insert(InputFlag(0, mty::mssm_input::MSL2));
        Block MSL2(flha::MSL2, MSL2_flags);

        std::set<InputFlag> MSu2_flags;
        MSu2_flags.insert(InputFlag(0, mty::mssm_input::MSu2));
        Block MSu2(flha::MSu2, MSu2_flags);

        std::set<InputFlag> MSd2_flags;
        MSd2_flags.insert(InputFlag(0, mty::mssm_input::MSd2));
        Block MSd2(flha::MSd2, MSd2_flags);

        std::set<InputFlag> MSe2_flags;
        MSe2_flags.insert(InputFlag(0, mty::mssm_input::MSe2));
        Block MSe2(flha::MSe2, MSe2_flags);

        std::set<InputFlag> Tu_flags;
        Tu_flags.insert(InputFlag(0, mty::mssm_input::Tu));
        Block TUIN(flha::TUIN, Tu_flags);

        std::set<InputFlag> Td_flags;
        Td_flags.insert(InputFlag(0, mty::mssm_input::Td));
        Block TDIN(flha::TDIN, Td_flags);

        std::set<InputFlag> Te_flags;
        Te_flags.insert(InputFlag(0, mty::mssm_input::Te));
        Block TEIN(flha::TEIN, Te_flags);


        return {
            SMINPUTS, MINPAR, EXTPAR,
            MSQ2, MSL2, MSu2, MSd2, MSe2, 
            TUIN, TDIN, TEIN
        };
    }

} // End of namespace flha
