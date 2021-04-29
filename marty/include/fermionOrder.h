#pragma once

#include <vector>

namespace csl { class Expr; }

namespace mty {

    class Insertion;
    class QuantumField;

    bool ordered(
            mty::Insertion const &left, 
            mty::Insertion const &right
            );

    std::vector<mty::Insertion*> fermionsOf(
            std::vector<mty::Insertion> &fields
            );

    std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion*> const &fields
            );

    inline std::vector<int> defaultFermionOrder(
            std::vector<mty::Insertion> &fields
            )
    {
        return defaultFermionOrder(fermionsOf(fields));
    }

    void applyFermionOrder(
            std::vector<mty::Insertion> &insertions,
            std::vector<int>      const &order
            );

    void applyDefaultFermionOrder(std::vector<mty::Insertion> &insertions);

}
