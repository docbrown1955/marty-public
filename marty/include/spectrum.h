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
 * @file spectrum.h
 * @brief File containing the Spectrum class, handling numerical diagonalization
 * for High-Energy physics models.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-08-31
 */
#ifndef SPECTRUM_H_INCLUDED
#define SPECTRUM_H_INCLUDED

#include "csl.h"
#include "lagrangian.h"
#include "matrix.h"

namespace mty {

class Particle;
struct MassBlock;

/**
 * @brief Class handling numerical diagonalization for HEP models.
 *
 * @details For some mass mixings, the standard way to proceed in MARTY (when 
 * a simple diagonalization is not possible) is to create a symbolic mixing 
 * matrix, fully general, that enters amplitudes. Calculations are done with
 * these mixings. If later one wants to specify numerically the initial mass
 * matrix, this class may diagonalize it and recover the mixing matrix from it.
 * Then, the actual (numerical) mixings may be applied on an expression (for 
 * example an amplitude or Wilson coefficient), replacing old masses by 
 * numerical eigenvalues and symbolic mixings by their values. 
 *
 * There is three parts when using this object. First: the initialization from
 * a MassBlock and mixing terms. It needs the initial mass matrix, and the 
 * mixing matrix that you instored to symbolically diagonalize it in full 
 * generality. Then, once all the matrix elements of the initial mass matrix 
 * have a numerical value you must call updateData() to initialize the 
 * numerical matrix. Finally, you may call applyOn() to apply this 
 * diagonalization to an expression (or several).
 *
 * Two types of diagonalization are possible. For a given quantum field 
 * \f$ \Phi \f$ (bosonic or fermionic), you may have something like
 * \f$ \Phi ^\dagger M \Phi \f$ in which case \f$ M \f$ is an hermitian 
 * matrix as the Lagrangian must be real. A **standard diagonalization** must
 * then be performed, i.e.
 * \f[
 *      \Phi \mapsto \Phi ^\prime = U\Phi,\\
 *      \Phi ^\dagger \mapsto \Phi ^{\prime\dagger} = \Phi ^\dagger U^\dagger,\\
 *      UMU^\dagger \equiv D, \text{ the diagonal mass matrix. }
 * \f]
 * Another possibility are terms of the form \f$ \Phi _L^\dagger M \Phi _R
 * + \Phi _R^\dagger M^\dagger \Phi_L \f$, with \f$ \Phi _L \f$ and
 * \f$ \Phi _R \f$ different fields. In that case, a **bi-diagonalization**
 * must be performed, i.e.
 * \f[
 *      \Phi _R \mapsto \Phi _R^\prime = U\Phi _R,\\
 *      \Phi _L \mapsto \Phi _L^\prime = V\Phi _L,\\
 *      V^\dagger MU = D, \text{ the diagonal mass matrix, }\\
 *      V^\dagger MM^\dagger V = D^2,\\
 *      U^\dagger M^\dagger M U = D^2\\
 * \f]
 * In this case there is actually two diagonalizations to perform, for 
 * \f$ MM^\dagger \f$ and \f$ M^\dagger M \f$. 
 *
 * @sa csl::Diagonalizer
 */
class Spectrum {

public:

    /**
     * @brief Using for matrices in boost libraries.
     *
     * @tparam T Type of the value type in the matrix.
     */
    template<class T>
        using matrix    = boost::numeric::ublas::matrix<T>;

    /**
     * @brief Using for identity matrices in boost libraries.
     *
     * @tparam T Type of the value type in the matrix.
     */
    template<class T>
        using id_matrix = boost::numeric::ublas::identity_matrix<T>;
    /**
     * @brief Type of floating point variable for numerical diagonalization.
     */
    using FloatType = double;

