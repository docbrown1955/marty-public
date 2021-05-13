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

#include "librarygenerator.h"
#include "librarytensor.h"
#include "evaluation.h"
#include "interface.h"
#include "indicial.h"
#include "counter.h"
#include "space.h"
#include "error.h"
#include "utils.h"
#include "algo.h"

#include "libmakefile_data.h"
#include "libdiagonalization_hdata.h"
#include "libdiagonalization_cppdata.h"
#include "libcallable_data.h"
#include "libcomplexop_hdata.h"
#include "librarytensor_hdata.h"

#include <iomanip>
#include <fstream>
#include <list>

namespace csl {

    void LibraryGenerator::setQuadruplePrecision(bool t_quadruple)
    {
        quadruple = t_quadruple;
        if (quadruple) {
            realType     = "__float128";
            complexType  = "__complex128";
        }
        else {
            realType     = "double";
            complexType  = "std::complex<"+realType+">";
        }
    }

    LibraryGenerator::LibraryGenerator(
            std::string const& t_name,
            std::string const& t_path
            )
        :name(t_name),
        path(t_path + "/" + t_name),
        IPath({incDir}),
        uniqueParamStruct(true)
    {
        if (quadruple) {
            dependencies.addLib("-lquadmath");
            dependencies.addInclude("quadmath.h", true);
        }
        groups.push_back(std::make_shared<LibraryGroup>("G", true));
        // General, default group
    }

    std::string LibraryGenerator::getName() const
    {
        return name;
    }

    std::string LibraryGenerator::getPath() const
    {
        return path;
    }

    std::set<std::string> const& LibraryGenerator::getIPath() const
    {
        return IPath;
    }

    std::set<std::string>& LibraryGenerator::getIPath()
    {
        return IPath;
    }

    std::set<std::string> const& LibraryGenerator::getLPath() const
    {
        return LPath;
    }

    std::set<std::string>& LibraryGenerator::getLPath()
    {
        return LPath;
    }

    LibDependency LibraryGenerator::getDependencies() const
    {
        return dependencies;
    }

    std::shared_ptr<LibraryGroup> LibraryGenerator::getGroup(
            std::string_view t_name
            ) const
    {
        for (const auto &g : groups)
            if (g->getName() == t_name)
                return g;
        std::cerr << "Possible groups are: " << std::endl;
        for (const auto &g : groups)
            std::cerr << g->getName() << std::endl;
        std::cerr << "Consider creating a group using lib.addGroup(\"";
        std::cerr << "groupName\")." << std::endl;
        CALL_SMERROR_SPEC(
                CSLError::KeyError,
                "Group named \"" + std::string(t_name) + "\" not found"
                );
        return groups[0];
    }

    void LibraryGenerator::setName(std::string const& t_name)
    {
        name = t_name;
    }

    void LibraryGenerator::setPath(std::string const& t_path)
    {
        path = t_path;
    }

    void LibraryGenerator::setIPath(std::vector<std::string> const& t_path)
    {
        for (const auto& p : t_path)
            addIPath(p);
    }

    void LibraryGenerator::setLPath(std::vector<std::string> const& t_path)
    {
        for (const auto& p : t_path)
            addLPath(p);
    }

    void LibraryGenerator::addIPath(std::string const& t_path)
    {
        IPath.insert(t_path);
    }

    void LibraryGenerator::addLPath(std::string const& t_path)
    {
        LPath.insert(t_path);
    }

    void LibraryGenerator::addDependency(LibDependency const& dep)
    {
        dependencies += dep;
    }

    void LibraryGenerator::addInclude(std::string const& include,
                             bool               global)
    {
        dependencies.addInclude(include, global);
    }

    void LibraryGenerator::addLibrary(std::string const& library)
    {
        dependencies.addLib(library);
    }

    void LibraryGenerator::importHeader(
            std::string const &filePath,
            std::string const &fileName
            )
    {
        [[maybe_unused]] auto res =
            system(("cp " + filePath + "/" + fileName + " " 
                          + path + "/" + incDir)
                    .c_str());
        addInclude(fileName, false);
    }

    void LibraryGenerator::importSource(
            std::string const &filePath,
            std::string const &fileName
            )
    {
        [[maybe_unused]] auto res =
            system(("cp " + filePath + "/" + fileName + " " 
                          + path + "/" + srcDir)
                    .c_str());
    }

    void LibraryGenerator::addTensor(Expr const& tensor) const
    {
        CSL_ASSERT_SPEC(IsIndicialTensor(tensor),
                CSLError::TypeError,
                "Expression " + toString(tensor) + " should be a tensor.");
        csl::Tensor parent = std::dynamic_pointer_cast<TensorParent>(
                tensor->getParent());
        if (tensors.end() == std::find_if(tensors.begin(), tensors.end(), 
                    [&](csl::Tensor const& other)
                    {
                        return other.get() == parent.get();
                    }))
            tensors.push_back(parent);
    }

    void LibraryGenerator::addGroup(
            std::string const &groupName,
            bool               complexReturn
            )
    {
        for (const auto &g : groups) {
            if (g->getName() == groupName) {
                CALL_SMERROR_SPEC(
                        CSLError::NameError, 
                        "Group of name \"" + groupName + "\" already exists !");
            }
        }
        groups.push_back(std::make_shared<LibraryGroup>(groupName, complexReturn));
    }

