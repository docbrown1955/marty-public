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
