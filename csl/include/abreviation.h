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
 * @version 1.3
 
 * \brief
 */
#ifndef ABREVIATION_H_INCLUDED
#define ABREVIATION_H_INCLUDED

#include "abstract.h"
#include "space.h"
#include "interface.h"
#include "index.h"
#include "utils.h"
#include "replace.h"
#include "algo.h"

namespace csl {

class AbstractParent;
template<class Base>
class Abbreviation;

inline std::map<std::string, std::vector<AbstractParent*>> abbreviationData;

class Abbrev {

    template<class Base>
    friend class Abbreviation;

    public:

    static inline bool avoidDuplicates = true;
    static inline bool useDichotomy    = true;

    struct compareParents {
        bool operator()(AbstractParent* const& A,
                        AbstractParent* const& B);
    };
        
    static std::map<std::string, size_t> id_name;

    static bool isAnAbbreviation(Expr const &ab);

    static AbstractParent* find(std::string_view name);

    static AbstractParent* find(Expr const& abreviation);

    static AbstractParent* find_opt(std::string_view name);

    static AbstractParent* find_opt(Expr const& abreviation);

    private:

    static void addAbreviation(
            AbstractParent* ptr,
            std::string const &t_name
            );

    static void removeAbreviation(
            AbstractParent* ptr,
            std::string const &t_name
            );

    static std::string getFinalName(std::string_view initialName);

    static std::vector<AbstractParent*> &getAbbreviationsForName(
            std::string_view name
            );

    static void cleanEmptyAbbreviation();

    public:

    static void printAbbreviations(std::ostream& fout = std::cout);

    static void printAbbreviations(
            std::string_view name,
            std::ostream    &fout = std::cout
            );

    static void enableEvaluation(std::string_view name);

    static void disableEvaluation(std::string_view name);

    static void toggleEvaluation(std::string_view name);

    static void enableGenericEvaluation(std::string_view name);

    static void disableGenericEvaluation(std::string_view name);

    static void toggleGenericEvaluation(std::string_view name);

    static void enableEvaluation(Expr const& abreviation);

    static void disableEvaluation(Expr const& abreviation);

    static void toggleEvaluation(Expr const& abreviation);

    static csl::IndexStructure getFreeStructure(
            csl::IndexStructure const& structure);

    static csl::IndexStructure getFreeStructure(
            Expr const& expr);

    static std::optional<Expr> findExisting(
            std::string_view name,                
            Expr        const& encaps);

    static Expr makeSubAbbrev(
            std::vector<csl::Expr> const &args,
            bool                          isProd
            );

    static Expr makeAbbreviation(
            std::string name,    
            Expr const& encapsulated,
            bool        split = true
            );

    static Expr makeAbbreviation(
            Expr const& encapsulated,
            bool        split = true
            );

    static void removeAbbreviations(
            std::string const &name
            );

    template<class ...Args>
    static void replace(
		    Args &&...args
		    )
    {
        for (auto &el : abbreviationData)
            for (auto &ab : el.second)
                ab->setEncapsulated(
                    csl::Replaced(
                       ab->getEncapsulated(),
                       std::forward<Args>(args)...
                    )
                );
    }

    public:

    Abbrev() = delete;
};


template<class BaseParent>
class Abbreviation: public BaseParent {

    friend class Abbrev;

    public:

    template<class ...Args>
    Abbreviation(Expr const&        t_encapsulated,
                std::string const& t_name,
                Args&&         ... args)
        :BaseParent(Abbrev::getFinalName(t_name), std::forward<Args>(args)...),
        encapsulated(t_encapsulated),
        baseName(t_name),
        initialStructure(Abbrev::getFreeStructure(t_encapsulated))
    {
        Abbrev::addAbreviation(this, t_name);
    }

    ~Abbreviation()
    {
        Abbrev::removeAbreviation(this, baseName);
    }

    bool isAnAbbreviation() const override { return true; }

    void printDefinition(
            std::ostream &out = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override
    {
        std::string indent(indentSize, ' ');
        std::string regName = csl::Abstract::regularName(this->name);
        std::string regLite = csl::Abstract::regularLiteral(this->name);
        out << indent;
        if (header)
            out << "inline ";
        out << "csl::Expr " << regName << "_encaps = ";
        encapsulated->printCode(1, out);
        out << ";\n";
        out << indent;
        if (header)
            out << "inline ";
        out << "csl::Expr " << regName 
            << " = csl::Abbrev::makeAbbreviation(\"" << regLite
            << "\", " << regName << "_encaps);\n";
    }

    std::string const &getBaseName() const override {
        return baseName;
    }

    void enableEvaluation() override {
        evaluation = true;
    };

    void disableEvaluation() override {
        evaluation = false;
    };

    void toggleEvaluation() override {
        evaluation = not evaluation;
    };

    std::optional<Expr> evaluate(
            Expr_info self,
            csl::eval::mode user_mode = csl::eval::base) const override {
        if (evaluation or csl::eval::isContained(user_mode, csl::eval::abbreviation)) {
            Expr res = DeepCopy(encapsulated);
            if (self->isComplexConjugate())
                res = csl::GetComplexConjugate(res);
            csl::IndexStructure structure = self->getIndexStructure();
            CSL_ASSERT_SPEC(structure.size() == initialStructure.size(),
                    CSLError::RuntimeError,
                    "Wrong indicial structure " + toString(structure)
                    + " to apply for " + toString(initialStructure) 
                    + " initially.");
            // csl::IndexStructure intermediate(structure);
            // for (auto &index : intermediate)
            //     index = index.rename();
            std::map<csl::Index, csl::Index> mapping;
            csl::ForEachLeaf(res, [&](Expr &sub)
            {
                if (!IsIndicialTensor(sub))
                    return;
                IndexStructure &index = sub->getIndexStructureView();
                for (auto &i : index) {
                    auto pos = std::find(
                            initialStructure.begin(), 
                            initialStructure.end(),
                            i);
                    if (pos == initialStructure.end()) {
                        auto pos2 = mapping.find(i);
                        const bool sign = i.getSign();
                        if (pos2 == mapping.end()) {
                            mapping[i] = i.rename();
                            mapping[i].setSign(false);
                            i = mapping[i];
                            if (sign)
                                i = +i;
                        }
                        else {
                            i = mapping[i];
                            if (sign)
                                i = +i;
                        }
                    }
                }
            });
            Replace(res, initialStructure, structure);
            return Evaluated(res, user_mode);
        }
        return std::nullopt;
    }

    Expr const &getEncapsulated() const override {
        return encapsulated;
    }

    void setEncapsulated(Expr const &t_encapsulated) override {
        encapsulated = t_encapsulated;
    }

    private:

    Expr encapsulated;

    std::string baseName;

    csl::IndexStructure initialStructure;

    bool evaluation = false;
};


} // End of namespace csl

#endif
