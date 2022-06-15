#pragma once

namespace mty {

/**
 * @brief Operator basis for Wilson coefficients.
 */
enum class OperatorBasis {
    /**
     * @brief Uses projectors \f$ P_L,P_R \f$.
     */
    Chiral,
    /**
     * @brief Uses \f$ 1,\gamma^5 \f$.
     */
    Standard,
    /**
     * @brief Does not force any particular decomposition.
     */
    None,
};
} // namespace mty