    LibFunction &LibraryGenerator::addFunction(
            std::string const &nameFunction,
            Expr               expression,
            std::string const &nameGroup
            )
    {
        // lib_log << "Adding function \"" << nameFunction << "\".\n";
        // lib_log << "Expr = " << expression << std::endl;
        auto group = getGroup(nameGroup);
        dependencies += GetLibraryDependencies(expression);
        dependencies.removeInclude("complex");
        Expr expr = csl::DeepCopy(expression);
        csl::Evaluate(expr, // csl::eval::abbreviation |
                           csl::eval::literal
                         | csl::eval::numerical);
        auto f = LibFunction{nameFunction, expr, group};
        return group->addFunction(f);
    }

    void LibraryGenerator::addDiagonalization(
            std::vector<std::string> const &mixings,
            std::vector<std::string> const &masses,
            std::vector<std::string> const &expressions,
            std::vector<std::string> const &dependencies,
            bool                            squaredMass
            )
    {
        diagData.push_back(
                {mixings, masses, expressions, dependencies, squaredMass}
                );
    }

    void LibraryGenerator::cleanExistingSources() const
    {
        [[maybe_unused]] int sysres =
                 system(("rm " + path + "/" + srcDir + "/*").c_str());
        sysres = system(("rm " + path + "/" + incDir + "/*").c_str());
    }

    void LibraryGenerator::print() const
    {
        updateDiagonalization();
        setupDirectory();
        printSource();
        if (hasGlobalFile())
            printGlobal();
        if (uniqueParamStruct) {
            std::shared_ptr<LibraryGroup> dummy
                = std::make_shared<LibraryGroup>("", false);
            dummy->addFunction(LibFunction("dummy", CSL_0, dummy));
            dummy->getFunctions()[0].setParameters(
                    diagonalizationParameters(diagData));
            groups.push_back(dummy);
            LibraryGroup::gatherParameters(groups, "param_t");
            groups.erase(groups.end() - 1);
            file paramFile(path + "/" + incDir + "/params.h");
            paramFile << "#ifndef CSL_LIB_PARAM_H_INCLUDED\n",
            paramFile << "#define CSL_LIB_PARAM_H_INCLUDED\n\n";
            paramFile << "#include \"common.h\"\n";
            paramFile << '\n';
            paramFile << "namespace " << regLibName() << " {\n\n";
            dummy->print(paramFile, true);
            paramFile << "\n\n";
            paramFile << "}\n\n";
            paramFile << "#endif\n";
        }
        for (auto &g : groups) {
            if (!g->empty()) {
                if (!uniqueParamStruct)
                    g->gatherParameters();
                printGroup(*g);
            }
        }
        printHeader();
        printCallable();
        printTest();
        printMakefile();
        printPythonDir();
    }

    void LibraryGenerator::setupDirectory() const
    {
        [[maybe_unused]] int res = system(("mkdir -p " + path).c_str());
        res = system(("mkdir -p " + path + "/" + srcDir).c_str());
        res = system(("mkdir -p " + path + "/" + incDir).c_str());
        res = system(("mkdir -p " + path + "/" + binDir).c_str());
        res = system(("mkdir -p " + path + "/" + objDir).c_str());
        res = system(("mkdir -p " + path + "/" + scriptDir).c_str());
        res = system(("mkdir -p " + path + "/" + scriptObjDir).c_str());
        res = system(("mkdir -p " + path + "/" + libDir).c_str());
    }

