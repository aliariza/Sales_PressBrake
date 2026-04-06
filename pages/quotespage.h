#ifndef QUOTESPAGE_H
#define QUOTESPAGE_H

#include <QWidget>
#include "quoterepository.h"

class QTableWidget;
class QPushButton;
class QLabel;
class QTextEdit;
class ToastMessage;

class QuotesPage : public QWidget
{
    Q_OBJECT

public:
    explicit QuotesPage(QWidget *parent = nullptr);

    void refreshQuotesTable();

private slots:
    void deleteSelectedQuote();
    void showSelectedQuoteDetails();
    void exportSelectedQuoteToPdf();

private:
    void setupUi();
    QString formatOptionsForDisplay(const QString &optionsData) const;
    bool exportQuoteToPdf(const QuoteRecord &quote, const QString &filePath, QString &errorMessage) const;

    QTableWidget *m_table;
    QPushButton *m_deleteButton;
    QPushButton *m_exportPdfButton;
    QLabel *m_statusLabel;

    QLabel *m_quoteCodeValue;
    QLabel *m_materialValue;
    QLabel *m_thicknessValue;
    QLabel *m_bendLengthValue;

    QLabel *m_customerValue;
    QLabel *m_customerAttentionValue;
    QLabel *m_customerAddressValue;
    QLabel *m_customerTelValue;
    QLabel *m_customerEmailValue;
    QLabel *m_customerTaxOfficeValue;

    QLabel *m_machineValue;
    QLabel *m_toolingValue;
    QLabel *m_machinePriceValue;
    QLabel *m_optionsTotalValue;
    QLabel *m_grandTotalValue;
    QLabel *m_createdAtValue;
    QTextEdit *m_optionsDetailText;
    QTextEdit *m_notesDetailText;

    ToastMessage *m_toast;

    QList<QuoteRecord> m_quotesCache;
    QuoteRepository m_quoteRepository;

};

#endif // QUOTESPAGE_H
