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

 #ifndef LATEXCOMPILER_H
#define LATEXCOMPILER_H

#include <QPixmap>
#include <string>
#include <map>
#include <optional>
#include "resourcehandler.h"

class latexcompiler: public QObject
{

    Q_OBJECT

public:
    latexcompiler(QObject *_parent);
    ~latexcompiler() override {}
    QPixmap getLabel(QString const &texCode);
    std::optional<QPixmap> getLabel(std::string const &texCode);

    void sendLabel(QString const &texCode);
    void sendLabel(std::string const &texCode);
    bool isReady() const {
        return pixmaps->isReady();
    }

public slots:

    void labelReady_slot(std::string const &);
signals:

    void labelReady(std::string const &);

private:
    static
    std::string getGrafedDir();
    static
    QPixmap generateLabel(std::string const &equation);

private:

    ResourceHandler *pixmaps;
};

#endif // LATEXCOMPILER_H
