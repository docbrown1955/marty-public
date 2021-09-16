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
 * \brief File containing classes handling gauge groups.
 * \details Many of formulas applied for group generators may be found in
 * http://chaosbook.org/GroupTheory/refs/PRD14-76.pdf, 'Group theory for
 * Feynman diagrams in non-Abelian gauge theories' (Predrag Cvitanovic,
 * PRD 1976).
 */
#ifndef GAUGEDGROUP_H_INCLUDED
#define GAUGEDGROUP_H_INCLUDED

#include "group.h"
#include "SU_N_algebra.h"
#include "generator.h"

namespace mty::gauge {

    enum class GroupType {
        U1,
        SU,
        SO,
        Sp,
        E6,
        E7,
        E8,
        F4,
        G2
    };

    std::ostream& operator<<(
            std::ostream& fout,
            GroupType     type
            );
}



namespace mty {

class QuantumField;
class QuantumFieldParent;
class Particle;
class GaugeBoson;
class GhostBoson;
class Gauge;

} // End of namespace mty

/*!
 * \brief Namespace containing features concerning gauged groups and gauge
 * choices.
 */
namespace mty::gauge {
/*!
 * \brief Different types of gauge ficing parameter for gauge boson propagators.
 * \details The propagator of a gauge boson is 
 * \f[
 *      -i\frac{g_{\mu\nu} - (1-\xi) \frac{p_\mu p_\nu}{p^2 - \xi M^2}}{p^2-M^2},
 * \f]with \f$ M \f$ the mass of the 
 * particle, and \f$ p^\mu \f$ its momentum. 
 * The Lorenz gauge corresponds to \f$ \xi=0\f$, the propagator then reads
 * \f[
 *      -i\frac{g_{\mu\nu} - \frac{p_\mu p_\nu}{p^2}}{p^2-M^2}.
 * \f]
 * The Feynman gauge corresponds to \f$ \xi=1\f$, the propagator then reads
 * \f[
 *      -i\frac{g_{\mu\nu}}{p^2-M^2}.
 * \f]
 * The Unitary gauge corresponds to \f$ \xi=\infty\f$, the propagator then reads
 * \f[
 *      -i\frac{g_{\mu\nu} - \frac{p_\mu p_\nu}{M^2}}{p^2-M^2}.
 * \f]
 * Finally, the \f$ \mathcal{R}_\xi \f$ gauge does not fix \f$ \xi \f$, this 
 * corresponds to the choice NotDefined.
 */
enum Type {

    NotDefined, /*!< \f$\xi \f$ is not fixed*/
    Lorenz,  /*!< \f$\xi = 0\f$.*/
    Feynman, /*!< \f$\xi = 1\f$.*/
    Unitary, /*!< \f$\xi = \infty\f$.*/
};

class Choice {

    public:

    Choice()
        :choice(Type::Feynman),
        xi(csl::variable_s("xi"))
    {
        xi->setElementary(true);
        setGauge(Type::Feynman);
    }

    csl::Expr getXi() const {
        return xi;
    }

    Type getChoice() const {
        return choice;
    }

    void setGauge(Type t_choice)
    {
        choice = t_choice;
        csl::Expr value;
        switch(choice) {
            case Lorenz:  value = CSL_0; break;
            case Feynman: value = CSL_1; break;
            case Unitary: value = CSL_INF; break;
            default:      value = CSL_UNDEF;
        }
        xi->setValue(value);
    }

    private:
        
    Type choice;
    const csl::Expr xi;
};

} // End of namespace mty::gauge

namespace mty {

template<class Type>
class IrrepMap {

public:

    using Pair = std::pair<mty::Irrep, Type>;

    IMPLEMENTS_STD_VECTOR_NO_OP(Pair, elements)

    const_iterator find(mty::Irrep const &t_key) const {
        auto iter = begin();
        while (iter != end()) {
            if (key(*iter) == t_key)
                break;
            ++iter;
        }
        return iter;
    }
    iterator find(mty::Irrep const &t_key) {
        auto iter = begin();
        while (iter != end()) {
            if (key(*iter) == t_key)
                break;
            ++iter;
        }
        return iter;
    }

    Type &operator[](mty::Irrep const &rep) {
        for (auto &el : elements)
            if (key(el) == rep)
                return value(el);
        elements.push_back({rep, Type()});
        return value(elements.back());
    }

private:

    inline static mty::Irrep &key  (Pair &p) { return p.first;  };
    inline static Type       &value(Pair &p) { return p.second; };
    inline static mty::Irrep const &key  (Pair const &p) { return p.first;  };
    inline static Type       const &value(Pair const &p) { return p.second; };

    std::vector<Pair> elements;
};


class GaugedGroup {

    public:

    GaugedGroup(SemiSimpleGroup* t_group,
                Gauge* gauge,
                std::string const& bosonName,
                bool addGhost);

    GaugedGroup(SemiSimpleGroup* t_group,
                Gauge* gauge,
                std::string const& bosonName,
                bool addGhost,
                const csl::Expr& t_coupling);

    virtual ~GaugedGroup();

    virtual mty::gauge::GroupType getType() const = 0;

    std::string getName() const {
        return group->getName();
    }

    SemiSimpleGroup* getGroup() const;

    bool isBroken() const;

    SemiSimpleAlgebra const *getAlgebra() const {
        return group->getAlgebra();
    }

    mty::gauge::Choice getGaugeChoice() const;

    mty::Generator getF() const;

    csl::Tensor getD(size_t p) const;

    virtual csl::Tensor getDTilde() const;

    void setGaugeChoice(gauge::Type t_choice);

