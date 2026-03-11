#include "machinestablewidget.h"
#include "stylehelper.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QFont>
#include <QLocale>

static QString normalizeMachineNumberForDisplay(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

static QString formatMachineNumberForDisplay(const QString &text, bool useGrouping = false)
{
    const QString normalized = normalizeMachineNumberForDisplay(text);

    bool ok = false;
    const double number = QLocale::c().toDouble(normalized, &ok);
    if (!ok)
        return normalized;

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

MachinesTableWidget::MachinesTableWidget(QWidget *parent)
    : QWidget(parent),
    m_table(nullptr)
{
    setupUi();
}

void MachinesTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Existing Machines", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels(QStringList()
                                       << "No"
                                       << "Model"
                                       << "Tonnage (tonf)"
                                       << "Length (mm)"
                                       << "Max t (mm)"
                                       << "Base Price (USD)"
                                       << "Action");

    m_table->horizontalHeaderItem(0)->setToolTip("Auto-generated machine number");
    m_table->horizontalHeaderItem(1)->setToolTip("Machine model or label");
    m_table->horizontalHeaderItem(2)->setToolTip("Maximum tonnage capacity in tonf");
    m_table->horizontalHeaderItem(3)->setToolTip("Working length in millimeters");
    m_table->horizontalHeaderItem(4)->setToolTip("Maximum sheet thickness in millimeters");
    m_table->horizontalHeaderItem(5)->setToolTip("Base machine price in USD");
    m_table->horizontalHeaderItem(6)->setToolTip("Available actions");

    auto *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    header->setDefaultAlignment(Qt::AlignCenter);

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setWordWrap(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);

    header->setStyleSheet(
        "QHeaderView::section {"
        " padding: 6px 8px;"
        " border: none;"
        " border-bottom: 1px solid #cfcfcf;"
        " font-weight: 600;"
        " }"
        );

    groupLayout->addWidget(m_table);
    mainLayout->addWidget(group);
}

void MachinesTableWidget::setMachines(const QList<MachineRecord> &machines)
{
    m_table->setRowCount(0);

    for (int i = 0; i < machines.size(); ++i) {
        const MachineRecord m = machines[i];
        m_table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, col, item);
        };

        set(0, m.no, true);
        set(1, m.model);
        set(2, formatMachineNumberForDisplay(m.maxTonnageTonf), true);
        set(3, formatMachineNumberForDisplay(m.workingLengthMm, true), true);
        set(4, formatMachineNumberForDisplay(m.maxThicknessMm), true);
        set(5, formatMachineNumberForDisplay(m.basePriceUSD, true), true);

        auto *actionsWidget = new QWidget(m_table);
        auto *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(4, 0, 4, 0);
        actionsLayout->setSpacing(6);

        auto *editButton = new QPushButton("Edit", actionsWidget);
        auto *deleteButton = new QPushButton("Delete", actionsWidget);
        editButton->setFixedHeight(24);
        deleteButton->setFixedHeight(24);
        editButton->setMinimumWidth(56);
        deleteButton->setMinimumWidth(64);
        StyleHelper::applySecondaryButtonStyle(editButton);
        StyleHelper::applySecondaryButtonStyle(deleteButton);

        connect(editButton, &QPushButton::clicked, this, [this, m]() { emit editRequested(m); });
        connect(deleteButton, &QPushButton::clicked, this, [this, m]() { emit deleteRequested(m); });

        actionsLayout->addStretch();
        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);
        actionsLayout->addStretch();

        m_table->setCellWidget(i, 6, actionsWidget);
    }
}
