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

#include <string>
#include <vector>
#include <string_view>
#include <iostream>
#include <fstream>
#include <functional>

namespace mty::doc {

class FileData {

public:

    enum Mode {
        None,
        Header,
        Source
    };

    enum Error {
        TypeError,
        IOErrorHeader,
        IOErrorSource
    };

    FileData();
    FileData(
            std::string_view headerName,
            std::string_view sourceName
            );

    FileData(FileData const &other) = delete;
    FileData &operator=(FileData const &other) = delete;

    ~FileData();

    void openStream();

    size_t getIndentSize() const { return m_indent.size(); }
    std::string const &indent() const;
    void setIndentSize(size_t s);

    std::ostream &getStream(Mode mode);

private:

    Mode          mode;
    std::ofstream header;
    std::ofstream source;
    std::string   headerName;
    std::string   sourceName;
    std::string   m_indent;
};

}
