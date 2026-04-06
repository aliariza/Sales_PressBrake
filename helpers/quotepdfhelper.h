#pragma once

#include <QFont>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QRect>
#include <QString>

class QuoteRecord;

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

    explicit PdfQuoteLayout(const QPdfWriter &writer);
};

namespace QuotePdfHelper {

void drawGuideRect(QPainter &painter,
                   const QRect &rect,
                   bool showGuides);

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

void drawApprovedHeader(QPainter &painter,
                        const PdfQuoteLayout &layout,
                        const QuoteRecord &quote,
                        bool showGuides);

void drawCustomerOnly(QPainter &painter,
                      const PdfQuoteLayout &layout,
                      const QuoteRecord &quote,
                      bool showGuides);

void drawBoxedSingleLineText(QPainter &painter,
                             const QRect &rect,
                             const QString &text,
                             const QFont &font,
                             Qt::Alignment alignment = Qt::AlignHCenter | Qt::AlignVCenter,
                             int penWidth = 2,
                             const QColor &borderColor = Qt::black);

} // namespace QuotePdfHelper
