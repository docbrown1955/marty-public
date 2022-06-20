#include "pdfsetupdialog.h"
#include "ui_pdfsetupdialog.h"
#include <QFileDialog>
#include <QImage>
#include <iostream>

PDFSetupDialog::PDFSetupDialog(MainWidget *parent)
    : QDialog(parent), mainWidget(parent), ui(new Ui::PDFSetupDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    loadOptions();
    renderPreview();
}

PDFSetupDialog::~PDFSetupDialog()
{
    delete ui;
}

void PDFSetupDialog::accept()
{
    if (saveFile.isEmpty()) {
        reject();
        return;
    }
    QDialog::accept();
}

void PDFSetupDialog::reject()
{
    QDialog::reject();
}

void PDFSetupDialog::renderPreview()
{
    options.width  = ui->preview->width();
    options.height = ui->preview->height();
    QImage image(ui->preview->width(),
                 ui->preview->height(),
                 QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    QPainter painter(&image);
    mainWidget->renderPDF(painter, options);
    ui->preview->setPixmap(QPixmap::fromImage(image));
    update();
}

void PDFSetupDialog::loadOptions()
{
    ui->rowSizeBox->setValue(options.rowSize);
    ui->hMarginSlider->setSliderPosition(
        fromRatio(options.horizontalMarginRatio));
    ui->hSpacingSlider->setSliderPosition(
        fromRatio(options.horizontalSpacingRatio));
    ui->vMarginSlider->setSliderPosition(
        fromRatio(options.verticalMarginRatio));
    ui->vSpacingSlider->setSliderPosition(
        fromRatio(options.verticalSpacingRatio));
    ui->numberBox->setChecked(options.showNumbers);
}

void PDFSetupDialog::browsePath()
{
    saveFile = QFileDialog::getSaveFileName(
        this, tr("Choose File"), "", tr("PDF files (*.pdf)"));
    if (!saveFile.isEmpty()) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
        ui->pathEdit->setText(saveFile);
    }
}

int PDFSetupDialog::fromRatio(float ratio)
{
    return int(ratio * 100);
}

void PDFSetupDialog::setRatio(int ratio, float &target)
{
    target = ratio * 0.01;
}

void PDFSetupDialog::setRowSize(int size)
{
    options.rowSize = size;
    renderPreview();
}

void PDFSetupDialog::setHMarginRatio(int ratio)
{
    setRatio(ratio, options.horizontalMarginRatio);
    renderPreview();
}

void PDFSetupDialog::setHSpacingRatio(int ratio)
{
    setRatio(ratio, options.horizontalSpacingRatio);
    renderPreview();
}

void PDFSetupDialog::setVMarginRatio(int ratio)
{
    setRatio(ratio, options.verticalMarginRatio);
    renderPreview();
}

void PDFSetupDialog::setVSpacingRatio(int ratio)
{
    setRatio(ratio, options.verticalSpacingRatio);
    renderPreview();
}

void PDFSetupDialog::enableNumbers(bool enable)
{
    options.showNumbers = enable;
    renderPreview();
}
