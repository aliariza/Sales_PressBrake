#include "recommendationpage.h"
#include "stylehelper.h"
#include "savequotedialog.h"
#include "toastmessage.h"
#include "recommendationformatutils.h"
#include "recommendationtablehelper.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QDoubleValidator>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

RecommendationPage::RecommendationPage(QWidget *parent)
    : QWidget(parent),
    m_materialComboBox(nullptr),
    m_thicknessEdit(nullptr),
    m_bendLengthEdit(nullptr),
    m_notesEdit(nullptr),
    m_recommendButton(nullptr),
    m_saveQuoteButton(nullptr),
    m_resultsTable(nullptr),
    m_toolingResultsTable(nullptr),
    m_optionsTable(nullptr),
    m_selectedMachineValueLabel(nullptr),
    m_selectedToolingValueLabel(nullptr),
    m_machinePriceLabel(nullptr),
    m_toolingInfoLabel(nullptr),
    m_optionsTotalLabel(nullptr),
    m_grandTotalLabel(nullptr),
    m_toast(nullptr),
    m_recommendationService(&m_machineRepository, &m_toolingRepository)
{
    QString errorMessage;

    if (!m_materialRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    if (!m_machineRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    if (!m_toolingRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    if (!m_optionRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    if (!m_quoteRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Başlatma Hatası", errorMessage);
    }

    setupUi();
    refreshData();
}

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

void RecommendationPage::setupUi()
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *contentWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(20, 12, 20, 20);
    mainLayout->setSpacing(16);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *inputGroup = createInputGroup(sectionFont);
    auto *machinesGroup = createMachinesGroup(sectionFont);
    auto *toolingGroup = createToolingGroup(sectionFont);
    auto *optionsGroup = createOptionsGroup(sectionFont);
    auto *quoteGroup = createQuoteGroup(sectionFont);

    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(machinesGroup);
    mainLayout->addWidget(toolingGroup);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(quoteGroup);
    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);

    connectSignals();

    m_toast = new ToastMessage(this);
    m_toast->raise();
}

QGroupBox *RecommendationPage::createInputGroup(const QFont &sectionFont)
{
    auto *inputGroup = new QGroupBox("Öneri Girdileri", this);
    inputGroup->setFont(sectionFont);

    auto *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setContentsMargins(16, 20, 16, 16);
    inputLayout->setSpacing(12);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    m_materialComboBox = new QComboBox(inputGroup);
    m_materialComboBox->setMinimumWidth(120);
    m_materialComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_thicknessEdit = new QLineEdit(inputGroup);
    m_thicknessEdit->setMinimumWidth(80);
    m_thicknessEdit->setPlaceholderText("e.g. 2.0");
    m_thicknessEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_bendLengthEdit = new QLineEdit(inputGroup);
    m_bendLengthEdit->setMinimumWidth(80);
    m_bendLengthEdit->setPlaceholderText("e.g. 2500");
    m_bendLengthEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_notesEdit = new QTextEdit(inputGroup);
    m_notesEdit->setMinimumHeight(60);
    m_notesEdit->setMaximumHeight(80);
    m_notesEdit->setPlaceholderText("İsteğe bağlı teklif notları...");

    auto *positiveValidator = new QDoubleValidator(0.0, 1e12, 6, inputGroup);
    positiveValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveValidator->setLocale(QLocale::c());

    m_thicknessEdit->setValidator(positiveValidator);
    m_bendLengthEdit->setValidator(positiveValidator);

    grid->addWidget(new QLabel("Malzeme:", inputGroup), 0, 0);
    grid->addWidget(m_materialComboBox, 0, 1);
    grid->addWidget(new QLabel("Kalınlık (mm):", inputGroup), 0, 2);
    grid->addWidget(m_thicknessEdit, 0, 3);
    grid->addWidget(new QLabel("Bükme Uzunluğu (mm):", inputGroup), 0, 4);
    grid->addWidget(m_bendLengthEdit, 0, 5);

    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);
    grid->setColumnStretch(5, 1);

    m_recommendButton = new QPushButton("ÖNER", inputGroup);
    m_recommendButton->setFixedHeight(36);
    m_recommendButton->setMinimumWidth(160);
    StyleHelper::applyPrimaryButtonStyle(m_recommendButton);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addStretch();
    buttonRow->addWidget(m_recommendButton);

    inputLayout->addLayout(grid);
    inputLayout->addWidget(new QLabel("Notlar:", inputGroup));
    inputLayout->addWidget(m_notesEdit);
    inputLayout->addLayout(buttonRow);

    return inputGroup;
}

QGroupBox *RecommendationPage::createMachinesGroup(const QFont &sectionFont)
{
    auto *machinesGroup = new QGroupBox("Önerilen Makineler", this);
    machinesGroup->setFont(sectionFont);

    auto *machinesLayout = new QVBoxLayout(machinesGroup);
    machinesLayout->setContentsMargins(16, 20, 16, 16);

    m_resultsTable = new QTableWidget(machinesGroup);
    m_resultsTable->setColumnCount(8);
    m_resultsTable->setHorizontalHeaderLabels(QStringList()
                                              << "Seç"
                                              << "Makine"
                                              << "Model"
                                              << "Ton (tonf)"
                                              << "Uzunluk (mm)"
                                              << "Maks t (mm)"
                                              << "Başlangıç Fiyatı (USD)"
                                              << "Gerekçe");
    m_resultsTable->setMinimumHeight(150);
    m_resultsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *machineHeader = m_resultsTable->horizontalHeader();
    machineHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(7, QHeaderView::Stretch);
    machineHeader->setDefaultAlignment(Qt::AlignCenter);

    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setShowGrid(false);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->verticalHeader()->setDefaultSectionSize(28);

    machineHeader->setStyleSheet(tableHeaderStyle(this));

    machinesLayout->addWidget(m_resultsTable);
    return machinesGroup;
}

QGroupBox *RecommendationPage::createToolingGroup(const QFont &sectionFont)
{
    auto *toolingGroup = new QGroupBox("Önerilen Takımlar", this);
    toolingGroup->setFont(sectionFont);

    auto *toolingLayout = new QVBoxLayout(toolingGroup);
    toolingLayout->setContentsMargins(16, 20, 16, 16);

    m_toolingResultsTable = new QTableWidget(toolingGroup);
    m_toolingResultsTable->setColumnCount(6);
    m_toolingResultsTable->setHorizontalHeaderLabels(QStringList()
                                                     << "Seç"
                                                     << "Ad"
                                                     << "V-kanal (mm)"
                                                     << "Zımba Yarıçapı (mm)"
                                                     << "Kalıp Yarıçapı (mm)"
                                                     << "Gerekçe");
    m_toolingResultsTable->setMinimumHeight(150);
    m_toolingResultsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *toolingHeader = m_toolingResultsTable->horizontalHeader();
    toolingHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(5, QHeaderView::Stretch);
    toolingHeader->setDefaultAlignment(Qt::AlignCenter);

    m_toolingResultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_toolingResultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_toolingResultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_toolingResultsTable->setAlternatingRowColors(true);
    m_toolingResultsTable->setShowGrid(false);
    m_toolingResultsTable->verticalHeader()->setVisible(false);
    m_toolingResultsTable->verticalHeader()->setDefaultSectionSize(28);

    toolingHeader->setStyleSheet(tableHeaderStyle(this));

    toolingLayout->addWidget(m_toolingResultsTable);
    return toolingGroup;
}

QGroupBox *RecommendationPage::createOptionsGroup(const QFont &sectionFont)
{
    auto *optionsGroup = new QGroupBox("Kullanılabilir Opsiyonlar", this);
    optionsGroup->setFont(sectionFont);

    auto *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->setContentsMargins(16, 20, 16, 16);

    m_optionsTable = new QTableWidget(optionsGroup);
    m_optionsTable->setColumnCount(4);
    m_optionsTable->setHorizontalHeaderLabels(QStringList()
                                              << "Seç"
                                              << "Kod"
                                              << "Ad"
                                              << "Fiyat (USD)");
    m_optionsTable->setMinimumHeight(140);
    m_optionsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *optionsHeader = m_optionsTable->horizontalHeader();
    optionsHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    optionsHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    optionsHeader->setSectionResizeMode(2, QHeaderView::Stretch);
    optionsHeader->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    optionsHeader->setDefaultAlignment(Qt::AlignCenter);

    m_optionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_optionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_optionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_optionsTable->setAlternatingRowColors(true);
    m_optionsTable->setShowGrid(false);
    m_optionsTable->verticalHeader()->setVisible(false);
    m_optionsTable->verticalHeader()->setDefaultSectionSize(28);

    optionsHeader->setStyleSheet(tableHeaderStyle(this));

    optionsLayout->addWidget(m_optionsTable);
    return optionsGroup;
}

QGroupBox *RecommendationPage::createQuoteGroup(const QFont &sectionFont)
{
    auto *quoteGroup = new QGroupBox("Teklif Özeti", this);
    quoteGroup->setFont(sectionFont);

    auto *quoteLayout = new QGridLayout(quoteGroup);
    quoteLayout->setContentsMargins(16, 20, 16, 16);
    quoteLayout->setHorizontalSpacing(10);
    quoteLayout->setVerticalSpacing(10);

    m_selectedMachineValueLabel = new QLabel("-", quoteGroup);
    m_selectedToolingValueLabel = new QLabel("-", quoteGroup);
    m_machinePriceLabel = new QLabel("-", quoteGroup);
    m_toolingInfoLabel = new QLabel("-", quoteGroup);
    m_optionsTotalLabel = new QLabel("0 USD", quoteGroup);
    m_grandTotalLabel = new QLabel("0 USD", quoteGroup);

    QFont totalFont;
    totalFont.setBold(true);
    m_grandTotalLabel->setFont(totalFont);

    m_saveQuoteButton = new QPushButton("TEKLİFİ KAYDET", quoteGroup);
    m_saveQuoteButton->setFixedHeight(36);
    m_saveQuoteButton->setMinimumWidth(150);
    StyleHelper::applyPrimaryButtonStyle(m_saveQuoteButton);

    quoteLayout->addWidget(new QLabel("Seçilen Makine:", quoteGroup), 0, 0);
    quoteLayout->addWidget(m_selectedMachineValueLabel, 0, 1);
    quoteLayout->addWidget(new QLabel("Makine Fiyatı:", quoteGroup), 0, 2);
    quoteLayout->addWidget(m_machinePriceLabel, 0, 3);

    quoteLayout->addWidget(new QLabel("Seçilen Takım:", quoteGroup), 1, 0);
    quoteLayout->addWidget(m_selectedToolingValueLabel, 1, 1);
    quoteLayout->addWidget(new QLabel("Takım Bilgisi:", quoteGroup), 1, 2);
    quoteLayout->addWidget(m_toolingInfoLabel, 1, 3);

    quoteLayout->addWidget(new QLabel("Opsiyonlar Toplamı:", quoteGroup), 2, 2);
    quoteLayout->addWidget(m_optionsTotalLabel, 2, 3);

    quoteLayout->addWidget(new QLabel("Genel Toplam:", quoteGroup), 3, 2);
    quoteLayout->addWidget(m_grandTotalLabel, 3, 3);

    quoteLayout->addWidget(m_saveQuoteButton, 4, 0, 1, 4, Qt::AlignRight);

    return quoteGroup;
}

void RecommendationPage::connectSignals()
{
    connect(m_recommendButton, &QPushButton::clicked,
            this, &RecommendationPage::runRecommendation);

    connect(m_saveQuoteButton, &QPushButton::clicked,
            this, &RecommendationPage::saveQuote);

    connect(m_thicknessEdit, &QLineEdit::returnPressed,
            this, &RecommendationPage::runRecommendation);

    connect(m_bendLengthEdit, &QLineEdit::returnPressed,
            this, &RecommendationPage::runRecommendation);

    connect(m_resultsTable, &QTableWidget::itemChanged,
            this, [this](QTableWidgetItem *item) {
                if (!item || item->column() != 0)
                    return;

                if (item->checkState() != Qt::Checked) {
                    updateQuoteSummary();
                    return;
                }

                m_resultsTable->blockSignals(true);
                for (int row = 0; row < m_resultsTable->rowCount(); ++row) {
                    if (row == item->row())
                        continue;

                    QTableWidgetItem *other = m_resultsTable->item(row, 0);
                    if (other)
                        other->setCheckState(Qt::Unchecked);
                }
                m_resultsTable->blockSignals(false);

                updateQuoteSummary();
            });

    connect(m_toolingResultsTable, &QTableWidget::itemChanged,
            this, [this](QTableWidgetItem *item) {
                if (!item || item->column() != 0)
                    return;

                if (item->checkState() != Qt::Checked) {
                    updateQuoteSummary();
                    return;
                }

                m_toolingResultsTable->blockSignals(true);
                for (int row = 0; row < m_toolingResultsTable->rowCount(); ++row) {
                    if (row == item->row())
                        continue;

                    QTableWidgetItem *other = m_toolingResultsTable->item(row, 0);
                    if (other)
                        other->setCheckState(Qt::Unchecked);
                }
                m_toolingResultsTable->blockSignals(false);

                updateQuoteSummary();
            });

    connect(m_optionsTable, &QTableWidget::itemChanged,
            this, [this](QTableWidgetItem *item) {
                if (item && item->column() == 0)
                    updateQuoteSummary();
            });
}

void RecommendationPage::refreshData()
{
    loadMaterials();

    m_resultsTable->setRowCount(0);
    m_toolingResultsTable->setRowCount(0);
    m_optionsTable->setRowCount(0);

    m_currentMachineResults.clear();
    m_currentToolingResults.clear();
    m_allOptions.clear();

    m_selectedMachineValueLabel->setText("-");
    m_selectedToolingValueLabel->setText("-");
    m_machinePriceLabel->setText("-");
    m_toolingInfoLabel->setText("-");
    m_optionsTotalLabel->setText("0 USD");
    m_grandTotalLabel->setText("0 USD");

    m_notesEdit->clear();
}

void RecommendationPage::loadMaterials()
{
    QString errorMessage;
    const QList<MaterialRecord> materials = m_materialRepository.getAllMaterials(errorMessage);

    m_materialComboBox->clear();

    if (!errorMessage.isEmpty()) {
        showErrorToast("Hata: " + errorMessage);
        return;
    }

    for (const MaterialRecord &material : materials) {
        m_materialComboBox->addItem(material.name);
    }
}

void RecommendationPage::loadOptions()
{
    QString errorMessage;
    m_allOptions = m_optionRepository.getAllOptions(errorMessage);

    m_optionsTable->setRowCount(0);

    if (!errorMessage.isEmpty()) {
        showErrorToast("Hata: " + errorMessage);
        return;
    }

    m_optionsTable->blockSignals(true);
    RecommendationTableHelper::fillOptionsTable(m_optionsTable, m_allOptions);
    m_optionsTable->blockSignals(false);
}

int RecommendationPage::selectedMachineRow() const
{
    if (!m_resultsTable)
        return -1;

    for (int row = 0; row < m_resultsTable->rowCount(); ++row) {
        QTableWidgetItem *item = m_resultsTable->item(row, 0);
        if (item && item->checkState() == Qt::Checked)
            return row;
    }

    return -1;
}

int RecommendationPage::selectedToolingRow() const
{
    if (!m_toolingResultsTable)
        return -1;

    for (int row = 0; row < m_toolingResultsTable->rowCount(); ++row) {
        QTableWidgetItem *item = m_toolingResultsTable->item(row, 0);
        if (item && item->checkState() == Qt::Checked)
            return row;
    }

    return -1;
}

void RecommendationPage::selectDefaultRows()
{
    if (!m_currentMachineResults.isEmpty()) {
        m_resultsTable->blockSignals(true);
        if (QTableWidgetItem *item = m_resultsTable->item(0, 0))
            item->setCheckState(Qt::Checked);
        m_resultsTable->blockSignals(false);
    }

    if (!m_currentToolingResults.isEmpty()) {
        m_toolingResultsTable->blockSignals(true);
        if (QTableWidgetItem *item = m_toolingResultsTable->item(0, 0))
            item->setCheckState(Qt::Checked);
        m_toolingResultsTable->blockSignals(false);
    }

    updateQuoteSummary();
}

double RecommendationPage::selectedMachinePrice() const
{
    const int index = selectedMachineRow();
    if (index < 0 || index >= m_currentMachineResults.size())
        return 0.0;

    bool ok = false;
    const double price = QLocale::c().toDouble(m_currentMachineResults[index].machine.basePriceUSD, &ok);
    return ok ? price : 0.0;
}

double RecommendationPage::selectedOptionsTotal() const
{
    double total = 0.0;

    for (int row = 0; row < m_optionsTable->rowCount() && row < m_allOptions.size(); ++row) {
        QTableWidgetItem *checkItem = m_optionsTable->item(row, 0);
        if (!checkItem || checkItem->checkState() != Qt::Checked)
            continue;

        bool ok = false;
        const double price = QLocale::c().toDouble(m_allOptions[row].priceUsd, &ok);
        if (ok)
            total += price;
    }

    return total;
}

QString RecommendationPage::buildSelectedOptionsData() const
{
    QStringList parts;

    for (int row = 0; row < m_optionsTable->rowCount() && row < m_allOptions.size(); ++row) {
        QTableWidgetItem *checkItem = m_optionsTable->item(row, 0);
        if (!checkItem || checkItem->checkState() != Qt::Checked)
            continue;

        const OptionRecord &option = m_allOptions[row];
        parts << option.code + "|" + option.name + "|" + option.priceUsd;
    }

    return parts.join(";");
}

void RecommendationPage::updateQuoteSummary()
{
    const int machineIndex = selectedMachineRow();
    const int toolingIndex = selectedToolingRow();

    if (machineIndex >= 0 && machineIndex < m_currentMachineResults.size()) {
        const MachineRecord &machine = m_currentMachineResults[machineIndex].machine;
        bool priceOk = false;
        const double machinePrice = QLocale::c().toDouble(machine.basePriceUSD, &priceOk);

        m_selectedMachineValueLabel->setText(machine.model);
        m_machinePriceLabel->setText(
            priceOk ? (formatRecommendationNumber(QString::number(machinePrice), true) + " USD") : "-"
            );
    } else {
        m_selectedMachineValueLabel->setText("-");
        m_machinePriceLabel->setText("-");
    }

    if (toolingIndex >= 0 && toolingIndex < m_currentToolingResults.size()) {
        const ToolingRecord &tooling = m_currentToolingResults[toolingIndex].tooling;

        m_selectedToolingValueLabel->setText(tooling.name);
        m_toolingInfoLabel->setText(
            "V=" + formatRecommendationNumber(tooling.vDieMm) +
            " / Rp=" + formatRecommendationNumber(tooling.punchRadiusMm) +
            " / Rd=" + formatRecommendationNumber(tooling.dieRadiusMm)
            );
    } else {
        m_selectedToolingValueLabel->setText("-");
        m_toolingInfoLabel->setText("-");
    }

    const double optionsTotal = selectedOptionsTotal();
    const double grandTotal = selectedMachinePrice() + optionsTotal;

    m_optionsTotalLabel->setText(formatRecommendationNumber(QString::number(optionsTotal), true) + " USD");
    m_grandTotalLabel->setText(formatRecommendationNumber(QString::number(grandTotal), true) + " USD");
}

void RecommendationPage::runRecommendation()
{
    if (m_materialComboBox->currentText().trimmed().isEmpty()) {
        showErrorToast("Hata: Lütfen malzeme seçin.");
        return;
    }

    if (!m_thicknessEdit->hasAcceptableInput() || !m_bendLengthEdit->hasAcceptableInput()) {
        showErrorToast("Hata: Kalınlık ve Bükme Uzunluğu geçerli sayılar olmalıdır.");
        return;
    }

    bool thicknessOk = false;
    bool lengthOk = false;

    const double thickness =
        QLocale::c().toDouble(m_thicknessEdit->text().trimmed().replace(',', '.'), &thicknessOk);
    const double bendLength =
        QLocale::c().toDouble(m_bendLengthEdit->text().trimmed().replace(',', '.'), &lengthOk);

    if (!thicknessOk || thickness <= 0.0) {
        showErrorToast("Hata: Kalınlık 0'dan büyük olmalıdır.");
        return;
    }

    if (!lengthOk || bendLength <= 0.0) {
        showErrorToast("Hata: Bükme Uzunluğu 0'dan büyük olmalıdır.");
        return;
    }

    QString machineError;
    const QList<RecommendationResult> machineResults =
        m_recommendationService.buildMachineRecommendations(
            m_materialComboBox->currentText(),
            thickness,
            bendLength,
            machineError);

    if (!machineError.isEmpty()) {
        showErrorToast("Hata: " + machineError);
        return;
    }

    QString toolingError;
    const QList<ToolingRecommendationResult> toolingResults =
        m_recommendationService.buildToolingRecommendations(thickness, toolingError);

    if (!toolingError.isEmpty()) {
        showErrorToast("Hata: " + toolingError);
        return;
    }

    if (machineResults.isEmpty()) {
        m_resultsTable->blockSignals(true);
        m_resultsTable->setRowCount(0);
        m_resultsTable->blockSignals(false);

        m_toolingResultsTable->blockSignals(true);
        m_toolingResultsTable->setRowCount(0);
        m_toolingResultsTable->blockSignals(false);

        m_optionsTable->blockSignals(true);
        m_optionsTable->setRowCount(0);
        m_optionsTable->blockSignals(false);

        m_currentMachineResults.clear();
        m_currentToolingResults.clear();
        m_allOptions.clear();

        m_selectedMachineValueLabel->setText("-");
        m_selectedToolingValueLabel->setText("-");
        m_machinePriceLabel->setText("-");
        m_toolingInfoLabel->setText("-");
        m_optionsTotalLabel->setText("0 USD");
        m_grandTotalLabel->setText("0 USD");

        showInfoToast("Uygun makine bulunamadı.");
        return;
    }

    m_currentMachineResults = machineResults;
    m_currentToolingResults = toolingResults;

    m_resultsTable->blockSignals(true);
    RecommendationTableHelper::fillMachineResultsTable(m_resultsTable, machineResults);
    m_resultsTable->blockSignals(false);

    m_toolingResultsTable->blockSignals(true);
    RecommendationTableHelper::fillToolingResultsTable(m_toolingResultsTable, toolingResults);
    m_toolingResultsTable->blockSignals(false);

    loadOptions();
    selectDefaultRows();

    showSuccessToast(QString::number(machineResults.size()) + " makine bulundu.");
}

void RecommendationPage::saveQuote()
{
    if (m_materialComboBox->currentText().trimmed().isEmpty()) {
        showErrorToast("Hata: Malzeme seçimi yapılmamış.");
        return;
    }

    if (!m_thicknessEdit->hasAcceptableInput() || !m_bendLengthEdit->hasAcceptableInput()) {
        showErrorToast("Hata: Kayıt öncesi kalınlık ve bükme uzunluğu geçerli sayılar olmalıdır.");
        return;
    }

    const int machineIndex = selectedMachineRow();
    if (machineIndex < 0 || machineIndex >= m_currentMachineResults.size()) {
        showErrorToast("Hata: Lütfen makine tablosundan bir seçim yapın.");
        return;
    }

    bool thicknessOk = false;
    bool bendLengthOk = false;
    const QString thicknessText = m_thicknessEdit->text().trimmed().replace(',', '.');
    const QString bendLengthText = m_bendLengthEdit->text().trimmed().replace(',', '.');

    const double thickness = QLocale::c().toDouble(thicknessText, &thicknessOk);
    const double bendLength = QLocale::c().toDouble(bendLengthText, &bendLengthOk);

    if (!thicknessOk || thickness <= 0.0 || !bendLengthOk || bendLength <= 0.0) {
        showErrorToast("Hata: Kalınlık ve Bükme Uzunluğu 0'dan büyük olmalıdır.");
        return;
    }

    QString errorMessage;
    QuoteRecord quote;

    quote.no = m_quoteRepository.getNextQuoteNo(errorMessage);
    if (!errorMessage.isEmpty()) {
        showErrorToast("Hata: " + errorMessage);
        return;
    }

    quote.quoteCode = m_quoteRepository.generateQuoteCode(errorMessage);
    if (!errorMessage.isEmpty()) {
        showErrorToast("Hata: " + errorMessage);
        return;
    }

    SaveQuoteDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        showInfoToast("Teklif kaydetme işlemi iptal edildi.");
        return;
    }

    quote.customerName = dialog.customerName().replace("||", " ");
    quote.customerAttention = dialog.customerAttention().replace("||", " ");
    quote.customerAddress = dialog.customerAddress().replace("||", " ").replace('\n', ' ');
    quote.customerTel = dialog.customerTel().replace("||", " ");
    quote.customerEmail = dialog.customerEmail().replace("||", " ");
    quote.customerTaxOffice = dialog.customerTaxOffice().replace("||", " ");

    quote.material = m_materialComboBox->currentText().trimmed();
    quote.thicknessMm = thicknessText;
    quote.bendLengthMm = bendLengthText;
    quote.machineModel = m_currentMachineResults[machineIndex].machine.model;

    const int toolingIndex = selectedToolingRow();
    if (toolingIndex >= 0 && toolingIndex < m_currentToolingResults.size())
        quote.toolingName = m_currentToolingResults[toolingIndex].tooling.name;
    else
        quote.toolingName.clear();

    const double machinePrice = selectedMachinePrice();
    const double optionsTotal = selectedOptionsTotal();
    const double grandTotal = machinePrice + optionsTotal;

    quote.optionsData = buildSelectedOptionsData();
    quote.machinePriceUsd = QString::number(machinePrice, 'f', 6);
    quote.optionsTotalUsd = QString::number(optionsTotal, 'f', 6);
    quote.grandTotalUsd = QString::number(grandTotal, 'f', 6);
    quote.notes = m_notesEdit->toPlainText().trimmed().replace("||", " ").replace('\n', ' ');
    quote.createdAt = QDateTime::currentDateTime().toString(Qt::ISODate);

    if (!m_quoteRepository.addQuote(quote, errorMessage)) {
        showErrorToast("Hata: " + errorMessage);
        return;
    }

    showSuccessToast("Teklif başarıyla kaydedildi. Kod: " + quote.quoteCode);
}

void RecommendationPage::showSuccessToast(const QString &message)
{
    if (m_toast)
        m_toast->showMessage(message, "#1E7A4D", "#ffffff", 2000);
}

void RecommendationPage::showErrorToast(const QString &message)
{
    if (m_toast)
        m_toast->showMessage(message, "#A33A3A", "#ffffff", 3000);
}

void RecommendationPage::showInfoToast(const QString &message)
{
    if (m_toast)
        m_toast->showMessage(message, "#50555C", "#ffffff", 3000);
}