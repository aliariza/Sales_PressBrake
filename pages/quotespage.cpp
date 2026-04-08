#include "quotespage.h"
#include "stylehelper.h"
#include "quotepdflayout.h"
#include "quotepdfheader.h"
#include "quotepdfcustomer.h"
#include "quotepdfdata.h"
#include "quotepdfdatabuilder.h"
#include "quotepdfitems.h"
#include "toastmessage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QLocale>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPdfWriter>
#include <QPainter>
#include <QPageSize>
#include <QMarginsF>
#include <QPalette>
#include <QHeaderView>

static bool isDarkTheme(const QWidget *w)
{
    return w->palette().color(QPalette::Window).lightness() < 128;
}

static QString tableHeaderStyle(const QWidget *w)
{
    if (isDarkTheme(w)) {
        return
            "QHeaderView::section {"
            " padding: 6px 8px;"
            " border: none;"
            " border-bottom: 1px solid #5A5A5A;"
            " background-color: #2B2B2B;"
            " color: #F2F2F2;"
            " font-weight: 600;"
            " }";
    }

    return
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #CFCFCF;"
        " background-color: #EDEDED;"
        " color: #111111;"
        " font-weight: 600;"
        " }";
}

static QString formatQuoteNumber(const QString &text, bool useGrouping = false)
{
    bool ok = false;
    const double number = QLocale::c().toDouble(text.trimmed(), &ok);
    if (!ok)
        return text.trimmed();

    QString result = QLocale::c().toString(number, 'f', 6);

    while (result.contains('.') && result.endsWith('0'))
        result.chop(1);

    if (result.endsWith('.'))
        result.chop(1);

    if (useGrouping) {
        const int dotIndex = result.indexOf('.');
        QString integerPart = (dotIndex >= 0) ? result.left(dotIndex) : result;
        const QString fractionalPart = (dotIndex >= 0) ? result.mid(dotIndex) : QString();

        bool intOk = false;
        const qlonglong intValue = integerPart.toLongLong(&intOk);
        if (intOk) {
            QLocale enUS(QLocale::English, QLocale::UnitedStates);
            integerPart = enUS.toString(intValue);
            result = integerPart + fractionalPart;
        }
    }

    return result;
}

