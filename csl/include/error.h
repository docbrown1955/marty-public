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

/*! \file error.h
 * \author Grégoire Uhlrich
 * \version 1.3
 * \brief File containing functions that are called by the program
 * when something wrong happened: determines the behaviour in unexpected
 * cases.
 */
#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <iostream>
#include <string_view>
#include <csignal>
#include "options.h"

namespace cslError{

    /*! \enum Error
     * \brief List all error possible.
     */
    enum Error{
        AbstractFuncCalled=1, /*!<  A function has been called with an object 
                               for which it is not defined. */
        BadCopyConstructor, /*!<  Copy constructor called with wrong type. */
        ElementSequence, /*!<  Setting an element of a vectorial expression  
                          with a sequence of element. */
        OutOfBounds, /*!<  Index out of bounds for Expression with mutiple
                      arguments. */
        EvalNotValued, /*!<  Evaluating an expression that cannot have a value. */
        UndefinedBehaviour, /*!<  Apparition of a case that has not been defined.*/
        ContractDummy, /*!<  Trying to contract an index that is already contracted. */
        ContractionMismatch, /*!<  Trying to contract indices that do not match. */
        SymmetryMismatch, /*!<  Incompatibility between symmetries in the 
                           properties of an indicial expression. */
        UnknownProperty, /*!<  Trying to get a property of Abstract that is not
                          defined..*/
        InvalidITensor, /*!<  Invalid creation of TensorElement: bad indices. */
        InvalidIndicialParent,  /*!<  Invalid parent: tensor not corresponding. */
        InvalidIndicialSum, /*!<  Summing two terms that have not the same index 
                             structure */
        BadDependency, /*!<  Setting a contradictory dependency for a variable. */
        BadType,  /*!<  Bad type of Abstract sent in a function. */
        BadSymmetry, /*!<  Bad Symmetry sent in TensorParent (wring dimension). */
        BadContraction,  /*!<  Bad contraction of indices in expression. */
        UnknownEquation, /*!<  Unknown type of equation used. */
        BadPropertySetting,  /*!<  Setting a property that is not an equality. */
        ArbitraryEvaluated, /*!<  An arbitrary Abstract is evaluated, should not. */
        ComparingNotDummy,  /*!<  A non-dummy Abstract is sent in Comparator. */
        BadCommutationProp,  /*!< Setting the commutation of an object for which it is determined. */
        NegativeGroupDimension,  /*!<  Negative dimension in creation of group. */
        NoContractionProperty,  /*!<  Calling a non-exiting special contraciton of ITensors. */
        BadSelfContraction, /*!<  Self-contraction property of TensorParent badely initialized. */
        BadFieldInitialization, /*!<  Bad initilization of field. */
        OperatorNotImplemented, /*!<  Rules for operator not implemented */
        BadOperator, /*!<  Bad initialization of operator */

    };

    /*! \enum Warning
     * \brief List of all warning possible.
     */
    enum Warning{
        UnknownType=0, /*!<   The type of an expression is not handled in some
                        function.*/
        InvalidDimension, /*!<  Dimension mismatch when using Vectorial. */
        FactorialFloat, /*!<   Evaluating a factorial of non-integer value.*/
        NotValued, /*!<  Evaluating a literal that is not valued. */
        Infinity, /*!<  Apparition of infinity or undefined value in calculation. */
        CopyingBuildingBlock, /*!<  Copying building block (should not, they are 
                                and should stay shared_from_this) */
        NCharatersNotImplemented, /*!<  The function getNCharacters is not implemented for an object. */

    };

}

