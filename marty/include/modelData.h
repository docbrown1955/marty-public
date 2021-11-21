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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief First abstraction layer for a High-Energy Physics model. 
 */
#ifndef MODEL_DATA_H_INCLUDED
#define MODEL_DATA_H_INCLUDED 

#include <type_traits>
#include <csl.h>
#include "quantumField.h"
#include "lagrangian.h"

/*!
 * \brief Namespace of MARTY.
 * \details This namespace contains all functions, objects, interfaces of 
 * MARTY, or more exactly of the physics part of MARTY. Mathematical objects,
 * for symbolic manipulations, lie in the namespace csl.
 */
namespace mty {

/*!
 * \brief The class ModelData contains all the necessary features to store 
 * and manipulate the content of a model. No model building (symmetry breaking, 
 * replacements etc) or computation are implemented in it.
 * \details This class contains the Lagrangian of the theory (all interaction, 
 * kinetic, and mass terms), the gauge, flavor symmetries, particles, 
 * quantum numbers, couplings, and momenta. It is really a container, owner of 
 * all the cited objects. In particular, deleting the ModelData will 
 * automatically delete the lagrangian, gauge, flavor. Particles are under 
 * shared ownership, so will be deleted only if there is no other reference to
 * it in the program. Beware however than even particles cannot be used if the 
 * model is deleted as they will refer to a non-existing gauge / flavor.
 * \warning ModelData instances (together with ModelBuilder and Model) should 
 * live at least as longer as any object that come from it, in particular 
 * particles, or expressions containing particles. To avoid any problem, define
 * it as a static variable in the main function, or as a dynamically allocated
 * variable deleted at the end of the main function.
 * \sa ModelBuilder, Model
 */
class ModelData {

public:

    ///////////////////////////////////////////////////
    // Public (static) data members
    ///////////////////////////////////////////////////

    /*!
     * \brief Number of momenta instantiated for the model (10). With 5 maximum 
     * external legs, this is fairly safe.
     */
    inline static size_t            nMomentums           = 10;
    /*!
     * \brief Default space-time for the model's particles. There is actually
     * no support for another space that csl::Minkowski yet.
     */
    inline static csl::Space const *defaultSpaceTime     = &csl::Minkowski;
    /*!
     * \brief Default quantum number for particles when not given.
     * \details When defining a quantum number in a model, for example the 
     * baryonic number B, you may define it for any particle. If you do nothing,
     * the default number is zero. If you define it, you can either give the 
     * quantum number, or nothing. It the latter case, the defined quantum 
     * number is equal to defaultQuantumNumber.
     * \sa getQuantumNumbers(), addQuantumNumber(), setQuantumNumbers()
     */
    inline static int               defaultQuantumNumber = 1;

public:

    ///////////////////////////////////////////////////
    // Constructors
    ///////////////////////////////////////////////////

    /*!
     * \brief Default constructor.
     * \details Instantiates a null flavor, and an empty gauge. This is a valid
     * state and you can add gauge groups through addGaugedGroup().
     */
    ModelData();
    /*!
     * \brief Constructor with one parameter.
     * \details Instantiates a null flavor, and the gauge to \b t_gauge. Calling
     * this constructor, the gauge you give to the model is no longer valid as 
     * the unique ownership is transfered to it. Using this constructor, the 
     * gauge is taken as is, and the model is initialized. It is in particular 
     * not possible to add gauged groups afterwards. If you want to add gauge 
     * groups through the addGaugedGroup() function, consider using the default
     * constructor. Here is a code snippet that show how to use this 
     * constructor.
     * \code
     * std::unique_ptr<mty::Gauge> gauge = makeGauge();
     * Model model(std::move(gauge));
     * // Here gauge is not valid anymore, model took the ownership
     * \endcode
     * \sa init(), addGaugedGroup()
     * \param t_gauge Gauge now owned by the Model.
     */
    explicit ModelData(std::unique_ptr<Gauge> &&t_gauge);
    /*!
     * \brief Constructor with 2 parameters.
     * \details Instantiates the model with a gauge and a flavor. See the 
     * constructor with one parameter for more information. The constructor 
     * simply instantiates the flavor with \b t_flavor. Here is a code snippet 
     * that show how to use this constructor.
     * \code
     * std::unique_ptr<mty::Gauge>  gauge  = makeGauge();
     * std::unique_ptr<mty::Flavor> flavor = makeFlavor();
     * Model model(std::move(gauge), std::move(flavor));
     * // Here gauge  is not valid anymore, model took the ownership
     * // Here flavor is not valid anymore, model took the ownership
     * \endcode

     * \param t_gauge  Gauge now owned by the Model.
     * \param t_flavor Flavor now owned by the Model.
     */
    ModelData(
            std::unique_ptr<Gauge>  &&t_gauge,
            std::unique_ptr<Flavor> &&t_flavor
            );
    /*!
     * \brief Constructor with 3 parameters.
     * \details Instantiates the model with a gauge, a flavor and a set of 
     * particles. See the constructor with two parameters for more information. 
     * The constructor simply adds particles. Those particles must have been 
     * created with the corresponding gauge and flavor. Covariant derivative
     * and mass terms for the particles are added automatically. Here is a code 
     * snippet that show how to use this constructor.
     * \code
     * std::unique_ptr<mty::Gauge>  gauge     = makeGauge();
     * std::unique_ptr<mty::Flavor> flavor    = makeFlavor();
     * std::vector<mty::Particle>   particles = makeParticles();
     * Model model(std::move(gauge), std::move(flavor), particles);
     * // Here gauge  is not valid anymore, model took the ownership
     * // Here flavor is not valid anymore, model took the ownership
     * \endcode

     * \param t_gauge     Gauge now owned by the Model.
     * \param t_flavor    Flavor now owned by the Model.
     * \param t_particles Particles to add to the Model after intializing the 
     * gauge and flavor.
     */
    ModelData(
            std::unique_ptr<Gauge>  &&t_gauge,
            std::unique_ptr<Flavor> &&t_flavor,
            std::vector<Particle>    &t_particles);
    /*!
     * \brief Creates a model from a model file (.json).
     * \sa jsonToPhysics.h, JSONToHEP
     */
    ModelData(std::string const& nameFile);

    /*!
     * \brief Destructor
     */
    virtual ~ModelData();

    /*!
     * \brief Defaulted move constructor.
     */
    ModelData(ModelData &&other) = default;
    /*!
     * \brief Defaulted move assignement operator.
     */
    ModelData &operator=(ModelData &&other) = default;
    /*!
     * \brief Defaulted copy constructor.
     */
    ModelData(ModelData const &other) = delete;
    /*!
     * \brief Defaulted copy assignement operator.
     */
    ModelData &operator=(ModelData const &other) = delete;

    ///////////////////////////////////////////////////
    // Initialization function
    ///////////////////////////////////////////////////

    /*!
     * \brief Initializes the gauge once and for all. This function must be 
     * called after finishing the gauge, and before adding matter particles.
     * Calling the constructors that take an already built gauge, this function
     * is called automatically. Otherwise you have to call it.
     * \param initGaugeTerms Boolean (default = true) that tells if kinetic and 
     * mass terms for gauge bosons and ghosts must be added.
     */
    void init(bool initGaugeTerms = true);

    ///////////////////////////////////////////////////
    // Check functions
    ///////////////////////////////////////////////////

    /*!
     * \brief Checks the hermiticity of the Lagrangian, i.e. checks if 
     * \f$ \mathcal{L}^\dagger = \mathcal{L} \f$.
     * \details If the Lagrangian is not hermitian, this function simply prints
     * out the problematic terms. There is no check for integration by parts,
     * so the Lagrangian may still be hermitic, this function simply does not 
     * automatically recognize this kind of patterns.
     */
    void checkHermiticity();

    ///////////////////////////////////////////////////
    // Write function
    ///////////////////////////////////////////////////

    virtual void saveModel(
            std::ostream    &out,
            int              indent = 4
            );

    void saveModelFunction(
            std::string_view name,
            std::ostream    &out,
            int              indent = 0
            );

    void saveModelFunctionWithRef(
            std::string_view name,
            std::ostream    &out,
            int              indent = 0
            );

    void saveModelFile(
            std::string_view   name,
            std::string const &fileName
            );

    ///////////////////////////////////////////////////
    // Getters
    ///////////////////////////////////////////////////

    /*!
     * \brief Returns a const reference to the Lagrangian.
     * \details The lagrangian should not be modified directly. Always prefer 
     * using buitl-in functions to modify it. This is why this function 
     * returns an const-reference. The way to get the lagrangian from it is
     * \code
     * mty::Lagrangian const &L = model.getLagrangian();
     * \endcode
     * \return The lagrangian of the model.
     */
    Lagrangian const &getLagrangian() const;