QuotesPage::QuotesPage(QWidget *parent)
    : QWidget(parent),
    m_table(nullptr),
    m_deleteButton(nullptr),
    m_exportPdfButton(nullptr),
    m_statusLabel(nullptr),
    m_quoteCodeValue(nullptr),
    m_materialValue(nullptr),
    m_thicknessValue(nullptr),
    m_bendLengthValue(nullptr),
    m_customerValue(nullptr),
    m_customerAttentionValue(nullptr),
    m_customerAddressValue(nullptr),
    m_customerTelValue(nullptr),
    m_customerEmailValue(nullptr),
    m_customerTaxOfficeValue(nullptr),
    m_machineValue(nullptr),
    m_toolingValue(nullptr),
    m_machinePriceValue(nullptr),
    m_optionsTotalValue(nullptr),
    m_grandTotalValue(nullptr),
    m_createdAtValue(nullptr),
    m_optionsDetailText(nullptr),
    m_notesDetailText(nullptr),
    m_toast(nullptr)
{
    QString errorMessage;
    if (!m_quoteRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    setupUi();
    refreshQuotesTable();
}

void QuotesPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(8);
    m_table->setHorizontalHeaderLabels(QStringList()
                                       << "No"
                                       << "Teklif Kodu"
                                       << "Malzeme"
                                       << "Kalınlık (mm)"
                                       << "Bükme Uzunluğu (mm)"
                                       << "Makine"
                                       << "Genel Toplam (USD)"
                                       << "Oluşturulma");

    auto *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::Stretch);
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    header->setDefaultAlignment(Qt::AlignCenter);

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);

    header->setStyleSheet(tableHeaderStyle(this));

    m_deleteButton = new QPushButton("SEÇİLİ TEKLİFİ SİL", this);
    m_deleteButton->setFixedHeight(36);
    StyleHelper::applySecondaryButtonStyle(m_deleteButton);

    m_exportPdfButton = new QPushButton("SEÇİLİ TEKLİFİ PDF OLARAK AKTAR", this);
    m_exportPdfButton->setFixedHeight(36);
    StyleHelper::applyPrimaryButtonStyle(m_exportPdfButton);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addStretch();
    buttonRow->addWidget(m_exportPdfButton);
    buttonRow->addWidget(m_deleteButton);

    auto *detailsGroup = new QGroupBox("Teklif Detayları", this);
    auto *detailsOuterLayout = new QVBoxLayout(detailsGroup);
    detailsOuterLayout->setContentsMargins(16, 20, 16, 16);
    detailsOuterLayout->setSpacing(12);

    auto *customerGroup = new QGroupBox("Müşteri Detayları", detailsGroup);
    auto *customerLayout = new QGridLayout(customerGroup);
    customerLayout->setContentsMargins(12, 16, 12, 12);
    customerLayout->setHorizontalSpacing(12);
    customerLayout->setVerticalSpacing(8);

    auto *quoteInfoGroup = new QGroupBox("Teklif / Makine Detayları", detailsGroup);
    auto *quoteInfoLayout = new QGridLayout(quoteInfoGroup);
    quoteInfoLayout->setContentsMargins(12, 16, 12, 12);
    quoteInfoLayout->setHorizontalSpacing(12);
    quoteInfoLayout->setVerticalSpacing(8);

    auto makeValueLabel = []() {
        auto *label = new QLabel("-");
        label->setWordWrap(true);
        return label;
    };

    m_quoteCodeValue = makeValueLabel();
    m_materialValue = makeValueLabel();
    m_thicknessValue = makeValueLabel();
    m_bendLengthValue = makeValueLabel();
    m_customerValue = makeValueLabel();
    m_customerAttentionValue = makeValueLabel();
    m_customerAddressValue = makeValueLabel();
    m_customerTelValue = makeValueLabel();
    m_customerEmailValue = makeValueLabel();
    m_customerTaxOfficeValue = makeValueLabel();
    m_machineValue = makeValueLabel();
    m_toolingValue = makeValueLabel();
    m_machinePriceValue = makeValueLabel();
    m_optionsTotalValue = makeValueLabel();
    m_grandTotalValue = makeValueLabel();
    m_createdAtValue = makeValueLabel();

    m_optionsDetailText = new QTextEdit(this);
    m_optionsDetailText->setReadOnly(true);
    m_optionsDetailText->setMinimumHeight(50);
    m_optionsDetailText->setMaximumHeight(55);

    m_notesDetailText = new QTextEdit(this);
    m_notesDetailText->setReadOnly(true);
    m_notesDetailText->setMinimumHeight(50);
    m_notesDetailText->setMaximumHeight(55);

    customerLayout->addWidget(new QLabel("Müşteri:", this), 0, 0);
    customerLayout->addWidget(m_customerValue, 0, 1);

    customerLayout->addWidget(new QLabel("Dikkatine:", this), 1, 0);
    customerLayout->addWidget(m_customerAttentionValue, 1, 1);

    customerLayout->addWidget(new QLabel("Adres:", this), 2, 0);
    customerLayout->addWidget(m_customerAddressValue, 2, 1);

    customerLayout->addWidget(new QLabel("Tel:", this), 3, 0);
    customerLayout->addWidget(m_customerTelValue, 3, 1);

    customerLayout->addWidget(new QLabel("Eposta:", this), 4, 0);
    customerLayout->addWidget(m_customerEmailValue, 4, 1);

    customerLayout->addWidget(new QLabel(QString::fromUtf8("Vergi Dairesi:")), 5, 0);
    customerLayout->addWidget(m_customerTaxOfficeValue, 5, 1);

    quoteInfoLayout->addWidget(new QLabel("Teklif Kodu:", this), 0, 0);
    quoteInfoLayout->addWidget(m_quoteCodeValue, 0, 1);
    quoteInfoLayout->addWidget(new QLabel("Oluşturulma:", this), 0, 2);
    quoteInfoLayout->addWidget(m_createdAtValue, 0, 3);

    quoteInfoLayout->addWidget(new QLabel("Malzeme:", this), 1, 0);
    quoteInfoLayout->addWidget(m_materialValue, 1, 1);
    quoteInfoLayout->addWidget(new QLabel("Kalınlık (mm):", this), 1, 2);
    quoteInfoLayout->addWidget(m_thicknessValue, 1, 3);

    quoteInfoLayout->addWidget(new QLabel("Bükme Uzunluğu (mm):", this), 2, 0);
    quoteInfoLayout->addWidget(m_bendLengthValue, 2, 1);
    quoteInfoLayout->addWidget(new QLabel("Makine:", this), 2, 2);
    quoteInfoLayout->addWidget(m_machineValue, 2, 3);

    quoteInfoLayout->addWidget(new QLabel("Takım:", this), 3, 0);
    quoteInfoLayout->addWidget(m_toolingValue, 3, 1);

    quoteInfoLayout->addWidget(new QLabel("Makine Fiyatı:", this), 4, 0);
    quoteInfoLayout->addWidget(m_machinePriceValue, 4, 1);
    quoteInfoLayout->addWidget(new QLabel("Opsiyonlar Toplamı:", this), 4, 2);
    quoteInfoLayout->addWidget(m_optionsTotalValue, 4, 3);

    quoteInfoLayout->addWidget(new QLabel("Genel Toplam:", this), 5, 0);
    quoteInfoLayout->addWidget(m_grandTotalValue, 5, 1);

    quoteInfoLayout->addWidget(new QLabel("Seçili Opsiyonlar:", this), 6, 0);
    quoteInfoLayout->addWidget(m_optionsDetailText, 6, 1, 1, 3);

    quoteInfoLayout->addWidget(new QLabel("Notlar:", this), 7, 0);
    quoteInfoLayout->addWidget(m_notesDetailText, 7, 1, 1, 3);

    detailsOuterLayout->addWidget(customerGroup);
    detailsOuterLayout->addWidget(quoteInfoGroup);

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    layout->addWidget(m_table, 1);
    layout->addLayout(buttonRow);
    layout->addWidget(detailsGroup);
    layout->addWidget(m_statusLabel);

    connect(m_deleteButton, &QPushButton::clicked, this, &QuotesPage::deleteSelectedQuote);
    connect(m_exportPdfButton, &QPushButton::clicked, this, &QuotesPage::exportSelectedQuoteToPdf);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &QuotesPage::showSelectedQuoteDetails);
    m_toast = new ToastMessage(this);

}

