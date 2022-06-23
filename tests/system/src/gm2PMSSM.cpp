#include "marty.h"
#include "marty/models/pmssm_lem.h"

using namespace mty;
using namespace sm_input;

int main() {

   std::cout.setstate(std::ios_base::failbit);
   std::cerr.setstate(std::ios_base::failbit);

   for (auto &value : {M_W, M_Z, theta_W, m_mu, e_em})
      value->setValue(CSL_UNDEF);

   PMSSM_LEM pmssm;
   FeynOptions options;
   options.setTopology(Topology::Triangle);
   options.setWilsonOperatorCoefficient(-e_em / (4 * m_mu));
   options.addFilters(
         filter::disableParticles({"W", "Z", "H0", "h0", "A0", "Gp", "G0", "Hp"}),
         [&](FeynmanDiagram const &diag) {
            return !diag.isInLoop("A");
         });

   Library{""}.setQuadruplePrecision(true);
   Library lib("gm2PMSSM");
   lib.cleanExistingSources();
   lib.generateSpectrum(pmssm);
   csl::Expr aa = csl::constant_s("aa");

   mty::option::displayAbbreviations = false;
   {
      auto options_cpy = options;
      options_cpy.addFilter(filter::disableParticles({"C_1", "C_2"}));
      auto ampl = pmssm.computeAmplitude(
            OneLoop,
            {Incoming("mu"), Outgoing("mu"), Outgoing("A")},
            options_cpy
            );
      auto wilsons = pmssm.getWilsonCoefficients(ampl, options);
      csl::Expr C  = getWilsonCoefficient(
               wilsons,
               chromoMagneticOperator(pmssm, wilsons, DiracCoupling::S));
      std::cout << "Neutralino" << '\n';
      lib.addFunction("gm2_N", C);
   }
   {
      auto options_cpy = options;
      options_cpy.addFilter(filter::forceParticles({"C_1", "C_2"}));
      auto ampl = pmssm.computeAmplitude(
            OneLoop,
            {Incoming("mu"), Outgoing("mu"), Outgoing("A")},
            options_cpy
            );
      auto wilsons = pmssm.getWilsonCoefficients(ampl, options);
      csl::Expr C  = getWilsonCoefficient(
               wilsons,
               chromoMagneticOperator(pmssm, wilsons, DiracCoupling::S));
      std::cout << "Chargino" << '\n';
      std::cout << csl::Evaluated(C/2, csl::eval::abbreviation) << '\n';
      lib.addFunction("gm2_C", C);
   }

   lib.print();

    [[maybe_unused]] int sysres 
        = system("cp libscripts/example_gm2pmssm.cpp gm2PMSSM/script");
    sysres = system("cd gm2PMSSM; rm data.txt");
    sysres = system("cd gm2PMSSM; make clean; make -j 4 >/dev/null && "
                    " bin/example_gm2pmssm.x > /dev/null");
    sysres = system("bin/comparedata.x data/gm2_pmssm.txt "
                    "gm2PMSSM/data.txt 1e-10");
    return sysres;
}
