#include "ckm.h"

using namespace c9_mssm;

void fillCKM(param_t &params)
{
    params.V_ud = real_t{0.97434};
    params.V_us = real_t{0.22506};
    auto V_ub = 0.00117;// + i*-0.00338;
    params.V_ub_mod = cabsq(V_ub);
    params.delta_wolf = -cargq(V_ub);
    // params.V_ub = complex_t{0.00117, -0.00338};
    params.V_cd = complex_t{-0.22492 , -0.00014};
    params.V_cs = complex_t{0.97351 , -0.00003};
    params.V_cb = real_t{0.04108};
    params.V_td = complex_t{0.00810 , -0.00329};
    params.V_ts = complex_t{-0.04029 , -0.00076};
    params.V_tb = real_t{0.99915};
}