    /**
     * @brief Small structure containing a mass term (without fields) and its
     * position in the mass matrix.
     */
    struct MatrixEl {
        /**
         * @brief Row index of the term.
         */
        std::size_t i;
        /**
         * @brief Column index of the term.
         */
        std::size_t j;
        /**
         * @brief Actual term. Fields are removed, it should then be a purely
         * scalar factor.
         */
        csl::Expr   term;
    };

    /**
     * @brief Constructor with 3 parameters, for standard diagonalization.
     *
     * @param t_particles    Particles to diagonalize.
     * @param t_newParticles New particles introduces after rotation.
     * @param terms          Vector of mass terms for \b particles.
     * @param t_mixing       Mixing introduced to symbolically diagonalize the
     * mass matrix (2D matrix as vector).
     */
    Spectrum(
            std::vector<mty::Particle>     const &t_particles,
            std::vector<mty::Particle>     const &t_newParticles,
            std::vector<csl::Expr>              const &terms,
            std::vector<std::vector<csl::Expr>> const &t_mixing
            );

    /**
     * @brief Constructor with 3 parameters, for standard diagonalization.
     *
     * @details This constructor will not do anything apart from reading 
     * parameters. These parameters are considered to have already been treated
     * by a Spectrum object. It us used in particular to re-build Spectrum 
     * object in a different program (Lagrangian code generation).
     *
     * @param t_newParticles New particles introduces after rotation.
     * @param t_mass         Initial mass matrix for the Spectrum object.
     * @param t_mixing       Mixing introduced to symbolically diagonalize the
     * mass matrix (2D matrix as vector).
     * @param t_mixing       Mixing introduced to symbolically diagonalize the
     * mass matrix (2D matrix as vector) for right particles, if there is.
     */
    Spectrum(
            std::vector<mty::Particle>     const &t_newParticles,
            std::vector<std::vector<csl::Expr>> const &t_mass,
            std::vector<std::vector<csl::Expr>> const &t_mixing,
            std::vector<std::vector<csl::Expr>> const &t_mixing2
                = std::vector<std::vector<csl::Expr>>()
            );

    /**
     * @brief Constructor with 6 parameters, for bi-diagonalization.
     *
     * @param partA    Left block of particles to diagonalize
     * @param partB    Right block of particles to diagonalize
     * @param newPartA New particles introduces after rotation, left block.
     * @param newPartB New particles introduces after rotation, right block.
     * @param terms    Vector of mass terms for the particles.
     * @param mixingA  Mixing introduced to symbolically diagonalize the
     * mass matrix (2D matrix as vector) for left particles.
     * @param mixingB  Mixing introduced to symbolically diagonalize the
     * mass matrix (2D matrix as vector) for right particles.
     */
    Spectrum(
            std::vector<mty::Particle> const &partA,
            std::vector<mty::Particle> const &partB,
            std::vector<mty::Particle> const &newPartA,
            std::vector<mty::Particle> const &newPartB,
            std::vector<csl::Expr>          const &terms,
            std::vector<std::vector<csl::Expr>> const &mixingA,
            std::vector<std::vector<csl::Expr>> const &mixingB
            );

    /**
     * @brief Default constructor.
     */
    Spectrum()  = default;
    /**
     * @brief Default destructor.
     */
    ~Spectrum() = default;

    /**
     * @brief Default copy constructor.
     */
    Spectrum(Spectrum const &) = default;
    /**
     * @brief Default move constructor.
     */
    Spectrum(Spectrum &&)      = default;
    /**
     * @brief Default copy assignement operator.
     */
    Spectrum &operator=(Spectrum const &) = default;
    /**
     * @brief Default move assignement operator.
     */
    Spectrum &operator=(Spectrum &&)      = default;

    std::vector<mty::Particle> const &getParticles() const {
        return newFields;
    }

    matrix<csl::Expr> const &getMassTerms() const {
        return mass;
    }

    matrix<csl::Expr> const &getMixings() const {
        return mixing;
    }

    matrix<csl::Expr> const &getMixings2() const {
        return mixing2;
    }

