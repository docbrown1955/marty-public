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

#include "latexcompiler.h"
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

#define GRAFED_DIR "grafed_temporary_directory"
#define GRAFED_DENSITY "300"

template <class T>
std::string toString(T const &t)
{
    std::ostringstream sout;
    sout << t;
    return sout.str();
}

std::string latexcompiler::getGrafedDir()
{
    AutoMutex     M;
    static size_t n      = 0;
    std::string   folder = GRAFED_DIR "_" + toString(n++);
    std::ifstream f;
    f.open(folder + "/eq.tex");
    if (!f)
        return folder;
    while (f) {
        f.close();
        folder = GRAFED_DIR "_" + toString(n++);
        f.open(folder + "/eq.tex");
    }
    return folder;
}

QPixmap latexcompiler::generateLabel(const std::string &texCode)
{
    std::string          grafed_dir = getGrafedDir();
    [[maybe_unused]] int res        = system(("mkdir " + grafed_dir).c_str());
    std::ofstream        f(grafed_dir + "/eq.tex");
    if (!f)
        return QPixmap();
    f << "\\documentclass[preview]{standalone}\n";
    f << "\\usepackage{amsmath}\n";
    f << "\\usepackage{slashed}\n";
    f << "\\usepackage{xcolor}\n";
    f << "\\begin{document}$" << texCode << "$\\end{document}";
    f.close();
    res = system(("cd " + grafed_dir
                  + " && "
                    " latex --interaction=nonstopmode eq.tex 1>/dev/null;"
                    // " convert -density 300 -antialias -quality 90 eq.pdf
                    // eq.png 1>/dev/null"));
                    " dvipng -q --png -D " GRAFED_DENSITY " -T tight -bg "
                                                          "Transparent eq.dvi "
                                                          "-o eq.png "
                                                          "1>/dev/null")
                     .c_str());
    QPixmap label((grafed_dir + "/eq.png").c_str());
    res = system(("rm -rf " + grafed_dir + " &>/dev/null").c_str());

    return label;
}
#undef GRAFED_DIR
#undef GRAFED_DENSITY

latexcompiler::latexcompiler(QObject *_parent)
    : QObject(_parent), pixmaps(new ResourceHandler(this))
{
    pixmaps->setBuilder(latexcompiler::generateLabel);
    connect(pixmaps,
            SIGNAL(valueReady(const std::string &)),
            this,
            SLOT(labelReady_slot(const std::string &)));
}

void latexcompiler::labelReady_slot(std::string const &label)
{
    emit labelReady(label);
}

void latexcompiler::sendLabel(QString const &texCode)
{
    sendLabel(texCode.toStdString());
}

void latexcompiler::sendLabel(std::string const &texCode)
{
    pixmaps->sendKey(texCode);
}

QPixmap latexcompiler::getLabel(const QString &texCode)
{
    // sendLabel(texCode);
    return pixmaps->buildInMainThread(texCode.toStdString());
    // return generateLabel(texCode.toStdString());
}
