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

/*! \file indicial.h
 * \author Grégoire Uhlrich
 * \version 1.0
 * \brief Objects handling indexed expressions in order to perform indicial 
 * tensor calculations.
 * \bug For now setting an object fully symmetric or antisymmetric will cause 
 * bug if all the indices don't lie in the same space. When taking permutations,
 * indices from different spaces will be exchanged and the tensor will not 
 * accept them.
 */
#ifndef INDICIAL_H_INCLUDED
#define INDICIAL_H_INCLUDED

#include "memory_pool.h"
#include "literal.h"
#include "numerical.h"
#include "operations.h"
#include "index.h"
#include "symmetry.h"
#include "parent.h"
#include "element.h"
#include "vector.h"

namespace csl {

class ISum;
class IProd;

csl::vector_expr getAllPermutations(const Expr& expr);

/*! 
 * \brief Small class that allows to handle contractions of indices between 
 * tensors.
 * \details This class provides a constructor that takes two TensorElement and 
 * determines automatically the contractions between the two, and an operator==
 * that compares two contractions to see if they are the same.
 */
class SelfContraction{

    protected:

    std::vector<std::pair<int,csl::Index>> specialAValues;

    std::vector<std::pair<int,csl::Index>> specialBValues;

    /*! \brief Contains a list of pairs of integers. Each pair corresponds to
     * the places of the two contracted indices (respectively in the two 
     * expressions).
     */
    std::vector<std::pair<int,int>> contraction;

    std::optional<std::function<bool(Expr_info, Expr_info)>> condition;

    std::array<IndexStructure, 2> freeStructures;

    bool swappable = true;

    public:

    SelfContraction() {}

    /*! \brief Constructor. Takes two tensors and determines the contractions between
     * the two.
     * \param A Must be an TensorElement.
     * \param B Must be an TensorElement.
     */
    SelfContraction(const Expr& A,
                    const Expr& B,
                    std::optional<std::function<bool(Expr_info, Expr_info)>>
                        t_condition = std::nullopt);

    SelfContraction(Expr_info A,
                    Expr_info B,
                    std::optional<std::function<bool(Expr_info, Expr_info)>>
                        t_condition = std::nullopt);


    /*! \return \b True if the SelfContraction is empty (no pair of integers).
     * \return \b False else.
     */
    bool empty() const;

    bool conditionAppliesOn(Expr_info A,
                            Expr_info B) const;

    bool conditionAppliesOn(const Expr& A,
                            const Expr& B) const;

    bool isSwappable() const {
        return swappable;
    }

    void setSwappable(bool t_swappable) {
        swappable = t_swappable;
    }

    /*! \brief Compares the SelfContraction with another.
     * \return \b True if the \b other is a part of *this.
     * \return \b False else.
     */
    bool operator==(const SelfContraction& other) const;

    Expr applyIndices(const Expr&            A,
                      const Expr&            B,
                      const SelfContraction& targetContraction,
                      const Expr&            res) const;

    /*! \function std::ostream& operator<<(std::ostream& fout, 
     *                                     const SelfContraction& C)
     *  \brief Displays the SelfContraction \b c.
     */
    friend std::ostream& operator<<(std::ostream&          fout,
                                    const SelfContraction& c);

    private:

    SelfContraction getSwapped() const;

    IndexStructure getFreeContractionIndex(const Expr& expr,
                                           int         index) const;

    bool compare(const SelfContraction& other) const;

    static bool comparePairs(const std::vector<std::pair<int,Index>>& A,
                             const std::vector<std::pair<int,Index>>& B);
};

class ContractionChain {

    public:

    ContractionChain();

    ContractionChain(csl::vector_expr&               t_contractedTensors,
                     std::vector<IndexStructure>&     t_structures,
                     std::vector<std::array<int, 4>>& t_contraction);

    explicit 
    ContractionChain(const csl::vector_expr& args,
                     const Expr& res = CSL_UNDEF);

