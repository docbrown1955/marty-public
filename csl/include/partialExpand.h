// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

 #pragma once

#include <functional>
#include "abstract.h"

namespace csl {

class Space;

using ExpanderEmitter  = std::function<bool(csl::Expr const&)>;
using ExpanderReceiver = std::function<bool(csl::Expr const&, csl::Expr const&)>;

bool haveCommonIndices(
        csl::Expr const &a, 
        csl::Expr const &b,
        csl::Space const *space
        );

void PartialExpandImplementation(
        Expr &prod,
        ExpanderEmitter  const &isEmitter,
        ExpanderReceiver const &isReceiver
        );

void PartialExpandImplementation(
        Expr &prod,
        ExpanderEmitter const &isEmitter,
        ExpanderEmitter const &isReceiver
        );

inline void PartialExpandImplementation(
        Expr &prod,
        ExpanderEmitter const &isEmitterReceiver
        )
{
    return PartialExpandImplementation(
            prod, 
            isEmitterReceiver, 
            isEmitterReceiver
            );
}

Expr PartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter,
                ExpanderReceiver const &isReceiver
                );
void PartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter,
                ExpanderReceiver const &isReceiver
                );
Expr DeepPartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter,
                ExpanderReceiver const &isReceiver
                );
void DeepPartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter,
                ExpanderReceiver const &isReceiver
                );

inline Expr PartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter,
                ExpanderEmitter const &isReceiver
                )
{
    return PartialExpanded(
                    init,
                    isEmitter,
                    [&isReceiver](csl::Expr const&, csl::Expr const &a) -> bool {
                        return bool(isReceiver(a));
                    });
}
inline void PartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter,
                ExpanderEmitter const &isReceiver
                )
{
    PartialExpand(
                    init,
                    isEmitter,
                    [&isReceiver](csl::Expr const&, csl::Expr const &a) -> bool {
                        return bool(isReceiver(a));
                    });
}
inline Expr DeepPartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter,
                ExpanderEmitter const &isReceiver
                )
{
    return DeepPartialExpanded(
                    init,
                    isEmitter,
                    [&isReceiver](csl::Expr const&, csl::Expr const &a) -> bool {
                        return bool(isReceiver(a));
                    });
}
inline void DeepPartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter,
                ExpanderEmitter const &isReceiver
                )
{
    DeepPartialExpand(
                    init,
                    isEmitter,
                    [&isReceiver](csl::Expr const&, csl::Expr const &a) -> bool {
                        return bool(isReceiver(a));
                    });
}

inline Expr PartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter
                )
{
    return PartialExpanded(init, isEmitter, isEmitter);
}
inline void PartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter
                )
{
    PartialExpand(init, isEmitter, isEmitter);
}
inline Expr DeepPartialExpanded(
                Expr const &init,
                ExpanderEmitter const &isEmitter
                )
{
    return DeepPartialExpanded(init, isEmitter, isEmitter);
}
inline void DeepPartialExpand(
                Expr &init,
                ExpanderEmitter const &isEmitter
                )
{
    DeepPartialExpand(init, isEmitter, isEmitter);
}

}
