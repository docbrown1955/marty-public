#pragma once

#include "looptools_patch.h"

namespace mty::lt {

inline bool       rescaleIntegral = false;
inline lti_real_t minScale = 1e-3;
inline lti_real_t regScale = 1e+0;

inline lti_complex_t A0iC(
        int id,
        lti_complex_t const &m1,
        [[maybe_unused]] lti_real_t const &eps = 0.
        )
{
    const std::complex<lti_real_t> res = ::A0iC(
            id,
            ensure_std_complex(m1)
            );
    return lti_complex_t{res.real(), res.imag()};
}

inline lti_complex_t B0iC(
        int id,
        lti_complex_t p1,
        lti_complex_t m1,
        lti_complex_t m2,
        [[maybe_unused]] lti_real_t const &eps = 0.
        )
{
    const std::complex<lti_real_t> res = ::B0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(m1),
            ensure_std_complex(m2)
            );
    return lti_complex_t{res.real(), res.imag()};
}

inline lti_complex_t C0iC(
        int id,
        lti_complex_t p1,
        lti_complex_t p2,
        lti_complex_t p3,
        lti_complex_t m1,
        lti_complex_t m2,
        lti_complex_t m3,
        lti_real_t const &eps = 0.
        )
{
    if (rescaleIntegral) {
       auto min = 10*minScale; // Ensure initial value higher than minscale
       for (const auto &value: {p1, p2, p3, m1, m2, m3}) {
           const auto re = abs_re(value);
           if (re != 0 && re < min)
               min = re;
       }
       if (min < minScale) {
           const auto scale = regScale / min;
           return rescaled(
                   scale, scaleFactorC(id, scale), lt::C0iC, 
                   id, eps, p1, p2, p3, m1, m2, m3);
       }
    }
    regulateC(p1, p2, p3, m1, m2, m3, eps);
    const std::complex<lti_real_t> res = ::C0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(p2),
            ensure_std_complex(p3),
            ensure_std_complex(m1),
            ensure_std_complex(m2),
            ensure_std_complex(m3)
            );
    return lti_complex_t{res.real(), res.imag()};
}

inline lti_complex_t D0iC(
        int id,
        lti_complex_t p1,
        lti_complex_t p2,
        lti_complex_t p3,
        lti_complex_t p4,
        lti_complex_t p5,
        lti_complex_t p6,
        lti_complex_t m1,
        lti_complex_t m2,
        lti_complex_t m3,
        lti_complex_t m4,
        [[maybe_unused]] lti_real_t const &eps = 0.
        )
{
    const std::complex<lti_real_t> res = ::D0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(p2),
            ensure_std_complex(p3),
            ensure_std_complex(p4),
            ensure_std_complex(p5),
            ensure_std_complex(p6),
            ensure_std_complex(m1),
            ensure_std_complex(m2),
            ensure_std_complex(m3),
            ensure_std_complex(m4)
            );
    return lti_complex_t{res.real(), res.imag()};
}

inline lti_complex_t E0iC(
        int id,
        lti_complex_t p1,
        lti_complex_t p2,
        lti_complex_t p3,
        lti_complex_t p4,
        lti_complex_t p5,
        lti_complex_t p6,
        lti_complex_t p7,
        lti_complex_t p8,
        lti_complex_t p9,
        lti_complex_t p10,
        lti_complex_t m1,
        lti_complex_t m2,
        lti_complex_t m3,
        lti_complex_t m4,
        lti_complex_t m5,
        [[maybe_unused]] lti_real_t const &eps = 0.
        )
{
    const std::complex<lti_real_t> res = ::E0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(p2),
            ensure_std_complex(p3),
            ensure_std_complex(p4),
            ensure_std_complex(p5),
            ensure_std_complex(p6),
            ensure_std_complex(p7),
            ensure_std_complex(p8),
            ensure_std_complex(p9),
            ensure_std_complex(p10),
            ensure_std_complex(m1),
            ensure_std_complex(m2),
            ensure_std_complex(m3),
            ensure_std_complex(m4),
            ensure_std_complex(m5)
            );

    return lti_complex_t{res.real(), res.imag()};
}

}
