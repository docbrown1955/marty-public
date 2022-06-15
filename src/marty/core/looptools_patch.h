#pragma once

#include "clooptools.h"
#include <iostream>

namespace mty::lt {

#ifdef QUAD
using lti_real_t    = __float128;
using lti_complex_t = __complex128;
inline auto lt_real(lti_complex_t const &z)
{
    return crealq(z);
}
inline auto lt_imag(lti_complex_t const &z)
{
    return cimagq(z);
}
inline auto lt_pow(lti_real_t const &x, int n)
{
    return powq(x, n);
}
inline std::complex<lti_real_t> ensure_std_complex(lti_complex_t const &z)
{
    return std::complex<lti_real_t>(lt_real(z), lt_imag(z));
}
#else
using lti_real_t    = double;
using lti_complex_t = std::complex<double>;
inline auto lt_real(lti_complex_t const &z)
{
    return (z).real();
}
inline auto lt_imag(lti_complex_t const &z)
{
    return (z).imag();
}
inline auto lt_pow(lti_real_t const &x, int n)
{
    return std::pow(x, n);
}
inline std::complex<lti_real_t> ensure_std_complex(lti_complex_t const &z)
{
    return z;
}
#endif

inline lti_real_t reg_int = 1e-8; // Default value, can be changed if needed

/**
 * @brief Applies a rescaling to any Looptools function before calling
 * it and returns the result.
 *
 * @details All the function parameters and the \f$ \mu \f$ dimension are
 * multiplied by the scale before the call. \f$ \mu \f$ is reset to its old
 * value at the end of the function. The pre-factor must be equal to
 * \f$ s^{d} \f$ for a scale factor \f$ s \f$ applied to an integral that
 * has an energy dimension \f$ d \f$.
 *
 * For a function \f$ f(\{m_i^2\}_i;\mu) \f$ depending on squared masses
 * \f$ m_i^2 \f$ and of energy dimension \f$ \left[f\right] \equiv d\f$,
 * this function returns for a given scale \f$ s \f$:
 * \f[
 *      s^d\cdot f(\{(sm_i)^2/\}; s\mu),
 * \f]
 * with \f$ s^d \f$ that must be user-provided in the ** preFactor **
 * parameter.
 *
 * @tparam ValueType     Type of number, adapted for double and quadruple.
 * @tparam LoopToolsFunc Looptools function type (or equivalent) to call.
 * @tparam ...Args       Types of function arguments.
 * @param scale          Scale to apply to the squared mass parameters.
 * @param preFactor      Pre-factor after the rescaling.
 * @param func           Function to call.
 * @param id             Looptools ID of the function.
 * @param args           Parameter pack of all function parameters given to
 * ** func **.
 *
 * @return The result of ** func ** with rescaled squared mass parameters.
 */
template <class ValueType, class LoopToolsFunc, class... Args>
auto rescaled(ValueType const &scale,
              ValueType const &preFactor,
              LoopToolsFunc && func,
              int              id,
              ValueType const &eps,
              Args &&... args)
{
    const auto mu = getmudim();
    setmudim(mu * scale);
    const auto res = preFactor * func(id, (args * scale)..., eps);
    setmudim(mu);
    return res;
}

inline lti_real_t abs_re(lti_complex_t const &value)
{
    const lti_real_t re = lt_real(value);
    return (re < 0) ? -re : re;
}

inline auto min_ct(lti_real_t const &first)
{
    return first;
}

template <class... Args>
auto min_ct(lti_real_t const &first, lti_real_t const &second, Args &&... args)
{
    const auto recmin = min_ct(second, std::forward<Args>(args)...);
    return (first < recmin) ? first : recmin;
}

template <class... Args>
auto abs_min_ct(Args &&... args)
{
    return min_ct(abs_re(args)...);
}

template <class ValueType, class EpsType>
void regulateC(ValueType &      p1_2,
               ValueType &      p2_2,
               ValueType &      p3_2,
               ValueType const &m1_2,
               ValueType const &m2_2,
               ValueType const &m3_2,
               EpsType const &  eps)
{
    const bool p1Zero = (p1_2 == 0.);
    const bool p3Zero = (p3_2 == 0.);
    if (p1Zero && p3Zero && p2_2 == 0.) {
        p1_2 = 1e-5;
        return;
    }
    else if (p1_2 == p2_2 && !p1Zero) {
        p2_2 = (1 + eps) * p1_2;
        if (abs_re(m1_2 / p1_2) > 1e5 || abs_re(m2_2 / p1_2) > 1e5
            || abs_re(m3_2 / p1_2) > 1e5)
            p1_2 *= -1;
    }
    else if (p1_2 == p3_2 && !p1Zero) {
        p3_2 = (1 + eps) * p1_2;
        if (abs_re(m1_2 / p2_2) > 1e5 || abs_re(m2_2 / p2_2) > 1e5
            || abs_re(m3_2 / p2_2) > 1e5)
            p2_2 *= -1;
    }
    else if (p3_2 == p2_2 && !p3Zero) {
        p3_2 = (1 + eps) * p2_2;
        if (abs_re(m1_2 / p3_2) > 1e5 || abs_re(m2_2 / p3_2) > 1e5
            || abs_re(m3_2 / p3_2) > 1e5)
            p3_2 *= -1;
    }
}

inline lti_real_t scaleFactorA(int id, lti_real_t const &scale)
{
    switch (id) {
    case aa0:
        return 1 / scale;
    case aa00:
        return 1 / (scale * scale);
    default:
        std::cerr << "Error: id " << id << " not recognized"
                  << " for integral A.\n ";
        exit(1);
    }
}

inline lti_real_t scaleFactorB(int id, lti_real_t const &scale)
{
    switch (id) {
    case bb00:
    case bb001:
    case dbb00:
    case dbb001:
        return 1 / scale;
    default:
        return 1;
    }
}

inline lti_real_t scaleFactorC(int id, lti_real_t const &scale)
{
    switch (id) {
    case cc00:
    case cc001:
    case cc002:
    case cc0011:
    case cc0012:
    case cc0022:
        return 1;

    case cc0000:
        return 1. / scale;

    default:
        return scale;
    }
}

inline lti_real_t scaleFactorD(int id, lti_real_t const &scale)
{
    if (id == dd0000 || (id >= dd00001 && id <= dd00003))
        return 1;
    if (id == dd00 || (id >= dd001 && id <= dd003)
        || (id >= dd0011 && id <= dd0033) || (id >= dd00111 && id <= dd00333))
        return scale;
    return scale * scale;
}

inline lti_real_t scaleFactorE(int id, lti_real_t const &scale)
{
    if (id == ee0000)
        return scale;
    if (id == ee00 || (id >= ee001 && id <= ee004)
        || (id >= ee0011 && id <= ee0044))
        return scale * scale;
    return scale * scale * scale;
}

} // namespace mty::lt
