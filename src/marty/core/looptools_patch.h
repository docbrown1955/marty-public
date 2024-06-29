#ifndef MTY_LOOPTOOLS_PATCH_H_INCLUDED
#define MTY_LOOPTOOLS_PATCH_H_INCLUDED

#include "clooptools.h"
#include <complex.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
#define _MTY_INLINE inline
#else
#define _MTY_INLINE static inline
#endif

#if defined __cplusplus && defined QUAD
    using lti_real_t    = __float128;
    using lti_complex_t = __complex128;
    _MTY_INLINE auto lt_real(lti_complex_t const &z) { return crealq(z); }
    _MTY_INLINE auto lt_imag(lti_complex_t const &z) { return cimagq(z); }
    _MTY_INLINE auto lt_pow(lti_real_t const &x, int n) { return powq(x, n); }
    _MTY_INLINE ComplexType ensure_std_complex(lti_complex_t const &z) {
        return ToComplex2(crealq(z), cimagq(z));
    }
#elif defined __cplusplus
    using lti_real_t    = double;
    using lti_complex_t = std::complex<double>;
    _MTY_INLINE auto lt_real(lti_complex_t const &z) { return (z).real(); }
    _MTY_INLINE auto lt_imag(lti_complex_t const &z) { return (z).imag(); }
    _MTY_INLINE auto lt_pow(lti_real_t const &x, int n) { return std::pow(x, n); }
    _MTY_INLINE ComplexType ensure_std_complex(lti_complex_t const &z) {
        return ToComplex2(z.real(), z.imag());
    }
#elif defined QUAD
    typedef __float128 lti_real_t;
    typedef __complex128 lti_complex_t;
    _MTY_INLINE lti_real_t lt_real(lti_complex_t z) { return crealq(z); }
    _MTY_INLINE lti_real_t lt_imag(lti_complex_t z) { return cimagq(z); }
    _MTY_INLINE lti_real_t lt_pow(lti_real_t x, int n) { return powq(x, n); }
    _MTY_INLINE ComplexType ensure_std_complex(lti_complex_t z) { 
        return ToComplex2(crealq(z), cimagq(z)); 
    }
#else
    typedef double lti_real_t;
    typedef double complex lti_complex_t;
    _MTY_INLINE lti_real_t lt_real(lti_complex_t z) { return creal(z); }
    _MTY_INLINE lti_real_t lt_imag(lti_complex_t z) { return cimag(z); }
    _MTY_INLINE lti_real_t lt_pow(lti_real_t x, int n) { return pow(x, n); }
    _MTY_INLINE ComplexType ensure_std_complex(lti_complex_t z) { 
        return ToComplex2(creal(z), cimag(z)); 
    }
#endif

#ifdef __cplusplus
struct integration_params_t {
    lti_real_t reg_int;
    bool rescaleIntegral;
    lti_real_t minScale;
    lti_real_t regScale;
};
#else
typedef struct IntegrationParams {
    lti_real_t reg_int;
    bool rescaleIntegral;
    lti_real_t minScale;
    lti_real_t regScale;
} integration_params_t;
#endif

_MTY_INLINE integration_params_t* getIntegrationParameters()
{
    static bool init = false;
    static integration_params_t params;
    if (!init)
    {
        params.reg_int = 1e-8;
        params.rescaleIntegral = true;
        params.minScale = 1e-3;
        params.regScale = 1e0;
        init = true;
    }
    return &params;
}


_MTY_INLINE lti_real_t abs_re(lti_complex_t value) {
    const lti_real_t re = lt_real(value);
    return (re < 0) ? -re : re;
}

_MTY_INLINE void regulateC(
        lti_complex_t *p1_2,
        lti_complex_t *p2_2,
        lti_complex_t *p3_2,
        lti_complex_t m1_2,
        lti_complex_t m2_2,
        lti_complex_t m3_2,
        lti_real_t    eps
        )
{
    const bool p1Zero = (*p1_2 == 0.);
    const bool p3Zero = (*p3_2 == 0.);
    if (p1Zero && p3Zero && *p2_2 == 0.) {
        *p1_2 = 1e-5;
        return;
    }
    else if (*p1_2 == *p2_2 && !p1Zero) {
        *p2_2 = (1 + eps) * *p1_2;
        if (abs_re(m1_2/(*p1_2)) > 1e5 || abs_re(m2_2/(*p1_2)) > 1e5 || abs_re(m3_2/(*p1_2)) > 1e5)
            *p1_2 *= -1;
    }
    else if (*p1_2 == *p3_2 && !p1Zero) {
        *p3_2 = (1 + eps) * *p1_2;
        if (abs_re(m1_2/(*p2_2)) > 1e5 || abs_re(m2_2/(*p2_2)) > 1e5 || abs_re(m3_2/(*p2_2)) > 1e5)
            *p2_2 *= -1;
    }
    else if (*p3_2 == *p2_2 && !p3Zero) {
        *p3_2 = (1 + eps) * *p2_2;
        if (abs_re(m1_2/(*p3_2)) > 1e5 || abs_re(m2_2/(*p3_2)) > 1e5 || abs_re(m3_2/(*p3_2)) > 1e5)
            *p3_2 *= -1;
    }
}

_MTY_INLINE lti_real_t scaleFactorA(int id, lti_real_t scale)
{
    switch(id) {
        case aa0:
            return 1/scale;
        case aa00:
            return 1/(scale * scale);
        default:
            exit(1);
    }
}

_MTY_INLINE lti_real_t scaleFactorB(int id, lti_real_t scale)
{
    switch(id) {
        case bb00:
        case bb001:
        case dbb00:
        case dbb001:
            return 1/scale;
        default:
            return 1;
    }
}

_MTY_INLINE lti_real_t scaleFactorC(int id, lti_real_t scale)
{
    switch(id) {
        case cc00:
        case cc001:
        case cc002:
        case cc0011:
        case cc0012:
        case cc0022:
            return 1;

        case cc0000:
            return 1./scale;

        default:
            return scale;
    }
}

_MTY_INLINE lti_real_t scaleFactorD(int id, lti_real_t scale)
{
    if (id  == dd0000 || (id >= dd00001 && id <= dd00003))
        return 1;
    if (id == dd00
            || (id >= dd001 && id <= dd003)
            || (id >= dd0011 && id <= dd0033) 
            || (id >= dd00111 && id <= dd00333))
        return scale;
    return scale*scale;
}

_MTY_INLINE lti_real_t scaleFactorE(int id, lti_real_t scale)
{
    if (id  == ee0000)
        return scale;
    if (id == ee00
            || (id >= ee001 && id <= ee004)
            || (id >= ee0011 && id <= ee0044))
        return scale*scale;
    return scale*scale*scale;
}

_MTY_INLINE lti_complex_t rescaledC(
        lti_real_t scale,
        lti_real_t preFactor,
        lti_complex_t (*lt_func)(
            int,
            lti_complex_t,
            lti_complex_t,
            lti_complex_t,
            lti_complex_t,
            lti_complex_t,
            lti_complex_t,
            lti_real_t
        ),
        int              id,
        lti_real_t       eps,
        lti_complex_t p1,
        lti_complex_t p2,
        lti_complex_t p3,
        lti_complex_t m1,
        lti_complex_t m2,
        lti_complex_t m3
        )
{
    const lti_real_t mu = getmudim();
    setmudim(mu * scale);
    lti_complex_t res = preFactor*lt_func(
            id,
            p1 * scale,
            p2 * scale, 
            p3 * scale,
            m1 * scale,
            m2 * scale,
            m3 * scale,
            eps);
    setmudim(mu);
    return res;
}

#endif
