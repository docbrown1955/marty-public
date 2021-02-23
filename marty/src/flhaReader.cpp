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

#include "flhaReader.h"
#include "mrtError.h"
#include <fstream>
#include <sstream>

namespace mty::flha {

    void Reader::readFLHA(std::string const& fileName)
    {
        std::ifstream f(fileName);
        HEPAssert(bool(f),
                mty::error::NameError,
                "File " + fileName + " not found.\n");
        readFLHA(f);
        f.close();
    }

    void Reader::readFLHA(std::ifstream &file)
    {
        std::list<std::string> line;
        while (not file.eof()) {
            line = parseLine(file);
            interpretLine(line);
        }
    }

    std::list<std::string> Reader::parseLine(std::ifstream& in)
    {
        std::list<std::string> line;
        std::string raw;
        std::getline(in, raw);
        for (auto iter = raw.begin(); iter != raw.end(); ++iter)
            if (*iter == '#') {
                raw.erase(iter, raw.end());
                break;
            }

        std::istringstream sin(raw);
        while (not sin.eof()) {
            std::string element;
            sin >> element;
            if (not element.empty())
                line.push_back(element);
        }

        return line;
    }

    void Reader::interpretLine(std::list<std::string> line)
    {
        if (line.empty())
            return;
        if (line.front() == "BLOCK"
                or line.front() == "Block") {
            line.pop_front();
            HEPAssert(line.size() == 1,
                    mty::error::NameError,
                    "BLOCK line in .flha should contain only 2 keywords.");
            auto nameBlock = stringToBlock(line.front());
            for (size_t i = 0; i != blocks.size(); ++i)
                if (nameBlock == blocks[i].first) {
                    indexBlock = i;
                    return;
                }
            indexBlock = -1;
        }
        else if (indexBlock != size_t(-1)){
            HEPAssert(line.size() >= 2,
                    mty::error::NameError,
                    "Parameter line in flha file should have at least 2"
                     + std::string(" numbers: id and value of the parameter."));
            std::istringstream sin(line.front());
            size_t id;
            sin >> id;
            line.pop_front();
            std::vector<int> indices;
            while (line.size() > 1) {
                int index;
                sin = std::istringstream(line.front());
                line.pop_front();
                sin >> index;
                indices.push_back(index - 1);
            }
            long double value;
            sin = std::istringstream(line.front());
            sin >> value;
            if (!indices.empty()) {
                indices.insert(indices.begin(), id-1);
                id = 0;
            }
            setValue(id, value, indices);
        }
    }

    void Reader::setValue(size_t                  id,
                          long double             value,
                          std::vector<int> const& indices)
    {
        HEPAssert(indexBlock < blocks.size(),
                mty::error::RuntimeError,
                "Block not given for id " + toString(id));
        auto flag = blocks[indexBlock].second.find(InputFlag(id));
        HEPAssert(flag != blocks[indexBlock].second.end(),
                mty::error::RuntimeError,
                "Id " + toString(id) + " not found for block.");
        flag->readValue(value, indices);
    }

} // namespace flha

