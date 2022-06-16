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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef SPACE_H_INCLUDED
#define SPACE_H_INCLUDED

#include "indicial.h"
#include "literal.h"
#include "numerical.h"
#include "vector.h"

namespace csl {

template <typename T, typename G>
typename std::vector<std::pair<T, G>>::iterator
findFirstInPair(std::vector<std::pair<T, G>> &v, const T &element)
{
    typename std::vector<std::pair<T, G>>::iterator iter = v.begin();
    while (iter != v.end() and (*iter).first != element)
        ++iter;

    return iter;
}

template <typename T, typename G>
typename std::vector<std::pair<T, G>>::iterator
findSecondInPair(std::vector<std::pair<T, G>> &v, const G &element)
{
    typename std::vector<std::pair<T, G>>::iterator iter = v.begin();
    while (iter != v.end() and (*iter).second != element)
        ++iter;

    return iter;
}

/*!
 *
 * \brief Vector space that has a name, a dimension, a delta tensor and
 * possibly a non-trivial metric. If a metric is given, the indices in this
 * space are signed, i.e. their up- or down- position matter in expressions.
 */
class Space {

  public:
    mutable bool keepCycles = false;

    static bool applyMetric;

  protected:
    /*!
     * \brief Name of the vector space.
     */
    std::string name;

    /*!
     * \brief Dimension of the space.
     */
    int dim;

    /*!
     * \brief Property of indices: if true, it means that the metric is non
     * trivial (given by the user at initialization) and that up- or down-
     * position of indices matter in expressions.
     */
    bool signedIndex;

  protected:
    mutable std::map<char, std::string> nameIndices;

    mutable std::map<std::string, char> specIndices;

    mutable std::vector<std::pair<std::string, Index::ID_type>>
        availableIndices;

    mutable std::vector<std::pair<std::string, unsigned short>>
        availableVectors;

    mutable std::vector<std::string>::const_iterator defaultName;

    std::vector<std::string> defaultIndexNames;

    /*!
     * \brief Kronecker delta, public attribute for readability in usage.
     * Element i,j of the tensor is then accessible by space.delta({i,j}) with
     * i,j indices in this space.
     *
     * \details This attribute is mutable because access functions for parents
     * (operator() to get TensorElement) is not const, and all many Space
     * objects are declared const. So we declare this attribute mutable so that
     * the compiler allows it to be changed, event for Space objets declared
     * const.
     */
    mutable Tensor delta;

    /*!
     * \brief Public attribute for readability in usage. Element i,j of the
     * tensor is then accessible by space.metric({i,j}) with i,j indices in
     * this space. It corresponds to the metric of the space if it is non
     * trivial, or is redirected to the kronecker delta else.
     *
     * \details This attribute is mutable because access functions for parents
     * (operator() to get TensorElement) is not const, and all many Space
     * objects are declared const. So we declare this attribute mutable so that
     * the compiler allows it to be changed, event for Space objets declared
     * const.
     */
    mutable Tensor metric;

    mutable Tensor inverseMetric;

    mutable Tensor epsilon;

  public:
    /*!
     * \brief Initializes the name and the dimension of the space, no metric
     * so only the kronecker delta is relevant, and indices are not signed.
     *
     * \param t_name Name of the Space.
     * \param t_dim Dimension of the Space.
     */
    Space(const std::string              &t_name,
          int                             t_dim,
          std::vector<std::string> const &indexNames = {"i", "j", "k", "l"});

    /*!
     * \brief Initializes name, dimension of the space. Here the metrix is
     * given by the user (also its explicit tensor, like diag (-1,1,1,1) for
     * the Minkowski Space). This constructor constructs then a Space with
     * signed indices and a non-trivial metric.
     *
     * \param t_name Name of the Space.
     * \param t_dim Dimension of the Space.
     * \param name_metric Name of the metric. Can be set to CSL_UNDEF. In this
     * case the tensor metric will not be defined (but the indexed object g_ab
     * will).
     * \param t_metric Explicit TensorElement for the metric
     * (should be a 2D square matrix, symmetric).
     */
    Space(const std::string              &t_name,
          int                             t_dim,
          const std::string              &name_metric,
          const Expr                     &t_metric,
          std::vector<std::string> const &indexNames = {"i", "j", "k", "l"});

    /*!
     * \brief Destructor.
     */
    virtual ~Space();

    virtual void printCode(std::ostream &out, int indentSize) const;

    virtual void printDefinition(std::ostream &out,
                                 int           indentSize,
                                 bool          header = false) const;

    /*!
     * \return The name of the Space.
     */
    virtual std::string getName() const;

    inline std::string getNextIndexName() const;

    inline std::string getIndexName(char spec) const;

    inline std::string_view getIndexNameView(char spec) const;

    inline char getSpecFromIndexName(std::string const &t_name) const;

    /*!
     * \return The dimension of the Space.
     */
    inline int getDim() const;

    /*!
     * \return \b True if the indices in this Space are signed.
     * \return \b False else.
     */
    inline bool getSignedIndex() const;

    virtual bool
    hasSpecialTraceProperty(const csl::vector_expr &tensors) const;

    csl::vector_expr getSignature() const;

    Tensor getMetric() const;

    Tensor getInverseMetric() const;

    Tensor getDelta() const;

    Tensor getEpsilon() const;

    Expr
    applyMetricOnTensor(Expr const &tensor, size_t axis, bool covariant) const;