    explicit
    ContractionChain(const Expr& expr,
                     const Expr& res = CSL_UNDEF);

    public:

    ContractionChain(const ContractionChain& other) = default;

    ContractionChain& operator=(const ContractionChain& other) = default;

    std::optional<ContractionChain> splitAndEvaluate() const;

    bool contains(const Expr& tensor) const;

    Expr getResult() const;

    std::optional<std::vector<int>> comparison(
                const ContractionChain& other,
                Expr&                   remnant) const;

    bool operator==(const ContractionChain& other) const;

    bool operator<(ContractionChain const& other) const;

    friend 
    std::ostream& operator<<(std::ostream&           fout,
                             const ContractionChain& c);

    private:

    void gatherContractions();

    std::vector<std::vector<int>> getPermutations(
            const ContractionChain& other) const;

    std::optional<ContractionChain> createChain(
            int                     nChains,
            const std::vector<int>& chainNumber) const;

    static void mergeChains(std::vector<int>& chainNumber, 
                            int               chain1,
                            int               chain2);

    bool compareContractions(const std::array<int, 4>& c1,
                             const std::array<int, 4>& c2) const;

    bool detectSpecialContraction();

    bool detectCycle(const Space* space);

    bool isGoodIndex(
            csl::Space const *space,
            size_t            iTensor, 
            size_t            iIndex
            ) const;

    std::pair<csl::Index, csl::Index> getContractedIndices(
            csl::Space const *space,
            size_t            iTensor
            ) const;

    std::vector<std::vector<size_t>> getCycles(const Space *space);

    private:

    Expr scalarFactor = CSL_1;

    csl::vector_expr                contractedTensors;

    std::vector<IndexStructure>     structures;

    std::vector<std::array<int, 4>> contraction;

    bool specialContraction = false;

    bool cycleTrace = false;

    const Space* traceSpace = nullptr;

    Expr resultOfContraction = CSL_UNDEF;
};

class TensorElement;
class Symbol;

/*! 
 * \brief Parent of an indicial object, allows to generate them (TensorElement).
 */
class TensorParent: public AbstractParent{

    protected:

    /*! \brief Vector spaces in which the indices live.
     */
    std::vector<const Space*> space;

    std::vector<bool>         covariant;

    bool keepBestPermutation = true;

    /*! \brief Symmetry properties of the tensor.
     */
    Symmetry symmetry;

    /*! \brief Attribute that can be set to \b True if the tensor must be
     * fully symmetric.
     */
    bool fullySymmetric;

    /*! \brief Attribute that can be set to \b True if the tensor must be
     * fully anti-symmetric.
     */
    bool fullyAntiSymmetric;

    /*! \brief List of (self) constractions that have special properties.
     */
    std::vector<std::pair<SelfContraction, Expr>>  selfContraction;
    std::vector<std::pair<SelfContraction, Expr>>  extContraction;
    std::vector<csl::AbstractParent const*> externalTensors;

    std::vector<csl::Space const*> traceLessNess;

    std::vector<ContractionChain> chainContraction;

    mutable
    std::map<const csl::Space*, std::vector<csl::Parent>> brokenParts; 

    /*! \brief Boolean that is \b True iif an explicit tensor is given.
     */
    bool valued;

    /*! \brief Explicit tensor that can be set by the user.
     */
    Expr tensor;

    /*! \brief (almost) Unused attribute going to disappear.
     */
    Expr trace;

    std::optional<std::pair<IndexStructure, Expr>>
        conjugateProperty;

    std::map<const Space*, std::pair<IndexStructure, Expr>>
        hermitianProperty;

    std::map<const Space*,std::pair<IndexStructure, Expr>>
        transposedProperty;

    public:

    /*! \brief Default constructor.
     */
    TensorParent();

    /*! \brief Constructor that initializes only the name of the parent.
     * \param t_name Name of the parent.
     */
    explicit TensorParent(const std::string& t_name);

    /*! \brief Copy constructor = c++ default.
     */
    TensorParent(const TensorParent& abstract) = default;

