#pragma once

#include "looptools_patch.h"

<<<<<<< HEAD:src/marty/core/looptools_interface.h
<<<<<<< HEAD:src/marty/core/looptools_interface.h
=======
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
#ifdef __cplusplus
#define _MTY_INLINE inline
#else
#define _MTY_INLINE static inline
#endif

_MTY_INLINE lti_complex_t mtylt_A0iC(
<<<<<<< HEAD:src/marty/core/looptools_interface.h
=======
inline lti_complex_t mtylt_A0iC(
>>>>>>> Intermediate:marty/include/looptools_interface.h
=======
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
        int id,
        lti_complex_t m1
        )
{
#ifdef __cplusplus
    auto res = ::A0iC(
            id,
            ensure_std_complex(m1)
            );
    return lti_complex_t{res.real(), res.imag()};
#else
    return A0iC(
            id,
            ensure_std_complex(m1)
            );
#endif
}

<<<<<<< HEAD:src/marty/core/looptools_interface.h
<<<<<<< HEAD:src/marty/core/looptools_interface.h
_MTY_INLINE lti_complex_t mtylt_B0iC(
=======
inline lti_complex_t mtylt_B0iC(
>>>>>>> Intermediate:marty/include/looptools_interface.h
=======
_MTY_INLINE lti_complex_t mtylt_B0iC(
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
        int id,
        lti_complex_t p1,
        lti_complex_t m1,
        lti_complex_t m2
        )
{
#ifdef __cplusplus
    auto res = ::B0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(m1),
            ensure_std_complex(m2)
            );
    return lti_complex_t{res.real(), res.imag()};
#else
    return B0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(m1),
            ensure_std_complex(m2)
            );
#endif
}

<<<<<<< HEAD:src/marty/core/looptools_interface.h
<<<<<<< HEAD:src/marty/core/looptools_interface.h
_MTY_INLINE lti_complex_t mtylt_C0iC(
=======
inline lti_complex_t mtylt_C0iC(
>>>>>>> Intermediate:marty/include/looptools_interface.h
=======
_MTY_INLINE lti_complex_t mtylt_C0iC(
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
        int id,
        lti_complex_t p1,
        lti_complex_t p2,
        lti_complex_t p3,
        lti_complex_t m1,
        lti_complex_t m2,
        lti_complex_t m3,
        lti_real_t eps
        )
{
    if (getIntegrationParameters()->rescaleIntegral) {
       // Ensure initial value higher than minscale
       lti_real_t min = 10*getIntegrationParameters()->minScale; 
       lti_complex_t values[6] = { p1, p2, p3, m1, m2, m3 };
       for (int i = 0; i != sizeof(values)/sizeof(lti_complex_t); ++i) {
           const lti_real_t re = abs_re(values[i]);
           if (re != 0 && re < min)
               min = re;
       }
       if (min < getIntegrationParameters()->minScale) {
           const lti_real_t scale = getIntegrationParameters()->regScale / min;
           return rescaledC(
                   scale, scaleFactorC(id, scale), 
                   mtylt_C0iC, 
                   id, eps, p1, p2, p3, m1, m2, m3);
       }
    }
    regulateC(&p1, &p2, &p3, m1, m2, m3, eps);
#ifdef __cplusplus
    auto res = ::C0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(p2),
            ensure_std_complex(p3),
            ensure_std_complex(m1),
            ensure_std_complex(m2),
            ensure_std_complex(m3)
            );
    return lti_complex_t{res.real(), res.imag()};
#else
    return C0iC(
            id,
            ensure_std_complex(p1),
            ensure_std_complex(p2),
            ensure_std_complex(p3),
            ensure_std_complex(m1),
            ensure_std_complex(m2),
            ensure_std_complex(m3)
            );
#endif
}

<<<<<<< HEAD:src/marty/core/looptools_interface.h
<<<<<<< HEAD:src/marty/core/looptools_interface.h
_MTY_INLINE lti_complex_t mtylt_D0iC(
=======
inline lti_complex_t mtylt_D0iC(
>>>>>>> Intermediate:marty/include/looptools_interface.h
=======
_MTY_INLINE lti_complex_t mtylt_D0iC(
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
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
        lti_complex_t m4
        )
{
#ifdef __cplusplus
    auto res = ::D0iC(
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
#else
    return D0iC(
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
#endif
}

<<<<<<< HEAD:src/marty/core/looptools_interface.h
<<<<<<< HEAD:src/marty/core/looptools_interface.h
_MTY_INLINE lti_complex_t mtylt_E0iC(
=======
inline lti_complex_t mtylt_E0iC(
>>>>>>> Intermediate:marty/include/looptools_interface.h
=======
_MTY_INLINE lti_complex_t mtylt_E0iC(
>>>>>>> Corrected looptools for C/Cpp libraries:marty/include/looptools_interface.h
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
        lti_complex_t m5
        )
{
#ifdef __cplusplus
    auto res = ::E0iC(
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
#else
    return E0iC(
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
#endif
}

