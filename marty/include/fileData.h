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
