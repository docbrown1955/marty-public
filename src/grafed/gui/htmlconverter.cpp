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

#include "htmlconverter.h"
#include <QString>
#include <iostream>

QString HTMLConverter::fromLatex(QString const &init)
{
    QString res;
    res.reserve(init.size());
    auto iter = init.begin();
    while (iter < init.end()) {
        if (*iter == '^') {
            res += "<sup>";
            res += readSingle(++iter, init.end());
            res += "</sup>";
            continue;
        }
        else if (*iter == '_') {
            res += "<sub>";
            res += readSingle(++iter, init.end());
            res += "</sub>";
            continue;
        }
        else if (*iter != ' ')
            res += *iter;
        ++iter;
    }

    // std::cout << "INIT   = " << init.toStdString() << std::endl;
    // std::cout << "INTER  = " << res.toStdString() << std::endl;
    QString html;
    html.reserve(res.size());
    iter = res.begin();
    html += "<html><span class=\"math\">";
    while (iter < res.end()) {
        if (*iter == '\\') {
            QString s = readToSeparator(++iter, res.end());
            if (s != "tilde" and s != "bar" and s != "")
                html += '&' + s + ';';
            else if (s != "") {
                html += readSingle(iter, init.end());
                if (s == "bar")
                    html += "&#772;";
                else if (s == "tilde")
                    html += "&#772;";
                ++iter;
            }
            if (iter == res.end())
                break;
            continue;
        }
        if (*iter != ' ')
            html += *iter;
        ++iter;
    }
    // std::cout << "RES  = " << html.toStdString() << std::endl;
    html += "</span></html>";

    return html;
}

bool HTMLConverter::isSeparator(QChar c)
{
    return std::find(separators.begin(), separators.end(), c)
           != separators.end();
}

QString HTMLConverter::readSingle(QString::const_iterator &iter,
                                  QString::const_iterator  end)
{
    if (*iter == '\\') {
        return '\\' + readToSeparator(++iter, end);
    }
    else if (*iter == '{') {
        return readTo('}', ++iter, end);
    }
    return *iter++;
}

QString HTMLConverter::readToSeparator(QString::const_iterator &iter,
                                       QString::const_iterator  end)
{
    QString res = "";
    while (iter < end) {
        if (isSeparator(*iter))
            break;
        res += *iter;
        ++iter;
    }
    return res;
}

QString HTMLConverter::readTo(QChar                    c,
                              QString::const_iterator &iter,
                              QString::const_iterator  end)
{
    QString res = "";
    while (iter < end) {
        if (c == *iter) {
            ++iter;
            return res;
        }
        res += *iter;
        ++iter;
    }
    std::cerr << "Latex to HTML parser detected an error while "
              << "searching for " << c.toLatin1() << std::endl;
    return res;
}