    /**
     * @return \b enableChecks
     */
    bool isEnableChecks() const {
        return enableChecks;
    }

    /**
     * @brief Sets \b enableChecks
     *
     * @param t_enable New boolean value
     */
    void setEnableChecks(bool t_enable) {
        enableChecks = t_enable;
    }

    /*!
     * \brief Returns a pointer to the space time of the mode (csl::Space). 
     * \details For now, only csl::Minkowski is supported.
     * \return The space-time of the model.
     */
    csl::Space const *getSpaceTime() const;

    /*!
     * \brief Returns a pointer to the model's gauge.
     * \details This function does not transfer the ownership. In particular, 
     * you have not to delete the data.
     * \return The gauge of the model.
     */
    mty::Gauge *getGauge() const;

    /*!
     * \brief Returns a pointer to the model's flavor.
     * \details This function does not transfer the ownership. In particular, 
     * you have not to delete the data.
     * \return The flavor of the model.
     */
    mty::Flavor *getFlavor() const;

    /*!
     * \brief Returns a const reference to the model's particles.
     * \details Particles should be modified through the built-in functions, 
     * not directly from this vector. This is why the function only returns a 
     * const reference. It must be called as follows
     * \code
     * std::vector<mty::Particle> const &particles = model.getParticles();
     * \endcode
     * \return The particles of the model.
     */
    std::vector<mty::Particle> const &getParticles() const;

    /**
     * @brief Returns a list of the physical particles in a model.
     *
     * @details Takes care of removing ghosts and particles that would appear
     * twice in the full list of particles (see getParticles()), in particular
     * removing left- and right-handed parts of Dirac fermions (letting only
     * the full 4-component fermion). This function is meant to be called when
     * repeting a particular calculation for all particles in a model. A 
     * boolean predicate can be given to filter more the list, for example
     * taking only bosons or fermions.
     * Example: 
     * \code
     *      auto all = model.getPhysicalParticles();
     *      auto bosons = model.getPhysicalParticles(
     *              [](Particle p) { return p->isBosonic(); }
     *      );
     *      auto fermions = model.getPhysicalParticles(
     *              [](Particle p) { return p->isFermionic(); }
     *      );
     *      auto vectors = model.getPhysicalParticles(
     *              [](Particle p) { return p->getSpinDimension() == 3; }
     *      );
     * \endcode
     *
     * @return The list of physical particles.
     */
    std::vector<mty::Particle> getPhysicalParticles(
            std::function<bool(mty::Particle)> const &predicate
                = [](mty::Particle) { return true; }
            ) const;

    /*!
     * \brief Returns the list of quantum numbers.
     * \return The quantum numbers of the model.
     * \sa quantumNumber.h
     */
    std::vector<mty::QuantumNumber> const &getQuantumNumbers() const;
    /*!
     * \brief Returns the list of quantum numbers.
     * \return The quantum numbers of the model.
     * \sa quantumNumber.h
     */
    std::vector<mty::QuantumNumber> &getQuantumNumbers();

    /*!
     * \brief Returns the scalar couplings (gauge coupling typically) of the 
     * model.
     * \details When a gauge group is created, a gauge coupling constant comes 
     * with it. This coupling is stored in the model's scalar couplings that you
     * can get with this function (as a constant reference).
     * \return The list of scalar couplings of the theory.
     */
    std::vector<csl::Expr> const &getScalarCouplings() const;

    /*!
     * \brief Returns the tensor couplings (Yukawa coupling typically) of the 
     * model.
     * \details When a Yukawa coupling is created, th corresponding tensor is 
     * stored in the model's tensor couplings that you can get with this 
     * function (as a constant reference).
     * \return The list of tensor couplings of the theory.
     */
    std::vector<csl::Tensor> const &getTensorCouplings() const;

    ///////////////////////////////////////////////////
    // Setters
    ///////////////////////////////////////////////////

    /*!
     * \brief Sets the space time of the model.
     * \warning This function as been written to possibly extend MARTY to other
     * space-time calculations than 4D Minkowski but it does not support it yet.
     * **This function should then not be used.**
     * \param t_spaceTime New space time for the model.
     */
    void setSpaceTime(csl::Space const *t_spaceTime);

    /*!
     * \brief Sets the gauge of the model.
     * \details This function should be used after creating a model with an 
     * empty gauge (in particular using the default constructor of ModelData).
     * It may be used to create the gauge separately, not using built-in 
     * functions of this class. The std::unique_ptr given as parameter loses 
     * the ownership of the gauge and is not valid after the function call. Here
     * is a code snippet that show how to use this function.
     * \code
     * Model model;
     * std::unique_ptr<mty::Gauge> gauge = makeGauge();
     * model.setGauge(std::move(gauge));
     * // Here gauge is not valid anymore, model took the ownership
     * \endcode
     * \warning This function must be called \b before adding any particle or 
     * other property as the gauge must be the first one defined.
     * \param t_gauge Gauge for the model.
     */
    void setGauge(std::unique_ptr<mty::Gauge> &&t_gauge);


    /**
     * @brief Adds a gauge group to the model. 
     * @details This function allow to add a gauged group to the model, before
     * initializing it (see init()) and adding matter particles. Given the type,
     * the name, the dimension and (optionnaly) a coupling constant, the 
     * corresponding group, gauge boson, ghosts are created. 
     * \note After adding all desired gauged groups, you must call the init() 
     * function that settels once and for all the gauge.
     * \note If not given, the particles and couplings names follow the group
     * name: A_<name> for the gauge bosons, c_<name> for the ghosts, g_<name> 
     * for the coupling constant.
     *
     * @param type     Type of the gauged group
     * @param name     Name of the group
     * @param dim      Dimension of the group (3 for SU(3) etc)
     * @param coupling Coupling constant (optional)
     *
     * @sa GaugedGroup, Gauge
     */
    void addGaugedGroup(
            group::Type      type,
            std::string_view name,
            int              dim,
            csl::Expr      const& coupling = nullptr);

    /**
     * @brief Adds a gauge group to the model. 
     *
     * @details This function allow to add a gauged group to the model, before
     * initializing it (see init()) and adding matter particles. Given the type,
     * the name, and (optionnaly) a coupling constant, the corresponding group,
     * gauge boson, ghosts are created. Here the dimension is not given. This 
     * function must then be used only for U(1), and exceptional algebras.
     *
     * @note After adding all desired gauged groups, you must call the init() 
     * function that settels once and for all the gauge.
     *
     * @note If not given, the particles and couplings names follow the group
     * name: A_<name> for the gauge bosons, c_<name> for the ghosts, g_<name> 
     * for the coupling constant.
     *
     * @param type     Type of the gauged group
     * @param name     Name of the group
     * @param coupling Coupling constant (optional)
     *
     * @sa GaugedGroup, Gauge
     */
    void addGaugedGroup(
            group::Type      type,
            std::string_view name,
            csl::Expr      const& coupling = nullptr);
    
    /**
     * @brief Adds a flavor group to the model.
     *
     * @details For now, only SU(N) and SO(N) fundamental representations are 
     * enabled for flavor symmetries. For a flavor symmetry between N complex 
     * fields, The fundamental representation of SU(N) is created. For real 
     * fields, the fundamental representation of SO(N) is created.
     *
     * @param name          Name of the flavor group
     * @param nFlavor       Number of flavors
     * @param complexFields Boolean (default = true) that tells if the fields 
     * are complex (SU(N) flavor) or not (SO(N) flavor).
     *
     * @sa FlavorGroup, Flavor
     */
    void addFlavorGroup(
            std::string_view name,
            int              nFlavor,
            bool             complexFields = true);

    /**
     * \brief Sets the flavor of the model.
     * \details This function should be used after creating a model with an 
     * empty flavor (in particular using the default constructor of ModelData).
     * It may be used to create the flavor separately, not using built-in 
     * functions of this class. The std::unique_ptr given as parameter loses 
     * the ownership of the flavor and is not valid after the function call. 
     * Here is a code snippet that show how to use this function.
     * \code
     * Model model;
     * std::unique_ptr<mty::Flavor> flavor = makeFlavor();
     * model.setFlavor(std::move(flavor));
     * // Here flavor is not valid anymore, model took the ownership
     * \endcode
     * \warning This function must be called \b before adding any particle or 
     * other property as the flavor must be the first one defined.
     * \param t_flavor Flavor for the model.
     */
    void setFlavor(std::unique_ptr<mty::Flavor> &&t_flavor);

