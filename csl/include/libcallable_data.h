#pragma once

#include <iostream>

namespace csl {
    inline void printCallableStructure(
            std::ostream &out
            )
    {
        out << "template<class ReturnType, class ParamType>\n";
        out << "struct Callable {\n";
        out << "\n";
        out << "    Callable(ReturnType (*t_f)(ParamType const&))\n";
        out << "        :f(t_f)\n";
        out << "    {}\n";
        out << "    Callable(std::function<ReturnType(ParamType const&)> const &t_f)\n";
        out << "        :f(t_f)\n";
        out << "    {}\n";
        out << "\n";
        out << "    inline\n";
        out << "    ReturnType operator()(ParamType const &params) const {\n";
        out << "        return f(params);\n";
        out << "    }\n";
        out << "\n";
        out << "    std::function<ReturnType(ParamType)> f;\n";
        out << "};\n";
    }
}