    /*! \brief Constructor of a 1-indexed parent.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the Index lives.
     */
    TensorParent(const std::string& t_name,
                 const Space* t_space);

    /*! \brief Constructor of a n-indexed parent.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     */
    TensorParent(const std::string& t_name,
                 const std::vector<const Space*>& t_space);

    /*! \brief Constructor of a n-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    TensorParent(const std::string& t_name,
                 const std::vector<const Space*>& t_space,
                 const Expr& t_tensor);

    /*! \brief Constructor of a 1-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    TensorParent(const std::string& t_name,
                 const Space* t_space,
                 const Expr& t_tensor);
        
    public:

    template<class ...Args>
        friend
        std::shared_ptr<TensorParent> tensor_s(Args&& ...args);

    template<class ...Args>
        friend
        std::shared_ptr<TensorParent> tensor_s(
                std::string const&               name,
                std::vector<const Space*> const& indices,
                Args&& ...args);


    cslParent::PrimaryType getPrimaryType() const override;

    cslParent::Type getType() const override;

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    int getDim(const Space* t_space) const override;

    /*! \return The list of vector-spaces in which live the indices.
     */
    std::vector<const Space*> getSpace() const override;

    /*! \return The Symmetry object describing the parent's properties.
     */
    Symmetry getSymmetry() const;

    bool getFullySymmetric() const override;

    bool getFullyAntiSymmetric() const override;
    
    std::vector<Permutation> getPermutation() const override;

    bool isValued() const override;

    Expr getTensor() const override;

    Expr getTensor(Expr_info self) const override;

    Expr getTrace() const override;

    bool dependsOn(Expr_info expr) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    const std::vector<Equation*>& getProperties() const override;

    void setComplexProperty(csl::ComplexProperty t_prop) override;

    void setKeepBestPermutation(bool keep);

    void setFullySymmetric() override;

    void setFullyAntiSymmetric() override;

    bool isTraceLessIn(csl::Space const* space) const override;
    void addTraceLessNess(csl::Space const* space) override;
    void removeTraceLessNess(csl::Space const* space) override;

    void addSpace(const Space* space) override;

    void addSymmetry(int i1, int i2) override;

    void addAntiSymmetry(int i1, int i2) override;

    void setSymmetry(const Symmetry& t_symetry) override;

    void setTensor(const Expr& t_tensor) override;

    void setTrace(const Expr& t_trace) override;

    /*!
     * \brief Sets all potential variables in the tensor associated with the
     * indicial object elementary. For example, setting a vector as elementary
     * will set all vector elements (if they are variables) as elementary
     * variables, i.e. that depend only on themselves.
     *
     * \param t_elementary Boolean property of elementary-ness.
     */
    void setElementary(bool t_elementary);

    bool hasContractionProperty(const Abstract* self,
                                        Expr_info B) const override;

    std::vector<ContractionChain> getContractionProperties() const override;

    Expr contraction(const Abstract* self, Expr_info B) const override;

    void addSelfContraction(
            const Expr& A,
            const Expr& B,
            const Expr& res,
            std::optional<std::function<bool(Expr_info, Expr_info)>>
                condition = std::nullopt
            ) override;

    void removeSelfContraction(
            const Expr& A,
            const Expr& B
            ) override;

    bool hasChainContractionProperty() const override;

    void addContractionProperty(
            csl::vector_expr const& leftHandSide,
            const Expr& rightHandSide) override;

    void removeContractionProperty(
            csl::vector_expr const& leftHandSide,
            const Expr& rightHandSide) override;

    std::optional<Expr> getComplexProperty(
            Expr_info self) const override;

    std::optional<Expr> getHermitianProperty(
            Expr_info  self,
            const Space* t_space) const override;

    std::optional<Expr> getTransposedProperty(
            Expr_info  self,
            const Space* t_space) const override;

    std::optional<Expr> evaluate(
            Expr_info self,
            csl::eval::mode user_mode = csl::eval::base) const override;

