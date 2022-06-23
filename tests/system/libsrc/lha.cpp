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

#include "lha.h"
#include "lhaData.h"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace mty::lha {

LHAFileData Reader::readFile(std::string const &fileName)
{
    std::ifstream f(fileName, std::ios::in);
    if (!f) {
        std::cerr << "Error: File \"" + fileName + "\" not found.\n";
    } 
    auto res = readFile(f);
    f.close();
    return res;
}

LHAFileData Reader::readFile(std::ifstream &in)
{
    LHAFileData data;
    std::vector<std::string> currentLine;
    size_t iLine = 1;
    std::string currentType = "";
    while (!in.eof()) {
        currentLine = split(nextLine(in));
        if (isBlockLine(currentLine)) {
            currentType = currentLine[1];
            data.addBlock(currentType);
        }
        else if (isValueLine(currentLine) 
                and !currentType.empty()) {
            data.addElement(currentType, readValue(currentLine));
        }
        else if (!currentLine.empty()) {
            currentType.clear();
            if (verbose) {
                std::cerr << "Warning: ignoring line " << iLine 
                    << "of lha file:\n";
                for (const auto &str: currentLine)
                    std::cerr << str << "  ";
                std::cerr << '\n';
            }
        }
        ++iLine;
    }

    return data;
}

std::string Reader::nextLine(std::ifstream &in)
{
    std::string res;
    if (in.eof())
        return res;
    std::getline(in, res);
    for (auto iter = res.begin(); iter != res.end(); ++iter)
        if (*iter == '#') {
            res.erase(iter, res.end());
            break;
        }

    return res;
}

std::vector<std::string> Reader::split(std::string const &line)
{
    std::vector<std::string> res;
    res.reserve(10);
    auto iter = line.begin();
    auto first = line.end();
    while (iter != line.end()) {
        if (*iter == ' ' or *iter == '\n') {
            if (first < iter) {
                res.emplace_back(first, iter);
                first = line.end();
            }
        }
        else if (first == line.end()) {
            first = iter;
        }
        ++iter;
    }
    if (first != line.end())
        res.emplace_back(first, line.end());

    return res;
}

static std::string tolower(std::string const &str)
{
    std::string cpy(str);
    std::for_each(cpy.begin(), cpy.end(), [&](char &c) {
        c = std::tolower(c);
    });
    return cpy;
}

bool Reader::isBlockLine(std::vector<std::string> const &line)
{
    return line.size() >= 2
        and tolower(line[0]) == "block";
}

bool Reader::isPositiveInteger(std::string const &str)
{
    try {
        std::stoull(str);
        return true;
    }
    catch (std::out_of_range&) {
        return false;
    }
    catch (std::invalid_argument&) {
        return false;
    }
}

bool Reader::isNumber(std::string const &str)
{
    try {
        std::stold(str);
        return true;
    }
    catch (std::out_of_range&) {
        return false;
    }
    catch (std::invalid_argument&) {
        return false;
    }
}

bool Reader::isValueLine(std::vector<std::string> const &line)
{
    if (line.size() != 2 and line.size() != 3)
        return false;
    if (!isPositiveInteger(line[0]) or !isNumber(line.back()))
        return false;
    return line.size() == 2 or isPositiveInteger(line[1]);
}

LHAElement Reader::readValue(std::vector<std::string> const &line)
{
    LHAElement element;
    element.id    = std::stoull(line[0]);
    element.value = std::stold (line.back());
    if (line.size() == 3)
        element.id_sup = std::stoull(line[1]);

    return element;
}

} // End of namespace mty::lha
