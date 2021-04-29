#pragma once

#include <iostream>

namespace mty::Topology {

    enum Type {
        Tadpole  = (1 << 0),
        Mass     = (1 << 1),
        Triangle = (1 << 2),
        Box      = (1 << 3),
        Pentagon = (1 << 4),
        All = Tadpole | Mass | Triangle | Box | Pentagon
    };
    inline constexpr auto topologyList = {
        Tadpole, Mass, Triangle, Box, Pentagon
    };
    inline Type legsToTopology(int nLegs) {
        return static_cast<Type>(
                ((nLegs == 1) * Tadpole)
                | ((nLegs == 2) * Mass)
                | ((nLegs == 3) * Triangle)
                | ((nLegs == 4) * Box)
                | ((nLegs == 5) * Pentagon)
                );
    }
    inline std::ostream &operator<<(std::ostream &out, Type t) {
        static constexpr auto names = {
            "Tadpole", "Mass", "Triangle", "Box", "Pentagon"
        };
        if (t == All) out << "All";
        else          out << *(names.begin() + static_cast<int>(t)/2);
        return out;
    }
}