    void LibraryGenerator::printGlobal() const
    {
        LibraryGenerator::file header(path + "/" + incDir + "/global.h");
        CSL_ASSERT_SPEC(
                header,
                CSLError::IOError,
                "File \"" + path + "/" + incDir + "/global.h\"" + " not found.");

        header << "#ifndef CSL_LIB_GLOBAL" << "\n";
        header << "#define CSL_LIB_GLOBAL" << "\n";
        if (uniqueParamStruct)
            header << "#include \"params.h\"\n";
        header << "#include \"common.h\"\n\n";

        header << "namespace " << regLibName() << " {\n\n";
        header << "struct SpectrumInput;\n";
        header << "struct SpectrumOutput;\n\n";
        header << "SpectrumOutput updateDiagonalization(SpectrumInput const&);\n\n";
        if (uniqueParamStruct)
            header << "void updateDiagonalization(param_t &params);\n\n";
        header << "////////////////////////////////////////////////////\n";
        header << "// Here are the parameters to set before calling    \n"
               << "// updateDiagonalization()                          \n";
        header << "////////////////////////////////////////////////////\n";
        auto dependencies = getDiagonalizationDependencies();
        header << "struct SpectrumInput {\n";
        for (const auto &dep : dependencies) {
            header << indent(1) << complexUsing << " " << 
                regularName(dep) << ";\n";
        }
        header << "};\n\n";
        header << "////////////////////////////////////////////////////\n";
        header << "// Here are the masses and mixings                 \n"
               << "// result of the diagonalization                   \n";
        header << "////////////////////////////////////////////////////\n";
        auto masses = getDiagonalizationMasses();
        header << "struct SpectrumOutput {\n";
        for (const auto &mass : masses) {
            header << indent(1) << realUsing << " " << regularName(mass) << ";\n";
        }
        header << '\n';
        auto mixings = getDiagonalizationMixings();
        for (const auto &mix : mixings) {
            header << indent(1) << complexUsing << " " << regularName(mix) << ";\n";
        }
        header << "};\n\n";
        header << "////////////////////////////////////////////////////\n";
        header << "// Here is a generic function to read results      \n"
               << "// of the diagonalization in a corresponding struct\n";
        header << "////////////////////////////////////////////////////\n";
        header << '\n';
        header << "template<class Type>\n";
        header << "void readDiagonalizationInputs(\n";
        header << indent(2) << "SpectrumInput &diagData,\n";
        header << indent(2) << "Type    const &input\n";
        header << indent(2) << ")\n";
        header << "{\n";
        for (const auto &dep : dependencies) {
            header << indent(1) << "diagData." << regularName(dep) << " = " 
                   << "input." << regularName(dep) << ";\n";
        }
        header << "}\n";
        header << '\n';
        header << "template<class Type>\n";
        header << "void readDiagonalizationOutputs(\n";
        header << indent(2) << "SpectrumOutput const &diagData,\n";
        header << indent(2) << "Type                 &output\n";
        header << indent(2) << ")\n";
        header << "{\n";
        for (const auto &mass : masses) {
            header << indent(1) << "output." << regularName(mass) << " = " 
                   << "diagData." << regularName(mass) << ";\n";
        }
        for (const auto &mix : mixings) {
            header << indent(1) << "output." << regularName(mix) << " = " 
                   << "diagData." << regularName(mix) << ";\n";
        }
        header << "}\n";
        header << '\n';
        header << "} // End of namespace " << regLibName() << "\n\n";
        header << "#endif\n";

        LibraryGenerator::file source(path + "/" + srcDir + "/global.cpp");
        source << "#include \"global.h\"\n";
        source << "#include \"libdiagonalization.h\"\n";
        source << "#include \"" << regLibName() << ".h\"\n";
        source << "#include \"libcomplexop.h\"\n\n";
        source << "namespace " << regLibName() << " {\n\n";
        source << '\n';
        if (uniqueParamStruct) {
            source << "void updateDiagonalization(param_t &params)\n";
            source << "{\n";
            source << indent(1) << "SpectrumInput  inputs;\n";
            source << indent(1) << "readDiagonalizationInputs (inputs,  params);\n";
            source << indent(1) << "SpectrumOutput outputs = updateDiagonalization(inputs);\n";
            source << indent(1) << "readDiagonalizationOutputs(outputs, params);\n";
            source << "}\n\n";
        }
        source << "SpectrumOutput updateDiagonalization(SpectrumInput const &inputs)\n";
        source << "{\n";
        for (const auto &dep : dependencies) {
            source << indent(1) << "auto const &" << 
                regularName(dep) << " = inputs." << regularName(dep) << ";\n";
        }
        source << '\n';
        source << indent(1) << "SpectrumOutput outputs;\n\n";
        for (const auto &diag : diagData) {
            source << indent(1);
            if (diag.masses.size()*diag.masses.size() == diag.mixings.size())
                source << "Diagonalizer::applyDiagonalization(\n";
            else
                source << "Diagonalizer::applyBiDiagonalization(\n";
            source << indent(2) << "{\n";
            for (const auto &expression : diag.expressions)
                source << indent(3) << expression << ",\n";
            source << indent(2) << "},\n";
            source << indent(2) << "{";
            size_t s1 = diag.masses.size()*diag.masses.size();
            CSL_ASSERT_SPEC(s1 <= diag.mixings.size(),
                    CSLError::RuntimeError,
                    "Too few mixings (" + toString(diag.mixings.size()) + ") "
                    "for the number of masses (" 
                    + toString(diag.masses.size()) + "), probably a name "
                    "conflict in the mixing matrix (be sure that all elements"
                    " have different names).")
            size_t s2 = diag.mixings.size();
            for (size_t i = 0; i != s1; ++i) 
                source << "&outputs." <<  diag.mixings[i] << ", ";
            source << "},\n";
            if (s2 != s1) {
                source << indent(2) << "{";
                CSL_ASSERT_SPEC(s2 <= diag.mixings.size(),
                        CSLError::RuntimeError,
                        "Too few mixings (" + toString(diag.mixings.size()) + ") "
                        "for the number of masses (" 
                        + toString(diag.masses.size()) + "), probably a name "
                        "conflict in the mixing matrix (be sure that all elements"
                        " have different names).")
                for (size_t i = s1; i != s2; ++i) 
                    source << "&outputs." << diag.mixings[i] << ", ";
                source << "},\n";
            }
            source << indent(2) << "{";
            for (const auto &mass : diag.masses)
                source << "&outputs." << mass << ", ";
            source << "}\n";
            source << indent(2) << ");\n";
            source << '\n';
            if (diag.squaredMass) {
                for (const auto &mass : diag.masses) {
                    source << "outputs." << mass << " = ";
                    source << ((quadruple) ? "sqrtq(" : "std::sqrt(");
                    source << "outputs." << mass << ");\n";
                }
            }
        }
        source << indent(1) << "return outputs;\n";
        source << "}\n\n";
        source << "} // End of namespace " << regLibName() << "\n\n";
    }

