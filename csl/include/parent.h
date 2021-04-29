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

/*! \file parent.h
 * \author Grégoire Uhlrich
 * \version 1.0
 * \brief Base classes for parents and elements.
 */
#ifndef PARENT_H_INCLUDED
#define PARENT_H_INCLUDED

#include "abstract.h"

namespace csl {
class Space;
class Permutation;
class Symmetry;
class ContractionChain;
}

/*! \namespace cslParent
 * \brief Stores types of parents.
 */
namespace cslParent{

    /*! \enum PrimaryType */
    enum PrimaryType{

        Literal,
        Indicial, /*!< Indicial type.*/
        Field, /*!< Field type.*/
    };

    /*! \enum Type */
    enum Type{

        Constant,
        Variable,
        GenericIndicial, /*!< Generic indicial type.*/
        Metric, /*!< Metric type.*/
        Delta, /*!< Delta type.*/
        Epsilon,
        ScalarField, /*!< ScalarField type.*/
        TensorFieldElement, /*!< TensorFieldElement type.*/
    };
}

namespace csl {

/*! 
 * \brief Base class for all parents (indicial, fields etc). All parents derive
 * from this class.
 * \details The principle of a parent is to contain properties of a variety of
 * objects that share the same abstract concept, like indicial objects. Every
 * specialization (Element) of a tensor for example, is an object with indices
 * like \f$ g_{\mu\nu} \f$. But the Element can appear many times in different 
 * forms in an expression, we want its properties to appear only at one place in
 * the program, and we want an object that can generate the Element. This is the
 * Parent. The parent then has three important features: 
 * \n 1. It is the only one to contain the properties of the object, if an 
 * element wants to access its own properties it has to ask its parent.
 * \n 2. It is the only object able to generate properly an Element of its 
 * type.
 * \n 2. It cannot appear in an expression (since it is not derived from
 * Abstract). And it is normal because in a expression we want a
 * \f$ g_{\mu\nu} \f$ with given \f$ \mu \f$ and \f$ \nu \f$, not a simple
 * \f$ g \f$.
 */
class AbstractParent: public std::enable_shared_from_this<AbstractParent>{

    protected:

    /*! Name of the parent (will be given to the elements when created).
     */
    std::string name;

    /*! Latex name of the parent.
     */
    std::string latexName;

    /*! Commutability property of the parent (will be given to the elements when
     * created).
     */
    bool commutable;

    csl::ComplexProperty complexProp = csl::ComplexProperty::Real;

    /*! Properties of the parent in a form of a list (std::vector) of equations.
     */
    std::vector<Equation*> props;

    public:

    /*! \brief Default constructor, parent with no name.
     */
    AbstractParent();

    /*! \brief Intializes the name of the parent.
     * \param t_name Name of the parent.
     */
    explicit AbstractParent(const std::string& t_name);

    /*! \brief Copy constructor, c++ default.
     */
    AbstractParent(const AbstractParent& abstract) = default;

    /*! \brief Destructor.
     */
    virtual ~AbstractParent(){};

    Parent self();

    virtual bool isAnAbbreviation() const { return false; }

    /*! \return name
     */
    std::string const &getName() const;

    std::string const &getLatexName() const;

    virtual std::string const &getBaseName() const;

    /*! \return commutable
     */
    bool getCommutable() const;

    csl::ComplexProperty getComplexProp() const;

    /*! \param t_name New name of the parent.
     */
    virtual void setName(std::string t_name);

    virtual void setLatexName(const std::string& t_name);

    /*! \param t_commutable New commutability property of the parent.
     */
    void setCommutable(bool t_commutable);

    virtual
    void setComplexProperty(csl::ComplexProperty t_prop);

    /*! \brief Adds a property for the parent.
     * \param prop Equation defining the new property.
     */
    void addProperty(Equation* prop);

    /*! \brief Removes a property.
     * \param prop Property to remove.
     */
    void removeProperty(Equation* prop);

    /*! \return The primary type of the parent.
     */
    virtual cslParent::PrimaryType getPrimaryType() const = 0;

    /*! \return The type of the parent.
     */
    virtual cslParent::Type getType() const = 0;

