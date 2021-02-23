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
 * @file lhaBuiltIn.h
 * @brief Contains built-in read functions, updating directly SM and 
 * MSSM parameters from a .lha file.
 * @author Grégoire Uhlrich
 * @version 1.0
 * @date 2020-09-02
 */
#ifndef LHABUILTIN_H_INCLUDED
#define LHABUILTIN_H_INCLUDED

#include <string>
#include <csl.h>

namespace mty::lha {

class LHAFileData;

/**
 * @brief Load a .lha file for built-in values in MARTY, in particular SM and 
 * MSSM inputs.
 *
 * @details Reading this function will automatically set values for all 
 * constants appearing in expressions if you are using SM or MSSM (or more 
 * generally variables in mty::sm_input or mty::mssm_input namespaces).
 *
 * @note This function applies directly the file content on the expressions it
 * knows. If you want to simply get the file data, consider using 
 * Reader::readFile() instead.
 *
 * @param fileName Name of the file to read.
 *
 * @sa Reader, Reader::readFile()
 */
void readFile(std::string const &fileName);
/**
 * @brief Load a .lha file for built-in values in MARTY, in particular SM and 
 * MSSM inputs.
 *
 * @details Reading this function will automatically set values for all 
 * constants appearing in expressions if you are using SM or MSSM (or more 
 * generally variables in mty::sm_input or mty::mssm_input namespaces).
 *  
 * @note This function applies directly the file content on the expressions it
 * knows. If you want to simply get the file data, consider using 
 * Reader::readFile() instead.
 *
 * @param file Input file stream for the .lha file.
 *
 * @sa Reader, Reader::readFile()
 */
void readFile(std::ifstream &file);

/**
 * @brief Applies a lha element on an expression, calling 
 * csl::Abstract::setValue().
 *
 * @param data      LHAFileData read from the file.
 * @param nameBlock Name of the block in which the value appears.
 * @param id        ID of the value.
 * @param expr      Expression, should be a literal (a csl::Constant
 * typically).
 *
 * @return \b True  if the value of \b expr has been changed.
 * @return \b False else.
 */
bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        size_t             id,
        csl::Expr              &expr
        );

/**
 * @brief Applies a lha element on an expression, calling 
 * csl::Abstract::setValue().
 *
 * @param data      LHAFileData read from the file.
 * @param nameBlock Name of the block in which the value appears.
 * @param i         Row position (starts from 1) of the matrix element.
 * @param j         Column position (starts from 1) of the matrix element.
 * @param expr      Expression, should be a literal (a csl::Constant
 * typically).
 *
 * @return \b True  if the value of \b expr has been changed.
 * @return \b False else.
 */
bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        size_t             i,
        size_t             j,
        csl::Expr              &expr
        );

/**
 * @brief Applies a matrix lha element on an expression, calling 
 * csl::Abstract::setValue().
 *
 * @param data      LHAFileData read from the file.
 * @param nameBlock Name of the block in which the value appears.
 * @param expr      Expression, should be a csl::Matrix.
 *
 * @return \b True  if the values in \b expr have been changed.
 * @return \b False else.
 *
 * @note If no value is found for matrix, nothing happens and this function 
 * returns false. If at least one value is found, all other values are set to
 * zero by default.
 */
bool applyLHA(
        LHAFileData const &data,
        std::string_view   nameBlock,
        csl::Expr              &expr
        );

/**
 * @brief Helper struct to define easily an object on which LHA data will 
 * apply.
 */
struct LHAElementInputData {
    /**
     * @brief Possible id (if not matrix) of the expression.
     */
    size_t id;
    /**
     * @brief Expression to modify.
     */
    csl::Expr   expr;
    /**
     * @brief True if the expression is a matrix. Ohterwise must be a literal.
     */
    bool   isMatrix;
};

/**
 * @brief Helper struct to define a block of LHAElementInputData to make easy
 * the definition of a modifiable block of expressions.
 */
struct LHAInputData: public std::vector<LHAElementInputData> {
    /**
     * @brief Name of the block.
     */
    std::string nameBlock;
};

/**
 * @brief Applies LHA data on a LHAInputData element.
 *
 * @param data   LHAFileData read from a .lha file.
 * @param inputs List of inputs for a given block.
 */
void applyLHAOnInputs(
        LHAFileData const &data,
        LHAInputData      &inputs
        );

/**
 * @brief Returns all the input data for the SM.
 *
 * @return A LHAInputData containing the SMINPUTS block of expressions.
 */
LHAInputData &getSMInput();

/**
 * @brief Returnrs all the input data for MINPAR block.
 *
 * @param model Model type (Block MODSEL, id = 0)
 *
 * @return The input data for MINPAR block in the model \b model.
 */
LHAInputData getMINPARInput(int model);

/**
 * @brief Returns the input data for the EXTPAR block.
 *
 * @return The input data for the EXTPAR block.
 */
LHAInputData getEXTPARInput();

/**
 * @brief Returns the input data for a matrix input.
 *
 * @param matrix Matrix associated to the LHA input.
 * @param name   Name of the block defining the matrix.
 *
 * @return The input data of the block named \b name, associated to the matric
 * \b matrix.
 */
LHAInputData getMatrixInput(
        csl::Expr              &matrix,
        std::string const &name
        );

/**
 * @brief Returns all the input data for the MSSM.
 *
 * @param model Model integer corresponding to id 0 on MODSEL block.
 *
 * @return A LHAInputData containing the various blocks of expressions for
 * MSSM inputs.
 */
std::vector<LHAInputData> &getMSSMInput(int model);

} // End of namespace mty::lha

#endif