    void LibraryGenerator::printHeader() const
    {
        LibraryGenerator::file globalHeader(path + "/" + incDir + "/" + 
                regLibName() + ".h");
        CSL_ASSERT_SPEC(
                globalHeader,
                CSLError::IOError,
                "File \"" + path + "/" + incDir + "/" + regLibName() 
                + ".h\" not found.");
        globalHeader << "#ifndef CSL_LIB_" << name << "_GLOBAL_H_INCLUDED\n",
        globalHeader << "#define CSL_LIB_" << name << "_GLOBAL_H_INCLUDED\n\n";
        globalHeader << "#include \"common.h\"\n";
        globalHeader << "#include \"librarytensor.h\"\n";
        if (hasGlobalFile()) {
            globalHeader << "#include \"global.h\"\n";
            globalHeader << "#include \"libdiagonalization.h\"\n";
        }
        globalHeader << "#include \"libcomplexop.h\"\n";
        globalHeader << '\n';
        for (const auto &g : groups) {
            if (!g->empty())
                globalHeader << "#include \"group_" << getGroupFileName(*g) 
                    << ".h\"\n";
        }
        globalHeader << '\n';
        globalHeader << "#endif\n";
        LibraryGenerator::file header(path + "/" + incDir + "/common.h");
        CSL_ASSERT_SPEC(
                header,
                CSLError::IOError,
                "File \"" + path + "/" + incDir + "/common.h\" not found.");

        header << "#ifndef CSL_LIB_" << name << "_COMMON_H_INCLUDED\n";
        header << "#define CSL_LIB_" << name << "_COMMON_H_INCLUDED\n";
        header << "#include <complex>\n";
        if (quadruple)
            header << "#include <quadmath.h>\n";
        if (needLibraryTensor()) {
            header << "#define CSL_LT_DISABLE_ITERATOR\n";
            header << "#include \"librarytensor.h\"\n";
        }
        header << '\n';
        header << "namespace " << regLibName() << " {\n\n";
        header << "using " << realUsing << " = " << realType << ";\n";
        header << "using " << complexUsing << " = " << complexType << ";\n";
        header << "static constexpr " << complexUsing << " _i_{0, 1};\n\n"; 
        if (dependencies.containsInclude("clooptools.h")) {
            header << "void setMu(const double mu);\n";
            header << "void setLambda2(const double lambda2);\n";
            header << "void setUVDiv(const double x);\n\n";
        }
        printTensors(header);
        header << "} // End of namespace " << regLibName() << "\n\n";
        header << "#endif\n";
    }

    void LibraryGenerator::printSource() const
    {
        LibraryGenerator::file source(path + "/" + srcDir + "/common.cpp");
        CSL_ASSERT_SPEC(source,
                CSLError::IOError,
                "File \"" + path + "/" + srcDir + "/common.cpp\"" 
                + " not found.");
        dependencies.printInclude(source);
        source << "#include \"common.h\"\n\n";
        source << "namespace " << regLibName() << " {\n\n";
        if (dependencies.containsInclude("clooptools.h")) {
            source << "void setMu(const double mu)\n"
                << "{\n"
                << indent() << "setmudim(mu * mu);\n"
                << "}\n\n";
            source << "void setLambda2(const double lambda2)\n"
                << "{\n"
                << indent() << "setlambda(lambda2);\n"
                << "}\n\n";
            source << "void setUVDiv(const double x)\n"
                << "{\n"
                << indent() << "setuvdiv(x);\n"
                << "}\n\n";
        }
        source << "} // End of namespace " << regLibName() << "\n\n";
    }

    void LibraryGenerator::printCallable() const
    {
        LibraryGenerator::file header(path + "/" + incDir + "/callable.h");
        CSL_ASSERT_SPEC(
                header,
                CSLError::IOError,
                "File \"" + path + "/" + incDir + "/common.h\" not found.");
        header << "#ifndef CSL_LIB_CALLABLE" << "\n";
        header << "#define CSL_LIB_CALLABLE" << "\n";
        header << "#include <string>\n";
        header << "#include <functional>\n\n";

        header << "namespace " << regLibName() << " {\n\n";
        printCallableStructure(header);
        header << '\n';
        header << "} // End of namespace " << regLibName() << "\n\n";
        header << "#endif\n";
    }

    void LibraryGenerator::printTest() const
    {
        std::string testName = path + "/" + scriptDir + "/example_" 
            + regLibName() + ".cpp";
        if (auto file = std::ifstream(testName); file) {
            file.close();
            return;
        }
        LibraryGenerator::file test(testName);
        test << "#include \"" << regLibName() << ".h" << "\"\n\n";
        test << "using namespace " << regLibName() << ";\n\n";
        test << "int main() {\n\n";
        test << indent() << "return 0;\n}";
    }

    void LibraryGenerator::printMakefile() const
    {
        std::string makeName = path  + "/Makefile";
        std::ofstream makefile(makeName);
        makefile << "# Part depending on the generated library\n\n";
        makefile << "NAME    = " << name << '\n';
        std::string libName = "lib";
        for (const char c : name)
            if (c >= 'A' and c <= 'Z') 
                libName += c + 'a' - 'A';
            else
                libName += c;
        makefile << "NAMELIB = " << libName << "\n\n";
        makefile << "INCPATH = ";
        for (const auto& path : IPath)
            makefile << "-I" << path << " ";
        makefile << "\nLIBPATH = ";
        for (const auto& path : LPath)
            makefile << "-L" << path << " ";
        makefile << "\nLIBS    = ";
        dependencies.printLib(makefile, compiler == clang);
        if (hasGlobalFile())
            makefile << " -lgsl -lgslcblas";
        makefile << "\n\n";
        print_libmakefile_data(makefile, quadruple, compiler == clang);
	      makefile.close();
    }

