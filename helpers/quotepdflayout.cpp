#include "quotepdflayout.h"

#include <QDebug>

PdfQuoteLayout::PdfQuoteLayout(const QPdfWriter &writer)
    : pageWidth(writer.width()),
    pageHeight(writer.height()),
    leftBlockX(0),
    rightBlockX(1000),
    rightBlockW(pageWidth - rightBlockX),
    rightEdge(rightBlockX + rightBlockW),
    customerX(0),
    customerY(550),
    customerW(pageWidth / 2),
    customerH(300),
    customerHeaderH(70),

    itemsX(0),
    itemsY(1000),
    itemsW(pageWidth),
    itemsHeaderH(75),
    itemsRowH(70),

    colModelW(450),
    colProductNameW(750),
    colQtyW(200),
    colUnitPriceW(350),
    colAmountW(pageWidth - (450 + 750 + 200 + 350)),

    termsX(0),
    termsY(0), // set later from the table bottom
    termsW(pageWidth),
    termsH(500),
    termsHeaderH(70)
{
    qDebug() << "Page Width:" << writer.width();
    qDebug() << "-------------------------------";
    qDebug() << "Page Height:" << writer.height();
    qDebug() << "-------------------------------";
    qDebug() << "Right Block Width:" << rightBlockW;
    qDebug() << "-------------------------------";
    qDebug() << "Right Block Starting Point:" << rightBlockX;
    qDebug() << "-------------------------------";
    qDebug() << "Column Amount Width:" << colAmountW;
}
