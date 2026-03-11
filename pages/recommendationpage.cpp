#include "recommendationpage.h"
#include "stylehelper.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QLocale>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <algorithm>
#include <cmath>

static QString formatRecommendationNumber(const QString &text, bool useGrouping = false)
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

RecommendationPage::RecommendationPage(QWidget *parent)
    : QWidget(parent),
    m_logoutButton(nullptr),
    m_materialComboBox(nullptr),
    m_thicknessEdit(nullptr),
    m_bendLengthEdit(nullptr),
    m_recommendButton(nullptr),
    m_statusLabel(nullptr),
    m_resultsTable(nullptr),
    m_toolingResultsTable(nullptr),
    m_selectedMachineComboBox(nullptr),
    m_selectedToolingComboBox(nullptr),
    m_machinePriceLabel(nullptr),
    m_toolingInfoLabel(nullptr),
    m_optionsTotalLabel(nullptr),
    m_grandTotalLabel(nullptr),
    m_optionsTable(nullptr)
{
    QString errorMessage;

    if (!m_materialRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    if (!m_machineRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    if (!m_toolingRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    if (!m_optionRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshData();
}

void RecommendationPage::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 12, 20, 20);
    mainLayout->setSpacing(16);

    auto *topBarLayout = new QHBoxLayout;
    topBarLayout->addStretch();

    m_logoutButton = new QPushButton("LOGOUT", this);
    m_logoutButton->setFixedHeight(34);
    m_logoutButton->setMinimumWidth(110);
    StyleHelper::applySecondaryButtonStyle(m_logoutButton);

    topBarLayout->addWidget(m_logoutButton);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *inputGroup = new QGroupBox("Recommendation Input", this);
    inputGroup->setFont(sectionFont);

    auto *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setContentsMargins(16, 20, 16, 16);
    inputLayout->setSpacing(12);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    m_materialComboBox = new QComboBox(this);
    m_materialComboBox->setMinimumWidth(220);

    m_thicknessEdit = new QLineEdit(this);
    m_thicknessEdit->setMinimumWidth(140);
    m_thicknessEdit->setPlaceholderText("e.g. 2.0");

    m_bendLengthEdit = new QLineEdit(this);
    m_bendLengthEdit->setMinimumWidth(140);
    m_bendLengthEdit->setPlaceholderText("e.g. 2500");

    auto *positiveValidator = new QDoubleValidator(0.0, 1e12, 6, this);
    positiveValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveValidator->setLocale(QLocale::c());

    m_thicknessEdit->setValidator(positiveValidator);
    m_bendLengthEdit->setValidator(positiveValidator);

    grid->addWidget(new QLabel("Material:", this), 0, 0);
    grid->addWidget(m_materialComboBox, 0, 1);
    grid->addWidget(new QLabel("Thickness (mm):", this), 0, 2);
    grid->addWidget(m_thicknessEdit, 0, 3);
    grid->addWidget(new QLabel("Bend Length (mm):", this), 0, 4);
    grid->addWidget(m_bendLengthEdit, 0, 5);

    m_recommendButton = new QPushButton("RECOMMEND", this);
    m_recommendButton->setFixedHeight(36);
    m_recommendButton->setMinimumWidth(160);
    StyleHelper::applyPrimaryButtonStyle(m_recommendButton);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addStretch();
    buttonRow->addWidget(m_recommendButton);

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    inputLayout->addLayout(grid);
    inputLayout->addLayout(buttonRow);
    inputLayout->addWidget(m_statusLabel);

    auto *machinesGroup = new QGroupBox("Recommended Machines", this);
    machinesGroup->setFont(sectionFont);

    auto *machinesLayout = new QVBoxLayout(machinesGroup);
    machinesLayout->setContentsMargins(16, 20, 16, 16);

    m_resultsTable = new QTableWidget(this);
    m_resultsTable->setColumnCount(6);
    m_resultsTable->setHorizontalHeaderLabels(QStringList()
                                              << "Model"
                                              << "Tonnage (tonf)"
                                              << "Length (mm)"
                                              << "Max t (mm)"
                                              << "Base Price (USD)"
                                              << "Reason");

    auto *machineHeader = m_resultsTable->horizontalHeader();
    machineHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    machineHeader->setSectionResizeMode(5, QHeaderView::Stretch);
    machineHeader->setDefaultAlignment(Qt::AlignCenter);

    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setShowGrid(false);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->verticalHeader()->setDefaultSectionSize(34);

    machineHeader->setStyleSheet(
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #cfcfcf;"
        " font-weight: 600;"
        " }"
        );

    machinesLayout->addWidget(m_resultsTable);

    auto *toolingGroup = new QGroupBox("Recommended Tooling", this);
    toolingGroup->setFont(sectionFont);

    auto *toolingLayout = new QVBoxLayout(toolingGroup);
    toolingLayout->setContentsMargins(16, 20, 16, 16);

    m_toolingResultsTable = new QTableWidget(this);
    m_toolingResultsTable->setColumnCount(5);
    m_toolingResultsTable->setHorizontalHeaderLabels(QStringList()
                                                     << "Name"
                                                     << "V-die (mm)"
                                                     << "Punch Radius (mm)"
                                                     << "Die Radius (mm)"
                                                     << "Reason");

    auto *toolingHeader = m_toolingResultsTable->horizontalHeader();
    toolingHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    toolingHeader->setSectionResizeMode(4, QHeaderView::Stretch);
    toolingHeader->setDefaultAlignment(Qt::AlignCenter);

    m_toolingResultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_toolingResultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_toolingResultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_toolingResultsTable->setAlternatingRowColors(true);
    m_toolingResultsTable->setShowGrid(false);
    m_toolingResultsTable->verticalHeader()->setVisible(false);
    m_toolingResultsTable->verticalHeader()->setDefaultSectionSize(34);

    toolingHeader->setStyleSheet(
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #cfcfcf;"
        " font-weight: 600;"
        " }"
        );

    toolingLayout->addWidget(m_toolingResultsTable);

    auto *quoteGroup = new QGroupBox("Quote Summary", this);
    quoteGroup->setFont(sectionFont);

    auto *quoteLayout = new QGridLayout(quoteGroup);
    quoteLayout->setContentsMargins(16, 20, 16, 16);
    quoteLayout->setHorizontalSpacing(10);
    quoteLayout->setVerticalSpacing(10);

    m_selectedMachineComboBox = new QComboBox(this);
    m_selectedMachineComboBox->setMinimumWidth(280);

    m_selectedToolingComboBox = new QComboBox(this);
    m_selectedToolingComboBox->setMinimumWidth(280);

    m_machinePriceLabel = new QLabel("-", this);
    m_toolingInfoLabel = new QLabel("-", this);
    m_optionsTotalLabel = new QLabel("0 USD", this);
    m_grandTotalLabel = new QLabel("0 USD", this);

    QFont totalFont;
    totalFont.setBold(true);
    m_grandTotalLabel->setFont(totalFont);

    quoteLayout->addWidget(new QLabel("Selected Machine:", this), 0, 0);
    quoteLayout->addWidget(m_selectedMachineComboBox, 0, 1);
    quoteLayout->addWidget(new QLabel("Machine Price:", this), 0, 2);
    quoteLayout->addWidget(m_machinePriceLabel, 0, 3);

    quoteLayout->addWidget(new QLabel("Selected Tooling:", this), 1, 0);
    quoteLayout->addWidget(m_selectedToolingComboBox, 1, 1);
    quoteLayout->addWidget(new QLabel("Tooling Info:", this), 1, 2);
    quoteLayout->addWidget(m_toolingInfoLabel, 1, 3);

    quoteLayout->addWidget(new QLabel("Options Total:", this), 2, 2);
    quoteLayout->addWidget(m_optionsTotalLabel, 2, 3);

    quoteLayout->addWidget(new QLabel("Grand Total:", this), 3, 2);
    quoteLayout->addWidget(m_grandTotalLabel, 3, 3);

    auto *optionsGroup = new QGroupBox("Available Options", this);
    optionsGroup->setFont(sectionFont);

    auto *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->setContentsMargins(16, 20, 16, 16);

    m_optionsTable = new QTableWidget(this);
    m_optionsTable->setColumnCount(4);
    m_optionsTable->setHorizontalHeaderLabels(QStringList()
                                              << "Select"
                                              << "Code"
                                              << "Name"
                                              << "Price (USD)");

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
    m_optionsTable->verticalHeader()->setDefaultSectionSize(34);

    optionsHeader->setStyleSheet(
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #cfcfcf;"
        " font-weight: 600;"
        " }"
        );

    optionsLayout->addWidget(m_optionsTable);

    mainLayout->addLayout(topBarLayout);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(machinesGroup);
    mainLayout->addWidget(toolingGroup);
    mainLayout->addWidget(quoteGroup);
    mainLayout->addWidget(optionsGroup, 1);

    connect(m_logoutButton, &QPushButton::clicked, this, &RecommendationPage::logoutRequested);
    connect(m_recommendButton, &QPushButton::clicked, this, &RecommendationPage::runRecommendation);
    connect(m_thicknessEdit, &QLineEdit::returnPressed, this, &RecommendationPage::runRecommendation);
    connect(m_bendLengthEdit, &QLineEdit::returnPressed, this, &RecommendationPage::runRecommendation);
    connect(m_selectedMachineComboBox, &QComboBox::currentIndexChanged, this, &RecommendationPage::updateQuoteSummary);
    connect(m_selectedToolingComboBox, &QComboBox::currentIndexChanged, this, &RecommendationPage::updateQuoteSummary);
}

void RecommendationPage::refreshData()
{
    loadMaterials();
    loadOptions();

    m_resultsTable->setRowCount(0);
    m_toolingResultsTable->setRowCount(0);
    m_selectedMachineComboBox->clear();
    m_selectedToolingComboBox->clear();
    m_machinePriceLabel->setText("-");
    m_toolingInfoLabel->setText("-");
    m_optionsTotalLabel->setText("0 USD");
    m_grandTotalLabel->setText("0 USD");
    m_statusLabel->clear();
    m_currentMachineResults.clear();
    m_currentToolingResults.clear();
}

void RecommendationPage::loadMaterials()
{
    QString errorMessage;
    const QList<MaterialRecord> materials = m_materialRepository.getAllMaterials(errorMessage);

    m_materialComboBox->clear();

    if (!errorMessage.isEmpty()) {
        m_statusLabel->setText("Error: " + errorMessage);
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
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
        m_statusLabel->setText("Error: " + errorMessage);
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    for (int i = 0; i < m_allOptions.size(); ++i) {
        const OptionRecord &option = m_allOptions[i];
        m_optionsTable->insertRow(i);

        auto *checkItem = new QTableWidgetItem;
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Unchecked);

        auto *codeItem = new QTableWidgetItem(option.code);
        auto *nameItem = new QTableWidgetItem(option.name);
        auto *priceItem = new QTableWidgetItem(formatRecommendationNumber(option.priceUsd, true));

        codeItem->setTextAlignment(Qt::AlignCenter);
        priceItem->setTextAlignment(Qt::AlignCenter);

        m_optionsTable->setItem(i, 0, checkItem);
        m_optionsTable->setItem(i, 1, codeItem);
        m_optionsTable->setItem(i, 2, nameItem);
        m_optionsTable->setItem(i, 3, priceItem);
    }

    connect(m_optionsTable, &QTableWidget::itemChanged, this, &RecommendationPage::updateQuoteSummary, Qt::UniqueConnection);
}

QList<RecommendationResult> RecommendationPage::buildRecommendations(const QString &materialName,
                                                                     double thickness,
                                                                     double bendLength,
                                                                     QString &errorMessage) const
{
    Q_UNUSED(materialName)

    QList<RecommendationResult> results;
    errorMessage.clear();

    const QList<MachineRecord> machines = m_machineRepository.getAllMachines(errorMessage);
    if (!errorMessage.isEmpty())
        return results;

    for (const MachineRecord &machine : machines) {
        bool thicknessOk = false;
        bool lengthOk = false;

        const double machineMaxThickness = QLocale::c().toDouble(machine.maxThicknessMm.trimmed(), &thicknessOk);
        const double machineWorkingLength = QLocale::c().toDouble(machine.workingLengthMm.trimmed(), &lengthOk);

        if (!thicknessOk || !lengthOk)
            continue;

        if (machineMaxThickness >= thickness && machineWorkingLength >= bendLength) {
            RecommendationResult result;
            result.machine = machine;
            result.reason =
                "Suitable because max thickness (" + machine.maxThicknessMm +
                " mm) and working length (" + machine.workingLengthMm +
                " mm) meet the requirement.";
            results.append(result);
        }
    }

    std::sort(results.begin(), results.end(),
              [thickness, bendLength](const RecommendationResult &a, const RecommendationResult &b) {
                  bool okA1 = false, okA2 = false, okA3 = false;
                  bool okB1 = false, okB2 = false, okB3 = false;

                  const double aThickness = QLocale::c().toDouble(a.machine.maxThicknessMm, &okA1);
                  const double aLength = QLocale::c().toDouble(a.machine.workingLengthMm, &okA2);
                  const double aPrice = QLocale::c().toDouble(a.machine.basePriceUSD, &okA3);

                  const double bThickness = QLocale::c().toDouble(b.machine.maxThicknessMm, &okB1);
                  const double bLength = QLocale::c().toDouble(b.machine.workingLengthMm, &okB2);
                  const double bPrice = QLocale::c().toDouble(b.machine.basePriceUSD, &okB3);

                  const double aThicknessExcess = okA1 ? (aThickness - thickness) : 1e12;
                  const double bThicknessExcess = okB1 ? (bThickness - thickness) : 1e12;

                  if (aThicknessExcess != bThicknessExcess)
                      return aThicknessExcess < bThicknessExcess;

                  const double aLengthExcess = okA2 ? (aLength - bendLength) : 1e12;
                  const double bLengthExcess = okB2 ? (bLength - bendLength) : 1e12;

                  if (aLengthExcess != bLengthExcess)
                      return aLengthExcess < bLengthExcess;

                  return (okA3 ? aPrice : 1e12) < (okB3 ? bPrice : 1e12);
              });

    return results;
}

QList<ToolingRecommendationResult> RecommendationPage::buildToolingRecommendations(double thickness,
                                                                                   QString &errorMessage) const
{
    QList<ToolingRecommendationResult> results;
    errorMessage.clear();

    const QList<ToolingRecord> toolingList = m_toolingRepository.getAllTooling(errorMessage);
    if (!errorMessage.isEmpty())
        return results;

    const double targetVdie = thickness * 8.0;

    for (const ToolingRecord &tooling : toolingList) {
        bool vOk = false;
        const double vDie = QLocale::c().toDouble(tooling.vDieMm.trimmed(), &vOk);
        if (!vOk)
            continue;

        ToolingRecommendationResult result;
        result.tooling = tooling;
        result.reason = "Closest to target V-die ≈ " + formatRecommendationNumber(QString::number(targetVdie)) + " mm.";
        results.append(result);
    }

    std::sort(results.begin(), results.end(),
              [targetVdie](const ToolingRecommendationResult &a, const ToolingRecommendationResult &b) {
                  bool okA = false;
                  bool okB = false;

                  const double aV = QLocale::c().toDouble(a.tooling.vDieMm, &okA);
                  const double bV = QLocale::c().toDouble(b.tooling.vDieMm, &okB);

                  const double aDiff = okA ? std::abs(aV - targetVdie) : 1e12;
                  const double bDiff = okB ? std::abs(bV - targetVdie) : 1e12;

                  return aDiff < bDiff;
              });

    if (results.size() > 5)
        results = results.mid(0, 5);

    return results;
}

void RecommendationPage::showMachineResults(const QList<RecommendationResult> &results)
{
    m_resultsTable->setRowCount(0);

    for (int i = 0; i < results.size(); ++i) {
        const RecommendationResult &r = results[i];
        m_resultsTable->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_resultsTable->setItem(i, col, item);
        };

        set(0, r.machine.model);
        set(1, formatRecommendationNumber(r.machine.maxTonnageTonf), true);
        set(2, formatRecommendationNumber(r.machine.workingLengthMm, true), true);
        set(3, formatRecommendationNumber(r.machine.maxThicknessMm), true);
        set(4, formatRecommendationNumber(r.machine.basePriceUSD, true), true);
        set(5, r.reason);
    }
}

void RecommendationPage::showToolingResults(const QList<ToolingRecommendationResult> &results)
{
    m_toolingResultsTable->setRowCount(0);

    for (int i = 0; i < results.size(); ++i) {
        const ToolingRecommendationResult &r = results[i];
        m_toolingResultsTable->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_toolingResultsTable->setItem(i, col, item);
        };

        set(0, r.tooling.name);
        set(1, formatRecommendationNumber(r.tooling.vDieMm), true);
        set(2, formatRecommendationNumber(r.tooling.punchRadiusMm), true);
        set(3, formatRecommendationNumber(r.tooling.dieRadiusMm), true);
        set(4, r.reason);
    }
}

