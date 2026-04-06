#include "quotepdfhelper.h"

#include <QDate>
#include <QDateTime>
#include <QDebug>

#include "quoterecord.h"

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
    customerHeaderH(70)
{
    qDebug() << "Page Width:" << writer.width();
    qDebug() << "-------------------------------";
    qDebug() << "Page Height:" << writer.height();
    qDebug() << "-------------------------------";
    qDebug() << "Right Block Width:" << rightBlockW;
    qDebug() << "-------------------------------";
    qDebug() << "Right Block Starting Point:" << rightBlockX;
}

namespace QuotePdfHelper {

namespace {

QString formatQuoteDate(const QString &createdAt)
{
    QDate baseDate;

    const QDateTime dt = QDateTime::fromString(createdAt, Qt::ISODate);
    if (dt.isValid()) {
        baseDate = dt.date();
    } else {
        baseDate = QDate::fromString(createdAt, "yyyy/MM/dd");
    }

    if (!baseDate.isValid()) {
        baseDate = QDate(2026, 3, 17);
    }

    return baseDate.toString("yyyy/MM/dd");
}

QString formatValidUntil(const QString &createdAt, int daysToAdd = 15)
{
    QDate baseDate;

    const QDateTime dt = QDateTime::fromString(createdAt, Qt::ISODate);
    if (dt.isValid()) {
        baseDate = dt.date();
    } else {
        baseDate = QDate::fromString(createdAt, "yyyy/MM/dd");
    }

    if (!baseDate.isValid()) {
        baseDate = QDate(2026, 3, 17);
    }

    return baseDate.addDays(daysToAdd).toString("yyyy/MM/dd");
}

void drawThickHorizontalLine(QPainter &painter, int y, int pageWidth, int penWidth = 2)
{
    QPen oldPen = painter.pen();

    QPen pen(Qt::black);
    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.drawLine(0, y, pageWidth, y);

    painter.setPen(oldPen);
}

} // namespace

void drawGuideRect(QPainter &painter, const QRect &rect, bool showGuides)
{
    if (showGuides) {
        painter.drawRect(rect);
    }
}

void drawTextBlock(QPainter &painter,
                   const QRect &rect,
                   const QString &text,
                   const QFont &font,
                   bool showGuides,
                   Qt::Alignment alignment)
{
    painter.setFont(font);
    drawGuideRect(painter, rect, showGuides);
    painter.drawText(rect, alignment | Qt::TextWordWrap, text);
}

void drawSingleLineTextBlock(QPainter &painter,
                             const QRect &rect,
                             const QString &text,
                             const QFont &font,
                             bool showGuides,
                             Qt::Alignment alignment)
{
    painter.setFont(font);
    drawGuideRect(painter, rect, showGuides);
    painter.drawText(rect, alignment, text);
}

void drawBoxedSingleLineText(QPainter &painter,
                             const QRect &rect,
                             const QString &text,
                             const QFont &font,
                             Qt::Alignment alignment,
                             int penWidth,
                             const QColor &borderColor)
{
    QPen oldPen = painter.pen();

    QPen boxPen(borderColor);
    boxPen.setWidth(penWidth);
    painter.setPen(boxPen);
    painter.drawRect(rect);

    painter.setPen(oldPen);
    painter.setFont(font);

    QRect innerRect = rect.adjusted(6, 0, -6, 0);
    painter.drawText(innerRect, alignment, text);
}

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
    const int headerH = 450;

    const int leftW = headerW / 2;
    const int rightX = headerX + leftW;
    const int rightW = headerW - leftW;

    const int logoW = 400;
    const int logoH = 200;

    QRect headerRect(headerX, headerY, headerW, headerH);
    drawGuideRect(painter, headerRect, showGuides);

    // Logo
    QPixmap logo(":/images/tumex_blue.png");
    QRect logoRect(headerX, headerY, logoW, logoH);
    drawGuideRect(painter, logoRect, showGuides);

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

    // Left company block
    QRect companyNameRect(headerX, logoH, leftW, 70);
    QRect companyLine1Rect(headerX, logoH + 70, leftW, 60);
    QRect companyLine2Rect(headerX, logoH + 130, leftW, 60);
    QRect companyLine3Rect(headerX, logoH + 190, leftW, 60);

    drawGuideRect(painter, companyNameRect, showGuides);
    drawGuideRect(painter, companyLine1Rect, showGuides);
    drawGuideRect(painter, companyLine2Rect, showGuides);
    drawGuideRect(painter, companyLine3Rect, showGuides);

