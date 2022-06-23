#include "marty.h"
#include "marty/models/sm.h"

using namespace mty;

csl::Expr gm2(Model &model, WilsonSet const &wilsons)
{
   csl::Expr C  = getWilsonCoefficient(
            wilsons,
            chromoMagneticOperator(model, wilsons, DiracCoupling::S));
   csl::Expr CL  = getWilsonCoefficient(
            wilsons,
            chromoMagneticOperator(model, wilsons, DiracCoupling::L));
   csl::Expr CR  = getWilsonCoefficient(
            wilsons,
            chromoMagneticOperator(model, wilsons, DiracCoupling::R));
   csl::Expr CP  = getWilsonCoefficient(
            wilsons,
            chromoMagneticOperator(model, wilsons, DiracCoupling::P));

   sm_input::m_e->setValue(CSL_UNDEF);
   csl::Expr factor = -sm_input::e_em / (4 * sm_input::m_e);
   C /= factor;
   csl::Replace(C, sm_input::e_em, csl::sqrt_s(4*CSL_PI*sm_input::alpha_em));

   std::cout << C << '\n';
   return C;
}

int main() {

   std::cout.setstate(std::ios_base::failbit);
   std::cerr.setstate(std::ios_base::failbit);

    using namespace sm_input;
    for (auto &value : {M_Z, M_W, theta_W, e_em, alpha_em, m_b, m_s, m_t, m_mu})
        value->setValue(CSL_UNDEF);

   SM_Model sm;

   Display(sm);

   auto rules = sm.getFeynmanRules();

   mty::Library::setQuadruplePrecision(true);
   mty::Library lib("gm2_sm");
   lib.cleanExistingSources();

   FeynOptions options;
   options.setTopology(Topology::Triangle);
   options.addFilter(
         [&](FeynmanDiagram const &diag) { return diag.isInLoop("A"); }
         );
   auto wil = sm.computeWilsonCoefficients(
         OneLoop, 
         {Incoming("e"), Outgoing("e"), Outgoing("A")}, 
         options);
   csl::Expr C = gm2(sm, wil);
   lib.addFunction("gm2_e", C, true);

   options.resetFilters();
   options.addFilter([&](FeynmanDiagram const &diag) { return !diag.isInLoop("A"); });
   wil = sm.computeWilsonCoefficients(
         OneLoop, 
         {Incoming("e"), Outgoing("e"), Outgoing("A")}, 
         options
         );
   Display(wil);
   C = gm2(sm, wil);
   lib.addFunction("gm2_eW", C, true);

   lib.print();

    [[maybe_unused]] int sysres 
        = system("cp libscripts/example_gm2_sm.cpp gm2_sm/script");
    sysres = system("cd gm2_sm; rm real.txt imag.txt");
    sysres = system("cd gm2_sm; make clean; make -j 4 >/dev/null && "
                    " bin/example_gm2_sm.x > /dev/null");
    sysres = system("bin/comparedata.x data/gm2_sm.txt gm2_sm/real.txt 1e-5"); 
    return sysres;
}
