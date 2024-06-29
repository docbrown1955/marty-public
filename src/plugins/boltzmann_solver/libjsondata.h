#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>
#include <grafed/jsonparser/jsonLoader.h>
//#include "marty/jsonLoader.h"

namespace mty::lib {

    /*!
     * @brief Container for a particle insertion in a diagram.
     *
     * @details This container features only a name and a boolean that 
     * determines if the particle is conjugated (anti-particle).
     */
    struct Particle {
        std::string name;
        bool        particle { true };

        /*!
         * @brief Creates and returns a particle from a JSON specifier.
         *
         * @param spec JSON specifier describing the particle. 
         *
         * @return The Particle object with the name and the right conjugation
         * property (particle or anti-particle).
         */
        static Particle buildFromSpecifier(std::string spec) {
            size_t const sz = spec.size();
            if (sz > 2 && spec[sz-2] == '#' && spec[sz-1] == 'c') {
                spec.erase(sz-2, 2);
                return {spec, false};
            }
            return {spec, true};
        }

        /*! 
         * @brief Compares two particles.
         *
         * @param other Particle to compare.
         *
         * @return **True** if the two particles have the same name and 
         * conjugation property.
         */
        bool operator==(Particle const &other) const {
            return name == other.name && particle == other.particle;
        }
    };

    /*!
     * @brief Container for a Process read in JSON data.
     */
    struct Process {

        /*! Process name corresponding to the function name in the library. */
        std::string name; 

        /*! List of incoming particles. */
        std::vector<Particle> inParticles;

        /*! List of outgoing particles. */
        std::vector<Particle> outParticles;

        /*! Map of non-trivial quantum numbers transitions for the process. */
        std::map<std::string, int> qNumbers;
    };

    /*!
     * @brief Container for rational numbers \f$ \frac{a}{b} \f$ with a 
     * numerator and denominator.
     */
    struct Rational {
        int num;
        int denom;

        /*! Returns the floating point value equal to \f$ a/b \f$. */
        double getValue() const { return num * 1. / denom; }
    };

    /*! 
     * @brief Container for a quantum number.
     *
     * @details Only integers are stored. To store rational numbers, a common
     * denominator is stored in the #factor attribute. To have the rational 
     * value, use 
     * \code  
     *    Rational number = quantumNumber.getQuantumNumberValue(particle);
     *    std::cout << number.num << " / " << number.denom << '\n';
     *    std::cout << "  = " << number.getValue() << '\n'; // Floating-point
     * \endcode
     *
     */
    struct QuantumNumber {

        /*! Name of the quantum number. */
        std::string name;

        /*! Common denominator for the quantum number of different particles. */
        int         factor;

        /*! Map of particle values (only numerators) for this quantum number. */
        std::map<std::string, int> particles;

        /*! 
         * @brief Returns the quantum number of a Particle, taking into account 
         * its conjugation (minus sign for anti-particles).
         *
         * @param particleName Particle name.
         * @param isParticle   Boolean (optional, default=true) telling if the 
         * particle is **not** an anti-particle.
         *
         * @return The value of the **particle** for this quantum number.
         */
        Rational getQuantumNumberValue(
            std::string const &particleName,
            bool               isParticle = true
            );

        /*! 
         * @brief Returns the quantum number of a Particle, taking into account 
         * its conjugation (minus sign for anti-particles).
         *
         * @param particle Particle
         *
         * @return The value of the **particle** for this quantum number.
         */
        Rational getQuantumNumberValue(
            Particle const &particle
            );
    };

    std::ostream &operator<<(std::ostream &out, Process const &process);
    std::ostream &operator<<(std::ostream &out, QuantumNumber const &qNumber);

    /*! 
     * @brief Main container for JSON data.
     *
     * @details Contains the particle list, quantum number list, process list
     * and the interface to retrieve this data. Any process name can be 
     * used to get the corresponding MARTY function. Usage:
     * \code
     *    mty::lib::ParticleData data;
     *    data.load("data.json");
     *    mty::lib::Process process = data.getProcesses()[0]; // First process
     *    auto f = f_G[process.name]; // Get the function from its name
     *
     *    param_t params; // Create and fill the param_t struct
     *    std::cout << f(params).real() << std::endl; // Use the function
     * \endcode
     */
    class ParticleData {

