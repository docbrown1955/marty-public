/*!
 * \file pdfoption.h
 * \brief Contains the PDFOption struct for GRAFED PDF output.
 */
#ifndef PDFOPTION_H
#define PDFOPTION_H

#include <QPdfWriter>

/*!
 * \brief Contains options for the PDF output of GRAFED
 */
struct PDFOption {

    /*! Ratio between height and width of a diagram box in the pdf. */
    float imageRatio = 1.;

    /*! Number of diagrams per row in the PDF. */
    int rowSize = 3;

    /*! Ratio for the horizontal margin (with respect to the page width). */
    float horizontalMarginRatio = 0.05;

    /*! Ratio for the vertical margin (with respect to the page height). */
    float verticalMarginRatio = 0.05;

    /*! Ratio for the horizontal spacing between daigrams (with respect to the page width). */
    float horizontalSpacingRatio = 0.05;

    /*! Ratio for the vertical spacing between daigrams (with respect to the page height). */
    float verticalSpacingRatio = 0.05;

    /*! Width of the pdf page if a PDF printer has been defined. */
    int width = 0;

    /*! Height of the pdf page if a PDF printer has been defined. */
    int height = 0;

    /*! Tells if numbers must be displayed on the top-left of diagrams. */
    bool showNumbers = true;

    /*! Returns the PDF printer of the option (can be nullptr). */
    QPdfWriter const *getPDFWriter() const {
        return writer;
    }

    /*! Returns the PDF printer of the option (can be nullptr). */
    QPdfWriter *getPDFWriter() {
        return writer;
    }

    /*! Sets the PDF printer, together with the width and height of the page. */
    void setPDFWriter(QPdfWriter *printer) {
        writer = printer;
        if (writer) {
            width = writer->width();
            height = writer->height();
        }
    }

private:

    /*! PDF printer. */
    QPdfWriter *writer = nullptr;
};


#endif // PDFOPTION_H
