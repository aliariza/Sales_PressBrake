#include "materialspage.h"
#include "materialformwidget.h"
#include "materialstablewidget.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

static QString normalizeDecimal(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

MaterialsPage::MaterialsPage(QWidget *parent)
    : QWidget(parent),
    m_formWidget(nullptr),
    m_tableWidget(nullptr),
    m_isEditMode(false)
{
    QString errorMessage;
    if (!m_materialRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshMaterialsTable();
}

void MaterialsPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    auto *topBarLayout = new QHBoxLayout;
    topBarLayout->addStretch();


    m_formWidget = new MaterialFormWidget(this);
    m_tableWidget = new MaterialsTableWidget(this);

    layout->addLayout(topBarLayout);
    layout->addWidget(m_formWidget);
    layout->addWidget(m_tableWidget, 1);

    connect(m_formWidget, &MaterialFormWidget::submitRequested, this, &MaterialsPage::createOrUpdateMaterial);
    connect(m_formWidget, &MaterialFormWidget::cancelEditRequested, this, &MaterialsPage::cancelEdit);
    connect(m_tableWidget, &MaterialsTableWidget::editRequested, this, &MaterialsPage::handleEditRequested);
    connect(m_tableWidget, &MaterialsTableWidget::deleteRequested, this, &MaterialsPage::handleDeleteRequested);
}

void MaterialsPage::enterEditMode(const MaterialRecord &material)
{
    m_isEditMode = true;
    m_editingNo = material.no;
    m_formWidget->setEditMode(material);
    m_formWidget->setStatusMessage("Edit mode.");
    m_formWidget->focusName();
}

void MaterialsPage::exitEditMode()
{
    m_isEditMode = false;
    m_editingNo.clear();
    m_formWidget->setCreateMode();
}

void MaterialsPage::cancelEdit()
{
    exitEditMode();
    m_formWidget->clearStatusMessage();
    m_formWidget->focusName();
}

void MaterialsPage::refreshMaterialsTable()
{
    QString errorMessage;
    const QList<MaterialRecord> materials = m_materialRepository.getAllMaterials(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_tableWidget->setMaterials(materials);
}

void MaterialsPage::handleEditRequested(const MaterialRecord &material)
{
    m_formWidget->clearStatusMessage();
    enterEditMode(material);
}

void MaterialsPage::handleDeleteRequested(const MaterialRecord &material)
{
    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Delete Material",
                              "Delete material '" + material.name + "'?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_materialRepository.deleteMaterial(material.no, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setStatusMessage("Material deleted successfully.");

    if (m_isEditMode && m_editingNo == material.no)
        exitEditMode();

    refreshMaterialsTable();
}

void MaterialsPage::createOrUpdateMaterial()
{
    QString errorMessage;

    if (m_formWidget->name().isEmpty()) {
        m_formWidget->setStatusMessage("Error: Material name cannot be empty.");
        return;
    }

    if (!m_formWidget->hasAcceptableNumericInputs()) {
        m_formWidget->setStatusMessage("Error: One or more numeric fields are invalid. Use dot as decimal separator.");
        return;
    }

    MaterialRecord material;
    material.no = m_isEditMode ? m_editingNo : m_materialRepository.getNextMaterialNo(errorMessage);
    material.name = m_formWidget->name();
    material.tensileStrengthMPa = normalizeDecimal(m_formWidget->tensileStrengthMPa());
    material.yieldStrengthMPa = normalizeDecimal(m_formWidget->yieldStrengthMPa());
    material.kFactorDefault = normalizeDecimal(m_formWidget->kFactorDefault());
    material.youngsModulusMPa = normalizeDecimal(m_formWidget->youngsModulusMPa());
    material.recommendedVdieFactor = normalizeDecimal(m_formWidget->recommendedVdieFactor());
    material.minThicknessMm = normalizeDecimal(m_formWidget->minThicknessMm());
    material.maxThicknessMm = normalizeDecimal(m_formWidget->maxThicknessMm());

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    if (m_isEditMode) {
        if (!m_materialRepository.updateMaterial(m_editingNo, material, errorMessage)) {
            m_formWidget->setStatusMessage("Error: " + errorMessage);
            return;
        }

        exitEditMode();
        m_formWidget->setStatusMessage("Material updated successfully.");
        refreshMaterialsTable();
        m_formWidget->focusName();
        return;
    }

    if (!m_materialRepository.addMaterial(material, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setCreateMode();
    m_formWidget->setStatusMessage("Material created successfully.");
    refreshMaterialsTable();
    m_formWidget->focusName();
}
