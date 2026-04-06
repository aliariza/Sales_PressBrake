#ifndef QUOTEREPOSITORY_H
#define QUOTEREPOSITORY_H

#include <QString>
#include <QList>
#include <optional>
#include "quoterecord.h"

class QuoteRepository
{
public:
    QuoteRepository();

    bool initialize(QString &errorMessage);

    QList<QuoteRecord> getAllQuotes(QString &errorMessage) const;
    QString getNextQuoteNo(QString &errorMessage) const;
    QString generateQuoteCode(QString &errorMessage) const;

    bool addQuote(const QuoteRecord &quote, QString &errorMessage) const;
    bool deleteQuote(const QString &no, QString &errorMessage) const;

private:
    std::optional<QuoteRecord> parseQuoteLine(const QString &line) const;
    bool validateQuote(const QuoteRecord &quote, QString &errorMessage) const;

    QString m_defaultResourcePath;
    QString m_filePath;
};

#endif // QUOTEREPOSITORY_H
