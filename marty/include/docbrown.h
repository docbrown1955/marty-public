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

    ~DocBrown();

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

inline std::unique_ptr<DocBrown> emmett
    = std::make_unique<DocBrown>(
            "emmett",
            "emmett_debugger" MARTY_VERSION_STR ".h", 
            "emmett_debugger" MARTY_VERSION_STR ".cpp",
            "emmett_debugger" MARTY_VERSION_STR ".json",
            "emmett_checkpoints" MARTY_VERSION_STR ".json"
            );

}

