#pragma once 

#include <sstream>
#include <iostream>

constexpr auto resetfont   = "\033[0m";
constexpr auto errorfont   = "\033[1m\033[31m";
constexpr auto successfont = "\033[1m\033[32m";
constexpr auto testfont    = "\033[1m\033[34m";

template<class ...Printable>
std::string buildMessage(
        Printable &&...printables
        )
{
    if constexpr (sizeof...(Printable) > 0) {
        std::ostringstream sout;
        ( sout << ... << printables );
        return sout.str();
    }
    return "";
}
struct Result {

    template<class ...Args>
    Result(bool t_success, Args &&...args)
        :success(t_success)
    {
        message = buildMessage(std::forward<Args>(args)...);
    }

    bool        success;
    std::string message;
};

class EnsureIO {
public:
    EnsureIO() {
        disabled = std::cout.rdstate() == std::ios_base::failbit;
        std::cout.clear();
    }
    ~EnsureIO() {
        if (disabled) std::cout.setstate(std::ios_base::failbit);
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

template<class BaseCallable, class ...Params>
int test(
        std::string_view message, 
        BaseCallable && callable,
        Params       && ...params
        )
{
    const Result res = callable(std::forward<Params>(params)...);
    EnsureIO e;
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

