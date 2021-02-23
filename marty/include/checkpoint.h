#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <type_traits>
#include <memory>
#include <cxxabi.h>
#include "csl.h"
#include "fileData.h"
#include "mtyversion.h"
#include "csldatahandler.h"
#include "std_vector_implementation.h"

namespace mty::doc {

class FileData;

class CheckPoint_Base {

public:

    CheckPoint_Base(
            std::string_view t_name,
            std::string_view t_nameComparator = ""
            )
        :name(t_name),
        nameComparator(t_nameComparator)
    {}

    virtual ~CheckPoint_Base() {}

    std::string const &getName()           const { return name; }
    std::string const &getNameComparator() const { return nameComparator; }

    void print(FileData &fileData) const {
        printHeader(fileData);
        printSource(fileData);
    }
    virtual void printHeader(FileData &fileData) const = 0;
    virtual void printSource(FileData &fileData) const = 0;

    virtual bool hasCSLData() const { return false; }
    virtual void updateCSLData(CSLDataHandler &) {}

    template<class Type>
    static std::unique_ptr<char const> typeName()
    {
        [[maybe_unused]] int success;
        char const *typeName = abi::__cxa_demangle(
                typeid(std::remove_cv_t<std::remove_reference_t<Type>>).name(), 
                nullptr,
                nullptr,
                &success
                );
        return std::unique_ptr<char const>(typeName);
    }

protected:
    std::string name;
    std::string nameComparator;
};

template<class ValueType>
class CheckPoint_Implementation {

public:

    IMPLEMENTS_STD_VECTOR(ValueType, data)

    virtual ~CheckPoint_Implementation() {}

    void declareFunction(
            std::ostream    &out,
            std::string_view name,
            std::string_view indent,
            FileData::Mode   mode
            ) const 
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

template<class ValueType>
class CheckPoint;

template<>
class CheckPoint<csl::Expr>
    :public CheckPoint_Base, 
     public CheckPoint_Implementation<csl::Expr> 
{
public:

    CheckPoint(
            std::string_view t_name,
            std::string_view t_nameComparator = ""
            )
        :CheckPoint_Base(t_name, t_nameComparator),
        CheckPoint_Implementation<csl::Expr>()
    {}

    void printHeader(FileData &fileData) const override {
        auto &out = fileData.getStream(FileData::Header);
        declareFunction(out, name, "", FileData::Header);
    }
    void printSource(FileData &fileData) const override {
        auto &out = fileData.getStream(FileData::Source);
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

    bool hasCSLData() const override { return true; }
    void updateCSLData(CSLDataHandler &cslData) override {
        data = cslData.readSequence(data.begin(), data.end());
    }
};


}