namespace csl {

#ifdef DEBUG
/*! \var STOP_ERROR_DEBUG_MODE
 * \brief This variable is set to true in debug mode, in order to intentionnaly
 * crash the program and allow gdb to get the Traceback of function calls. In 
 * release mode, this variable should be set to false.
 */
constexpr static const bool STOP_ERROR_DEBUG_MODE = true;
#else
constexpr static const bool STOP_ERROR_DEBUG_MODE = true;
#endif

/*! \fn callError(cslError::Error error, const std::string& caller, T spec)
 * \brief Displays an error message depending on the error \b error, the name 
 * of the caller function \b caller and a possible specificity of the error \b spec.
 * For example spec is the index for OutOfBound error. This function stops 
 * the program.
 * \param error The type of error.
 * \param caller The caller function.
 * \param spec A printable specificity of the error.
 */
template<typename T>
inline void callError(cslError::Error error, const std::string& caller, T spec)
{
    using namespace cslError;
    std::cout<<"Error "<<(int)error<<": ";
    switch(error)
    {
        case AbstractFuncCalled:
               std::cout<<"Abstract function \""<<caller<<"\" called. Should not be, virtual function.\n"; break;
        case BadCopyConstructor:
               std::cout<<"Copy constructor called with wrong type in "<<caller<<": "<<spec<<".\n"; break;
        case OutOfBounds:
               std::cout<<"Index "<<spec<<" out of bounds in "<<caller<<".\n"; break;
        case ElementSequence:
               std::cout<<"Initializing a vector element with a sequence in "<<caller<<".\n"; break;
        case EvalNotValued:
               std::cout<<"Evaluating the value of non valued Abstract in "<<caller<<".\n"; break;
        case UndefinedBehaviour:
               std::cout<<"Undefined behaviour in "<<caller<<"\n."; break;
        case ContractDummy:
               std::cout<<"Contracting dummy index in "<<caller<<": "<<spec<<std::endl; break;
        case ContractionMismatch: 
               std::cout<<"Mismatch in contraction ("<<caller<<"): "<<spec<<std::endl; break;
        case SymmetryMismatch: 
               std::cout<<"Mismatch in symmetry in "<<caller<<".\n"; break;
        case UnknownProperty:
               std::cout<<"Unknown property in "<<caller<<": "<<spec<<std::endl; break;
        case InvalidITensor:
               std::cout<<"Bad set of indices in creation of TensorElement in "<<caller<<std::endl; break;
        case InvalidIndicialParent:
               std::cout<<"Invalid initialization of TensorParent in "<<caller<<": bad tensor.\n"; break;
        case InvalidIndicialSum:
               std::cout<<"Invalid sum of indicial objects in "<<caller<<std::endl; break;
        case BadDependency:
               std::cout<<"Bad dependency set for "<<spec<<" in "<<caller<<"\n"; break;
        case BadType: 
               std::cout<<"Bad type of expression sent in "<<caller<<": requires "<<spec<<".\n"; break;
        case BadSymmetry :
               std::cout<<"Bad symmetry set in TensorParent (wrong dimension) in "<<caller<<".\n"; break;
        case BadContraction:
               std::cout<<"Bad contraction of indices in "<<caller<<": "<<spec<<".\n"; break;
        case UnknownEquation:
               std::cout<<"Unknown type of equation in "<<caller<<": "<<spec<<".\n"; break;
        case BadPropertySetting:
               std::cout<<"Setting a property with something else than an equality in "<<caller<<": "<<spec<<".\n"; break;
        case ArbitraryEvaluated:
               std::cout<<"An arbitrary abstract has been evaluated from "<<caller<<". Should not be.\n"; break;
        case ComparingNotDummy:
               std::cout<<"Comparing non-dummy Abstract in "<<caller<<".\n"; break;
        case BadCommutationProp:
               std::cout<<"Setting commutability property of an object for which it is not possible in "<<caller<<".\n"; break;
        case NegativeGroupDimension:
               std::cout<<"Negative dimension encountered in "<<caller<<": "<<spec<<std::endl; break;
        case NoContractionProperty:
               std::cout<<"Calling a special contraction of TensorElement in "<<caller<<", but there is no contraction.\n"; break;
        case BadSelfContraction:
               std::cout<<"Initializing badly a self-contraction property of TensorParent in "<<caller<<".\n"; break;
        case BadFieldInitialization:
               std::cout<<"Bas initialization of field in "<<caller<<": need a vector of the right space.\n"; break;

        case OperatorNotImplemented:
               std::cout<<"Rules of the operator are not implemented in "<<caller<<".\n"; break;
        case BadOperator:
               std::cout<<"Bad initialization of operator in "<<caller<<".\n"; break;
               
        default:
               std::cout<<"SmError "<<(int)error<<" not recognized.\n";
    }
    if (STOP_ERROR_DEBUG_MODE)
        std::raise(SIGSEGV);
    else if (option::errorStopsProgram)
        exit((int)error);
}
/*! \fn callError(cslError::Error error, const std::string& caller)
 * \brief Calls callError(cslError::Error, const std::string&,T) with no spec.
 */
void callError(cslError::Error error, const std::string& caller);

/*! \fn callWarning(cslError::Warning warning, const std::string& caller, T spec)
 * \brief Displays a warning message depending on the warning \b warning, the name 
 * of the caller function \b caller and a possible specificity of the warning \b spec.
 * For example spec is the dimension for InvalidDimension error. This function
 * does not stop the program.
 * \param warning The type of warning.
 * \param caller The caller function.
 * \param spec A printable specificity of the warning.
 */
template<typename T>
inline void callWarning(cslError::Warning warning, const std::string& caller, T spec)
{
    using namespace cslError;
    std::cout<<"Warning "<<(int)warning<<": ";
    switch(warning)
    {
        case UnknownType:
               std::cout<<"ScmType "<<" unknown in function \""<<caller<<"\".\n"; break;
        case InvalidDimension:
               std::cout<<"Invalid dimension "<<spec<<" in function "<<caller<<".\n"; break;
        case FactorialFloat:
               std::cout<<"Float value encountered in Factorial. Replacing by rounded value.\n"; break;
        case NotValued:
               std::cout<<"Evaluating non valued in "<<caller<<". Replacing by 0.\n"; break;
        case Infinity:
               std::cout<<"Infinite result found in "<<caller<<".\n"; break;
        case CopyingBuildingBlock:
               std::cout<<"Copying building block in "<<caller<<": "<<spec<<"\n"; break;
        case NCharatersNotImplemented:
               std::cout<<"The function getNCharacters() is not implemented for type "<<spec<<" in "<<caller<<".\n"; break;

        default:
               std::cout<<"SmWarning "<<(int)warning<<" not recognized.\n";
    }
}

} // End of namespace csl


