#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "marty.h"

constexpr auto resetfont   = "\033[0m";
constexpr auto errorfont   = "\033[1m\033[31m";
constexpr auto successfont = "\033[1m\033[32m";
constexpr auto testfont    = "\033[1m\033[34m";

void defineLibPath(mty::Library &lib)
{
#ifdef MARTY_LIBRARY_PATH
    lib.addLPath(MARTY_LIBRARY_PATH);
#endif
#ifdef MARTY_INCLUDE_PATH
    lib.addIPath(MARTY_INCLUDE_PATH);
#endif
}

template <class... Printable>
std::string buildMessage(Printable &&...printables)
{
    if constexpr (sizeof...(Printable) > 0) {
        std::ostringstream sout;
        (sout << ... << printables);
        return sout.str();
    }
    return "";
}
struct Result {

    template <class... Args>
    Result(bool t_success, Args &&...args) : success(t_success)
    {
        message = buildMessage(std::forward<Args>(args)...);
    }

    bool        success;
    std::string message;
};

class EnsureIO {
  public:
    EnsureIO()
    {
        disabled = std::cout.rdstate() == std::ios_base::failbit;
        std::cout.clear();
    }
    ~EnsureIO()
    {
        if (disabled)
            std::cout.setstate(std::ios_base::failbit);
    }

  private:
    bool disabled;
};

inline void success(std::string_view message)
{
    EnsureIO e;
    std::cout << successfont << "Success : " << resetfont << '\n';
    std::cout << message << '\n';
}

inline void error(std::string_view message)
{
    EnsureIO e;
    std::cout << errorfont << "Error : " << resetfont << '\n';
    std::cout << message << '\n';
}

template <class BaseCallable, class... Params>
int test(std::string_view message, BaseCallable &&callable, Params &&...params)
{
    const Result res = callable(std::forward<Params>(params)...);
    EnsureIO     e;
    std::cout << testfont << "Test : " << resetfont << message << '\n';
    if (res.success) {
        success(res.message);
        return 0;
    }
    else {
        error(res.message);
        return 1;
    }
}

inline int assert_binary(std::string const &libName)
{
    std::string lowerLibName = libName;
    for (char &c : lowerLibName) {
        c = tolower(c);
    }
    std::string binaryFileName
        = "libs/" + libName + "/bin/example_" + lowerLibName + ".x";
    std::ifstream file(binaryFileName);
    if (!file) {
        return 1;
    }
    return 0;
}
