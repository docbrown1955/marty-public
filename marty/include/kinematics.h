#pragma once

#include <csl.h>
#include <numeric>
#include "insertion.h"

namespace mty {

    class Kinematics {

    public:

        Kinematics() = default;

        Kinematics(std::vector<mty::Insertion> const &t_insertions);

        Kinematics(
                std::vector<mty::Insertion> const &t_insertions,
                std::vector<csl::Tensor>    const &t_momenta
                );

        Kinematics(
                std::vector<mty::Insertion> const &t_insertions,
                std::vector<size_t>         const &indices
                );

        size_t size() const {
            return momenta.size();
        }

        mty::Insertion const &insertion(size_t i) const {
            return insertions[i];
        }

        csl::Tensor const &momentum(size_t i) const {
            return momenta[i];
        }

        csl::Expr squaredMomentum(size_t i, size_t j) const {
            return squaredMomenta[squaredMomentumIndex(i, j)];
        }

        std::vector<mty::Insertion> const &getInsertions() const {
            return insertions;
        }

        std::vector<csl::Tensor> const &getMomenta() const {
            return momenta;
        }

        std::vector<csl::Expr> const &getSquaredMomenta() const {
            return squaredMomenta;
        }

        csl::Expr getDegreesOfFreedomFactor() const;

        Kinematics subset(std::vector<size_t> pos) const;

        void applyPermutation(std::vector<size_t> const &pos);

        Kinematics applyMap(
                std::vector<size_t> const &mapping
                ) const;

        static void replace(
                csl::Expr        &expr,
                Kinematics const &k1,
                Kinematics const &k2
                );

        static std::vector<size_t> defaultIndices(size_t N) {
            std::vector<size_t> indices(N);
            std::iota(begin(indices), end(indices), 0);
            return indices;
        }

        friend std::ostream &operator<<(
                std::ostream     &out,
                Kinematics const &kin
                );

    private:

        void initMomentaSquared(std::vector<size_t> const &indices);

        void addContraction(
                csl::Tensor     &p1,
                csl::Tensor     &p2, 
                csl::Expr const &res
                );

        void setSquaredMomenta(
                size_t           i, 
                size_t           j,
                csl::Expr const &res);

        size_t squaredMomentumIndex(size_t i, size_t j) const {
            return i * momenta.size() + j;
        }

    private:

        std::vector<mty::Insertion> insertions;
        std::vector<csl::Tensor>    momenta;
        std::vector<csl::Expr>      squaredMomenta;
    };

} // namespace mty
