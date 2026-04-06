#include "quotepdfcommon.h"

#include <QDate>
#include <QDateTime>
#include <QPen>

namespace QuotePdfCommon {

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

void drawThickHorizontalLine(QPainter &painter, int y, int pageWidth, int penWidth)
{
    QPen oldPen = painter.pen();

    QPen pen(Qt::black);
    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.drawLine(0, y, pageWidth, y);

    painter.setPen(oldPen);
}

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

QString formatValidUntil(const QString &createdAt, int daysToAdd)
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

} // namespace QuotePdfCommon
