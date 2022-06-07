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

/**
 * @file fermionFlow.h
 * @brief File containing the ConjugationInfo class. Used for Majorana 
 * amplitudes.
 * @author Grégoire Uhlrich
 * @version  1.0
 * @date 2020-08-25
 */
#ifndef FERMIONFLOW_H_INCLUDED
#define FERMIONFLOW_H_INCLUDED

#include <vector>
#include <csl.h>

namespace mty {

    class QuantumField;
    class ConjugationList;
    struct FermionLine;

    /**
     * @brief Class encapsulating necesarry data to place conjugation matrices
     * in amplitudes when treating Majorana fermions.
     */
    class ConjugationInfo {

    public:

        /**
         * @brief Type of ConjugationInfo.
         */
        enum Type {

            /*!
             * @brief Undefined ConjugationInfo.
             */
            Undefined,

            /**
             * @brief Conjugation of an external leg.
             */
            External,

            /**
             * @brief Conjugation of a part of a gamma-matrices chain.
             */
            Internal
        };

        /**
         * @brief Quick definition of the data structure for a field list in 
         * this file.
         */
        using FieldList = std::vector<mty::QuantumField const*>;

        /**
         * @brief Maximum number of fermion lines in a diagram.
         *
         * @details This number is 3 because for now a diagram (that can be
         * fully simplified in MARTY) cannot have 6 external legs (5 at most) 
         * and is at the 1-loop level. There can be two external fermionic 
         * lines, and one internal in the loop. If in the future MARTY is 
         * extended to more, this number may be incremented or left as it is, 
         * the consequence will only be an increase in execution time for the
         * function followFermionLines (if not incremented). This number is the
         * memory allocated the the return value. If there is more fermion 
         * lines, memory will have to be reallocated elsewhere with more space 
         * (implies deallocation, reallocation, copies etc).
         */
        constexpr static std::size_t maxFermionLines = 3;

        /**
         * @brief Resolves all fermion lines in a set of fields.
         *
         * @details This function is usefull when Majorana fermions come in 
         * amplitudes. Fermion flows must be defined for each fermion line 
         * (arbitrarily, 
         * see https://cds.cern.ch/record/240381/files/CM-P00063014.pdf)
         * to write down the amplitude. Each propagator and vertex must be 
         * matched from the beginning to the end, and rules must then be applied
         * to ensure that the fermion chain is continuous (no transposed gamma-
         * matrix in the middle).
         *
         * @param init      Initial set of fields in the Wick contraction.
         * @param ruleOrder Order of fields in the Feynman rules.
         *
         * @return A list of ConjugationInfo, telling where to apply the 
         * conjugation matrix to recover a clean fermion flow, and a sign coming
         * from fermion ordering.
         */
        static ConjugationList resolveFermionLines(
                FieldList        init,
                FieldList const &ruleOrder,
                bool             ruleMode
                );

        /**
         * @brief Resolves all fermion lines in a set of fields.
         *
         * @details This function is just an overload to convert a vector of 
         * QuantumField (given by the class Graph) to a FieldList.
         *
         * @param init      Initial set of fields in the Wick contraction.
         * @param ruleOrder Order of fields in the Feynman rules.
         *
         * @return A list of ConjugationInfo, telling where to apply the 
         * conjugation matrix to recover a clean fermion flow.
         */
        static ConjugationList resolveFermionLines(
                FieldList                 const &init,
                std::vector<QuantumField> const &ruleOrder,
                bool                             ruleMode
                );

        /**
         * @brief Simplifies a set of ConjugationInfo, merging the consecutive
         * ones.
         *
         * @details This functions uses the fact that \f$ C^TC = 1. \f$ For a 
         * list of conjugation, consecutive ones are merged. For example,
         * \f[
         *      \left(C\Gamma _1 C^T\right)\cdot \left(C\Gamma _2C^T\right)
         *      = C\Gamma _1\Gamma _2C^T.
         * \f]
         *
         * @param info     Vector of ConjugationInfo to simplify.
         * @param ruleMode Boolean, must be true if Feynman rule calculation.
         */
        static void simplify(
                ConjugationList &infoList,
                bool             ruleMode
                );

        static void applyConjugation(
                ConjugationList infoList,
                csl::Expr           &expr,
                bool            ruleMode
                );

    public:

        /**
         * @brief Constructs an empty ConjugationInfo. Will do nothing.
         *
         * @sa setInfo()
         */
        ConjugationInfo();

        /**
         * @brief Constructs a ConjugationInfo for an external leg.
         *
         * @param t_A          Index of the external leg to conjugate.
         * @param t_conjugated Boolean that must be true if the external leg is
         * initially complex conjugated.
         */
        explicit 
        ConjugationInfo(
                csl::Index const &t_A,
                bool              t_conjugated
                );

        /**
         * @brief Constructs a ConjugationInfo for a part of a fermion chain.
         *
         * @param t_A First index (to the left) of the fermion chain.
         * @param t_B Second index (to the right) of the fermion chain.
         */
        ConjugationInfo(
                csl::Index const &t_A,
                csl::Index const &t_B,
                csl::Index const &t_preA,
                csl::Index const &t_postB
                );

        /**
         * @brief Default constructor.
         */
        ~ConjugationInfo() = default;

