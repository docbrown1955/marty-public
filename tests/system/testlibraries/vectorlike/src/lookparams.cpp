#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "lookparams.h"

namespace vl {

  constexpr std::size_t string_length(char const *str)
  {
      return (sizeof(str) / sizeof(char)) - 1;
  }

  bool partialTokenComparison(
          std::string_view const &token,
          std::string_view const &partialToken
          )
  {
      if (token.size() < partialToken.size())
          return false;
      auto iterToken = token.begin();
      auto iterPartial = partialToken.begin();
      while (iterPartial != partialToken.end()) {
          if (*iterToken != *iterPartial)
              return false;
          ++iterToken;
          ++iterPartial;
      }
      return true;
  }

  std::vector<LibParam> getParamsFromLibrary(
          std::string const &libPath
          )
  {
      std::ifstream fin(libPath + "/include/params.h");
      if (!fin)
        return {};

      std::vector<LibParam> params;
      params.reserve(100);
      std::string line, token;
      constexpr auto triggerToken = "csl::InitSanitizer";
      while (!std::getline(fin, line).eof()) {
          std::istringstream linein(line);
          linein >> token;
          if (partialTokenComparison(token, triggerToken)) {
              linein >> token;
              params.push_back(token);
          } 
      }
      fin.close();
      return params;
  }

  std::vector<LibParam> getParamSetupFromLibrary(
          std::string const &fileName
          )
  {
      std::ifstream fin(fileName);
      if (!fin) {
          return {};
      }
      std::vector<LibParam> setupParams;
      setupParams.reserve(100);
      std::string line, token;
      constexpr auto triggerToken = "params.";
      constexpr std::size_t triggerLength = string_length(triggerToken);
      while (!std::getline(fin, line).eof()) {
          std::istringstream linein(line);
          linein >> token;
          if (partialTokenComparison(token, triggerToken)) {
              std::string param(
                  token.begin() + triggerLength, 
                  token.end()
              );
              linein >> token;
              if (token == "=") {
                  setupParams.push_back(param);
              }
          } 
      }
      fin.close();

      return setupParams;
  }

  std::vector<LibParam> getDifference(
          std::vector<LibParam> const &A,
          std::vector<LibParam> const &B,
          bool                         elementsInA
          )
  {
      auto A_eff = elementsInA ? A : B;
      auto const &B_eff = elementsInA ? B : A;
      for (const auto &toRemove : B_eff) {
          auto pos = std::find(A_eff.begin(), A_eff.end(), toRemove);
          if (pos != A_eff.end()) {
              A_eff.erase(pos);
          }
      }
      return A_eff;
  }

  static std::string getValue(
        std::string const &param,
        std::vector<std::pair<std::string, std::string>> const &values,
        std::string const &defaultValue = "1."
        )
  {
      auto pos = std::find_if(values.begin(), values.end(), 
            [&](std::pair<std::string, std::string> const &value) {
                return value.first == param;
            });
      return (pos == values.end()) ? defaultValue : pos->second;
  }

  static void commentParamDefinitions(
          std::string           const &fileName,
          std::vector<LibParam> const &params
          )
  {
      [[maybe_unused]] int sysres;
      for (const auto &param : params) {
          sysres = system(
              ("sed -i 's/params\\." + param + " = /"
               "\\/\\/ params\\." + param + " = /g' " + fileName).c_str()
              );
      }
  }

  static void addParamDefinitions(
          std::string fileName,
          std::vector<LibParam> const &params,
          std::vector<std::pair<std::string, std::string>> const &defaultValues,
          std::string const &indent
          )
  {
      std::string insertion;
      insertion.reserve(1000);
      for (const auto &param : params) {
          std::string value = getValue(param, defaultValues);
          insertion += indent + "params." + param + " = " + value + ";\\n";
      }
      [[maybe_unused]] int sysres = system(
            ("sed -i 's/\\#PARAMDEF/" + insertion + "/g' " + fileName).c_str()
            );
  }

  void setupLibrary(
          std::string const &libName,
          std::vector<std::string> const &masses,
          std::vector<std::pair<std::string, std::string>> const &defaultParams,
          std::string const &indent
          )
  {
      std::string lowerLibraryName = libName;
      for (char &c : lowerLibraryName) {
          c = std::tolower(c);
      }
      std::string programName = "example_" + lowerLibraryName + ".cpp";
      auto fullProgramPath = libName + "/script/" + programName;
          
      [[maybe_unused]] int sysres = system(
            ("cp generic/example_calc.cpp " + fullProgramPath).c_str()
            );
      sysres = system(
            ("sed -i 's/\\#LIBRARYNAME/" + lowerLibraryName + "/g' " 
            + fullProgramPath).c_str()
            );
      if (masses.size() == 1) {
        sysres = system(
              ("sed -i 's/\\#MASSPARAM/M/g' " 
              + fullProgramPath).c_str()
              );
      }
      else if (masses.size() == 2) {
        sysres = system(
              ("sed -i 's/\\#MASSPARAM/params." 
              + masses[0] + ", params." + masses[1] + "/g' " 
              + fullProgramPath).c_str()
              );
      }
      else {
          std::cerr << "Warning: Models that are composite of " << masses.size() 
            << " models are not supported." << std::endl;
      }
      setupParamsForLibrary(libName, programName, defaultParams, indent);
      setupVLQMasses(libName, programName, masses, indent);
  }
  
  void setupParamsForLibrary(
          std::string const &libPath,
          std::string const &programName,
          std::vector<std::pair<std::string, std::string>> const &defaultParams,
          std::string const &indent
          )
  {
      std::string fileName = libPath + "/script/" + programName;
      std::vector<LibParam> params = getParamsFromLibrary(libPath);
      std::vector<LibParam> definedParams = getParamSetupFromLibrary(fileName);
      std::vector<LibParam> toDefine = getDifference(
          params, definedParams, true);
      std::vector<LibParam> toRemove = getDifference(
          params, definedParams, false);

      addParamDefinitions(fileName, toDefine, defaultParams, indent);
      commentParamDefinitions(fileName, toRemove);
  }

  void setupVLQMasses(
          std::string const &libPath,
          std::string const &programName,
          std::vector<std::string> const &masses,
          std::string const &indent
          )
  {
      std::string fileName = libPath + "/script/" + programName;
      std::string insertion;
      insertion.reserve(100);
      for (const auto &mass : masses) {
          insertion += indent + indent + "params." + mass + " = M;\\n";
      }
      [[maybe_unused]] int sysres = system(
            ("sed -i 's/\\#MASSDEF/" + insertion + "/g' " + fileName).c_str()
            );
  }

}
