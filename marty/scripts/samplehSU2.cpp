#include <marty.h>
#include "hSU2.h"
using namespace std;
using namespace mty;
using namespace csl;

int main(){

  int init = 1;
  cout << "Testing without anything declared" << endl; 

  cout << "Declaring class" << endl;

  mty::hSU2_Model horizSU2(init);

  cout << horizSU2 << endl;
      auto hSU2_rules = horizSU2.getFeynmanRules(); // Rules in terminal
  Display(hSU2_rules);
//  Show(hSU2_rules);
  return 0;
}
