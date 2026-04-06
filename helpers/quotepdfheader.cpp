#include "quotepdfheader.h"

#include <QDebug>
#include <QFont>
#include <QPixmap>

#include "quotepdflayout.h"
#include "quotepdfcommon.h"
#include "quoterecord.h"

namespace QuotePdfHeader {

// =========================
// APPROVED HEADER LAYOUT
// Locked baseline for PDF export
// =========================
void drawApprovedHeader(QPainter &painter,
                        const PdfQuoteLayout &layout,
                        const QuoteRecord &quote,
                        bool showGuides)
{
    QFont companyFont("Arial", 12, QFont::DemiBold);
    QFont smallFont("Arial", 10);
    QFont titleFont("Arial", 22, QFont::DemiBold);
    QFont metaFont("Arial", 10);

    const QColor fieldBorderColor(180, 180, 180);

    const int headerX = 0;
    const int headerY = 0;
    const int headerW = layout.rightEdge - headerX;

    const int leftW = headerW / 2;
    const int rightX = headerX + leftW;
    const int rightW = headerW - leftW;

    const int logoW = 400;
    const int logoH = 200;

    QRect headerRect(headerX, headerY, headerW, 450);
    QuotePdfCommon::drawGuideRect(painter, headerRect, showGuides);

    QPixmap logo(":/images/tumex_blue.png");
    QRect logoRect(headerX, headerY, logoW, logoH);
    QuotePdfCommon::drawGuideRect(painter, logoRect, showGuides);

    if (!logo.isNull()) {
        QPixmap scaledLogo = logo.scaled(logoRect.size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);

        const int logoX = logoRect.x() + (logoRect.width() - scaledLogo.width()) / 2;
        const int logoY = logoRect.y() + (logoRect.height() - scaledLogo.height()) / 2;
        painter.drawPixmap(logoX, logoY, scaledLogo);
    } else {
        qDebug() << "Logo failed to load";
        painter.drawRect(logoRect);
    }

    QRect companyNameRect(headerX, logoH, leftW, 70);
    QRect companyLine1Rect(headerX, logoH + 70, leftW, 60);
    QRect companyLine2Rect(headerX, logoH + 130, leftW, 60);
    QRect companyLine3Rect(headerX, logoH + 190, leftW, 60);

    QuotePdfCommon::drawGuideRect(painter, companyNameRect, showGuides);
    QuotePdfCommon::drawGuideRect(painter, companyLine1Rect, showGuides);
    QuotePdfCommon::drawGuideRect(painter, companyLine2Rect, showGuides);
    QuotePdfCommon::drawGuideRect(painter, companyLine3Rect, showGuides);

    QuotePdfCommon::drawSingleLineTextBlock(painter, companyNameRect,
                            "Tumex Mümessillik ve Dış Ticaret Ltd. Şti.",
                            companyFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    QuotePdfCommon::drawSingleLineTextBlock(painter, companyLine1Rect,
                            "Birlik Mah. 408. Sok. No:9/2 Evkur Birlik Apt.",
                            smallFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    QuotePdfCommon::drawSingleLineTextBlock(painter, companyLine2Rect,
                            "Çankaya / Ankara",
                            smallFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    QuotePdfCommon::drawSingleLineTextBlock(painter, companyLine3Rect,
                            "info@tum-ex.com  |  +90 530 712 4897",
                            smallFont, false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    QRect titleRect(rightX, headerY, rightW, 200);
    QuotePdfCommon::drawGuideRect(painter, titleRect, showGuides);

    QuotePdfCommon::drawSingleLineTextBlock(painter, titleRect,
                            "TEKLİF",
                            titleFont, false,
                            Qt::AlignRight | Qt::AlignVCenter);

    const int metaX = rightX;
    const int metaY = headerY + logoH;
    const int metaLabelW = rightW / 2;
    const int metaValueW = rightW / 2;
    const int metaLabelInset = 20;

    const QString formattedDate = QuotePdfCommon::formatQuoteDate(quote.createdAt);
    const QString validUntil = QuotePdfCommon::formatValidUntil(quote.createdAt);
    const QString quoteCode = quote.quoteCode.isEmpty() ? "TMX-2024-001" : quote.quoteCode;

    struct MetaRow {
        QString label;
        QString value;
        int rowH;
    };

    const MetaRow rows[] = {
        { "TARİH",      formattedDate, 70 },
        { "TEKLİF #",   quoteCode,     60 },
        { "GEÇERLİLİK", validUntil,    60 },
        { "MÜŞTERİ #",  "M-X Firması", 60 }
    };

    int currentY = metaY;

    for (const MetaRow &row : rows) {
        QRect labelRect(metaX, currentY, metaLabelW - metaLabelInset, row.rowH);
        QRect valueRect(metaX + metaLabelW, currentY, metaValueW, row.rowH);

        QuotePdfCommon::drawGuideRect(painter, labelRect, showGuides);

        QuotePdfCommon::drawSingleLineTextBlock(painter, labelRect,
                                row.label, metaFont, false,
                                Qt::AlignRight | Qt::AlignVCenter);

        QuotePdfCommon::drawBoxedSingleLineText(painter, valueRect,
                                row.value, metaFont,
                                Qt::AlignHCenter | Qt::AlignVCenter,
                                2, fieldBorderColor);

        currentY += row.rowH;
    }

    QuotePdfCommon::drawThickHorizontalLine(painter, 500, layout.pageWidth);
}

} // namespace QuotePdfHeader