    void LibraryGenerator::printPythonDir() const
    {
        // std::string pythonPath = path + "/python";
          std::ofstream file;
          if (hasGlobalFile()) {
              file.open(path + "/" + incDir + "/libdiagonalization.h");
              print_libdiagonalization_hdata(file);
              file.close();
              file.open(path + "/" + srcDir + "/libdiagonalization.cpp");
              print_libdiagonalization_cppdata(file);
              file.close();
          }
          file.open(path + "/" + incDir + "/libcomplexop.h");
          print_libcomplexop_hdata(file);
          file.close();
          file.open(path + "/" + incDir + "/librarytensor.h");
          print_librarytensor_hdata(file);
          file.close();
        // system(("mkdir -p " + pythonPath).c_str());
        // printCMakeLists();
        // printGenerator();
        // printPythonTest();
    }

    // void LibraryGenerator::printCMakeLists() const
    // {
    //     std::string cmakelistsName = path + "/CMakeLists.txt";
    //     LibraryGenerator::file CMakeLists(cmakelistsName);
    //     CMakeLists << "cmake_minimum_required(VERSION 2.8)\n";
    //     CMakeLists << "include(ExternalProject)\n";

    //     CMakeLists << "# Ajout d'un module cmake personnalisé "
    //                << "pour générer boost\n";
    //     CMakeLists << "list(APPEND CMAKE_MODULE_PATH  "
    //                << "\"${CMAKE_SOURCE_DIR}/../../Boost\")\n";

    //     CMakeLists << "project (auto_lib)\n";

    //     CMakeLists << "# Trouver les bibliothèques et interpréteurs python"
    //                << " par défaut\n";
    //     CMakeLists << "find_package(PythonInterp REQUIRED)\n";
    //     CMakeLists << "find_package(PythonLibs REQUIRED)\n";
    //     CMakeLists << "include(BuildBoost) # module personnalisé\n";

    //     CMakeLists << "include_directories(${Boost_INCLUDE_DIR}\n";
    //     CMakeLists << "${PYTHON_INCLUDE_DIRS} \n";
    //     for (const auto& path : IPath)
    //         CMakeLists << path << '\n';
    //     CMakeLists << ")\n";
    //     CMakeLists << "link_directories(${Boost_LIBRARY_DIR} \n";
    //     for (const auto& path : LPath)
    //         CMakeLists << path << '\n';
    //     CMakeLists << ")\n";
    //     CMakeLists << "# Génère et lie le module " << name << "\n";
    //     CMakeLists << "add_library(" << name << " SHARED " 
    //         << getSources() << ")\n";
    //     CMakeLists << "target_link_libraries(" << name << " \n"
    //                << "                      ${Boost_LIBRARIES}\n"
    //                << "                      ${PYTHON_LIBRARIES}\n"
    //                << "                      -lstdc++ -lm -lgsl -lgslcblas\n";
    //     dependencies.printLib(CMakeLists);
    //     CMakeLists << "                      )\n";
    //     CMakeLists << "add_dependencies(" << name << " Boost)\n";

    //     CMakeLists << "# Ajuste le nom de la bibliothèque pour coller à ce "
    //                << "qu'attend Python\n";
    //     CMakeLists << "set_target_properties(" << name 
    //                << " PROPERTIES SUFFIX .so)\n";
    //     CMakeLists << "set_target_properties(" << name 
    //                << " PROPERTIES PREFIX \"\")\n";
    //     CMakeLists << "set(CMAKE_CXX_FLAGS \"-O3 -std=c++17"
    //                << " -Wno-deprecated-declarations -fPIC\")\n";
    // }

    std::string LibraryGenerator::regLibName() const
    {
        std::string regName(name);
        for (auto &c : regName)
            if (c >= 'A' and c <= 'Z')
                c += 'a' - 'A';
        return regName;
    }

    std::string LibraryGenerator::getFunctionFileName(LibFunction const &f) const
    {
        return f.getName();
    }

    std::string LibraryGenerator::getGroupFileName(
            LibraryGroup const &g
            ) const
    {
        std::string res;
        for (const char c : g.getName())
            if (c >= 'A' and c <= 'Z')
                res += c + 'a' - 'A';
            else
                res += c;
        return res;
    }

    // std::string LibraryGenerator::getHeaders() const
    // {
    //     std::ostringstream sout;
    //     sout << "\"" << incDir << "/librarytensor.h\",\n";
    //     sout << "\"" << incDir << "/libcomplexop.h\",\n";
    //     sout << "\"" << incDir << "/common.h\",\n";
    //     if (hasGlobalFile()) {
    //         sout << "\"" << incDir << "/global.h\",\n";
    //         sout << "\"" << incDir << "/libdiagonalization.h\",\n";
    //     }
    //     for (const auto &f : functions) {
    //         sout << "\"" << incDir << "/" << getFunctionFileName(f) 
    //              << ".h\",\n";
    //     }
    //     return sout.str();
    // }

