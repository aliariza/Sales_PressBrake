#include "quotepdfitems.h"

#include <QFont>
#include <QRect>

#include "quotepdflayout.h"
#include "quotepdfcommon.h"
#include "quotepdfdata.h"

using namespace QuotePdfCommon;

namespace QuotePdfItems {
static void drawTableCell(QPainter &painter,
                          const QRect &rect,
                          const QString &text,
                          const QFont &font,
                          Qt::Alignment alignment,
                          bool showGuides)
{
    painter.save();
    painter.setFont(font);
    painter.drawRect(rect);

    QRect innerRect = rect.adjusted(12, 0, -12, 0);

    drawSingleLineTextBlock(painter,
                            innerRect,
                            text,
                            font,
                            showGuides,
                            alignment | Qt::AlignVCenter);
    painter.restore();
}

int drawItemsTable(QPainter &painter,
                   const PdfQuoteLayout &layout,
                   const QuotePdfData &data,
                   bool showGuides)
{
    QFont headerFont("Arial", 10);
    headerFont.setBold(true);

    QFont rowFont("Arial", 10);

    const int x = layout.itemsX;
    const int y = layout.itemsY;

    const int headerH = layout.itemsHeaderH;
    const int rowH = layout.itemsRowH;

    const int w1 = layout.colModelW;
    const int w2 = layout.colProductNameW;
    const int w3 = layout.colQtyW;
    const int w4 = layout.colUnitPriceW;
    const int w5 = layout.colAmountW;

    int currentX = x;

    QRect h1(currentX, y, w1, headerH);
    currentX += w1;
    QRect h2(currentX, y, w2, headerH);
    currentX += w2;
    QRect h3(currentX, y, w3, headerH);
    currentX += w3;
    QRect h4(currentX, y, w4, headerH);
    currentX += w4;
    QRect h5(currentX, y, w5, headerH);

    drawTableCell(painter, h1, "Model", headerFont, Qt::AlignCenter, showGuides);
    drawTableCell(painter, h2, QString::fromUtf8("Ürün Adı"), headerFont, Qt::AlignCenter, showGuides);
    drawTableCell(painter, h3, QString::fromUtf8("Miktar"), headerFont, Qt::AlignCenter, showGuides);
    drawTableCell(painter, h4, QString::fromUtf8("Birim Fiyat"), headerFont, Qt::AlignCenter, showGuides);
    drawTableCell(painter, h5, QString::fromUtf8("Tutar"), headerFont, Qt::AlignCenter, showGuides);

    int currentY = y + headerH;

    for (const auto &item : data.mainItems) {
        currentX = x;

        QRect r1(currentX, currentY, w1, rowH);
        currentX += w1;
        QRect r2(currentX, currentY, w2, rowH);
        currentX += w2;
        QRect r3(currentX, currentY, w3, rowH);
        currentX += w3;
        QRect r4(currentX, currentY, w4, rowH);
        currentX += w4;
        QRect r5(currentX, currentY, w5, rowH);

        drawTableCell(painter, r1, item.model, rowFont, Qt::AlignLeft, showGuides);
        drawTableCell(painter, r2, item.productName, rowFont, Qt::AlignLeft, showGuides);
        drawTableCell(painter, r3, item.quantity, rowFont, Qt::AlignCenter, showGuides);
        drawTableCell(painter, r4, item.unitPrice, rowFont, Qt::AlignRight, showGuides);
        drawTableCell(painter, r5, item.amount, rowFont, Qt::AlignRight, showGuides);

        currentY += rowH;
    }
    const int emptyRowCount = 3;

    for (int i = 0; i < emptyRowCount; ++i) {
        currentX = x;

        QRect r1(currentX, currentY, w1, rowH);
        currentX += w1;
        QRect r2(currentX, currentY, w2, rowH);
        currentX += w2;
        QRect r3(currentX, currentY, w3, rowH);
        currentX += w3;
        QRect r4(currentX, currentY, w4, rowH);
        currentX += w4;
        QRect r5(currentX, currentY, w5, rowH);

        drawTableCell(painter, r1, "", rowFont, Qt::AlignLeft, showGuides);
        drawTableCell(painter, r2, "", rowFont, Qt::AlignLeft, showGuides);
        drawTableCell(painter, r3, "", rowFont, Qt::AlignCenter, showGuides);
        drawTableCell(painter, r4, "", rowFont, Qt::AlignRight, showGuides);
        drawTableCell(painter, r5, "", rowFont, Qt::AlignRight, showGuides);

        currentY += rowH;
    }

    for (const auto &group : data.groupedItems) {
        const int groupHeight = group.rows.size() * rowH;

        QRect mergedModelRect(x, currentY, w1, groupHeight);
        drawTableCell(painter,
                      mergedModelRect,
                      group.groupTitle,
                      rowFont,
                      Qt::AlignCenter,
                      showGuides);

        int groupRowY = currentY;

        for (const auto &row : group.rows) {
            QRect productRect(x + w1, groupRowY, w2, rowH);
            QRect qtyRect(x + w1 + w2, groupRowY, w3, rowH);
            QRect unitPriceRect(x + w1 + w2 + w3, groupRowY, w4, rowH);
            QRect totalRect(x + w1 + w2 + w3 + w4, groupRowY, w5, rowH);

            drawTableCell(painter, productRect, row.productName, rowFont, Qt::AlignLeft, showGuides);
            drawTableCell(painter, qtyRect, row.quantity, rowFont, Qt::AlignCenter, showGuides);
            drawTableCell(painter, unitPriceRect, row.unitPrice, rowFont, Qt::AlignCenter, showGuides);
            drawTableCell(painter, totalRect, row.amount, rowFont, Qt::AlignCenter, showGuides);

            groupRowY += rowH;
        }

        currentY += groupHeight;
    }

    const int totalLabelW = w1 + w2 + w3 + w4;

    QFont totalFont("Arial", 10);
    totalFont.setBold(true);

    QRect totalLabelRect(x, currentY, totalLabelW, rowH);
    QRect totalValueRect(x + totalLabelW, currentY, w5, rowH);

    drawTableCell(painter,
                  totalLabelRect,
                  QString::fromUtf8("GENEL TOPLAM"),
                  totalFont,
                  Qt::AlignRight,
                  showGuides);

    drawTableCell(painter,
                  totalValueRect,
                  data.terms.grandTotal,
                  totalFont,
                  Qt::AlignRight,
                  showGuides);

    currentY += rowH;

    const int fullTableW = w1 + w2 + w3 + w4 + w5;

    QFont wordsFont("Arial", 10);

    QRect amountInWordsRect(x, currentY, fullTableW, rowH);

    drawTableCell(painter,
                  amountInWordsRect,
                  data.terms.amountInWords,
                  wordsFont,
                  Qt::AlignRight,
                  showGuides);

    currentY += rowH;

    return currentY;
}

void drawOtherTermsBlock(QPainter &painter,
                         const PdfQuoteLayout &layout,
                         const QuotePdfData &data,
                         int topY,
                         bool showGuides)
{
    Q_UNUSED(showGuides);

    const int x = layout.termsX;
    const int y = topY;
    const int w = layout.termsW;
    const int h = layout.termsH;
    const int headerH = layout.termsHeaderH;

    QFont headerFont("Arial", 11);
    headerFont.setBold(true);

    QFont bodyFont("Arial", 10);

    QRect outerRect(x, y, w, h);
    QRect headerRect(x, y, w, headerH);
    QRect bodyRect(x, y + headerH, w, h - headerH);

    painter.drawRect(outerRect);
    painter.drawLine(x, y + headerH, x + w, y + headerH);

    QRect headerInner = headerRect.adjusted(12, 0, -12, 0);
    drawSingleLineTextBlock(painter,
                            headerInner,
                            QString::fromUtf8("Diğer Şartlar"),
                            headerFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    QRect bodyInner = bodyRect.adjusted(12, 12, -12, -12);

    painter.save();
    painter.setFont(bodyFont);
    painter.drawText(bodyInner,
                     Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                     data.terms.otherTerms);
    painter.restore();
}
}
