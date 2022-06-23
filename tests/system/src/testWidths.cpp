#include <marty.h>
#include "marty/models/sm.h"
#include "testutility.h"

void addWidth(
      mty::Model                        &model,
      mty::Library                      &lib,
      std::string                 const &name,
      std::vector<mty::Insertion> const &ins,
      mty::Order                         order = mty::TreeLevel,
      mty::FeynOptions                   options = {}
      )
{
   auto ampl  = model.computeAmplitude(order, ins, options);
   auto sampl = model.computeSquaredAmplitude(ampl);
   std::cout << sampl << '\n';
   std::cout << csl::Evaluated(sampl, csl::eval::abbreviation) << '\n';
   lib.addFunction(name, sampl);
}

void reinitializeInputs()
{
   using namespace mty::sm_input;
   for (auto &input : {e_em, g_s, M_W, M_Z, m_b, m_t, m_h, theta_W, V_cb})
      input->setValue(CSL_UNDEF);
}

int main() {

   std::cout.setstate(std::ios_base::failbit);
   mty::SM_Model SM;
   // Display(SM.getFeynmanRules());

   mty::option::displayAbbreviations = false;
   reinitializeInputs();

   [[maybe_unused]] int sysres = system("rm -r widths_lib/* >/dev/null");
   mty::Library lib("widths_lib");
   addWidth(SM, lib, "M2_hWW", {
         mty::Incoming("h0"), mty::Outgoing("W"), mty::Outgoing(mty::AntiPart("W"))
         });
   addWidth(SM, lib, "M2_hZZ", {
         mty::Incoming("h0"), mty::Outgoing("Z"), mty::Outgoing("Z")
         });
   addWidth(SM, lib, "M2_hbb", {
         mty::Incoming("h0"), mty::Outgoing("b"), mty::Outgoing(mty::AntiPart("b"))
         });
   addWidth(SM, lib, "M2_Wlnu", {
         mty::Incoming("W"), mty::Outgoing("nu_mu"), mty::Outgoing(mty::AntiPart("mu"))
         });
   addWidth(SM, lib, "M2_Wcb", {
         mty::Incoming("W"), mty::Outgoing("c"), mty::Outgoing(mty::AntiPart("b"))
         });
   addWidth(SM, lib, "M2_Zss", {
         mty::Incoming("Z"), mty::Outgoing("s"), mty::Outgoing(mty::AntiPart("s"))
         });
   mty::FeynOptions options;
   options.addFilter(mty::filter::forceParticle("t"));
   // h0 -> tloop -> AA
   addWidth(SM, lib, "M2_hAA_t", {
         mty::Incoming("h0"), mty::Outgoing("A"), mty::Outgoing(mty::AntiPart("A"))
         }, mty::OneLoop, options);

   // h0 -> tloop -> GG
   // SM.getParticle("t")->setMass(SM.getParticle("b")->getMass());
   addWidth(SM, lib, "M2_hGG", {
         mty::Incoming("h0"), mty::Outgoing("G"), mty::Outgoing(mty::AntiPart("G"))
         }, mty::OneLoop, options);

   // h0 -> Wloop -> AA
   options.resetFilters();
   options.addFilter(mty::filter::forceParticles({"W", "Gp"}));
   addWidth(SM, lib, "M2_hAA_W", {
         mty::Incoming("h0"), mty::Outgoing("A"), mty::Outgoing(mty::AntiPart("A"))
         }, mty::OneLoop, options);
   lib.print();

   sysres = system("cp libscripts/example_widths_lib.cpp widths_lib/script");
   sysres = system("cd widths_lib; make >/dev/null && bin/example_widths_lib.x >/dev/null");
   sysres = system("bin/comparedata.x data/widths_with_hBB_1L.txt widths_lib/data.txt 1e-5");

   return sysres;
}
