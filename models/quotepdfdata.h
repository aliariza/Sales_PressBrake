#ifndef QUOTEPDFDATA_H
#define QUOTEPDFDATA_H

#include <QString>
#include <QVector>

struct QuotePdfLineItem
{
    QString model;
    QString productName;
    QString quantity;
    QString unitPrice;
    QString amount;
};

struct QuotePdfGroupedItem
{
    QString groupTitle;
    QVector<QuotePdfLineItem> rows;
};

struct QuotePdfCustomerInfo
{
    QString name;
    QString attention;
    QString address;
    QString tel;
    QString email;
    QString taxOffice;
};

struct QuotePdfTerms
{
    QString grandTotal;
    QString amountInWords;
    QString otherTerms;
};

struct QuotePdfData
{
    QuotePdfCustomerInfo customer;
    QVector<QuotePdfLineItem> mainItems;
    QVector<QuotePdfGroupedItem> groupedItems;
    QuotePdfTerms terms;
};

#endif // QUOTEPDFDATA_H