    drawSingleLineTextBlock(painter, companyNameRect,
                            "Tumex Mümessillik ve Dış Ticaret Ltd. Şti.",
                            companyFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, companyLine1Rect,
                            "Birlik Mah. 408. Sok. No:9/2 Evkur Birlik Apt.",
                            smallFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, companyLine2Rect,
                            "Çankaya / Ankara",
                            smallFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter, companyLine3Rect,
                            "info@tum-ex.com  |  +90 530 712 4897",
                            smallFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    // Right title block
    QRect titleRect(rightX, headerY, rightW, 200);
    drawGuideRect(painter, titleRect, showGuides);

    drawSingleLineTextBlock(painter, titleRect,
                            "TEKLİF",
                            titleFont,
                            false,
                            Qt::AlignRight | Qt::AlignVCenter);

    // Right meta block
    const int metaX = rightX;
    const int metaY = headerY + logoH;
    const int metaLabelW = rightW / 2;
    const int metaValueW = rightW / 2;
    const int metaLabelInset = 20;

    const QString formattedDate = formatQuoteDate(quote.createdAt);
    const QString validUntil = formatValidUntil(quote.createdAt);
    const QString quoteCode = quote.quoteCode.isEmpty() ? "TMX-2024-001" : quote.quoteCode;

    struct MetaRow
    {
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
        QRect labelRect(metaX,
                        currentY,
                        metaLabelW - metaLabelInset,
                        row.rowH);

        QRect valueRect(metaX + metaLabelW,
                        currentY,
                        metaValueW,
                        row.rowH);

        drawGuideRect(painter, labelRect, showGuides);

        drawSingleLineTextBlock(painter, labelRect,
                                row.label,
                                metaFont,
                                false,
                                Qt::AlignRight | Qt::AlignVCenter);

        drawBoxedSingleLineText(painter, valueRect,
                                row.value,
                                metaFont,
                                Qt::AlignHCenter | Qt::AlignVCenter,
                                2,
                                fieldBorderColor);

        currentY += row.rowH;
    }

    drawThickHorizontalLine(painter, 500, layout.pageWidth);
}

void drawCustomerOnly(QPainter &painter,
                      const PdfQuoteLayout &layout,
                      const QuoteRecord &quote,
                      bool showGuides)
{
    Q_UNUSED(quote);

    QFont sectionFont("Arial", 11);
    QFont customerFont("Arial", 10);

    const int leftW = layout.customerW;

    QRect customerHeaderRect(layout.customerX,
                             layout.customerY,
                             leftW,
                             layout.customerHeaderH);

    QRect customerHeaderInnerRect = customerHeaderRect.adjusted(10, 0, 0, 0);

    drawSingleLineTextBlock(painter,
                            customerHeaderInnerRect,
                            "DİKKATİNE:",
                            sectionFont,
                            showGuides,
                            Qt::AlignLeft | Qt::AlignVCenter);

    const int linesLeft = layout.customerX;
    const int linesRight = layout.customerX + leftW;
    const int lineGap = 60;
    const int startY = layout.customerY + layout.customerHeaderH;

    // Background guide lines
    for (int i = 0; i < 6; ++i) {
        const int y = startY + i * lineGap;
        painter.drawLine(linesLeft, y, linesRight, y);
    }

    // Hardcoded customer text for visual testing
    const int textLeft = linesLeft + 10;
    const int textW = leftW - 20;
    const int textH = 60;
    const int textTop = startY;

    QRect customerLine1Rect(textLeft, textTop + 0 * lineGap, textW, textH);
    QRect customerLine2Rect(textLeft, textTop + 1 * lineGap, textW, textH);
    QRect customerLine3Rect(textLeft, textTop + 2 * lineGap, textW, textH);
    QRect customerLine4Rect(textLeft, textTop + 3 * lineGap, textW, textH);

    drawSingleLineTextBlock(painter,
                            customerLine1Rect,
                            "M-X Firması",
                            customerFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter,
                            customerLine2Rect,
                            "Sayın Mehmet Yılmaz",
                            customerFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter,
                            customerLine3Rect,
                            "Organize Sanayi Bölgesi 2. Cad. No:15",
                            customerFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawSingleLineTextBlock(painter,
                            customerLine4Rect,
                            "Konya / Türkiye",
                            customerFont,
                            false,
                            Qt::AlignLeft | Qt::AlignVCenter);

    drawThickHorizontalLine(painter, 970, layout.pageWidth);
}

} // namespace QuotePdfHelper
