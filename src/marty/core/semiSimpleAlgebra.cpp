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

#include "semiSimpleAlgebra.h"
#include "groupIndices.h"
#include "mrtError.h"
#include "representation.h"

using namespace std;
using namespace csl;

namespace mty {

int getCoxeterNumber(mty::algebra::Type type, int l)
{
    switch (type) {
    case mty::algebra::Type::A:
        return l + 1;
    case mty::algebra::Type::B:
        return 2 * l - 1;
    case mty::algebra::Type::C:
        return l + 1;
    case mty::algebra::Type::D:
        return 2 * l - 2;
    case mty::algebra::Type::E6:
        return 12;
    case mty::algebra::Type::E7:
        return 18;
    case mty::algebra::Type::E8:
        return 30;
    case mty::algebra::Type::F4:
        return 9;
    case mty::algebra::Type::G2:
        return 4;
    default:
        return 0;
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class SemiSimpleAlgebra                       //
/*************************************************/
///////////////////////////////////////////////////

SemiSimpleAlgebra::SemiSimpleAlgebra(int t_l)
    : l(t_l),
      rho(std::vector<int>(l, 0)),
      squaredNorm(std::vector<int>(l, 0)),
      positiveRoots(std::vector<std::vector<int>>(l, std::vector<int>(l, 0))),
      cartanMatrix(std::vector<std::vector<int>>(l, std::vector<int>(l))),
      rootMetric(std::vector<std::vector<int>>(l, std::vector<int>(l))),
      symbolicCartan(matrix_s(l)),
      inverseCartan(matrix_s(l)),
      symbolicMetric(matrix_s(l)),
      symbolicRoots(csl::vector_expr(l)),
      symbolicSquaredNorms(csl::vector_expr(l))
{
    HEPAssert(t_l >= 0,
              mty::error::ValueError,
              "Wrong initialization of semi-simple algebra.");
    for (int i = 0; i != l; ++i)
        positiveRoots[i][i] = 1; // simple roots
}

csl::Expr
SemiSimpleAlgebra::getQuadraticCasimir(const AlgebraState &highest) const
{
    // C_R = eta / 2 * (a_i + 2) * G_ij * a_j
    // eta == 2, a_i dinkin labels
    vector<int> labels = highest.getLabels();

    csl::Expr exprLabels = vector_s(labels.size());
    csl::Expr norms      = vector_s(labels.size());
    for (size_t i = 0; i != labels.size(); ++i) {
        exprLabels->setArgument(int_s(labels[i]), i);
        norms->setArgument(absoluteNorms[i], i);
    }
    csl::Expr C2
        = getEta() / 2
          * (norms * (exprLabels + 2))->dot(inverseCartan->dot(exprLabels));
    return C2;
}

csl::Expr SemiSimpleAlgebra::getIndex(const Irrep &irrep, size_t p) const
{
    for (const auto &index : indices)
        if (index.state == irrep.getHighestWeight() and index.p == p)
            return index.index;
    Irrep            defining = getDefiningRep();
    std::vector<int> labels   = irrep.getHighestWeight().getLabels();
    if (p == 2)
        return mty::getIndex(getType(), labels, p)
               * mty::getIndexNorm(getType(), labels, p);
    if (irrep == defining)
        return mty::getIndexNorm(getType(), labels, p);
    csl::Expr index_value = mty::getIndex(getType(), labels, p);
    csl::Expr norm        = mty::getIndexNorm(getType(), labels, p);
    index_value *= norm
                   / mty::getIndex(
                       getType(), defining.getHighestWeight().getLabels(), p);
    indices.push_back({irrep.getHighestWeight(), p, index_value});
    return index_value;
}

int SemiSimpleAlgebra::getOrderL() const
{
    return l;
}

void SemiSimpleAlgebra::init()
{
    setCartanMatrix();
    setSquaredNorm();
    for (int i = 0; i != l; ++i)
        for (int j = 0; j != l; ++j)
            // Missing factor 2 to avoid half_integer
            rootMetric[i][j] = squaredNorm[j] * cartanMatrix[i][j];
    absoluteNorms = csl::vector_expr(l);
    generateWeylGroup();
    invertCartanMatrix();

    for (int i = 0; i != l; ++i) {
        absoluteNorms[i] = CSL_0;
        for (size_t j = 0; j != positiveRoots.size(); ++j) {
            csl::Expr intermediate = CSL_0;
            for (int k = 0; k != l; ++k) {
                intermediate
                    = intermediate + +positiveRoots[j][k] * cartanMatrix[i][k];
            }
            absoluteNorms[i] = absoluteNorms[i] + intermediate * intermediate;
        }
        absoluteNorms[i] = 2 / absoluteNorms[i];
    }
}

bool SemiSimpleAlgebra::isPositiveRoot(const vector<int> &root)
{
    for (const auto &coef : root)
        if (coef < 0)
            return false;
    return true;
}

bool SemiSimpleAlgebra::isPositiveWeight(const AlgebraState &state)
{
    const vector<int> &labels = state.getLabels();
    for (const auto &l : labels)
        if (l < 0)
            return false;
    return true;
}

AlgebraState SemiSimpleAlgebra::addWeights(const AlgebraState &A,
                                           const AlgebraState &B)
{
    return AlgebraState(A.getLabels() + B.getLabels());
}

bool SemiSimpleAlgebra::isOnWall(const AlgebraState &state) const
{
    const vector<int> &labels = state.getLabels();
    for (const auto &l : labels)
        if (l == 0)
            return true;
    return false;
}

AlgebraState
SemiSimpleAlgebra::toDominantWeylChamber(const AlgebraState &state,
                                         int                &sign) const
{
    sign = 1;
    if (isPositiveWeight(state))
        // Already in the dominant weyl chamber
        return state;
    vector<AlgebraState> states(1, state);
    while (true) {
        // We generate new states, weyl reflection of those we already
        // have. When we found the one in the dominant Weyl chamber we return
        // it, keeping track of the number of reflections: sign = +1 or -1.
        vector<AlgebraState> newStates(states);
        sign *= -1;
        for (int i = 0; i != l; ++i)
            for (const auto &s : states) {
                AlgebraState s2 = weylReflection(i, s);
                if (isPositiveWeight(s2))
                    return s2;
                if (findInVector(newStates, s2) == -1)
                    newStates.push_back(s2);
            }
        states = newStates;
    }
}

AlgebraState SemiSimpleAlgebra::weylReflection(int                 simpleRoot,
                                               const AlgebraState &state) const
{
    csl::Expr converted = convertState(state);
    csl::Expr reflected = Copy(converted);
    for (int i = 0; i != converted->getNArgs(); ++i)
        reflected->setArgument(reflected->getArgument(simpleRoot)
                                   - converted->getArgument(i)
                                         * cartanMatrix[i][simpleRoot],
                               simpleRoot);
    reflected = inverseCartan->transpose()->inverseMatrix()->dot(reflected);
    vector<int> newLabels(0);
    for (const auto &value : *reflected)
        newLabels.push_back(value->evaluateScalar());
    return AlgebraState(newLabels);
}

vector<int> SemiSimpleAlgebra::weylReflection(int                simpleRoot,
                                              const vector<int> &root) const
{
    vector<int> reflectedRoot(root);
    for (int i = 0; i != l; ++i)
        reflectedRoot[simpleRoot] -= root[i] * cartanMatrix[i][simpleRoot];

    return reflectedRoot;
}

vector<int> SemiSimpleAlgebra::weylReflection(int reflector,
                                              int reflected) const
{
    vector<int> reflectedRoot(l);
    reflectedRoot[reflected] = 1;
    reflectedRoot[reflector] = -cartanMatrix[reflected][reflector];

    return reflectedRoot;
}

void SemiSimpleAlgebra::invertCartanMatrix()
{
    // Inverting Cartan matrix
    for (int i = 0; i != l; ++i)
        for (int j = 0; j != l; ++j)
            symbolicCartan[i][j] = cartanMatrix[i][j];
    inverseCartan = Expanded(symbolicCartan->inverseMatrix());

    // Setting the symbolicMetric
    for (int i = 0; i != l; ++i)
        for (int j = 0; j != l; ++j)
            symbolicMetric->setArgument(int_s(rootMetric[i][j]) / 2, {i, j});
    symbolicRoots = csl::vector_expr(positiveRoots.size());

    // Setting the symbolicRoots
    symbolicSquaredNorms = csl::vector_expr(positiveRoots.size());
    for (size_t i = 0; i != positiveRoots.size(); ++i) {
        symbolicRoots[i] = vector_s(l);
        for (int j = 0; j != l; ++j)
            symbolicRoots[i]->setArgument(int_s(positiveRoots[i][j]), j);
        symbolicSquaredNorms[i]
            = symbolicRoots[i]->dot(symbolicMetric->dot(symbolicRoots[i]));
    }

    // Setting the symbolicRho
    symbolicRho = symbolicRoots[0];
    for (size_t i = 1; i != symbolicRoots.size(); ++i)
        symbolicRho = symbolicRho + symbolicRoots[i];
    symbolicRho = Expanded(symbolicRho / 2);
}

void SemiSimpleAlgebra::generateWeylGroup()
{
    vector<vector<int>> newRoots(0);

    // Getting all initial roots reflection of a simple root onto another.
    for (int i = 0; i < l; ++i)
        for (int j = 0; j != l; ++j)
            if (i != j) {
                vector<int> r = weylReflection(i, j);
                if (isPositiveRoot(r)
                    and findInVector(positiveRoots, r) == -1) {
                    newRoots.push_back(r);
                    positiveRoots.push_back(r);
                    squaredNorm.push_back(r * rootMetric * r / 2);
                }
            }

    // Iterating while we find new roots, taking reflection of all newRoots
    // onto simple roots.
    while (newRoots.size() > 0) {
        vector<vector<int>> newRootsBis(0);
        for (const auto &root : newRoots)
            for (int i = 0; i != l; ++i) {
                vector<int> r = weylReflection(i, root);
                if (isPositiveRoot(r)
                    and findInVector(positiveRoots, r) == -1) {
                    newRootsBis.push_back(r);
                    positiveRoots.push_back(r);
                    squaredNorm.push_back(r * rootMetric * r / 2);
                }
            }
        newRoots = newRootsBis;
    }

    // Setting rho once we found all simple roots.
    rho = vector<int>(l, 0);
    for (const auto &pRoot : positiveRoots)
        rho = rho + pRoot;
}

csl::Expr SemiSimpleAlgebra::convertState(const AlgebraState &state) const
{
    vector<int> labels = state.getLabels();
    csl::Expr   rep    = vector_s(l);
    for (int i = 0; i != l; ++i)
        rep->setArgument(int_s(labels[i]), i);

    return Expanded(inverseCartan->transpose()->dot(rep));
}

bool SemiSimpleAlgebra::comparatorHighest(const AlgebraState &A,
                                          const AlgebraState &B) const
{
    csl::Expr EA = convertState(A);
    csl::Expr EB = convertState(B);

    return EA->getSum() >= EB->getSum();
}

csl::Expr SemiSimpleAlgebra::computeScalarDot(const csl::Expr &rootA,
                                              const csl::Expr &rootB) const
{
    return rootA->dot(symbolicMetric->dot(rootB));
}

csl::Expr SemiSimpleAlgebra::computeSquaredNorm(const csl::Expr &root) const
{
    return root->dot(symbolicMetric->dot(root));
}

void SemiSimpleAlgebra::sortRep(vector<AlgebraState> &rep,
                                vector<int>          &mult) const
{
    for (size_t i = 0; i != rep.size() - 1; ++i) {
        size_t iMax = i;
        for (size_t j = i + 1; j != rep.size(); ++j)
            if (comparatorHighest(rep[j], rep[i])) // State j >= State i
                iMax = j;
        if (iMax != i) {
            swap(rep[i], rep[iMax]);
            swap(mult[i], mult[iMax]);
        }
    }
}

AlgebraState
SemiSimpleAlgebra::applyAnnihilationOperator(int                 annihilator,
                                             const AlgebraState &state) const
{
    vector<int> shifts(l);
    for (int i = 0; i != l; ++i)
        shifts[i] = -cartanMatrix[annihilator][i];

    return state.applyAnnihilator(shifts);
}

AlgebraState
SemiSimpleAlgebra::applyAnnihilationOperator(const vector<int>  &root,
                                             const AlgebraState &state) const
{
    AlgebraState newState(state);
    for (size_t i = 0; i != root.size(); ++i) {
        if (root[i] < 0)
            continue;
        for (int j = 0; j != root[i]; ++j)
            newState = applyAnnihilationOperator(i, newState);
    }

    return newState;
}

AlgebraState
SemiSimpleAlgebra::applyCreationOperator(int                 creator,
                                         const AlgebraState &state) const
{
    vector<int> shifts(l);
    for (int i = 0; i != l; ++i)
        shifts[i] = +cartanMatrix[creator][i];

    return state.applyAnnihilator(shifts);
}

AlgebraState
SemiSimpleAlgebra::applyCreationOperator(const vector<int>  &root,
                                         const AlgebraState &state) const
{
    AlgebraState newState(state);
    for (size_t i = 0; i != root.size(); ++i) {
        if (root[i] < 0)
            continue;
        for (int j = 0; j != root[i]; ++j)
            newState = applyCreationOperator(i, newState);
    }

    return newState;
}

std::vector<AlgebraState>
SemiSimpleAlgebra::getSingleChain(const AlgebraState &state,
                                  int                 direction) const

{
    int eigenValue = state.eigenValue(
        positiveRoots[direction], squaredNorm, squaredNorm[direction]);
    vector<AlgebraState> newStates;
    if (eigenValue > 0)
        newStates.reserve(eigenValue);
    if (eigenValue > 0) {
        AlgebraState foo = state;
        for (int i = 0; i != eigenValue; ++i) {
            foo = applyAnnihilationOperator(positiveRoots[direction], foo);
            newStates.push_back(foo);
        }
    }
    return newStates;
}

void SemiSimpleAlgebra::getSingleChainExperimental(
    const AlgebraState             &state,
    std::vector<AlgebraState>      &states,
    std::vector<std::vector<bool>> &directionExplored,
    int                             direction) const

{
    int eigenValue = state.eigenValue(
        positiveRoots[direction], squaredNorm, squaredNorm[direction]);
    if (eigenValue > 0) {
        AlgebraState foo = state;
        for (int i = 0; i != eigenValue; ++i) {
            foo = applyAnnihilationOperator(positiveRoots[direction], foo);
            if (findInVector(states, foo) == -1) {
                states.push_back(foo);
                directionExplored.push_back(
                    vector<bool>(positiveRoots.size(), false));
                directionExplored[states.size() - 1][direction] = true;
            }
        }
    }
}

std::vector<AlgebraState>
SemiSimpleAlgebra::getRootChain(const AlgebraState  &heighestWeightState,
                                vector<AlgebraState> states,
                                vector<vector<bool>> directionExplored) const
{
    if (states.size() == 0)
        // If empty it is the first call of the function
        // -> we initialize
        states.push_back(heighestWeightState);
    if (directionExplored.size() == 0)
        // Same
        directionExplored.push_back(vector<bool>(positiveRoots.size(), false));

    vector<AlgebraState> newStates(1, heighestWeightState);
    vector<int>          normPositiveRoots(0);
    while (newStates.size() > 0) {
        // We only get RootChain for new states.
        vector<AlgebraState> newStatesBis;
        newStatesBis.reserve(std::max(size_t(10), 10 * newStates.size()));
        for (const auto &s : newStates) {
            // For all the states
            for (size_t iRoot = 0; iRoot != positiveRoots.size(); ++iRoot) {
                // and for all directions (positive roots)
                int iState = findInVector(states, s);
                // If we already checked that line we pass to the next
                // direction
                if (directionExplored[iState][iRoot])
                    continue;
                // else we say that we have explored that line and continue
                directionExplored[iState][iRoot] = true;

                // Get a single chain root along the root of index iRoot
                // (sl(2,C) module)
                vector<AlgebraState> foo = getSingleChain(s, iRoot);
                for (const auto &s2 : foo) {
                    // If the state is new we add it with its properties
                    if (findInVector(states, s2) == -1) {
                        newStatesBis.push_back(s2);
                        states.push_back(s2);
                        directionExplored.push_back(
                            vector<bool>(positiveRoots.size(), false));
                        directionExplored[states.size() - 1][iRoot] = true;
                    }
                }
            }
        }
        newStates.clear();
        for (const auto &s : newStatesBis) {
            // For each new state we calculate recursively the representation,
            // telling which directions have been explored
            vector<AlgebraState> recursiveChain
                = getRootChain(s, states, directionExplored);
            for (const auto &s2 : recursiveChain) {
                if (findInVector(states, s2) == -1) {
                    newStates.push_back(s2);
                    states.push_back(s2);
                    directionExplored.push_back(
                        vector<bool>(positiveRoots.size(), true));
                }
            }
        }
        newStates.insert(
            newStates.end(), newStatesBis.begin(), newStatesBis.end());
    }

    return states;
}

void SemiSimpleAlgebra::getRootChainExperimental(
    const AlgebraState   &heighestWeightState,
    vector<AlgebraState> &states,
    vector<vector<bool>> &directionExplored) const
{
    if (states.size() == 0) {
        // If empty it is the first call of the function
        // -> we initialize
        states.push_back(heighestWeightState);
    }
    if (directionExplored.size() == 0)
        // Same
        directionExplored.push_back(vector<bool>(positiveRoots.size(), false));

    vector<AlgebraState> newStates(1, heighestWeightState);
    size_t               nNewStates = 1;
    while (nNewStates > 0) {
        // We only get RootChain for new states.
        size_t nStates_beg = states.size();
        for (const auto &s : newStates) {
            // For all the states
            for (size_t iRoot = 0; iRoot != positiveRoots.size(); ++iRoot) {
                // and for all directions (positive roots)
                int iState = findInVector(states, s);
                // If we already checked that line we pass to the next
                // direction
                if (directionExplored[iState][iRoot])
                    continue;
                // else we say that we have explored that line and continue
                directionExplored[iState][iRoot] = true;

                // Get a single chain root along the root of index iRoot
                // (sl(2,C) module)
                getSingleChainExperimental(
                    s, states, directionExplored, iRoot);
            }
        }
        size_t nStates_end = states.size();
        for (size_t i = nStates_beg; i != nStates_end; ++i)
            getRootChainExperimental(states[i], states, directionExplored);
        nNewStates = states.size() - nStates_beg;
    }
}

void SemiSimpleAlgebra::computeMultiplicity(
    std::vector<int>                &multiplicity,
    const std::vector<AlgebraState> &rep,
    const AlgebraState              &state) const
{
    if (rep[0] == state)
        return;
    csl::Expr mu                 = convertState(rep[0]);
    csl::Expr mu_rho             = symbolicRho + mu;
    csl::Expr mu_rho_squared     = computeSquaredNorm(mu_rho);
    csl::Expr lambda             = convertState(state);
    csl::Expr lambda_rho_squared = computeSquaredNorm(lambda + symbolicRho);
    std::vector<csl::Expr> mulTerms;
    for (size_t i = 0; i != positiveRoots.size(); ++i) {
        AlgebraState fooState(state);
        while (true) {
            fooState = applyCreationOperator(positiveRoots[i], fooState);
            int posState;
            if ((posState = findInVector(rep, fooState)) == -1)
                // If the state is not in the representation we break
                break;
            if (multiplicity[posState] == -1)
                // If the needed multiplicity has not already been computed,
                // we compute it.
                computeMultiplicity(multiplicity, rep, rep[posState]);
            // Applying multiplicity formula
            mulTerms.push_back(
                int_s(multiplicity[posState])
                * computeScalarDot(convertState(fooState), symbolicRoots[i]));
        }
    }
    csl::Expr mul = csl::sum_s(mulTerms);
    int       pos;
    HEPAssert((pos = findInVector(rep, state)) != -1,
              mty::error::ValueError,
              "In multiplicity calculation found a state out of the rep.");
    if (mul == CSL_0)
        multiplicity[pos] = 1;
    else
        multiplicity[pos]
            = (int) (mul * 2 / (mu_rho_squared - lambda_rho_squared))
                  ->evaluateScalar();
}

constexpr bool useExperimental = true;

std::vector<int>
SemiSimpleAlgebra::multiplicities(std::vector<AlgebraState> &rep) const
{
    vector<int> mult(rep.size(), -1);
    if (rep.size() == 0)
        return mult;
    if (rep.size() == 1)
        return vector<int>(1, 1);
    // highest weight has multiplicity 1
    mult[0] = 1;
    for (size_t i = 1; i != rep.size(); ++i)
        if (mult[i] == -1) {
            computeMultiplicity(mult, rep, rep[i]);
        }
    return mult;
}

mty::Irrep
SemiSimpleAlgebra::highestWeightRep(const AlgebraState &highestWeight,
                                    bool                computeMul) const
{
    for (int label : highestWeight) {
        HEPAssert(label >= 0,
                  mty::error::ValueError,
                  "Cannot have a highest weight with negative label !")
    }
    for (const auto &[highest, rep] : irreps) {
        if (highest == highestWeight) {
            return rep;
        }
    }
    Irrep res_rep;
    if constexpr (useExperimental) {
        std::vector<AlgebraState>      state;
        std::vector<std::vector<bool>> dep;
        state.reserve(1000);
        dep.reserve(1000);
        getRootChainExperimental(highestWeight, state, dep);
        if (computeMul)
            res_rep = mty::Irrep(this, state, multiplicities(state));
        else
            res_rep
                = mty::Irrep(this, state, std::vector<int>(state.size(), 1));
    }
    else {
        vector<AlgebraState> rep = getRootChain(highestWeight);
        if (computeMul)
            res_rep = mty::Irrep(this, rep, multiplicities(rep));
        else
            res_rep = mty::Irrep(this, rep, std::vector<int>(rep.size(), 1));
    }
    irreps.push_back({highestWeight, res_rep});
    return res_rep;
}
mty::Irrep SemiSimpleAlgebra::highestWeightRep(const std::vector<int> &weight,
                                               bool computeMul) const
{
    if (weight.end() == weight.begin())
        return getTrivialRep();
    return highestWeightRep(AlgebraState(weight), computeMul);
}

mty::Irrep SemiSimpleAlgebra::getTrivialRep() const
{
    return highestWeightRep(vector<int>(l, 0));
}

mty::Irrep SemiSimpleAlgebra::getDefiningRep() const
{
    CallHEPError(mty::error::NotImplementedError, "");
    return Irrep();
}

mty::SumIrrep SemiSimpleAlgebra::tensorProduct(const mty::SumIrrep &A,
                                               const mty::Irrep    &B,
                                               bool computeMul) const
{
    SumIrrep res;
    for (const auto &r : A)
        res = res + tensorProduct(r, B, computeMul);
    return res;
}

mty::SumIrrep SemiSimpleAlgebra::tensorProduct(const mty::Irrep    &A,
                                               const mty::SumIrrep &B,
                                               bool computeMul) const
{
    return tensorProduct(B, A, computeMul);
}

mty::SumIrrep SemiSimpleAlgebra::tensorProduct(const mty::SumIrrep &A,
                                               const mty::SumIrrep &B,
                                               bool computeMul) const
{
    SumIrrep res;
    for (const auto &r1 : A)
        for (const auto &r2 : B)
            res = res + tensorProduct(r1, r2, computeMul);
    return res;
}

mty::SumIrrep SemiSimpleAlgebra::tensorProduct(const mty::Irrep &A,
                                               const mty::Irrep &B,
                                               bool computeMul) const
{
    vector<AlgebraState>        highestWeights(0);
    vector<int>                 multiplicity(0);
    AlgebraState                rhoWeight(vector<int>(l, 1));
    const vector<AlgebraState> &lambda1            = A.getRep();
    const vector<int>          &multiplicitiesRep1 = A.getMult();
    const AlgebraState         &Lambda2            = B.getHighestWeight();

    // We determine all different highest weights of the irreducible reps
    // (in the decomposition as a sum of irreducible reps) and their
    // multiplicities.
    for (size_t i = 0; i != lambda1.size(); ++i) {
        AlgebraState root
            = addWeights(addWeights(lambda1[i], Lambda2), rhoWeight);
        if (isOnWall(root)) {
            continue;
        }
        int sign;
        root          = toDominantWeylChamber(root, sign);
        root          = AlgebraState(root.getLabels() - rhoWeight.getLabels());
        bool negative = false;
        for (int label : root)
            if (label < 0) {
                negative = true;
                continue;
            }
        if (negative)
            continue;
        int pos;
        if ((pos = findInVector(highestWeights, root)) != -1)
            multiplicity[pos] += sign * multiplicitiesRep1[i];
        else {
            highestWeights.push_back(root);
            multiplicity.push_back(sign * multiplicitiesRep1[i]);
        }
    }

    // For each highest weight in the decomposition we calculate the
    // corresponding mty::Irrep and add it as many times as the multiplicity of
    // the state.
    vector<mty::Irrep> directSum(0);
    for (size_t i = 0; i != highestWeights.size(); ++i) {
        if (multiplicity[i] <= 0)
            continue;
        mty::Irrep newRep = highestWeightRep(highestWeights[i], computeMul);
        for (int n = 0; n != multiplicity[i]; ++n)
            directSum.push_back(newRep);
    }

    // Sorting the irreps in the sum by increasing dimensions.
    sort(directSum.begin(),
         directSum.end(),
         [](const mty::Irrep &e1, const mty::Irrep &e2) {
             return e1.getDim() < e2.getDim();
         });

    return mty::SumIrrep(directSum);
}

std::ostream &operator<<(std::ostream &fout, const SemiSimpleAlgebra &algebra)
{
    fout << "Rank : " << algebra.l << std::endl;
    fout << "Positive roots:\n";
    for (const auto &p : algebra.positiveRoots) {
        fout << " -> ";
        for (const auto &el : p)
            fout << el << ",";
        fout << std::endl;
    }
    fout << "Symbolic roots: ";
    for (const auto &r : algebra.symbolicRoots)
        r->print();
    fout << "rho = ";
    for (const auto &el : algebra.rho)
        fout << el << "  ";
    fout << endl;
    fout << "Symbolic rho: ";
    algebra.symbolicRho->print();
    fout << "Norms: ";
    for (const auto &n : algebra.squaredNorm)
        fout << n << "  ";
    fout << "\nMetric:\n[";
    for (const auto &row : algebra.rootMetric) {
        fout << "[ ";
        for (const auto &element : row)
            fout << element << " ";
        fout << " ]\n";
    }
    fout << "\n Symbolic metric:\n";
    algebra.symbolicMetric->print();
    fout << "Norms with metric: ";
    for (const auto &p : algebra.positiveRoots) {
        fout << p * algebra.rootMetric * p / 2 << "  ";
    }
    fout << "\nSymbolic norms:\n";
    for (const auto &n : algebra.symbolicSquaredNorms)
        n->print();
    fout << "\nCartan matrix:\n[";
    for (const auto &row : algebra.cartanMatrix) {
        fout << "[ ";
        for (const auto &element : row)
            fout << element << " ";
        fout << " ]\n";
    }
    fout << "\nInverse Cartan matrix:\n";
    algebra.inverseCartan->print();

    return fout;
}
} // End of namespace mty

using namespace mty::algebra;

///////////////////////////////////////////////////
/*************************************************/
// Class R                                       //
/*************************************************/
///////////////////////////////////////////////////

R::R() : SemiSimpleAlgebra(0)
{
}

mty::algebra::Type R::getType() const
{
    return mty::algebra::Type::R;
}

void R::setCartanMatrix()
{
}

void R::setSquaredNorm()
{
}

mty::Irrep R::highestWeightRep(const mty::AlgebraState &highestWeight,
                               bool) const
{
    vector<mty::AlgebraState> rep(1);
    // Getting the PGCD of the fractionnal charge by creating a IntFraction
    IntFraction foo(highestWeight[0], highestWeight[1]);
    rep[0] = mty::AlgebraState({(int) foo.getNum(), (int) foo.getDenom()});
    vector<int> mult(1, 1);
    return mty::Irrep(this, rep, mult);
}

mty::Irrep R::highestWeightRep(const vector<int> &highestWeight, bool) const
{
    HEPAssert(highestWeight.size() == 1 or highestWeight.size() == 2,
              mty::error::ValueError,
              "Initializing U1 rep with wrong number of \
            arguments");
    if (highestWeight.size() == 1)
        return highestWeightRep(
            mty::AlgebraState({*highestWeight.begin(), 1}));
    return highestWeightRep(mty::AlgebraState(highestWeight));
}

mty::Irrep R::getTrivialRep() const
{
    return mty::Irrep(
        this,
        vector<mty::AlgebraState>(1, mty::AlgebraState({0, 1})), // charge 0
        vector<int>(1, 1));                                      // multip. 1
}

mty::SumIrrep
R::tensorProduct(const mty::Irrep &A, const mty::Irrep &B, bool) const
{
    // First (and single) state of rep, first (and single) term of the sum
    // and first(and single) term of the product of state.
    mty::AlgebraState stateA = A[0];
    mty::AlgebraState stateB = B[0];

    csl::Expr chargeA = intfraction_s(stateA[0], stateA[1]);
    csl::Expr chargeB = intfraction_s(stateB[0], stateB[1]);

    csl::Expr charge = chargeA + chargeB;
    int       num    = (charge->getType() == csl::Type::IntFraction)
                           ? charge->getNum()
                           : charge->evaluateScalar();
    int       denom  = (charge->getType() == csl::Type::IntFraction)
                           ? charge->getDenom()
                           : 1;
    vector<mty::AlgebraState> rep(1);
    rep[0] = mty::AlgebraState({num, denom});
    vector<int> mult(1, 1);

    return mty::SumIrrep(mty::Irrep(this, rep, mult));
}

void R::sortRep(vector<mty::AlgebraState> &rep, vector<int> & /*mult*/) const
{
    // Sorting by increasing charge
    sort(rep.begin(),
         rep.end(),
         [](const mty::AlgebraState &A, const mty::AlgebraState &B) {
             return A[0] * 1. / A[1] < B[0] * 1. / B[1];
         });
}

///////////////////////////////////////////////////
/*************************************************/
// Class A                                       //
/*************************************************/
///////////////////////////////////////////////////

A::A(int t_l) : SemiSimpleAlgebra(t_l)
{
    HEPAssert(l > 0,
              mty::error::ValueError,
              "Semisimple algebra A must be initializated with l > 0");
    init();
}

mty::algebra::Type A::getType() const
{
    return mty::algebra::Type::A;
}

void A::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i + 1 != l) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
}

