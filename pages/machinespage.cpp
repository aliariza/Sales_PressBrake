#include "machinespage.h"
#include "machineformwidget.h"
#include "machinestablewidget.h"
#include "stylehelper.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

static QString normalizeMachineDecimal(const QString &text)
{
    QString value = text.trimmed();
    value.replace(',', '.');

    if (value.startsWith('.'))
        value.prepend('0');

    if (value.startsWith("-."))
        value.replace(0, 2, "-0.");

    return value;
}

MachinesPage::MachinesPage(QWidget *parent)
    : QWidget(parent),

    m_formWidget(nullptr),
    m_tableWidget(nullptr),
    m_isEditMode(false)
{
    QString errorMessage;
    if (!m_machineRepository.initialize(errorMessage)) {
        QMessageBox::critical(this, "Initialization Error", errorMessage);
    }

    setupUi();
    refreshMachinesTable();
}

void MachinesPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 12, 20, 20);
    layout->setSpacing(16);

    auto *topBarLayout = new QHBoxLayout;
    topBarLayout->addStretch();


    m_formWidget = new MachineFormWidget(this);
    m_tableWidget = new MachinesTableWidget(this);

    layout->addLayout(topBarLayout);
    layout->addWidget(m_formWidget);
    layout->addWidget(m_tableWidget, 1);

    connect(m_formWidget, &MachineFormWidget::submitRequested, this, &MachinesPage::createOrUpdateMachine);
    connect(m_formWidget, &MachineFormWidget::cancelEditRequested, this, &MachinesPage::cancelEdit);
    connect(m_tableWidget, &MachinesTableWidget::editRequested, this, &MachinesPage::handleEditRequested);
    connect(m_tableWidget, &MachinesTableWidget::deleteRequested, this, &MachinesPage::handleDeleteRequested);
}

void MachinesPage::enterEditMode(const MachineRecord &machine)
{
    m_isEditMode = true;
    m_editingNo = machine.no;
    m_formWidget->setEditMode(machine);
    m_formWidget->setStatusMessage("Edit mode.");
    m_formWidget->focusModel();
}

void MachinesPage::exitEditMode()
{
    m_isEditMode = false;
    m_editingNo.clear();
    m_formWidget->setCreateMode();
}

void MachinesPage::cancelEdit()
{
    exitEditMode();
    m_formWidget->clearStatusMessage();
    m_formWidget->focusModel();
}

void MachinesPage::refreshMachinesTable()
{
    QString errorMessage;
    const QList<MachineRecord> machines = m_machineRepository.getAllMachines(errorMessage);

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_tableWidget->setMachines(machines);
}

void MachinesPage::handleEditRequested(const MachineRecord &machine)
{
    m_formWidget->clearStatusMessage();
    enterEditMode(machine);
}

void MachinesPage::handleDeleteRequested(const MachineRecord &machine)
{
    const QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Delete Machine",
                              "Delete machine '" + machine.model + "'?",
                              QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QString errorMessage;
    if (!m_machineRepository.deleteMachine(machine.no, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setStatusMessage("Machine deleted successfully.");

    if (m_isEditMode && m_editingNo == machine.no)
        exitEditMode();

    refreshMachinesTable();
}

void MachinesPage::createOrUpdateMachine()
{
    QString errorMessage;

    if (m_formWidget->model().isEmpty()) {
        m_formWidget->setStatusMessage("Error: Machine model cannot be empty.");
        return;
    }

    if (!m_formWidget->hasAcceptableNumericInputs()) {
        m_formWidget->setStatusMessage("Error: One or more numeric fields are invalid. Use dot as decimal separator.");
        return;
    }

    MachineRecord machine;
    machine.no = m_isEditMode ? m_editingNo : m_machineRepository.getNextMachineNo(errorMessage);
    machine.model = m_formWidget->model();
    machine.maxTonnageTonf = normalizeMachineDecimal(m_formWidget->maxTonnageTonf());
    machine.workingLengthMm = normalizeMachineDecimal(m_formWidget->workingLengthMm());
    machine.maxThicknessMm = normalizeMachineDecimal(m_formWidget->maxThicknessMm());
    machine.basePriceUSD = normalizeMachineDecimal(m_formWidget->basePriceUSD());

    if (!errorMessage.isEmpty()) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    if (m_isEditMode) {
        if (!m_machineRepository.updateMachine(m_editingNo, machine, errorMessage)) {
            m_formWidget->setStatusMessage("Error: " + errorMessage);
            return;
        }

        exitEditMode();
        m_formWidget->setStatusMessage("Machine updated successfully.");
        refreshMachinesTable();
        m_formWidget->focusModel();
        return;
    }

    if (!m_machineRepository.addMachine(machine, errorMessage)) {
        m_formWidget->setStatusMessage("Error: " + errorMessage);
        return;
    }

    m_formWidget->setCreateMode();
    m_formWidget->setStatusMessage("Machine created successfully.");
    refreshMachinesTable();
    m_formWidget->focusModel();
}