    void addComplexProperty(const Expr& init,
                            const Expr& res) override;

    void addHermitianProperty(const Space* space,
                              const Expr& init,
                              const Expr&  res) override;

    void addTransposedProperty(const Space* space,
                               const Expr& init,
                               const Expr&  res) override;

    std::vector<Parent> getBrokenParts(const Space* broken) const override;

    std::vector<Parent> breakSpace(
            const Space*                     broken,
            const std::vector<const Space*>& newSpace,
            const std::vector<size_t>&       pieces
            ) const override;

    virtual Expr operator()(Index index) override;

    virtual Expr operator()(std::vector<Index> indices) override;

    virtual Expr operator()(const std::vector<int>& indices) override;

    /*! \brief Compares the parent with another.
     * \return \b True if the parents are equivalent.
     * \return \b False else.
     */
    virtual bool operator==(const TensorParent& other) const;

    /*! \brief Compares the parent with another.
     * \return \b False if the parents are equivalent.
     * \return \b True else.
     */
    virtual bool operator!=(const TensorParent& other) const;

    /*! \function std::ostream& operator<<(std::ostream& fout,
     *                                     const TensorParent& p)
     * \brief Displays an TensorParent p.
     */
    friend std::ostream& operator<<(std::ostream& fout, const TensorParent&);

    protected:

    void applyProperty(Expr_info self,
                       IndexStructure const& structure,
                       Expr& res) const;

    void checkIndexRequest(const Index& index);

    void checkIndexRequest(const std::vector<Index>& request);

    void createFixedIndices(Index& index) const;

    void createFixedIndices(std::vector<Index>& indices) const;
};

template<class ...Args>
    std::shared_ptr<TensorParent> tensor_s(Args&& ...args) {
        return csl::make_shared<TensorParent>(std::forward<Args>(args)...);
    }

template<class ...Args>
    std::shared_ptr<TensorParent> tensor_s(
            std::string const&               name,
            std::vector<const Space*> const& indices,
            Args&& ...args) {
        return csl::make_shared<TensorParent>(
                name,
                indices,
                std::forward<Args>(args)...);
    }

/*! 
 * \brief Specialization of TensorParent for a kronecker delta.
 */
class EpsilonParent: public TensorParent{

    public: 

    EpsilonParent(const EpsilonParent& abstract) = delete;

    explicit EpsilonParent(const Space* t_space);

    cslParent::Type getType() const override;

    bool hasContractionProperty(const Abstract* self,
                                Expr_info B) const override;

    Expr contraction(const Abstract* self, Expr_info B) const override;

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    Expr operator()(std::vector<Index> indices) override;
};

/*! 
 * \brief Specialization of TensorParent for a kronecker delta.
 */
class DeltaParent: public TensorParent{

    public: 

    /*! \brief Default constructor
     */
    DeltaParent();

    DeltaParent(const DeltaParent& abstract) = delete;

    explicit DeltaParent(const Space* t_space);

    cslParent::Type getType() const override;

    bool hasContractionProperty(const Abstract* self,
                                Expr_info B) const override;

    Expr contraction(const Abstract* self, Expr_info B) const override;

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    Expr operator()(std::vector<Index> indices) override;
};

/*! 
 * \brief Specialization of TensorParent for a matric object (two indices
 * symmetric etc). An object of type Space initializated with a metric will
 * automatically have signed indices.
 */
class MetricParent: public TensorParent{

    public: 

    /*! 
     * \brief Default constructor.
     */
    MetricParent();

    MetricParent(const MetricParent& abstract) = delete;

    /*! 
     * \brief Copy constructor from a DeltaParent: the metric is just
     * a kronecker delta.
     */
    explicit MetricParent(const DeltaParent& abstract);

