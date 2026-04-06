#include "savequotedialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

SaveQuoteDialog::SaveQuoteDialog(QWidget *parent)
    : QDialog(parent),
    m_customerNameEdit(nullptr),
    m_customerAttentionEdit(nullptr),
    m_customerAddressEdit(nullptr),
    m_customerTelEdit(nullptr),
    m_customerEmailEdit(nullptr),
    m_customerTaxOfficeEdit(nullptr)
{
    setupUi();
}

void SaveQuoteDialog::setupUi()
{
    setWindowTitle("TEKLİFİ KAYDET");
    setModal(true);

    setMinimumSize(620, 360);
    resize(620, 360);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    auto *titleLabel = new QLabel("Lütfen müşteri detaylarını girin.", this);
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);

    auto makeLabel = [this](const QString &text) {
        auto *label = new QLabel(text, this);
        label->setMinimumWidth(110);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return label;
    };

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(10);

    m_customerNameEdit = new QLineEdit(this);
    m_customerAttentionEdit = new QLineEdit(this);

    m_customerAddressEdit = new QPlainTextEdit(this);
    m_customerAddressEdit->setMinimumHeight(90);
    m_customerAddressEdit->setMaximumHeight(90);

    m_customerTelEdit = new QLineEdit(this);
    m_customerTelEdit->setMinimumWidth(120);

    m_customerEmailEdit = new QLineEdit(this);
    m_customerEmailEdit->setMinimumWidth(180);

    m_customerTaxOfficeEdit = new QLineEdit(this);

    grid->addWidget(makeLabel("Firma:"), 0, 0);
    grid->addWidget(m_customerNameEdit, 0, 1, 1, 5);

    grid->addWidget(makeLabel("Dikkatine:"), 1, 0);
    grid->addWidget(m_customerAttentionEdit, 1, 1, 1, 5);

    grid->addWidget(makeLabel("Adres:"), 2, 0);
    grid->addWidget(m_customerAddressEdit, 2, 1, 1, 5);

    grid->addWidget(makeLabel("Tel:"), 3, 0);
    grid->addWidget(m_customerTelEdit, 3, 1, 1, 2);

    grid->addWidget(makeLabel("Eposta:"), 3, 3);
    grid->addWidget(m_customerEmailEdit, 3, 4, 1, 2);

    grid->addWidget(makeLabel(QString::fromUtf8("Vergi #:")), 4, 0);
    grid->addWidget(m_customerTaxOfficeEdit, 4, 1, 1, 5);

    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 1);
    grid->setColumnStretch(3, 1);
    grid->setColumnStretch(4, 1);
    grid->setColumnStretch(5, 1);

    mainLayout->addLayout(grid);

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_customerNameEdit->text().trimmed().isEmpty()) {
            m_customerNameEdit->setFocus();
            return;
        }
        if (m_customerAddressEdit->toPlainText().trimmed().isEmpty()) {
            m_customerAddressEdit->setFocus();
            return;
        }
        if (m_customerTelEdit->text().trimmed().isEmpty()) {
            m_customerTelEdit->setFocus();
            return;
        }
        if (m_customerTaxOfficeEdit->text().trimmed().isEmpty()) {
            m_customerTaxOfficeEdit->setFocus();
            return;
        }
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

QString SaveQuoteDialog::customerName() const
{
    return m_customerNameEdit->text().trimmed();
}

QString SaveQuoteDialog::customerAttention() const
{
    return m_customerAttentionEdit->text().trimmed();
}

QString SaveQuoteDialog::customerAddress() const
{
    return m_customerAddressEdit->toPlainText().trimmed();
}

QString SaveQuoteDialog::customerTel() const
{
    return m_customerTelEdit->text().trimmed();
}

QString SaveQuoteDialog::customerEmail() const
{
    return m_customerEmailEdit->text().trimmed();
}

QString SaveQuoteDialog::customerTaxOffice() const
{
    return m_customerTaxOfficeEdit->text().trimmed();
}
