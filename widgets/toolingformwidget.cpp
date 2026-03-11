#include "toolingformwidget.h"
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

ToolingFormWidget::ToolingFormWidget(QWidget *parent)
    : QWidget(parent),
    m_nameEdit(nullptr),
    m_vDieEdit(nullptr),
    m_punchRadiusEdit(nullptr),
    m_dieRadiusEdit(nullptr),
    m_submitButton(nullptr),
    m_cancelEditButton(nullptr),
    m_statusLabel(nullptr)
{
    setupUi();
    setCreateMode();
}

void ToolingFormWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Create / Update Tooling", this);
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

    m_nameEdit = makeField(240);
    m_nameEdit->setPlaceholderText("e.g. V8 Standard");
    m_nameEdit->setToolTip("Tooling name or label.");
    m_nameEdit->setClearButtonEnabled(true);

    m_vDieEdit = makeField();
    m_vDieEdit->setPlaceholderText("e.g. 8");
    m_vDieEdit->setToolTip("V-opening width in mm.");

    m_punchRadiusEdit = makeField();
    m_punchRadiusEdit->setPlaceholderText("e.g. 1");
    m_punchRadiusEdit->setToolTip("Punch tip radius in mm.");

    m_dieRadiusEdit = makeField();
    m_dieRadiusEdit->setPlaceholderText("e.g. 1.5");
    m_dieRadiusEdit->setToolTip("Die shoulder or effective die radius in mm.");

    auto *positiveValidator = new QDoubleValidator(0.0, 1e12, 6, this);
    positiveValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveValidator->setLocale(QLocale::c());

    m_vDieEdit->setValidator(positiveValidator);
    m_punchRadiusEdit->setValidator(positiveValidator);
    m_dieRadiusEdit->setValidator(positiveValidator);

    grid->addWidget(new QLabel("Name:", this), 0, 0);
    grid->addWidget(m_nameEdit, 0, 1, 1, 3);

    grid->addWidget(new QLabel("V-die (mm):", this), 1, 0);
    grid->addWidget(m_vDieEdit, 1, 1);
    grid->addWidget(new QLabel("Punch Radius (mm):", this), 1, 2);
    grid->addWidget(m_punchRadiusEdit, 1, 3);

    grid->addWidget(new QLabel("Die Radius (mm):", this), 2, 0);
    grid->addWidget(m_dieRadiusEdit, 2, 1);

    m_submitButton = new QPushButton("CREATE TOOLING", this);
    m_submitButton->setFixedHeight(34);
    m_submitButton->setMinimumWidth(150);
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

    connect(m_submitButton, &QPushButton::clicked, this, &ToolingFormWidget::submitRequested);
    connect(m_cancelEditButton, &QPushButton::clicked, this, &ToolingFormWidget::cancelEditRequested);

    connect(m_nameEdit, &QLineEdit::returnPressed, this, &ToolingFormWidget::submitRequested);
    connect(m_vDieEdit, &QLineEdit::returnPressed, this, &ToolingFormWidget::submitRequested);
    connect(m_punchRadiusEdit, &QLineEdit::returnPressed, this, &ToolingFormWidget::submitRequested);
    connect(m_dieRadiusEdit, &QLineEdit::returnPressed, this, &ToolingFormWidget::submitRequested);
}

QString ToolingFormWidget::name() const { return m_nameEdit->text().trimmed(); }
QString ToolingFormWidget::vDieMm() const { return m_vDieEdit->text().trimmed(); }
QString ToolingFormWidget::punchRadiusMm() const { return m_punchRadiusEdit->text().trimmed(); }
QString ToolingFormWidget::dieRadiusMm() const { return m_dieRadiusEdit->text().trimmed(); }

bool ToolingFormWidget::hasAcceptableNumericInputs() const
{
    return m_vDieEdit->hasAcceptableInput()
    && m_punchRadiusEdit->hasAcceptableInput()
        && m_dieRadiusEdit->hasAcceptableInput();
}

void ToolingFormWidget::setCreateMode()
{
    clearForm();
    m_submitButton->setText("CREATE TOOLING");
    m_cancelEditButton->hide();
    setStyleSheet("");
}

void ToolingFormWidget::setEditMode(const ToolingRecord &tooling)
{
    m_nameEdit->setText(tooling.name);
    m_vDieEdit->setText(tooling.vDieMm);
    m_punchRadiusEdit->setText(tooling.punchRadiusMm);
    m_dieRadiusEdit->setText(tooling.dieRadiusMm);

    m_submitButton->setText("UPDATE TOOLING");
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

void ToolingFormWidget::clearForm()
{
    m_nameEdit->clear();
    m_vDieEdit->clear();
    m_punchRadiusEdit->clear();
    m_dieRadiusEdit->clear();
}

void ToolingFormWidget::setStatusMessage(const QString &text)
{
    m_statusLabel->setText(text);

    if (text.startsWith("Error:", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
    else if (text.contains("successfully", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600;");
    else
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500;");
}

void ToolingFormWidget::clearStatusMessage()
{
    m_statusLabel->clear();
    m_statusLabel->setStyleSheet("");
}

void ToolingFormWidget::focusName()
{
    m_nameEdit->setFocus();
}
