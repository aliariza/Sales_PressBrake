#include "optionstablewidget.h"
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

static QString formatOptionNumberForDisplay(const QString &text, bool useGrouping = false)
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

OptionsTableWidget::OptionsTableWidget(QWidget *parent)
    : QWidget(parent),
    m_table(nullptr)
{
    setupUi();
}

void OptionsTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Existing Options", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels(QStringList()
                                       << "No"
                                       << "Code"
                                       << "Name"
                                       << "Price (USD)"
                                       << "Action");

    auto *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(2, QHeaderView::Stretch);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header->setDefaultAlignment(Qt::AlignCenter);

    m_table->horizontalHeaderItem(0)->setToolTip("Auto-generated option number");
    m_table->horizontalHeaderItem(1)->setToolTip("Unique option code");
    m_table->horizontalHeaderItem(2)->setToolTip("Option name or description");
    m_table->horizontalHeaderItem(3)->setToolTip("Option price in USD");
    m_table->horizontalHeaderItem(4)->setToolTip("Available actions");

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

void OptionsTableWidget::setOptions(const QList<OptionRecord> &options)
{
    m_table->setRowCount(0);

    for (int i = 0; i < options.size(); ++i) {
        const OptionRecord o = options[i];
        m_table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, col, item);
        };

        set(0, o.no, true);
        set(1, o.code, true);
        set(2, o.name);
        set(3, formatOptionNumberForDisplay(o.priceUsd, true), true);

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

        connect(editButton, &QPushButton::clicked, this, [this, o]() { emit editRequested(o); });
        connect(deleteButton, &QPushButton::clicked, this, [this, o]() { emit deleteRequested(o); });

        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);

        m_table->setCellWidget(i, 4, actionsWidget);
    }
}
