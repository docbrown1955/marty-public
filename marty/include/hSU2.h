/// class for BSM models implementing a simiple SU(2) horizontal symmetry.
// created : 22/02/22
// last edited : 22/02/22

#ifndef hSU2_H_INCLUDED
#define hSU2_H_INCLUDED

#include <csl.h>
#include "SM.h"

namespace mty{
/* hSU2 master class */

  class hSU2_Model: public mty::SM_Model{
   
    public:
      hSU2_Model(bool init = true);

      void init();
      void horizontalSymmetryBreaking();
      void adjust_fermions(); // ?
      
  };
} // End of namespace mty

#endif /* class hSU2_Model */
