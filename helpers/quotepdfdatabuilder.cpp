#include "quotepdfdatabuilder.h"

#include <QLocale>
#include <QStringList>

static QString formatUsdForPdf(const QString &text)
{
    bool ok = false;
    const double value = QLocale::c().toDouble(text.trimmed(), &ok);
    if (!ok)
        return text.trimmed();

    const qlonglong whole = static_cast<qlonglong>(value + 0.5);

    QLocale enUS(QLocale::English, QLocale::UnitedStates);
    return "$" + enUS.toString(whole) + ".-";
}

static QString convertTens(int value)
{
    static const QStringList ones = {
        "", "Bir", "İki", "Üç", "Dört", "Beş", "Altı", "Yedi", "Sekiz", "Dokuz"
    };

    static const QStringList tens = {
        "", "On", "Yirmi", "Otuz", "Kırk", "Elli", "Altmış", "Yetmiş", "Seksen", "Doksan"
    };

    if (value == 0)
        return "";

    if (value < 10)
        return ones[value];

    if (value < 100) {
        const int t = value / 10;
        const int o = value % 10;
        return tens[t] + (o > 0 ? " " + ones[o] : "");
    }

    const int h = value / 100;
    const int rest = value % 100;

    QString result;
    if (h == 1)
        result = "Yüz";
    else
        result = ones[h] + " Yüz";

    const QString restText = convertTens(rest);
    if (!restText.isEmpty())
        result += " " + restText;

    return result;
}

static QString numberToTurkishWords(qlonglong value)
{
    if (value == 0)
        return "Sıfır";

    QStringList parts;

    const qlonglong billions = value / 1000000000;
    value %= 1000000000;

    const qlonglong millions = value / 1000000;
    value %= 1000000;

    const qlonglong thousands = value / 1000;
    value %= 1000;

    const qlonglong units = value;

    if (billions > 0) {
        parts << convertTens(static_cast<int>(billions)) + " Milyar";
    }

    if (millions > 0) {
        parts << convertTens(static_cast<int>(millions)) + " Milyon";
    }

    if (thousands > 0) {
        if (thousands == 1)
            parts << "Bin";
        else
            parts << convertTens(static_cast<int>(thousands)) + " Bin";
    }

    if (units > 0) {
        parts << convertTens(static_cast<int>(units));
    }

    return parts.join(" ").simplified();
}

static QString buildUsdAmountInWords(const QString &text)
{
    bool ok = false;
    const double value = QLocale::c().toDouble(text.trimmed(), &ok);
    if (!ok)
        return "";

    const qlonglong whole = static_cast<qlonglong>(value + 0.5);
    const QString words = numberToTurkishWords(whole);

    return "Yalnız " + words + " Amerika Birleşik Devletleri Doları.";
}

QuotePdfData QuotePdfDataBuilder::buildFromQuoteRecord(const QuoteRecord &quote)
{
    QuotePdfData data;

    data.customer.name = quote.customerName;
    data.customer.attention = quote.customerAttention;
    data.customer.address = quote.customerAddress;
    data.customer.tel = quote.customerTel;
    data.customer.email = quote.customerEmail;
    data.customer.taxOffice = quote.customerTaxOffice;

    {
        QuotePdfLineItem item;
        item.model = quote.machineModel;
        item.productName = quote.toolingName;
        item.quantity = "1";
        item.unitPrice = formatUsdForPdf(quote.machinePriceUsd);
        item.amount = formatUsdForPdf(quote.machinePriceUsd);
        data.mainItems.append(item);
    }

    const QStringList optionEntries = quote.optionsData.split(";", Qt::SkipEmptyParts);

    for (const QString &entry : optionEntries) {
        const QStringList parts = entry.split("|");
        if (parts.size() < 3)
            continue;

        const QString optionName = parts[1].trimmed();
        const QString optionPriceRaw = parts[2].trimmed();

        QuotePdfLineItem item;
        item.model = parts[0].trimmed();
        item.productName = parts[1].trimmed();
        item.quantity = "1";
        item.unitPrice = formatUsdForPdf(optionPriceRaw);
        item.amount = formatUsdForPdf(optionPriceRaw);

        data.mainItems.append(item);
    }

    {
        QuotePdfGroupedItem group;
        group.groupTitle = QString::fromUtf8("SARF MALZEMELERİ");
        group.rows.append({"", QString::fromUtf8("KORUYUCU LENS"), "10 Adet", QString::fromUtf8("DAHİL"), ""});
        group.rows.append({"", "NOZZLE", "20 Adet", QString::fromUtf8("DAHİL"), ""});
        group.rows.append({"", QString::fromUtf8("SERAMİK"), "2 Adet", QString::fromUtf8("DAHİL"), ""});
        data.groupedItems.append(group);
    }

    {
        QuotePdfGroupedItem group;
        group.groupTitle = QString::fromUtf8("EKSTRA");
        group.rows.append({"", QString::fromUtf8("REGÜLATÖR"), "1 Adet", QString::fromUtf8("DAHİL"), ""});
        group.rows.append({"", QString::fromUtf8("KOMPRESÖR"), "1 Adet", QString::fromUtf8("DAHİL"), ""});
        group.rows.append({"", QString::fromUtf8("ÇİN NAKLİYE"), "", QString::fromUtf8("DAHİL"), ""});
        data.groupedItems.append(group);
    }

    data.terms.grandTotal = formatUsdForPdf(quote.grandTotalUsd);
    data.terms.amountInWords = buildUsdAmountInWords(quote.grandTotalUsd);
    data.terms.otherTerms = quote.notes;

    return data;
}
