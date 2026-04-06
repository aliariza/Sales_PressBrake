#include "quoterepository.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDevice>
#include <QTextStream>
#include <QStringList>
#include <QLocale>
#include <QDateTime>

QuoteRepository::QuoteRepository()
    : m_defaultResourcePath(":/quotes.csv")
{
    const QString appDataDir =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    m_filePath = appDataDir + "/quotes.csv";
}

bool QuoteRepository::initialize(QString &errorMessage)
{
    errorMessage.clear();

    const QFileInfo fileInfo(m_filePath);
    const QString dirPath = fileInfo.absolutePath();

    QDir dir;
    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            errorMessage = "Could not create app data directory:\n" + dirPath;
            return false;
        }
    }

    QFile writableFile(m_filePath);

    if (!writableFile.exists()) {
        QFile resourceFile(m_defaultResourcePath);

        if (!resourceFile.exists()) {
            errorMessage = "Default quotes.csv not found in Qt resources:\n" + m_defaultResourcePath;
            return false;
        }

        if (!resourceFile.copy(m_filePath)) {
            errorMessage = "Could not copy default quotes.csv to writable location:\n" + m_filePath;
            return false;
        }
    }

    QFile permissionsFile(m_filePath);
    if (!permissionsFile.setPermissions(
            QFileDevice::ReadOwner  | QFileDevice::WriteOwner |
            QFileDevice::ReadUser   | QFileDevice::WriteUser  |
            QFileDevice::ReadGroup  |
            QFileDevice::ReadOther)) {
        errorMessage = "Could not set writable permissions on quotes.csv:\n" + m_filePath;
        return false;
    }

    return true;
}

std::optional<QuoteRecord> QuoteRepository::parseQuoteLine(const QString &line) const
{
    const QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty())
        return std::nullopt;

    const QStringList parts = trimmedLine.split("||");

    if (parts.size() < 19)
        return std::nullopt;

    QuoteRecord quote;
    quote.no = parts[0].trimmed();
    quote.quoteCode = parts[1].trimmed();

    quote.customerName = parts[2].trimmed();
    quote.customerAttention = parts[3].trimmed();
    quote.customerAddress = parts[4].trimmed();
    quote.customerTel = parts[5].trimmed();
    quote.customerEmail = parts[6].trimmed();
    quote.customerTaxOffice = parts[7].trimmed();

    quote.material = parts[8].trimmed();
    quote.thicknessMm = parts[9].trimmed();
    quote.bendLengthMm = parts[10].trimmed();
    quote.machineModel = parts[11].trimmed();
    quote.toolingName = parts[12].trimmed();
    quote.optionsData = parts[13].trimmed();
    quote.machinePriceUsd = parts[14].trimmed();
    quote.optionsTotalUsd = parts[15].trimmed();
    quote.grandTotalUsd = parts[16].trimmed();
    quote.notes = parts[17].trimmed();
    quote.createdAt = parts[18].trimmed();

    return quote;
}

bool QuoteRepository::validateQuote(const QuoteRecord &quote, QString &errorMessage) const
{
    errorMessage.clear();

    if (quote.no.trimmed().isEmpty()) {
        errorMessage = "Quote number cannot be empty.";
        return false;
    }

    if (quote.quoteCode.trimmed().isEmpty()) {
        errorMessage = "Quote code cannot be empty.";
        return false;
    }

    if (quote.customerName.trimmed().isEmpty()) {
        errorMessage = "Customer name cannot be empty.";
        return false;
    }

    if (quote.customerAddress.trimmed().isEmpty()) {
        errorMessage = "Customer address cannot be empty.";
        return false;
    }

    if (quote.customerTel.trimmed().isEmpty()) {
        errorMessage = "Customer tel cannot be empty.";
        return false;
    }

    if (quote.customerTaxOffice.trimmed().isEmpty()) {
        errorMessage = "Customer tax office cannot be empty.";
        return false;
    }

    if (quote.material.trimmed().isEmpty()) {
        errorMessage = "Material cannot be empty.";
        return false;
    }

    if (quote.machineModel.trimmed().isEmpty()) {
        errorMessage = "Machine model cannot be empty.";
        return false;
    }

    bool ok = false;

    const double thickness = QLocale::c().toDouble(quote.thicknessMm.trimmed(), &ok);
    if (!ok || thickness <= 0.0) {
        errorMessage = "Thickness must be a valid number greater than 0.";
        return false;
    }

    const double bendLength = QLocale::c().toDouble(quote.bendLengthMm.trimmed(), &ok);
    if (!ok || bendLength <= 0.0) {
        errorMessage = "Bend length must be a valid number greater than 0.";
        return false;
    }

    const double machinePrice = QLocale::c().toDouble(quote.machinePriceUsd.trimmed(), &ok);
    if (!ok || machinePrice < 0.0) {
        errorMessage = "Machine price must be a valid number greater than or equal to 0.";
        return false;
    }

    const double optionsTotal = QLocale::c().toDouble(quote.optionsTotalUsd.trimmed(), &ok);
    if (!ok || optionsTotal < 0.0) {
        errorMessage = "Options total must be a valid number greater than or equal to 0.";
        return false;
    }

    const double grandTotal = QLocale::c().toDouble(quote.grandTotalUsd.trimmed(), &ok);
    if (!ok || grandTotal < 0.0) {
        errorMessage = "Grand total must be a valid number greater than or equal to 0.";
        return false;
    }

    if (quote.createdAt.trimmed().isEmpty()) {
        errorMessage = "Created timestamp cannot be empty.";
        return false;
    }

    return true;
}

