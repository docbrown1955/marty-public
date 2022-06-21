#ifndef CSL_LIB_CALLABLE
#define CSL_LIB_CALLABLE
#include <string>
#include <functional>

namespace test_spectrum {

template<class ReturnType, class ParamType>
struct Callable {

    Callable(
            std::string_view t_name,
            ReturnType (*t_f)(ParamType const&)
            )
        :name(t_name),
         f(t_f)
    {}
    Callable(
            std::string_view t_name,
            std::function<ReturnType(ParamType const&)> const &t_f
            )
        :name(t_name),
         f(t_f)
    {}

    inline
    ReturnType operator()(ParamType const &params) const {
        return f(params);
    }

    std::string name;
    std::function<ReturnType(ParamType)> f;
};

} // End of namespace test_spectrum

#endif
