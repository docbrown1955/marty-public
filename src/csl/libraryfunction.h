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
#ifndef CSL_LIBRARYFUNCTION_H_INCLUDED
#define CSL_LIBRARYFUNCTION_H_INCLUDED

#include <string>
#include "abstract.h"
#include "indicial.h"
#include "default_move_cpy.h"
#include "libraryevaluator.h"

namespace csl {

    class LibraryGroup;

    struct LibParameter {
        std::string name;
        std::string type;
        std::string ctype;
    };

    inline bool operator<(LibParameter const &A, LibParameter const &B) {
        return A.name < B.name;
    }

    class LibFunction {

        public:

        static inline size_t nParamThresholdStructure = 10;

        DEFINE_DEFAULT_CPY_MV(LibFunction)

        LibFunction(std::string_view t_name,
                    Expr      const& t_expression,
                    std::shared_ptr<LibraryGroup> const &t_group,
                    bool forceCppFunc = false);

        ~LibFunction() {}

        bool isTrivial() const;

        bool isCppFunc() const {
            return cppFunc;
        }

        std::string const& getName() const;
        std::string getTensorName(csl::Parent const &tensor) const;
        LibraryGroup const &getGroup() const { return *group; }
        LibraryGroup       &getGroup()       { return *group; }
        Expr& getExpression();
        Expr const& getExpression() const;
        bool isTensorial() const;
        int getPosTensorParameter() const { return tensorParameter; }
        void setPosTensorParameter(int t_pos) { tensorParameter = t_pos; }
        std::vector<csl::Tensor> const& getTensors() const;
        std::vector<csl::Tensor>& getTensors();
        std::vector<std::string> const& getIntermediateSteps() const;
        std::vector<std::string>& getIntermediateSteps();
        std::vector<LibParameter> const& getParameters() const;

        void setName(std::string_view t_name);
        void setExpression(Expr const& t_expression);
        void setTensorial(bool t_tensorial);
        void addTensor(Expr const& tensor);
        void setTensors(std::vector<csl::Tensor> const& t_tensors);
        Expr addIntermediate(Expr const& intermediate);
        void setIntermediateSteps(std::vector<std::string> const& t_interm);
        void addParameter(std::string const &param);
        void setParameters(std::vector<LibParameter> const& t_parameters);
        void removeParameter(std::string_view param);
        void addInitInstruction(std::string const &t_inst);

        static
        void cutParameters(
                std::vector<LibParameter> &parameters,
                int                       &tensorParameter
                );
        static
        void sortParameters(
                std::vector<LibParameter> &parameters,
                int                        tensorParameter
                );

        void printExternC(std::ostream& out,
                   bool          header,
                   std::string const &initInstruction = ""
                   ) const;
        void print(std::ostream& out,
                   bool          header,
                   std::string const &initInstruction = ""
                   ) const;
        void printCppWrapper(
                   std::ostream& out
                   ) const;

        private:

        void printName(std::ostream& out) const;
        void printCName(std::ostream& out) const;
        void printBody(
                std::ostream& out, 
                std::string const &initInstruction,
                bool isCSource
                ) const;
        void printExpression(std::ostream& out,
                             Expr const&   expression,
                             int           indent,
                             std::string const& beginning = "") const;

        void parse();

        private:
        
        std::string name;

        std::string varType;

        bool cppFunc;

        Expr        expression;

        bool        tensorial;

        mutable 
        LibEvalSession            session;

        mutable 
        LibEvalSession::Perf      perf;

        std::shared_ptr<LibraryGroup> group;

        std::vector<csl::Tensor>  tensors;

        std::vector<std::string>  intermediateSteps;

        std::vector<std::string>  initInstructions;

        std::vector<LibParameter> parameters;

        int tensorParameter = -1;
    };

} // End of namespace csl

#endif // ifndef CSL_LIBRARYFUNCTION_H_INCLUDED
