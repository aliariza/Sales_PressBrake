#include "materialformwidget.h"
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

MaterialFormWidget::MaterialFormWidget(QWidget *parent)
    : QWidget(parent),
    m_nameEdit(nullptr),
    m_tensileStrengthEdit(nullptr),
    m_yieldStrengthEdit(nullptr),
    m_kFactorEdit(nullptr),
    m_youngsModulusEdit(nullptr),
    m_recommendedVdieFactorEdit(nullptr),
    m_minThicknessEdit(nullptr),
    m_maxThicknessEdit(nullptr),
    m_submitButton(nullptr),
    m_cancelEditButton(nullptr),
    m_statusLabel(nullptr)
{
    setupUi();
    setCreateMode();
}

void MaterialFormWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);

    auto *group = new QGroupBox("Create / Update Material", this);
    group->setFont(sectionFont);

    auto *groupLayout = new QVBoxLayout(group);
    groupLayout->setContentsMargins(16, 20, 16, 16);
    groupLayout->setSpacing(12);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);

    auto makeField = [this](int minWidth = 120) {
        auto *e = new QLineEdit(this);
        e->setMinimumWidth(minWidth);
        return e;
    };

    m_nameEdit = makeField(220);
    m_nameEdit->setPlaceholderText("e.g. S235JR");
    m_nameEdit->setToolTip("Material name or grade.");
    m_nameEdit->setClearButtonEnabled(true);

    m_tensileStrengthEdit = makeField();
    m_tensileStrengthEdit->setPlaceholderText("e.g. 360");
    m_tensileStrengthEdit->setToolTip("Ultimate tensile strength (Rm) in MPa.");

    m_yieldStrengthEdit = makeField();
    m_yieldStrengthEdit->setPlaceholderText("e.g. 235");
    m_yieldStrengthEdit->setToolTip("Yield strength (Re) in MPa.");

    m_kFactorEdit = makeField();
    m_kFactorEdit->setPlaceholderText("0.00 .. 1.00");
    m_kFactorEdit->setToolTip("Default bend K factor. Use dot as decimal separator, for example 0.25.");

    m_youngsModulusEdit = makeField();
    m_youngsModulusEdit->setPlaceholderText("e.g. 210000");
    m_youngsModulusEdit->setToolTip("Young's modulus (E) in MPa.");

    m_recommendedVdieFactorEdit = makeField();
    m_recommendedVdieFactorEdit->setPlaceholderText("e.g. 6 or 8");
    m_recommendedVdieFactorEdit->setToolTip("Recommended V-die factor. Positive number.");

    m_minThicknessEdit = makeField();
    m_minThicknessEdit->setPlaceholderText("e.g. 0.5");
    m_minThicknessEdit->setToolTip("Minimum valid thickness in mm. Use dot as decimal separator.");

    m_maxThicknessEdit = makeField();
    m_maxThicknessEdit->setPlaceholderText("e.g. 6.0");
    m_maxThicknessEdit->setToolTip("Maximum valid thickness in mm. Must be greater than or equal to minimum thickness.");

    auto *positiveValidator = new QDoubleValidator(0.0, 1e12, 6, this);
    positiveValidator->setNotation(QDoubleValidator::StandardNotation);
    positiveValidator->setLocale(QLocale::c());

    auto *kFactorValidator = new QDoubleValidator(0.0, 1.0, 6, this);
    kFactorValidator->setNotation(QDoubleValidator::StandardNotation);
    kFactorValidator->setLocale(QLocale::c());

    m_tensileStrengthEdit->setValidator(positiveValidator);
    m_yieldStrengthEdit->setValidator(positiveValidator);
    m_youngsModulusEdit->setValidator(positiveValidator);
    m_recommendedVdieFactorEdit->setValidator(positiveValidator);
    m_minThicknessEdit->setValidator(positiveValidator);
    m_maxThicknessEdit->setValidator(positiveValidator);
    m_kFactorEdit->setValidator(kFactorValidator);

    grid->addWidget(new QLabel("Name:", this), 0, 0);
    grid->addWidget(m_nameEdit, 0, 1, 1, 3);
    grid->addWidget(new QLabel("Rm (MPa):", this), 0, 4);
    grid->addWidget(m_tensileStrengthEdit, 0, 5);

    grid->addWidget(new QLabel("Re (MPa):", this), 1, 0);
    grid->addWidget(m_yieldStrengthEdit, 1, 1);
    grid->addWidget(new QLabel("K Factor:", this), 1, 2);
    grid->addWidget(m_kFactorEdit, 1, 3);
    grid->addWidget(new QLabel("E (MPa):", this), 1, 4);
    grid->addWidget(m_youngsModulusEdit, 1, 5);

    grid->addWidget(new QLabel("V-die Factor:", this), 2, 0);
    grid->addWidget(m_recommendedVdieFactorEdit, 2, 1);
    grid->addWidget(new QLabel("Min t (mm):", this), 2, 2);
    grid->addWidget(m_minThicknessEdit, 2, 3);
    grid->addWidget(new QLabel("Max t (mm):", this), 2, 4);
    grid->addWidget(m_maxThicknessEdit, 2, 5);

    m_submitButton = new QPushButton("CREATE MATERIAL", this);
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

    connect(m_submitButton, &QPushButton::clicked, this, &MaterialFormWidget::submitRequested);
    connect(m_cancelEditButton, &QPushButton::clicked, this, &MaterialFormWidget::cancelEditRequested);

    connect(m_nameEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_tensileStrengthEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_yieldStrengthEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_kFactorEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_youngsModulusEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_recommendedVdieFactorEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_minThicknessEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
    connect(m_maxThicknessEdit, &QLineEdit::returnPressed, this, &MaterialFormWidget::submitRequested);
}