    /**
     * @brief Adds a particle to the model.
     *
     * @details When adding a particle, its gauge representation should be 
     * already set, and the model must be **initialized** (see init()). You can
     * choose to let MARTY create kinetic (including covariant derivative) and 
     * mass terms (default) or disable all automatic interaction terms with the
     * \b initializeTerms parameter.
     *
     * @param part            Particle to add to the model
     * @param initializeTerms Boolean (default = true) that tells if kinetic and
     * mass terms must be added automatically.
     *
     * @sa removeParticle()
     */
    void addParticle(
            mty::Particle const &part,
            bool                 initializeTerms = true);
    /**
     * @brief Adds a list of particles to the model.
     *
     * @details When adding a particle, its gauge representation should be 
     * already set, and the model must be **initialized** (see init()). You can
     * choose to let MARTY create kinetic (including covariant derivative) and 
     * mass terms (default) or disable all automatic interaction terms with the
     * \b initializeTerms parameter.
     *
     * @param part            Particles to add to the model
     * @param initializeTerms Boolean (default = true) that tells if kinetic and
     * mass terms must be added automatically.
     *
     * @sa removeParticle()
     */
    void addParticles(
            std::vector<mty::Particle> const &parts,
            bool                              initializeTerms = true);

    void addParticlesIn(csl::Expr const &expr);

    /**
     * @brief Removes a particle from the model.
     *
     * @details When removing a particle, all interaction terms containing it 
     * are removed. It is a non-return point. If you simply want to disable a 
     * given particle in calculations, consider using mty::DisableParticle().
     *
     * @param part Particle to remove.
     *
     * @sa addParticle(), mty::DisableParticle()
     */
    void removeParticle(mty::Particle const &part);

    /**
     * @brief Removes particles from the model.
     *
     * @details When removing a particle, all interaction terms containing it 
     * are removed. It is a non-return point. If you simply want to disable a 
     * given particle in calculations, consider using mty::DisableParticle().
     *
     * @param parts Particles to remove.
     *
     * @sa addParticle(), mty::DisableParticle()
     */
    void removeParticles(std::vector<mty::Particle> const &parts);

    /**
     * @brief Removes a particle from the model.
     *
     * @details When removing a particle, all interaction terms containing it 
     * are removed. It is a non-return point. If you simply want to disable a 
     * given particle in calculations, consider using mty::DisableParticle().
     *
     * @param part Name of the particle to remove.
     *
     * @sa addParticle(), mty::DisableParticle()
     */
    void removeParticle(std::string const &part);

    /**
     * @brief Removes particles from the model.
     *
     * @details When removing a particle, all interaction terms containing it 
     * are removed. It is a non-return point. If you simply want to disable a 
     * given particle in calculations, consider using mty::DisableParticle().
     *
     * @param parts Names of the particles to remove.
     *
     * @sa addParticle(), mty::DisableParticle()
     */
    void removeParticles(std::vector<std::string> const &parts);

    /**
     * @brief Adds a scalar coupling constant in the model.
     *
     * @details This function creates a csl::Constant named \b name and adds it
     * to the list of coupling constants.
     *
     * @param t_name Name for the coupling constant.
     */
    void addScalarCoupling(std::string_view t_name);
    /**
     * @brief Adds a scalar coupling constant in the model.
     *
     * @details This function adds \b coupling to the list of coupling constants.
     *
     * @param coupling New coupling constant.
     */
    void addScalarCoupling(csl::Expr const &coupling);

    /**
     * @brief Adds a tensor coupling constant in the model.
     *
     * @details This function adds \b tensorCoupling to the list of tensor 
     * coupling constants.
     *
     * @param tensorCoupling New tensorial coupling constant.
     */
    void addTensorCoupling(csl::Parent const &tensorCoupling);

    /**
     * @brief Adds a new quantum number to a model, giving eigenvalues for 
     * particles also.
     *
     * @details You must give a list of particles, and a list of eigenvalues. 
     * The list of eigenvalues may be empty: in this case, all eigenvalues are
     * considered to be 1 by default. All particles not present in \b fields 
     * have 0 eigenvalue. Optionally, you may set \b conserved to false if you 
     * do not want this quantum number to interfere in calculations. Here is a
     * code snippet that shows how to use this function, considering for example
     * a quark q, and a lepton l (both of type mty::Particle).
     * \code
     * model.addQuantumNumber("F", {l, q}, {}, false);
     * // l and q have fermionic number F = 1, and it is not 
     * // used in calculations
     * model.addQuantumNumber("B", {l, q}, {0, 1});
     * // Baryonic number 
     * model.addQuantumNumber("L", {l, q}, {1, 0});
     * // Leptonic number 
     * \endcode
     *
     * @param name      Name of the quantum number
     * @param fields    List of fields for which a non-trivial eigenvalue is 
     * defined
     * @param values    Eigenvalues for the fields (all 1 if the list is empty)
     * @param conserved Boolean (default = true) that tells if the quantum 
     * number must be conserved in calculations.
     */
    void addQuantumNumber(
            std::string_view                  name,
            std::vector<mty::Particle> const& fields,
            std::vector<int>           const& values,
            bool                              conserved = true
            );
    /**
     * @brief Adds a new quantum number to a model, giving eigenvalues for 
     * particles also.
     *
     * @details You must give a list of particles, and a list of eigenvalues. 
     * The list of eigenvalues may be empty: in this case, all eigenvalues are
     * considered to be 1 by default. All particles not present in \b fields 
     * have 0 eigenvalue. Optionally, you may set \b conserved to false if you 
     * do not want this quantum number to interfere in calculations. Here is a
     * code snippet that shows how to use this function, considering for example
     * a quark q, and a lepton l (both of type mty::Particle).
     * \code
     * model.addQuantumNumber("F", {l, q}, {}, false);
     * // l and q have fermionic number F = 1, and it is not 
     * // used in calculations
     * model.addQuantumNumber("B", {l, q}, {0, 1});
     * // Baryonic number 
     * model.addQuantumNumber("L", {l, q}, {1, 0});
     * // Leptonic number 
     * \endcode
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle()
     * function
     * @param name      Name of the quantum number
     * @param fields    List of fields for which a non-trivial eigenvalue is 
     * defined
     * @param values    Eigenvalues for the fields (all 1 if the list is empty)
     * @param conserved Boolean (default = true) that tells if the quantum 
     * number must be conserved in calculations.
     */
    template<class FieldType>
        void addQuantumNumber(
                std::string_view              name,
                std::vector<FieldType> const& fields,
                std::vector<int>       const& values,
                bool                          conserved = true
                );
    /**
     * @brief Adds a new quantum number to a model, giving eigenvalues for 
     * particles also.
     *
     * @details You must give a list of particles, and a list of eigenvalues. 
     * The list of eigenvalues may be empty: in this case, all eigenvalues are
     * considered to be 1 by default. All particles not present in \b fields 
     * have 0 eigenvalue. Optionally, you may set \b conserved to false if you 
     * do not want this quantum number to interfere in calculations. Here is a
     * code snippet that shows how to use this function, considering for example
     * a quark q, and a lepton l (both of type mty::Particle).
     * \code
     * model.addQuantumNumber("F", {l, q}, {}, false);
     * // l and q have fermionic number F = 1, and it is not 
     * // used in calculations
     * model.addQuantumNumber("B", {l, q}, {0, 1});
     * // Baryonic number 
     * model.addQuantumNumber("L", {l, q}, {1, 0});
     * // Leptonic number 
     * \endcode
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle()
     * function
     * @param name      Name of the quantum number
     * @param fields    List of fields for which a non-trivial eigenvalue is 
     * defined
     * @param values    Eigenvalues for the fields (all 1 if the list is empty)
     * @param conserved Boolean (default = true) that tells if the quantum 
     * number must be conserved in calculations.
     */
    template<class FieldType>
        void addQuantumNumber(
                std::string_view                 name,
                std::initializer_list<FieldType> fields,
                std::vector<int>          const &values,
                bool                             conserved = true
                );

    /**
     * @brief Sets the quantum numbers of the model.
     *
     * @details This function should in principle not be used. It replaces the
     * entire set of quantum numbers, without defining eigenvalues for 
     * particles. Consider using addQuantumNumber() instead.
     *
     * @param numbers New quantum numbers
     */
    void setQuantumNumbers(
            std::vector<mty::QuantumNumber> const &numbers
            );
        

    ///////////////////////////////////////////////////
    // Content
    ///////////////////////////////////////////////////

    /**
     * @brief Searches a particle in the model.
     *
     * @param t_name Name of the particle to search.
     *
     * @return \b True  if the particle exists
     * @return \b False else
     *
     * @sa getParticle()
     */
    bool findParticle(std::string_view t_name) const;
    /**
     * @brief Searches a particle in the model.
     *
     * @param part Particle to search in the model
     *
     * @return \b True  if the particle exists
     * @return \b False else
     *
     * @sa getParticle()
     */
    bool findParticle(mty::Particle const &part) const;
    /**
     * @brief Searches a particle in the model.
     *
     * @param part Particle to search in the model
     *
     * @return \b True  if the particle exists
     * @return \b False else
     *
     * @sa getParticle()
     */
    bool findParticle(mty::QuantumFieldParent const *part) const;

