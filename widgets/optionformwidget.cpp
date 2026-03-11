#include "optionformwidget.h"
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

OptionFormWidget::OptionFormWidget(QWidget *parent)
    : QWidget(parent),
    m_codeEdit(nullptr),
    m_nameEdit(nullptr),
    m_priceUsdEdit(nullptr),
    m_submitButton(nullptr),
    m_cancelEditButton(nullptr),
    m_statusLabel(nullptr)
{
    setupUi();
    setCreateMode();
}

void OptionFormWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Create / Update Option", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);
    groupLayout->setSpacing(12);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    m_codeEdit = new QLineEdit(this);
    m_codeEdit->setMinimumWidth(180);
    m_codeEdit->setPlaceholderText("e.g. OP-001");
    m_codeEdit->setToolTip("Unique option code.");
    m_codeEdit->setClearButtonEnabled(true);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setMinimumWidth(320);
    m_nameEdit->setPlaceholderText("e.g. Laser Safety System");
    m_nameEdit->setToolTip("Option name or description.");
    m_nameEdit->setClearButtonEnabled(true);

    m_priceUsdEdit = new QLineEdit(this);
    m_priceUsdEdit->setMinimumWidth(140);
    m_priceUsdEdit->setPlaceholderText("e.g. 4800");
    m_priceUsdEdit->setToolTip("Option price in USD.");

    auto *priceValidator = new QDoubleValidator(0.0, 1e12, 6, this);
    priceValidator->setNotation(QDoubleValidator::StandardNotation);
    priceValidator->setLocale(QLocale::c());
    m_priceUsdEdit->setValidator(priceValidator);

    grid->addWidget(new QLabel("Code:", this), 0, 0);
    grid->addWidget(m_codeEdit, 0, 1);
    grid->addWidget(new QLabel("Name:", this), 0, 2);
    grid->addWidget(m_nameEdit, 0, 3);

    grid->addWidget(new QLabel("Price (USD):", this), 1, 0);
    grid->addWidget(m_priceUsdEdit, 1, 1);

    m_submitButton = new QPushButton("CREATE OPTION", this);
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

    connect(m_submitButton, &QPushButton::clicked, this, &OptionFormWidget::submitRequested);
    connect(m_cancelEditButton, &QPushButton::clicked, this, &OptionFormWidget::cancelEditRequested);

    connect(m_codeEdit, &QLineEdit::returnPressed, this, &OptionFormWidget::submitRequested);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &OptionFormWidget::submitRequested);
    connect(m_priceUsdEdit, &QLineEdit::returnPressed, this, &OptionFormWidget::submitRequested);
}

QString OptionFormWidget::code() const { return m_codeEdit->text().trimmed(); }
QString OptionFormWidget::name() const { return m_nameEdit->text().trimmed(); }
QString OptionFormWidget::priceUsd() const { return m_priceUsdEdit->text().trimmed(); }

bool OptionFormWidget::hasAcceptableNumericInputs() const
{
    return m_priceUsdEdit->hasAcceptableInput();
}

void OptionFormWidget::setCreateMode()
{
    clearForm();
    m_submitButton->setText("CREATE OPTION");
    m_cancelEditButton->hide();
    setStyleSheet("");
}

void OptionFormWidget::setEditMode(const OptionRecord &option)
{
    m_codeEdit->setText(option.code);
    m_nameEdit->setText(option.name);
    m_priceUsdEdit->setText(option.priceUsd);

    m_submitButton->setText("UPDATE OPTION");
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

void OptionFormWidget::clearForm()
{
    m_codeEdit->clear();
    m_nameEdit->clear();
    m_priceUsdEdit->clear();
}

void OptionFormWidget::setStatusMessage(const QString &text)
{
    m_statusLabel->setText(text);

    if (text.startsWith("Error:", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600;");
    else if (text.contains("successfully", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600;");
    else
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500;");
}

void OptionFormWidget::clearStatusMessage()
{
    m_statusLabel->clear();
    m_statusLabel->setStyleSheet("");
}

void OptionFormWidget::focusCode()
{
    m_codeEdit->setFocus();
}
