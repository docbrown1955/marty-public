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

/*! \file jsonLoader.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief File containing the JSON::Reader and the JSON::Parser to read .json
 * files and get data from it.
 */
#ifndef JSONLOADER_H_INCLUDED
#define JSONLOADER_H_INCLUDED

#include "jsonObject.h"
#include <fstream>
#include <optional>

namespace JSON {

/*!
 * \function void JSONAssert(bool condition, std::string const& spec)
 * \brief Assertion function for JSON reading / writing.
 * \param condition Consition to test in assertion.
 * \param spec      String to display in case of error.
 */
void JSONAssert(bool               condition,
                std::string const& spec);

/*!
 * 
 * \brief Class for static purpose only (i.e. not constructible) that handles
 * reading .json files. Once a file read, it can also rewrite its data in a 
 * file. However this functionnality is not used for now.
 */
class Reader {

    public:

    Reader() = delete;

    public:

    /*!
     * \brief Reads a .json file and returns a tree containing all the JSON
     * structure in ther file.
     * \param nameFile Name of the file.
     * \return A std::unique_ptr to the root Node of the tree (transfers 
     * ownership to the caller).
     */
    static std::unique_ptr<Node> loadFromFile(std::string const& nameFile);

    /*!
     * \brief Writes a tree into a file in json format.
     * \param nameFile Name of the file.
     * \param tree     A Node*, root Node of the tree.
     */
    static void saveToFile(std::string const& nameFile,
                           Node*              tree);

    /*!
     * \brief Writes a tree into a file in json format.
     * \param nameFile Name of the file.
     * \param tree     A std::unique_ptr<Node>, root Node of the tree.
     */
    static void saveToFile(std::string           const& nameFile,
                           std::unique_ptr<Node> const& tree);

    private:

    /*!
     * \return The current line when reading in a file. Useful to spot errors.
     */
    static int readLineNumber();

    /*!
     * \brief Displays an error message when a bad structure is encountered
     * in reading.
     * \param error Specification of the error.
     */
    static void readingError(std::string const& error);

    /*!
     * \brief Raises an error when the end of the file is reached while reading.
     */
    static void unexpectedEndOfFile();

    /*!
     * \brief Reads the next separator ('{', '[', ':', ...) ignoring spaces and
     * line break.
     * \return The separator char.
     */
    static char readSeparator();

    /*!
     * \brief Reads a string (parameter between "") once the first " has been 
     * read.
     * \return The string between "".
     */
    static std::string readString();

    /*!
     * \brief Reads a boolean, "treu" of "false", into a std::string.
     * \param first First character of the boolean (read by the caller).
     * \param strNumber String that is modified to store the boolean.
     * \return The separator encountered after the number.
     */
    static char readBoolean(char         first,
                            std::string& strBool);

    /*!
     * \brief Reads a number, double or int, into a std::string.
     * \param first First character of the number (read by the caller).
     * \param strNumber String that is modified to store the number.
     * \return The separator encountered after the number.
     */
    static char readNumber(char         first,
                           std::string& strNumber);

    /*!
     * \brief Constructs a Leaf that stores a boolean read in the file.
     * \param child unique_ptr to an Object modified during the call to point
     * to the Leaf created.
     * \param specifier The specifier of the Leaf.
     * \param first The first character of the boolean read by the caller.
     * \return The separator encountered after the boolean.
     */
    static char getBooleanLeaf(Child&             child,
                               std::string const& specifier,
                               char               first);

    /*!
     * \brief Constructs a Leaf that stores a number read in the file.
     * \param child unique_ptr to an Object modified during the call to point
     * to the Leaf created.
     * \param specifier The specifier of the Leaf.
     * \param first The first character of the number read by the caller.
     * \return The separator encountered after the number.
     */
    static char getNumberLeaf(Child&             child,
                              std::string const& specifier,
                              char               first);

    /*!
     * \brief Constructs a Leaf that stores a string read in the file.
     * \param child unique_ptr to an Object modified during the call to point
     * to the Leaf created.
     * \param specifier The specifier of the Leaf.
     * \return The separator encountered after the string.
     */
    static char getStringLeaf(Child&             child,
                              std::string const& specifier);

    /*!
     * \brief Constructs a Node read in the file.
     * \param child unique_ptr to an Object modified during the call to point
     * to the Node created.
     * \param specifier The specifier of the Leaf.
     * \return The separator encountered after the Node.
     */
    static char readNode(Child&             child,
                         std::string const& specifier);

    /*!
     * \brief Constructs a List read in the file.
     * \param child unique_ptr to an Object modified during the call to point
     * to the List created.
     * \param specifier The specifier of the Leaf.
     * \return The separator encountered after the List.
     */
    static char readList(Child&             child,
                         std::string const& specifier);

    /*!
     * \brief Constructs a Object read in the file.
     * \param child     unique_ptr to an Object modified during the call to
     * point to the Object created.
     * \param specifier The specifier of the Leaf.
     * \return The separator encountered after the Object.
     */
    static char readObject(Child&             child,
                           std::string const& specifier);

    /*!
     * \brief Constructs a Object read in the file.
     * \param child     unique_ptr to an Object modified during the call to
     * point to the Object created.
     * \param specifier The specifier of the Leaf.
     * \param first     The first character of the Object read by the caller.
     * \return The separator encountered after the Object.
     */
    static char readObject(Child&             child,
                           std::string const& specifier,
                           char               first);

