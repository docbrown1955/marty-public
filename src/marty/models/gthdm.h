#ifndef MTY_GTHDM_H_INCLUDED
#define MTY_GTHDM_H_INCLUDED

#include "thdm.h"

namespace mty {

constexpr int GTHDM_type = 2;

class GTHDM_Model : public TwoHDM_Model<GTHDM_type> {

  public:
    constexpr static int type = GTHDM_type;

    GTHDM_Model(bool initialize = true);

  protected:
    void init();
    void replaceYukawas();
};

} // namespace mty

#endif