        /**
         * @brief Default copy constructor.
         *
         * @param ConjugationInfo to copy.
         */
        ConjugationInfo(ConjugationInfo const &) = default;
        /**
         * @brief Default move constructor.
         *
         * @param ConjugationInfo to move.
         */
        ConjugationInfo(ConjugationInfo &&) = default;
        /**
         * @brief Default copy assignement operator.
         *
         * @param ConjugationInfo to copy.
         *
         * @return A reference to the target of the copy.
         */
        ConjugationInfo &operator=(ConjugationInfo const &) = default;
        /**
         * @brief Default move assignement operator.
         *
         * @param ConjugationInfo to move.
         *
         * @return A reference to the target of the copy.
         */
        ConjugationInfo &operator=(ConjugationInfo &&) = default;

        /**
         * @brief Returns the type.
         */
        Type getType() const { return type; }

        /**
         * @brief Returns the first index of the ConjugationInfo
         */
        csl::Index getA() const { return A; }

        /**
         * @brief Returns the second index of the ConjugationInfo, has a meaning
         * only for Internal ConjugationInfo.
         */
        csl::Index getB() const { return B; }

        /**
         * @brief Returns the index before A in the chain, has a meaning
         * only for Internal ConjugationInfo.
         */
        csl::Index getPreA() const { return preA; }

        /**
         * @brief Returns the index after B in the chain, has a meaning
         * only for Internal ConjugationInfo.
         */
        csl::Index getPostB() const { return postB; }

        /**
         * @brief Returns the conjugation of the ConjugationInfo, has a meaning
         * only for External ConjugationInfo.
         */
        bool getConjugated() const { return conjugated; }

        /**
         * @brief Sets info for an external ConjugationInfo. See the 
         * corresponding constructor for more info.
         *
         * @param t_A          Index of the external leg
         * @param t_conjugated Conjugation of the external leg
         */
        void setInfo(
                csl::Index const &t_A,
                bool              t_conjugated
                );

        /**
         * @brief Sets info for an internal ConjugationInfo. See the 
         * corresponding constructor for more info.
         *
         * @param t_A First index of the chain
         * @param t_B Second index of the chain
         */
        void setInfo(
                csl::Index const &t_A,
                csl::Index const &t_B,
                csl::Index const &t_preA,
                csl::Index const &t_postB
                );

        /**
         * @brief Applies the conjugation on the underlying expression.
         *
         * @details Inserts in the fermion chain (replacing indices carefully)
         * the conjugation matrix (or matrices) in the right place. After those
         * insertions, the C-matrix simplification should leave a clean and 
         * ordered fermion chain.
         *
         * @param expr     Expression to modify.
         * @param ruleMode Boolean, true if Feynman rule calculation.
         * @param lines    Fermion lines in the diagram.
         */
        void apply(
                csl::Expr &expr, 
                bool ruleMode, 
                std::vector<FermionLine> const &lines
                ) const;

    protected:

        void insertInChain(
                csl::Expr &expr
                ) const;

        void insertInChainForRule(
                csl::Expr &expr,
                std::vector<FermionLine> const &lines
                ) const;

        void insertExternal(
                csl::Expr &expr
                ) const;

    protected:

        /**
         * @brief Type of ConjugationInfo. Can be external (external leg) or 
         * internal (part of gamma-matrices chain).
         */
        Type       type;

        /**
         * @brief First (or only for External type) index of the chain.
         */
        csl::Index A;

        /**
         * @brief Second index of the chain, not valued for External 
         * ConjugationInfo.
         */
        csl::Index B;

        /**
         * @brief Index before A in the chain, not valued for External 
         * ConjugationInfo.
         */
        csl::Index preA;

        /**
         * @brief Index after B in the chain, not valued for External 
         * ConjugationInfo.
         */
        csl::Index postB;

        /**
         * @brief Boolean for External ConjugationInfo, tells if the external 
         * leg is complex conjugated.
         */
        bool       conjugated;
    };

    struct FermionLine {

        ConjugationInfo::FieldList fields;
        std::vector<csl::Index> indices;
    };

    class ConjugationList: public std::vector<ConjugationInfo> {

        public:

        ConjugationList(
                std::vector<ConjugationInfo> const &info,
                int                                &t_sign,
                std::vector<FermionLine> const     &t_lines,
                std::vector<QuantumField const*> const &t_bosons
                )
            :std::vector<ConjugationInfo>(info),
            sign(t_sign),
            lines(t_lines),
            bosons(t_bosons)
        {}

        std::vector<QuantumField const *> getFinalOrder(
                std::size_t sizeHint = 10
                ) const {
            std::vector<QuantumField const *> finalOrder;
            finalOrder.reserve(sizeHint);
            for (const auto &b : bosons) 
                finalOrder.push_back(b);
            for (const auto &line : lines)
                for (const auto &f : line.fields)
                    finalOrder.push_back(f);

            return finalOrder;
        }

        int sign = 1;
        std::vector<FermionLine>         lines;
        std::vector<QuantumField const*> bosons;
    };

    std::ostream &operator<<(
            std::ostream          &out,
            ConjugationInfo::Type type
            );

    std::ostream &operator<<(
            std::ostream          &out,
            ConjugationInfo const &info
            );

    ConjugationInfo::FieldList::iterator getExternalBegin(
            ConjugationInfo::FieldList &fields
            );
    ConjugationInfo::FieldList::iterator getInternalBegin(
            ConjugationInfo::FieldList &fields
            );
    ConjugationInfo::FieldList::iterator getBegin(
            ConjugationInfo::FieldList &fields
            );

} // End of namespace mty

#endif
