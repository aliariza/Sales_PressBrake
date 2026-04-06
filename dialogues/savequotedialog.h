#ifndef SAVEQUOTEDIALOG_H
#define SAVEQUOTEDIALOG_H

#include <QDialog>

class QLineEdit;
class QPlainTextEdit;

class SaveQuoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveQuoteDialog(QWidget *parent = nullptr);

    QString customerName() const;
    QString customerAttention() const;
    QString customerAddress() const;
    QString customerTel() const;
    QString customerEmail() const;
    QString customerTaxOffice() const;

private:
    void setupUi();

    QLineEdit *m_customerNameEdit;
    QLineEdit *m_customerAttentionEdit;
    QPlainTextEdit *m_customerAddressEdit;
    QLineEdit *m_customerTelEdit;
    QLineEdit *m_customerEmailEdit;
    QLineEdit *m_customerTaxOfficeEdit;
};

#endif // SAVEQUOTEDIALOG_H