    // std::string LibraryGenerator::getSources() const
    // {
    //     std::ostringstream sout;
    //     sout << name << "_pylink.cpp\n";
    //     sout << srcDir << "/common.cpp\n";
    //     if (hasGlobalFile()) {
    //         sout << srcDir << "/libdiagonalization.cpp\n";
    //         sout << srcDir << "/global.cpp\n";
    //     }
    //     for (const auto &f : functions)
    //         sout << srcDir << "/" << getFunctionFileName(f) 
    //              << ".cpp\n";

    //     return sout.str();
    // }

    // void LibraryGenerator::printGenerator() const
    // {
    //     LibraryGenerator::file generator(path + "/generator.py");
    //     generator << "#!/usr/bin/python\n";
    //     generator << "# -*- coding: utf-8 -*-\n";
    //     generator << "\n";
    //     generator << "from pygccxml import parser\n";
    //     generator << "from pyplusplus import module_builder\n";
    //     generator << "\n";
    //     generator << "# Configurations que vous pouvez avoir à "
    //               << "changer sur votre système\n";
    //     generator << "generator_path = \"/usr/bin/castxml\"\n";
    //     generator << "generator_name = \"castxml\"\n";
    //     generator << "compiler = \"gnu\"\n";
    //     generator << "compiler_path = \"/usr/bin/gcc\" \n";
    //     generator << "\n";
    //     generator << "# Créé une configuration pour CastXML\n";
    //     generator << "xml_generator_config = "
    //               << "parser.xml_generator_configuration_t(\n";
    //     generator << "xml_generator_path=generator_path,\n";
    //     generator << "xml_generator=generator_name,\n";
    //     generator << "compiler=compiler,\n";
    //     generator << "compiler_path=compiler_path)\n";
    //     generator << "\n";
    //     generator << "# Liste de tous les fichiers d'en-tête "
    //               << "de votre bibliothèque\n";
    //     generator << "header_collection = [" << getHeaders() << "]\n";
    //     generator << "\n";
    //     generator << "# Analyse les fichiers sources et créé un objet "
    //               << "module_builder\n";
    //     generator << "builder = module_builder.module_builder_t(\n";
    //     generator << "header_collection,\n";
    //     generator << "xml_generator_path=generator_path,\n";
    //     generator << "xml_generator_config=xml_generator_config)\n";
    //     generator << "\n";
    //     generator << "# Détecte automatiquement les propriétés et les "
    //               << "accesseurs/mutateurs associés\n";
    //     generator << "builder.classes().add_properties(exclude_accessors=True) \n";
    //     generator << "\n";
    //     generator << "# Définit un nom pour le module\n";
    //     generator << "builder.build_code_creator(module_name=\"" << name
    //               << "\")\n";
    //     generator << "\n";
    //     generator << "# Écrit le fichier d'interface C++\n";
    //     generator << "builder.write_module('" << name 
    //         << "_pylink.cpp')\n";
    // }

    // void LibraryGenerator::printPythonTest() const
    // {
    //     std::string fileName = path + "/python/" + name + ".py";
    //     if (auto file = std::ifstream(fileName); file) {
    //         file.close();
    //         return;
    //     }
    //     LibraryGenerator::file test(fileName);
    //     test << "#!/usr/bin/python\n";
    //     test << "# -*- coding: utf-8 -*-\n";
    //     test << "\n";
    //     test << "import numpy as np\n";
    //     test << "import matplotlib.pyplot as plt\n";
    //     test << "import " << name << "\n";
    //     test << '\n';
    //     test << "if __name__ == '__main__':";
    // }

    void LibraryGenerator::updateDiagonalization() const
    {
        // Here cut identical masses and mixings
        auto cutSimilar = [](std::vector<std::string> &vec) {
            std::sort(vec.begin(), vec.end());
            auto last = std::unique(vec.begin(), vec.end());
            vec.erase(last, vec.end());
        };
        for (auto &diag : diagData) {
            cutSimilar(diag.mixings);
            cutSimilar(diag.masses);
            cutSimilar(diag.dependencies);
        }
        // for (auto &f : functions) {
        //     for (const auto &diag : diagData) {
        //         for (const auto &name : diag.mixings)
        //             f.removeParameter(name);
        //         for (const auto &name : diag.masses)
        //             f.removeParameter(name);
        //         for (const auto &name : diag.dependencies)
        //             f.removeParameter(name);
        //     }
        // }
    }

    bool LibraryGenerator::needLibraryTensor() const
    {
        return true;
        // for (const auto& f : functions)
        //     if (f.isTensorial())
        //         return true;
        // return false;
    }