        public:

        /*!
         * @brief Returns the list of particle names in the library.
         */
        std::vector<std::string> const &getParticleNames() const {
            return particleNames;
        }

        /*!
         * @brief Returns the list of quantum numbers in the library.
         */
        std::vector<QuantumNumber> const &getQuantumNumbers() const {
            return qNumbers;
        }

        /*! 
         * @brief Returns the quantum number of a Particle, taking into account 
         * its conjugation (minus sign for anti-particles).
         *
         * @param quantumNumber Quantum number name.
         * @param particleName  Particle name.
         * @param isParticle    Boolean (optional, default=true) telling if the 
         * particle is **not** an anti-particle.
         *
         * @return The value of the **particle** for the quantum number.
         */
        Rational getQuantumNumberValue(
            std::string const &quantumNumber,
            std::string const &particleName,
            bool               isParticle = true
            );

        /*! 
         * @brief Returns the quantum number of a Particle, taking into account 
         * its conjugation (minus sign for anti-particles).
         *
         * @param quantumNumber Quantum number name.
         * @param particle      Particle object.
         *
         * @return The value of the **particle** for the quantum number.
         */
        Rational getQuantumNumberValue(
            std::string const &quantumNumber,
            Particle    const &particle
            );

        /*!
         * @brief Returns the full process list.
         */
        std::vector<Process> const &getProcesses() const {
            return processes;
        }

        /*!
         * @brief Returns the list of processes respecting a given condition.
         *
         * @param selection Functional object (function, lambda, etc) returning
         * true if the process (given as parameter) must be selected i.e. 
         * included in the list.
         *
         * @return The list of processes passing the selection.
         */
        std::vector<Process> getProcesses(
                std::function<bool(Process const&)> selection
                ) const;

        /*!
         * @brief Returns the list of processes that have one (or two) particles
         * as incoming particles.
         *
         * @param p1 First particle name, all selected processes should have 
         * this particle as incoming.
         * @param p2 Optional second particle, same behabior as **p1** if given, 
         * ignored otherwise (default).
         *
         * @return The list of processes with **p1** (and **p2** if given) as
         * incoming particles.
         */
        std::vector<Process> getProcessesFromIncoming(
                std::string const &p1,
                std::string const &p2 = ""
                ) const;

        /*! 
         * @brief Returns the processes conserving a given quantum number.
         *
         * @param qNumber Quantum number to test.
         *
         * @return All processes that conserve **qNumber**.
         */
        std::vector<Process> getProcessesFromQNumberConservation(
                std::string const &qNumber
                ) const;

        /*! 
         * @brief Returns the processes violating a given quantum number.
         *
         * @param qNumber Quantum number to test.
         *
         * @return All processes that violate **qNumber**.
         */
        std::vector<Process> getProcessesFromQNumberViolation(
                std::string const &qNumber
                ) const;

        /*!
         * @brief Loads JSON data from a file.
         *
         * @param nameFile Name of the JSON file.
         *
         * @details Usage:
         * \code
         *    mty::lib::ParticleData data;
         *    data.load("file.json");
         *    std::cout << data << std::endl;
         * \endcode
         */
        void loadFile(std::string const &nameFile);

        void clear();

        private:

        void load(std::unique_ptr<JSON::Node> const &tree);

        void loadQNumberNode(JSON::Node const *node);

        void loadProcessNode(JSON::Node const *node);

        bool isConjugated(std::string &name) const;

        std::vector<Particle> loadParticles(JSON::Node const *node) const;

        void addParticles(std::vector<Particle> const &particles);

        void loadProcessQNumber(
                JSON::Node const *node,
                Process          &process
                ) const;

        public:

        friend
            std::ostream &operator<<(
                    std::ostream       &out,
                    ParticleData const &data
                    );

        private:

        /*! List of the particles names in the library. */
        std::vector<std::string> particleNames;

        /*! List of the quantum numbers in the library. */
        std::vector<QuantumNumber> qNumbers;

        /*! List of the processes in the library. */
        std::vector<Process> processes;
    };

}