QString QuotesPage::formatOptionsForDisplay(const QString &optionsData) const
{
    const QString trimmed = optionsData.trimmed();
    if (trimmed.isEmpty())
        return "Opsiyon seçili değil.";

    const QStringList entries = trimmed.split(";", Qt::SkipEmptyParts);
    QStringList lines;

    for (const QString &entry : entries) {
        const QStringList parts = entry.split("|");
        if (parts.size() >= 3) {
            lines << parts[0].trimmed() + " - " + parts[1].trimmed() + " - " +
                         formatQuoteNumber(parts[2].trimmed(), true) + " USD";
        } else {
            lines << entry.trimmed();
        }
    }

    return lines.join("\n");
}

void QuotesPage::refreshQuotesTable()
{
    QString errorMessage;
    m_quotesCache = m_quoteRepository.getAllQuotes(errorMessage);

    m_table->setRowCount(0);

    if (!errorMessage.isEmpty()) {
        m_toast->showMessage("Hata: " + errorMessage,
                            "#A33A3A");
        return;
    }

    for (int i = 0; i < m_quotesCache.size(); ++i) {
        const QuoteRecord &q = m_quotesCache[i];
        m_table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, col, item);
        };

        set(0, q.no, true);
        set(1, q.quoteCode, true);
        set(2, q.material);
        set(3, formatQuoteNumber(q.thicknessMm), true);
        set(4, formatQuoteNumber(q.bendLengthMm, true), true);
        set(5, q.machineModel);
        set(6, formatQuoteNumber(q.grandTotalUsd, true), true);
        set(7, q.createdAt, true);
    }

    if (!m_quotesCache.isEmpty()) {
        m_table->selectRow(0);
        showSelectedQuoteDetails();
    } else {
        m_quoteCodeValue->setText("-");
        m_materialValue->setText("-");
        m_thicknessValue->setText("-");
        m_bendLengthValue->setText("-");
        m_customerValue->setText("-");
        m_customerAttentionValue->setText("-");
        m_customerAddressValue->setText("-");
        m_customerTelValue->setText("-");
        m_customerEmailValue->setText("-");
        m_customerTaxOfficeValue->setText("-");
        m_machineValue->setText("-");
        m_toolingValue->setText("-");
        m_machinePriceValue->setText("-");
        m_optionsTotalValue->setText("-");
        m_grandTotalValue->setText("-");
        m_createdAtValue->setText("-");
        m_optionsDetailText->setPlainText("Seçili opsiyon yok.");
        m_notesDetailText->setPlainText("-");
    }
}