    void LibraryGenerator::printGroup(LibraryGroup const &g) const
    {
        std::string nameHeader = "group_";
        std::string nameSource = nameHeader;
        for (const char c : g.getName())
            if (c >= 'A' and c <= 'Z')
                nameHeader += c + 'a' - 'A';
            else
                nameHeader += c;
        nameSource = nameHeader;
        nameHeader += ".h";
        nameSource += ".cpp";

        file header(path + "/" + incDir + "/" + nameHeader);
        header << "#ifndef CSL_LIB_" << name << "_" << g.getName() 
            << "_H_INCLUDED\n";
        header << "#define CSL_LIB_" << name << "_" << g.getName() 
            << "_H_INCLUDED\n\n";
        header << "#include <array>\n";
        header << "#include \"common.h\"\n";
        header << "#include \"librarytensor.h\"\n";
        header << "#include \"callable.h\"\n";
        if (uniqueParamStruct)
            header << "#include \"params.h\"\n";
        for (const auto &f : g.getFunctions())
            header << "#include \"" << getFunctionFileName(f) << ".h\"\n";
        header << '\n';
        header << "namespace " << regLibName() << " {\n\n";
        if (!uniqueParamStruct)
            g.print(header, true);
        header << '\n';
        g.printFunctionStack(header, 0);
        header << '\n';
        header << "}\n // End of namespace " << regLibName() << "\n\n";
        header << "#endif\n";

        file source(path + "/" + srcDir + "/" + nameSource);
        source << "#include \"" << nameHeader << "\"\n";
        source << "namespace " << regLibName() << " {\n\n";
        g.print(source, false);
        source << "\n} // End of namespace " << regLibName() << "\n";

        for (const auto &f : g.getFunctions())
            printFunction(f);
    }

    void LibraryGenerator::printFunction(LibFunction const& f) const
    {
        std::string nameHeader = f.getName();
        std::string nameSource = nameHeader;
        nameHeader += ".h";
        nameSource += ".cpp";

        file header(path + "/" + incDir + "/" + nameHeader);
        header << "#ifndef CSL_LIB_" << name << "_" << f.getName() 
            << "_H_INCLUDED\n";
        header << "#define CSL_LIB_" << name << "_" << f.getName() 
            << "_H_INCLUDED\n\n";
        header << "#include \"common.h\"\n";
        header << "#include \"librarytensor.h\"\n\n";
        header << "namespace " << regLibName() << " {\n\n";
        f.print(header, true);
        header << "}\n // End of namespace " << regLibName() << "\n\n";
        header << "#endif\n";

        file source(path + "/" + srcDir + "/" + nameSource);
        std::string initInstruction;
        // if (hasGlobalFile()) {
        //     initInstruction = 
        //         indent(1) + "if (!parametersInitialized) {\n"
        //         + indent(2) + "std::cerr << \"Error: diagonalization "
        //         "parameters are not initialized.\"\n"
        //         + indent(2) + "             \"Consider setting initial mass "
        //         "parameters value and\"\n"
        //         + indent(2) + "             \"call updateDiagonalization() "
        //         "(see file global.h).\\n\";\n"
        //         + indent(2) + "exit(1);\n"
        //         + indent(1) + "}\n";
        // }
        // initInstruction += indent(1) + "clearcache();\n";
        dependencies.printInclude(source);
        source << "#include \"" << nameHeader << "\"\n";
        source << "#include \"common.h\"\n\n";
        if (uniqueParamStruct)
            source << "#include \"params.h\"\n";
        source << "#include \"" << "group_" << getGroupFileName(f.getGroup())
            << ".h\"\n\n";
        if (hasGlobalFile()) {
            source << "#include \"global.h\"\n";
        }
        source << "#include \"libcomplexop.h\"\n\n";
        source << "namespace " << regLibName() << " {\n\n";
        f.print(source, false, initInstruction);
        source << "} // End of namespace " << regLibName() << "\n";
    }

    void LibraryGenerator::printTensors(std::ostream& out) const
    {
        auto isEvaluated = [&](csl::Tensor const &t)
        {
            return csl::AllOfLeafs(t->getTensor(), [&](Expr const &el)
            {
                return el->getPrimaryType() 
                    == csl::PrimaryType::Numerical;
            });
        };
        std::vector<csl::Tensor> tensors = gatherTensors();
        for (const auto& tensor : tensors) {
            std::string name = regularName(tensor->getName());
            if (tensor->getType() == cslParent::Metric
                    or tensor->getType() == cslParent::Delta
                    or tensor->getType() == cslParent::Epsilon)
                name += "_" + regularName(tensor->getSpace()[0]->getName());
            out << "static ";
            bool isEval = isEvaluated(tensor);
            if (isEval)
                out << "const ";
            out << tensorUsing << "<";
            if (isEval)
                out << complexUsing;
            else
                out << realUsing;
            out << "> \n" << indent(1) << name << "(\n";
            out << indent(2) << "{";
            std::vector<const csl::Space*> spaces = tensor->getSpace();
            for (size_t i = 0; i != spaces.size(); ++i) {
                out << spaces[i]->getDim();
                if (i != spaces.size() - 1)
                    out << ", ";
            }
            if (not isEvaluated(tensor)) {
                out << "}, 0\n" << indent(2) << ");\n";
                continue;
            }
            out << "},\n";
            out << indent(2) << "{\n";
            Expr expr = tensor->getTensor();
            CSL_ASSERT_SPEC(expr != CSL_0,
                    CSLError::TypeError,
                    "Tensor " + std::string(tensor->getName()) 
                    + " not allowed in library generation, it has no value.")
            std::list<size_t> maxima;
            size_t max_iter = 1;
            for (auto& s : spaces) {
                maxima.push_back(s->getDim());
                max_iter *= s->getDim();
            }

            counter<size_t> index_counter(maxima);
            index_counter.restart();
            std::vector<int> indices(spaces.size());
            size_t iter = 0;
            size_t periodicity = spaces[spaces.size()-1]->getDim();
            out.precision(nDigits);
            out << indent(3);
            out << std::left;
            do {
                index_counter.getCount(indices.rbegin());
                long double real = GetRealPart(expr->getArgument(indices))
                    ->evaluateScalar();
                long double imag = GetImaginaryPart(expr->getArgument(indices))
                    ->evaluateScalar();
                if (real != 0) {
                    out << real;
                    if (real == std::round(real))
                        out << '.';
                }
                if (imag != 0) {
                    out << (real == 0 ? "" : " + ") << imag;
                    if (imag == std::round(imag))
                        out << '.';
                    out << "*_i_";
                }
                else if (real == 0)
                    out << 0;
                if (iter != max_iter - 1)
                    out << ",";
                if ((1 + iter) % periodicity == 0) {
                    out << "\n";
                    if (iter != max_iter - 1)
                        out << indent(3);
                }
                else
                    out << " ";
                ++iter;
            } while (++index_counter);
            out << indent(2) << "});\n\n";
        }
    }

