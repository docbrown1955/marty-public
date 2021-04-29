#pragma once

#include <iostream>

namespace sgl {

    enum class Exception {
        AbstractCall,
        MathError,
        TypeError,
        IndexError,
        ValueError,
    };

    inline std::ostream &operator<<(
            std::ostream &out,
            Exception     ex
            )
    {
        switch (ex) {
            case Exception::AbstractCall: out << "AbstractCall"; break;
            case Exception::MathError:    out << "MathError";    break;
            case Exception::TypeError:    out << "TypeError";    break;
            case Exception::IndexError:   out << "IndexError";   break;
            case Exception::ValueError:   out << "ValueError";   break;
        }
        return out;
    }
}
