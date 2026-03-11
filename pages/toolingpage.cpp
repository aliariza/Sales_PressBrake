#include "toolingpage.h"
#include "toolingformwidget.h"
#include "toolingstablewidget.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

static QString normalizeToolingDecimal(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

ToolingPage::ToolingPage(QWidget *parent)
    : QWidget(parent),
    m_formWidget(nullptr),
    m_tableWidget(nullptr),
    m_isEditMode(false)
{
    QString errorMessage;
    if (!m_toolingRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshToolingTable();
}

void ToolingPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    auto *topBarLayout = new QHBoxLayout;
    topBarLayout->addStretch();


    m_formWidget = new ToolingFormWidget(this);
    m_tableWidget = new ToolingsTableWidget(this);

    layout->addLayout(topBarLayout);
    layout->addWidget(m_formWidget);
    layout->addWidget(m_tableWidget, 1);

    connect(m_formWidget, &ToolingFormWidget::submitRequested, this, &ToolingPage::createOrUpdateTooling);
    connect(m_formWidget, &ToolingFormWidget::cancelEditRequested, this, &ToolingPage::cancelEdit);
    connect(m_tableWidget, &ToolingsTableWidget::editRequested, this, &ToolingPage::handleEditRequested);
    connect(m_tableWidget, &ToolingsTableWidget::deleteRequested, this, &ToolingPage::handleDeleteRequested);
}

void ToolingPage::enterEditMode(const ToolingRecord &tooling)
{
    m_isEditMode = true;
    m_editingNo = tooling.no;
    m_formWidget->setEditMode(tooling);
    m_formWidget->setStatusMessage("Edit mode.");
    m_formWidget->focusName();
}

void ToolingPage::exitEditMode()
{
    m_isEditMode = false;
    m_editingNo.clear();
    m_formWidget->setCreateMode();
}

void ToolingPage::cancelEdit()
{
    exitEditMode();
    m_formWidget->clearStatusMessage();
    m_formWidget->focusName();
}

void ToolingPage::refreshToolingTable()
{
    QString errorMessage;
    const QList<ToolingRecord> toolingList = m_toolingRepository.getAllTooling(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_tableWidget->setToolingList(toolingList);
}

void ToolingPage::handleEditRequested(const ToolingRecord &tooling)
{
    m_formWidget->clearStatusMessage();
    enterEditMode(tooling);
}

void ToolingPage::handleDeleteRequested(const ToolingRecord &tooling)
{
    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Delete Tooling",
                              "Delete tooling '" + tooling.name + "'?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_toolingRepository.deleteTooling(tooling.no, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setStatusMessage("Tooling deleted successfully.");

    if (m_isEditMode && m_editingNo == tooling.no)
        exitEditMode();

    refreshToolingTable();
}

void ToolingPage::createOrUpdateTooling()
{
    QString errorMessage;

    if (m_formWidget->name().isEmpty()) {
        m_formWidget->setStatusMessage("Error: Tooling name cannot be empty.");
        return;
    }

    if (!m_formWidget->hasAcceptableNumericInputs()) {
        m_formWidget->setStatusMessage("Error: One or more numeric fields are invalid. Use dot as decimal separator.");
        return;
    }

    ToolingRecord tooling;
    tooling.no = m_isEditMode ? m_editingNo : m_toolingRepository.getNextToolingNo(errorMessage);
    tooling.name = m_formWidget->name();
    tooling.vDieMm = normalizeToolingDecimal(m_formWidget->vDieMm());
    tooling.punchRadiusMm = normalizeToolingDecimal(m_formWidget->punchRadiusMm());
    tooling.dieRadiusMm = normalizeToolingDecimal(m_formWidget->dieRadiusMm());

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    if (m_isEditMode) {
        if (!m_toolingRepository.updateTooling(m_editingNo, tooling, errorMessage)) {
            m_formWidget->setStatusMessage("Error: " + errorMessage);
            return;
        }

        exitEditMode();
        m_formWidget->setStatusMessage("Tooling updated successfully.");
        refreshToolingTable();
        m_formWidget->focusName();
        return;
    }

    if (!m_toolingRepository.addTooling(tooling, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setCreateMode();
    m_formWidget->setStatusMessage("Tooling created successfully.");
    refreshToolingTable();
    m_formWidget->focusName();
}
