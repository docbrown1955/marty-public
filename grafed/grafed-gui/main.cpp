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

#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //for (const auto &str : QFontDatabase().families())
     //   std::cout << str.toStdString() << std::endl;
    a.setWindowIcon(QIcon("logo-white.png"));
    QString nameFile;
    if (argc == 2) {
        nameFile = argv[1];
    }
    else {
        nameFile = "graphs.json";
    }
    MainWindow w(nameFile.toStdString().c_str());
    w.show();

    return a.exec();
}
