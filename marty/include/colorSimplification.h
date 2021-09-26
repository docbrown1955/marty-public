#pragma once

#include "colorSpace.h"

namespace mty {

    csl::Expr CalculateFColorTrace(csl::Expr const &expr);

    csl::Expr CalculateTrace(
            const mty::SemiSimpleGroup* colorGroup,
            const csl::Expr&            expr);

    csl::Expr CalculateColorTrace(csl::Expr const &init);

} // namespace mty
