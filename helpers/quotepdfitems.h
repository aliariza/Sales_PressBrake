#pragma once

#include <QPainter>

struct PdfQuoteLayout;
struct QuotePdfData;

namespace QuotePdfItems {

int drawItemsTable(QPainter &painter,
                   const PdfQuoteLayout &layout,
                   const QuotePdfData &data,
                   bool showGuides);

void drawOtherTermsBlock(QPainter &painter,
                         const PdfQuoteLayout &layout,
                         const QuotePdfData &data,
                         int topY,
                         bool showGuides);

} // namespace QuotePdfItems