    /*!
     * \brief Constructor from a Space and a tensor. The tensor here must be 
     * specified.
     *
     * \param t_space Space in which the metric must be applied.
     * \param t_tensor Tensor (Squared symmetric matrix) representing the 
     * metric.
     * \param t_name Name of the metric. Default = "g".
     */
    MetricParent(const Space* t_space, const Expr& t_tensor,
            const std::string& t_name="g");

    MetricParent(const Space* t_space,
            const std::string& t_name="g");
     

    cslParent::Type getType() const override;

    bool hasContractionProperty(const Abstract* self,
                                Expr_info B) const override;

    Expr contraction(const Abstract* self, Expr_info B) const override;

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    Expr operator()(std::vector<Index> indices) override;
};

class Tensor: public std::shared_ptr<TensorParent> {

    public:

    INHERIT_SHARED_PTR_CONSTRUCTOR(Tensor, TensorParent)

    /*! \brief Constructor of a 1-indexed parent.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the Index lives.
     */
    Tensor(
            const std::string& t_name,
            const Space* t_space
            )
        :Tensor(tensor_s(t_name, t_space))
    {}

    /*! \brief Constructor of a n-indexed parent.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     */
    Tensor(
            const std::string& t_name,
            const std::vector<const Space*>& t_space
            )
        :Tensor(tensor_s(t_name, t_space))
    {}

    /*! \brief Constructor of a n-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    Tensor(
            const std::string& t_name,
            const std::vector<const Space*>& t_space,
            const Expr& t_tensor
            )
        :Tensor(tensor_s(t_name, t_space, t_tensor))
    {}

    /*! \brief Constructor of a 1-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    Tensor(
            const std::string& t_name,
            const Space* t_space,
            const Expr& t_tensor
            )
        :Tensor(tensor_s(t_name, t_space, t_tensor))
    {}


    template<class ...Args>
        inline
        Expr operator()(Args&& ...args)
        {
            return (**this)(std::forward<Args>(args)...);
        }

    template<class ...Args>
        inline
        Expr operator()(const std::vector<int>& indices, Args&& ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    template<class ...Args>
        inline
        Expr operator()(const std::vector<Index>& indices, Args&& ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }
};

DEFINE_SHARED_PTR_OPERATOR(Tensor)

/*! 
 * 
 * \brief Specialization of Abstract for Indicial tensor. Building block
 * carrying indices, and respecting Einstein's summation convention.
 */
class TensorElement: public AbstractElement{

    protected:

    /*! \brief IndexStructure of the tensor.*/
    IndexStructure index;

    public:


    TensorElement() = default;
    /*!
     * \brief Constructor called by an TensorParent, for a tensor with 
     * one index.
     *
     * \param t_index (Only) index of the tensor.
     * \param t_parent Pointer to the TensorParent creating the tensor.
     */
    TensorElement(const Index&          t_index,
            const Parent& t_parent);

    /*!
     * \brief Constructor called by an TensorParent, for a tensor with 
     * several indices.
     *
     * \param t_index List of indices for the tensor.
     * \param t_parent Pointer to the TensorParent creating the tensor.
     */
    TensorElement(const std::vector<Index>& indices, 
            const Parent&     t_parent);

    /*!
     * \brief Constructor called by an TensorParent, for a tensor with 
     * several indices.
     *
     * \param t_index List of indices for the tensor in the form of an already
     * created IndexStructure.
     * \param t_parent Pointer to the TensorParent creating the tensor.
     */
    TensorElement(const IndexStructure& indices, 
            const Parent& t_parent);

    public:

    /*!
     * \brief Copy constructor from an Abstract*. Works only if the pointer 
     * points to another TensorElement.
     *
     * \param expression
     */
    explicit TensorElement(const Abstract*& expression);

    /*!
     * \brief Copy constructor from an Expr. Works only if the pointer 
     * points to another TensorElement.
     *
     * \param expression
     */
    explicit TensorElement(const Expr& expression);

