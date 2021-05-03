#include "docbrown.h"
#include "jsonLoader.h"
#include <fstream>

namespace mty::doc {

DocBrown::DocBrown(
        std::string_view t_path,
        std::string_view t_headerName,
        std::string_view t_sourceName,
        std::string_view t_jsonCSLData,
        std::string_view t_jsonCheckPoints
        )
    :fileData(join(t_path, t_headerName), join(t_path, t_sourceName)),
    path(t_path),
    headerName(t_headerName),
    sourceName(t_sourceName),
    jsonCSLData(t_jsonCSLData),
    jsonCheckPoints(t_jsonCheckPoints)
{
    ensurePath();
}

std::string DocBrown::join(
        std::string_view path,
        std::string_view file
        )
{
    std::string res(path);
    if (res.size() > 1 and *(res.end() - 1) != '/') {
        res.insert(res.end(), '/');
    }
    res.insert(res.end(), file.begin(), file.end());
    return res;
}

void DocBrown::ensurePath() 
{
    [[maybe_unused]] int sysres = system(("mkdir -p " + path).c_str());
}

void DocBrown::writeData()
{
    writeCheckPointsData();
    writeCSLData();
    writeComparator();
}

void DocBrown::addCheckPoint(std::shared_ptr<CheckPoint_Base> const &cp)
{
    const auto pos = std::find(checkPoints.begin(), checkPoints.end(), cp);
    if (pos == checkPoints.end()) {
        checkPoints.push_back(cp);
    }
}

void DocBrown::writeCheckPointsData()
{
    auto tree    = JSON::Node::make("root");
    auto version = JSON::Leaf<std::string>::make(
            "version", 
            MARTY_VERSION_STR
            );
    tree->addChild(version);
    auto nameList = JSON::List::make("checkpoints");
    for (const auto &cp : checkPoints) {
        auto leaf = JSON::Leaf<std::string>::make("checkpoints", cp->getName());
        nameList->addChild(leaf);
    }
    tree->addChild(nameList);
    JSON::Reader::saveToFile(join(path, jsonCheckPoints), tree);
}

void DocBrown::writeCSLData()
{
    for (const auto &cp : checkPoints) {
        if (cp->hasCSLData())
            cp->updateCSLData(cslHandler);
    }
    cslHandler.printJSON(join(path, jsonCSLData));
}

void DocBrown::writeComparator()
{
    auto &header = fileData.getStream(FileData::Header);
    auto &source = fileData.getStream(FileData::Source);
    header << "#pragma once\n\n";
    header << "#include \"csl.h\"\n\n";
    header << "namespace mty::doc::" MARTY_VERSION_STR " {\n\n";
    source << "#include \"common.h\"\n";
    source << "#include \"" << headerName << "\"\n\n";
    source << "namespace mty::doc::" << MARTY_VERSION_STR " {\n\n";
    for (const auto &cp : checkPoints)
        cp->print(fileData);
    header << "} // End of namespace mty::doc::" MARTY_VERSION_STR "\n";
    source << "} // End of namespace mty::doc::" MARTY_VERSION_STR "\n";
}

}