    /**
     * @brief Searches a scalar coupling in the model.
     *
     * @param t_name Name of the coupling to search.
     *
     * @return \b True  if the coupling exists
     * @return \b False else
     *
     * @sa getScalarCoupling()
     */
    bool findScalarCoupling(std::string_view t_name) const;
    /**
     * @brief Searches a scalar coupling in the model.
     *
     * @param coupling Coupling to search
     *
     * @return \b True  if the coupling exists
     * @return \b False else
     *
     * @sa getScalarCoupling()
     */
    bool findScalarCoupling(csl::Expr const &coupling) const;

    /**
     * @brief Searches a tensor coupling in the model.
     *
     * @param t_name Name of the coupling to search.
     *
     * @return \b True  if the coupling exists
     * @return \b False else
     *
     * @sa getTensorCoupling()
     */
    bool findTensorCoupling(std::string_view t_name) const;
    /**
     * @brief Searches a tensor coupling in the model.
     *
     * @param coupling Coupling to search in the model
     *
     * @return \b True  if the coupling exists
     * @return \b False else
     *
     * @sa getTensorCoupling()
     */
    bool findTensorCoupling(csl::Parent const &coupling) const;
    /**
     * @brief Searches a tensor coupling in the model.
     *
     * @param coupling Coupling to search in the model
     *
     * @return \b True  if the coupling exists
     * @return \b False else
     *
     * @sa getTensorCoupling()
     */
    bool findTensorCoupling(csl::AbstractParent const *coupling) const;

    ///////////////////////////////////////////////////
    // Finders
    ///////////////////////////////////////////////////

    /**
     * @brief Returns a Group (pointer to const) given its name.
     *
     * @param t_name Name of the group to search
     *
     * @return The group of name \b t_name in the model
     *
     * @sa getGaugedGroup()
     */
    mty::Group const *getGroup(std::string_view t_name) const;
    /**
     * @brief Returns a Group (pointer) given its name.
     *
     * @param t_name Name of the group to search
     *
     * @return The group of name \b t_name in the model
     *
     * @sa getGaugedGroup()
     */
    mty::Group *getGroup(std::string_view t_name);

    /**
     * @brief Returns a Group (pointer to const) given its flavor group.
     *
     * @param flavGroup Initial flavor group.
     *
     * @return The group of flavor \b flavGroup in the model
     *
     * @sa getFlavorGroup()
     */
    mty::Group const *getGroup(mty::FlavorGroup const *flavGroup) const;
    /**
     * @brief Returns a Group (pointer to const) given its flavor group.
     *
     * @param flavGroup Initial flavor group.
     *
     * @return The group of flavor \b flavGroup in the model
     *
     * @sa getFlavorGroup()
     */
    mty::Group *getGroup(mty::FlavorGroup const *flavGroup);
    /**
     * @brief Returns a GaugedGroup (pointer to const) given its name.
     *
     * @param t_name Name of the gauged group to search
     *
     * @return The gauged group of name \b t_name in the model
     *
     * @sa getGroup()
     */
    mty::GaugedGroup const *getGaugedGroup(std::string_view t_name) const;
    /**
     * @brief Returns a GaugedGroup (pointer) given its name.
     *
     * @param t_name Name of the gauged group to search
     *
     * @return The gauged group of name \b t_name in the model
     *
     * @sa getGroup()
     */
    mty::GaugedGroup *getGaugedGroup(std::string_view t_name);

    /**
     * @brief Returns a FlavorGroup (pointer) given its name.
     *
     * @param t_name Name of the flavor group to search
     *
     * @return The flavor group of name \b t_name in the model
     *
     * @sa getGroup()
     */
    mty::FlavorGroup const *getFlavorGroup(std::string_view t_name) const;
    /**
     * @brief Returns a FlavorGroup (pointer) given its name.
     *
     * @param t_name Name of the flavor group to search
     *
     * @return The flavor group of name \b t_name in the model
     *
     * @sa getGroup()
     */
    mty::FlavorGroup *getFlavorGroup(std::string_view t_name);

    std::vector<mty::Particle> getParticles(
            std::initializer_list<std::string_view> names
            ) const;
    /**
     * @brief Returns a particle in the model given a name.
     *
     * @details In case no particle is found, an error message is printed and 
     * the code stops. If you want to know if a particle exists, consider using
     * findParticle() instead.
     *
     * @param t_name Name of the particle to get.
     *
     * @return The particle of name \b name.
     */
    mty::Particle getParticle(std::string_view t_name) const;
    /**
     * @brief Returns a particle in the model given a pointer to
     * QuantumFieldParent.
     *
     * @details In case no particle is found, an error message is printed and 
     * the code stops. If you want to know if a particle exists, consider using
     * findParticle() instead.
     *
     * @param field Field from which you want a mty::Particle type.
     *
     * @return The mty::Particle (a std::shared_ptr to **field**). 
     */
    mty::Particle getParticle(mty::QuantumFieldParent const *field) const;
    /**
     * @brief Returns a particle in the model given a QuantumFieldParent.
     *
     * @details In case no particle is found, an error message is printed and 
     * the code stops. If you want to know if a particle exists, consider using
     * findParticle() instead.
     *
     * @param field Field from which you want a mty::Particle type.
     *
     * @return The mty::Particle (a std::shared_ptr to \b field). 
     */
    mty::Particle getParticle(mty::QuantumFieldParent const &field) const;
    /**
     * @brief Returns a particle in the model given a QuantumField.
     *
     * @details In case no particle is found, an error message is printed and 
     * the code stops. If you want to know if a particle exists, consider using
     * findParticle() instead.
     *
     * @param field Field from which you want a mty::Particle type.
     *
     * @return The mty::Particle (a std::shared_ptr to \b field's parent). 
     */
    mty::Particle getParticle(mty::QuantumField const &field) const;
    /**
     * @brief Returns a particle in the model given an expression.
     *
     * @details In case no particle is found, an error message is printed and 
     * the code stops. If you want to know if a particle exists, consider using
     * findParticle() instead. The expression \b field must of course be of type
     * QuantumField, i.e. pointing to an object of this type.
     *
     * @param field Field from which you want a mty::Particle type.
     *
     * @return The mty::Particle (a std::shared_ptr to \b field's parent). 
     */
    mty::Particle getParticle(csl::Expr const &field) const;

    /**
     * @brief Returns a scalar coupling given its name.
     *
     * @details If the scalar coupling does not exist, this function prints an 
     * error message and stops the program. If you want to know if a scalar 
     * coupling exists, try using findScalarCoupling() instead.
     *
     * @param t_name Name of the scalar coupling to get
     *
     * @return The scalar coupling of name \b t_name
     */
    csl::Expr getScalarCoupling(std::string_view t_name) const;

    /**
     * @brief Returns a tensor coupling given its name.
     *
     * @details If the tensor coupling does not exist, this function prints an 
     * error message and stops the program. If you want to know if a tensor 
     * coupling exists, try using findTensorCoupling() instead.
     *
     * @param t_name Name of the tensor coupling to get
     *
     * @return The tensor coupling of name \b t_name
     */
    csl::Tensor getTensorCoupling(std::string_view t_name) const;
    /**
     * @brief Returns a tensor coupling.
     *
     * @details If the tensor coupling does not exist, this function prints an 
     * error message and stops the program. If you want to know if a tensor 
     * coupling exists, try using findTensorCoupling() instead.
     *
     * @param t_coupling Coupling to search in the model
     *
     * @return The tensor coupling of name \b t_name
     */
    csl::Tensor getTensorCoupling(csl::Parent const &t_coupling) const;

    /**
     * @brief Returns the gauge representation of a field.
     *
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     *
     * @return The gauge representation of the particle.
     */
    template<class FieldType>
        mty::GaugeIrrep getGaugeIrrep(FieldType &&field) const;

    /**
     * @brief Returns the group representation of a field.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     * @param group Group.
     *
     * @return The group representation of \b field in \b group.
     */
    template< class GroupType, class FieldType >
        mty::Irrep getGroupIrrep(
                FieldType &&field,
                GroupType &&group
                ) const;

    /**
     * @brief Returns the charge representation of a field in a U(1) group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     * @param group U(1) group.
     *
     * @return The charge representation of \b field in \b group.
     */
    template< class GroupType, class FieldType >
        csl::Expr getCharge(
                FieldType &&field,
                GroupType &&group
                ) const;

    /**
     * @brief Returns the flavor representation of a field.
     *
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     *
     * @return The flavor representation of the particle.
     */
    template<class FieldType>
        mty::FlavorIrrep getFlavorIrrep(FieldType &&field) const ;

