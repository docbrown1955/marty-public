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

#include "parent.h"
#include "index.h"
#include "indicial.h"

using namespace std;

namespace csl {

AbstractParent::AbstractParent(const string &t_name) : commutable(true)
{
    setName(t_name);
}

AbstractParent::AbstractParent() : commutable(true)
{
}

Parent AbstractParent::self()
{
    return shared_from_this();
}

std::string const &AbstractParent::getName() const
{
    return name;
}

std::string const &AbstractParent::getLatexName() const
{
    return latexName;
}

std::string const &AbstractParent::getBaseName() const
{
    static std::string emptyName = "";
    return emptyName;
}

bool AbstractParent::getCommutable() const
{
    return commutable;
}

csl::ComplexProperty AbstractParent::getComplexProp() const
{
    return complexProp;
}

void AbstractParent::setName(string t_name)
{
    for (size_t i = 0; i != t_name.size(); ++i) {
        if (t_name[i] == ';') {
            if (i > 0 and t_name[i - 1] == '\\') {
                t_name.erase(i - 1);
            }
            else if (i + 1 != t_name.size()) {
                std::string reg(t_name.begin(), t_name.begin() + i);
                std::string latex(t_name.begin() + i + 1, t_name.end());
                for (size_t i = reg.size(); i-- > 0;) {
                    if (reg[i] == ' ')
                        reg.erase(reg.begin() + i);
                    else
                        break;
                }
                for (size_t i = 0; i != latex.size(); ++i) {
                    if (latex[i] == ' ') {
                        latex.erase(latex.begin() + i);
                        --i;
                    }
                    else
                        break;
                }
                name      = reg;
                latexName = (latex.empty()) ? reg : latex;
                return;
            }
        }
    }
    name      = t_name;
    latexName = t_name;
}

void AbstractParent::setLatexName(const string &t_name)
{
    latexName = t_name;
}

void AbstractParent::setCommutable(bool t_commutable)
{
    commutable = t_commutable;
}

void AbstractParent::setComplexProperty(csl::ComplexProperty t_prop)
{
    complexProp = t_prop;
}

int AbstractParent::getDim() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

int AbstractParent::getDim(const Space *) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

void AbstractParent::printPropDefinition(std::ostream &out,
                                         int           indentSize,
                                         bool          header) const
{
    auto indent  = std::string(indentSize, ' ');
    auto regName = csl::Abstract::regularName(getName());
    if (header) {
        if (complexProp == csl::ComplexProperty::Imaginary) {
            out << indent << "inline bool func_init_prop_" << regName
                << "() {\n";
            out << indent << indent << regName << "->setComplexProperty("
                << "csl::ComplexProperty::Imaginary);\n";
            out << indent << indent << "return true;\n";
            out << indent << "}\n";
            out << indent << "inline bool init_prop_" << regName << " = ";
            out << "func_init_prop_" << regName << "();\n";
        }
        else if (complexProp == csl::ComplexProperty::Complex) {
            out << indent << "inline bool func_init_prop_" << regName
                << "() {\n";
            out << indent << indent << regName << "->setComplexProperty("
                << "csl::ComplexProperty::Complex);\n";
            out << indent << indent << "return true;\n";
            out << indent << "}\n";
            out << indent << "inline bool init_prop_" << regName << " = ";
            out << "func_init_prop_" << regName << "();\n";
        }
    }
    else {
        if (complexProp == csl::ComplexProperty::Imaginary)
            out << indent << regName << "->setComplexProperty("
                << "csl::ComplexProperty::Imaginary);\n";
        else if (complexProp == csl::ComplexProperty::Complex)
            out << indent << regName << "->setComplexProperty("
                << "csl::ComplexProperty::Complex);\n";
    }
}

void AbstractParent::enableEvaluation()
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::disableEvaluation()
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::toggleEvaluation()
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

optional<Expr> AbstractParent::evaluate(Expr_info, csl::eval::mode) const
{
    return nullopt;
}

Expr const &AbstractParent::getEncapsulated() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::getExactEncapsulated(Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

void AbstractParent::setEncapsulated(Expr const &)
{
    CALL_SMERROR(CSLError::AbstractCallError);
}

vector<const Space *> AbstractParent::getSpace() const
{
    return vector<const Space *>();
}

const Space *AbstractParent::getFieldSpace() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

void AbstractParent::setFieldSpace(const Space *)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool AbstractParent::getFullySymmetric() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

bool AbstractParent::getFullyAntiSymmetric() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

vector<Permutation> AbstractParent::getPermutation() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

bool AbstractParent::isValued() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}

Expr AbstractParent::getTensor() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr AbstractParent::getTensor(Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

Expr AbstractParent::getTrace() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return nullptr;
}

bool AbstractParent::dependsOn(Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

bool AbstractParent::dependsExplicitlyOn(Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

void AbstractParent::addSpace(const Space *)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::setFullySymmetric()
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::setFullyAntiSymmetric()
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool AbstractParent::isTraceLessIn(csl::Space const *) const
{
    CALL_SMERROR(CSLError::AbstractCallError);
    return 0;
}
void AbstractParent::addTraceLessNess(csl::Space const *)
{
    CALL_SMERROR(CSLError::AbstractCallError);
}
void AbstractParent::removeTraceLessNess(csl::Space const *)
{
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::addSymmetry(int, int)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::addAntiSymmetry(int, int)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::setSymmetry(const Symmetry &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::setTensor(const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::setTrace(const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool AbstractParent::hasContractionProperty(const Abstract *, Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

Expr AbstractParent::contraction(const Abstract *, Expr_info) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return false;
}

void AbstractParent::addSelfContraction(
    const Expr &,
    const Expr &,
    const Expr &,
    optional<function<bool(Expr_info, Expr_info)>>)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::removeSelfContraction(const Expr &, const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

bool AbstractParent::hasChainContractionProperty() const
{
    return false;
}

vector<ContractionChain> AbstractParent::getContractionProperties() const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

void AbstractParent::addContractionProperty(csl::vector_expr const &,
                                            const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::removeContractionProperty(csl::vector_expr const &,
                                               const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

optional<Expr> AbstractParent::getComplexProperty(Expr_info) const
{
    return nullopt;
}

optional<Expr> AbstractParent::getHermitianProperty(Expr_info,
                                                    const Space *) const
{
    return nullopt;
}

optional<Expr> AbstractParent::getTransposedProperty(Expr_info,
                                                     const Space *) const
{
    return nullopt;
}

void AbstractParent::addComplexProperty(const Expr &, const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::addHermitianProperty(const Space *,
                                          const Expr &,
                                          const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

void AbstractParent::addTransposedProperty(const Space *,
                                           const Expr &,
                                           const Expr &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
}

vector<Parent> AbstractParent::getBrokenParts(const Space *) const
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return {};
}

vector<Parent> AbstractParent::breakSpace(const Space *,
                                          const vector<const Space *> &,
                                          const vector<size_t> &) const
{
    return vector<Parent>();
}

Expr AbstractParent::operator()(Index)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(const Tensor &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(Index, const Tensor &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(std::vector<Index>)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(const std::vector<int> &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(const std::vector<int> &, const Tensor &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

Expr AbstractParent::operator()(std::vector<Index>, const Tensor &)
{
    std::cout << getName() << ": type " << getType() << std::endl;
    CALL_SMERROR(CSLError::AbstractCallError);
    return CSL_UNDEF;
}

std::ostream &operator<<(std::ostream &fout, const AbstractParent &parent)
{
    fout << parent.name;
    if (not parent.commutable)
        fout << " not commutable";

    return fout;
}
} // End of namespace csl
