#include <marty.h>
#include "hSU2.h"
using namespace std;
using namespace mty;
using namespace csl;

int main(){

  cout << "Testing without anything declared" << endl; 
  cout << "Declaring class" << endl;

  mty::hSU2_Model horizSU2("savefile.out");

  cout << "Calculating Spectrum..." << endl;
  
  mty::Library lib("test_spectrum");
  lib.cleanExistingSources();
  lib.generateSpectrum(horizSU2);
  lib.print();
  
  cout << "Preparing to display Feynman diagrams..." ;
  // Show(hSU2_rules);
  cout << endl; 
  return 0;
}