void A::setSquaredNorm()
{
    for (int i = 0; i != l; ++i)
        squaredNorm[i] = 1;
}

mty::Irrep A::getDefiningRep() const
{
    std::vector<int> labels(getOrderL(), 0);
    ++labels[0];
    return highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class B                                       //
/*************************************************/
///////////////////////////////////////////////////

B::B(int t_l) : SemiSimpleAlgebra(t_l)
{
    HEPAssert(l > 1,
              mty::error::ValueError,
              "Semisimple algebra B must be initializated with l > 1");
    init();
}

mty::algebra::Type B::getType() const
{
    return mty::algebra::Type::B;
}

void B::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i + 1 != l) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
    cartanMatrix[l - 2][l - 1] = -2;
}

void B::setSquaredNorm()
{
    for (int i = 0; i != l - 1; ++i)
        squaredNorm[i] = 2;
    squaredNorm[l - 1] = 1;
}

mty::Irrep B::getDefiningRep() const
{
    if (l == 2)
        return highestWeightRep({0, 1});
    std::vector<int> labels(l, 0);
    ++labels[0];
    return highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class C                                       //
/*************************************************/
///////////////////////////////////////////////////

C::C(int t_l) : SemiSimpleAlgebra(t_l)
{
    HEPAssert(l > 1,
              mty::error::ValueError,
              "Semisimple algebra C must be initializated with l > 1");
    init();
}

void C::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i + 1 != l) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
    cartanMatrix[l - 1][l - 2] = -2;
}