    /**
     * @brief Returns the flavor group representation of a field.
     *
     * @tparam FlavorType mty::Flavor* or valid type for the function 
     * getFlavorGroup()
     * @tparam FieldType  mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     * @param flavor Flavor group.
     *
     * @return The flavor group representation of \b field in \b flavor.
     */
    template< class FlavorType, class FieldType >
        mty::Irrep getFlavorIrrep(
                FieldType  &&field,
                FlavorType &&flavor
                ) const;

    /**
     * @brief Returns the vector space coresponding to the representation of a 
     * field in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     * @param group Group.
     *
     * @return The vector space coresponding to the representation of a 
     * field in a given group.
     */
    template< class GroupType, class FieldType >
        csl::Space const *getVectorSpace(
                GroupType &&group,
                FieldType &&field
                ) const;
        

    /**
     * @brief Returns an index coresponding to the representation of a field 
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param field Particle.
     * @param group Group.
     *
     * @return An index coresponding to the representation of a 
     * field in a given group.
     */
    template< class GroupType, class FieldType >
        csl::Index generateIndex(
                GroupType &&group,
                FieldType &&field
                ) const;

    /**
     * @brief Returns an index coresponding to the fundamental representation
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @param group Group.
     *
     * @warning For now only enabled for flavor groups, not gauged groups.
     *
     * @return An index coresponding to the fundamental representation of a 
     * given group.
     */
    template< class GroupType >
        csl::Index generateIndex(
                GroupType &&group
                ) const;

    /**
     * @brief Returns indices coresponding to the representation of a field 
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param nIndices Number of indices to generate.
     * @param group    Group.
     * @param field    Particle.
     *
     * @return \b nIndices indices coresponding to the representation of a 
     * field in a given group.
     */
    template< class GroupType, class FieldType >
        std::vector<csl::Index> generateIndices(
                size_t      nIndices,
                GroupType &&group,
                FieldType &&field
                ) const;

    /**
     * @brief Returns indices coresponding to the fundamental representation
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @param group Group.
     *
     * @warning For now only enabled for flavor groups, not gauged groups.
     *
     * @return Indices coresponding to the fundamental representation of a 
     * given group.
     */
    template< class GroupType >
        std::vector<csl::Index> generateIndices(
                size_t      nIndices,
                GroupType &&group
                ) const;

        
    /**
     * @brief Returns an index coresponding to the representation of a field 
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param name  Name for the generated index.
     * @param group Group.
     * @param field Particle.
     *
     * @return An index coresponding to the representation of a 
     * field in a given group.
     */
    template< class GroupType, class FieldType >
        csl::Index generateIndex(
                std::string_view nameIndex,
                GroupType      &&group,
                FieldType      &&field
                ) const;
        
    /**
     * @brief Returns the generator coresponding to the representation of a field 
     * in a given group.
     *
     * @tparam GroupType mty::Group* or valid type for the function getGroup()
     * @tparam FieldType mty::Particle or valid type for the function 
     * getParticle()
     * @param group Group.
     * @param field Particle.
     *
     * @return The generator coresponding to the representation of a 
     * field in a given group if it exists.
     * @return nullptr else.
     */
    template< class GroupType, class FieldType >
        mty::Generator getGenerator(
                GroupType &&group,
                FieldType &&field
                ) const;
    
    /**
     * @brief Returns a quantum number (const reference) given its name.
     *
     * @details If the quantum number does not exist, this function prints an 
     * error message and stops the program.
     *
     * @param t_name Name of the quantum number to get
     *
     * @return The quantum number of name \b t_name
     */
    mty::QuantumNumber const &getQuantumNumber(std::string_view name) const;
    /**
     * @brief Returns a quantum number (reference) given its name.
     *
     * @details If the quantum number does not exist, this function prints an 
     * error message and stops the program.
     *
     * @param t_name Name of the quantum number to get
     *
     * @return The quantum number of name \b t_name
     */
    mty::QuantumNumber &getQuantumNumber(std::string_view name);

    ///////////////////////////////////////////////////
    // Model building
    ///////////////////////////////////////////////////

    /**
     * @brief Checks the validity of a particle when adding it to the model.
     *
     * @details This function checks if the gauge, flavor of the particle are
     * the same as the ones defined in the model. If the particle is not valid,
     * an error message is printed and the program stops.
     *
     * @param part Particle to check
     */
    void checkValidity(mty::Particle const &part) const;

    /**
     * @brief Checks the validity of a term when adding it to the Lagrangian.
     *
     * @details This function simply computes the tensor product of the gauge 
     * representations of all particles in the interaction (taking conjugate
     * representations for conjugate fields) and checks that the trivial gauge
     * representation exists in the resulting decomposition. This represents a 
     * quick check of gauge invariance, even though the exact gauge invariance
     * is not guaranteed at this point.
     *
     * @param term Term to check
     */
    void checkValidity(mty::Lagrangian::TermType const &term) const;

    /**
     * @brief Adds a lagrangian term to the model.
     *
     * @details The term can be a kinetic, mass term, or interaction term. It 
     * must be of course gauge invariant (see checkValidity()), and have no free
     * indices (scalar term). Setting \b addHermitic to true, to hermitian 
     * conjugate will be computed and added automatically. For example, you may
     * have in a Lagrangian
     * \f[
     *     \mathcal{L} \ni \frac{g}{\sqrt{2}} \bar{u}W^+\cdot \gamma P_Ld + h.c,
     * \f]
     * (in that case the '+h.c' corresponds to \b addHermitic = true), or
     * \f[
     *     \mathcal{L} \ni \frac{2}{3}e\bar{u}A\cdot \gamma u,
     * \f]
     * that do not have an hermitian counter part.
     *
     * @param term        Lagrangian term to add
     * @param addHermitic Boolean (default = false) that tells if the hermitian
     * conjugate of the term must be also added
     */
    void addLagrangianTerm(
            csl::Expr const &term,
            bool        addHermitic = false);
    /**
     * @brief Adds a lagrangian term to the model.
     *
     * @details The term can be a kinetic, mass term, or interaction term. It 
     * must be of course gauge invariant (see checkValidity()), and have no free
     * indices (scalar term). Setting \b addHermitic to true, to hermitian 
     * conjugate will be computed and added automatically. For example, you may
     * have in a Lagrangian
     * \f[
     *     \mathcal{L} \ni \frac{g}{\sqrt{2}} \bar{u}W^+\cdot \gamma P_Ld + h.c,
     * \f]
     * (in that case the '+h.c' corresponds to \b addHermitic = true), or
     * \f[
     *     \mathcal{L} \ni \frac{2}{3}e\bar{u}A\cdot \gamma u,
     * \f]
     * that do not have an hermitian counter part.
     *
     * @param term        Lagrangian term to add
     * @param addHermitic Boolean (default = false) that tells if the hermitian
     * conjugate of the term must be also added
     */
    void addLagrangianTerm(
            mty::Lagrangian::TermType const &term,
            bool                             addHermitic = false);

    /**
     * @brief Adds a bosonic mass term to the model.
     *
     * @details A bosonic mass term for a field \f$ \phi \f$ of mass \f$ m \f$
     * is of the form
     * \f[ \mathcal{L}_M \ni \pm \eta m^2 \phi ^\dagger \phi \f],
     * with \f$ \eta = 1/2 \f$ if \f$ \phi ^\dagger = \phi\f$, \f$ \eta = 1 \f$
     * otherwise. For spin 1 particles, the sign is positive, and negative for
     * spin 0 particles.
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle() 
     * function.
     * @param field Field in the mass term (must be bosonic).
     * @param mass  Name of the mass to add.
     */
    template<class FieldType>
        void addBosonicMass(
                FieldType      &&field,
                std::string_view mass
                );
    /**
     * @brief Adds a bosonic mass term to the model.
     *
     * @details A bosonic mass term for a field \f$ \phi \f$ of mass \f$ m \f$
     * is of the form
     * \f[ \mathcal{L}_M \ni \pm \eta m^2 \phi ^\dagger \phi \f],
     * with \f$ \eta = 1/2 \f$ if \f$ \phi ^\dagger = \phi\f$, \f$ \eta = 1 \f$
     * otherwise. For spin 1 particles, the sign is positive, and negative for
     * spin 0 particles.
     * After the function call, the mass of the boson is set to the
     * given mass \f$ m \f$, and any other mass term with $$\phi$$ is 
     * suppressed.
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle() 
     * function.
     * @param field Field in the mass term (must be bosonic).
     * @param mass  Mass to add (must be scalar).
     */
    template<class FieldType>
        void addBosonicMass(
                FieldType      &&field,
                csl::Expr      const &mass
                );

