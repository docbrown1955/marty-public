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
 * @version 1.0
 
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

    class LibFunction {

        public:

        static inline size_t nParamThresholdStructure = 10;

        DEFINE_DEFAULT_CPY_MV(LibFunction)

        LibFunction(std::string_view t_name,
                    Expr      const& t_expression,
                    bool             t_complexParameters = true);

        ~LibFunction() {}

        std::string const& getName() const;
        std::string getTensorName(csl::Parent const &tensor) const;
        Expr& getExpression();
        Expr const& getExpression() const;
        bool isTensorial() const;
        bool isComplexParameters() const;
        std::vector<csl::Tensor> const& getTensors() const;
        std::vector<csl::Tensor>& getTensors();
        std::vector<std::string> const& getIntermediateSteps() const;
        std::vector<std::string>& getIntermediateSteps();
        std::vector<std::string> const& getParameters() const;

        void setName(std::string_view t_name);
        void setExpression(Expr const& t_expression);
        void setTensorial(bool t_tensorial);
        void setComplexParameters(bool t_complexParameters);
        void addTensor(Expr const& tensor);
        void setTensors(std::vector<csl::Tensor> const& t_tensors);
        Expr addIntermediate(Expr const& intermediate);
        void setIntermediateSteps(std::vector<std::string> const& t_interm);
        void addParameter(std::string const &param);
        void setParameters(std::vector<std::string> const& t_parameters);
        void removeParameter(std::string_view param);

        void print(std::ostream& out,
                   bool          header,
                   std::string const &initInstruction = ""
                   ) const;

        private:

        void printName(std::ostream& out) const;
        void printParameters(std::ostream& out,
                             std::string const& sep = ",",
                             bool          sepLast = false) const;
        void printBody(
                std::ostream& out, 
                std::string const &initInstruction
                ) const;
        void printTensorBody(
                std::ostream& out,
                std::string const &initInstruction
                ) const;
        void printExpression(std::ostream& out,
                             Expr const&   expression,
                             int           indent,
                             std::string const& beginning = "") const;

        void cutParameters();
        void sortParameters();
        void parse();

        private:
        
        std::string name;

        std::string varType;

        Expr        expression;

        bool        tensorial;

        bool        complexParameters;

        mutable 
        LibEvalSession            session;

        mutable 
        LibEvalSession::Perf      perf;

        std::vector<csl::Tensor>  tensors;

        std::vector<std::string>  intermediateSteps;

        std::vector<std::string>  parameters;

        int tensorParameter = -1;
    };

} // End of namespace csl

#endif // ifndef CSL_LIBRARYFUNCTION_H_INCLUDED
