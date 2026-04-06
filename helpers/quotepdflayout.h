#pragma once

#include <QPdfWriter>

struct PdfQuoteLayout
{
    int pageWidth;
    int pageHeight;

    int leftBlockX;
    int rightBlockX;
    int rightBlockW;
    int rightEdge;

    int customerX;
    int customerY;
    int customerW;
    int customerH;
    int customerHeaderH;

    // Product table
    int itemsX;
    int itemsY;
    int itemsW;
    int itemsHeaderH;
    int itemsRowH;

    int colModelW;
    int colProductNameW;
    int colQtyW;
    int colUnitPriceW;
    int colAmountW;

    //Terms box
    int termsX;
    int termsY;
    int termsW;
    int termsH;
    int termsHeaderH;

    explicit PdfQuoteLayout(const QPdfWriter &writer);
};
