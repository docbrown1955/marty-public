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

/**
 * @file docbrown.h
 * @brief Main utilities for doc brown's debugger.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2021-05-06
 */
#pragma once

#include "checkpoint.h"
#include "fileData.h"
#include "csldatahandler.h"

namespace mty::doc {

class DocBrown {

    friend class DocBuilder;

public:

    DocBrown(
            std::string_view path,
            std::string_view headerName,
            std::string_view sourceName,
            std::string_view jsonCSLDataName,
            std::string_view jsonCheckPointNames
            );

    void addCheckPoint(std::shared_ptr<CheckPoint_Base> const &cp);

    void writeData();

    static
    std::string join(
            std::string_view path,
            std::string_view file
            );

private:

    void ensurePath();
    void writeCheckPointsData();
    void writeCSLData();
    void writeComparator();

private:

    CSLDataHandler cslHandler;
    FileData       fileData;
    std::vector<std::shared_ptr<CheckPoint_Base>> checkPoints;
    std::string path;
    std::string headerName;
    std::string sourceName;
    std::string jsonCSLData;
    std::string jsonCheckPoints;
};

#ifdef DOC_BROWN_DEBUG_MODE
    inline std::unique_ptr<DocBrown> emmett
        = std::make_unique<DocBrown>(
                "emmett",
                "emmett_debugger" MARTY_VERSION_STR ".h", 
                "emmett_debugger" MARTY_VERSION_STR ".cpp",
                "emmett_debugger" MARTY_VERSION_STR ".json",
                "emmett_checkpoints" MARTY_VERSION_STR ".json"
                );
#endif

}
