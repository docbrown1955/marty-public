#pragma once

#include "marty.h"
#include "marty/models/sm.h"

namespace vl {

    class VectorLike_Model: public mty::SM_Model {

    public:

        struct Model {
            bool model5;
            bool model6;
            bool model7; 
            bool model8; 
            bool model9;
            bool model10;
            bool model11;
        };

        VectorLike_Model(Model const &model5);

        static std::vector<std::string> getVLQMasses(Model const &model);

        void addVectorLike(
                std::string const &name,
                int                irrepSU2,
                csl::Expr   const &hyperCharge,
                csl::Expr   const &mass
                );

        void renameSMQuarkMasses();
    };
}
