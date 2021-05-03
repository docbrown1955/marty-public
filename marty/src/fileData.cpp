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

void FileData::setIndentSize(size_t s) 
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
