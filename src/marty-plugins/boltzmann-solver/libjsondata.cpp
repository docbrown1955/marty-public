#include "libjsondata.h"

using namespace JSON;

namespace mty::lib {

    Rational QuantumNumber::getQuantumNumberValue(
          std::string const &particleName,
          bool               isParticle
          )
    {
        return getQuantumNumberValue(Particle{particleName, isParticle});
    }

    Rational QuantumNumber::getQuantumNumberValue(
          Particle const &particle
          )
    {
        int sign = particle.particle ? 1 : -1;
        return Rational { sign * particles[particle.name], factor };
    }

    Rational ParticleData::getQuantumNumberValue(
          std::string const &quantumNumber,
          std::string const &particleName,
          bool               isParticle
          )
    {
        return getQuantumNumberValue(
            quantumNumber, 
            Particle{particleName, isParticle});
    }

    Rational ParticleData::getQuantumNumberValue(
          std::string const &quantumNumber,
          Particle    const &particle
          )
    {
        auto pos_number = std::find_if(
            qNumbers.begin(), 
            qNumbers.end(), 
            [&](QuantumNumber const &q) { return q.name == quantumNumber; }
            );
        if (pos_number == qNumbers.end()) {
            std::cerr << "Error: Quantum number \"" << quantumNumber
                << "\" not found in the list.\n";
            std::cerr << "Candidates are: \n";
            for (const auto &qNumber : qNumbers) {
                std::cerr << qNumber << std::endl;
            }
            exit(1);
        }
        return pos_number->getQuantumNumberValue(particle);
    }

    void ParticleData::loadFile(std::string const &nameFile)
    {
        clear();
        auto tree = JSON::Reader::loadFromFile(nameFile);
        load(tree);
    }

    void ParticleData::clear()
    {
        particleNames.clear();
        qNumbers.clear();
        processes.clear();
    }

    std::vector<Process> ParticleData::getProcesses(
            std::function<bool(Process const&)> selection
            ) const 
    {
        std::vector<Process> res;
        res.reserve(100);
        for (const auto &process : processes)
            if (selection(process))
                res.push_back(process);
        return res;
    }

    std::vector<Process> ParticleData::getProcessesFromIncoming(
            std::string const &p1Name,
            std::string const &p2Name
            ) const
    {
        Particle p1 = Particle::buildFromSpecifier(p1Name);
        Particle p2 = Particle::buildFromSpecifier(p2Name);
        return getProcesses([&](Process const &process) {
            if (p2Name == "") {
                return process.inParticles.size() == 1
                    && process.inParticles[0] == p1;
            }
            if (process.inParticles.size() != 2)
                return false;
            auto p1_process = process.inParticles[0];
            auto p2_process = process.inParticles[1];
            return (p1 == p1_process && p2 == p2_process)
                || (p1 == p2_process && p2 == p1_process);
        });
    }

    std::vector<Process> ParticleData::getProcessesFromQNumberConservation(
            std::string const &qNumber
            ) const
    {
        return getProcesses([&](Process const &process) {
            auto pos = process.qNumbers.find(qNumber);
            return (pos == process.qNumbers.end() || pos->second == 0);
        });
    }

    std::vector<Process> ParticleData::getProcessesFromQNumberViolation(
            std::string const &qNumber
            ) const
    {
        return getProcesses([&](Process const &process) {
            auto pos = process.qNumbers.find(qNumber);
            return (pos != process.qNumbers.end() && pos->second != 0);
        });
    }

    void ParticleData::load(std::unique_ptr<Node> const &tree)
    {
        auto qNumberNode = Parser::parseNode(tree.get(), "quantum numbers", true);
        auto processNode = Parser::parseNode(tree.get(), "processes", true);
        loadQNumberNode(qNumberNode);
        loadProcessNode(processNode);
    }

