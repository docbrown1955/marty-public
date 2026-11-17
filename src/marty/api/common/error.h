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

/**
 * @file error.h
 * @brief Typed errors of the MARTY API layer and translation of core errors.
 *
 * Core errors (mty::error::Exception, the bare mty::error::Type and CSLError
 * enums, GammaAPIError and std::exception) are converted to the typed
 * hierarchy at the API boundary with rethrowAsApiError().
 */

#ifndef MTY_API_COMMON_ERROR_H_INCLUDED
#define MTY_API_COMMON_ERROR_H_INCLUDED

#include "../../../csl/api/error.h"
#include "../../../csl/error.h"
#include "../../core/mrtError.h"
#include "../gamma_api_error.h"

#include <sstream>
#include <string>

namespace mty::api {

using Error               = csl::api::Error;
using ValueError          = csl::api::ValueError;
using TypeError           = csl::api::TypeError;
using LookupError         = csl::api::LookupError;
using NotImplementedError = csl::api::NotImplementedError;
using StateError          = csl::api::StateError;

/// Model building failed (core mty::error::ModelBuildingError).
class BuildError : public Error {
  public:
    using Error::Error;
};

namespace detail {

[[noreturn]] inline void throwFromCoreType(mty::error::Type   type,
                                           std::string const &what)
{
    switch (type) {
    case mty::error::TypeError:
        throw TypeError(what);
    case mty::error::ValueError:
    case mty::error::ZeroDivisionError:
    case mty::error::OverflowError:
        throw ValueError(what);
    case mty::error::KeyError:
    case mty::error::IndexError:
    case mty::error::NameError:
    case mty::error::AttributeError:
        throw LookupError(what);
    case mty::error::NotImplementedError:
        throw NotImplementedError(what);
    case mty::error::ModelBuildingError:
        throw BuildError(what);
    default:
        throw Error(what);
    }
}

[[noreturn]] inline void throwFromCSLType(CSLError           type,
                                          std::string const &what)
{
    switch (type) {
    case CSLError::TypeError:
        throw TypeError(what);
    case CSLError::ValueError:
    case CSLError::ZeroDivisionError:
    case CSLError::OverflowError:
        throw ValueError(what);
    case CSLError::KeyError:
    case CSLError::IndexError:
    case CSLError::NameError:
    case CSLError::AttributeError:
        throw LookupError(what);
    case CSLError::NotImplementedError:
        throw NotImplementedError(what);
    default:
        throw Error(what);
    }
}

} // namespace detail

/// Rethrows the exception currently being handled as a typed API error.
/// Must be called from a catch handler. Errors already typed are rethrown
/// unchanged.
[[noreturn]] inline void rethrowAsApiError()
{
    try {
        throw;
    }
    catch (Error const &) {
        throw;
    }
    catch (mty::error::Exception const &ex) {
        detail::throwFromCoreType(ex.type(), ex.what());
    }
    catch (mty::error::Type const &type) {
        std::ostringstream sout;
        sout << type;
        detail::throwFromCoreType(type, sout.str());
    }
    catch (CSLError const &type) {
        std::ostringstream sout;
        sout << type;
        detail::throwFromCSLType(type, sout.str());
    }
    catch (GammaAPIError const &ex) {
        throw Error(ex.what());
    }
    catch (std::exception const &ex) {
        throw Error(ex.what());
    }
    catch (...) {
        throw Error("unknown error in MARTY core");
    }
}

} // namespace mty::api

#endif