enum class CSLError{

    AbstractCallError,  //Raised when an abstract (not implemented) fucntion is called.
    AttributeError, 	//Raised when attribute assignment or reference fails.
    EOFError,           //Raised when the input() functions hits end-of-file condition.
    FloatingPointError, //Raised when a floating point operation fails.
    GeneratorExit,  	//Raise when a generator's close() method is called.
    IndexError,     	//Raised when index of a sequence is out of range.
    IOError,            //Raised when a file is not found.
    KeyError,       	//Raised when a key is not found in a dictionary.
    MathError,    
    NameError,      	//Raised when a variable is not found in local or global scope.
    NotImplementedError,//Raised by abstract methods.
    OverflowError,   	//Raised when result of an arithmetic operation is too large to be represented.
    RuntimeError,       //Raised when an error does not fall under any other category.
    SyntaxError,    	//Raised by parser when syntax error is encountered.
    TypeError,      	//Raised when a function or operation is applied to an object of incorrect type.
    ValueError,     	//Raised when a function gets argument of correct type but improper value.
    ZeroDivisionError   //Raised when second operand of division or modulo operation is zero.
};

std::ostream& operator<<(std::ostream& fout, CSLError error);

void CallErrorImpl(CSLError          error,
                   std::string_view func,
                   std::string_view file,
                   int              line);

void CallErrorImpl(CSLError          error,
                   std::string_view func,
                   std::string_view file,
                  int              line,
                   std::string_view message);

#define CSL_ASSERT(cond, error)\
    if (not (cond)) {\
        CallErrorImpl(error, __func__, __FILE__, __LINE__);\
    }
#define CSL_ASSERT_SPEC(cond, error, message)\
    if (not (cond)) {\
        CallErrorImpl(error, __func__, __FILE__, __LINE__, message);\
    }

#define CALL_SMERROR(error)\
    CallErrorImpl(error, __func__, __FILE__, __LINE__)

#define CALL_SMERROR_SPEC(error, message)\
    CallErrorImpl(error, __func__, __FILE__, __LINE__, message)


#endif
