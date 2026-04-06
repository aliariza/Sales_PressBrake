#pragma once

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QString>

namespace QuotePdfCommon {

void drawGuideRect(QPainter &painter, const QRect &rect, bool showGuides);

void drawTextBlock(QPainter &painter,
                   const QRect &rect,
                   const QString &text,
                   const QFont &font,
                   bool showGuides,
                   Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter);

void drawSingleLineTextBlock(QPainter &painter,
                             const QRect &rect,
                             const QString &text,
                             const QFont &font,
                             bool showGuides,
                             Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter);

void drawBoxedSingleLineText(QPainter &painter,
                             const QRect &rect,
                             const QString &text,
                             const QFont &font,
                             Qt::Alignment alignment = Qt::AlignHCenter | Qt::AlignVCenter,
                             int penWidth = 2,
                             const QColor &borderColor = QColor(180, 180, 180));

void drawThickHorizontalLine(QPainter &painter,
                             int y,
                             int pageWidth,
                             int penWidth = 2);

QString formatQuoteDate(const QString &createdAt);
QString formatValidUntil(const QString &createdAt, int daysToAdd = 15);

} // namespace QuotePdfCommon