void RecommendationPage::populateSelectionCombos()
{
    m_selectedMachineComboBox->clear();
    m_selectedToolingComboBox->clear();

    for (const RecommendationResult &result : m_currentMachineResults) {
        m_selectedMachineComboBox->addItem(result.machine.model);
    }

    for (const ToolingRecommendationResult &result : m_currentToolingResults) {
        m_selectedToolingComboBox->addItem(result.tooling.name);
    }

    updateQuoteSummary();
}

double RecommendationPage::selectedMachinePrice() const
{
    const int index = m_selectedMachineComboBox->currentIndex();
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

QString RecommendationPage::currentMachinePriceText() const
{
    return formatRecommendationNumber(QString::number(selectedMachinePrice()), true) + " USD";
}

void RecommendationPage::updateQuoteSummary()
{
    const double machinePrice = selectedMachinePrice();
    const double optionsTotal = selectedOptionsTotal();
    const double grandTotal = machinePrice + optionsTotal;

    if (machinePrice > 0.0)
        m_machinePriceLabel->setText(formatRecommendationNumber(QString::number(machinePrice), true) + " USD");
    else
        m_machinePriceLabel->setText("-");

    const int toolingIndex = m_selectedToolingComboBox->currentIndex();
    if (toolingIndex >= 0 && toolingIndex < m_currentToolingResults.size()) {
        const ToolingRecord &tooling = m_currentToolingResults[toolingIndex].tooling;
        m_toolingInfoLabel->setText(
            "V=" + formatRecommendationNumber(tooling.vDieMm) +
            " / Rp=" + formatRecommendationNumber(tooling.punchRadiusMm) +
            " / Rd=" + formatRecommendationNumber(tooling.dieRadiusMm)
            );
    } else {
        m_toolingInfoLabel->setText("-");
    }

    m_optionsTotalLabel->setText(formatRecommendationNumber(QString::number(optionsTotal), true) + " USD");
    m_grandTotalLabel->setText(formatRecommendationNumber(QString::number(grandTotal), true) + " USD");
}

void RecommendationPage::runRecommendation()
{
    if (m_materialComboBox->currentText().trimmed().isEmpty()) {
        m_statusLabel->setText("Error: Please select a material.");
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    if (!m_thicknessEdit->hasAcceptableInput() || !m_bendLengthEdit->hasAcceptableInput()) {
        m_statusLabel->setText("Error: Thickness and Bend Length must be valid numbers.");
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    bool thicknessOk = false;
    bool lengthOk = false;

    const double thickness = QLocale::c().toDouble(m_thicknessEdit->text().trimmed().replace(',', '.'), &thicknessOk);
    const double bendLength = QLocale::c().toDouble(m_bendLengthEdit->text().trimmed().replace(',', '.'), &lengthOk);

    if (!thicknessOk || thickness <= 0.0) {
        m_statusLabel->setText("Error: Thickness must be greater than 0.");
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    if (!lengthOk || bendLength <= 0.0) {
        m_statusLabel->setText("Error: Bend Length must be greater than 0.");
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    QString machineError;
    const QList<RecommendationResult> machineResults =
        buildRecommendations(m_materialComboBox->currentText(), thickness, bendLength, machineError);

    if (!machineError.isEmpty()) {
        m_statusLabel->setText("Error: " + machineError);
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    QString toolingError;
    const QList<ToolingRecommendationResult> toolingResults =
        buildToolingRecommendations(thickness, toolingError);

    if (!toolingError.isEmpty()) {
        m_statusLabel->setText("Error: " + toolingError);
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
        return;
    }

    m_currentMachineResults = machineResults;
    m_currentToolingResults = toolingResults;

    showMachineResults(machineResults);
    showToolingResults(toolingResults);
    populateSelectionCombos();

    if (machineResults.isEmpty()) {
        m_statusLabel->setText("No suitable machines found.");
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500;");
    } else {
        m_statusLabel->setText(QString::number(machineResults.size()) + " machine(s) found. Select machine, tooling, and options below.");
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600;");
    }
}
