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
 * \brief Contains **interface functions** for the user.
 */
#ifndef HEPINTERFACE_H_INCLUDED
#define HEPINTERFACE_H_INCLUDED

#include "insertion.h"
#include "model.h"

namespace mty {

template <class Type>
struct enable_model {
    using type = typename std::enable_if<std::is_same<
        mty::Model,
        std::remove_reference<std::remove_cv<Type>>>::value>::type;
};

template <class Type>
struct disable_model {
    using type = typename std::enable_if<not std::is_same<
        mty::Model,
        std::remove_reference<std::remove_cv<Type>>>::value>::type;
};

class Insertion;

///////////////////////////////////////////////////
/*************************************************/
// Expression utilities                          //
/*************************************************/
///////////////////////////////////////////////////

/*!
 * \brief Returns a csl::Tensor, vector in csl::Minkowski space.
 * \details The vector is by definition a tensor with one index.
 * For example, a Minkowski vector may be created by:
 * \code
 * csl::Tensor X = MinkowskiVector("X");
 * \endcode
 * Then, assuming that 'mu' is an object of type csl::Index,
 * \code
 *      X(mu)
 * \endcode
 * means the tensor with a lowered index \f$ X_\mu \f$, and
 * \code
 *      X(+mu)
 * \endcode
 * means the tensor with index up \f$ X^\mu \f$. For more information about
 * tensors and indices see csl::TensorElement and csl::Index.
 * \param name Name of the vector.
 * \return An indicial Minkowski vector.
 */
csl::Tensor MinkowskiVector(std::string const &name);

/*!
 * \brief Returns a csl::Index in csl::Minkowki space.
 * \details If no name is provided, the name will be chosen in the name cycle
 * of csl::Minkowski, i.e. one of \f$ \mu, \nu, \rho, \sigma, \lambda, \tau
 * \f$. For example, a Minkowski index may be created with the following code:
 * \code
 * csl::Index mu = mty::MinkowskiIndex("\\mu");
 * \endcode
 * or with a name automatically determined:
 * \code
 * csl::Index mu = mty::MinkowskiIndex();
 * \endcode
 * \param name Name of the index (optional).
 * \return An index living in csl::Minkowski space.
 */
csl::Index MinkowskiIndex(std::string const &name = "");

/*!
 * \brief Returns indices in the 4-dimensional Dirac space mty::dirac4.
 * \details If no name is provided, the name will be chosen in the name cycle
 * of mty::dirac4, i.e. one of \f$ \alpha, \beta, \gamma, \delta, \epsilon \f$.
 * For example, a dirac index may be created with the following code:
 * \code
 * auto alpha = mty::DiracIndices(10);
 * // alpha[0], alpha[1] are indices (csl::Index) in Dirac space
 * \endcode
 * \param n    Number of indices to generate.
 * \param name Name of the index (optional).
 * \return \b n indices living in mty::dirac4 space.
 */
std::vector<csl::Index> DiracIndices(size_t n, std::string const &name = "");

/*!
 * \brief Returns indices in csl::Minkowki space.
 * \details If no name is provided, the name will be chosen in the name cycle
 * of csl::Minkowski, i.e. one of \f$ \mu, \nu, \rho, \sigma, \lambda, \tau
 * \f$. For example, Minkowski indices may be created with the following code:
 * \code
 * auto mu = mty::MinkowskiIndices(10);
 * // mu[0], mu[1] are indices (csl::Index) in csl::Minkowski space.
 * \endcode
 * \param n    Number of indices to generate.
 * \param name Name of the index (optional).
 * \return \b n indices living in csl::Minkowski space.
 */
std::vector<csl::Index> MinkowskiIndices(size_t             n,
                                         std::string const &name = "");

/*!
 * \brief Returns a csl::Index in the 4-dimensional Dirac space mty::dirac4.
 * \details If no name is provided, the name will be chosen in the name cycle
 * of mty::dirac4, i.e. one of \f$ \alpha, \beta, \gamma, \delta, \epsilon \f$.
 * For example, a dirac index may be created with the following code:
 * \code
 * csl::Index alpha = mty::DiracIndex("\\alpha");
 * \endcode
 * or with a name automatically determined:
 * \code
 * csl::Index alpha = mty::DiracIndex();
 * \endcode
 * \param name Name of the index (optional).
 * \return An index living in mty::dirac4 space.
 */
csl::Index DiracIndex(std::string const &name = "");

/*!
 * \brief Returns a csl::Index living in the representation of a certain field
 * in a certain group.
 * \details The model must be given to access the gauge group. The group can be
 * given in the form of a std::string, the name of the group. Finally, the
 * field is given as a mty::Particle. For example, for the standard model 'SM'
 * (a mty::Model object), the 'SU3_c' group, one can get an index with the
 * right name in the octet representation of the gluon 'g' with the following
 * code: \code csl::Index A = mty::GaugeIndex("A", SM, "SU3_c", g); \endcode
 * @tparam GroupType mty::Group* or valid type for the function getGroup()
 * @tparam FieldType mty::Particle or valid type for the function
 * getParticle()
 * @param nameIndex Name for the generated index.
 * @param model     Model that generated the index.
 * @param group     Group.
 * @param field     Particle.
 *
 * \return An index named \b nameIndex living in the vector space (csl::Space)
 * of the representation of \b part in the group named \b nameGroup, in the
 * model \b model.
 */
template <class GroupType, class FieldType>
csl::Index GaugeIndex(std::string const &nameIndex,
                      mty::Model const & model,
                      GroupType &&       group,
                      FieldType &&       part)
{
    return model.generateIndex<GroupType, FieldType>(
        nameIndex,
        std::forward<GroupType>(group),
        std::forward<FieldType>(part));
}

/*!
 * \brief Returns a csl::Index for a given flavor group.
 * \details The model must be given to access the flavor group. The group can
 * be given in the form of a std::string, the name of the group. Finally, the
 * field is given as a mty::Particle.
 * @tparam GroupType mty::FlavorGroup* or valid type for the function
 * getGroup()
 * @param model     Model that generated the index.
 * @param group     Flavor group.
 *
 * \return An index living in the vector space (csl::Space)
 * of the fundamental representation of the group named \b group, in the
 * model \b model.
 */
template <class GroupType>
csl::Index FlavorIndex(mty::Model const &model, GroupType &&group)
{
    return model.generateIndex(std::forward<GroupType>(group));
}

/*!
 * \brief Returns indices for a given flavor group.
 * \details The model must be given to access the flavor group. The group can
 * be given in the form of a std::string, the name of the group. Finally, the
 * field is given as a mty::Particle.
 * @tparam GroupType mty::FlavorGroup* or valid type for the function
 * getGroup()
 * @param model     Model that generated the index.
 * @param group     Flavor group.
 *
 * \return Indices living in the vector space (csl::Space)
 * of the fundamental representation of the group named \b group, in the
 * model \b model.
 */
template <class GroupType>
std::vector<csl::Index>
FlavorIndices(size_t nIndices, mty::Model const &model, GroupType &&group)
{
    std::vector<csl::Index> indices(nIndices);
    for (size_t i = 0; i != nIndices; ++i)
        indices[i]
            = FlavorIndex<GroupType>(model, std::forward<GroupType>(group));

    return indices;
}

/*!
 * \brief Returns a csl::Index living in the representation of a certain field
 * in a certain group.
 * \details The model must be given to access the gauge group. The group can be
 * given in the form of a std::string, the name of the group. Finally, the
 * field is given as a mty::Particle. For example, for the standard model 'SM'
 * (a mty::Model object), the 'SU3_c' group, one can get an index with the
 * right name in the octet representation of the gluon 'g' with the following
 * code: \code csl::Index A = mty::GaugeIndex("A", SM, "SU3_c", g); \endcode
 * @tparam GroupType mty::Group* or valid type for the function getGroup()
 * @tparam FieldType mty::Particle or valid type for the function
 * getParticle()
 * @param model     Model that generated the index.
 * @param group     Group.
 * @param field     Particle.
 *
 * \return An index living in the vector space (csl::Space)
 * of the representation of \b part in the group named \b nameGroup, in the
 * model \b model.
 */
template <class GroupType, class FieldType>
csl::Index
GaugeIndex(mty::Model const &model, GroupType &&group, FieldType &&part)
{
    return model.generateIndex<GroupType, FieldType>(
        std::forward<GroupType>(group), std::forward<FieldType>(part));
}

/*!
 * \brief Returns indices living in the representation of a certain field
 * in a certain group.
 * \details The model must be given to access the gauge group. The group must
 * be given in the form of a std::string, the name of the group. Finally, the
 * field is given as a mty::Particle. For example, for the standard model 'SM'
 * (a mty::Model object), the 'SU3_c' group, one can get an index with the
 * right name in the octet representation of the gluon 'g' with the following
 * code: \code csl::Index A = mty::GaugeIndex("A", SM, "SU3_c", g); \endcode
 * @tparam GroupType mty::Group* or valid type for the function getGroup()
 * @tparam FieldType mty::Particle or valid type for the function
 * getParticle()
 * @param nIndices  Number of indices to generate.
 * @param model     Model that generated the index.
 * @param group     Group.
 * @param field     Particle.
 *
 * \return Indices living in the vector space (csl::Space)
 * of the representation of \b part in the group named \b nameGroup, in the
 * model \b model.
 */
template <class GroupType, class FieldType>
std::vector<csl::Index> GaugeIndices(size_t            nIndices,
                                     mty::Model const &model,
                                     GroupType &&      group,
                                     FieldType &&      part)
{
    std::vector<csl::Index> indices(nIndices);
    for (size_t i = 0; i != nIndices; ++i)
        indices[i]
            = GaugeIndex<GroupType, FieldType>(model,
                                               std::forward<GroupType>(group),
                                               std::forward<FieldType>(part));

    return indices;
}

/*!
 * \brief Returns the vector space corresponding to the representation of a
 * given particle in a given group.
 * \details This function allows to get the vector space associated to any
 * representation in the model, in particular to generate indices in that
 * space. For example in QCD, a quark q and a gluon g would yield for \f$ SU(3)
 * \f$ respectively a 3D and a 8D vector spaces. The code would look like:
 * \code
 *      _3D_space = VectorSpace(QCD, "SU_3", q);
 *      _8D_space = VectorSpace(QCD, "SU_3", g);
 * \endcode
 * \param model     Model in which the particle lives.
 * \param nameGroup Name of the group.
 * \param part      Particle from which we take the representation.
 * \return The vector space of the representation of \b part in the group named
 * \b nameGroup.
 */
const csl::Space *VectorSpace(mty::Model const & model,
                              std::string const &nameGroup,
                              Particle const &   part);

/*!
 * \brief Generates an index in a given space.
 * \details If the name is not given (optional), the name will be taken in the
 * predefined names for the space.
 * \param space Space in which we create an index.
 * \param name  Name of the index (optional).
 * \return An index living in \b space.
 * \sa csl::Space, csl::Index, csl::TensorParent, csl::TensorElement.
 */
csl::Index GenerateIndex(csl::Space const * space,
                         std::string const &name = "");

/*!
 * \brief Returns the Kronecker delta of a given vector space.
 * \details The delta may be called next with indices of the corresponding
 * space. For example, taking \f$ \delta _{ij} \f$ in a space named E3 would
 * look like:
 * \code
 *      csl::Space *E3 = ...;
 *      csl::Index i = GenerateIndex(E3, "i");
 *      csl::Index j = GenerateIndex(E3, "j");
 *      csl::Tensor delta = Delta(E3);
 *      csl::Expr kronecker = delta({i, j});
 * \endcode
 * \param space Vector space from which we want the delta.
 * \return The Kronecker delta of the space.
 * \sa GenerateIndex, csl::Space, csl::Index, csl::TensorParent,
 * csl::TensorElement.
 */
csl::Tensor Delta(const csl::Space *space);

/*!
 * \brief Returns the metric of a given vector space.
 * \details The metric may be called next with indices of the corresponding
 * space. For example, taking \f$ g ^{\mu\nu}g_{\rho\sigma} \f$ in a (signed)
 * space named Minko would look like:
 * \code
 *      csl::Space *Minko = ...;
 *      csl::Index mu    = GenerateIndex(Minko, "\\mu");
 *      csl::Index nu    = GenerateIndex(Minko, "\\nu");
 *      csl::Index rho   = GenerateIndex(Minko, "\\rho");
 *      csl::Index sigma = GenerateIndex(Minko, "\\sigma");
 *      csl::Tensor g = Metric(Minko);
 *      csl::Expr metricTerm = g({+mu, +nu}) * g({rho, sigma});
 * \endcode
 * \param space Vector space from which we want the metric.
 * \return The metric of the space.
 * \sa GenerateIndex, csl::Space, csl::Index, csl::TensorParent,
 * csl::TensorElement.
 */
csl::Tensor Metric(const csl::Space *space);

/*!
 * \brief Returns the epsilon symbol of a given vector space.
 * \details The epsilon may be called next with indices of the corresponding
 * space. For example, taking \f$ \epsilon _{ijk} \f$ in a space named E3 would
 * look like:
 * \code
 *      csl::Space *E3 = ...;
 *      csl::Index i = GenerateIndex(E3, "i");
 *      csl::Index j = GenerateIndex(E3, "j");
 *      csl::Index k = GenerateIndex(E3, "k");
 *      csl::Tensor eps = Epsilon(E3);
 *      csl::Expr antiSym = eps({i, j, k});
 * \endcode
 * \param space Vector space from which we want the epsilon symbol.
 * \return The epsilon symbol of the space.
 * \sa GenerateIndex, csl::Space, csl::Index, csl::TensorParent,
 * csl::TensorElement.
 */
csl::Tensor Epsilon(const csl::Space *space);

/**
 * @return The Dirac tensor \f$ \gamma \f$ in the standard 4-dim Dirac space.
 */
csl::Tensor DiracGamma();
/**
 * @return The Dirac tensor \f$ \gamma ^5\f$ in the standard 4-dim Dirac space.
 */
csl::Tensor DiracGamma5();
/**
 * @return The Dirac tensor \f$ \sigma \f$ in the standard 4-dim Dirac space.
 */
csl::Tensor DiracSigma();
/**
 * @return The Dirac tensor \f$ P_L \f$ in the standard 4-dim Dirac space.
 */
csl::Tensor DiracPL();
/**
 * @return The Dirac tensor \f$ P_R \f$ in the standard 4-dim Dirac space.
 */
csl::Tensor DiracPR();

/**
 * @brief Returns a projector in Dirac 4-dim space depending on a chirality.
 *
 * @param chir Chirality
 *
 * @return Left projector \f$ P_L \f$ for left chirality.
 * @return Right projector \f$ P_R \f$ otherwise.
 */
inline csl::Tensor DiracProjector(Chirality chir)
{
    return (chir == Chirality::Left) ? DiracPL() : DiracPR();
}

csl::Tensor DiracCMatrix();

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
template <class GroupType, class FieldType>
mty::Generator
GetGenerator(mty::Model &model, GroupType &&group, FieldType &&field)
{
    return model.getGenerator(std::forward<GroupType>(group),
                              std::forward<FieldType>(field));
}

/**
 * @brief Returns the vector space corresponding the the representation of a
 * given field in a given group.
 *
 * @tparam GroupType Group* or group name.
 * @tparam FieldType Particle or any type (including a name) that the function
 * ModelData::getParticle() takes.
 * @param model Model
 * @param group Group in which we search the representation.
 * @param field Particle from which we take the representation.
 *
 * @return The vector space for the representation of \b field in
 * \b group.
 */
template <class GroupType, class FieldType>
csl::Space const *
GetVectorSpace(mty::Model &model, GroupType &&group, FieldType &&field)
{
    return model.getVectorSpace(std::forward<GroupType>(group),
                                std::forward<FieldType>(field));
}

/**
 * @brief Returns the vector space corresponding to a flavor group.
 *
 * @tparam GroupType FlavorGroup* or flavor group name.
 * @param model Model
 * @param group Flavor group in which we search the representation.
 *
 * @return The vector space of the \b flavor \b group.
 */
template <class GroupType>
csl::Space const *
GetVectorSpace(mty::Model &model, GroupType &&group)
{
    return model.getVectorSpace(std::forward<GroupType>(group));
}

///////////////////////////////////////////////////
/*************************************************/
// Computation utilities                         //
/*************************************************/
///////////////////////////////////////////////////

Amplitude
SelfEnergy(mty::Order order, mty::Particle &particle, mty::Model &model);

Amplitude SelfEnergy(mty::Particle &particle, mty::Model &model);

csl::Expr ComputeSquaredAmplitude(mty::Model &model, Amplitude const &ampl);

std::vector<FeynmanRule> ComputeFeynmanRules(mty::Model &model);

void ContractIndices(csl::Expr &init);

void ContractIndices(std::vector<csl::Expr> &init);

void Display(std::vector<FeynmanRule> const &rules,
             std::ostream &                  out = std::cout);

void Display(std::vector<csl::Expr> const &amplitudes,
             std::ostream &                out      = std::cout,
             bool                          simplify = false);
void Display(mty::Amplitude const &amplitudes,
             std::ostream &        out      = std::cout,
             bool                  simplify = false);

void Display(WilsonSet const &wilsons, std::ostream &out = std::cout);

void Show(std::vector<FeynmanRule> const &rules);
void Show(std::vector<std::shared_ptr<wick::Graph>> const &diagrams,
          bool                                             showMomenta = true);
void Show(mty::Amplitude const &diagrams);
void Show(WilsonSet const &wilsons);

void Show(std::vector<FeynmanRule> const &rules, size_t first, size_t last);
void Show(std::vector<std::shared_ptr<wick::Graph>> const &diagrams,
          size_t                                           first,
          size_t                                           last);
void Show(mty::Amplitude const &diagrams, size_t first, size_t last);
void Show(WilsonSet const &wilsons, size_t first, size_t last);

void SaveDiagrams(std::string const &                              fileName,
                  std::vector<std::shared_ptr<wick::Graph>> const &diagams,
                  bool showMomenta = true);
void SaveDiagrams(std::string const &   fileName,
                  mty::Amplitude const &amplitude);
void SaveDiagrams(std::string const &             fileName,
                  std::vector<FeynmanRule> const &rules);

template <class T>
void Display(T const &printable, std::ostream &out = std::cout)
{
    out << printable << std::endl;
}

void DisplayAbbreviations(std::ostream &out = std::cout);
void DisplayAbbreviations(std::string const &name,
                          std::ostream &     out = std::cout);

void ExportPDF(std::string const &                              nameFiles,
               std::vector<std::shared_ptr<wick::Graph>> const &graphs);

void ExportPDF(std::string const &             nameFiles,
               std::vector<FeynmanRule> const &rules);

void ExportPNG(std::string const &                              nameFiles,
               std::vector<std::shared_ptr<wick::Graph>> const &graphs);

void ExportPNG(std::string const &             nameFiles,
               std::vector<FeynmanRule> const &rules);

///////////////////////////////////////////////////
/*************************************************/
// Model building utilities                      //
/*************************************************/
///////////////////////////////////////////////////

template <class... Args>
void AddGaugedGroup(mty::Model &model, Args &&... args)
{
    model.addGaugedGroup(std::forward<Args>(args)...);
}

template <class... Args>
void AddFlavorGroup(mty::Model &model, Args &&... args)
{
    model.addFlavorGroup(std::forward<Args>(args)...);
}

void AddParticle(mty::Model &   model,
                 mty::Particle &particle,
                 bool           initTerms = true);

void AddTerm(mty::Model &model, csl::Expr const &term, bool addCC = false);

void AddTerm(mty::Model &                  model,
             std::vector<csl::Expr> const &terms,
             bool                          addCC = false);

mty::Particle GetParticle(mty::Model const &model, std::string const &name);

csl::Expr GetMass(mty::Particle const &particle);

mty::Particle GenerateSimilarParticle(std::string const &  name,
                                      mty::Particle const &particle);

mty::Particle GetFieldStrength(Particle const &particle);

void PromoteGoldstone(mty::Model &   model,
                      mty::Particle &goldstone,
                      mty::Particle &gaugeBoson);

void PromoteGoldstone(mty::Model &model,
                      char const *goldstone,
                      char const *gaugeBoson);

csl::Expr GetCoupling(mty::Model const & model,
                      std::string const &nameCoupling);

csl::Tensor GetYukawa(mty::Model const & model,
                      std::string const &nameCoupling);

void Rename(mty::Model &       model,
            std::string const &oldName,
            std::string const &newName);

template <class... Args>
void SetMass(Particle &part, Args &&... args)
{
    part->setMass(std::forward<Args>(args)...);
}

template <class... Args>
void SetWidth(Particle &part, Args &&... args)
{
    part->setWidth(std::forward<Args>(args)...);
}

void SetSelfConjugate(Particle &part, bool selfConjugate);

void SetGroupRep(mty::Particle &         part,
                 std::string const &     nameGroup,
                 std::vector<int> const &highestWeight);
void SetGroupRep(mty::Particle &    part,
                 std::string const &nameGroup,
                 int                charge);
void SetFlavorRep(mty::Particle &part, std::string const &nameGroup);

void Init(mty::Model &model);

/*!
 * \brief Replaces an expression in all kinetic / mass / interactions terms of
 * a model. \param model  Model in which we do the replacement. \param init
 * Expression to replace. \param target New expression.
 */
void Replaced(mty::Model &     model,
              csl::Expr const &init,
              csl::Expr const &target);

/*!
 * \brief Replaces an expression in all kinetic / mass / interactions terms of
 * a model. \details The new expression should have a free index structure
 * corresponding to the tensor that is replaced. \param model  Model in which
 * we do the replacement. \param init   Tensor to replace. \param target New
 * expression.
 */
void Replaced(mty::Model &model, csl::Tensor &init, csl::Expr const &target);

/*!
 * \brief Replaces an expression in all kinetic / mass / interactions terms of
 * a model. \param model  Model in which we do the replacement. \param init
 * Tensor to replace. \param target New tensor, should have the same structure
 * as the replaced one.
 */
void Replaced(mty::Model &model, csl::Tensor &init, csl::Tensor &target);

/*!
 * \brief Replaces a particle in all kinetic / mass / interactions terms of a
 * model.
 * \param model    Model in which we do the replacement.
 * \param particle Particle to replace.
 * \param newTerm  New expression.
 * \sa The overload of this function with 4 parameters.
 */
void Replaced(mty::Model &     model,
              Particle const & particle,
              csl::Expr const &newTerm);

/*!
 * \brief Rotates a bunch of fields to another using a given matrix.
 * \details A useful feature for model building is field rotation. For example
 * when dealing with non trivial mass matrices, one could want to diagonalize
 * them by rotating fields. For example, in a theory with two real scalar
 * fields \f$ \phi _1(X) \f$ and \f$ \phi _2(X) \f$ with a lagrangian
 * containing \f[ \mathcal{L} \ni -\frac{1}{2}m^2(\phi _1^2 + 2\phi _1\phi _2 +
 * \phi _2^2)
 *     - \frac{\lambda}{3!}\phi _1^3,
 * \f]
 * one may want to replace \f$ \phi _1 \rightarrow (\phi _1+ \phi _2)/\sqrt{2}
 * \f$ massive field, and \f$ \phi _2\rightarrow (\phi _1 - \phi _2)/\sqrt{2}
 * \f$ massless field. The corresponding rotation reads
 * \f[
 *     \left(\begin{array}{c}
 *     \phi _1 \\ \phi _2
 *     \end{array}\right) =  \frac{1}{\sqrt{2}}
 *     \left(\begin{array}{c c}
 *     1 & 1 \\ 1 & -1
 *     \end{array}\right)\cdot
 *     \left(\begin{array}{c}
 *     \tilde{\phi} _1 \\ \tilde{\phi}_2
 *     \end{array}\right).
 * \f]
 * Applying that rotation in a code would look like (given all fields and the
 * model variables already exist):
 * \code
 *      Rotate(
 *              model,
 *              {phi1, phi2},
 *              {phi_1_tilde, phi_2_tilde},
 *              {{1 / csl::sqrt_s(2),  1 / csl::sqrt_s(2)},
 *               {1 / csl::sqrt_s(2), -1 / csl::sqrt_s(2)}}
 *               );
 * \endcode
 * The rotation will yield the final lagrangian (getting rid of the tildas):
 * \f[
 *      \mathcal{L} \ni -\frac{1}{2}m^2\phi _1^2
 *      - \frac{\lambda}{12\sqrt{2}}\left(\phi _1^3 + \phi _2^3\right)
 *      - \frac{\lambda}{6\sqrt{2}}\left(\phi _1\phi _2^2 + \phi _1^2\phi _2
 *      \right).
 * \f]
 * \param model Model in which we rotate particles.
 * \param fields Initial fields to rotate.
 * \param newFields New fields after the rotation.
 * \param rotation Rotation matrix.
 * \param diagonalizeMasses Boolean (optional, default = false). If true, all
 * non diagonal mass term between new fields are suppressed after the rotation.
 */
void Rotate(mty::Model &                               model,
            std::vector<mty::Particle> const &         fields,
            std::vector<mty::Particle> const &         newFields,
            std::vector<std::vector<csl::Expr>> const &rotation,
            bool diagonalizeMasses = false);

/*!
 * \brief Rotates a bunch of fields to another using a given matrix.
 * \param model Model in which we rotate particles.
 * \param fields Initial names of the fields to rotate.
 * \param newFields Names of the new fields after the rotation.
 * \param rotation Rotation matrix.
 * \param diagonalizeMasses Boolean (optional, default = false). If true, all
 * non diagonal mass term between new fields are suppressed after the rotation.
 * \sa The overload of this function taking mty::Particle as parameter instead
 * of names.
 */
void Rotate(mty::Model &                               model,
            std::vector<std::string> const &           fields,
            std::vector<std::string> const &           newFields,
            std::vector<std::vector<csl::Expr>> const &rotation,
            bool diagonalizeMasses = false);

void DiagonalizeMassMatrices(mty::Model &model);

void BreakGaugeSymmetry(mty::Model &                 model,
                        std::string const &          brokenGroup,
                        std::vector<Particle> const &brokenFields,
                        std::vector<std::vector<std::string>> const &newNames);

void BreakGaugeSymmetry(mty::Model &                              model,
                        std::string const &                       brokenGroup,
                        std::initializer_list<std::string> const &brokenFields,
                        std::vector<std::vector<std::string>> const &newNames);

void BreakFlavorSymmetry(
    mty::Model &                                 model,
    std::string const &                          brokenFlavor,
    std::initializer_list<std::string> const &   brokenFields,
    std::vector<std::vector<std::string>> const &newNames);

void BreakFlavorSymmetry(
    mty::Model &                                 model,
    std::string const &                          brokenFlavor,
    std::vector<mty::Particle> const &           brokenFields,
    std::vector<std::vector<std::string>> const &newNames);

template <class... Args>
void BreakGaugeSymmetry(mty::Model &model, Args &&... args)
{
    model.breakGaugeSymmetry(std::forward<Args>(args)...);
}

template <class... Args>
void BreakFlavorSymmetry(mty::Model &model, Args &&... args)
{
    model.breakFlavorSymmetry(std::forward<Args>(args)...);
}

void Refreshed(mty::Model &model);

void IntegrateOutParticle(mty::Particle &particle, bool value = true);

///////////////////////////////////////////////////
/*************************************************/
// Group theory                                  //
/*************************************************/
///////////////////////////////////////////////////

/*!
 * \brief Creates a specilization of SemiSimpleAlgebra of type \b type.
 * \details For a non exceptional algebra, i.e. different of \f$ E_6 \f$,
 * \f$ E_7 \f$, \f$ E_8 \f$, \f$ F_4 \f$, or \f$ G_2 \f$; the rank \b l of the
 * algebra must be given to specify the dimension: \f$ A_l \f$ for
 * \f$ SU(l+1) \f$, \f$ B_l \f$, \f$ C_l \f$, \f$ D_l \f$.
 * For example, \f$ A_{N-1} \f$ being the algebra of \f$ SU(N) \f$, the code
 * allowing to get \f$ SU(5) \f$ is:
 * \code
 *      // std::unique_ptr<mty::SemiSimpleAlgebra> may be replaced by
 *      // the auto keyword here.
 *
 *      std::unique_ptr<mty::SemiSimpleAlgebra> SU_5
 *              = createAlgebra(algebra::Type::A, 4);
 *
 *      // Here SU_5 is a non-copyable pointer type.
 *      SU_5;       // is a std::unique_ptr<SemiSimpleAlgebra>: use ->
 *      SU_5.get(); // is a raw pointer SemiSimpleAlgebra*    : use ->
 *      *SU_5;      // is a SemiSimpleAlgebra                 : use .
 *
 * \endcode
 * \note A std::unique_ptr is not copyable. But the raw pointer inside
 * may be copied. The raw pointer is accessible by u_ptr.get(). See the manual
 * of CSL for more information.
 * \param type Element of the enum algebra::Type, specifying the type
 * of the algebra to create.
 * \param l Integer, rank of the algebra for non exceptional algebras.
 * \return A std::unique_ptr to the SemiSimpleAlgebra.
 */
std::unique_ptr<mty::SemiSimpleAlgebra> CreateAlgebra(algebra::Type type,
                                                      int           l = -1);

mty::Irrep GetIrrep(mty::SemiSimpleAlgebra const *algebra,
                    std::vector<int> const &      labels);

mty::Irrep GetIrrep(std::unique_ptr<mty::SemiSimpleAlgebra> const &algebra,
                    std::vector<int> const &                       labels);

mty::Irrep GetConjugateRep(mty::Irrep const &rep);

mty::SemiSimpleAlgebra const *GetAlgebraOf(Irrep const &rep);

int GetDimension(mty::Irrep const &irrep);

bool ContainsIrrep(mty::SumIrrep const &decomposition,
                   mty::Irrep const &   irrep);

bool ContainsTrivialrep(mty::SumIrrep const &decomposition);

/*!
 * \brief This function changes the gauge fixing parameter for the gauge
 * boson of name \b nameGroup. Depending on this choice, the propagator of
 * gauge bosons is modified. \details The propagator of a gauge boson is \f[
 * -i\frac{g_{\mu\nu} - (1-\xi)\frac{p_\mu p_\nu}{p^2 - \xi M^2}}{p^2-M^2}
 * \f], with \f$ M \f$ the mass of the
 * particle. The Lorentz gauge corresponds to \f$ \xi=0\f$, the Feynman gauge
 * to \f$ \xi=1\f$, the Unitary gauge to \f$ \xi=\infty\f$. Finally the choice
 * NoGauge let \f$ \xi\f$ free, for the so-called \f$\mathcal{R}_\xi\f$ gauge.
 * For more information see gauge::Type. \param model        Model in which the
 * particle is present. \param nameParticle The name of the GaugeBoson to
 * modify. \param choice       New gauge fixing choice, element of gauge::Type.
 */
void SetGaugeChoice(mty::Model &       model,
                    std::string const &nameParticle,
                    mty::gauge::Type   choice);

/*!
 * \brief This function changes the gauge choice in \b model for the gauge
 * boson \b particle. Depending on this choice, the propagator of gauge
 * bosons is modified.
 * \details The propagator of a gauge boson is \f[
 * -i\frac{g_{\mu\nu} - (1-\xi)\frac{p_\mu p_\nu}{p^2 - \xi M^2}}{p^2-M^2}
 * \f], with \f$ M \f$ the mass of the
 * particle. The Lorentz gauge corresponds to \f$ \xi=0\f$, the Feynman gauge
 * to \f$ \xi=1\f$, the Unitary gauge to \f$ \xi=\infty\f$. Finally the choice
 * NoGauge let \f$ \xi\f$ free, for the so-called \f$\mathcal{R}_\xi\f$ gauge.
 * For more information see gauge::Type. \param particle Particle (must be a
 * GaugeBoson) for gauge fixing. \param choice   New gauge fixing choice,
 * element of gauge::Type.
 */
void SetGaugeChoice(mty::Particle particle, mty::gauge::Type choice);

std::string FindProcessName(std::string const &                initName,
                            std::vector<mty::Insertion> const &insertions);

} // namespace mty

#endif
