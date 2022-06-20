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

/*!
 * @file
 * @author Grégoire Uhlrich
 * @version 2.0

 * \brief
 */
#ifndef HTMLCONVERTER_H
#define HTMLCONVERTER_H

#include <QChar>
#include <QString>
#include <vector>

class HTMLConverter {

  public:
    HTMLConverter() = delete;

    static QString fromLatex(QString const &init);

  private:
    static bool    isSeparator(QChar c);
    static QString readToSeparator(QString::const_iterator &iter,
                                   QString::const_iterator  end);
    static QString readTo(QChar                    c,
                          QString::const_iterator &iter,
                          QString::const_iterator  end);
    static QString readSingle(QString::const_iterator &iter,
                              QString::const_iterator  end);

  private:
    inline static std::vector<QChar> separators
        = {'\\', '(', ')', '_', '^', '{', '}', ' ', '<', '>'};
};

#endif // HTMLCONVERTER_H
