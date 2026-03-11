#include "machineformwidget.h"
#include "stylehelper.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QDoubleValidator>
#include <QLocale>

MachineFormWidget::MachineFormWidget(QWidget *parent)
    : QWidget(parent),
    m_modelEdit(nullptr),
    m_maxTonnageTonfEdit(nullptr),
    m_workingLengthMmEdit(nullptr),
    m_maxThicknessMmEdit(nullptr),
    m_basePriceUSDEdit(nullptr),
    m_submitButton(nullptr),
    m_cancelEditButton(nullptr),
    m_statusLabel(nullptr)
{
    setupUi();
    setCreateMode();
}

void MachineFormWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Create / Update Machine", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);
    groupLayout->setSpacing(12);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    auto makeField = [this](int minWidth = 140) {
        auto *e = new QLineEdit(this);
        e->setMinimumWidth(minWidth);
        return e;
    };

    m_modelEdit = makeField(240);
    m_modelEdit->setPlaceholderText("e.g. PB-100T-3200");
    m_modelEdit->setToolTip("Machine model or label.");
    m_modelEdit->setClearButtonEnabled(true);

    m_maxTonnageTonfEdit = makeField();
    m_maxTonnageTonfEdit->setPlaceholderText("e.g. 100");
    m_maxTonnageTonfEdit->setToolTip("Maximum tonnage capacity in tonf.");

    m_workingLengthMmEdit = makeField();
    m_workingLengthMmEdit->setPlaceholderText("e.g. 3200");
    m_workingLengthMmEdit->setToolTip("Working length in mm.");

    m_maxThicknessMmEdit = makeField();
    m_maxThicknessMmEdit->setPlaceholderText("e.g. 8");
    m_maxThicknessMmEdit->setToolTip("Maximum sheet thickness in mm.");

    m_basePriceUSDEdit = makeField();
    m_basePriceUSDEdit->setPlaceholderText("e.g. 28500");
    m_basePriceUSDEdit->setToolTip("Base machine price in USD.");

    auto *positiveValidator = new QDoubleValidator(0.0, 1e12, 6, this);
    positiveValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveValidator->setLocale(QLocale::c());

    m_maxTonnageTonfEdit->setValidator(positiveValidator);
    m_workingLengthMmEdit->setValidator(positiveValidator);
    m_maxThicknessMmEdit->setValidator(positiveValidator);
    m_basePriceUSDEdit->setValidator(positiveValidator);

    grid->addWidget(new QLabel("Model:", this), 0, 0);
    grid->addWidget(m_modelEdit, 0, 1, 1, 3);

    grid->addWidget(new QLabel("Max Tonnage (tonf):", this), 1, 0);
    grid->addWidget(m_maxTonnageTonfEdit, 1, 1);
    grid->addWidget(new QLabel("Working Length (mm):", this), 1, 2);
    grid->addWidget(m_workingLengthMmEdit, 1, 3);

    grid->addWidget(new QLabel("Max Thickness (mm):", this), 2, 0);
    grid->addWidget(m_maxThicknessMmEdit, 2, 1);
    grid->addWidget(new QLabel("Base Price (USD):", this), 2, 2);
    grid->addWidget(m_basePriceUSDEdit, 2, 3);

    m_submitButton = new QPushButton("CREATE MACHINE", this);
    m_submitButton->setFixedHeight(34);
    m_submitButton->setMinimumWidth(160);
    StyleHelper::applyPrimaryButtonStyle(m_submitButton);

    m_cancelEditButton = new QPushButton("CANCEL EDIT", this);
    m_cancelEditButton->setFixedHeight(34);
    m_cancelEditButton->setMinimumWidth(130);
    StyleHelper::applySecondaryButtonStyle(m_cancelEditButton);
    m_cancelEditButton->hide();

    auto *buttonRow = new QHBoxLayout;
    buttonRow->addStretch();
    buttonRow->addWidget(m_submitButton);
    buttonRow->addWidget(m_cancelEditButton);

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    groupLayout->addLayout(grid);
    groupLayout->addLayout(buttonRow);
    groupLayout->addSpacing(4);
    groupLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(group);

    connect(m_submitButton, &QPushButton::clicked, this, &MachineFormWidget::submitRequested);
    connect(m_cancelEditButton, &QPushButton::clicked, this, &MachineFormWidget::cancelEditRequested);

    connect(m_modelEdit, &QLineEdit::returnPressed, this, &MachineFormWidget::submitRequested);
    connect(m_maxTonnageTonfEdit, &QLineEdit::returnPressed, this, &MachineFormWidget::submitRequested);
    connect(m_workingLengthMmEdit, &QLineEdit::returnPressed, this, &MachineFormWidget::submitRequested);
    connect(m_maxThicknessMmEdit, &QLineEdit::returnPressed, this, &MachineFormWidget::submitRequested);
    connect(m_basePriceUSDEdit, &QLineEdit::returnPressed, this, &MachineFormWidget::submitRequested);
}

QString MachineFormWidget::model() const { return m_modelEdit->text().trimmed(); }
QString MachineFormWidget::maxTonnageTonf() const { return m_maxTonnageTonfEdit->text().trimmed(); }
QString MachineFormWidget::workingLengthMm() const { return m_workingLengthMmEdit->text().trimmed(); }
QString MachineFormWidget::maxThicknessMm() const { return m_maxThicknessMmEdit->text().trimmed(); }
QString MachineFormWidget::basePriceUSD() const { return m_basePriceUSDEdit->text().trimmed(); }

bool MachineFormWidget::hasAcceptableNumericInputs() const
{
    return m_maxTonnageTonfEdit->hasAcceptableInput()
    && m_workingLengthMmEdit->hasAcceptableInput()
        && m_maxThicknessMmEdit->hasAcceptableInput()
        && m_basePriceUSDEdit->hasAcceptableInput();
}

void MachineFormWidget::setCreateMode()
{
    clearForm();
    m_submitButton->setText("CREATE MACHINE");
    m_cancelEditButton->hide();
    setStyleSheet("");
}

void MachineFormWidget::setEditMode(const MachineRecord &machine)
{
    m_modelEdit->setText(machine.model);
    m_maxTonnageTonfEdit->setText(machine.maxTonnageTonf);
    m_workingLengthMmEdit->setText(machine.workingLengthMm);
    m_maxThicknessMmEdit->setText(machine.maxThicknessMm);
    m_basePriceUSDEdit->setText(machine.basePriceUSD);

    m_submitButton->setText("UPDATE MACHINE");
    m_cancelEditButton->show();

    setStyleSheet(
        "QGroupBox {"
        " border: 1px solid #7aa7ff;"
        " border-radius: 6px;"
        " margin-top: 8px;"
        " }"
        "QGroupBox::title {"
        " subcontrol-origin: margin;"
        " left: 10px;"
        " padding: 0 4px;"
        " }"
        );
}

void MachineFormWidget::clearForm()
{
    m_modelEdit->clear();
    m_maxTonnageTonfEdit->clear();
    m_workingLengthMmEdit->clear();
    m_maxThicknessMmEdit->clear();
    m_basePriceUSDEdit->clear();
}

void MachineFormWidget::setStatusMessage(const QString &text)
{
    m_statusLabel->setText(text);

    if (text.startsWith("Error:", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
    else if (text.contains("successfully", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600;");
    else
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500;");
}

void MachineFormWidget::clearStatusMessage()
{
    m_statusLabel->clear();
    m_statusLabel->setStyleSheet("");
}

void MachineFormWidget::focusModel()
{
    m_modelEdit->setFocus();
}