void C::setSquaredNorm()
{
    for (int i = 0; i != l - 1; ++i)
        squaredNorm[i] = 1;
    squaredNorm[l - 1] = 2;
}

mty::algebra::Type C::getType() const
{
    return mty::algebra::Type::C;
}

mty::Irrep C::getDefiningRep() const
{
    std::vector<int> labels(l, 0);
    ++labels[0];
    return highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class D                                       //
/*************************************************/
///////////////////////////////////////////////////

D::D(int t_l) : SemiSimpleAlgebra(t_l)
{
    HEPAssert(t_l >= 1,
              mty::error::ValueError,
              "Semisimple algebra D must be initializated with l >= 1.");
    init();
}

void D::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i + 1 != l) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
    if (l != 2) {
        cartanMatrix[l - 1][l - 3] = -1;
        cartanMatrix[l - 3][l - 1] = -1;
    }
    cartanMatrix[l - 2][l - 1] = 0;
    cartanMatrix[l - 1][l - 2] = 0;
}

mty::algebra::Type D::getType() const
{
    return mty::algebra::Type::D;
}

void D::setSquaredNorm()
{
    for (int i = 0; i != l; ++i)
        squaredNorm[i] = 1;
}

mty::Irrep D::getDefiningRep() const
{
    if (l == 3)
        return highestWeightRep({0, 0, 1});
    std::vector<int> labels(l, 0);
    ++labels[0];
    return highestWeightRep(labels);
}