    /**
     * @brief Adds a fermionic mass to the model.
     *
     * @details This function adds a Weyl mass term for \b left and \b right. 
     * Considering two Weyl fermions \f$ \psi _L \f$ and \f$ \psi _R \f$, the 
     * mass term reads
     * \f[
     *     m\left(\psi _L^\dagger \psi _R + \psi _R^\dagger \psi _L\right).
     * \f]
     * After the function call, the mass of the two weyl fermions is set to the
     * given mass \f$ m \f$, and any other mass term with \f$ \psi_ L \f$ or 
     * \f$ \psi _R \f$ is suppressed.
     *
     * @tparam FieldType1 mty::Particle or any valid type for the getParticle()
     * function.
     * @tparam FieldType2 mty::Particle or any valid type for the getParticle()
     * function.

     * @param left  Left-handed Weyl fermion.
     * @param right Right-handed Weyl fermion.
     * @param mass  Name of the mass for the fermion.
     */
    template<class FieldType1, class FieldType2>
        void addFermionicMass(
                FieldType1     &&left,
                FieldType2     &&right,
                std::string_view mass
                );
    /**
     * @brief Adds a fermionic mass to the model.
     *
     * @details This function adds a Weyl mass term for \b left and \b right. 
     * Considering two Weyl fermions \f$ \psi _L \f$ and \f$ \psi _R \f$, the 
     * mass term reads
     * \f[
     *     m\left(\psi _L^\dagger \psi _R + \psi _R^\dagger \psi _L\right).
     * \f]
     * After the function call, the mass of the two weyl fermions is set to the
     * given mass \f$ m \f$, and any other mass term with \f$ \psi_ L \f$ or 
     * \f$ \psi _R \f$ is suppressed.
     *
     * @tparam FieldType1 mty::Particle or any valid type for the getParticle()
     * function.
     * @tparam FieldType2 mty::Particle or any valid type for the getParticle()
     * function.

     * @param left  Left-handed Weyl fermion.
     * @param right Right-handed Weyl fermion.
     * @param mass  Mass for the fermion.
     */
    template<class FieldType1, class FieldType2>
        void addFermionicMass(
                FieldType1 &&left,
                FieldType2 &&right,
                csl::Expr const &mass
                );
    /**
     * @brief Adds a fermionic mass to the model.
     *
     * @details This function adds a simple fermionic mass term for a particle, 
     * actually similar to a bosonic mass term replacing \f$ \phi ^* \f$ by 
     * $\bar{\psi}$. See addBosonicMass() for more details.
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle()
     * function.

     * @param diracFermion Dirac fermion.
     * @param mass         Name of the mass for the fermion.
     */
    template<class FieldType>
        void addFermionicMass(
                FieldType      &&diracFermion,
                std::string_view mass
                );
    /**
     * @brief Adds a fermionic mass to the model.
     *
     * @details This function adds a simple fermionic mass term for a particle, 
     * actually similar to a bosonic mass term replacing \f$ \phi ^\dagger \f$ 
     * by $\bar{\psi}$. See addBosonicMass() for more details.
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle()
     * function.

     * @param diracFermion Dirac fermion.
     * @param mass         Mass for the fermion.
     */
    template<class FieldType>
        void addFermionicMass(
                FieldType &&diracFermion,
                csl::Expr const &mass
                );

    /**
     * @brief Renames a particle of the model.
     *
     * @tparam FieldType mty::Particle or any valid type for the getParticle()
     * function.
     * 
     * @param field   Particle to rename
     * @param newName New name for the particle
     */
    template<class FieldType>
    void renameParticle(
            FieldType      &&field,
            std::string_view newName
            );

    /**
     * @brief Renames a (scalar or tensor) coupling of the model.
     *
     * @param name    Initial name of the coupling
     * @param newName New name for the particle
     */
    void renameCoupling(
            std::string_view name,
            std::string_view newName
            );

    ///////////////////////////////////////////////////
    // Display
    ///////////////////////////////////////////////////

    /**
     * @brief Prints a sub part of the Lagrangian including some particles.
     *
     * @param particles Particles that printed terms must have
     * @param out       Output stream (std::cout, standard stream by default)
     */
    void printSubPart(
            std::vector<std::string> const &particles,
            std::ostream &out = std::cout
            ) const;
    /**
     * @brief Prints a sub part of the Lagrangian including some particles.
     *
     * @param particles Particles that printed terms must have
     * @param out       Output stream (std::cout, standard stream by default)
     */
    void printSubPart(
            std::initializer_list<std::string> particles,
            std::ostream &out = std::cout
            ) const;
    /**
     * @brief Prints a sub part of the Lagrangian including some particles.
     *
     * @param particles Particles that printed terms must have
     * @param out       Output stream (std::cout, standard stream by default)
     */
    void printSubPart(
            std::string const &particle,
            std::ostream &out = std::cout
            ) const;

    /**
     * @brief Prints the model.
     *
     * @details This function prints out the model gauge, the particle content
     * and the Lagrangian (kinetic, mass, and interaction terms).
     *
     * @param out   Output stream (default is standard stream std::cout).
     * @param model Model to output.
     *
     * @return 
     */
    friend std::ostream &operator<<(
            std::ostream    &out,
            ModelData const &model);

protected:

    ///////////////////////////////////////////////////
    // Protected member functions
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Init functions
    ///////////////////////////////////////////////////

    /**
     * @brief Initializes gauge terms (gauge bosons, ghosts) and fixes the 
     * gauge.
     *
     * @param initGaugeTerms Boolean (default = true) that tells if kinetic and
     * mass terms must be added automatically.
     */
    void initGaugedGroups(bool initGaugeTerms = true);

    ///////////////////////////////////////////////////
    // Write functions
    ///////////////////////////////////////////////////

    void writeHeader(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined
            );
    void writeGauge(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined
            );
    void writeFlavor(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined
            );
    void writeMassAndWidth(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined
            );
    void writeParticles(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined
            );
    void writeDependencies(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Parent> &defined,
            std::vector<csl::Expr> &kinetic,
            std::vector<csl::Expr> &mass,
            std::vector<csl::Expr> &interac
            );
    void writeLagrangian(
            std::ostream &out,
            int           indentSize,
            std::vector<csl::Expr> const &kinetic,
            std::vector<csl::Expr> const &mass,
            std::vector<csl::Expr> const &interac
            );

    ///////////////////////////////////////////////////
    // Helper functions for template overloads
    ///////////////////////////////////////////////////

    /**
     * @brief Actually returns the gauge representation of a particle.
     *
     * @param part Particle from which we want the representation.
     *
     * @return The gauge representation of \b part.
     * 
     * @sa getGaugeIrrep()
     */
    mty::GaugeIrrep doGetGaugeIrrep(mty::Particle const &part) const;
    /**
     * @brief Actually returns the group representation of a particle.
     *
     * @param part  Particle from which we want the representation
     * @param group Group
     *
     * @return The group representation of \b part or the group \b group.
     * 
     * @sa getGroupIrrep()
     */
    mty::Irrep doGetGroupIrrep(
            mty::Particle   const &part,
            mty::Group      const *group
            ) const;
    /**
     * @brief Actually returns the charge representation of a particle for a 
     * U(1) group.
     *
     * @param part  Particle from which we want the charge
     * @param group U(1) Group
     *
     * @return The charge representation of \b part or the group \b group.
     * 
     * @sa getCharge()
     */
    csl::Expr doGetCharge(
            mty::Particle const &particle,
            mty::Group    const *group
            ) const;

    /**
     * @brief Actually returns the flavor representation of a particle.
     *
     * @param part Particle from which we want the representation.
     *
     * @return The flavor representation of \b part.
     * 
     * @sa getFlavorIrrep()
     */
    mty::FlavorIrrep doGetFlavorIrrep(mty::Particle const &part) const;
    /**
     * @brief Actually returns the flavor group representation of a particle.
     *
     * @param part  Particle from which we want the representation
     * @param group Flavor group
     *
     * @return The flavor group representation of \b part or the group \b group.
     * 
     * @sa getFlavorIrrep()
     */
    mty::Irrep doGetFlavorIrrep(
            mty::Particle    const &part,
            mty::Group       const *flavor
            ) const;

    /**
     * @brief Actually returns the vector space corresponding to a particle's 
     * representation.
     *
     * @param group Group
     * @param irrep Particle
     *
     * @return The vector space of the group representation of \b irrep in the 
     * group \b group.
     *
     * @sa getVectorSpace()
     */
    csl::Space const *doGetVectorSpace(
            mty::Group    const *group,
            mty::Particle const &irrep
            ) const; 

    /**
     * @brief Actually returns the vector space corresponding to a particle's 
     * representation.
     *
     * @param group Flavor group
     * @param irrep Particle
     *
     * @return The vector space of the flavor group representation of \b irrep
     * in the flavor group \b group.
     *
     * @sa getVectorSpace()
     */
    csl::Space const *doGetVectorSpace(
            mty::FlavorGroup const *group,
            mty::Particle    const &irrep
            ) const; 

