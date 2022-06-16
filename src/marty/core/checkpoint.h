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
 * @file checkpoint.h
 * @brief Contains checkpoints for doc brown's debugger procedure.
 * @author Grégoire Uhlrich
 * @version 2.0
 * @date 2021-05-06
 */
#pragma once

#include "../../csl/csl.h"
#include "csldatahandler.h"
#include "fileData.h"
#include "mtyversion.h"
#include "std_vector_implementation.h"
#include <cxxabi.h>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace mty::doc {

class FileData;

class CheckPoint_Base {

  public:
    CheckPoint_Base(std::string_view t_name,
                    std::string_view t_nameComparator = "")
        : name(t_name), nameComparator(t_nameComparator)
    {
    }

    virtual ~CheckPoint_Base()
    {
    }

    std::string const &getName() const
    {
        return name;
    }
    std::string const &getNameComparator() const
    {
        return nameComparator;
    }

    void print(FileData &fileData) const
    {
        printHeader(fileData);
        printSource(fileData);
    }
    virtual void printHeader(FileData &fileData) const = 0;
    virtual void printSource(FileData &fileData) const = 0;

    virtual bool hasCSLData() const
    {
        return false;
    }
    virtual void updateCSLData(CSLDataHandler &)
    {
    }

    template <class Type>
    static std::unique_ptr<char const> typeName()
    {
        [[maybe_unused]] int success;
        char const *         typeName = abi::__cxa_demangle(
            typeid(std::remove_cv_t<std::remove_reference_t<Type>>).name(),
            nullptr,
            nullptr,
            &success);
        return std::unique_ptr<char const>(typeName);
    }

  protected:
    std::string name;
    std::string nameComparator;
};

template <class ValueType>
class CheckPoint_Implementation {

  public:
    IMPLEMENTS_STD_VECTOR(ValueType, data)

    virtual ~CheckPoint_Implementation()
    {
    }

    void declareFunction(std::ostream &   out,
                         std::string_view name,
                         std::string_view indent,
                         FileData::Mode   mode) const
    {
        out << indent << "std::vector<";
        out << CheckPoint_Base::typeName<ValueType>().get() << "> ";
        out << name << "()";
        if (mode == FileData::Header)
            out << ";\n";
        out << '\n';
    }

  protected:
    std::vector<ValueType> data;
};

template <class ValueType>
class CheckPoint;

template <>
class CheckPoint<csl::Expr> : public CheckPoint_Base,
                              public CheckPoint_Implementation<csl::Expr> {
  public:
    CheckPoint(std::string_view t_name, std::string_view t_nameComparator = "")
        : CheckPoint_Base(t_name, t_nameComparator),
          CheckPoint_Implementation<csl::Expr>()
    {
    }

    void printHeader(FileData &fileData) const override
    {
        auto &out = fileData.getStream(FileData::Header);
        declareFunction(out, name, "", FileData::Header);
    }
    void printSource(FileData &fileData) const override
    {
        auto &      out    = fileData.getStream(FileData::Source);
        auto const &indent = fileData.indent();
        declareFunction(out, name, "", FileData::Source);
        out << "{\n";
        out << indent << "std::vector<csl::Expr> res(" << size() << ");\n";
        out << indent << "auto iter = res.begin();\n\n";
        for (const auto &expr : *this) {
            out << indent << "*iter++ = ";
            expr->printCode(0, out);
            out << ";\n\n";
        }
        out << indent << "return res;\n";
        out << "}\n\n";
    }

    bool hasCSLData() const override
    {
        return true;
    }
    void updateCSLData(CSLDataHandler &cslData) override
    {
        data = cslData.readSequence(data.begin(), data.end());
    }
};

} // namespace mty::doc
