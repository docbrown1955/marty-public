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
 * @file librarygenerator.h
 * @brief
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-11-05
 */
#ifndef CSL_LIBRARY_GENERATOR
#define CSL_LIBRARY_GENERATOR

#include "abstract.h"
#include "librarydependency.h"
#include "libraryfunction.h"
#include "librarygroup.h"
#include "literal.h"
#include <fstream>
#include <limits>

namespace csl {

// inline std::fstream lib_log("lib_debug_log.txt", std::ios::out);

class LibraryGenerator {

  public:
    struct file : public std::ofstream {
        file(std::string const &name) : std::ofstream(name)
        {
        }
        ~file()
        {
            close();
        }
    };

    inline static int nDigits = std::numeric_limits<double>::max_digits10 - 2;
    inline static std::string realType      = "double";
    inline static std::string crealType     = "double";
    inline static std::string complexType   = "std::complex<double>";
    inline static std::string ccomplexType  = "double _Complex";
    inline static std::string realUsing     = "real_t";
    inline static std::string complexUsing  = "complex_t";
    inline static std::string crealUsing    = "creal_t";
    inline static std::string ccomplexUsing = "ccomplex_t";
    inline static std::string tensorUsing   = "csl::LibraryTensor";
    inline static std::string indexType     = "int";
    inline static short       nSpaceIndent  = 4;
    inline static const Expr  imaginary     = csl::constant_s("_i_");
    // inline static std::string boostPath    = "../../Boost";

    inline static std::string incDir       = "include";
    inline static std::string srcDir       = "src";
    inline static std::string objDir       = "obj";
    inline static std::string binDir       = "bin";
    inline static std::string libDir       = "lib";
    inline static std::string clibDir      = "clib";
    inline static std::string scriptDir    = "script";
    inline static std::string scriptObjDir = "script/obj";

    static inline std::string toUpper(std::string const &name)
    {
        std::string copy(name);
        for (char &c : copy)
            c = toupper(c);
        return copy;
    }

    static std::string regularName(std::string_view name);

    static std::string indent(short nIndent = 1);

    static bool isQuadruplePrecision()
    {
        return quadruple;
    }
    static void setQuadruplePrecision(bool quadruple);

    struct DiagonalizationData {
        std::vector<std::string> mixings;
        std::vector<std::string> masses;
        std::vector<std::string> expressions;
        std::vector<std::string> dependencies;
        bool                     squaredMass;
    };

  private:
    static inline bool quadruple = false;

  public:
    LibraryGenerator(std::string const &t_name,
                     std::string const &t_path = ".");

    ~LibraryGenerator()
    {
    }

    std::string                   getName() const;
    std::string                   getPath() const;
    std::set<std::string> const  &getIPath() const;
    std::set<std::string>        &getIPath();
    std::set<std::string> const  &getLPath() const;
    std::set<std::string>        &getLPath();
    LibDependency                 getDependencies() const;
    std::shared_ptr<LibraryGroup> getGroup(std::string_view name) const;
    bool hasUniqueParamStruct() const
    {
        return uniqueParamStruct;
    }

    bool getOptimization() const
    {
        return optimize;
    }

    void cleanExistingSources() const;
    void setName(std::string const &t_name);
    void setUniqueParamStruct(bool t_par)
    {
        uniqueParamStruct = t_par;
    }
    void setOptimization(bool optimize)
    {
        this->optimize = optimize;
    }
    void setPath(std::string const &t_path);
    void setIPath(std::vector<std::string> const &t_path);
    void setLPath(std::vector<std::string> const &t_path);
    void addIPath(std::string const &t_path);
    void addLPath(std::string const &t_path);
    void addDependency(LibDependency const &dep);
    void addInclude(std::string const &include, bool global = false);
    void importHeader(std::string const &filePath,
                      std::string const &fileName);
    void importSource(std::string const &filePath,
                      std::string const &fileName);
    void addLibrary(std::string const &library);

    void addTensor(Expr const &tensor) const;

    void addGroup(std::string const &groupName, bool complexReturn = true);
    void addDefaultParameter(std::string const &name, bool isComplex);
    LibFunction &addFunction(std::string const &nameFunction,
                             Expr               expression,
                             bool               force_cpp_function = false);
    void         addMassExpression(std::string const &mass);
    void         addDiagonalization(std::vector<std::string> const &mixing,
                                    std::vector<std::string> const &masses,
                                    std::vector<std::string> const &expressions,
                                    std::vector<std::string> const &dependencies,
                                    bool                            squaredMass);

    void print() const;
    void setupDirectory() const;
    void printHeader() const;
    void printGlobal() const;
    void printSource() const;
    void printCallable() const;
    void printTest() const;

    void printDiagonalizationFacility(std::ostream &header,
                                      std::ostream &source) const;

    void printMassExpressionsFacility(std::ostream &header,
                                      std::ostream &source) const;

    void printMakefile() const;
    void printPythonDir() const;
    // void printCMakeLists() const;
    // void printGenerator() const;
    // void printPythonTest() const;

    void build(unsigned int nJobs = 1) const;

    void buildCppLib(unsigned int nJobs) const;
    // void buildPythonLib() const;

  private:
    void updateDiagonalization() const;

    bool needLibraryTensor() const;

    void printCppFunctions(LibraryGroup const &g) const;
    void printCFunction(LibFunction const &f) const;
    void printFunction(LibFunction const &f) const;
    void printGroup(LibraryGroup const &g) const;

    void printTensors(std::ostream &out) const;

    std::vector<csl::Tensor> gatherTensors() const;

    std::vector<std::string> getDiagonalizationMasses() const;
    std::vector<std::string> getDiagonalizationMixings() const;
    std::vector<std::string> getDiagonalizationDependencies() const;

    std::string regLibName() const;
    std::string getFunctionFileName(LibFunction const &f) const;
    std::string getGroupFileName(LibraryGroup const &g) const;
    // std::string getHeaders() const;
    // std::string getSources() const;

    std::vector<LibParameter> inputParams(
        std::vector<LibraryGenerator::DiagonalizationData> const &diagData)
        const;

    std::vector<LibParameter> outputParams(
        std::vector<LibraryGenerator::DiagonalizationData> const &diagData,
        std::vector<std::string> const &massExpressions) const;

  protected:
    std::string name;

    std::string path;

    bool optimize = true;

    std::set<std::string> IPath;

    std::set<std::string> LPath;

    LibDependency dependencies;

    std::vector<LibParameter> defaultParameters;

    mutable std::vector<csl::Tensor> tensors;

    mutable std::vector<std::shared_ptr<LibraryGroup>> groups;

    mutable std::vector<DiagonalizationData> diagData;

    mutable std::vector<std::string> massExpressions;

    bool uniqueParamStruct;
};

} // End of namespace csl

#endif