    /**
     * @brief Actually returns a csl::Index corresponding to a particle's 
     * representation.
     *
     * @param group    Group
     * @param fieldRep Particle
     *
     * @return An index corresponding to the group representation of \b irrep 
     * in the group \b group.
     *
     * @sa generateIndex()
     */
    csl::Index doGenerateIndex(
            mty::Group    const *group,
            mty::Particle const &fieldRep
            ) const;

    /**
     * @brief Actually returns a csl::Index corresponding to the fundamental
     * representation of a given group.
     * @param group    Group
     *
     * @return An index corresponding to the fundamental representation 
     * of the group \b group.
     *
     * @warning For now only enabled for flavor groups, not gauged groups.
     *
     * @sa generateIndex()
     */
    csl::Index doGenerateIndex(
            mty::Group    const *group
            ) const;
    /**
     * @brief Actually returns a csl::Index corresponding to a particle's 
     * representation.
     *
     * @param name     Name of the index to generate
     * @param group    Group
     * @param fieldRep Particle
     *
     * @return An index corresponding to the group representation of \b irrep 
     * in the group \b group.
     *
     * @sa generateIndex()
     */
    csl::Index doGenerateIndex(
            std::string_view     nameIndex,
            mty::Group    const *group,
            mty::Particle const &fieldRep
            ) const;

    /**
     * @brief Actually returns a generator corresponding to a particle's 
     * representation.
     *
     * @param group    Group
     * @param fieldRep Particle
     *
     * @return The generator corresponding to the group representation of 
     * \b fieldRep in the group \b group if it is not trivial.
     * @return nullptr else
     *
     * @sa generateIndex()
     */
    mty::Generator doGetGenerator(
            mty::Group    const *group,
            mty::Particle const &fieldRep
            ) const;

    /*
     * @brief Actually adds a bosonic mass term to the Lagrangian.
     *
     * @param particle Particle in the mass term.
     * @param mass     Name of the mass for the particle.
     *
     * @sa addBosonicMass()
     */
    void doAddBosonicMass(
            mty::Particle const &particle,
            std::string_view     mass
            );
    /*
     * @brief Actually adds a bosonic mass term to the Lagrangian.
     *
     * @param particle Particle in the mass term.
     * @param mass     Mass of the particle.
     *
     * @sa addBosonicMass()
     */
    void doAddBosonicMass(
            mty::Particle const &particle,
            csl::Expr          const &mass
            );

    /*
     * @brief Actually adds a fermionic mass term to the Lagrangian.
     *
     * @param left  Left-handed Weyl fermion.
     * @param right Right-handed Weyl fermion.
     * @param mass  Name of the mass for the particle.
     *
     * @sa addFermionicMass()
     */
    void doAddFermionicMass(
            mty::Particle const &left,
            mty::Particle const &right,
            std::string_view     mass
            );
    /*
     * @brief Actually adds a fermionic mass term to the Lagrangian.
     *
     * @param left  Left-handed Weyl fermion.
     * @param right Right-handed Weyl fermion.
     * @param mass  Mass of the particle.
     *
     * @sa addFermionicMass()
     */
    void doAddFermionicMass(
            mty::Particle const &left,
            mty::Particle const &right,
            csl::Expr          const &mass
            );
    /*
     * @brief Actually adds a fermionic mass term to the Lagrangian.
     *
     * @param diracFermion Dirac fermion.
     * @param mass         Name of the mass for the particle.
     *
     * @sa addFermionicMass()
     */
    void doAddFermionicMass(
            mty::Particle const &diracFermion,
            std::string_view     mass
            );
    /*
     * @brief Actually adds a fermionic mass term to the Lagrangian.
     *
     * @param diracFermion Dirac fermion.
     * @param mass         Mass of the particle.
     *
     * @sa addFermionicMass()
     */
    void doAddFermionicMass(
            mty::Particle const &diracFermion,
            csl::Expr          const &mass
            );

    /**
     * @brief Actually renames a particle.
     *
     * @param part    Particle to rename.
     * @param newName New name for the particle.
     *
     * @sa renameParticle()
     */
    void doRenameParticle(
            mty::Particle const &part,
            std::string_view     newName
            );
   
protected:

    /**
     * @brief Lagrangian of the model.
     *
     * @details The lagrangian is modified only through built-in functions, 
     * either from this class (ModelData) for basic content modifications, or
     * from the ModelBuilder class for more complicated model building features
     * (symmetry breaking, replacements etc).
     */
    mty::Lagrangian                 L;

    /**
     * @brief Space time for the model.
     *
     * @details By default, this variable is a pointer to csl::Minkowski (4D). 
     * There is actually no support for another space time so for now, nothing
     * to do with this member.
     */
    csl::Space const               *spaceTime;

    /**
     * @brief Gauge of the model.
     */
    std::unique_ptr<mty::Gauge>     gauge;

    /**
     * @brief Flavor of the model.
     */
    std::unique_ptr<mty::Flavor>    flavor;

    /**
     * @brief Particle content (including gauge bosons, ghosts etc) of the 
     * model.
     *
     * @sa getParticle(), addParticle()
     */
    std::vector<mty::Particle>      particles;

    /**
     * @brief List of quantum numbers in the model. 
     *
     * @details Particles can have different eigenvalues for these quantum 
     * numbers (0 by default, if not given).
     *
     * @sa getQuantumNumbers(), getQuantumNumber(), addQuantumNumber()
     */
    std::vector<mty::QuantumNumber> quantumNumbers;

    /**
     * @brief Lists of scalar couplings of the model.
     *
     * @sa getScalarCouplings(), getScalarCoupling(), addScalarCoupling()
     */
    std::vector<csl::Expr>               scalarCouplings;

    /**
     * @brief List of tensor couplings of the model.
     *
     * @sa getTensorCouplings(), getTensorCoupling(), addTensorCoupling()
     */
    std::vector<csl::Tensor>        tensorCouplings;

    /**
     * @brief Boolean that tells if the gauge model is initilialized.
     *
     * @details When building a model, the gauge must be created first before
     * adding any other particle. The gauge must be fixed by calling the init()
     * function. Only then, particles can be added using addParticle() function.
     *
     * @sa init(), setGauge(), addGaugedGroup(), ModelData()
     */
    bool gaugeLocked = false;

    /**
     * @brief Boolean telling if checks must be done with lagrangian terms added
     * to the model (gauge representation, dispatching ...)
     *
     * @sa addLagrangianTerm(), isEnableChecks(), setEnableCheck()
     */
    bool enableChecks = true;
};


///////////////////////////////////////////////////
// Template functions for ModelData setters
///////////////////////////////////////////////////

template<class FieldType>
    void ModelData::addQuantumNumber(
            std::string_view              name,
            std::vector<FieldType> const& fields,
            std::vector<int>       const& values,
            bool                          conserved
            )
    {
        std::vector<mty::Particle> parts;
        parts.reserve(fields.size());
        for (FieldType const &f : fields)
            parts.push_back(getParticle(f));
        addQuantumNumber(name, parts, values, conserved);
    }
template<class FieldType>
    void ModelData::addQuantumNumber(
            std::string_view                 name,
            std::initializer_list<FieldType> fields,
            std::vector<int>          const &values,
            bool                             conserved
            )
    {
        addQuantumNumber(
                name, 
                std::vector<FieldType>(fields),
                values, 
                conserved);
    }

///////////////////////////////////////////////////
// Template functions for ModelData finders
///////////////////////////////////////////////////


template<class FieldType>
    mty::GaugeIrrep ModelData::getGaugeIrrep(FieldType &&field) const 
    {
        constexpr bool field_valid =
            std::is_convertible<FieldType, mty::Particle const&>::value;

        if constexpr(field_valid)
            return doGetGaugeIrrep(field);
        else
            return getGaugeIrrep(
                    getParticle(std::forward<FieldType>(field)));
    }

template< class GroupType, class FieldType >
    csl::Expr ModelData::getCharge(
            FieldType &&field,
            GroupType &&group
            ) const
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGetCharge(field, group);
            else
                return getCharge(
                        getParticle(std::forward<FieldType>(field)), 
                        group);
        }
        else if constexpr (field_valid)
            return getCharge(
                    field, 
                    getGroup(std::forward<GroupType>(group)));
        else 
            return getCharge(
                    getParticle(std::forward<FieldType>(field)), 
                    getGroup(std::forward<GroupType>(group)));
    }

template< class GroupType, class FieldType >
    mty::Irrep ModelData::getGroupIrrep(
            FieldType &&field,
            GroupType &&group
            ) const
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGetGroupIrrep(field, group);
            else
                return getGroupIrrep(
                        getParticle(std::forward<FieldType>(field)), 
                        group);
        }
        else if constexpr (field_valid)
            return getGroupIrrep(
                    field, 
                    getGroup(std::forward<GroupType>(group)));
        else 
            return getGroupIrrep(
                    getParticle(std::forward<FieldType>(field)), 
                    getGroup(std::forward<GroupType>(group)));
    }