    /*! \return The dimension of a parent, i.e. the number of indices.
     */
    virtual int getDim() const;

    virtual void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const = 0;

    void printPropDefinition(
            std::ostream &out,
            int           indentSize,
            bool          header
            ) const;

    virtual
    void enableEvaluation();

    virtual
    void disableEvaluation();

    virtual
    void toggleEvaluation();

    virtual
    std::optional<Expr> evaluate(
            Expr_info self,
            csl::eval::mode user_mode = csl::eval::base) const;

    virtual
    Expr const &getEncapsulated() const;

    virtual
    void setEncapsulated(Expr const &);

    /*! \brief Returns the dimension of the parent relatively to a certain 
     * vector space, i.e. the number of indices in this vector space.
     * \param t_space Vector space to investigate.
     * \return The dimansion in \b t_space.
     */
    virtual int getDim(const Space* t_space) const;

    virtual std::vector<const Space*> getSpace() const;

    virtual const Space* getFieldSpace() const;

    virtual void setFieldSpace(const Space* t_space);

    /*! \return \b True if the parent is indicial and fully symmetric in its 
     * indices.
     * \return \b False else.
     */
    virtual bool getFullySymmetric() const;

    /*! \return \b True if the parent is indicial and fully antisymmetric in its 
     * indices.
     * \return \b False else.
     */
    virtual bool getFullyAntiSymmetric() const;
    
    /*! \brief For an TensorParent returns all the possible permutations of 
     * the tensor with respect to the symmetries or antisymmetries in a vector
     * of Permutation.
     * \return A std::vector<Permutation> with all possible permutations of the 
     * tensor.
     */
    virtual std::vector<Permutation> getPermutation() const;

    virtual bool isValued() const;

    /*! \brief For a valued TensorParent returns the corresponding tensor. 
     * Valued means that the tensor has been defined by the user.
     * \return tensor.
     */
    virtual Expr getTensor() const;

    virtual Expr getTensor(Expr_info expr) const;

    /*! \brief For an TensorParent of dimension 2 (in a given unique space)
     * returns the trace of it, i.e. the sum of its diagonal elements.
     */
    virtual Expr getTrace() const;

    /*! \brief Tells if the parent depends on another expression or not.
     * \param expr from which we test the dependency.
     * \return \b True if the parent depends on\b expr.
     * \return \b False else.
     */
    virtual  bool dependsOn(Expr_info expr) const;

    /*! \brief Tells if the parent depends explicitely on another expression or
     * not.
     * \param expr from which we test the dependency.
     * \return \b True if the parent depends on\b expr.
     * \return \b False else.
     */
    virtual bool dependsExplicitlyOn(Expr_info expr) const;

    /*! \return props.
     */
    virtual const std::vector<Equation*>& getProperties() const;

    /*! \brief Adds a dimension to an TensorParent in a specific vector space.
     * \param t_space Space in which we add an index.
     */
    virtual void addSpace(const Space* t_space);

    /*! \brief Set the TensorParent fully symmetric.
     */
    virtual void setFullySymmetric();

    /*! \brief Set the TensorParent fully antisymmetric.
     */
    virtual void setFullyAntiSymmetric();

    virtual bool isTraceLessIn(csl::Space const* space) const;
    virtual void addTraceLessNess(csl::Space const* space);
    virtual void removeTraceLessNess(csl::Space const* space);

    /*! \brief Adds a symmetry between indices in position \b i1 and \b i2.
     * \param i1 Index of the first index in the symmetry.
     * \param i2 Index of the second index in the symmetry.
     */
    virtual void addSymmetry(int i1, int i2);

    /*! \brief Adds an antisymmetry between indices in position \b i1 and \b i2.
     * \param i1 Index of the first index in the symmetry.
     * \param i2 Index of the second index in the symmetry.
     */
    virtual void addAntiSymmetry(int i1, int i2);

    /*! \brief Sets all the symmetry properties of an TensorParent from an 
     * object of type Symmetry. This erase all possible properties of 
     * (anti)symmetry the parent had before that.
     * \param t_symmetry New symmetry of the parent.
     */
    virtual void setSymmetry(const Symmetry& t_symmetry);