QString MaterialFormWidget::name() const { return m_nameEdit->text().trimmed(); }
QString MaterialFormWidget::tensileStrengthMPa() const { return m_tensileStrengthEdit->text().trimmed(); }
QString MaterialFormWidget::yieldStrengthMPa() const { return m_yieldStrengthEdit->text().trimmed(); }
QString MaterialFormWidget::kFactorDefault() const { return m_kFactorEdit->text().trimmed(); }
QString MaterialFormWidget::youngsModulusMPa() const { return m_youngsModulusEdit->text().trimmed(); }
QString MaterialFormWidget::recommendedVdieFactor() const { return m_recommendedVdieFactorEdit->text().trimmed(); }
QString MaterialFormWidget::minThicknessMm() const { return m_minThicknessEdit->text().trimmed(); }
QString MaterialFormWidget::maxThicknessMm() const { return m_maxThicknessEdit->text().trimmed(); }

bool MaterialFormWidget::hasAcceptableKFactor() const
{
    return m_kFactorEdit && m_kFactorEdit->hasAcceptableInput();
}

bool MaterialFormWidget::hasAcceptableNumericInputs() const
{
    return m_tensileStrengthEdit->hasAcceptableInput()
    && m_yieldStrengthEdit->hasAcceptableInput()
        && m_kFactorEdit->hasAcceptableInput()
        && m_youngsModulusEdit->hasAcceptableInput()
        && m_recommendedVdieFactorEdit->hasAcceptableInput()
        && m_minThicknessEdit->hasAcceptableInput()
        && m_maxThicknessEdit->hasAcceptableInput();
}

void MaterialFormWidget::setCreateMode()
{
    clearForm();
    m_submitButton->setText("CREATE MATERIAL");
    m_cancelEditButton->hide();
    setStyleSheet("");
}

void MaterialFormWidget::setEditMode(const MaterialRecord &material)
{
    m_nameEdit->setText(material.name);
    m_tensileStrengthEdit->setText(material.tensileStrengthMPa);
    m_yieldStrengthEdit->setText(material.yieldStrengthMPa);
    m_kFactorEdit->setText(material.kFactorDefault);
    m_youngsModulusEdit->setText(material.youngsModulusMPa);
    m_recommendedVdieFactorEdit->setText(material.recommendedVdieFactor);
    m_minThicknessEdit->setText(material.minThicknessMm);
    m_maxThicknessEdit->setText(material.maxThicknessMm);

    m_submitButton->setText("UPDATE MATERIAL");
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

void MaterialFormWidget::clearForm()
{
    m_nameEdit->clear();
    m_tensileStrengthEdit->clear();
    m_yieldStrengthEdit->clear();
    m_kFactorEdit->clear();
    m_youngsModulusEdit->clear();
    m_recommendedVdieFactorEdit->clear();
    m_minThicknessEdit->clear();
    m_maxThicknessEdit->clear();
}

void MaterialFormWidget::setStatusMessage(const QString &text)
{
    m_statusLabel->setText(text);

    if (text.startsWith("Error:", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #C62828; font-weight: 600; padding-top: 4px;");
    else if (text.contains("successfully", Qt::CaseInsensitive))
        m_statusLabel->setStyleSheet("color: #2E7D32; font-weight: 600; padding-top: 4px;");
    else
        m_statusLabel->setStyleSheet("color: #444444; font-weight: 500; padding-top: 4px;");
}

void MaterialFormWidget::clearStatusMessage()
{
    m_statusLabel->clear();
    m_statusLabel->setStyleSheet("");
}

void MaterialFormWidget::focusName()
{
    m_nameEdit->setFocus();
}
