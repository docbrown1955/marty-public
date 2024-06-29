#include "processdata.h"
#include "quantumnumberdata.h"

namespace mty {

    ProcessData::ProcessData(mty::Model const &t_model)
        : m_model(&t_model)
    {

    }

    ProcessData::Process &ProcessData::addProcess(
            std::string                 const &name,
            std::vector<mty::Insertion> const &insertions,
            QuantumNumberData           const &qData
            )
    {
        Process p;
        p.name = name;
        p.inParticles.reserve(insertions.size());
        p.outParticles.reserve(insertions.size());
        for (const auto &ins : insertions) {
            if (ins.isIncoming())
                p.inParticles.push_back(ins);
            else
                p.outParticles.push_back(ins);
        }
        processQuantumNumbers(p, qData);
        processes.push_back(p);

        return processes.back();
    }

    std::unique_ptr<JSON::Node> ProcessData::generateJSONParticleList(
            std::string                 const &listName,
            std::vector<mty::Insertion> const &particles
            ) const
    {
        using namespace JSON;
        auto pList = List::make(listName);
        for (const auto &part : particles) {
            auto name = part.getField()->getName();
            if (!part.isParticle()) {
                name += "#c";
            }
            pList->addChild(Leaf<std::string>::make(listName, name));
        }
        return pList;
    }

    std::unique_ptr<JSON::Node> ProcessData::generateJSONQNumberList(
            std::string                const &listName,
            std::map<std::string, int> const &qNumbers
            ) const
    {
        using namespace JSON;
        if (qNumbers.empty())
            return nullptr;
        auto pList = List::make(listName);
        for (const auto &[qName, value] : qNumbers) {
            auto node = Node::make(listName);
            node->addChild(Leaf<std::string>::make("name", qName));
            node->addChild(Leaf<int>::make("value", value));
            pList->addChild(node);
        }
        return pList;
    }

    std::unique_ptr<JSON::Node> ProcessData::generateJSONData() const
    {
        using namespace JSON;
        const std::string listName = "processes";
        auto pList = List::make(listName);
        for (const auto &process : processes) {
            auto node = Node::make(listName);
            node->addChild(Leaf<std::string>::make("name", process.name));
            node->addChild(generateJSONParticleList(
                        "incoming", process.inParticles));
            node->addChild(generateJSONParticleList(
                        "outgoing", process.outParticles));
            auto qNumberNode = generateJSONQNumberList(
                        "quantum numbers", process.qNumbers);
            if (qNumberNode)
                node->addChild(qNumberNode);
            pList->addChild(node);
        }
        return pList;
    }

    void ProcessData::processQuantumNumbers(
            Process                 &process,
            QuantumNumberData const &qData
            ) const
    {
        for (const auto &qNumber : qData.getQNumbers()) {
            int delta = 0;
            auto const &name = qNumber.name;
            for (const auto &in : process.inParticles) {
                delta -= qData.value(name, in);
            }
            for (const auto &out : process.outParticles) {
                delta += qData.value(name, out);
            }
            if (delta != 0)
                process.qNumbers[name] = delta;
        }
    }

    void saveParticleData(
            std::string       const &fileName,
            QuantumNumberData const &qData,
            ProcessData       const &pData
            )
    {
        using namespace JSON;
        auto node = Node::make("root");
        node->addChild(qData.generateJSONData());
        node->addChild(pData.generateJSONData());
        Reader::saveToFile(fileName, node);
    }
}
