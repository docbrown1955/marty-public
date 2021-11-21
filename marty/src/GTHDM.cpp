#include "GTHDM.h"

using namespace csl;

namespace mty {

    GTHDM_Model::GTHDM_Model(bool initialize)
        :TwoHDM_Model<2>(false)
    {
        if (initialize)
            init();
    }

    void GTHDM_Model::init()
    {
        initContent();
        gaugeSymmetryBreaking();
        replaceWboson();
        replaceHiggs();
        diagonalize2By2Matrices();
        GTHDM_Model::replaceYukawas(); // replace this function from 2HDM
        flavorSymmetryBreaking();     
        adjust();
    }

    void GTHDM_Model::replaceYukawas()
    {
        ///////////////////////////////////////////////////
        // Taking care of yukawa couplings
        ///////////////////////////////////////////////////

        csl::Tensor Ye1 = GetYukawa(*this, "Y1e");
        csl::Tensor Yu1 = GetYukawa(*this, "Y1u");
        csl::Tensor Yd1 = GetYukawa(*this, "Y1d");

        csl::Tensor Ye2 = GetYukawa(*this, "Y2e");
        csl::Tensor Yu2 = GetYukawa(*this, "Y2u");
        csl::Tensor Yd2 = GetYukawa(*this, "Y2d");

        csl::Expr m_e   = csl::constant_s("m_e");
        csl::Expr m_mu  = csl::constant_s("m_mu");
        csl::Expr m_tau = csl::constant_s("m_tau");
        csl::Expr m_u   = csl::constant_s("m_u");
        csl::Expr m_c   = csl::constant_s("m_c");
        csl::Expr m_t   = csl::constant_s("m_t");
        csl::Expr m_d   = csl::constant_s("m_d");
        csl::Expr m_s   = csl::constant_s("m_s");
        csl::Expr m_b   = csl::constant_s("m_b");

        const csl::Space* flavorSpace = GetSpace(Ye1);
        csl::Tensor M_e = csl::tensor_s(
                "M_e",
                {flavorSpace, flavorSpace},
                csl::matrix_s({{m_e, CSL_0, CSL_0},
                          {CSL_0, m_mu, CSL_0},
                          {CSL_0, CSL_0, m_tau}}));

        csl::Tensor M_u = csl::tensor_s(
                "M_u",
                {flavorSpace, flavorSpace},
                csl::matrix_s({{m_u, CSL_0, CSL_0},
                          {CSL_0, m_c, CSL_0},
                          {CSL_0, CSL_0, m_t}}));

        csl::Tensor M_d = csl::tensor_s(
                "M_d",
                {flavorSpace, flavorSpace},
                csl::matrix_s({{m_d, CSL_0, CSL_0},
                          {CSL_0, m_s, CSL_0},
                          {CSL_0, CSL_0, m_b}}));


        csl::Index f_i = GetIndex(flavorSpace);
        csl::Index f_j = GetIndex(flavorSpace);
        csl::Index f_k = GetIndex(flavorSpace);
        csl::Index f_l = GetIndex(flavorSpace);
        csl::Tensor delta_flav  = Delta(flavorSpace);

        buildCKM(flavorSpace);

        mty::Particle D_L = GetParticle(*this, "D_L");
        mty::Particle D_R = GetParticle(*this, "D_R");
        csl::Index a1  = DiracIndex();
        csl::Index A   = GaugeIndex(*this, "SU3c", D_L);
        Replaced(*this,
                D_L({f_j, A, a1}),
                V_CKM({f_j, f_k}) * D_L({f_k, A, a1}));
        Replaced(*this,
                D_R({f_i, A, a1}),
                V_CKM({f_i, f_j}) * D_R({f_j, A, a1}));

        // Replace Y1
        Replaced(*this,
                Ye1({f_i, f_j}),
                sqrt_s(2)/(v*cos_s(beta)) * M_e({f_i, f_j})
                - tan_s(beta) * Ye2({f_i, f_j})
                );
        Replaced(*this,
                Yd1({f_i, f_j}),
                V_CKM({f_i, f_k}) * (
                    sqrt_s(2)/(v*cos_s(beta)) * M_d({f_k, f_l})
                ) * GetComplexConjugate(V_CKM({f_j, f_l}))    
                - tan_s(beta) * Yd2({f_i, f_j})
                );
        Replaced(*this,
                Yu1({f_i, f_j}),
                sqrt_s(2)/(v*cos_s(beta)) * M_d({f_i, f_j})
                - tan_s(beta) * Yu2({f_i, f_j})
                );

        // Replace Y2 by new parametrization xi
        // Lepton Yukawa parameters
        csl::Expr xil_mm = csl::constant_s("xil_mm");
        csl::Expr xil_mt = csl::constant_s("xil_mt");
        csl::Expr xil_tt = csl::constant_s("xil_tt");
        // Up-type quarks Yukawa parameters
        csl::Expr xiu_cc = csl::constant_s("xiu_cc");
        csl::Expr xiu_ct = csl::constant_s("xiu_ct");
        csl::Expr xiu_tt = csl::constant_s("xiu_tt");
        // Down-type quarks Yukawa parameters
        csl::Expr xid_ss = csl::constant_s("xid_ss");
        csl::Expr xid_sb = csl::constant_s("xid_sb");
        csl::Expr xid_bb = csl::constant_s("xid_bb");

        // Creating tensors
        csl::Tensor xil(
                "xil", 
                {flavorSpace, flavorSpace},
                csl::matrix_s(
                    {{CSL_0, CSL_0, CSL_0},
                    {CSL_0, xil_mm, xil_mt},
                    {CSL_0, xil_mt, xil_tt}}));
        csl::Tensor xiu(
                "xiu", 
                {flavorSpace, flavorSpace},
                csl::matrix_s(
                    {{CSL_0, CSL_0, CSL_0},
                    {CSL_0, xiu_cc, xiu_ct},
                    {CSL_0, xiu_ct, xiu_tt}}));
        csl::Tensor xid(
                "xid", 
                {flavorSpace, flavorSpace},
                csl::matrix_s(
                    {{CSL_0, CSL_0, CSL_0},
                    {CSL_0, xid_ss, xid_sb},
                    {CSL_0, xid_sb, xid_bb}}));

        csl::Expr cbeta = csl::cos_s(beta);
        csl::Expr sbeta = csl::sin_s(beta);
        csl::Expr sqrt2 = csl::sqrt_s(2);
        // Finally replacing
        Replaced(*this,
                Ye2({f_i, f_j}),
                cbeta * xil({f_i, f_j}) + sqrt2 / v * sbeta * M_e({f_i, f_j})
                );
        Replaced(*this,
                Yu2({f_i, f_j}),
                cbeta * xiu({f_i, f_j}) + sqrt2 / v * sbeta * M_u({f_i, f_j})
                );
        Replaced(*this,
                Yd2({f_i, f_j}),
                cbeta * xid({f_i, f_j}) + 
                V_CKM({f_i, f_k}) * (
                    sqrt2 / v * sbeta * M_d({f_k, f_l})
                ) * GetComplexConjugate(V_CKM({f_j, f_l}))
                );
    }


} // namespace mty
