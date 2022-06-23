#pragma once

#include <string>
#include <string_view>

namespace vl {

  using LibParam = std::string;

  bool partialTokenComparison(
          std::string_view const &token,
          std::string_view const &partialToken
          );

  std::vector<LibParam> getParamsFromLibrary(
          std::string const &libPath
          );

  std::vector<LibParam> getParamSetupFromLibrary(
          std::string const &fileName
          );

  std::vector<LibParam> getDifference(
          std::vector<LibParam> const &A,
          std::vector<LibParam> const &B,
          bool elementsInA
          );

  void setupLibrary(
          std::string const &libName,
          std::vector<std::string> const &masses,
          std::vector<std::pair<std::string, std::string>> const &defaultParams = {},
          std::string const &indent = "    "
      );

  void setupParamsForLibrary(
          std::string const &libPath,
          std::string const &programName,
          std::vector<std::pair<std::string, std::string>> const &defaultParams = {},
          std::string const &indent = "    "
          );

  void setupVLQMasses(
          std::string const &libPath,
          std::string const &programName,
          std::vector<std::string> const &masses,
          std::string const &indent = "    "
          );

}
