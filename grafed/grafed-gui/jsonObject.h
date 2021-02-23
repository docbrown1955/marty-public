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

/*! \file jsonObject.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief Stores object classes necessary to create a tree representing the
 * structure of .json files (Nodes and Leaf objects).
 */
#ifndef JSONOBJECT_H_INCLUDED
#define JSONOBJECT_H_INCLUDED

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

/*!
 * \namespace JSON
 * \brief Contains all objects related to JSON reading / writing.
 */
namespace JSON {

/*!
 * 
 * \brief Abstract object in JSON tree structure. Can be specialized either in
 * Node or in Leaf\<T\> with a specific type T.
 */
class Object {

    public:

    /*!
     * \brief Construtor with one paramter that initializes \b specifier.
     * \param t_specifier Name of the Object's specifier.
     */
    explicit
    Object(std::string const& t_specifier): specifier(t_specifier) {}

    /*!
     * \brief Destructor.
     */
    virtual ~Object() {};

    /*!
     * \return \b specifier The Object's specifier.
     */
    std::string getSpecifier() const {
        return specifier;
    }

    /*!
     * \brief Tells if the Object is a Node or not. This function is
     * reimplemented in Node.
     * \return False (default).
     */
    virtual bool isNode() const {
        return false;
    }

    protected:

    /*!
     * \brief String that represents the specifier of the object. For example:
     * "name": "electron" is an object of which the specifier is the string 
     * "name".
     */
    std::string specifier;
};

/*!
 * \brief Type definition for unique_ptr\<Object\>. Lighten the vector of 
 * Object owned by Node objects.
 */
typedef std::unique_ptr<Object> Child;

/*!
 * 
 * \brief Inherits from JSON::Object, specialized in JSON Node. A Node owns a 
 * vector of Object. The Node's children can either be Leaf or other Node 
 * objects. This allows to store the tree structure of a .json file.
 */
class Node: public Object {

    public:

    /*!
     * \brief Construtor with one paramter that initializes \b specifier.
     * \param t_specifier Name of the Node's specifier.
     */
    explicit
    Node(std::string const& specifier): Object(specifier) {}

    static
    std::unique_ptr<Node> make(std::string const& specifier) {
        return std::make_unique<Node>(specifier);
    }

    /*!
     * \brief Tells if the Object is a Node or not.
     * \return True.
     */
    bool isNode() const override {
        return true;
    }

    /*!
     * \brief Adds a child in the Node.
     * \param child New child added in \b children.
     */
    virtual void addChild(Child& child) {
        children.push_back(std::move(child));
    }

    /*!
     * \brief Adds a child in the Node.
     * \param child New child added in \b children.
     */
    template<class T>
    inline void addChild(std::unique_ptr<T>& child) {
        Child c = std::move(child);
        addChild(c);
    }
    
    /*!
     * \return **children.size()**, the number of children.
     */
    size_t size() const {
        return children.size();
    }

    /*!
     * \brief Tells if the Node is empty.
     * \return \b True if the node has no child.
     * \return \b False else.
     */
    bool empty() const {
        return children.empty();
    }

    /*!
     * \return A **const_iterator** to the begining of \b children.
     */
    std::vector<Child>::const_iterator begin() const {
        return children.begin();
    }

    /*!
     * \return A **const_iterator** to the end of \b children.
     */
    std::vector<Child>::const_iterator end() const {
        return children.end();
    }

    /*!
     * \return An **iterator** to the begining of \b children.
     */
    std::vector<Child>::iterator begin() {
        return children.begin();
    }

    /*!
     * \return An **iterator** to the end of \b children.
     */
    std::vector<Child>::iterator end() {
        return children.end();
    }

    /*!
     * \brief Returns the first child that has a given specifier.
     * \param spec String of the specifier to search.
     * \return An Object* pointing to the first child specified by \b spec
     * in \b children.
     * \return \b nullptr if no child has been found.
     */
    Object* getChild(std::string const& spec) const {
        for (const auto& child : children)
            if (child->getSpecifier() == spec)
                return child.get();
        return nullptr;
    }
    
