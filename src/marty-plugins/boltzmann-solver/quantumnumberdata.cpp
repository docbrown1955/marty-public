#include "quantumnumberdata.h"

namespace mty {

    QuantumNumberData::QuantumNumberData(mty::Model const &t_model)
        : m_model(&t_model)
    {

    }

    void QuantumNumberData::addQuantumNumber(
            std::string              const &qName,
            std::vector<std::string> const &particleNames,
            std::vector<csl::Expr>          values
            )
    {
        HEPAssert(particleNames.size() == values.size(),
                mty::error::ValueError,
                "Expected as many particles as values, " 
                + std::to_string(particleNames.size()) + " and "
                + std::to_string(values.size()) + " given respectively.")

        std::vector<mty::Particle> particles = getParticles(particleNames);
        int factor = getCommonDenominator(values);
        std::vector<int> integerValues(values.size());
        for (size_t i = 0; i != values.size(); ++i) {
            integerValues[i] = static_cast<int>(values[i]->evaluateScalar());
        }
        completeDiracEmbedding(particles, integerValues);
        qNumbers.push_back({qName, factor, particles, integerValues});
    }

    void QuantumNumberData::completeDiracEmbedding(
            std::vector<mty::Particle> &particles,
            std::vector<int>           &values
            ) const
    {
        const size_t sz = particles.size();
        auto add = [&](mty::Particle const &p, int v) {
            if (p) {
                particles.push_back(p);
                values.push_back(v);
            }
        };
        for (size_t i = 0; i != sz; ++i) {
            add(particles[i]->getDiracParent(), values[i]);
            add(particles[i]->getWeylFermion(Chirality::Left), values[i]);
            add(particles[i]->getWeylFermion(Chirality::Right), values[i]);
            if (IsOfType<mty::WeylFermion>(particles[i]))
                add(particles[i]->getChiralityCounterPart(), values[i]);
        }
    }

    std::unique_ptr<JSON::Node> QuantumNumberData::generateJSONData() const
    {
        using namespace JSON;
        const std::string listName = "quantum numbers";
        auto qList = List::make(listName);
        for (const auto &qNumber : qNumbers) {
            auto node = Node::make(listName);
            node->addChild(Leaf<std::string>::make("name",   qNumber.name));
            node->addChild(Leaf<int>        ::make("factor", qNumber.factor));
            auto particleList = List::make("particles");
            for (size_t i = 0; i != qNumber.particles.size(); ++i) {
                auto const &part  = qNumber.particles[i];
                auto const &value = qNumber.values[i];
                auto particleNode = Node::make("particles");
                particleNode->addChild(
                        Leaf<std::string>::make("name", part->getName()));
                particleNode->addChild(Leaf<int>::make("value", value));
                particleList->addChild(particleNode);
            }
            node->addChild(particleList);
            qList->addChild(node);
        }
        return qList;
    }

    int QuantumNumberData::factor(std::string const &qName) const
    {
        for (const auto &qNumber : qNumbers)
            if (qNumber.name == qName)
                return qNumber.factor;
        CallHEPError(mty::error::NameError,
                "Quantum number \"" + qName + "\" not found.")
        return -1;
    }

    int QuantumNumberData::value(
            std::string    const &qName,
            mty::Insertion const &particle
            ) const
    {
        for (const auto &qNumber : qNumbers) {
            if (qNumber.name == qName) {
                const auto &name = particle.getField()->getName();
                const bool  conj = !particle.isParticle();
                for (size_t i = 0; i != qNumber.particles.size(); ++i) {
                    if (qNumber.particles[i]->getName() == name) {
                        const auto value = qNumber.values[i];
                        return conj ? -value : value;
                    }
                }
                return 0;
            }
        }
        CallHEPError(mty::error::NameError,
                "Quantum number \"" + qName + "\" not found.")
        return -1;
    }

    std::vector<mty::Particle> QuantumNumberData::getParticles(
            std::vector<std::string> const &particleNames
            ) const
    {
        std::vector<mty::Particle> particles(particleNames.size());
        for (size_t i = 0; i != particles.size(); ++i) {
            checkParticleValidity(particleNames[i]);
            particles[i] = m_model->getParticle(particleNames[i]);
        }
        return particles;
    }

    int QuantumNumberData::getCommonDenominator(
            std::vector<csl::Expr> &values
            ) const
    {
        std::vector<int> denominators;
        denominators.reserve(values.size());
        for (auto &x : values) {
            checkValueValidity(x);
            if (csl::IsIntFraction(x)) {
                denominators.push_back(x->getDenom());
            }
        }
        int factor = 1;
        for (int d : denominators) {
            factor = std::lcm(factor, d);
        }
        for (auto &x : values) {
            x *= factor;
        }

        return factor;
    }

    void QuantumNumberData::checkParticleValidity(
            std::string const &particleName
            ) const
    {
        HEPAssert(m_model->findParticle(particleName),
                mty::error::NameError,
                "Particle \"" + particleName + "\" not found in the model.")
    }

    void QuantumNumberData::checkValueValidity(
            csl::Expr const &value
            ) const
    {
        const csl::Type type = value->getType();
        HEPAssert(type == csl::Type::Integer
                || type == csl::Type::IntFraction,
                mty::error::TypeError,
                "Expected an integer (csl::Type::Integer) or a rational number"
                " (csl::Type::IntFraction) for quantum number value, \""
                + toString(value) + "\" given." )
    }
}
