#ifndef MACHINESPAGE_H
#define MACHINESPAGE_H

#include <QWidget>
#include "machinerepository.h"

class QPushButton;
class MachineFormWidget;
class MachinesTableWidget;

class MachinesPage : public QWidget
{
    Q_OBJECT

public:
    explicit MachinesPage(QWidget *parent = nullptr);

    void refreshMachinesTable();

signals:
    void backRequested();

private slots:
    void createOrUpdateMachine();
    void cancelEdit();
    void handleEditRequested(const MachineRecord &machine);
    void handleDeleteRequested(const MachineRecord &machine);

private:
    void setupUi();
    void enterEditMode(const MachineRecord &machine);
    void exitEditMode();


    MachineFormWidget *m_formWidget;
    MachinesTableWidget *m_tableWidget;

    bool m_isEditMode;
    QString m_editingNo;

    MachineRepository m_machineRepository;
};

#endif // MACHINESPAGE_H
