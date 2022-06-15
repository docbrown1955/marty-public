#pragma once

#include "2HDM.h"

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
