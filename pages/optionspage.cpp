#include "optionspage.h"
#include "optionformwidget.h"
#include "optionstablewidget.h"

#include <QVBoxLayout>
#include <QMessageBox>

static QString normalizeOptionDecimal(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

OptionsPage::OptionsPage(QWidget *parent)
    : QWidget(parent),
    m_formWidget(nullptr),
    m_tableWidget(nullptr),
    m_isEditMode(false)
{
    QString errorMessage;
    if (!m_optionRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshOptionsTable();
}

void OptionsPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    m_formWidget = new OptionFormWidget(this);
    m_tableWidget = new OptionsTableWidget(this);

    layout->addWidget(m_formWidget);
    layout->addWidget(m_tableWidget, 1);

    connect(m_formWidget, &OptionFormWidget::submitRequested, this, &OptionsPage::createOrUpdateOption);
    connect(m_formWidget, &OptionFormWidget::cancelEditRequested, this, &OptionsPage::cancelEdit);
    connect(m_tableWidget, &OptionsTableWidget::editRequested, this, &OptionsPage::handleEditRequested);
    connect(m_tableWidget, &OptionsTableWidget::deleteRequested, this, &OptionsPage::handleDeleteRequested);
}

void OptionsPage::enterEditMode(const OptionRecord &option)
{
    m_isEditMode = true;
    m_editingNo = option.no;
    m_formWidget->setEditMode(option);
    m_formWidget->setStatusMessage("Edit mode.");
    m_formWidget->focusCode();
}

void OptionsPage::exitEditMode()
{
    m_isEditMode = false;
    m_editingNo.clear();
    m_formWidget->setCreateMode();
}

void OptionsPage::cancelEdit()
{
    exitEditMode();
    m_formWidget->clearStatusMessage();
    m_formWidget->focusCode();
}

void OptionsPage::refreshOptionsTable()
{
    QString errorMessage;
    const QList<OptionRecord> options = m_optionRepository.getAllOptions(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_tableWidget->setOptions(options);
}

void OptionsPage::handleEditRequested(const OptionRecord &option)
{
    m_formWidget->clearStatusMessage();
    enterEditMode(option);
}

void OptionsPage::handleDeleteRequested(const OptionRecord &option)
{
    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Delete Option",
                              "Delete option '" + option.code + "'?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_optionRepository.deleteOption(option.no, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setStatusMessage("Option deleted successfully.");

    if (m_isEditMode && m_editingNo == option.no)
        exitEditMode();

    refreshOptionsTable();
}

void OptionsPage::createOrUpdateOption()
{
    QString errorMessage;

    if (m_formWidget->code().isEmpty()) {
        m_formWidget->setStatusMessage("Error: Option code cannot be empty.");
        return;
    }

    if (m_formWidget->name().isEmpty()) {
        m_formWidget->setStatusMessage("Error: Option name cannot be empty.");
        return;
    }

    if (!m_formWidget->hasAcceptableNumericInputs()) {
        m_formWidget->setStatusMessage("Error: Price USD is invalid. Use dot as decimal separator.");
        return;
    }

    OptionRecord option;
    option.no = m_isEditMode ? m_editingNo : m_optionRepository.getNextOptionNo(errorMessage);
    option.code = m_formWidget->code();
    option.name = m_formWidget->name();
    option.priceUsd = normalizeOptionDecimal(m_formWidget->priceUsd());

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    if (m_isEditMode) {
        if (!m_optionRepository.updateOption(m_editingNo, option, errorMessage)) {
            m_formWidget->setStatusMessage("Error: " + errorMessage);
            return;
        }

        exitEditMode();
        m_formWidget->setStatusMessage("Option updated successfully.");
        refreshOptionsTable();
        m_formWidget->focusCode();
        return;
    }

    if (!m_optionRepository.addOption(option, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setCreateMode();
    m_formWidget->setStatusMessage("Option created successfully.");
    refreshOptionsTable();
    m_formWidget->focusCode();
}
