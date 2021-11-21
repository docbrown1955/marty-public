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

#include "jsonLoader.h"
#include <fstream>
#include <sstream>
#include <csignal>

using namespace std;

namespace JSON {

void JSONAssert(bool condition, std::string const& spec)
{
    if (not condition) {
        cout << "JSONError: " << spec << endl;
        std::raise(SIGTERM);
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class JSON::Reader                            //
/*************************************************/
///////////////////////////////////////////////////

///////////////////////////////////////////////////
// Public members (load and save from/to file)
///////////////////////////////////////////////////

std::unique_ptr<Node> Reader::loadFromFile(string const& fileName)
{
    fin = ifstream(fileName);
    unique_ptr<Object> root;
    if (fin) {
        currentFileName = fileName;
        indent = 0;
        char c = readSeparator();
        while (c != '{' and not fin.eof())
            c = readSeparator();
        readNode(root, "##ROOT##");
        fin.close();
    }
    else
        cerr << "File \"" + fileName + "\" not found!\n";

    return unique_ptr<Node>(static_cast<Node*>(root.release()));
}

void Reader::saveToFile(std::string const& fileName,
                        Node*              tree)
{
    fout = ofstream(fileName);
    if (fout) {
        currentFileName = fileName;
        writeToFile(tree);
        fout.close();
    }
    else
        cerr << "File \"" + fileName + "\" not found!\n";
}

void Reader::saveToFile(std::string      const& fileName,
                        unique_ptr<Node> const& tree)
{
    saveToFile(fileName, tree.get());
}

///////////////////////////////////////////////////
// Private members
///////////////////////////////////////////////////

int Reader::readLineNumber()
{
    int lineCount = 1;
    fin.clear();     // need to clear error bits otherwise tellg returns -1.
    auto originalPos = fin.tellg();
    if (originalPos < 0)
        return -1;
    fin.seekg(0);

    char c;
    while ((fin.tellg() < originalPos) && fin.get(c)) {
        if (c == '\n')
            ++lineCount;
    }
    return lineCount;
}

void Reader::readingError(string const& error)
{
    int lineNumber = readLineNumber();
    ostringstream serr;
    serr << "readingError in fin \"" << currentFileName << "\" (l. "
        << lineNumber << "): " << error << endl;
    JSONAssert(false, serr.str());
}

void Reader::unexpectedEndOfFile()
{
    readingError("unexpected end of file \"" + currentFileName + "\".");
}

char Reader::readSeparator()
{
    char c;
    while (fin.get(c)) {
        if (c != ' ' and c != '\n')
            return c;
    }
    //unexpectedEndOfFile(fin);
    return c;
}

string Reader::readString()
{
    char c;
    string str;
    while (fin.get(c)) {
        if (c == '"')
            break;
        str += c;
    }
    if (fin.eof())
        unexpectedEndOfFile();

    return str;
}

char Reader::readBoolean(char first,
                         string& strNumber)
{
    strNumber = first;
    char end;
    while (fin.get(end)) {
        if (end == ',' or end == '}' or end == ']')
            break;
        if (end != ' ' and end != '\n')
            strNumber += end;
    }
    if (fin.eof())
        unexpectedEndOfFile();
    return end;
}

char Reader::readNumber(char first,
                        string& strNumber)
{
    strNumber = first;
    char end;
    while (fin.get(end)) {
        if (end == ',' or end == '}' or end == ']')
            break;
        if (end != ' ' and end != '\n')
            strNumber += end;
    }
    if (fin.eof())
        unexpectedEndOfFile();
    return end;
}

char Reader::getBooleanLeaf(Child& child,
                            string const& specifier,
                            char first)
{
    string strNumber;
    char end = readBoolean(first, strNumber);
    if (strNumber == "true") {
        child = make_unique<Leaf<bool>>(specifier, true);
    }
    else if (strNumber == "false") {
        child = make_unique<Leaf<bool>>(specifier, false);
    }
    else {
        readingError("Boolean value " + strNumber + " not recognized. Should be "
                "either true or false.");
    }
    return end;
}

char Reader::getNumberLeaf(Child& child,
                           string const& specifier,
                           char first)
{
    string strNumber;
    char end = readNumber(first, strNumber);
    bool isInteger = true;
    for (const auto& c : strNumber)
        if (not (c >= '0' and c <= '9')
            and not (c == '-')) {
            isInteger = false;
            break;
        }
    istringstream sin(strNumber);
    if (isInteger) {
        int num;
        sin >> num;
        child = make_unique<Leaf<int>>(specifier, num);
    }
    else {
        double num;
        sin >> num;
        child = make_unique<Leaf<double>>(specifier, num);
    }
    return end;
}

char Reader::getStringLeaf(Child&        child,
                           string const& specifier)
{
    string argument = readString();
    child = make_unique<Leaf<string>>(specifier, argument);

    return readSeparator();
}

char Reader::readNode(Child&        child,
                      string const& mainSpecifier)
{
    child = make_unique<Node>(mainSpecifier);
    char c;
    while ((c = readSeparator()) == '"') {
        string specifier = readString();
        c = readSeparator();
        if (c != ':')
            readingError((string)"expected ':' after specifier, not '"
                    + c + "'.");
        Child newChild;
        c = readObject(newChild, specifier);
        static_cast<Node*>(child.get())->addChild(newChild);
        if (c == '}')
            break;
        if (c != ',')
            readingError((string)"expecting '}' or ',' before '"
                    + c + "'.");
    }
    if (not fin.eof())
        c = readSeparator();

    return c;
} 

char Reader::readList(Child&        child,
                      string const& specifier)
{
    child = make_unique<List>(specifier);
    char c = readSeparator();
    while (c != ']') {
        Child newChild;
        c = readObject(newChild, specifier, c);
        static_cast<List*>(child.get())->addChild(newChild);
        if (c == ']')
            break;
        if (c != ',')
            readingError((string)"expecting ']' or ',' before '"
                    + c + "'.");
        c = readSeparator();
    }
    if (not fin.eof())
        c = readSeparator();

    return c;
}

char Reader::readObject(Child&        child,
                        string const& specifier)
{
    char c = readSeparator();
    return readObject(child, specifier, c);
}

char Reader::readObject(Child&        child,
                        string const& specifier,
                        char          first)
{
    switch(first) {
        case '{': return readNode(child, specifier); break;
        case '[': return readList(child, specifier); break;
        default:
            if (first == '"') 
                return getStringLeaf(child, specifier);
            if (first == 't' or first == 'f')
                return getBooleanLeaf(child, specifier, first);
            return getNumberLeaf(child, specifier, first);
    }
}

void Reader::writeToFileWithIndent(string const& str)
{
    fout << std::string(static_cast<size_t>(indent), ' ') << str;
}

void Reader::writeToFile(Object* object,
                         bool    writeSpec)
{
    if (writeSpec and object->getSpecifier() != "##ROOT##")
        writeToFileWithIndent("\""+object->getSpecifier()+"\" : ");
    if (typeid(*object) == typeid(Node) ) {
        fout << "{\n";
        Node* node = static_cast<Node*>(object);
        indent += indentStep;
        for (auto iter = node->begin(); iter != node->end(); ++iter) {
            writeToFile(iter->get());
            if (iter+1 != node->end())
                fout << ",\n";
        }
        indent -= indentStep;
        fout << "\n";
        writeToFileWithIndent("}");
    }
    else if (typeid(*object) == typeid(List)) {
        List* node = static_cast<List*>(object);
        if (writeSpec)
            fout << "[\n";
        else 
            fout << "[";
        indent += indentStep;
        if (writeSpec)
            writeToFileWithIndent("");
        for (auto iter = node->begin(); iter != node->end(); ++iter) {
            writeToFile(iter->get(), false);
            auto const &pointed = **iter;
            if (iter+1 != node->end()
                    and (typeid(pointed) == typeid(List) 
                    or typeid(pointed) == typeid(Node))) {
                fout << ", ";
                fout << "\n";
                writeToFileWithIndent("");
            }
            else if (iter+1 != node->end())
                fout << ", ";
        }
        indent -= indentStep;
        if (writeSpec){
            fout << "\n";
            writeToFileWithIndent("]");
        }
        else
            fout << "]";
    }
    else {
        auto leaf = dynamic_cast<AbstractLeaf*>(object);
        fout << leaf->getArgument();
    }
}

///////////////////////////////////////////////////
/*************************************************/
// Class JSON::Parser                            //
/*************************************************/
///////////////////////////////////////////////////

Node* Parser::parseNode(Node*              parent,
                        std::string const& specifier,
                        bool               mandatory)
{
    for (const auto& object : *parent)
        if (object->getSpecifier() == specifier) {
            JSONAssert(object->isNode(),
                    "Expecting node of type \"" + specifier + "\" but "
                    + " a leaf is given.");
            return static_cast<Node*>(object.get());
        }
    JSONAssert(not mandatory,
            "Node \"" + specifier
            + "\" should be specified but is not given.");
    return nullptr;
}

Node* Parser::parseNode(unique_ptr<Object> const& parent,
                        std::string        const& specifier,
                        bool                      mandatory)
{
    return parseNode(convert(parent), specifier, mandatory);
}

Node* Parser::convert(const std::unique_ptr<Object>& node)
{
    JSONAssert(node->isNode(),
            "Converting into a node an object that is not a node: \""
            + node->getSpecifier() + "\".");
    return static_cast<Node*>(node.get());
}

} // End of namespace JSON
