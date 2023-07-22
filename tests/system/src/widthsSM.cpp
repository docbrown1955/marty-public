#include "comparedata.h"
#include "testutility.h"
#include "marty/models/sm.h"
#include <marty.h>

void addWidth(mty::Model                        &model,
              mty::Library                      &lib,
              std::string const                 &name,
              std::vector<mty::Insertion> const &ins,
              mty::Order                         order   = mty::TreeLevel,
              mty::FeynOptions                   options = {})
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

int main()
{
    mty::sm_input::redefineNumericalValues(); // for compatibility
    mty::SM_Model SM;

    mty::option::displayAbbreviations = false;
    reinitializeInputs();

    [[maybe_unused]] int sysres = system("rm -rf libs/widthsSM");
    mty::Library         lib("widthsSM", "libs");
    addWidth(SM,
             lib,
             "M2_hWW",
             {mty::Incoming("h"),
              mty::Outgoing("W"),
              mty::Outgoing(mty::AntiPart("W"))});
    addWidth(SM,
             lib,
             "M2_hZZ",
             {mty::Incoming("h"), mty::Outgoing("Z"), mty::Outgoing("Z")});
    addWidth(SM,
             lib,
             "M2_hbb",
             {mty::Incoming("h"),
              mty::Outgoing("b"),
              mty::Outgoing(mty::AntiPart("b"))});
    addWidth(SM,
             lib,
             "M2_Wlnu",
             {mty::Incoming("W"),
              mty::Outgoing("nu_mu"),
              mty::Outgoing(mty::AntiPart("mu"))});
    addWidth(SM,
             lib,
             "M2_Wcb",
             {mty::Incoming("W"),
              mty::Outgoing("c"),
              mty::Outgoing(mty::AntiPart("b"))});
    addWidth(SM,
             lib,
             "M2_Zss",
             {mty::Incoming("Z"),
              mty::Outgoing("s"),
              mty::Outgoing(mty::AntiPart("s"))});
    mty::FeynOptions options;
    options.addFilter(mty::filter::forceParticle("t"));
    // h -> tloop -> AA
    addWidth(SM,
             lib,
             "M2_hAA_t",
             {mty::Incoming("h"),
              mty::Outgoing("A"),
              mty::Outgoing(mty::AntiPart("A"))},
             mty::OneLoop,
             options);

    // h -> tloop -> GG
    // SM.getParticle("t")->setMass(SM.getParticle("b")->getMass());
    addWidth(SM,
             lib,
             "M2_hGG",
             {mty::Incoming("h"),
              mty::Outgoing("G"),
              mty::Outgoing(mty::AntiPart("G"))},
             mty::OneLoop,
             options);

    // h -> Wloop -> AA
    options.resetFilters();
    options.addFilter(mty::filter::forceParticles({"W", "Gp"}));
    addWidth(SM,
             lib,
             "M2_hAA_W",
             {mty::Incoming("h"),
              mty::Outgoing("A"),
              mty::Outgoing(mty::AntiPart("A"))},
             mty::OneLoop,
             options);
    defineLibPath(lib);
    lib.print();

    sysres = system("cp libsrc/example_widths_lib.cpp libs/widthsSM/script");
    sysres = system("cd libs/widthsSM; make && bin/example_widths_lib.x");
    return assert_equal(
        "data/output/widths_with_hBB_1L.txt", "libs/widthsSM/data.txt", 1e-5);
}