///////////////////////////////////////////////////
/*************************************************/
// Class E6                                      //
/*************************************************/
///////////////////////////////////////////////////

E6::E6() : SemiSimpleAlgebra(6)
{
    init();
}

mty::algebra::Type E6::getType() const
{
    return mty::algebra::Type::E6;
}

void E6::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i == 2) {
            cartanMatrix[i][l - 1] = -1;
            cartanMatrix[l - 1][i] = -1;
        }
        if (i < l - 2) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
}

void E6::setSquaredNorm()
{
    for (int i = 0; i != l; ++i)
        squaredNorm[i] = 1;
}

mty::Irrep E6::getDefiningRep() const
{
    return highestWeightRep({1, 0, 0, 0, 0, 0});
}

///////////////////////////////////////////////////
/*************************************************/
// Class E7                                      //
/*************************************************/
///////////////////////////////////////////////////

E7::E7() : SemiSimpleAlgebra(7)
{
    init();
}

mty::algebra::Type E7::getType() const
{
    return mty::algebra::Type::E7;
}

void E7::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i == 2) {
            cartanMatrix[i][l - 1] = -1;
            cartanMatrix[l - 1][i] = -1;
        }
        if (i < l - 2) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
}

void E7::setSquaredNorm()
{
    for (int i = 0; i != l; ++i)
        squaredNorm[i] = 1;
}

