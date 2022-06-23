#include <marty.h>
#include "marty/models/pmssm_lem.h"
#include <fstream>
#include <sstream>

using namespace mty;

std::vector<std::vector<mty::Insertion>> read_processes(
        std::istream &file)
{
    std::vector<std::vector<mty::Insertion>> res;
    res.reserve(3000);
    std::string line;
    std::getline(file, line);
    while (!line.empty())
    {
        std::string token;
        res.push_back(std::vector<mty::Insertion>{});
        auto &process = res.back();
        process.reserve(4);
        std::istringstream line_stream(line);
        for (int i = 0; i != 4; ++i)
        {
            line_stream >> token;
            bool conj = false;
            const size_t len = token.size();
            if (len > 2 && token[len-2] == '#') {
                token = std::string(token.begin(), token.begin() + len-2);
                conj = true;
            }
            auto insertion = (i < 2) ? mty::Incoming : mty::Outgoing;
            if (conj) {
                process.push_back(insertion(AntiPart(token)));
            }
            else {
                process.push_back(insertion(token));
            }
        }
        std::getline(file, line);
    }
    return res;
}


std::string getProcessName(std::vector<mty::Insertion> const &process)
{
    std::string name;
    for (size_t i = 0; i != process.size(); ++i)
    {
        name += process[i].getField()->getName();
        if (!process[i].isParticle())
        {
            name += "_c";
        }
        if (i == 1)
        {
            name += "_to_";
        }
    }
    return name;
}


void calculateProcesses(
        mty::Model &model,
        mty::Library &lib,
        std::vector<std::vector<mty::Insertion>> const &processes,
        bool unitary
        )
{
    if (unitary)
    {
        model.getParticle("W")->setGaugeChoice(mty::gauge::Unitary);
        model.getParticle("Z")->setGaugeChoice(mty::gauge::Unitary);
    }
    else
    {
        model.getParticle("W")->setGaugeChoice(mty::gauge::Feynman);
        model.getParticle("Z")->setGaugeChoice(mty::gauge::Feynman);
    }
    for (const auto &process : processes)
    {
        auto ampl = model.computeAmplitude(TreeLevel, process);
        auto squared_ampl = model.computeSquaredAmplitude(ampl);
        std::string processName = getProcessName(process);
        lib.addFunction(processName, squared_ampl);
    }
}

int main() {

    PMSSM_LEM pmssm;
    std::ifstream process_file("processes.marty");
    auto feynman_processes = read_processes(process_file);
    auto unitary_processes = read_processes(process_file);
    process_file.close();

    mty::Library lib("full_MSSM");
    lib.cleanExistingSources();
    lib.generateSpectrum(pmssm);

    calculateProcesses(pmssm, lib, feynman_processes, false);
    calculateProcesses(pmssm, lib, unitary_processes, true);

    lib.print();


    return 0;
}
