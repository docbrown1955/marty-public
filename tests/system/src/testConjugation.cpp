/*
 * This programs test RSIF (relative signs of Feynman diagrams) from article
 * "Feynman rules for fermion-number-violating interactions" (A. Denner),
 * eq. 3.4a, 3.5a and 3.6a
 *
 * The signs have been checked analytically, and the squared amplitude is 
 * calculated symbolically and evaluated to match the validated squared 
 * amplitude.
 */
#include <marty.h>
#include <array>
#include "testutility.h"

using namespace csl;
using namespace mty;

auto cc(csl::Expr const &expr) { return GetComplexConjugate(expr); }

template<size_t N>
auto loadKinematics(csl::Expr &expr)
{
   for (size_t i = 0; i != N; ++i) {
      for (size_t j = i+1; j < N; ++j) {
         const int num = 10 * (i+1) + j+1;
         const std::string name = "s_" + std::to_string(num);
         auto symbol = csl::GetSymbol(name, expr);
         if (symbol)
            csl::Replace(expr, symbol, csl::int_s(num));
      }
   }
}

template<size_t N>
auto createKinematics()
{
   std::array<csl::Expr, N*(N-1)/2> res;
   auto iter = res.begin();
   for (size_t i = 0; i != N; ++i) {
      for (size_t j = i+1; j < N; ++j) {
         const int num = 10 * (i+1) + j+1;
         const std::string name = "s_" + std::to_string(num);
         *iter++ = csl::constant_s(name);
      }
   }

   return res;
}

int main() {

   std::cout.setstate(std::ios_base::failbit);
   std::cerr.setstate(std::ios_base::failbit);

   Model model;
   model.init();

   Particle psi = diracfermion_s("psi ; \\psi", model);
   Particle lam = diracfermion_s("lam ; \\lambda", model);
   lam->setSelfConjugate(true);
   Particle phi = scalarboson_s("phi ; \\phi", model);
   Particle A   = vectorboson_s("A",           model);

   model.addParticles({psi, lam, phi, A});

   Index a = DiracIndex();
   Index b = DiracIndex();
   Index mu = MinkowskiIndex();
   Tensor gamma = dirac4.gamma;

   csl::Expr g = csl::constant_s("g");
   model.addLagrangianTerm(
         constant_s("a") * phi() * cc(psi(a)) * psi(a) 
         );
   model.addLagrangianTerm(
         g * phi() * cc(psi(a)) * lam(a),
         true
         );
   model.addLagrangianTerm(
         CSL_HALF * constant_s("c") * phi() * cc(lam(a)) * lam(a)
         );

   model.addLagrangianTerm(
         constant_s("a") * A(mu) * cc(psi(a)) * gamma({+mu, a, b}) * psi(b) 
         );
   model.addLagrangianTerm(
         g * A(mu) * cc(psi(a)) * gamma({+mu, a, b}) * lam(b),
         true
         );
   model.addLagrangianTerm(
         CSL_HALF * constant_s("c") * A(mu) * cc(lam(a)) * gamma({+mu, a, b}) * lam(b)
         );

   model.refresh();

   std::cout << model << '\n';

   Display(model.getFeynmanRules());

   FeynOptions options;
   options.addFilter(filter::disableParticle("A"));
   mty::option::orderExternalFermions = false;
   options.orderExternalFermions = false;
   auto res1 = model.computeAmplitude(
         TreeLevel,
         {Incoming("psi"), Incoming("psi"), Outgoing("psi"), Outgoing("lam"), Outgoing("phi")},
         options
         );
   for (auto &d : res1.getDiagrams()) {
      Evaluate(d.getExpression(), eval::abbreviation);
   }

   Display(res1);
   //Show(res1);
   csl::Expr square1 = model.computeSquaredAmplitude(res1);

   options.resetFilters();
   options.addFilter(filter::disableParticle("phi"));
   auto res2 = model.computeAmplitude(
         TreeLevel,
         {Incoming("psi"), Incoming("psi"), Outgoing("psi"), Outgoing("lam"), Outgoing("A")},
         options
         );
   for (auto &d : res2.getDiagrams()) {
      Evaluate(d.getExpression(), eval::abbreviation);
   }

   Display(res2);
   //Show(res2);
   mty::option::orderExternalFermions = false;
   options.orderExternalFermions = false;
   csl::Expr square2 = model.computeSquaredAmplitude(res2);
   mty::Propagator::Peps->setValue(CSL_0);
   square1 = csl::Evaluated(csl::DeepExpanded(square1), csl::eval::all);
   square2 = csl::Evaluated(csl::DeepExpanded(square2), csl::eval::all);

   //auto [s_12, s_13, s_14, s_15, s_23, s_24, s_25, s_34, s_35, s_45] = createKinematics<5>();
   loadKinematics<5>(square1);
   loadKinematics<5>(square2);

   square1 = csl::Evaluated(csl::DeepExpanded(square1), csl::eval::all);
   square2 = csl::Evaluated(csl::DeepExpanded(square2), csl::eval::all);

   csl::Expr shouldBeS1 =  1.15139*csl::pow_s(g, 6) + 14637*csl::pow_s(-csl::int_s(10)/1547, 2)*csl::pow_s(g, 6);
   csl::Evaluate(shouldBeS1, csl::eval::all);

   csl::Expr shouldBeS2 = 13.0823*csl::pow_s(g, 6);

   loadKinematics<5>(shouldBeS1);
   loadKinematics<5>(shouldBeS2);

   std::cout << (square1->printLaTeX() == shouldBeS1->printLaTeX()) << '\n';
   std::cout << (square1->printLaTeX() == shouldBeS2->printLaTeX()) << '\n';
   std::cout << (square2->printLaTeX() == shouldBeS2->printLaTeX()) << '\n';
   std::cout << (square1->printLaTeX() == shouldBeS2->printLaTeX()) << '\n';

   if (square1->printLaTeX() != shouldBeS1->printLaTeX()) {
      ::error(buildMessage("Bad comparison of ", square1, " and ", shouldBeS1));
      return 1;
   }
   if (square2->printLaTeX() != shouldBeS2->printLaTeX()) {
      ::error(buildMessage("Bad comparison of ", square2, " and ", shouldBeS2));
      return 1;
   }
   if (square1->printLaTeX() == shouldBeS2->printLaTeX()) {
      ::error(buildMessage("Bad comparison (should be !=) of ", square1, " and ", shouldBeS2));
      return 1;
   }
   if (square2->printLaTeX() == shouldBeS1->printLaTeX()) {
      ::error(buildMessage("Bad comparison (should be !=) of ", square2, " and ", shouldBeS1));
      return 1;
   }
   ::success("Conjugation matrix calculations agree.");

   return 0;
}
