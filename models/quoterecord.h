#ifndef QUOTERECORD_H
#define QUOTERECORD_H

#include <QString>

struct QuoteRecord
{
    QString no;
    QString quoteCode;

    QString customerName;
    QString customerAttention;
    QString customerAddress;
    QString customerTel;
    QString customerEmail;
    QString customerTaxOffice;

    QString material;
    QString thicknessMm;
    QString bendLengthMm;
    QString machineModel;
    QString toolingName;
    QString optionsData;
    QString machinePriceUsd;
    QString optionsTotalUsd;
    QString grandTotalUsd;
    QString notes;
    QString createdAt;
};

#endif // QUOTERECORD_H