mty::Irrep E7::getDefiningRep() const
{
    return highestWeightRep({0, 0, 0, 0, 0, 1, 0});
}

///////////////////////////////////////////////////
/*************************************************/
// Class E8                                      //
/*************************************************/
///////////////////////////////////////////////////

E8::E8() : SemiSimpleAlgebra(8)
{
    init();
}

mty::algebra::Type E8::getType() const
{
    return mty::algebra::Type::E8;
}

void E8::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i == 2) {
            cartanMatrix[i][l - 1] = -1;
            cartanMatrix[l - 1][i] = -1;
        }
        if (i < l - 2) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
}

void E8::setSquaredNorm()
{
    for (int i = 0; i != l; ++i)
        squaredNorm[i] = 1;
}

mty::Irrep E8::getDefiningRep() const
{
    return highestWeightRep({1, 0, 0, 0, 0, 0, 0, 0});
}

///////////////////////////////////////////////////
/*************************************************/
// Class F4                                      //
/*************************************************/
///////////////////////////////////////////////////

F4::F4() : SemiSimpleAlgebra(4)
{
    init();
}

mty::algebra::Type F4::getType() const
{
    return mty::algebra::Type::F4;
}

void F4::setCartanMatrix()
{
    for (int i = 0; i != l; ++i) {
        cartanMatrix[i][i] = 2;
        if (i + 1 != l) {
            cartanMatrix[i][i + 1] = -1;
            cartanMatrix[i + 1][i] = -1;
        }
    }
    cartanMatrix[1][2] = -2;
}