void QuotesPage::showSelectedQuoteDetails()
{
    const int row = m_table->currentRow();
    if (row < 0 || row >= m_quotesCache.size())
        return;

    const QuoteRecord &q = m_quotesCache[row];

    m_quoteCodeValue->setText(q.quoteCode);
    m_materialValue->setText(q.material);
    m_thicknessValue->setText(formatQuoteNumber(q.thicknessMm));
    m_bendLengthValue->setText(formatQuoteNumber(q.bendLengthMm, true));
    m_customerValue->setText(q.customerName.isEmpty() ? "-" : q.customerName);
    m_customerAttentionValue->setText(q.customerAttention.isEmpty() ? "-" : q.customerAttention);
    m_customerAddressValue->setText(q.customerAddress.isEmpty() ? "-" : q.customerAddress);
    m_customerTelValue->setText(q.customerTel.isEmpty() ? "-" : q.customerTel);
    m_customerEmailValue->setText(q.customerEmail.isEmpty() ? "-" : q.customerEmail);
    m_customerTaxOfficeValue->setText(q.customerTaxOffice.isEmpty() ? "-" : q.customerTaxOffice);
    m_machineValue->setText(q.machineModel);
    m_toolingValue->setText(q.toolingName.isEmpty() ? "-" : q.toolingName);
    m_machinePriceValue->setText(formatQuoteNumber(q.machinePriceUsd, true) + " USD");
    m_optionsTotalValue->setText(formatQuoteNumber(q.optionsTotalUsd, true) + " USD");
    m_grandTotalValue->setText(formatQuoteNumber(q.grandTotalUsd, true) + " USD");
    m_createdAtValue->setText(q.createdAt);
    m_optionsDetailText->setPlainText(formatOptionsForDisplay(q.optionsData));
    m_notesDetailText->setPlainText(q.notes.isEmpty() ? "-" : q.notes);
}

bool QuotesPage::exportQuoteToPdf(const QuoteRecord &quote,
                                  const QString &filePath,
                                  QString &errorMessage) const
{
    errorMessage.clear();

    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    writer.setResolution(300);

    QPainter painter(&writer);
    if (!painter.isActive()) {
        errorMessage = "PDF yazıcısı başlatılamadı.";
        return false;
    }

    painter.setPen(Qt::black);

    const bool showGuides = false;
    PdfQuoteLayout layout(writer);

    QuotePdfHeader::drawApprovedHeader(painter, layout, quote, showGuides);

    const QuotePdfData pdfData = QuotePdfDataBuilder::buildFromQuoteRecord(quote);
    QuotePdfCustomer::drawCustomerOnly(painter, layout, pdfData, showGuides);

    const int itemsBottomY =
        QuotePdfItems::drawItemsTable(painter, layout, pdfData, showGuides);

    const int termsY = itemsBottomY + 30;
    QuotePdfItems::drawOtherTermsBlock(painter, layout, pdfData, termsY, showGuides);

    painter.end();
    return true;
}

void QuotesPage::exportSelectedQuoteToPdf()
{
    const int row = m_table->currentRow();
    if (row < 0 || row >= m_quotesCache.size()) {
        m_toast->showMessage("Hata: Lütfen önce teklif seçin.",
                            "#A33A3A");
        return;
    }

    const QuoteRecord &quote = m_quotesCache[row];

    const QString defaultDir =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    const QString defaultFileName = defaultDir + "/" + quote.quoteCode + ".pdf";

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "Teklifi PDF Olarak Aktar",
        defaultFileName,
        "PDF Dosyaları (*.pdf)");

    if (filePath.trimmed().isEmpty())
        return;

    QString errorMessage;
    if (!exportQuoteToPdf(quote, filePath, errorMessage)) {
        m_toast->showMessage("Hata: " + errorMessage,
                            "#A33A3A");
        return;
    }

    m_toast->showMessage("Teklif başarıyla PDF olarak kaydedildi.",
                        "#1E7A4D");
}

void QuotesPage::deleteSelectedQuote()
{
    const int row = m_table->currentRow();
    if (row < 0) {
        m_toast->showMessage("Hata: Lütfen önce teklif seçin.",
                            "#A33A3A");
        return;
    }

    const QString no = m_table->item(row, 0) ? m_table->item(row, 0)->text().trimmed() : QString();
    const QString quoteCode = m_table->item(row, 1) ? m_table->item(row, 1)->text().trimmed() : QString();

    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Teklifi Sil",
                              "'" + quoteCode + "' kodlu teklifi silmek istiyor musunuz?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_quoteRepository.deleteQuote(no, errorMessage)) {
        m_toast->showMessage("Hata: " + errorMessage,
                            "#A33A3A");
        return;
    }

    refreshQuotesTable();
    m_toast->showMessage("Teklif başarıyla silindi.",
                        "#1E7A4D");
}

