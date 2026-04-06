#pragma once

#include <QPainter>

struct PdfQuoteLayout;
struct QuotePdfData;

namespace QuotePdfCustomer {

void drawCustomerOnly(QPainter &painter,
                      const PdfQuoteLayout &layout,
                      const QuotePdfData &data,
                      bool showGuides);

} // namespace QuotePdfCustomer