void F4::setSquaredNorm()
{
    squaredNorm[0] = 2;
    squaredNorm[1] = 2;
    squaredNorm[2] = 1;
    squaredNorm[3] = 1;
}

mty::Irrep F4::getDefiningRep() const
{
    return highestWeightRep({0, 0, 0, 1});
}

///////////////////////////////////////////////////
/*************************************************/
// Class G2                                      //
/*************************************************/
///////////////////////////////////////////////////

G2::G2() : SemiSimpleAlgebra(2)
{
    init();
}

mty::algebra::Type G2::getType() const
{
    return mty::algebra::Type::G2;
}

void G2::setCartanMatrix()
{
    cartanMatrix[0][0] = 2;
    cartanMatrix[1][0] = -3;
    cartanMatrix[0][1] = -1;
    cartanMatrix[1][1] = 2;
}

void G2::setSquaredNorm()
{
    squaredNorm[0] = 1;
    squaredNorm[1] = 3;
}

mty::Irrep G2::getDefiningRep() const
{
    return highestWeightRep({0, 1});
}

///////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////

namespace mty::algebra {

ostream &operator<<(ostream &fout, mty::algebra::Type obj)
{
    switch (obj) {
    case mty::algebra::Type::R:
        fout << "R";
        break;
    case mty::algebra::Type::A:
        fout << "A";
        break;
    case mty::algebra::Type::B:
        fout << "B";
        break;
    case mty::algebra::Type::C:
        fout << "C";
        break;
    case mty::algebra::Type::D:
        fout << "D";
        break;
    case mty::algebra::Type::E6:
        fout << "E6";
        break;
    case mty::algebra::Type::E7:
        fout << "E7";
        break;
    case mty::algebra::Type::E8:
        fout << "E8";
        break;
    case mty::algebra::Type::F4:
        fout << "F4";
        break;
    case mty::algebra::Type::G2:
        fout << "G2";
        break;
    default:
        fout << "Type number " << (int) obj << " unknown...";
    }
    return fout;
}

} // End of namespace mty::algebra
