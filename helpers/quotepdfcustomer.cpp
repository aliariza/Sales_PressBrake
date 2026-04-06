#include "quotepdfcustomer.h"

#include <QFont>
#include <QRect>

#include "quotepdflayout.h"
#include "quotepdfcommon.h"
#include "quotepdfdata.h"

using namespace QuotePdfCommon;

namespace QuotePdfCustomer {

void drawCustomerOnly(QPainter &painter,
                      const PdfQuoteLayout &layout,
                      const QuotePdfData &data,
                      bool showGuides)
{
    QFont sectionFont("Arial", 11);
    QFont customerFont("Arial", 10);

    const int leftW = layout.customerW;

    QRect customerHeaderRect(layout.customerX,
                             layout.customerY,
                             leftW,
                             layout.customerHeaderH);

    QRect customerHeaderInnerRect = customerHeaderRect.adjusted(10, 0, 0, 0);

    drawSingleLineTextBlock(painter, customerHeaderInnerRect,
                            "DİKKATİNE:",
                            sectionFont,
                            showGuides,
                            Qt::AlignLeft | Qt::AlignVCenter);

    const int linesLeft = layout.customerX;
    const int linesRight = layout.customerX + leftW;
    const int lineGap = 60;
    const int startY = layout.customerY + layout.customerHeaderH;

    for (int i = 0; i < 7; ++i) {
        const int y = startY + i * lineGap;
        painter.drawLine(linesLeft, y, linesRight, y);
    }

    const int textLeft = linesLeft + 10;
    const int textW = leftW - 20;
    const int textH = 60;
    const int textTop = startY;

    QRect customerLine1Rect(textLeft, textTop + 0 * lineGap, textW, textH);
    QRect customerLine2Rect(textLeft, textTop + 1 * lineGap, textW, textH);
    QRect customerLine3Rect(textLeft, textTop + 2 * lineGap, textW, textH);
    QRect customerLine4Rect(textLeft, textTop + 3 * lineGap, textW, textH);
    QRect customerLine5Rect(textLeft, textTop + 4 * lineGap, textW, textH);
    QRect customerLine6Rect(textLeft, textTop + 5 * lineGap, textW, textH);

    QString address1 = data.customer.address;
    QString address2;

    const int splitPos = data.customer.address.lastIndexOf(' ', data.customer.address.size() / 2);

    if (splitPos > 0) {
        address1 = data.customer.address.left(splitPos).trimmed();
        address2 = data.customer.address.mid(splitPos + 1).trimmed();
    }

    drawSingleLineTextBlock(painter, customerLine1Rect,
                            data.customer.name,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, customerLine2Rect,
                            data.customer.attention,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, customerLine3Rect,
                            address1,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, customerLine4Rect,
                            address2,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, customerLine5Rect,
                            "Tel: " + data.customer.tel + "   E-posta: " + data.customer.email,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, customerLine6Rect,
                            QString::fromUtf8("Vergi Dairesi: ") + data.customer.taxOffice,
                            customerFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);
}

} // namespace QuotePdfCustomer
