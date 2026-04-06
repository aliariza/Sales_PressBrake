#ifndef QUOTEPDFDATABUILDER_H
#define QUOTEPDFDATABUILDER_H

#include "models/quotepdfdata.h"
#include "models/quoterecord.h"

class QuotePdfDataBuilder
{
public:
    static QuotePdfData buildFromQuoteRecord(const QuoteRecord &quote);
};

#endif // QUOTEPDFDATABUILDER_H