    void LibraryGenerator::build(unsigned int nJobs) const
    {
        CSL_ASSERT_SPEC(
                nJobs > 0, 
                CSLError::ValueError, 
                "Please give at least 1 job for compilation !")
        print();
        buildCppLib(nJobs);
        // buildPythonLib();
    }

    void LibraryGenerator::buildCppLib(unsigned int nJobs) const
    {
        [[maybe_unused]] int res = system(("cd " + path + " ; make -j "
                    + toString(nJobs) + " all").c_str());
    }

    // void LibraryGenerator::buildPythonLib() const
    // {
    //     [[maybe_unused]] int res = system(("cd " + path + ";"
    //             + " python generator.py; "
    //             + " cd python; "
    //             + " cmake ..;"
    //             + " make all; ").c_str());
    // }

    std::vector<csl::Tensor> LibraryGenerator::gatherTensors() const
    {
        auto find = [&](std::vector<csl::Tensor> const &tensors,
                        csl::Tensor              const &t)
        {
            auto pos = std::find_if(
                    tensors.begin(), 
                    tensors.end(),
                    [&](csl::Tensor const &t2)
                    {
                        return t2 == t or t2->getName() == t->getName();
                    });
            return pos != tensors.end();
        };
        std::vector<csl::Tensor> tensors;
        for (const auto &g : groups) {
            if (g->empty())
                continue;
            for (const auto& f : g->getFunctions()) {
                const std::vector<csl::Tensor>& f_tensors = f.getTensors();
                for (const csl::Tensor& t : f_tensors)
                    if (!find(tensors, t))
                        tensors.push_back(t);
            }
        }
        std::sort(
                tensors.begin(),
                tensors.end(),
                [&](csl::Tensor const &A, csl::Tensor const &B)
                {
                    return A->getName() < B->getName();
                });

        return tensors;
    }

    std::vector<std::string> LibraryGenerator::getDiagonalizationMasses() const
    {
        std::vector<std::string> res;
        for (const auto &diag : diagData)
            res.insert(
                    res.end(),
                    diag.masses.begin(),
                    diag.masses.end()
                    );
        std::sort(res.begin(), res.end());
        auto last = std::unique(res.begin(), res.end());
        res.erase(last, res.end());

        return res;
    }

    std::vector<std::string> LibraryGenerator::getDiagonalizationMixings() const
    {
        std::vector<std::string> res;
        for (const auto &diag : diagData)
            res.insert(
                    res.end(),
                    diag.mixings.begin(),
                    diag.mixings.end()
                    );
        std::sort(res.begin(), res.end());
        auto last = std::unique(res.begin(), res.end());
        res.erase(last, res.end());

        return res;
    }

    std::vector<std::string> LibraryGenerator::getDiagonalizationDependencies() const
    {
        std::vector<std::string> res;
        for (const auto &diag : diagData)
            res.insert(
                    res.end(),
                    diag.dependencies.begin(),
                    diag.dependencies.end()
                    );
        std::sort(res.begin(), res.end());
        auto last = std::unique(res.begin(), res.end());
        res.erase(last, res.end());
        if (res.size() > 0 and res[0].empty())
            res.erase(res.begin());

        return res;
    }

    std::string LibraryGenerator::regularName(std::string_view t_name)
    {
        auto isLetter = [](char c)
        {
            return ((c >= 'a') and (c <= 'z'))
                or ((c >= 'A') and (c <= 'Z'));
        };
        std::string name;
        bool first = true;
        for (char c : t_name) {
            if (c == '_' or isLetter(c)) {
                first = false;
                name += c;
                continue;
            }
            if (first) {
                first = false;
                continue;
            }
            if ((c < '0') or (c > '9')) {
                continue;
            }
            name += c;
        }

        return name;
    }

    std::string LibraryGenerator::indent(short nIndent)
    {
        return std::string(nIndent * nSpaceIndent, ' ');
    }

    std::vector<LibParameter> diagonalizationParameters(
            std::vector<LibraryGenerator::DiagonalizationData> const &diagData
            )
    {
        std::vector<LibParameter> res;
        res.reserve(4 * diagData.size());
        for (const auto &data : diagData) {
            for (const auto &mass : data.masses)
                res.push_back({mass, "real_t"});
            for (const auto &mix : data.mixings)
                res.push_back({mix, "complex_t"});
            for (const auto &dep : data.dependencies)
                res.push_back({dep, "complex_t"});
        }
        return res;
    }

} // End of namespace csl