    template<typename ...Args>
    friend Expr tensorelement_s(Args&& ...args);


#ifdef DEBUG
    ~TensorElement(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    csl::PrimaryType getPrimaryType() const override {
        return csl::PrimaryType::Indicial;
    }

    size_t memoryOverhead() const override;

    bool isIndexed() const override {
        return true;
    }

    int getNIndices() const override {
        return index.size();
    }

    void resetIndexStructure() override;

    IndexStructure getIndexStructure() const override {
        return index;
    }

    const IndexStructure& getIndexStructureView() const override {
        return index;
    }

    IndexStructure& getIndexStructureView() override {
        return index;
    }

    bool compareWithDummy(Expr_info              expr,
                          std::map<Index,Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    csl::Type getType() const override {
        return csl::Type::TensorElement;
    }

    std::optional<Expr> getComplexConjugate() const override;

    std::optional<Expr> getHermitianConjugate(
            const Space* space) const override;

    std::optional<Expr> getHermitianConjugate(
            const std::vector<const Space*>& spaces) const override;

    std::optional<Expr> getTransposed(
            const Space* space,
            bool         applyProp = true) const override;

    std::optional<Expr> getTransposed(
            const std::vector<const Space*>& t_spaces,
            bool                             applyProp = true) const override;

    Index getIndex(int i) const override;

    /*!
     * \brief Determines if the term \b expr can be factorized out from the 
     * TensorElement, i.e. if \b expr is equal to the tensor to (may be) a renaming
     * of index.
     * \details If \b exact is set to \b true, the function returns \b true
     * if the IndexStructure matches exactly. Else, the 
     * Comparator::freeIndexComparison() function is called.
     *
     * \param expr Term to compare.
     * \param exact Tells if the comparison must be exact regarding indices.
     *
     * \return \b True if \b expr can be factorized out.
     * \return \b False else.
     */
    bool askTerm(Expr_info expr, bool exact=false) const override;

    bool dependsOn(Expr_info expr) const override;
    
    bool dependsOn(Parent_info t_parent) const override;

    bool dependsExplicitlyOn(Expr_info expr) const override;

    const std::vector<Equation*>& getProperties() const override;

    void addProperty(Equation* property) override;

    void removeProperty(Equation* property) override;

    bool checkIndexStructure(
            const std::vector<Index>& t_index) const override;

    void selfCheckIndexStructure();

    std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    std::optional<Expr> replaceIndices(
             std::vector<Index> const &indexToReplace,
             std::vector<Index> const &newIndex,
             bool         refresh = true,
             bool         flipped = false) const override;

     void replaceIndexInPlace(
         Index const &oldIndex,
         Index const &newIndex) override;

    csl::vector_expr breakSpace(
            const Space*                     brokenSpace,
            const std::vector<const Space*>& newSpaces,
            const std::vector<std::string>&  indexNames
            ) const override;

    void setIndexStructure(const IndexStructure& t_index) override;

    bool hasContractionProperty(Expr_info B) const override;

    std::vector<ContractionChain> getContractionProperties() const override;

    Expr contraction(Expr_info B) const override;

    bool hasChainContractionProperty() const override;

    /*!
     * \brief Applies a permutation of the indices on a copy of the TensorElement,
     * and returns it.
     *
     * \param permutations Permutation to apply on the new TensorElement.
     *
     * \return 
     */
    Expr applyPermutation(const Permutation& permutations) const;

    csl::vector_expr getPermutations(bool optimize = true) const override;

    csl::vector_expr getAlternateForms() const override;

    Expr getCanonicalPermutation() const override;

    void print(int mode=0,
               std::ostream& out=std::cout,
               bool lib = false) const override;

    void printCode(
            int mode=0,
            std::ostream& out=std::cout
            ) const override;

    std::string printLaTeX(int mode=0) const override;

    std::vector<Parent> getSubSymbols() const override;

    std::optional<Expr> derive(Expr_info expr) const override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base
            ) const override;

    unique_Expr copy_unique() const override;

    Expr refresh() const override;

    Expr deepRefresh() const override;

    bool operator==(Expr_info expr) const override;

    bool operator<(const Abstract* expr) const override;

    protected:

    bool comparePermutations(Expr_info perm1,
                             Expr_info perm2) const;

    void selfCheckAutoIndices();

    Expr& applySelfStructureOn(Expr& expr) const override;

    void adjustMetricDeltaParent();

    csl::vector_expr applyBrokenIndices(
            std::vector<Parent>&             brokenParents,
            const Space*                     broken,
            const std::vector<const Space*>& newSpaces,
            const std::vector<Index>&        indices
            ) const;
};

inline csl::allocator<TensorElement> alloc_itensor;
inline csl::allocator<ISum>    alloc_isum;
inline csl::allocator<IProd>   alloc_iprod;

template<typename ...Args>
Expr tensorelement_s(Args&& ...args)
{
    return csl::make_shared<TensorElement, alloc_itensor>(
            std::forward<Args>(args)...)
        ->getCanonicalPermutation();
}

bool IsIndicialTensor(const Expr& expr);

bool IsIndicialTensor(Expr_info expr);

/*!
 * \brief Fills an tensor with variables of the same name with 
 * the numbers correponding to their place in the tensor. Allows for example
 * to name elements of a vector \f$ V \f$: \f$ \left( V_0,V_1,V_2,V_3\right)\f$.
 *
 * \details The parameter first is due to the recursive nature of the function.
 * It allows to know if the call of nameTensor() is the user's (first  = true)
 * or from another nameTensor()'s call (first = false). This parameter should 
 * then not be given by the user, but let to its default value \b true.
 *
 * \param name Name of the elements.
 * \param tensor Tensor to fill (the Expr is modified in the function).
 * \param first This parameter should not be given by the user.
 */
void nameTensor(const std::string& name, Expr& tensor, bool first=true);

/*!
 * \brief Generates a tensor of name \b name that lives in 
 * a list of spaces, filled with variables given by nameTensor().
 *
 * \param name Name of the tensor.
 * \param spaces List of spaces in which the tensor lives (just to get the 
 * dimensions). 
 *
 * \return An Expr that is the generated tensor.
 */
Expr generateTensor(const std::string& name,
        const std::vector<const Space*>& spaces);

/*! 
 * \brief Specialization of a Prod object to handle TensorElement objects in the
 * product (apply contraction of indices etc).
 */
class ISum: public Sum{