QList<QuoteRecord> QuoteRepository::getAllQuotes(QString &errorMessage) const
{
    errorMessage.clear();

    QList<QuoteRecord> quotes;

    QFile file(m_filePath);

    if (!file.exists()) {
        errorMessage = "Writable quotes.csv not found:\n" + m_filePath;
        return quotes;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMessage = "Could not open writable quotes.csv:\n" + m_filePath;
        return quotes;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        const QString line = in.readLine();
        const std::optional<QuoteRecord> quote = parseQuoteLine(line);

        if (!quote.has_value())
            continue;

        quotes.append(*quote);
    }

    return quotes;
}

QString QuoteRepository::getNextQuoteNo(QString &errorMessage) const
{
    errorMessage.clear();

    const QList<QuoteRecord> quotes = getAllQuotes(errorMessage);
    if (!errorMessage.isEmpty())
        return QString();

    int maxNo = 0;

    for (const QuoteRecord &quote : quotes) {
        bool ok = false;
        const int value = quote.no.toInt(&ok);

        if (ok && value > maxNo)
            maxNo = value;
    }

    return QString::number(maxNo + 1);
}

QString QuoteRepository::generateQuoteCode(QString &errorMessage) const
{
    const QString nextNo = getNextQuoteNo(errorMessage);
    if (!errorMessage.isEmpty())
        return QString();

    const QString datePart = QDate::currentDate().toString("yyyyMMdd");
    return "Q-" + datePart + "-" + nextNo;
}

bool QuoteRepository::addQuote(const QuoteRecord &quote, QString &errorMessage) const
{
    errorMessage.clear();

    if (!validateQuote(quote, errorMessage))
        return false;

    QFile file(m_filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        errorMessage = "Could not open writable quotes.csv for appending:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);
    out << quote.no.trimmed() << "||"
        << quote.quoteCode.trimmed() << "||"
        << quote.customerName.trimmed() << "||"
        << quote.customerAttention.trimmed() << "||"
        << quote.customerAddress.trimmed() << "||"
        << quote.customerTel.trimmed() << "||"
        << quote.customerEmail.trimmed() << "||"
        << quote.customerTaxOffice.trimmed() << "||"
        << quote.material.trimmed() << "||"
        << quote.thicknessMm.trimmed() << "||"
        << quote.bendLengthMm.trimmed() << "||"
        << quote.machineModel.trimmed() << "||"
        << quote.toolingName.trimmed() << "||"
        << quote.optionsData.trimmed() << "||"
        << quote.machinePriceUsd.trimmed() << "||"
        << quote.optionsTotalUsd.trimmed() << "||"
        << quote.grandTotalUsd.trimmed() << "||"
        << quote.notes.trimmed() << "||"
        << quote.createdAt.trimmed() << "\n";

    return true;
}

bool QuoteRepository::deleteQuote(const QString &no, QString &errorMessage) const
{
    errorMessage.clear();

    const QString normalizedNo = no.trimmed();

    if (normalizedNo.isEmpty()) {
        errorMessage = "Quote number cannot be empty.";
        return false;
    }

    QList<QuoteRecord> quotes = getAllQuotes(errorMessage);
    if (!errorMessage.isEmpty())
        return false;

    QList<QuoteRecord> filteredQuotes;
    bool found = false;

    for (const QuoteRecord &quote : quotes) {
        if (quote.no.trimmed() == normalizedNo) {
            found = true;
            continue;
        }
        filteredQuotes.append(quote);
    }

    if (!found) {
        errorMessage = "Quote to delete was not found.";
        return false;
    }

    QFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        errorMessage = "Could not open writable quotes.csv for rewriting:\n" + m_filePath;
        return false;
    }

    QTextStream out(&file);

    for (const QuoteRecord &quote : filteredQuotes) {
        out << quote.no.trimmed() << "||"
            << quote.quoteCode.trimmed() << "||"
            << quote.customerName.trimmed() << "||"
            << quote.customerAttention.trimmed() << "||"
            << quote.customerAddress.trimmed() << "||"
            << quote.customerTel.trimmed() << "||"
            << quote.customerEmail.trimmed() << "||"
            << quote.customerTaxOffice.trimmed() << "||"
            << quote.material.trimmed() << "||"
            << quote.thicknessMm.trimmed() << "||"
            << quote.bendLengthMm.trimmed() << "||"
            << quote.machineModel.trimmed() << "||"
            << quote.toolingName.trimmed() << "||"
            << quote.optionsData.trimmed() << "||"
            << quote.machinePriceUsd.trimmed() << "||"
            << quote.optionsTotalUsd.trimmed() << "||"
            << quote.grandTotalUsd.trimmed() << "||"
            << quote.notes.trimmed() << "||"
            << quote.createdAt.trimmed() << "\n";
    }

    return true;
}
