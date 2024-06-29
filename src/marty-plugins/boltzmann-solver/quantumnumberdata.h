#pragma once

#include <marty.h>
#include <grafed/jsonparser/jsonLoader.h>
//#include "marty/jsonLoader.h"

namespace mty {

    class QuantumNumberData {

    public:

        struct QuantumNumber {
            std::string                name;
            int                        factor;
            std::vector<mty::Particle> particles;
            std::vector<int>           values;
        };

        QuantumNumberData(Model const &t_model);

        void addQuantumNumber(
                std::string              const &qName,
                std::vector<std::string> const &particles,
                std::vector<csl::Expr>          values
                );

        void addQuantumNumber(
                std::string                 const &qName,
                std::initializer_list<std::string> particles,
                std::initializer_list<csl::Expr>   values
                )
        {
            return addQuantumNumber(
                    qName, 
                    std::vector<std::string>(particles),
                    std::vector<csl::Expr>  (values)
                    );
        }

        std::unique_ptr<JSON::Node> generateJSONData() const;

        int factor(std::string const &qName) const;

        int value(
                std::string    const &qName,
                mty::Insertion const &particleName
                ) const;

        std::vector<QuantumNumber> const &getQNumbers() const {
            return qNumbers;
        }

        std::vector<QuantumNumber> &getQNumbers() {
            return qNumbers;
        }

    private:

        std::vector<mty::Particle> getParticles(
                std::vector<std::string> const &particleNames
                ) const;

        void completeDiracEmbedding(
                std::vector<mty::Particle> &particles,
                std::vector<int>           &values
                ) const;

        int getCommonDenominator(
                std::vector<csl::Expr> &values
                ) const;

        void checkParticleValidity(std::string const &particleName) const;

        void checkValueValidity   (csl::Expr   const &value) const;

    private:

        Model const *m_model;
        std::vector<QuantumNumber> qNumbers;
    };

}
