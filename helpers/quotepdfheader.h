#pragma once

#include <QPainter>

class QuoteRecord;
struct PdfQuoteLayout;

namespace QuotePdfHeader {

void drawApprovedHeader(QPainter &painter,
                        const PdfQuoteLayout &layout,
                        const QuoteRecord &quote,
                        bool showGuides);

} // namespace QuotePdfHeader
