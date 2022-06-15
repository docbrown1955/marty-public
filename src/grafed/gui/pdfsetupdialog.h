#ifndef PDFSETUPDIALOG_H
#define PDFSETUPDIALOG_H

#include "mainwidget.h"
#include "pdfoption.h"
#include <QDialog>

namespace Ui {
class PDFSetupDialog;
}

class PDFSetupDialog : public QDialog {
    Q_OBJECT

  public:
    explicit PDFSetupDialog(MainWidget *parent = nullptr);
    ~PDFSetupDialog();

    PDFOption getOptions() const
    {
        return options;
    }

    QString getSaveFile() const
    {
        return saveFile;
    }

  public slots:

    void accept() override;
    void reject() override;

    void renderPreview();

    void browsePath();

    void setRowSize(int);
    void setHMarginRatio(int);
    void setHSpacingRatio(int);
    void setVMarginRatio(int);
    void setVSpacingRatio(int);

    void enableNumbers(bool);

  protected:
    void loadOptions();

    int  fromRatio(float ratio);
    void setRatio(int value, float &target);

  private:
    MainWidget *        mainWidget;
    Ui::PDFSetupDialog *ui;
    PDFOption           options;
    QString             saveFile;
};

#endif // PDFSETUPDIALOG_H