    void ParticleData::loadQNumberNode(Node const *node)
    {
        qNumbers.reserve(node->size());
        for (const auto &child : *node) {
            auto name = Parser::parseArgument<std::string>(
                    child, "name", true).value();
            auto factor = Parser::parseArgument<int>(
                    child, "factor", true).value();
            QuantumNumber qNumber { name, factor, {} };
            auto particles = Parser::parseNode(child, "particles");
            if (particles) {
                for (const auto &p : *particles) {
                    auto name = Parser::parseArgument<std::string>(
                            p, "name", true).value();
                    auto value = Parser::parseArgument<int>(
                            p, "value", true).value();
                    qNumber.particles[name] = value;
                }
            }
            qNumbers.push_back(qNumber);
        }
    }

    void ParticleData::loadProcessNode(Node const *node)
    {
        processes.reserve(node->size());
        for (const auto &child : *node) {
            auto name = Parser::parseArgument<std::string>(
                    child, "name", true).value();
            auto incoming = Parser::parseNode(child, "incoming", true);
            auto outgoing = Parser::parseNode(child, "outgoing", true);
            auto qNumbers = Parser::parseNode(child, "quantum numbers");
            Process process;
            process.name = name;
            process.inParticles = loadParticles(incoming);
            process.outParticles = loadParticles(outgoing);
            addParticles(process.inParticles);
            addParticles(process.outParticles);
            if (qNumbers)
                loadProcessQNumber(qNumbers, process);
            processes.push_back(process);
        }
    }

    void ParticleData::loadProcessQNumber(
            JSON::Node const *node,
            Process          &process
            ) const
    {
        for (const auto &child : *node) {
            auto name = Parser::parseArgument<std::string>(
                    child, "name", true).value();
            auto value = Parser::parseArgument<int>(
                    child, "value", true).value();
            process.qNumbers[name] = value;
        }
    }

    std::vector<Particle> ParticleData::loadParticles(Node const *node) const
    {
        std::vector<Particle> particles;
        particles.reserve(node->size());
        for (const auto &part : *node) {
            std::string name = Parser::interpretObject<std::string>(part.get());
            particles.push_back(Particle::buildFromSpecifier(name));
        }
        return particles;
    }

    void ParticleData::addParticles(std::vector<Particle> const &particles)
    {
        for (const auto &particle : particles) {
            auto pos = std::find(
                particleNames.begin(), 
                particleNames.end(),
                particle.name);
            if (pos == particleNames.end()) {
                particleNames.push_back(particle.name);
            }
        }
    }

    std::ostream &operator<<(std::ostream &out, Process const &process)
    {
        out << "Process \"" << process.name << "\":\n";
        out << "  Incoming particles : ";
        for (const auto &p : process.inParticles) {
            out << p.name;
            if (!p.particle)
                out << "#c";
            out << "  ";
        }
        out << "\n  Outgoing particles : ";
        for (const auto &p : process.outParticles) {
            out << p.name;
            if (!p.particle)
                out << "#c";
            out << "  ";
        }
        out << "\n  Quantum numbers: " << '\n';
        for (const auto &[name, value] : process.qNumbers) {
            out << "    " << name << " : " << value << "\n";
        }
        return out;
    }

    std::ostream &operator<<(std::ostream &out, QuantumNumber const &qNumber)
    {
        out << qNumber.name << ", global factor : " << qNumber.factor;
        out << '\n';
        for (const auto &[name, value] : qNumber.particles) {
            out << "  -> " << name << " : " << value << '\n';
        }
        return out;
    }

    std::ostream &operator<<(
            std::ostream &out,
            ParticleData const &data
            )
    {
        out << "Particles :\n";
        for (const auto &particle : data.particleNames) {
            out << particle << " ";
        }
        out << '\n';
        out << "Quantum numbers :\n";
        for (const auto &qNumber : data.qNumbers) {
            out << qNumber;
        }
        out << "Processes :\n";
        for (const auto &process : data.processes) {
            out << process;
        }
        return out;
    }

}