    /*!
     * \brief Generates and returns an index in the right Space (itself) of
     * name \b name.
     *
     * \param name Name of the Index.
     *
     * \return An Index of name \b name living in the Space *this.
     */
    inline Index generateIndex(const std::string &name) const;

    /*!
     * \brief Generates and returns an index in the right Space (itself) of
     * name defined by the program. For now the index is name i_{number}, but
     * in the future one should be able to defined a set of names for a given
     * space, for example {"mu", "nu", "rhi", "sigma"}...
     *
     * \return An Index of name i_{number} living in the Space *this.
     */
    inline Index generateIndex() const;

    inline std::vector<Index> generateIndices(size_t N) const;
    inline std::vector<Index> generateIndices(size_t             N,
                                              const std::string &name) const;

    inline Index generateSimilar(const Index &model) const;

    inline void resetIndexNumber() const;

    inline void refreshIndexName(Index &index) const;

    /*!
     * \brief Generates and returns a Vector in the right Space (itself) of
     * name \b name, filled with Variable objects as components (X_0, X_1, ...)
     * for a \b name "X".
     *
     * \param name Name of the Vector.
     *
     * \return A Vector of Variables living in the Space *this.
     */
    Expr generateVector(const std::string &t_name) const;

    std::string generateVectorName(const std::string &t_name) const;

    void addIndexNames(const std::string &name) const;

    void addVectorNames(const std::string &name) const;

    void addIndexNames(const std::vector<std::string> &names) const;

    void addVectorNames(const std::vector<std::string> &names) const;

    virtual Expr calculateTrace(csl::vector_expr tensors) const;

  private:
    void buildEpsilon();

    inline Index::ID_type getID(std::string_view name) const;

    std::string getProperVectorName(const std::string &initialName) const;
};

std::string Space::getIndexName(char spec) const
{
    return nameIndices[spec];
}

std::string_view Space::getIndexNameView(char spec) const
{
    return nameIndices[spec];
}

char Space::getSpecFromIndexName(std::string const &t_name) const
{
    if (specIndices.find(t_name) == specIndices.end()) {
        constexpr char maxChar = std::numeric_limits<char>::max();
        char           c       = 1;
        while (true) {
            if (nameIndices.find(c) == nameIndices.end()) {
                nameIndices[c]      = t_name;
                specIndices[t_name] = c;
                return c;
            }
            if (c == maxChar)
                break;
            ++c;
        }
    }
    return specIndices[t_name];
}

int Space::getDim() const
{
    return dim;
}

bool Space::getSignedIndex() const
{
    return signedIndex;
}

void Space::resetIndexNumber() const
{
    availableIndices.clear();
    defaultName = defaultIndexNames.begin();
}

void Space::refreshIndexName(Index &index) const
{
    index.setID(getID(index.getName()));
}

Index::ID_type Space::getID(std::string_view t_name) const
{
    for (size_t i = 0; i != availableIndices.size(); ++i)
        if (availableIndices[i].first == t_name) {
            return ++availableIndices[i].second;
        }
    availableIndices.push_back(make_pair(std::string(t_name), 0));
    return 0;
}

Index Space::generateSimilar(const Index &model) const
{
    Index newIndex = model;
    refreshIndexName(newIndex);
    return newIndex;
}

Index Space::generateIndex(const std::string &t_name) const
{
    return Index(t_name, this, getID(t_name));
}

std::string Space::getNextIndexName() const
{
    auto iter = defaultName;
    ++defaultName;
    if (defaultName == defaultIndexNames.end())
        defaultName = defaultIndexNames.begin();
    return *iter;
}

Index Space::generateIndex() const
{
    return generateIndex(getNextIndexName());
}

std::vector<Index> Space::generateIndices(size_t N) const
{
    std::vector<Index> indices(N);
    for (size_t i = 0; i != N; ++i)
        indices[i] = generateIndex();

    return indices;
}

std::vector<Index> Space::generateIndices(size_t             N,
                                          const std::string &name) const
{
    std::vector<Index> indices(N);
    for (size_t i = 0; i != N; ++i)
        indices[i] = generateIndex(name);

    return indices;
}

inline Expr DMinko = csl::constant_s("D", csl::int_s(4));

inline const Space &buildMinkowski()
{
    static const Space Minkowski(
        "Minko",
        4,
        "g",
        matrix_s({{CSL_1, CSL_0, CSL_0, CSL_0},
                  {CSL_0, CSL_M_1, CSL_0, CSL_0},
                  {CSL_0, CSL_0, CSL_M_1, CSL_0},
                  {CSL_0, CSL_0, CSL_0, CSL_M_1}}),
        {"\\mu", "\\nu", "\\rho", "\\sigma", "\\lambda", "\\tau"});
    Minkowski.getDelta()->setTrace(DMinko);
    Minkowski.getMetric()->setTrace(DMinko);
    return Minkowski;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////

void fillEpsilonTensor(Expr &tensor, int dim);

/*!
 * \brief Space \f$ R^2 \f$.
 */
inline const Space Euclid_R2("R2", 2);

/*!
 * \brief Space \f$ R^3 \f$.
 */
inline const Space Euclid_R3("R3", 3);

/*!
 * \brief Space \f$ R^4 \f$.
 */
inline const Space Euclid_R4("R4", 4);

/*!
 * \brief Space \f$ R^{1,3} \f$ with a metric g = diag(-1,1,1,1).
 */
inline const Space &Minkowski = buildMinkowski();

} // End of namespace csl

#endif
