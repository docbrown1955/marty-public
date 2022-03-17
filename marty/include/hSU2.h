/// class for BSM models implementing a simiple SU(2) horizontal symmetry.
// created : 22/02/22
// last edited : 22/02/22

#ifndef hSU2_H_INCLUDED
#define hSU2_H_INCLUDED

#include <csl.h>
#include "SM.h"
#include "model.h"

namespace mty{
/* hSU2 master class */

  class hSU2_Model: public mty::SM_Model{
   
    public:
      hSU2_Model(
          int init = 1,
          std::string const &saveFile = "saveFile.out"
          );

      //~hSU2_Model();

    protected: 
      void initContent(); // → Encapsulates the following methods
        void  initGauge();
        void  initLeptons();
        void  initQuarks();

      void initInteractions();
        void gatherhSU2Inputs();
        void horizontalSymmetryBreaking();
        void  initHiggs();
        void getToLowEnergyLagrangian();
        void checkHermiticity();
        void computeFeynmanRules();
        void adjust_fermions(); // ?
     

    private:
      std::string saveFile;
  };
} 

#endif /* class hSU2_Model */