    /**
     * @return \b replaceMasses.
     */
    bool getReplaceMasses() const {
        return replaceMasses;
    }
    /**
     * @return \b replaceMixings.
     */
    bool getReplaceMixings() const {
        return replaceMixings;
    }

    /**
     * @brief Sets \b replaceMasses.
     *
     * @param t_replaceMasses New value for \b replaceMasses.
     */
    void setReplaceMasses(bool t_replaceMasses) {
        replaceMasses = t_replaceMasses;
    }
    /**
     * @brief Sets \b replaceMixings.
     *
     * @param t_replaceMixings New value for \b replaceMixings.
     */
    void setReplaceMixings(bool t_replaceMixings) {
        replaceMasses = t_replaceMixings;
    }

    /**
     * @brief Tells if the mass matrix has been numerically diagonalized.
     *
     * @details If this function returns true, it means that replacement may
     * be applied on expressions, using applyOn(). If initial mass parameters
     * have a numerical value, consider calling updateData(), then checking
     * that everything is going well checking that this function returns true,
     * and finally apply the diagonalization on any expression you want using
     * applyOn().
     *
     * @return \b True  if all masses and mixings have been computed 
     * numerically.
     * @return \b False else.
     *
     * @sa applyOn(), updateData()
     */
    bool isDiagonalized() const;

    /**
     * @brief Instantiates the Spectrum object with a mass terms and a mixing
     * matrix for standard diagonalization.
     *
     * @details This function simply gathers all terms in \b terms to create 
     * the mass matrix. The mixing matrix used to symbolically diagonalize the 
     * fields must also be given.
     *
     * @param terms    Mass terms for the fields.
     * @param t_mixing Mixing matrix.
     */
    void setBlock(
            std::vector<csl::Expr>              const &terms,
            std::vector<std::vector<csl::Expr>> const &t_mixing
            );

    /**
     * @brief Instantiates the Spectrum object with a mass terms and two mixing
     * matrices for bi-diagonalization.
     *
     * @details This function simply gathers all terms in \b terms to create 
     * the mass matrix. The mixing matrices used to symbolically diagonalize 
     * the fields must also be given.
     *
     * @param terms   Mass terms for the fields.
     * @param mixingA Mixing matrix of left fields.
     * @param mixingB Mixing matrix of right fields.
     */
    void setBlock(
            std::vector<csl::Expr>              const &terms,
            std::vector<std::vector<csl::Expr>> const &mixingA,
            std::vector<std::vector<csl::Expr>> const &mixingB
            );
    /**
     * @brief Updates the mass matrix, if the elements have values (for example)
     * after a lha file loading.
     *
     * @details This function should be called once all the initial elements of
     * the mass matrix have a defined value. 
     *
     * To properly define a value, you must use the function 
     * csl::Abstract::setValue() of the symbolic constants. For example with 
     * a mass matrix \f$ \left(\begin{array}{cc}a && b \\ c && d 
     * \end{array}\right),\f$ one must write 
     * \code
     *          M[0][0]->setValue(523.5); // good
     * \endcode
     * and **not**
     * \code
     *          M[0][0] = 523.5; // very bad
     * \endcode
     * In the latter case, the matrix element is replaced by the value 523.5,
     * but the variable a that actually appears in the Spectrum class still has
     * no value. The diagonalization cannot be performed then.
     */
    void updateData();
    /**
     * @brief Applies the diagonalization on an expression, replacing masses
     * and mixings.
     *
     * @details This function should be called if updateData() has already been
     * called and if all elements of the initial mass matrix have defined 
     * values.
     *
     * @param expr Expression on which the diagonalization is applied.
     */
    void applyOn(csl::Expr &expr) const;
    /**
     * @brief Applies the diagonalization on a vector of expressions, replacing 
     * masses and mixings.
     *
     * @details This function should be called if updateData() has already been
     * called and if all elements of the initial mass matrix have defined 
     * values.
     *
     * @param expr Expressions on which the diagonalization is applied.
     */
    void applyOn(std::vector<csl::Expr> &expr) const;

protected:

    /**
     * @brief Returns the position of the field \b field in the list.
     *
     * @param field QuantumField.
     *
     * @return The position of \b field in the attribute \b fields.
     */
    std::size_t getFieldPos(QuantumField const &field) const;

    /*!
     * @brief Returns a MatrixEl object from a Lagrangian term.
     *
     * @param term Lagrangian term.
     *
     * @return The MatrixEl containing the position of the term in the matrix,
     * and the term itself.
     */
    MatrixEl getMassFromTerm(
            csl::Expr const &term
            ) const;

    /**
     * @brief Replaces corresponding mixings / masses in an expression,
     * if the numerical diagonalization has been performed.
     *
     * @details Is abbreviations are encountered, the content of the 
     * abbreviation os also looked up for the replacement.
     *
     * @param expr    Expression in which we apply the diagonalization.
     * @param oldExpr Old mixings / masses.
     * @param newExpr New mixings / masses.
     */
    static
    void replace(
            csl::Expr                    &expr, 
            std::vector<csl::Expr> const &oldExpr,
            std::vector<csl::Expr> const &newExpr
            );

    /**
     * @brief Adds a mass term to the Spectrum from a MatrixEl object.
     *
     * @param matrixEl Matrix element containing the term and its position in 
     * the mass matrix.
     */
    void addMassTerm(MatrixEl &&matrixEl);

    /**
     * @brief Diagonalizes the mass matrix numerically.
     */
    void diagonalize();

    /**
     * @brief Bi-diagonalizes the mass mattrix numerically.
     */
    void bidiagonalize();

    /**
     * @brief Applies the result of the diagonalization on an expression.
     *
     * @param expr csl::Expr to modify.
     */
    void applyDiagonalizationOn(csl::Expr &expr) const;

    /**
     * @brief Applies the result of the bi-diagonalization on an expression.
     *
     * @param expr csl::Expr to modify.
     */
    void applyBidiagonalizationOn(csl::Expr &expr) const;

protected:

    /**
     * @brief Tells if this class is used for a standard diagonalization (false)
     * or a bi-diagonalization (true). Default is false.
     */
    bool bidiagonalization;
    /**
     * @brief Tells if masses must be directly replaced by their numerical 
     * values when applying diagonalization (default = true).
     */
    bool replaceMasses;
    /**
     * @brief Tells if mixings must be directly replaced by their numerical 
     * values when applying diagonalization (default = true).
     */
    bool replaceMixings;
    /**
     * @brief List of fields in the mass block.
     */
    std::vector<mty::Particle> fields;
    /**
     * @brief List of fields introduced after the rotation.
     */
    std::vector<mty::Particle> newFields;
    /**
     * @brief Mass matrix. This object should contain only quantities with 
     * defined numerical when calling the diagonalization.
     */
    matrix<csl::Expr>               mass;
    /**
     * @brief Initial mixing matrix, contains the symbolic variables that enter
     * expressions before the diagonalization is done.
     */
    matrix<csl::Expr>               mixing;
    /**
     * @brief Second mixing matrix, used for bi-diagonalization.
     */
    matrix<csl::Expr>               mixing2;
    /**
     * @brief Numerical mass matrix, once values have been given and 
     * updateData() has been called.
     */
    csl::Expr                       massData;
    /**
     * @brief Diagonalized mass matrix if the diagonalization has been done.
     */
    csl::Expr                       diagonal = nullptr;
    /**
     * @brief Second transfer matrix used for bi-diagonalization.
     */
    csl::Expr                       transfer = nullptr;
    /**
     * @brief Transfer matrix if the diagonalization has been done.
     */
    csl::Expr                       transfer2 = nullptr;
};

} // End of namespace mty

#endif