    /*! \param New tensor corresponding to the TensorParent. Must match 
     * the index structure of course.
     */
    virtual void setTensor(const Expr& t_tensor);

    virtual void setTrace(const Expr& t_trace);

    /*! \brief Tells if an TensorParent has contraction properties with 
     * another expression.
     * \param self A pointer to the element corresponding to the parent.
     * \param B The other expression.
     * \return \b True if there is one particular contraction property.
     * \return \b False else.
     */
    virtual bool hasContractionProperty(const Abstract* self,
                                        Expr_info B) const;

    /*! \brief Applies a contractoin property if there is one between \b self
     * and \b B.
     * \param self A pointer to the element corresponding to the parent.
     * \param B The other expression.
     * \return The contracted object.
     */
    virtual Expr contraction(const Abstract* self, Expr_info B) const;

    virtual bool hasChainContractionProperty() const;

    virtual std::vector<ContractionChain>
        getContractionProperties() const;

    virtual void addContractionProperty(
            csl::vector_expr const& leftHandSide,
            const Expr&              rightHandSide);

    virtual void removeContractionProperty(
            csl::vector_expr const& leftHandSide,
            const Expr&              rightHandSide);

    /*! \brief Adds a contraction property between an Element and another that 
     * share the parent (*this).
     * \param A First Element in the contraction.
     * \param B Second Element in the contraction.
     * \return res The result of the contraction.
     */
    virtual void addSelfContraction(
            const Expr& A,
            const Expr& B,
            const Expr& res,
            std::optional<std::function<bool(Expr_info, Expr_info)>>
                condition = std::nullopt);

    virtual void removeSelfContraction(
            const Expr& A,
            const Expr& B
            );

    virtual std::optional<Expr> getComplexProperty(
            Expr_info self
            ) const;

    virtual std::optional<Expr> getHermitianProperty(
            Expr_info  self,
            const Space* space
            ) const;

    virtual std::optional<Expr> getTransposedProperty(
            Expr_info self,
            const Space* space
            ) const;

    virtual void addComplexProperty(const Expr& init,
                                    const Expr& res);

    virtual void addHermitianProperty(const Space* space,
                                      const Expr& init,
                                      const Expr&  res);

    virtual void addTransposedProperty(const Space* space,
                                       const Expr& init,
                                       const Expr&  res);

    virtual
    std::vector<Parent> getBrokenParts(const Space* broken) const;
    
    virtual
    std::vector<Parent> breakSpace(
            const Space*                     broken,
            const std::vector<const Space*>& newSpace,
            const std::vector<size_t>&       pieces
            ) const;

    /*! \brief Generate the Element for an TensorParent of dimension 1 (a 
     * single index).
     * \param index The Index of the Element.
     * \return The corresponding TensorElement.
     */
    virtual Expr operator()(Index index);

    virtual Expr operator()(const Tensor& point);

    /*! \brief Generate the Element for an TensorParent of arbitrary dimension
     * .
     * \param indices List of indices to generate the TensorElement.
     * \return The corresponding TensorElement.
     */
    virtual Expr operator()(std::vector<Index> indices);

    virtual Expr operator()(const std::vector<int>& indices);

    virtual Expr operator()(Index          index,
                            const Tensor& point);

    virtual Expr operator()(std::vector<Index> indices,
                            const Tensor&     point);

    virtual Expr operator()(const std::vector<int>& indices,
                            const Tensor&          point);

    /*! \function std::ostream& operator<<(std::ostream& fout ,
     *                                    const AbstractParent& p)
     *  \brief Display name and commutability of the parent in standard output.
     */
    friend std::ostream& operator<<(std::ostream& fout, const AbstractParent&);
};

class Parent: public std::shared_ptr<AbstractParent> {

    public:

    INHERIT_SHARED_PTR_CONSTRUCTOR(Parent, AbstractParent)

    template<class ...Args>
        inline
        Expr operator()(Args&& ...args)
        {
            return (**this)(args...);
        }

    template<class ...Args>
        inline
        Expr operator()(std::vector<Index> indices, Args&& ...args)
        {
            return (**this)(indices, args...);
        }

};


} // End of namespace csl

#endif
