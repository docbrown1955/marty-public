/*!
 * @file electriccharge.h
 *
 * @brief Contains the algorithms to identify the charge of a particle directly
 * from a model's Lagrangian.
 *
 * @note A user of MARTY-UFO probably does not need to go through this file.
 */
#pragma once

#include <optional>
#include <marty.h>
#include "ufosmdata.h"
#include "charge.h"

namespace mty::experimental::ufo {

    struct ElectricChargeMessenger {
        mty::QuantumFieldParent const *matterField;
        mty::QuantumFieldParent const *bosonField;
        Model                         &model;
        SMData                  const &smData;
    };

    std::optional<mty::FeynmanRule const*> electricChargeVertex(
        ElectricChargeMessenger const &messenger
        );

    ufo::Charge getScalarElectricCharge(
        mty::FeynmanRule        const &chargeVertex,
        ElectricChargeMessenger const &messenger
        );
    ufo::Charge getFermionElectricCharge(
        mty::FeynmanRule        const &chargeVertex,
        ElectricChargeMessenger const &messenger
        );
    csl::Expr getVectorElectricCharge(
        csl::Expr               const &term,
        ElectricChargeMessenger const &messenger
        );
    ufo::Charge getVectorElectricCharge(
        mty::FeynmanRule        const &chargeVertex,
        ElectricChargeMessenger const &messenger
        );
    ufo::Charge getElectricCharge(
        ElectricChargeMessenger const &messenger
        );

    csl::Expr getTerm(
        mty::FeynmanRule        const &vertex,
        ElectricChargeMessenger const &messenger
        );

    ufo::Charge exprToCharge(csl::Expr expr);

    struct ElectricChargeError {
        csl::Expr const &term;
        std::string      message;
    };

    void raiseElectricChargeError(
        ElectricChargeError const &error
        );

} // namespace mty::experimental::ufo