    /*!
     * \brief Writes a string into the file (when writing) with the indentation
     * level \b indent.
     * \param str String to write in the file.
     */
    static void writeToFileWithIndent(std::string const& str);

    /*!
     * \brief Writes an Object into the file (when writing). 
     * \param object    Pointer to the Object to write.
     * \param writeSpec Boolean specifying if the specifier must be written or
     * not (must not be written for elements of a List).
     */
    static void writeToFile(Object* object,
                            bool    writeSpec = true);

    private:

    /*!
     * \brief Output stream for the Reader when writing in a file.
     */
    inline static std::ofstream       fout;

    /*!
     * \brief Input stream for the Reader when reading in a file.
     */
    inline static std::ifstream       fin;

    /*!
     * \brief Current file name when reading / writing. The only use of this 
     * variable is for specifying errors more clearly if there is multiple 
     * files.
     */
    inline static std::string         currentFileName;

    /*!
     * \brief Number of spaces for the indentation when writing in a file.
     */
    constexpr static const int indentStep = 2;

    /*!
     * \brief Current level of indentation when writing in a file.
     */
    inline static int                 indent;
};

/*!
 * 
 * \brief Class for static purpose only (i.e. not constructible) that gather 
 * data from a tree (JSON::Node object) read by JSON::Reader in a .json file.
 */
class Parser {

    public:

    Parser() = delete;

    public:

    /*!
     * \brief Allows to get arguments of a node, given the specifier and the 
     * type of the desired object. This function is not recursive: We search in
     * children only, not grand-children.
     * \tparam Targ     Type of the considered Leaf (int, double, string, ...).
     * \param node      Pointer to the Node in which we search the Leaf.
     * \param specifier Specifier to search.
     * \param mandatory Tells if the presence of the argument is mandatory. If 
     * true and not found, this function raises an error.
     * \return A std::optional\<Targ\> contaning the argument if found.
     * \return \b nullopt else.
     */
    template<typename Targ>
        static std::optional<Targ> parseArgument(
                Node*              node,
                std::string const& specifier,
                bool               mandatory = false) {
            if (not node)
                return std::nullopt;
            for (const auto& object : *node) {
                if (object->getSpecifier() == specifier) {
                    return interpretObject<Targ>(object.get());
                }
            }
            JSONAssert(not mandatory,
                    "Argument \"" + specifier + "\" should be specified in "
                    + "node \"" + node->getSpecifier() + "\".");
            return std::nullopt;
        }

    /*!
     * \brief Allows to get arguments of a node, given the specifier and the 
     * type of the desired object. This function is not recursive: We search in
     * children only, not grand-children.
     * \tparam Targ     Type of the considered Leaf (int, double, string, ...).
     * \param object    unique_ptr to the Object in which we search the Leaf.
     * \param specifier Specifier to search.
     * \param mandatory Tells if the presence of the argument is mandatory. If 
     * true and not found, this function raises an error.
     * \return A std::optional\<Targ\> contaning the argument if found.
     * \return \b nullopt else.
     */
    template<typename Targ>
        static std::optional<Targ> parseArgument(
                std::unique_ptr<Object> const& object,
                std::string             const& specifier,
                bool                           mandatory = false) {
            return parseArgument<Targ>(convert(object), specifier, mandatory);
        }

    /*!
     * \brief Allows to get a subNode of a node, given the specifier.
     * This function is not recursive: We search in children only, not 
     * grand-children.
     * \param parent    Pointer to the Node in which we search the sub-Node.
     * \param specifier Specifier to search.
     * \param mandatory Tells if the presence of the sub-Node is mandatory. If 
     * true and not found, this function raises an error.
     * \return A pointer to the sub-Node if found.
     * \return nullptr else.
     */
    static Node* parseNode(Node*              parent,
                           std::string const& specifier,
                           bool               mandatory = false); 

    /*!
     * \brief Allows to get a subNode of a node, given the specifier.
     * This function is not recursive: We search in children only, not 
     * grand-children.
     * \param parent    unique_ptr to the Object in which we search the sub-Node.
     * \param specifier Specifier to search.
     * \param mandatory Tells if the presence of the sub-Node is mandatory. If 
     * true and not found, this function raises an error.
     * \return A pointer to the sub-Node if found.
     * \return nullptr else.
     */
    static Node* parseNode(std::unique_ptr<Object> const& parent,
                           std::string             const& specifier,
                           bool                           mandatory = false); 

    /*!
     * \brief Returns the argument with the correct type from an Object that 
     * must be a Leaf.
     * \tparam Targ  Type of the argument to get (int, double, string, ...).
     * \param object unique_ptr to the Object. Must be a Leaf a type \b Targ, 
     * else this function raises an error.
     * \return The argument of the correct type.
     */
    template<typename Targ>
        static Targ interpretObject(Object* object) {
            JSONAssert(typeid(*object) == typeid(Leaf<Targ>),
                    "Bad type required in JSON::interpretObject for object \""
                  + object->getSpecifier() + "\": \""
                  + (std::string)(typeid(*object).name()) + "\" given, \""
                  + (std::string)(typeid(Leaf<Targ>).name()) + "\" expected.");
            return static_cast<Leaf<Targ>*>(object)->getTypedArgument();
        }

    static Node* convert(const std::unique_ptr<Object>& node); 
};

} // End of namespace JSON


#endif