template<class FieldType>
    mty::FlavorIrrep ModelData::getFlavorIrrep(FieldType &&field) const 
    {
        constexpr bool field_valid =
            std::is_convertible<FieldType, mty::Particle const&>::value;

        if constexpr(field_valid)
            return doGetFlavorIrrep(field);
        else
            return getFlavorIrrep(
                    getParticle(std::forward<FieldType>(field)));
    }

template< class FlavorType, class FieldType >
    mty::Irrep ModelData::getFlavorIrrep(
            FieldType  &&field,
            FlavorType &&flavor
            ) const
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        constexpr bool flavor_valid = 
            std::is_convertible<
                FlavorType, 
                mty::Group const*>::value;

        if constexpr(flavor_valid) {
            if constexpr(field_valid) 
                return doGetFlavorIrrep(field, flavor);
            else
                return getFlavorIrrep(
                        getParticle(std::forward<FieldType>(field)), 
                        flavor);
        }
        else if constexpr (field_valid)
            return getFlavorIrrep(
                    field, 
                    getGroup(std::forward<FlavorType>(flavor))
                    );
        else 
            return getFlavorIrrep(
                    getParticle(std::forward<FieldType>(field)), 
                    getGroup(std::forward<FlavorType>(flavor)));
    }

template< class GroupType, class FieldType >
    csl::Space const *ModelData::getVectorSpace(
            GroupType &&group,
            FieldType &&field
            ) const
    {
        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGetVectorSpace(group, field);
            else
                return getVectorSpace(group, 
                        getParticle(std::forward<FieldType>(field)));
        }
        else if constexpr (field_valid)
            return getVectorSpace(
                    getGroup(std::forward<GroupType>(group)), 
                    field);
        else
            return getVectorSpace(
                    getGroup(std::forward<GroupType>(group)), 
                    getParticle(std::forward<FieldType>(field)));
    }

template< class GroupType, class FieldType >
    csl::Index ModelData::generateIndex(
            GroupType &&group,
            FieldType &&field
            ) const
    {
        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGenerateIndex(group, field);
            else
                return generateIndex(
                        group, 
                        getParticle(std::forward<FieldType>(field)));
        }
        else if constexpr (field_valid)
            return generateIndex(
                    getGroup(std::forward<GroupType>(group)), 
                    field);
        else 
            return generateIndex(
                    getGroup(std::forward<GroupType>(group)), 
                    getParticle(std::forward<FieldType>(field)));
    }

template< class GroupType >
    csl::Index ModelData::generateIndex(
            GroupType &&group
            ) const
    {
        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        if constexpr(group_valid) {
            return doGenerateIndex(group);
        }
        else 
            return generateIndex(
                    getGroup(std::forward<GroupType>(group))
                    );
    }

    template< class GroupType, class FieldType >
    std::vector<csl::Index> ModelData::generateIndices(
            size_t      nIndices,
            GroupType &&group,
            FieldType &&field
            ) const
    {
        std::vector<csl::Index> indices(nIndices);
        for (size_t i = 0; i != nIndices; ++i) 
            indices[i] = generateIndex<GroupType, FieldType>(
                    std::forward<GroupType>(group),
                    std::forward<FieldType>(field)
                    );

        return indices;
    }

    template< class GroupType >
    std::vector<csl::Index> ModelData::generateIndices(
            size_t      nIndices,
            GroupType &&group
            ) const
    {
        std::vector<csl::Index> indices(nIndices);
        for (size_t i = 0; i != nIndices; ++i) 
            indices[i] = generateIndex<GroupType>(
                    std::forward<GroupType>(group)
                    );

        return indices;
    }

template< class GroupType, class FieldType >
    csl::Index ModelData::generateIndex(
            std::string_view nameIndex,
            GroupType      &&group,
            FieldType      &&field
            ) const
    {
        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGenerateIndex(nameIndex, group, field);
            else
                return generateIndex(
                        nameIndex, 
                        group, 
                        getParticle(std::forward<FieldType>(field)));
        }
        else if constexpr (field_valid)
            return generateIndex(
                    nameIndex, 
                    getGroup(std::forward<GroupType>(group)), 
                    field);
        else 
            return generateIndex(
                    nameIndex, 
                    getGroup(std::forward<GroupType>(group)), 
                    getParticle(std::forward<FieldType>(field)));
    }

template< class GroupType, class FieldType >
    mty::Generator ModelData::getGenerator(
            GroupType &&group,
            FieldType &&field
            ) const
    {
        constexpr bool group_valid = 
            std::is_convertible<
                GroupType, 
                mty::Group const*>::value;

        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr(group_valid) {
            if constexpr(field_valid) 
                return doGetGenerator(group, field);
            else
                return getGenerator(
                        group, 
                        getParticle(std::forward<FieldType>(field)));
        }
        else if constexpr (field_valid)
            return getGenerator(
                    getGroup(std::forward<GroupType>(group)), 
                    field);
        else 
            return getGenerator(
                    getGroup(std::forward<GroupType>(group)), 
                    getParticle(std::forward<FieldType>(field)));
    }

template<class FieldType>
    void ModelData::addBosonicMass(
            FieldType      &&field,
            std::string_view mass
            )
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr (field_valid)
            doAddBosonicMass(field, mass);
        else
            addBosonicMass(
                    getParticle(std::forward<FieldType>(field)),
                    mass
                    );
    }
template<class FieldType>
    void ModelData::addBosonicMass(
            FieldType      &&field,
            csl::Expr      const &mass
            )
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr (field_valid)
            doAddBosonicMass(field, mass);
        else
            addBosonicMass(
                    getParticle(std::forward<FieldType>(field)),
                    mass
                    );
    }

template<class FieldType1, class FieldType2>
    void ModelData::addFermionicMass(
            FieldType1     &&left,
            FieldType2     &&right,
            std::string_view mass
            )
    {
        constexpr bool field1_valid = 
            std::is_convertible<
                FieldType1, 
                mty::Particle const&>::value;
        constexpr bool field2_valid = 
            std::is_convertible<
                FieldType2, 
                mty::Particle const&>::value;

        if constexpr(field1_valid) {
            if constexpr(field2_valid) 
                return doAddFermionicMass(left, right, mass);
            else
                return addFermionicMass(
                        left,
                        getParticle(std::forward<FieldType2>(right)),
                        mass
                        );
        }
        else if constexpr (field2_valid)
                return addFermionicMass(
                        getParticle(std::forward<FieldType1>(left)),
                        right,
                        mass
                        );
        else 
                return addFermionicMass(
                        getParticle(std::forward<FieldType1>(left)),
                        getParticle(std::forward<FieldType2>(right)),
                        mass
                        );
    }
template<class FieldType1, class FieldType2>
    void ModelData::addFermionicMass(
            FieldType1 &&left,
            FieldType2 &&right,
            csl::Expr  const &mass
            )
    {
        constexpr bool field1_valid = 
            std::is_convertible<
                FieldType1, 
                mty::Particle const&>::value;
        constexpr bool field2_valid = 
            std::is_convertible<
                FieldType2, 
                mty::Particle const&>::value;

        if constexpr(field1_valid) {
            if constexpr(field2_valid) 
                return doAddFermionicMass(left, right, mass);
            else
                return addFermionicMass(
                        left,
                        getParticle(std::forward<FieldType2>(right)),
                        mass
                        );
        }
        else if constexpr (field2_valid)
                return addFermionicMass(
                        getParticle(std::forward<FieldType1>(left)),
                        right,
                        mass
                        );
        else 
                return addFermionicMass(
                        getParticle(std::forward<FieldType1>(left)),
                        getParticle(std::forward<FieldType2>(right)),
                        mass
                        );
    }
template<class FieldType>
    void ModelData::addFermionicMass(
            FieldType      &&diracFermion,
            std::string_view mass
            )
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr (field_valid)
            doAddFermionicMass(diracFermion, mass);
        else
            addFermionicMass(
                    getParticle(std::forward<FieldType>(diracFermion)),
                    mass
                    );
    }
template<class FieldType>
    void ModelData::addFermionicMass(
            FieldType &&diracFermion,
            csl::Expr const &mass
            )
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr (field_valid)
            doAddFermionicMass(diracFermion, mass);
        else
            addFermionicMass(
                    getParticle(std::forward<FieldType>(diracFermion)),
                    mass
                    );
    }


template<class FieldType>
    void ModelData::renameParticle(
            FieldType      &&field,
            std::string_view newName
            )
    {
        constexpr bool field_valid = 
            std::is_convertible<
                FieldType, 
                mty::Particle const&>::value;

        if constexpr(field_valid)
            return doRenameParticle(field, newName);
        else
            return renameParticle(
                    getParticle(std::forward<FieldType>(field)), 
                    newName);
    }

} // End of namespace mty

#endif 