    private:

    /*!
     * \brief std::vector of Child owned by the Node. Each child is in the form
     * of a unique_ptr to an Object, and can be either a Leaf or another Node.
     */
    std::vector<Child> children;
};

/*!
 * 
 * \brief A List is a particular Node of which all children are of the same 
 * type and have the same specifier.
 */
class List: public Node {

    public:

    /*!
     * \brief Construtor with one paramter that initializes \b specifier.
     * \param t_specifier Name of the List's specifier.
     */
    explicit
    List(std::string const& specifier): Node(specifier) {}

    static
    std::unique_ptr<List> make(std::string const& specifier) {
        return std::make_unique<List>(specifier);
    }

    /*!
     * \brief Adds a child in the List. Checks if the specifier is the same as 
     * the one of the List (each element must have the same). 
     * \param child New child added in \b children.
     */
    void addChild(Child& child) override {
        if (child->getSpecifier() != specifier) {
            std::cerr << "JSONParsingError: difference specification"
                << " \"" << child->getSpecifier() << "\" in list.\n";
            exit(1);
        }
        Node::addChild(child);
    }

    /*!
     * \brief Adds a child in the List. Checks if the specifier is the same as 
     * the one of the List (each element must have the same). 
     * \param child New child added in \b children.
     */
    template<class T>
    void addChild(std::unique_ptr<T>& child) {
        if (child->getSpecifier() != specifier) {
            std::cerr << "JSONParsingError: difference specification"
                << " \"" << child->getSpecifier() << "\" in list.\n";
            exit(1);
        }
        Node::addChild(child);
    }
};

/*!
 * 
 * \brief Abstract class inherited from Object, from which all leafs will
 * derive. This class only contains a pure virtual fucntion, that can then
 * be called for a Leaf without knowing its exact type.
 */
class AbstractLeaf: public Object {
    public:

    /*!
     * \brief Construtor with one paramter that initializes \b specifier.
     * \param t_specifier Name of the Leaf's specifier.
     */
    explicit
    AbstractLeaf(std::string const& specifier): Object(specifier) {}

    /*!
     * \brief Returns a std::string representing the argument of the Leaf. 
     * \return The std::string corresponding to the argument.
     */
    virtual std::string getArgument() const = 0;
};

/*!
 * 
 * \brief Template class inherited from Object that stores a parameter of a 
 * .json file. This class does not have any child or structure, simply contains
 * final values (double, int, string, ...).
 * \tparam Type Type of the parameter read in the .json file.
 */
template<typename Type>
class Leaf: public AbstractLeaf {

    public:

    /*!
     * \brief Constructor with two paramters. The specifier and the argument of
     * the Leaf.
     * \param specifier Name of the Leaf's specifier.
     * \param t_argument Argument of the Leaf (can be an int, a double, a 
     * string...).
     */
    Leaf(std::string const& specifier, Type t_argument)
        :AbstractLeaf(specifier), argument(t_argument) {}

    static
    Child make(std::string const& specifier, Type t_argument) {
        return std::make_unique<Leaf<Type>>(specifier, t_argument);
    }

    std::string getArgument() const {
        std::ostringstream sout;
        if (typeid(argument) == typeid(std::string)) 
            sout << "\"" << argument << "\"";
        else
            sout << argument;
        if constexpr(std::is_same<Type, double>::value) {
            if (argument == std::round(argument)) {
                sout << '.' << '0';
            }
        }

        return sout.str();
    }

    /*!
     * \brief Returns the bare argument with its own type, not a string (like
     * the function getArgument() does).
     * \return \b argument.
     */
    Type getTypedArgument() const {
        return argument;
    }

    private:

    /*!
     * \brief Argument of the Leaf. Final position on JSON tree, contains the 
     * int, double, string, ..., needed further in the program.
     */
    mutable Type argument;
};

} // End of namespace JSON

#endif
