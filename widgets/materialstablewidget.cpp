#include "materialstablewidget.h"
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

static QString normalizeDecimalForDisplay(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

static QString formatNumberForDisplay(const QString &text, int decimals = 6, bool useGrouping = false)
{
    const QString normalized = normalizeDecimalForDisplay(text);

    bool ok = false;
    const double number = QLocale::c().toDouble(normalized, &ok);

    if (!ok)
        return normalized;

    QString result = QLocale::c().toString(number, 'f', decimals);

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
MaterialsTableWidget::MaterialsTableWidget(QWidget *parent)
    : QWidget(parent),
    m_table(nullptr)
{
    setupUi();
}

void MaterialsTableWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Existing Materials", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(10);
    m_table->setHorizontalHeaderLabels(QStringList()
                                       << "No"
                                       << "Name"
                                       << "Rm (MPa)"
                                       << "Re (MPa)"
                                       << "K"
                                       << "E (MPa)"
                                       << "V Factor"
                                       << "Min t (mm)"
                                       << "Max t (mm)"
                                       << "Action");
    auto *header = m_table->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // No
    header->setSectionResizeMode(1, QHeaderView::Stretch);          // Name
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Rm
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Re
    header->setSectionResizeMode(4, QHeaderView::ResizeToContents); // K
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents); // E
    header->setSectionResizeMode(6, QHeaderView::ResizeToContents); // V
    header->setSectionResizeMode(7, QHeaderView::ResizeToContents); // Min t
    header->setSectionResizeMode(8, QHeaderView::ResizeToContents); // Max t
    header->setSectionResizeMode(9, QHeaderView::ResizeToContents); // Action

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setWordWrap(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->verticalHeader()->setDefaultSectionSize(34);

    m_table->horizontalHeaderItem(2)->setToolTip("Ultimate tensile strength in MPa");
    m_table->horizontalHeaderItem(3)->setToolTip("Yield strength in MPa");
    m_table->horizontalHeaderItem(4)->setToolTip("Default bend K factor");
    m_table->horizontalHeaderItem(5)->setToolTip("Young's modulus in MPa");
    m_table->horizontalHeaderItem(6)->setToolTip("Recommended V-die factor");
    m_table->horizontalHeaderItem(7)->setToolTip("Minimum thickness in mm");
    m_table->horizontalHeaderItem(8)->setToolTip("Maximum thickness in mm");

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

void MaterialsTableWidget::setMaterials(const QList<MaterialRecord> &materials)
{
    m_table->setRowCount(0);

    for (int i = 0; i < materials.size(); ++i) {
        const MaterialRecord m = materials[i];
        m_table->insertRow(i);

        auto set = [&](int col, const QString &text, bool center = false) {
            auto *item = new QTableWidgetItem(text);
            if (center)
                item->setTextAlignment(Qt::AlignCenter);
            m_table->setItem(i, col, item);
        };

        set(0, m.no, true);
        set(1, m.name);
        set(2, formatNumberForDisplay(m.tensileStrengthMPa, 6, true), true);
        set(3, formatNumberForDisplay(m.yieldStrengthMPa, 6, true), true);
        set(4, formatNumberForDisplay(m.kFactorDefault, 6, false), true);
        set(5, formatNumberForDisplay(m.youngsModulusMPa, 6, true), true);
        set(6, formatNumberForDisplay(m.recommendedVdieFactor, 6, false), true);
        set(7, formatNumberForDisplay(m.minThicknessMm, 6, false), true);
        set(8, formatNumberForDisplay(m.maxThicknessMm, 6, false), true);

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

        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);

        m_table->setCellWidget(i, 9, actionsWidget);
    }
}
