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

 #include "fileData.h"

namespace mty::doc {

FileData::FileData(
        std::string_view t_headerName,
        std::string_view t_sourceName
        )
    :headerName(t_headerName),
    sourceName(t_sourceName),
    m_indent(4, ' ')
{}

FileData::~FileData() 
{
    if (header) header.close();
    if (source) source.close();
}

void FileData::openStream()
{
    if (!header.is_open()) {
        header.open(headerName.c_str());
        if (!header) {
            throw IOErrorHeader;
        }
    }
    if (!source.is_open()) {
        source.open(sourceName.c_str());
        if (!source) {
            header.close();
            throw IOErrorSource;
        }
    }
}

void FileData::setIndentSize(std::size_t s) 
{
    m_indent = std::string(s, ' ');
}

std::ostream &FileData::getStream(Mode mode) 
{
    openStream();
    switch(mode) {
        case Header: return header;
        case Source: return source;
        case None: break;
    }
    throw TypeError;
}

std::string const &FileData::indent() const 
{ 
    return m_indent; 
}


}