    std::string getNameGenerator() const;

    void setNameGenerator(const std::string& newName);

    csl::Expr getCouplingConstant() const;

    void setBroken(bool t_broken);

    void setCouplingConstant(const csl::Expr& expr);

    Particle buildVectorBoson();

    Particle buildGhost();

    AlgebraState getHighestWeight(const csl::Space* space) const;

    virtual csl::Expr covariantDerivative(
            QuantumFieldParent&            field,  
            const csl::Index&              mu,    
            const std::vector<csl::Index>& fieldIndices,    
            csl::Tensor&                  point) = 0;

    csl::Expr covariantDerivative(const csl::Expr&      field,
                             const csl::Index& mu);

    mty::Generator getGenerator(const Irrep& field);
    mty::Generator getGenerator(const QuantumFieldParent& field);
    mty::Generator getGenerator(std::vector<int> const &labels);

    protected:

    bool isFieldInTrivialRep(const QuantumFieldParent& field) const;

    virtual
    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator);

    bool hasGeneratorForRep(const Irrep& irrep) const;

    virtual void generateRepGenerator(const Irrep& irrep);

    void initF();

    protected:
    
    Gauge*                         gauge;

    SemiSimpleGroup*               group;

    bool                           broken;

    csl::Expr                      coupling;

    std::string                    nameGenerator = "T";

    std::string                    nameBoson;

    bool                           ghostEnabled = false;

    const csl::Space*              adjointSpace;

    mty::Generator                 f;

    mutable
    std::map<size_t, csl::Tensor> d;

    IrrepMap<mty::Generator>      generator;

    std::shared_ptr<GaugeBoson>   boson;

    std::shared_ptr<GhostBoson>   ghost;

    /*!
     * \brief Association of AlgebraState (highest weights of irreps) and 
     * vector spaces. To each irrep (i.e. each highest weight) is associated
     * a different and unique vector space (csl::Space). See getVectorSpace().
     */
    mutable
    std::vector<std::pair<AlgebraState, const csl::Space*>> vectorSpaces;
};


class U1Gauged: public GaugedGroup {

    public:

    U1Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson);

    U1Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             const csl::Expr& t_coupling);

    ~U1Gauged(){};

    mty::gauge::GroupType getType() const override;

    csl::Expr covariantDerivative(QuantumFieldParent&           field,
                             const csl::Index&              mu,
                             const std::vector<csl::Index>& fieldIndices,
                             csl::Tensor&                  point) override;
};

class NonAbelianGauged: public GaugedGroup {

    public:

    NonAbelianGauged(SemiSimpleGroup* t_group,
                     Gauge* gauge,
                     std::string const& nameBoson,
                     bool addGhost);

    NonAbelianGauged(SemiSimpleGroup* t_group,
                     Gauge* gauge,
                     std::string const& nameBoson,
                     bool addGhost,
                     const csl::Expr& t_coupling);

    ~NonAbelianGauged(){};

    csl::Expr covariantDerivative(QuantumFieldParent&           field,
                             const csl::Index&              mu,
                             const std::vector<csl::Index>& fieldIndices,
                             csl::Tensor&                  point) override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    protected:

    bool isAdjointGenerator(mty::Generator const &tensor) const;

    int getGeneratorDim(mty::Generator const &tensor) const;
};

class SUGauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_HALF;

    public:

    SUGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    SUGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~SUGauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    private:

    void init_f_d_ABC();
};

class SOGauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    public:

    SOGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    SOGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~SOGauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    csl::Tensor getDTilde() const override;

    void initDTilde();

    private:

    csl::Tensor d_tilde;
};

class SpGauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    public:

    SpGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    SpGauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~SpGauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

};

class E6Gauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    inline static csl::Expr alpha = 5 * CSL_HALF; 

    public:

    E6Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    E6Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~E6Gauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    void generateRepGenerator(const Irrep& irrep) override;

    private:

    std::map<Irrep, csl::Tensor> d_invariant;
};

class E7Gauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    inline static csl::Expr alpha = CSL_THIRD;

    public:

    E7Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    E7Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~E7Gauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    void generateRepGenerator(const Irrep& irrep) override;

    private:

    std::map<Irrep, csl::Tensor> d_invariant;
};

class E8Gauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    public:

    E8Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    E8Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~E8Gauged(){};

    mty::gauge::GroupType getType() const override;
};

class F4Gauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    inline static csl::Expr alpha = csl::int_s(7) / 3;

    public:

    F4Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    F4Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~F4Gauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    void generateRepGenerator(const Irrep& irrep) override;

    private:

    std::map<Irrep, csl::Tensor> d_invariant;
};

class G2Gauged: public NonAbelianGauged {

    public:

    inline static csl::Expr a = CSL_1;

    inline static csl::Expr alpha = csl::int_s(-6);

    public:

    G2Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost);

    G2Gauged(SemiSimpleGroup* t_group,
             Gauge* gauge,
             std::string const& nameBoson,
             bool addGhost,
             const csl::Expr& t_coupling);

    ~G2Gauged(){};

    mty::gauge::GroupType getType() const override;

    void setGeneratorProperties(Irrep const& rep,
                                mty::Generator& generator) override;

    void generateRepGenerator(const Irrep& irrep) override;

    private:

    std::map<Irrep, csl::Tensor> f_invariant;

};

std::unique_ptr<GaugedGroup> createGaugedGroup(
        SemiSimpleGroup* group,
        Gauge* gauge,
        std::string nameBoson,
        bool addGhost = true,
        const csl::Expr& coupling = CSL_UNDEF);

} // End of namespace mty

#endif