    public:

    /*! 
     * \brief Default constructor.
     */
    ISum();

    /*!
     * \brief Constructor for a vector of arguments, and the explicitSum
     * parameter (if \b true, we do not execute for mergeTerms(), and the 
     * vector of arguments stays as it is.
     *
     * \param t_argument std::vector of arguments in the sum.
     * \param explicitSum Tells if the sum is explicit or not, if it 
     * must stay as this explicit list of arguments or not.
     */
    ISum(const csl::vector_expr& t_argument, bool explicitSum=0);

    /*!
     * \brief Constructor with a left and right operands for the sum.
     * The explicitSum variable has the same meaning as for the other 
     * constructor.
     *
     * \param leftOperand Left operand.
     * \param rightOperand Right Operand.
     * \param explicitProd 
     */
    ISum(const Expr& leftOperand,
          const Expr& rightOperand,
          bool explicitSum=0); 


#ifdef DEBUG
    ~ISum(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    bool isIndexed() const override;

    IndexStructure getIndexStructure() const override;

    std::optional<Expr> factor(bool full = false) const override;

    /*!
     * \brief Checks for index contractions in the product. Applies Einstein's
     * summation convention and sums automatically repeated indices. This 
     * function is called after the construction in order to leave a correct
     * expression.
     */
    void selfCheckIndexStructure();

    std::optional<Expr> getTransposed(
            const Space* space,
            bool         applyProp = true) const override;

    std::optional<Expr> getTransposed(
            const std::vector<const Space*>& spaces,
            bool                             applyProp = true) const override;

    std::optional<Expr> getHermitianConjugate(
            const Space* space) const override;

    std::optional<Expr> getHermitianConjugate(
            const std::vector<const Space*>& spaces) const override;

    std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    std::optional<Expr> replaceIndices(
             std::vector<Index> const &indexToReplace,
             std::vector<Index> const &newIndex,
             bool         refresh = true,
             bool         flipped = false) const override;

    csl::vector_expr breakSpace(
            const Space*                     brokenSpace,
            const std::vector<const Space*>& newSpaces,
            const std::vector<std::string>&  indexNames
            ) const override;

    bool compareWithDummy(Expr_info               other,
                          std::map<Index, Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    bool operator==(Expr_info expr) const override;

    protected:

    void gatherFactors(csl::vector_expr& factors,
                       csl::vector_expr& arg,
                       bool full) const override;

    void clearRedundantIndicialFactors(csl::vector_expr& factors) const;

    void checkIndicialFactors(csl::vector_expr& factors) const;
};

/*! 
 * \brief Specialization of a Prod object to handle TensorElement objects in the
 * product (apply contraction of indices etc).
 */
class IProd: public Prod{

    public:

    /*! 
     * \brief Default constructor.
     */
    IProd();

    /*!
     * \brief Constructor for a vector of arguments, and the explicitProd
     * parameter (if \b true, we do not execute for mergeTerms(), and the 
     * vector of arguments stays as it is.
     *
     * \param t_argument std::vector of arguments in the product.
     * \param explicitProd Tells if the product is explicit or not, if it 
     * must stay as this explicit list of arguments or not.
     */
    IProd(const csl::vector_expr& t_argument, bool explicitProd=0);

    /*!
     * \brief Constructor with a left and right operands for the product.
     * The explicitProd variable has the same meaning as for the other 
     * constructor.
     *
     * \param leftOperand Left operand.
     * \param rightOperand Right Operand.
     * \param explicitProd 
     */
    IProd(const Expr& leftOperand, const Expr& rightOperand, bool explicitProd=0); 


#ifdef DEBUG
    ~IProd(){ __record_data_alloc(static_cast<int>(getType())); }
#endif

    bool isIndexed() const override;

    IndexStructure getIndexStructure() const override;

    Expr suppressTerm(Expr_info term) const override;

    void setArgument(const Expr& arg, int iArg=0) override;

    std::optional<Expr> evaluate(
            csl::eval::mode user_mode = csl::eval::base) const override;

    std::optional<Expr> getTransposed(
            const Space* space,
            bool         applyProp = true) const override;

    std::optional<Expr> getTransposed(
            const std::vector<const Space*>& spaces,
            bool                             applyProp = true) const override;

    std::optional<Expr> replaceIndex(
            const Index& indexToReplace,
            const Index& newIndex,
            bool         refresh = true) const override;

    std::optional<Expr> replaceIndices(
             std::vector<Index> const &indexToReplace,
             std::vector<Index> const &newIndex,
             bool         refresh = true,
             bool         flipped = false) const override;

    csl::vector_expr breakSpace(
            const Space*                     brokenSpace,
            const std::vector<const Space*>& newSpaces,
            const std::vector<std::string>&  indexNames
            ) const override;

    bool mergeTerms() override;

    void leftInsert(const Expr& expr) override;

    void rightInsert(const Expr& expr) override;

    bool compareWithDummy(Expr_info               expr,
                          std::map<Index, Index>& constraints,
                          bool keepAllCosntraints = false) const override;

    bool operator==(Expr_info expr) const override;

    protected:

    /*!
     * \brief Checks for index contractions in the product. Applies Einstein's
     * summation convention and sums automatically repeated indices. This 
     * function is called after the construction in order to leave a correct
     * expression.
     */
    void selfCheckIndexStructure(bool explicitTimes = false);

    bool partialComparison(Expr_info expr) const override;

    void applyContractionChains();

    bool hasSeparableIndicialDenominator() const;

    bool compareDenominatorSeparately(Expr_info other) const;
};

} // End of namespace csl

#endif
